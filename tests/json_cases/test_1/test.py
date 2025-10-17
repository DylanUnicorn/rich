import json
import sys
import os
import subprocess
import pathlib
import difflib

def parse_multiple_json(file_path):
    results = []
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
        decoder = json.JSONDecoder()
        idx = 0
        length = len(content)
        while idx < length:
            # 跳过空白
            while idx < length and content[idx].isspace():
                idx += 1
            if idx >= length:
                break
            obj, end = decoder.raw_decode(content, idx)
            results.append(obj)
            idx = end
    return results

def run_single_test(executable_path: pathlib.Path, test_dir: pathlib.Path):
    """
    为单个测试目录运行测试、重定向I/O并比较结果。
    根据成功或失败返回退出码。
    """
    test_name = test_dir.name
    print(f"--- Running test case: {test_name} ---")

    input_file = test_dir / "input.txt"    # 程序标准输入重定向自此文件，指令以换行符分隔
    expected_file = test_dir / "expected_result.json"
    preset_file = test_dir / "preset.json"

    dump_file = test_dir / "dump.json"
    output_file = test_dir / "output.txt"  # 程序标准输出重定向到此文件

    if not input_file.exists():
        print(f"FAIL: 'input.txt' not found in {test_dir}", file=sys.stderr)
        return 1  # 失败退出码
    if not expected_file.exists():
        print(f"FAIL: 'expected_result.txt' not found in {test_dir}", file=sys.stderr)
        return 1

    try:
        with open(input_file, "r", encoding="utf-8") as infile, \
             open(output_file, "w", encoding="utf-8", newline="\n") as outfile:

            indata = infile.read()
            if not indata.endswith('\n'):
                indata += '\n'  # 确保输入以换行符结尾
            indata += 'quit\n'  # 添加退出命令
            indata = indata.encode('utf-8')

            # 执行C程序
            result = subprocess.run(
                [executable_path, test_dir],
                input=indata,
                stdout=outfile,
                stderr=subprocess.PIPE,
                timeout=1.5,  # 1.5秒超时
                check=False  # 不要对非零退出码抛出异常
            )

        if result.returncode != 0:
            print(f"FAIL: Program exited with non-zero code: {result.returncode}", file=sys.stderr)
            if result.stderr:
                print(f"Stderr:\n{result.stderr.decode('utf-8', errors='ignore')}", file=sys.stderr)
            return 1

        if result.stderr:
            print(f"FAIL: Program produced stderr output:\n{result.stderr.decode('utf-8', errors='ignore')}", file=sys.stderr)
            return 1

        if not dump_file.exists():
            print(f"FAIL: Dump file '{dump_file}' was not created.", file=sys.stderr)
            # return 1
            actual_results = []
        else:
            actual_results = parse_multiple_json(dump_file)
        expected_results = parse_multiple_json(expected_file)

        if len(actual_results) < len(expected_results):
            print(f"FAIL: Test '{test_name}' failed. 实际结果条目数少于预期。", file=sys.stderr)
            return 1

        if compare_result(expected_results, actual_results, test_name) is None:
            print(f"PASS: Test '{test_name}' passed.")
            return 0
        else:
            print(f"FAIL: Test '{test_name}' failed. 结果不匹配。", file=sys.stderr)
            return 1

    except subprocess.TimeoutExpired:
        print(f"FAIL: Test '{test_name}' timed out after 5 seconds.", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"FAIL: An unexpected error occurred in test '{test_name}': {e}", file=sys.stderr)
        return 1


import sys
from collections import Counter


# 新的递归比较函数
def deep_compare_json(expected, actual, path="root"):
    """
    递归地比较两个 JSON 对象（字典或列表）。
    - 只检查 expected 中存在的字段。
    - 根据 path 应用特殊比较规则。
    """
    # 检查类型是否匹配
    if type(expected) is not type(actual):
        print(f"FAIL: Type mismatch at '{path}'. Expected {type(expected).__name__}, but got {type(actual).__name__}.",
              file=sys.stderr)
        return False

    # 字典的比较逻辑
    if isinstance(expected, dict):
        # 针对 houses 的特殊规则：键和数量必须完全匹配
        if path == "root.houses":
            expected_keys = set(expected.keys())
            actual_keys = set(actual.keys())
            if expected_keys != actual_keys:
                print(f"FAIL: Mismatch in house locations at '{path}'.", file=sys.stderr)
                if expected_keys - actual_keys:
                    print(f"  Missing in actual: {sorted(list(expected_keys - actual_keys))}", file=sys.stderr)
                if actual_keys - expected_keys:
                    print(f"  Extra in actual: {sorted(list(actual_keys - expected_keys))}", file=sys.stderr)
                return False

        # 遍历 expected 字典中的所有键值对
        for key, expected_value in expected.items():
            if key not in actual:
                print(f"FAIL: Key '{key}' missing at path '{path}'.", file=sys.stderr)
                return False

            # 递归调用进行深层比较
            if not deep_compare_json(expected_value, actual[key], f"{path}.{key}"):
                return False
        return True

    # 列表的比较逻辑
    elif isinstance(expected, list):
        # 针对 players 列表的特殊规则：乱序比较，通过 index 匹配
        if path == "root.players":
            if len(expected) > len(actual):
                print(
                    f"FAIL: Actual player list is shorter than expected at '{path}'. Expected {len(expected)}, got {len(actual)}.",
                    file=sys.stderr)
                return False

            # 将 actual 列表转换为以 index 为键的字典，便于查找
            actual_map = {p.get("index"): p for p in actual if isinstance(p, dict)}

            for expected_player in expected:
                idx = expected_player.get("index")
                if idx not in actual_map:
                    print(f"FAIL: Player with index {idx} not found at '{path}'.", file=sys.stderr)
                    return False

                actual_player = actual_map[idx]

                expected_player_copy = expected_player.copy()

                if not deep_compare_json(expected_player_copy, actual_player, f"{path}[{idx}]"):
                    return False
            return True

        # 针对 placed_prop 内列表的特殊规则：乱序比较
        elif path in ("root.placed_prop.bomb", "root.placed_prop.barrier"):
            # 使用 Counter 来比较两个列表的内容是否一致（忽略顺序）
            if Counter(expected) != Counter(actual):
                print(f"FAIL: Item mismatch in list at '{path}'.", file=sys.stderr)
                print(f"  Expected (sorted): {sorted(expected)}", file=sys.stderr)
                print(f"  Actual (sorted):   {sorted(actual)}", file=sys.stderr)
                return False
            return True

        # 其他列表的通用比较逻辑：顺序必须一致
        else:
            if len(expected) > len(actual):
                print(f"FAIL: Actual list is shorter than expected at '{path}'.", file=sys.stderr)
                return False
            for i, expected_item in enumerate(expected):
                if not deep_compare_json(expected_item, actual[i], f"{path}[{i}]"):
                    return False
            return True

    # 基本数据类型（int, str, bool等）的比较逻辑
    else:
        if path == "root.god.location":
            return True
        if expected != actual:
            print(f"FAIL: Value mismatch at '{path}'.", file=sys.stderr)
            print(f"  Expected: {expected!r}", file=sys.stderr)
            print(f"  Actual:   {actual!r}", file=sys.stderr)
            return False
        return True


# 替换原来的 compare_result 函数
def compare_result(expected_results, actual_results, test_name=""):
    # 逐条比较 JSON 对象（游戏状态）
    for i, expected in enumerate(expected_results):
        try:
            actual = actual_results[i]
        except IndexError:
            print(
                f"FAIL: Test '{test_name}' failed. Actual results have fewer entries than expected ({len(actual_results)} vs {len(expected_results)}).",
                file=sys.stderr)
            return 1  # 失败

        # 调用新的递归比较函数
        if not deep_compare_json(expected, actual):
            print(f"--- Mismatch found in result entry {i + 1} for test '{test_name}'.", file=sys.stderr)
            return 1  # 失败

        if expected.get('god', False):
            expected = expected.get('god')
            actual = actual.get('god', {})
            if not actual:
                return 1

            if expected.get('location', -1) == -1:
                if actual.get('location', -2) != -1:
                    print(f"FAIL: God location should be -1 when expected is -1.", file=sys.stderr)
                    return 1
            else:
                unplaceable_locations = [35, 28, -1]  # 道具店、礼品店
                actual_location = actual.get('location', -1)
                # 添加对 actual_location 的范围检查
                # 获取所有玩家位置
                player_locations = set()
                if 'players' in actual:
                    for player in actual['players']:
                        player_locations.add(player['location'])

                # 获取已放置道具位置
                placed_prop_locations = set()
                if 'placed_prop' in actual:
                    placed_prop = actual['placed_prop']
                    if 'bomb' in placed_prop:
                        placed_prop_locations.update(placed_prop['bomb'])
                    if 'barrier' in placed_prop:
                        placed_prop_locations.update(placed_prop['barrier'])

                # 检查财神位置是否合法
                forbidden_locations = unplaceable_locations + list(player_locations) + list(placed_prop_locations)
                if actual_location in forbidden_locations:
                    print(f"FAIL: God location {actual_location} is forbidden (conflicts with players, props, or special areas).", file=sys.stderr)
                    return 1

    return None  # 成功（遵循原脚本的返回逻辑）

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python test.py <path_to_executable> <path_to_test_directory>", file=sys.stderr)
        sys.exit(2)

    executable = pathlib.Path(sys.argv[1]).resolve()
    test_dir = pathlib.Path(sys.argv[2]).resolve()

    if not executable.exists():
        print(f"Error: Executable not found at '{executable}'", file=sys.stderr)
        sys.exit(2)

    if not test_dir.is_dir():
        print(f"Error: Test directory not found at '{test_dir}'", file=sys.stderr)
        sys.exit(2)

    # 执行单个测试并使用其退出码作为脚本的退出码
    exit_code = run_single_test(executable, test_dir)
    sys.exit(exit_code)

    debug_actual_json = parse_multiple_json("debug_test/dump.json")
    debug_expected_json = parse_multiple_json("debug_test/expected_result.json")

    result = compare_result(debug_expected_json, debug_actual_json, "debug")
    if result is None:
        print("Debug comparison passed")
    else:
        print("Debug comparison failed")