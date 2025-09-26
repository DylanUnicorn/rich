#!/usr/bin/env python3
import json
import subprocess
import sys
import os

def load_json(path):
    with open(path, 'r', encoding='utf-8') as f:
        return json.load(f)

def json_equal(a, b):
    return a == b

def run_case(game_exe, case_dir):
    input_path = os.path.join(case_dir, 'input.txt')
    expected_path = os.path.join(case_dir, 'expected_result.json')
    dump_path = os.path.join(case_dir, 'dump.json')

    if not os.path.exists(input_path):
        print(f"[SKIP] {case_dir} missing input.txt")
        return None
    if not os.path.exists(expected_path):
        print(f"[SKIP] {case_dir} missing expected_result.json")
        return None

    with open(input_path, 'rb') as fin:
        proc = subprocess.run([game_exe, case_dir], stdin=fin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if proc.returncode != 0:
        print(f"[FAIL] {os.path.basename(case_dir)}: process exited with {proc.returncode}")
        sys.stdout.buffer.write(proc.stdout)
        sys.stderr.buffer.write(proc.stderr)
        return False
    if not os.path.exists(dump_path):
        print(f"[FAIL] {os.path.basename(case_dir)}: dump.json not generated")
        return False

    dump = load_json(dump_path)
    expected = load_json(expected_path)
    if json_equal(dump, expected):
        print(f"[PASS] {os.path.basename(case_dir)}")
        return True
    else:
        print(f"[FAIL] {os.path.basename(case_dir)}: JSON mismatch")
        # Print a small diff-like here (keys only) to help
        print("  got:    ", json.dumps(dump, ensure_ascii=False, indent=2)[:400])
        print("  expect: ", json.dumps(expected, ensure_ascii=False, indent=2)[:400])
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: runner.py <game_exe> [cases_dir]")
        return 2
    game_exe = sys.argv[1]
    cases_root = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(__file__), 'json_cases')
    if not os.path.isdir(cases_root):
        print(f"Cases dir not found: {cases_root}")
        return 2

    case_dirs = [os.path.join(cases_root, d) for d in os.listdir(cases_root) if os.path.isdir(os.path.join(cases_root, d))]
    passed = 0
    total = 0
    for c in sorted(case_dirs):
        res = run_case(game_exe, c)
        if res is None:
            continue
        total += 1
        if res:
            passed += 1
    print(f"Summary: {passed}/{total} passed")
    return 0 if passed == total else 1

if __name__ == '__main__':
    sys.exit(main())
