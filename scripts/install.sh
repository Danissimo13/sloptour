#!/usr/bin/env bash
# Build sloptour in Release and install it as a command (macOS and Linux).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PREFIX="${SLOPTOUR_PREFIX:-${HOME}/.local}"
BUILD="${ROOT}/build/install"
MARKER="# added by the sloptour installer"
WITH_TESTS=OFF
UPDATE_PATH=1
UNINSTALL=0
USE_SUDO=0

usage() {
    cat <<EOF
Usage: scripts/install.sh [options]

Builds sloptour (Release) and installs the binary as \`sloptour\`.

Options:
  --prefix DIR     install into DIR/bin (default: ~/.local, or \$SLOPTOUR_PREFIX)
  --system         install into /usr/local/bin (uses sudo when needed)
  --no-path        do not add the bin directory to your shell profile
  --with-tests     build and run the test suite before installing
  --uninstall      remove the installed binary and the PATH line
  -h, --help       show this help
EOF
}

say() { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33mwarning:\033[0m %s\n' "$*" >&2; }
die() { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

while [[ $# -gt 0 ]]; do
    case "$1" in
        --prefix) [[ $# -ge 2 ]] || die "--prefix needs a directory"; PREFIX="$2"; shift 2 ;;
        --system) PREFIX="/usr/local"; UPDATE_PATH=0; shift ;;
        --no-path) UPDATE_PATH=0; shift ;;
        --with-tests) WITH_TESTS=ON; shift ;;
        --uninstall) UNINSTALL=1; shift ;;
        -h|--help) usage; exit 0 ;;
        *) die "unknown option: $1 (see --help)" ;;
    esac
done

PREFIX="$(mkdir -p "${PREFIX}" 2>/dev/null && cd "${PREFIX}" && pwd || echo "${PREFIX}")"
BIN_DIR="${PREFIX}/bin"
if [[ -d "${BIN_DIR}" ]]; then [[ -w "${BIN_DIR}" ]] || USE_SUDO=1; else [[ -w "${PREFIX}" ]] || USE_SUDO=1; fi

as_owner() {
    if [[ "${USE_SUDO}" == 1 ]]; then
        sudo "$@"
    else
        "$@"
    fi
}

shell_profile() {
    case "$(basename "${SHELL:-}")" in
        zsh) echo "${ZDOTDIR:-${HOME}}/.zshrc" ;;
        bash) [[ "$(uname -s)" == Darwin ]] && echo "${HOME}/.bash_profile" || echo "${HOME}/.bashrc" ;;
        fish) echo "${XDG_CONFIG_HOME:-${HOME}/.config}/fish/config.fish" ;;
        *) echo "${HOME}/.profile" ;;
    esac
}

path_line() {
    if [[ "$(basename "${SHELL:-}")" == fish ]]; then
        echo "fish_add_path \"${BIN_DIR}\" ${MARKER}"
    else
        echo "export PATH=\"${BIN_DIR}:\$PATH\" ${MARKER}"
    fi
}

uninstall() {
    local binary="${BIN_DIR}/sloptour"
    if [[ -e "${binary}" ]]; then
        as_owner rm -f "${binary}"
        say "removed ${binary}"
    else
        warn "nothing installed at ${binary}"
    fi
    local profile
    profile="$(shell_profile)"
    if [[ -f "${profile}" ]] && grep -qF "${MARKER}" "${profile}"; then
        { grep -vF "${MARKER}" "${profile}" || true; } > "${profile}.sloptour.tmp"
        mv "${profile}.sloptour.tmp" "${profile}"
        say "removed the PATH line from ${profile}"
    fi
}

install_hint() {
    case "$(uname -s)" in
        Darwin) echo "brew install cmake ninja conan llvm" ;;
        Linux)
            if command -v apt-get >/dev/null; then
                echo "sudo apt-get install -y build-essential g++-14 cmake ninja-build git pipx && pipx install conan"
            elif command -v dnf >/dev/null; then
                echo "sudo dnf install -y gcc-c++ cmake ninja-build git pipx && pipx install conan"
            elif command -v pacman >/dev/null; then
                echo "sudo pacman -S --needed base-devel cmake ninja git python-pipx && pipx install conan"
            else
                echo "install a C++23 compiler (GCC 14+ or Clang 18+), cmake, ninja, git and conan (pipx install conan)"
            fi ;;
        *) echo "install a C++23 compiler, cmake, ninja, git and conan" ;;
    esac
}

check_tools() {
    local missing=()
    for tool in cmake ninja conan git; do
        command -v "${tool}" >/dev/null || missing+=("${tool}")
    done
    if ! command -v "${CXX:-c++}" >/dev/null && ! command -v g++ >/dev/null && ! command -v clang++ >/dev/null; then
        missing+=("a C++ compiler")
    fi
    if [[ ${#missing[@]} -gt 0 ]]; then
        die "missing: ${missing[*]}
       install them with:  $(install_hint)"
    fi
}

check_compiler() {
    local compiler="${CXX:-c++}"
    command -v "${compiler}" >/dev/null || compiler="$(command -v g++ || command -v clang++)"
    local probe="${BUILD}/probe"
    mkdir -p "${probe}"
    printf '#include <expected>\n#include <print>\nint main() { std::expected<int, int> value = 1; std::println("{}", *value); }\n' > "${probe}/probe.cpp"
    if ! "${compiler}" -std=c++23 "${probe}/probe.cpp" -o "${probe}/probe" >/dev/null 2>&1; then
        die "${compiler} cannot build C++23 (<expected>, <print>). Use GCC 14+, Clang 18+ or a recent Apple Clang,
       for example:  CXX=g++-14 scripts/install.sh"
    fi
}

ensure_conan_profile() {
    if ! conan profile path default >/dev/null 2>&1; then
        say "creating the default conan profile"
        conan profile detect --force >/dev/null
    fi
}

build() {
    say "installing dependencies with conan (first run builds them, this can take a few minutes)"
    SLOPTOUR_NO_USER_PRESETS=1 conan install "${ROOT}" --output-folder "${BUILD}" --build=missing \
        -s build_type=Release -s compiler.cppstd=23 -c tools.cmake.cmaketoolchain:generator=Ninja
    local toolchain
    toolchain="$(find "${BUILD}" -name conan_toolchain.cmake -path '*generators*' | head -n 1)"
    [[ -n "${toolchain}" ]] || die "conan did not generate a toolchain"

    say "configuring"
    cmake -S "${ROOT}" -B "${BUILD}/cmake" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="${toolchain}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SKIP_RPATH=ON \
        -DSLOPTOUR_BUILD_TESTS="${WITH_TESTS}" \
        -DSLOPTOUR_ENABLE_CLANG_TIDY=OFF \
        -DSLOPTOUR_WARNINGS_AS_ERRORS=OFF >/dev/null

    say "building"
    cmake --build "${BUILD}/cmake" --parallel

    if [[ "${WITH_TESTS}" == ON ]]; then
        say "running tests"
        ctest --test-dir "${BUILD}/cmake" --output-on-failure
    fi
}

install_binary() {
    say "installing into ${BIN_DIR}"
    as_owner cmake --install "${BUILD}/cmake" --prefix "${PREFIX}" --strip >/dev/null
    "${BIN_DIR}/sloptour" --help >/dev/null || die "the installed binary does not start"
}

update_path() {
    case ":${PATH}:" in
        *":${BIN_DIR}:"*) return ;;
    esac
    if [[ "${UPDATE_PATH}" == 0 ]]; then
        warn "${BIN_DIR} is not on your PATH"
        return
    fi
    local profile
    profile="$(shell_profile)"
    mkdir -p "$(dirname "${profile}")"
    if ! grep -qF "${MARKER}" "${profile}" 2>/dev/null; then
        path_line >> "${profile}"
        say "added ${BIN_DIR} to PATH in ${profile} (open a new terminal or: source ${profile})"
    fi
}

if [[ "${UNINSTALL}" == 1 ]]; then
    uninstall
    exit 0
fi

check_tools
check_compiler
ensure_conan_profile
build
install_binary
update_path

say "done: $("${BIN_DIR}/sloptour" agents 2>/dev/null | grep -c '✓') agent(s) ready - run \`sloptour\` inside any repository"
