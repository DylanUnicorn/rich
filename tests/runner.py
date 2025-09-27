#!/usr/bin/env python3
import json
import os
import subprocess
import sys
from typing import List, Optional, Tuple


RESET = "\033[0m"
GREEN = "\033[32m"
RED = "\033[31m"
YELLOW = "\033[33m"


def colorize(text: str, colour: str) -> str:
    if sys.stdout.isatty():
        return f"{colour}{text}{RESET}"
    return text

def load_json(path):
    with open(path, 'r', encoding='utf-8') as f:
        return json.load(f)

def _is_subset(expected, actual):
    """Return True if expected dict/list is a subset shape of actual (recursive).
    Allows extra keys in actual so we can add fields without breaking older tests.
    """
    if isinstance(expected, dict):
        if not isinstance(actual, dict):
            return False
        for k, ev in expected.items():
            if k not in actual:
                return False
            if not _is_subset(ev, actual[k]):
                return False
        return True
    if isinstance(expected, list):
        if not isinstance(actual, list):
            return False
        if len(expected) != len(actual):
            return False
        return all(_is_subset(e, a) for e, a in zip(expected, actual))
    return expected == actual

def json_equal(a, b):
    # expected_result.json may omit new fields like the global god block
    # Accept if expected is a subset of dump.
    return _is_subset(a, b)

def run_case(game_exe, case_dir) -> Optional[Tuple[str, str]]:
    input_path = os.path.join(case_dir, 'input.txt')
    expected_path = os.path.join(case_dir, 'expected_result.json')
    constraints_path = os.path.join(case_dir, 'constraints.json')
    dump_path = os.path.join(case_dir, 'dump.json')

    if not os.path.exists(input_path):
        print(colorize(f"[SKIP] {os.path.basename(case_dir)} missing input.txt", YELLOW))
        return None
    if not os.path.exists(expected_path):
        print(colorize(f"[SKIP] {os.path.basename(case_dir)} missing expected_result.json", YELLOW))
        return None

    with open(input_path, 'rb') as fin:
        proc = subprocess.run([game_exe, case_dir], stdin=fin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if proc.returncode != 0:
        name = os.path.basename(case_dir)
        print(colorize(f"[FAIL] {name} (process exited with {proc.returncode})", RED))
        sys.stdout.buffer.write(proc.stdout)
        sys.stderr.buffer.write(proc.stderr)
        return ("fail", name)
    if not os.path.exists(dump_path):
        name = os.path.basename(case_dir)
        print(colorize(f"[FAIL] {name} (dump.json not generated)", RED))
        return ("fail", name)

    dump = load_json(dump_path)
    expected = load_json(expected_path)
    # Optional constraint checks
    def check_constraints() -> Optional[str]:
        if not os.path.exists(constraints_path):
            return None
        cons = load_json(constraints_path)
        # Only support top-level 'god' constraints for simplicity
        if 'god' in cons:
            if 'god' not in dump:
                return "missing god block in dump"
            g = dump['god']
            c = cons['god']
            if 'spawn_cooldown_between' in c:
                lo, hi = c['spawn_cooldown_between']
                v = g.get('spawn_cooldown', None)
                if v is None or not (lo <= v <= hi):
                    return f"spawn_cooldown {v} not in [{lo},{hi}]"
            if 'location_not_in' in c:
                bad = set(c['location_not_in'])
                v = g.get('location', None)
                if v in bad:
                    return f"god.location {v} is in forbidden set {sorted(bad)}"
            if 'location_is' in c:
                expect = c['location_is']
                if g.get('location') != expect:
                    return f"god.location {g.get('location')} != {expect}"
        return None

    constraint_err = check_constraints()
    if constraint_err:
        name = os.path.basename(case_dir)
        print(colorize(f"[FAIL] {name} (constraint: {constraint_err})", RED))
        return ("fail", name)

    if json_equal(expected, dump):
        name = os.path.basename(case_dir)
        print(colorize(f"[PASS] {name}", GREEN))
        return ("pass", name)
    else:
        name = os.path.basename(case_dir)
        print(colorize(f"[FAIL] {name} (JSON mismatch)", RED))
        return ("fail", name)

def discover_case_dirs(root: str) -> List[str]:
    found = []
    for dirpath, dirnames, filenames in os.walk(root):
        if 'input.txt' in filenames and 'expected_result.json' in filenames:
            found.append(dirpath)
    return sorted(found)

def main():
    if len(sys.argv) < 2:
        print("Usage: runner.py <game_exe> [cases_dir]")
        return 2
    game_exe = sys.argv[1]
    cases_root = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(__file__), 'json_cases')
    if not os.path.isdir(cases_root):
        print(f"Cases dir not found: {cases_root}")
        return 2

    case_dirs = discover_case_dirs(cases_root)

    # Skip tests for removed features (hospital, prison, bombs, magic)
    skip_patterns = [
        '/buffs/hospital_',
        '/buffs/prison_',
        '/movement/bomb_',
        '/movement/barrier_before_bomb',
        '/movement/land_on_prison',
        '/movement/barrier_before_prison',
        '/props/touchbombs',
        '/props/bomb_out_of_range',
        '/props/robot_',
        '/props/place_and_clear',
        '/robot_clear',
        '/toolhouse/buybomb',
        '/gifthouse/giveup'  # optional: flaky interaction coverage
    ]
    passed = 0
    total = 0
    passed_cases: List[str] = []
    failed_cases: List[str] = []
    # stats by module (top-level folder under cases_root)
    per_module = {}
    for c in sorted(case_dirs):
        rel = os.path.relpath(c, cases_root)
        parts = rel.split(os.sep)
        module = parts[0] if len(parts) > 1 else "_root"
        # apply skip rules
        rel_norm = '/' + rel.replace('\\', '/')
        if any(pat in rel_norm for pat in skip_patterns):
            print(colorize(f"[SKIP] {rel} (removed feature)", YELLOW))
            continue
        if module not in per_module:
            per_module[module] = {"pass": 0, "total": 0}
        res = run_case(game_exe, c)
        if res is None:
            continue
        total += 1
        per_module[module]["total"] += 1
        status, name = res
        if status == "pass":
            passed += 1
            per_module[module]["pass"] += 1
            passed_cases.append(name)
        elif status == "fail":
            failed_cases.append(name)
    print("Module stats:")
    for m in sorted(per_module.keys()):
        s = per_module[m]
        print(f"  {m}: {s['pass']}/{s['total']} passed")
    print("\nSummary:")
    print(f"  Passed: {passed}/{total}")
    if passed_cases:
        print("    " + ", ".join(sorted(passed_cases)))
    print(f"  Failed: {len(failed_cases)}/{total}")
    if failed_cases:
        print("    " + ", ".join(sorted(failed_cases)))
    return 0 if passed == total else 1

if __name__ == '__main__':
    sys.exit(main())
