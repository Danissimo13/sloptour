#include "App/CommandLine.hpp"

#include "Infrastructure/Json/TourKindNames.hpp"

#include <CLI/CLI.hpp>

#include <map>
#include <string>

namespace sloptour::app {

namespace {

constexpr std::string_view DESCRIPTION =
    "sloptour - an AI agent explores your repository and builds an interactive, step-by-step tour of it.\n"
    "Run without arguments to open the interactive home screen.";

struct Options {
    std::string repository = ".";
    std::string language;
    std::string storage;
    std::string kind = "onboarding";
    std::string agent;
    std::string model;
    std::string depth;
    std::string detail;
    std::string since;
    std::size_t last = 0;
    bool no_working_tree = false;
    std::string focus;
    std::string functionality;
    std::string name;
    bool plain = false;
    std::string tour;
};

[[nodiscard]] domain::TourKind kind_of(const std::string& kind) {
    return infrastructure::kind_named(kind).value_or(domain::TourKind::Onboarding);
}

[[nodiscard]] std::optional<std::string> present(const std::string& value) {
    return value.empty() ? std::nullopt : std::optional(value);
}

void describe_create(CLI::App& create, Options& options) {
    create
        .add_option("kind", options.kind,
                    "onboarding (learn a codebase), recap (catch up on recent changes) or functional (follow one "
                    "feature through the code)")
        ->check(CLI::IsMember({"onboarding", "recap", "functional"}));
    create.add_option("functionality", options.functionality,
                      "functional: the feature to follow, e.g. \"checkout and payment\"");
    create.add_option("-a,--agent", options.agent, "agent to use, see `sloptour agents`");
    create.add_option("-m,--model", options.model, "model for the agent, e.g. opus or sonnet for claude");
    create.add_option("-d,--depth", options.depth, "how much is covered: light, medium or high");
    create.add_option("--detail", options.detail, "how wordy each step is: light, medium or high");
    create.add_option("-s,--since", options.since, "recap: explain everything after this revision");
    create.add_option("-l,--last", options.last, "recap: explain the last N commits");
    create.add_flag("--no-working-tree", options.no_working_tree, "recap: ignore uncommitted changes");
    create.add_option("-f,--focus", options.focus, "what the tour should focus on");
    create.add_option("-n,--name", options.name, "tour name, generated when omitted");
    create.add_flag("-p,--plain", options.plain, "print progress as plain text instead of the interactive screen");
}

[[nodiscard]] CreateCommand create_command(const Options& options) {
    return {.kind = kind_of(options.kind),
            .agent = present(options.agent),
            .model = options.model,
            .depth = options.depth,
            .detail = options.detail,
            .since = present(options.since),
            .last = options.last == 0 ? std::nullopt : std::optional(options.last),
            .include_working_tree = !options.no_working_tree,
            .focus = options.focus,
            .functionality = options.functionality,
            .name = present(options.name),
            .plain = options.plain};
}

[[nodiscard]] Command chosen(const CLI::App& app, const Options& options) {
    if (app.got_subcommand("new")) {
        return create_command(options);
    }
    if (app.got_subcommand("play")) {
        return PlayCommand{.tour = present(options.tour)};
    }
    if (app.got_subcommand("list")) {
        return ListCommand{};
    }
    if (app.got_subcommand("agents")) {
        return AgentsCommand{};
    }
    if (app.got_subcommand("eject-prompts")) {
        return EjectPromptsCommand{};
    }
    return BrowseCommand{};
}

}

ParsedCommandLine parse_command_line(int argc, const char* const* argv) {
    CLI::App app{std::string(DESCRIPTION), "sloptour"};
    Options options;
    app.require_subcommand(0, 1);
    app.add_option("-C,--repo", options.repository, "repository to tour (default: the current directory)");
    app.add_option("--lang", options.language,
                   "interface and agent language: en or ru (remembered from the home screen)");
    app.add_option("--storage", options.storage, "where tours live: repository, global or memory")
        ->check(CLI::IsMember({"repository", "global", "memory"}));
    describe_create(*app.add_subcommand("new", "generate a new tour with an agent"), options);
    app.add_subcommand("play", "continue a tour (the latest one by default)")
        ->add_option("tour", options.tour, "tour id, see `sloptour list`");
    app.add_subcommand("list", "list the tours of this repository");
    app.add_subcommand("agents", "list the configured agents and whether they are installed");
    app.add_subcommand("eject-prompts", "copy the built-in prompts to .sloptour/prompts to customise them");
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& error) {
        return ExitRequested{.code = app.exit(error)};
    }
    return Invocation{.repository = options.repository,
                      .language = present(options.language),
                      .storage = present(options.storage),
                      .command = chosen(app, options)};
}

}
