# PwnProbe-benchmarks

Evaluation material for [**PwnProbe**](https://github.com/MoriartyPuth/PwnProbes),
a Go CLI that automatically solves simple local/remote CTF pwn challenges. This
repository holds the challenge **sources**, their **build recipes**, a
**manifest** describing each one, the **expected outcomes**, and **comparison
results** tied to a specific PwnProbe commit.

It is kept separate from the tool so that users can clone just the tool, while
contributors can optionally download the labs to reproduce or extend the
evaluation. The tool repository keeps only small unit tests (parsing, payload
serialization, cancellation, flag extraction); no labs live there.

## What is measured

For each lab the harness builds the source with its recorded flags, stages the
lab's `flag.txt` beside the binary, runs `pwnprobe solve` in that directory, and
checks that the solver **reports solved and recovers exactly the expected flag**.
A flag counts only when the program discloses it and it was not present in the
payload — the same honesty rule PwnProbe uses internally — so a crash or an
echoed input never registers as a solve. The winning strategy is recorded; a
strategy other than the one expected is reported as a warning, not a failure,
because the real signal is whether the planted flag came out.

This is a **known-to-the-implementation** suite: every lab maps to a strategy the
tool implements, so it is a coverage and regression suite, not a held-out
generalization benchmark.

## Layout

```
labs/<name>/vuln.c      lab source
labs/<name>/flag.txt     the planted flag (read at runtime / via a spawned shell)
manifest.json            every lab: source, build flags, class, expected strategy + flag, protections
run.py                   solve-benchmark harness (build -> solve -> compare)
detect-manifest.json     detection cases for the tool's own `pwnprobe benchmark` command
results/<commit>.json    machine-readable comparison results for a PwnProbe commit
results/<commit>.md      human-readable summary of the same run
```

## Lab catalog

Each lab is 64-bit and exercises one PwnProbe strategy.

| Lab | Class | Expected strategy |
|-----|-------|-------------------|
| lab01_variable_overwrite | stack overflow / guard variable | `stack_overwrite` |
| lab02_ret2win_basic | stack overflow / ret2win | `stack_overwrite` |
| lab03_ret2win_alignment | ret2win + movaps alignment | `stack_overwrite` |
| lab04_ret2win_params | two-argument ret2win (ROP) | `rop_ret2win_args` |
| lab05_shellcode_execstack | shellcode on an executable stack | `shellcode` |
| lab06_format_string_leak | format-string read | `stack_dump` |
| lab07_ret2libc_intro | ret2libc | `ret2libc` |
| lab08_fmt_got_overwrite | format-string GOT overwrite | `fmt_got_overwrite` |
| lab09_fmt_write_ret | format-string write to saved RIP (full RELRO) | `fmt_write_ret` |
| lab10_canary_ret2win | canary bypass via format-string leak | `canary_ret2win` |
| lab11_ret2syscall | execve ROP, statically linked | `ret2syscall` |
| lab12_partial_overwrite | PIE partial overwrite, no leak | `partial_overwrite` |
| lab13_angr_logic | logic/stdin puzzle (symbolic execution) | `angr_logic` |
| heap_simple | heap UAF, minimal menu | `heap_uaf` |
| heap_realistic | heap UAF, index + size prompts | `heap_uaf` |
| heap_variety | heap UAF, synonym menu, fn ptr at offset 8 | `heap_uaf` |

The exact build flags, expected flags, and protection profiles are in
[`manifest.json`](manifest.json).

## Running the suite

Requires Linux with `gcc` and Python 3, plus a built PwnProbe binary from the
[tool repo](https://github.com/MoriartyPuth/PwnProbes). On Windows, build a Linux
binary (`GOOS=linux GOARCH=amd64`) and run everything inside WSL.

```sh
# from the tool repo: build the solver
go build -o bin/pwnprobe ./cmd/pwnprobe

# from this repo: run the whole suite
./run.py --pwnprobe /path/to/pwnprobe --out results/$(git -C /path/to/PwnProbes rev-parse --short HEAD).json
```

`lab13_angr_logic` needs symbolic execution: set `PWNPROBE_PYTHON` to a Python
that has [angr](https://angr.io) installed, otherwise that strategy is skipped
and the lab is reported unsolved.

```sh
PWNPROBE_PYTHON=/path/to/angr-venv/bin/python ./run.py --pwnprobe /path/to/pwnprobe
```

Useful flags: `--only NAME` (repeatable) to run one lab, `--timeout` for the
solver's per-run deadline, `--wall` for the per-lab wall-clock limit.

### Detection cases

[`detect-manifest.json`](detect-manifest.json) is consumable by PwnProbe's own
detection benchmark and pins the detector's current observations (format-string
behaviour, crash, and protection readings) per lab:

```sh
pwnprobe benchmark --manifest detect-manifest.json --json
```

## Environment notes

Results depend on the host toolchain and libc:

- `lab07_ret2libc_intro` resolves offsets from the **local** libc; the recorded
  run used that libc as the target. A different libc needs `pwnprobe solve
  --libc`.
- The `heap_*` labs rely on glibc tcache behaviour. The function-pointer UAF they
  use does not touch tcache `fd` pointers, so **safe-linking (glibc >= 2.32) does
  not affect them** — but tcache-poisoning labs (not present here) would.
- `lab12_partial_overwrite` is **probabilistic** (one ASLR nibble): the solver
  retries across fresh runs, so its wall-clock time varies run to run.

The recorded results state the PwnProbe commit, the OS, the glibc version, and
the gcc version so a run can be reproduced or explained.

## Contributing a lab

1. Add `labs/<name>/vuln.c` and `labs/<name>/flag.txt`.
2. Add a `manifest.json` entry: `source`, `build_flags`, `class`,
   `expected_strategy`, `expected_flag`, and the `protections` profile.
3. Run `./run.py --pwnprobe ... --only <name>` and confirm it solves.
4. Regenerate `results/<commit>.*` and note the commit evaluated.

Licensed under MIT, matching the tool.
