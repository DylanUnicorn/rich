# cleanup_test_outputs.py
import os
import glob
import sys
from pathlib import Path


def main(root_dir):
    """
    在指定的根目录下查找并删除 test/*/[指定文件名] 的文件
    """
    # --- 只需修改这里 ---
    # 定义所有需要清理的文件名
    filenames_to_clean = [
        "dump.json",
        "output.txt",
        "result.txt",
        "result.json"
    ]
    # -------------------

    print(f"将在根目录 '{root_dir}' 中清理以下文件: {', '.join(filenames_to_clean)}")

    all_files_to_delete = []
    test_dir = Path(root_dir) / 'test'

    # 遍历每个需要清理的文件名
    for filename in filenames_to_clean:
        # 构建 glob 模式，例如：/path/to/project/test/*/dump.json
        search_pattern = str(test_dir / '*' / filename)

        # 查找匹配的文件并添加到总列表中
        found_files = glob.glob(search_pattern)
        if found_files:
            all_files_to_delete.extend(found_files)

    if not all_files_to_delete:
        print("没有找到需要清理的测试输出文件。")
        return

    print("\n找到以下文件将被删除:")
    for f_path in all_files_to_delete:
        try:
            print(f"- {f_path}")
            os.remove(f_path)
        except OSError as e:
            print(f"错误：删除文件 {f_path} 失败: {e}", file=sys.stderr)

    print("\n清理完成。")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    else:
        # 默认使用脚本所在的目录作为项目根目录
        project_root = Path(__file__).resolve().parent

    main(project_root)