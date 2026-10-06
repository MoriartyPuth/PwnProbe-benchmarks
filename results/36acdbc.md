# Results — PwnProbe `36acdbc`

Solve benchmark of every lab in [`manifest.json`](../manifest.json) against the
PwnProbe build at commit
[`36acdbc`](https://github.com/MoriartyPuth/PwnProbes/commit/36acdbc) (ROP chain
planner, phase 0+1). Machine-readable: [`36acdbc.json`](36acdbc.json).

| Field | Value |
|-------|-------|
| PwnProbe commit | `36acdbc` |
| Date | 2026-10-07 |
| OS | Ubuntu 24.04.3 LTS |
| glibc | 2.39 |
| gcc | 13.3.0 |
| Solver per-run timeout | 2s |
| Flag pattern | `(?i)flag\{[^}]+\}` |

## Summary

**18 / 18 labs solved** with the expected flag, each via its expected strategy.
Every PwnProbe solve strategy is exercised.

| Lab | Strategy | Flag | Time |
|-----|----------|:----:|-----:|
| lab01_variable_overwrite | stack_overwrite | ✓ | 0.7s |
| lab02_ret2win_basic | stack_overwrite | ✓ | 0.2s |
| lab03_ret2win_alignment | stack_overwrite | ✓ | 1.6s |
| lab04_ret2win_params | rop_ret2win_args | ✓ | 16.8s |
| lab05_shellcode_execstack | shellcode | ✓ | 8.0s |
| lab06_format_string_leak | stack_dump | ✓ | 0.3s |
| lab07_ret2libc_intro | ret2libc | ✓ | 15.0s |
| lab08_fmt_got_overwrite | fmt_got_overwrite | ✓ | 3.1s |
| lab09_fmt_write_ret | fmt_write_ret | ✓ | 7.1s |
| lab10_canary_ret2win | canary_ret2win | ✓ | 148.8s |
| lab11_ret2syscall | ret2syscall | ✓ | 128.6s |
| lab12_partial_overwrite | partial_overwrite | ✓ | 16.4s |
| lab13_angr_logic | angr_logic | ✓ | 2.2s |
| lab14_ret2plt_string | ret2plt | ✓ | 6.4s |
| lab15_rop_chain | rop_planner | ✓ | 255.8s |
| heap_simple | heap_uaf | ✓ | 4.0s |
| heap_realistic | heap_uaf | ✓ | 4.9s |
| heap_variety | heap_uaf | ✓ | 15.7s |

Detection manifest ([`detect-manifest.json`](../detect-manifest.json)) passes
18/18 under `pwnprobe benchmark`.

## Real binaries (ROP Emporium)

Via [`third-party-manifest.json`](../third-party-manifest.json) against the
prebuilt binaries in [`third_party/ropemporium/`](../third_party/ropemporium):

| Challenge | Strategy | Result |
|-----------|----------|--------|
| ropemporium_ret2win | stack_overwrite | solved (`ROPE{…}`) |
| ropemporium_split | ret2plt | solved (`ROPE{…}`) |
| ropemporium_callme | rop_planner | solved (`ROPE{…}`) |

See [`real-world-ropemporium.md`](real-world-ropemporium.md).

## Notes

- `lab15_rop_chain` is the slowest synthetic lab: the planner runs only after
  every recipe (including a ~90s angr exploration, since its `print_flag` is a
  symbol angr targets), then sweeps offsets × call orderings × argument
  permutations. It finishes well under the 300s per-lab wall but is the thinnest
  margin.
- `lab10`/`lab11` are session sweeps (one connection per attempt).
- `lab12` is probabilistic (one ASLR nibble); its time varies.
- `lab13`/`lab15` benefit from `PWNPROBE_PYTHON` (angr); `lab13` needs it.

## Reproduce

```sh
git -C PwnProbes checkout 36acdbc
( cd PwnProbes && GOOS=linux GOARCH=amd64 go build -o bin/pwnprobe ./cmd/pwnprobe )
PWNPROBE_PYTHON=/path/to/angr-venv/bin/python \
  ./run.py --pwnprobe PwnProbes/bin/pwnprobe --out results/36acdbc.json
./run.py --pwnprobe PwnProbes/bin/pwnprobe --manifest third-party-manifest.json \
  --out results/real-world-ropemporium.json
```
