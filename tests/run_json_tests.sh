#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
BIN_DIR="${BUILD_DIR}/bin"
GAME_EXE="${BIN_DIR}/RichGame"

echo "Building project..." >&2
mkdir -p "${BUILD_DIR}"
(cd "${BUILD_DIR}" && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --config Release)

python3 "${SCRIPT_DIR}/runner.py" "${GAME_EXE}" "${SCRIPT_DIR}/json_cases"