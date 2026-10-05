# Third-party challenge binaries — ROP Emporium

These are **prebuilt x86_64 challenge binaries from [ROP Emporium](https://ropemporium.com)**,
not authored by this project. They are included here, at the repository owner's
request, so PwnProbe's real-world results can be reproduced directly against the
original binaries rather than only against the synthetic `labs/`.

- Source: https://ropemporium.com (per-challenge downloads `ret2win.zip`,
  `split.zip`, `callme.zip`).
- `ret2win/` and `split/` include the challenge's own `flag.txt` (the ROP
  Emporium placeholder `ROPE{a_placeholder_32byte_flag!}`), which the binary
  prints on a successful exploit.
- `callme/` includes its original `libcallme.so`, key files, and
  `encrypted_flag.dat`; it has no plaintext `flag.txt`.

All rights to these binaries remain with ROP Emporium / its author. They are
redistributed here only as a convenience for reproducing the evaluation; if
their inclusion is not desired, delete this directory — nothing else in the
repository depends on it (the synthetic `labs/` fully cover every strategy).

Observed results are recorded in [`../../results/real-world-ropemporium.md`](../../results/real-world-ropemporium.md).
