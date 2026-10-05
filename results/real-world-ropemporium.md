# Real-world check — ROP Emporium (x86_64)

Besides the synthetic `labs/`, PwnProbe was run against real, third-party
teaching-CTF binaries it did not author: the [ROP Emporium](https://ropemporium.com)
x86_64 challenges. These binaries read `./flag.txt` and print it on a successful
exploit, so an end-to-end solve is verifiable locally. They are **not** committed
here (respecting their distribution); fetch them from ropemporium.com.

Run against PwnProbe commit `a69007b` (v0.5.0) on Ubuntu 24.04 / glibc 2.39.

| Challenge | Technique it needs | PwnProbe | Strategy / reason |
|-----------|--------------------|:--------:|-------------------|
| ret2win | overflow → call a win function | ✅ solved | `stack_overwrite`: found the 40-byte offset, inserted a stack-aligning `ret`, jumped to `ret2win()`; recovered `ROPE{a_placeholder_32byte_flag!}` |
| split | call `system@plt` with a pointer to an in-binary `"/bin/cat flag.txt"` string | ❌ not solved | no `ret2plt`-call-with-string-argument strategy; there is no single win function to jump to, and the argument is a specific pointer, not a sentinel |
| callme | three chained calls, each with three register arguments (RDI/RSI/RDX) | ❌ not solved | PwnProbe builds single calls with up to two arguments only |

**Reading the result.** `ret2win` is squarely in PwnProbe's documented scope
(overflow to a win function) and solves autonomously. `split` and `callme` need
multi-stage ROP that the README/architecture roadmap explicitly lists as not yet
implemented (broader ROP: calling a PLT function with a crafted string pointer,
and ≥3-argument chained calls). So this is a faithful boundary check: the
in-scope challenge is solved, and the two misses are exactly the out-of-scope
techniques — no false claims, consistent with the honesty invariant.

## Reproduce

```sh
mkdir rop && cd rop
for c in ret2win split callme; do
  wget -q https://ropemporium.com/binary/$c.zip && unzip -o -q $c.zip
done
B=/path/to/pwnprobe
( cd ret2win && "$B" solve --timeout 2s ./ret2win )   # solves, prints ROPE{...}
( cd split   && "$B" solve --timeout 2s ./split )     # not solved (ret2plt+string)
( cd callme  && "$B" solve --timeout 2s ./callme )    # not solved (3x3-arg chain)
```

## Candidate next strategy

`split` is the obvious next capability: a non-PIE, no-leak **`ret2plt` call with
a string-pointer argument** (`pop rdi; ret` → address of an in-binary command
string → `system@plt`). It generalizes several simple real challenges and would
turn this miss into a solve.
