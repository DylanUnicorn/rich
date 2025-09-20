# Tests Folder Structure

Recommended layout for organizing test artifacts and scripts:

```
tests/
  run_tests.sh           # Linux/Unix test runner
  run_tests.bat          # Windows test runner
  run_all_tests.bat      # Windows multi-case runner
  README.md
  cases/
    helloworld/
      cmdlist.txt        # stdin commands for this case
      expected.txt       # expected output for stdin mode
      expected_direct.txt# expected output for direct mode
  artifacts/             # auto-generated actual outputs
```

How to add a new case:
1. Create a subfolder under `cases/<case-name>/`
2. Add `cmdlist.txt` with input commands (end with `dump` if needed)
3. Add `expected.txt` capturing the expected output
4. Update `run_tests.sh` (and/or `.bat`) to include the new case

These conventions keep the repo root clean and make it easier to scale tests.
