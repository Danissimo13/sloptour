# sloptour — Agent Guide

C++23 CLI/TUI, FTXUI, reproc, CLI11, nlohmann_json, Conan 2, CMake + Ninja, clang-tidy, GoogleTest. An external coding agent (Claude Code, Codex, Gemini, …) explores a repository and writes an interactive tour of it; sloptour drives the agent, stores the tour and plays it in the terminal.

## Rules

1. **No comments** in `src/` and `tests/`.
2. **Clean code.** Small classes and functions, intention-revealing names, SOLID, KISS, YAGNI, composition over inheritance, injected dependencies.
3. **DDD.** Types are named after domain concepts (`Tour`, `Chapter`, `Step`, `CodeLocation`, `TourProgress`). Entities guard their invariants, value objects are immutable. The `Tour` aggregate changes only through its root, by an Application service (`TourSession`).
4. **Zero warnings, zero clang-tidy findings.** Never `NOLINT`; if a check is wrong for an area, change a scoped `.clang-tidy` (see `src/Infrastructure/Process/.clang-tidy`).
5. **No raw pointers**, `new` or `delete`. Exceptions: reproc and C APIs in `src/Infrastructure/Process/*.cpp` and `src/Infrastructure/Platform/*.cpp`, member addresses given to FTXUI components (`Menu`, `Toggle`, `Checkbox`, `Input`), `main(argc, argv)`.
6. **No magic literals.** Named `constexpr` for structural constants, `assets/config/defaults.json` for tunables, `assets/prompts/*.md` for agent prompts, `assets/syntax/languages.json` for highlighting.
7. **Reuse, never copy-paste.**
8. **Responsive UI.** Agent work never runs on the UI thread. Background threads talk to the screen only through `GuardedPoster`; UI state is touched only on the UI thread.
9. **Agents are data.** A new agent is a `agents.<name>` entry in config, never code. A new agent output format is one `IAgentOutput` implementation registered in `AgentOutputs.cpp`.
10. **Polymorphism, never kind dispatch.** Each kind is a class behind an interface or a `std::variant` alternative with `std::visit`, chosen in one place (registry, visitor). Guard clauses and algorithm conditions stay plain `if`s.
11. **Errors are values.** No `throw`, `try`, `catch` in `src/`, except catching a throwing third-party call at its single call site (`CLI::App::parse`). May fail: `std::expected<T, core::Error<TContext>>` with an `XResult<T>` alias. Failing construction: static `create`. Never `.value()` in `src/`, never ignore a result. Broken invariant: `core::panic`. Tests use `.value()`.

## Naming (enforced by clang-tidy)

| Kind | Style |
|---|---|
| Classes, structs, enums, enum values, aliases, template params | `UpperCamelCase` |
| Functions, locals, parameters, public fields | `lower_snake_case` |
| Private and protected fields | `m_lower_snake_case` |
| Constants | `UPPER_CASE` |
| Namespaces | `lower_case` |
| Files | class name, `.hpp` / `.cpp` |

## Style

- One class per file, `#pragma once`, absolute includes from `src/`, `clang-format` (4 spaces, 120 columns).
- `[[nodiscard]]`, `const`, `noexcept`, `enum class`, designated initializers, `std::optional` for "maybe", `.at()` instead of `[]`.
- Polymorphic interfaces delete copy and move. More than three constructor dependencies: nested `Dependencies` struct.
- Pass `const std::stop_token&`; long-running work must honour it.

## Layers

Each layer is a static library. The `layer_dependencies` test enforces the table; fix a violation by moving code, never by adding a dependency.

| Layer | May depend on | Holds |
|---|---|---|
| `Core` | std | Errors, panic, text helpers |
| `Domain` | Core | Tour aggregate, outline, progress |
| `Application` | Domain | Use cases (`GenerateTour`, `AskAboutStep`, `DeepenStep`, `TourLibrary`, `TourSession`, `Workspace`) and ports |
| `Infrastructure` | Application, nlohmann_json, reproc (private), embedded assets (private) | Agents, JSON documents, storage, git, prompts, config, editor |
| `Terminal` | Application, FTXUI | Screens: home, wizard, generation, player |
| `App` | all | CLI and composition root only |

- Generation: one agent run drafts the outline, chapters are written in parallel (`generation.parallel_chapters`), then `ComposeQuiz` writes the final quiz. Unparseable replies get `generation.repair_attempts` repair rounds. Depth and detail presets (`generation.depths` / `details`) drive counts and prompt guidance; the chosen agent, model, depth and detail are stored in `TourMetadata`.
- Agents report progress through `AgentListener` (activity lines + token deltas). `UsageMeter` turns per-message samples into deltas; an authoritative sample (claude's `result`) replaces the running estimate. Agents without usage get a `~` estimate.
- UI text lives only in `assets/i18n/<lang>.json` (same keys in every file, checked by a test); views take a `Localizer` and never hold literals. Plurals use `plural.<noun>.one/few/many`. Prompts get `{{language}}`.
- Tours are stored through `StorageRouter` (repository / global / memory), switched live by `UserPreferences`.
- Theme colors are terminal palette names by default so the UI follows the terminal theme; never hard-code RGB in views.
- Prompts are templates with `{{variables}}`; a project can override any of them in `.sloptour/prompts/` (`sloptour eject-prompts`).
- Tours live in `<repo>/.sloptour/tours/<id>.json`, progress and Q&A in `<repo>/.sloptour/progress/<id>.json`.
- Config layers: embedded `assets/config/defaults.json` ← `~/.config/sloptour/config.json` ← `<repo>/.sloptour/config.json` (JSON merge patch).

## Tests

`tests/<Layer>/`, fakes in `tests/Fakes/`. Test rules, contracts, edge cases and fixed regressions; one test per rule, no tests for getters or plumbing. Same code rules as `src/`.

## Build

```bash
./scripts/build.sh            # conan + configure + build + tests (Debug)
./scripts/build.sh Release
./build/Debug/src/App/sloptour
```

`scripts/install.sh` (macOS/Linux) and `scripts/install.ps1` / `install.cmd` (Windows) build a self-contained Release binary in `build/install` (no clang-tidy, warnings not fatal) and install it on PATH. OS differences live only in `src/Infrastructure/Platform/Platform.cpp`; keep every other file portable (MSVC, GCC, Clang).

Sources are globbed. Assets are embedded into the binary at build time (`cmake/EmbedAssets.cmake`). To try the UI without spending agent tokens, add a scripted agent to `<repo>/.sloptour/config.json` whose command prints canned JSON.

## Commits

Conventional Commits: `type(scope): Message`, imperative, capitalized, no trailing period. Linear history: rebase, land with `git merge --ff-only`, no merge commits.

## Done means

Zero warnings and clang-tidy findings, tests pass, new rules covered by tests, UI changes checked in a real terminal (tmux capture works).
