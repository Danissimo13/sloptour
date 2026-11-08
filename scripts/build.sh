#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE="${1:-Debug}"
PRESET="conan-$(echo "${BUILD_TYPE}" | tr '[:upper:]' '[:lower:]')"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT}"
conan install . --build=missing -s build_type="${BUILD_TYPE}" -s compiler.cppstd=23
cmake --preset "${PRESET}"
cmake --build --preset "${PRESET}"
ctest --preset "${PRESET}" --output-on-failure
