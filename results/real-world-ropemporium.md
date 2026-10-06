# Real-world check — ROP Emporium (x86_64)

Besides the synthetic `labs/`, PwnProbe was run against real, third-party
teaching-CTF binaries it did not author: the [ROP Emporium](https://ropemporium.com)
x86_64 challenges. These binaries read `./flag.txt` and print it on a successful
exploit, so an end-to-end solve is verifiable locally. They are **not** committed
here (respecting their distribution); fetch them from ropemporium.com.

Run on Ubuntu 24.04 / glibc 2.39.

| Challenge | Technique it needs | PwnProbe | Strategy / reason |
|-----------|--------------------|:--------:|-------------------|
| ret2win | overflow → call a win function | ✅ solved | `stack_overwrite`: found the 40-byte offset, inserted a stack-aligning `ret`, jumped to `ret2win()`; recovered `ROPE{a_placeholder_32byte_flag!}` |
| split | call `system@plt` with a pointer to an in-binary `"/bin/cat flag.txt"` string | ✅ solved | `ret2plt` (since `c70b9bc`): `pop rdi; ret` → string address → stack-aligning `ret` → `system@plt`; recovered `ROPE{a_placeholder_32byte_flag!}` |
| callme | three chained calls, each with three register arguments (RDI/RSI/RDX) | ✅ solved | `rop_planner`: chains three 3-argument calls to callme_one/two/three with the magic constants harvested from `libcallme.so` |

**History.** `ret2win` has always been in scope (overflow to a win function).
`split` was a miss until the `ret2plt` strategy was added in PwnProbe `c70b9bc`
after this real-world check surfaced the gap — a non-PIE, no-leak call of
`system` with an in-binary string pointer. `callme` was then the next gap, and
the ROP chain planner (`internal/rop`) was built to close it: it synthesizes the
three chained three-argument calls. The remaining ladder (`write4`, `badchars`,
`fluff`, `ret2csu`, `pivot`) needs write-what-where, the ret2csu universal
gadget, and stack pivots / leaked-libc gadgets — the planner's later phases.

The `ret2plt` capability is also covered by the committable synthetic lab
[`lab14_ret2plt_string`](../labs/lab14_ret2plt_string/vuln.c), which mirrors
`split` (and ships an explicit `pop rdi; ret` gadget, since glibc >= 2.34 dropped
`__libc_csu_init`).

## Reproduce

```sh
mkdir rop && cd rop
for c in ret2win split callme; do
  wget -q https://ropemporium.com/binary/$c.zip && unzip -o -q $c.zip
done
B=/path/to/pwnprobe
( cd ret2win && "$B" solve --timeout 2s ./ret2win )   # solves (stack_overwrite)
( cd split   && "$B" solve --timeout 2s ./split )     # solves (ret2plt)
( cd callme  && "$B" solve --timeout 2s ./callme )    # solves (rop_planner)
```
