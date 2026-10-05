# Results — PwnProbe `c70b9bc`

Solve benchmark of every lab in [`manifest.json`](../manifest.json) against the
PwnProbe build at commit
[`c70b9bc`](https://github.com/MoriartyPuth/PwnProbes/commit/c70b9bc).
Machine-readable data: [`c70b9bc.json`](c70b9bc.json).

| Field | Value |
|-------|-------|
| PwnProbe commit | `c70b9bc` |
| Date | 2026-10-05 |
| OS | Ubuntu 24.04.3 LTS |
| glibc | 2.39 |
| gcc | 13.3.0 |
| Solver per-run timeout | 2s |
| Flag pattern | `(?i)flag\{[^}]+\}` |

## Summary

**17 / 17 labs solved** with the expected flag, each via its expected strategy.
Every one of PwnProbe's solve strategies is exercised.

| Lab | Strategy | Flag | Time |
|-----|----------|:----:|-----:|
| lab01_variable_overwrite | stack_overwrite | ✓ | 0.7s |
| lab02_ret2win_basic | stack_overwrite | ✓ | 0.3s |
| lab03_ret2win_alignment | stack_overwrite | ✓ | 1.7s |
| lab04_ret2win_params | rop_ret2win_args | ✓ | 15.3s |
| lab05_shellcode_execstack | shellcode | ✓ | 8.2s |
| lab06_format_string_leak | stack_dump | ✓ | 0.1s |
| lab07_ret2libc_intro | ret2libc | ✓ | 13.7s |
| lab08_fmt_got_overwrite | fmt_got_overwrite | ✓ | 2.5s |
| lab09_fmt_write_ret | fmt_write_ret | ✓ | 5.5s |
| lab10_canary_ret2win | canary_ret2win | ✓ | 149.4s |
| lab11_ret2syscall | ret2syscall | ✓ | 217.3s |
| lab12_partial_overwrite | partial_overwrite | ✓ | 20.1s |
| lab13_angr_logic | angr_logic | ✓ | 4.6s |
| lab14_ret2plt_string | ret2plt | ✓ | 10.1s |
| heap_simple | heap_uaf | ✓ | 4.0s |
| heap_realistic | heap_uaf | ✓ | 15.8s |
| heap_variety | heap_uaf | ✓ | 15.8s |

Detection manifest ([`detect-manifest.json`](../detect-manifest.json)) also
passes 17/17 under `pwnprobe benchmark`.

## Real binaries

PwnProbe is also run against the prebuilt third-party ROP Emporium binaries in
[`third_party/ropemporium/`](../third_party/ropemporium), via
[`third-party-manifest.json`](../third-party-manifest.json):

| Challenge | Strategy | Result |
|-----------|----------|--------|
| ropemporium_ret2win | stack_overwrite | solved (`ROPE{…}`) |
| ropemporium_split | ret2plt | solved (`ROPE{…}`) |
| ropemporium_callme | — | out of scope (3×3-arg chain); correctly not solved |

See [`real-world-ropemporium.md`](real-world-ropemporium.md).

## Notes

- `lab10`/`lab11` are the slow cases (session sweeps, one connection per attempt).
- `lab12` is probabilistic (one ASLR nibble); its time varies.
- `lab13` needs `PWNPROBE_PYTHON` set to a Python with angr.

## Reproduce

```sh
git -C PwnProbes checkout c70b9bc
( cd PwnProbes && GOOS=linux GOARCH=amd64 go build -o bin/pwnprobe ./cmd/pwnprobe )
PWNPROBE_PYTHON=/path/to/angr-venv/bin/python \
  ./run.py --pwnprobe PwnProbes/bin/pwnprobe --out results/c70b9bc.json
./run.py --pwnprobe PwnProbes/bin/pwnprobe --manifest third-party-manifest.json \
  --out results/real-world-ropemporium.json
```
