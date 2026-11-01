# Results — held-out generalization set (real ROP Emporium binaries)

This is the **generalization** benchmark: prebuilt, third-party challenge
binaries ([ROP Emporium](https://ropemporium.com/)) that **no PwnProbe strategy
was tuned to**, staged via [`third-party-manifest.json`](../third-party-manifest.json)
from [`third_party/ropemporium/`](../third_party/ropemporium). It is reported
**separately** from the known-to-implementation synthetic lab suite
([`results/<commit>.md`](.)), because the synthetic labs each map to a strategy
by construction and so measure coverage/regression, not generalization.

Machine-readable: [`real-world-ropemporium.json`](real-world-ropemporium.json).

| Field | Value |
|-------|-------|
| PwnProbe commit | `02827fe` (1.0.0 + follow-on PRs 16–20) |
| Date | 2026-11-01 |
| OS | Fedora Linux 44 |
| glibc | 2.43 |
| gcc | 16.2.1 |
| Solver per-run timeout | 2s |
| Per-challenge wall | 900s |

## Summary

**3 / 3 solved**, each with the real flag, on binaries the tool was not built
against.

| Challenge | Winning strategy | Flag | Time |
|-----------|------------------|:----:|-----:|
| ropemporium_ret2win | stack_overwrite | ✓ | 9.2s |
| ropemporium_split | ret2plt | ✓ | 120.1s |
| ropemporium_callme | rop_planner | ✓ | 303.7s |

- Solve rate: 3/3. No failures, no timeouts.
- Latency: median 120.1s; tail (max) 303.7s (`callme`, the multi-call ROP chain
  synthesized by the planner). `callme` needs a wall above the default 300s on
  this host, so the suite runs with `--wall 900`.
- Timings are host-specific and are not performance targets; set any numeric
  threshold from numbers reproduced on the target host.

## Reproduce

```sh
git -C PwnProbes checkout 02827fe
( cd PwnProbes && go build -o bin/pwnprobe ./cmd/pwnprobe )
./run.py --pwnprobe PwnProbes/bin/pwnprobe --manifest third-party-manifest.json \
  --wall 900 --out results/real-world-ropemporium.json
```
