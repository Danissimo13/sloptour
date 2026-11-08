# sloptour

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus)
![Platforms](https://img.shields.io/badge/platforms-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE)

**Interactive code tours of any repository, right in your terminal.** An AI coding agent explores the code and writes a step-by-step tour; sloptour plays it back.

| Tour | When | What you get |
|---|---|---|
| `onboarding` | You joined a big project | Architecture, key flows through the code, build/run/test, conventions, where to start |
| `functional` | You need to understand one feature | That feature end to end: entry points, every layer in execution order, data, edge cases, tests |
| `recap` | An agent worked for a while | What changed in the commits and working tree, how it fits in, decisions made, what to double-check |

## Features

- **Steps grounded in code**: an explanation, highlighted code with line numbers, commands to try and a self-check question.
- **Chat on every step**: ask a question and watch what the agent reads while it answers. Conversations are saved with the step.
- **Go deeper**: the agent writes extra sub-steps and inserts them after the current one.
- **Final quiz** with explanations and a score.
- **Bring your agent**: Claude Code, Codex, Gemini, Cursor, or any command that reads a prompt on stdin.
- **Tunable output**: choose the model, depth (how much is covered) and detail (how much each step says).
- **Live token usage**: exact with cost for Claude, a `~` estimate for other agents.
- **Fits your setup**: follows your terminal theme, speaks English and Russian, saves progress so you can resume any time.

## Install

You need a C++23 toolchain, CMake, Ninja and Conan, plus at least one agent CLI on `PATH`: [Claude Code](https://claude.com/claude-code) (`claude`, the default), `codex`, `gemini` or `cursor-agent`.

| OS | Toolchain |
|---|---|
| macOS | Xcode command line tools, `brew install cmake ninja conan` |
| Linux | GCC 14+ or Clang 18+, cmake, ninja, git, `pipx install conan` |
| Windows | Visual Studio 2022 C++ tools, `winget install Kitware.CMake Ninja-build.Ninja Git.Git`, `pip install conan` |

```bash
git clone https://github.com/Danissimo13/sloptour && cd sloptour
./scripts/install.sh     # macOS / Linux, installs to ~/.local/bin (see --help)
scripts\install.cmd      # Windows, installs to %LOCALAPPDATA%\Programs\sloptour\bin
```

The first build compiles dependencies and takes a few minutes. Run `sloptour agents` to see which agents are available.

## Usage

```bash
cd your/repo
sloptour                                               # home screen: your tours + start a new one
sloptour new onboarding -f "billing service"
sloptour new functional "adding a todo and listing it"
sloptour new recap --last 15                           # last 15 commits + uncommitted changes
sloptour new recap --since v1.4.0 --no-working-tree
sloptour new onboarding --model sonnet --depth high --detail light
sloptour new recap --last 5 --plain                    # no TUI, for scripts and CI
sloptour play                                          # continue the latest tour
sloptour list
```

| Key | Action | Key | Action |
|---|---|---|---|
| `→` `←` | next / previous step | `a` | ask in the chat |
| `]` `[` | next / previous chapter | `d` | go deeper |
| `↑` `↓` | scroll | `o` | open in `$EDITOR` |
| `tab` | next code location | `Q` | final quiz |
| `enter` | reveal the answer | `?` | all keys |

> [!NOTE]
> The default `claude` and `codex` configurations restrict the agent to read-only access. The `gemini` and `cursor` entries pass no sandbox flags, so they run with whatever permissions your CLI allows by default.

## Storage

Pick where tours live on the home screen or with `--storage`:

| Mode | Location |
|---|---|
| `repository` | `<repo>/.sloptour/`, can be committed and shared with your team |
| `global` | Per-repository folder in your user data directory, keeps the repository clean |
| `memory` | Nothing is written; tours last until you quit |

## Configuration

Settings merge in order: [built-in defaults](assets/config/defaults.json) ← `~/.config/sloptour/config.json` ← `<repo>/.sloptour/config.json`.

An agent is just config. Add any command that reads the prompt on stdin and prints its answer on stdout:

```json
{
  "agent": "my-agent",
  "agents": {
    "my-agent": {
      "command": ["my-agent", "{model_arguments}", "--read-only"],
      "models": ["fast", "smart"],
      "model_arguments": ["--model", "{model}"],
      "output": "text",
      "timeout_seconds": 900
    }
  }
}
```

To customize the prompts, run `sloptour eject-prompts` and edit the files in `.sloptour/prompts/`.

## Development

```bash
./scripts/build.sh            # Debug build with clang-tidy and tests
./scripts/build.sh Release
```

Architecture and code rules are in [AGENTS.md](AGENTS.md).

## License

[MIT](LICENSE)
