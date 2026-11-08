#include "Infrastructure/Git/GitRepositoryInspector.hpp"

#include "Core/Text/Text.hpp"

#include <chrono>
#include <format>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view GIT = "git";
constexpr std::chrono::seconds GIT_TIMEOUT{30};
constexpr char FIELD_SEPARATOR = '\x1f';
constexpr std::string_view COMMIT_FORMAT = "--format=%h%x1f%s%x1f%an%x1f%ar";
constexpr std::string_view LOG_FORMAT = "--format=%h %s (%an, %ar)";
constexpr std::string_view TRUNCATED = "\n... (truncated)";
constexpr std::string_view UNCOMMITTED = "uncommitted changes";
constexpr std::size_t HASH_FIELD = 0;
constexpr std::size_t SUBJECT_FIELD = 1;
constexpr std::size_t AUTHOR_FIELD = 2;
constexpr std::size_t DATE_FIELD = 3;
constexpr std::size_t COMMIT_FIELDS = 4;
constexpr std::size_t SHORT_HASH_LENGTH = 12;

[[nodiscard]] std::vector<std::string> fields_of(std::string_view line) {
    std::vector<std::string> fields;
    std::size_t start = 0;
    for (auto end = line.find(FIELD_SEPARATOR); end != std::string_view::npos;
         end = line.find(FIELD_SEPARATOR, start)) {
        fields.emplace_back(line.substr(start, end - start));
        start = end + 1;
    }
    fields.emplace_back(line.substr(start));
    return fields;
}

[[nodiscard]] std::string range_of(const application::ChangeScope& scope, bool dirty) {
    std::string range = scope.base_revision ? *scope.base_revision + "..HEAD" : std::string();
    if (scope.include_working_tree && dirty) {
        range += range.empty() ? std::string(UNCOMMITTED) : " + " + std::string(UNCOMMITTED);
    }
    return range;
}

}

application::InspectionResult<application::RepositoryOverview> GitRepositoryInspector::overview() const {
    application::RepositoryOverview overview{.name = m_root.filename().string(),
                                             .root = m_root.string(),
                                             .versioned = is_versioned(),
                                             .branch = {},
                                             .head = {}};
    if (overview.versioned) {
        overview.branch = std::string(core::trim(git({"rev-parse", "--abbrev-ref", "HEAD"}).value_or("")));
        overview.head = std::string(core::trim(git({"log", "-1", std::string(LOG_FORMAT)}).value_or("")));
    }
    return overview;
}

application::InspectionResult<std::vector<application::CommitSummary>>
GitRepositoryInspector::recent_commits(std::size_t limit) const {
    if (!is_versioned()) {
        return std::vector<application::CommitSummary>{};
    }
    auto log = git({"log", "-n", std::to_string(limit), std::string(COMMIT_FORMAT)});
    if (!log) {
        return std::unexpected(log.error());
    }
    std::vector<application::CommitSummary> commits;
    for (const auto line : core::split_lines(*log)) {
        const auto fields = fields_of(line);
        if (fields.size() == COMMIT_FIELDS) {
            commits.push_back({.hash = fields.at(HASH_FIELD),
                               .subject = fields.at(SUBJECT_FIELD),
                               .author = fields.at(AUTHOR_FIELD),
                               .date = fields.at(DATE_FIELD)});
        }
    }
    return commits;
}

application::InspectionResult<application::ChangeDigest>
GitRepositoryInspector::changes(const application::ChangeScope& scope) const {
    if (!is_versioned()) {
        return std::unexpected(application::InspectionError("a recap tour needs a git repository"));
    }
    application::ChangeDigest digest;
    if (scope.base_revision) {
        auto base = verified(*scope.base_revision);
        if (!base) {
            return std::unexpected(base.error());
        }
        digest.commits = this->digest({"log", std::string(LOG_FORMAT), *base + "..HEAD"});
        digest.diff_stat = scope.include_working_tree ? this->digest({"diff", "--stat", *base})
                                                      : this->digest({"diff", "--stat", *base, "HEAD"});
    }
    if (scope.include_working_tree) {
        digest.working_tree = this->digest({"status", "--short", "--untracked-files=all"});
    }
    digest.range = range_of(scope, !digest.working_tree.empty());
    if (digest.range.empty()) {
        return std::unexpected(application::InspectionError("nothing to recap: no base revision and a clean tree"));
    }
    return digest;
}

application::InspectionResult<std::string> GitRepositoryInspector::git(std::vector<std::string> arguments) const {
    arguments.insert(arguments.begin(), std::string(GIT));
    auto outcome = m_runner->run(
        {.arguments = std::move(arguments), .input = {}, .working_directory = m_root, .timeout = GIT_TIMEOUT},
        {.output = [](std::string_view /*line*/) {}, .errors = [](std::string_view /*line*/) {}}, std::stop_token{});
    if (!outcome) {
        return std::unexpected(application::InspectionError(outcome.error()));
    }
    if (outcome->status != 0) {
        return std::unexpected(application::InspectionError(std::string(core::trim(outcome->errors))));
    }
    return std::move(outcome->output);
}

bool GitRepositoryInspector::is_versioned() const {
    const auto inside = git({"rev-parse", "--is-inside-work-tree"});
    return inside && core::trim(*inside) == "true";
}

std::string GitRepositoryInspector::digest(std::vector<std::string> arguments) const {
    auto text = std::string(core::trim(git(std::move(arguments)).value_or("")));
    if (text.size() > m_settings.max_digest_characters) {
        text.resize(m_settings.max_digest_characters);
        text += TRUNCATED;
    }
    return text;
}

application::InspectionResult<std::string> GitRepositoryInspector::verified(const std::string& revision) const {
    auto resolved = git({"rev-parse", "--verify", "--quiet", revision + "^{commit}"});
    if (!resolved) {
        return std::unexpected(application::InspectionError("unknown revision '" + revision + "'"));
    }
    return std::string(core::trim(*resolved)).substr(0, SHORT_HASH_LENGTH);
}

}
