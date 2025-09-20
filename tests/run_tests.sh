#!/usr/bin/env bash
set -euo pipefail

# Determine repo root (script dir is tests)
SCRIPT_DIR="$(cd "$(dirname \"${BASH_SOURCE[0]}\")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
BIN_DIR="${BUILD_DIR}/bin"
GAME_EXE="${BIN_DIR}/RichGame"
RESULT_FILE="${SCRIPT_DIR}/checkresult.txt"
CASES_DIR="${SCRIPT_DIR}/cases"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Ensure executable exists
if [[ ! -x "${GAME_EXE}" ]]; then
  echo "Game executable not found: ${GAME_EXE}" >&2
  echo "Hint: Configure and build first:" >&2
  echo "  mkdir -p '${BUILD_DIR}' && cd '${BUILD_DIR}' && cmake .. && cmake --build . --config Release" >&2
  exit 1
fi

# Clean previous
rm -f "${RESULT_FILE}"
rm -rf "${ARTIFACTS_DIR}"
mkdir -p "${ARTIFACTS_DIR}"

pass_cnt=0
case_cnt=0

run_case() {
  local name="$1"
  local cmd="$2"
  local expected_file="$3"
  local actual_file="$4"
  ((case_cnt++))
  echo "Running ${name}..."
  bash -c "${cmd}" >"${actual_file}" 2>&1 || true
  if diff -u "${expected_file}" "${actual_file}" >/dev/null; then
    echo "${name} PASS" | tee -a "${RESULT_FILE}"
    ((pass_cnt++))
  else
    echo "${name} FAIL" | tee -a "${RESULT_FILE}"
    echo "--- Expected:" >>"${RESULT_FILE}"
    cat "${expected_file}" >>"${RESULT_FILE}"
    echo "--- Actual:" >>"${RESULT_FILE}"
    cat "${actual_file}" >>"${RESULT_FILE}"
    echo >>"${RESULT_FILE}"
  fi
}

# Case 1: cmdlist helloworld
run_case "case1_helloworld" "'${GAME_EXE}' < '${CASES_DIR}/helloworld/cmdlist.txt'" \
         "${CASES_DIR}/helloworld/expected.txt" "${ARTIFACTS_DIR}/helloworld_actual.txt"

# Case 2: direct command
printf "Hello World!\n" >"${CASES_DIR}/helloworld/expected_direct.txt"
run_case "case2_direct_command" "'${GAME_EXE}' testhelloworld" \
         "${CASES_DIR}/helloworld/expected_direct.txt" "${ARTIFACTS_DIR}/direct_actual.txt"

# Summary
echo "" >>"${RESULT_FILE}"
echo "Summary: ${pass_cnt}/${case_cnt} passed" | tee -a "${RESULT_FILE}"

if [[ ${pass_cnt} -eq ${case_cnt} ]]; then
  echo -e "${GREEN}ALL TESTS PASSED${NC}"
  exit 0
else
  echo -e "${RED}SOME TESTS FAILED${NC}"
  exit 1
fi
