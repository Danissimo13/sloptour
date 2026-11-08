#include "Infrastructure/Git/GitRepositoryInspector.hpp"
#include "Infrastructure/Storage/RepositorySourceFiles.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace sloptour::tests {

namespace {

constexpr std::chrono::milliseconds POLL{10};
constexpr std::chrono::seconds GENEROUS{20};
constexpr infrastructure::GitSettings GIT{.recent_commits = 10, .max_digest_characters = 4000};

class GitInspectorTests : public ::testing::Test {
protected:
    void SetUp() override {
        m_root = std::filesystem::temp_directory_path() /
                 (std::string("sloptour-git-") + ::testing::UnitTest::GetInstance()->current_test_info()->name());
        std::filesystem::remove_all(m_root);
        std::filesystem::create_directories(m_root);
        git("git init -q && git config user.email t@t && git config user.name t");
        commit("a.txt", "first");
        commit("b.txt", "second");
    }

    void TearDown() override {
        std::filesystem::remove_all(m_root);
    }

    void git(const std::string& script) const {
        const auto outcome = m_runner.run(
            {.arguments = {"/bin/sh", "-c", script}, .input = {}, .working_directory = m_root, .timeout = GENEROUS},
            {.output = [](std::string_view) {}, .errors = [](std::string_view) {}}, std::stop_token{});
        ASSERT_TRUE(outcome.has_value() && outcome->status == 0) << script;
    }

    void commit(const std::string& file, const std::string& message) const {
        ASSERT_TRUE(infrastructure::write_text(m_root / file, message).has_value());
        git("git add -A && git commit -qm " + message);
    }

    [[nodiscard]] infrastructure::GitRepositoryInspector inspector() const {
        return {m_root, std::make_shared<const infrastructure::ProcessRunner>(POLL), GIT};
    }

    [[nodiscard]] const std::filesystem::path& root() const {
        return m_root;
    }

private:
    std::filesystem::path m_root;
    infrastructure::ProcessRunner m_runner{POLL};
};

}

TEST_F(GitInspectorTests, ListsRecentCommitsNewestFirst) {
    const auto commits = inspector().recent_commits(GIT.recent_commits).value();
    ASSERT_EQ(commits.size(), 2U);
    EXPECT_EQ(commits.front().subject, "second");
}

TEST_F(GitInspectorTests, DigestCoversCommitsAfterTheBaseAndTheWorkingTree) {
    ASSERT_TRUE(infrastructure::write_text(root() / "c.txt", "draft").has_value());
    const auto digest = inspector().changes({.base_revision = "HEAD~1", .include_working_tree = true}).value();
    EXPECT_TRUE(digest.commits.contains("second"));
    EXPECT_FALSE(digest.commits.contains("first"));
    EXPECT_TRUE(digest.working_tree.contains("c.txt"));
    EXPECT_TRUE(digest.range.contains("uncommitted"));
}

TEST_F(GitInspectorTests, RejectsUnknownRevisions) {
    EXPECT_FALSE(inspector().changes({.base_revision = "nope", .include_working_tree = false}).has_value());
}

TEST_F(GitInspectorTests, SourceFilesStayInsideTheRepository) {
    const infrastructure::RepositorySourceFiles sources(root());
    EXPECT_EQ(sources.read_lines("a.txt").value(), (std::vector<std::string>{"first"}));
    EXPECT_FALSE(sources.read_lines("../../etc/hosts").has_value());
}

}
