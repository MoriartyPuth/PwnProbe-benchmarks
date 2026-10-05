#!/usr/bin/env python3
"""PwnProbe solve-benchmark harness.

For each lab in manifest.json this builds the lab's vuln.c with its recorded
build flags, stages its flag.txt beside the binary, runs `pwnprobe solve --json`
in that directory, and compares the result against the lab's expected flag and
strategy. A lab PASSES when the solver reports solved and recovers exactly the
expected flag; a different winning strategy is reported as a warning, not a
failure (the honest signal is whether the real flag came out).

Usage:
    ./run.py --pwnprobe /path/to/pwnprobe [options]

Options:
    --pwnprobe PATH   path to the pwnprobe binary (or set $PWNPROBE)
    --manifest PATH   manifest file (default: manifest.json beside this script)
    --timeout DUR     per-run solver deadline passed through, e.g. 2s (default 2s)
    --wall SECONDS    per-lab wall-clock limit for the whole solve (default 240)
    --only NAME       run just the named lab (repeatable)
    --out PATH        also write a JSON results document to PATH

Requires Linux with gcc. Set $PWNPROBE_PYTHON to a Python that has angr for any
lab whose strategy is angr_logic (none of the base labs need it).
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))


def build(lab, workdir):
    """Compile the lab's source into workdir/vuln. Returns (ok, message)."""
    src = os.path.join(HERE, lab["source"])
    out = os.path.join(workdir, "vuln")
    cmd = ["gcc", "-O0", "-o", out, src] + list(lab.get("build_flags", []))
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode != 0:
        return False, "build failed: " + p.stderr.strip()
    flag_file = lab.get("flag_file")
    if flag_file:
        shutil.copyfile(os.path.join(HERE, flag_file), os.path.join(workdir, "flag.txt"))
    return True, out


def solve(pwnprobe, target, workdir, timeout, wall, pattern):
    """Run `pwnprobe solve --json` on target in workdir. Returns the parsed
    report dict, or {} on a harness-level failure, plus a note string."""
    cmd = [pwnprobe, "solve", "--json", "--timeout", timeout]
    if pattern:
        cmd += ["--flag-pattern", pattern]
    cmd.append("./vuln")
    try:
        p = subprocess.run(cmd, cwd=workdir, capture_output=True, text=True, timeout=wall)
    except subprocess.TimeoutExpired:
        return {}, "wall-clock timeout after %ds" % wall
    # solve exits nonzero when unsolved; the JSON report is still on stdout.
    try:
        return json.loads(p.stdout), ""
    except json.JSONDecodeError:
        tail = (p.stderr or p.stdout).strip()[-300:]
        return {}, "no JSON report (exit %d): %s" % (p.returncode, tail)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pwnprobe", default=os.environ.get("PWNPROBE"))
    ap.add_argument("--manifest", default=os.path.join(HERE, "manifest.json"))
    ap.add_argument("--timeout", default="2s")
    ap.add_argument("--wall", type=int, default=300)
    ap.add_argument("--flag-pattern", default=r"(?i)flag\{[^}]+\}",
                    help="flag regexp passed to the solver; a lab may override via its manifest flag_pattern")
    ap.add_argument("--only", action="append", default=[])
    ap.add_argument("--out")
    args = ap.parse_args()

    if not args.pwnprobe:
        sys.exit("error: --pwnprobe PATH (or $PWNPROBE) is required")
    if not shutil.which("gcc"):
        sys.exit("error: gcc not found; this harness requires Linux with gcc")

    with open(args.manifest) as f:
        labs = json.load(f)
    if args.only:
        labs = [l for l in labs if l["name"] in args.only]

    results = []
    npass = 0
    for lab in labs:
        name = lab["name"]
        workdir = tempfile.mkdtemp(prefix="pwnprobe-bench-")
        entry = {"name": name, "expected_strategy": lab.get("expected_strategy"),
                 "expected_flag": lab.get("expected_flag")}
        try:
            ok, msg = build(lab, workdir)
            if not ok:
                entry.update(passed=False, error=msg)
                results.append(entry)
                print("FAIL  %-28s %s" % (name, msg))
                continue
            t0 = time.time()
            pattern = lab.get("flag_pattern", args.flag_pattern)
            report, note = solve(args.pwnprobe, msg, workdir, args.timeout, args.wall, pattern)
            dt = time.time() - t0
            solved = bool(report.get("solved"))
            flags = report.get("flags") or []
            strat = (report.get("winning_attempt") or {}).get("strategy")
            got_flag = lab["expected_flag"] in flags
            passed = solved and got_flag
            entry.update(passed=passed, solved=solved, got_strategy=strat,
                         flags=flags, duration_s=round(dt, 2))
            if note:
                entry["note"] = note
            results.append(entry)
            warn = ""
            if passed and strat != lab.get("expected_strategy"):
                warn = "  (strategy %s, expected %s)" % (strat, lab.get("expected_strategy"))
            status = "PASS" if passed else "FAIL"
            if passed:
                npass += 1
            detail = note if not passed and note else ("flag=%s strategy=%s" % (flags, strat) if passed else "solved=%s flags=%s" % (solved, flags))
            print("%-5s %-28s %6.1fs  %s%s" % (status, name, dt, detail, warn))
        finally:
            shutil.rmtree(workdir, ignore_errors=True)

    total = len(results)
    print("\n%d/%d labs solved with the expected flag." % (npass, total))

    if args.out:
        doc = {"passed": npass, "total": total, "timeout": args.timeout, "cases": results}
        with open(args.out, "w") as f:
            json.dump(doc, f, indent=2)
        print("wrote %s" % args.out)

    sys.exit(0 if npass == total else 1)


if __name__ == "__main__":
    main()
