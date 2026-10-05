#!/usr/bin/env python3
"""Generate detect-manifest.json from real `pwnprobe detect` observations.

For each lab in manifest.json this builds the source, runs `pwnprobe detect
--json`, and records the detector's actual readings (format-string behaviour,
crash, deadline timeout, and the nx/pie/relro/canary protection statuses) as a
case in the tool's benchmark format. The result pins current detection behaviour
so `pwnprobe benchmark --manifest detect-manifest.json` is a regression check.
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
PWN = sys.argv[sys.argv.index("--pwnprobe") + 1] if "--pwnprobe" in sys.argv else os.environ.get("PWNPROBE")
PROT_KEYS = ["nx", "pie", "relro", "canary"]


def main():
    if not PWN:
        sys.exit("need --pwnprobe PATH or $PWNPROBE")
    labs = json.load(open(os.path.join(HERE, "manifest.json")))
    cases = []
    for lab in labs:
        wd = tempfile.mkdtemp(prefix="pwnprobe-detgen-")
        try:
            target = os.path.join(wd, "vuln")
            cmd = ["gcc", "-O0", "-o", target, os.path.join(HERE, lab["source"])] + list(lab.get("build_flags", []))
            b = subprocess.run(cmd, capture_output=True, text=True)
            if b.returncode != 0:
                print("skip %s: build failed: %s" % (lab["name"], b.stderr.strip()[:120]))
                continue
            shutil.copyfile(os.path.join(HERE, lab["flag_file"]), os.path.join(wd, "flag.txt"))
            p = subprocess.run([PWN, "detect", "--json", "--timeout", "2s", "./vuln"],
                               cwd=wd, capture_output=True, text=True)
            rep = json.loads(p.stdout)
            fmt = any(f.get("kind") == "format_string" for f in rep.get("findings", []))
            crash = any(f.get("kind") == "crash" for f in rep.get("findings", []))
            timed = any(a.get("result", {}).get("outcome") == "timeout" for a in rep.get("attempts", []))
            prot = {}
            binp = (rep.get("binary") or {}).get("protections") or {}
            for k in PROT_KEYS:
                st = (binp.get(k) or {}).get("status")
                if st:
                    prot[k] = st
            cases.append({
                "name": lab["name"],
                "source": lab["source"],
                "flags": lab.get("build_flags", []),
                "protections": prot,
                "format_string": fmt,
                "crash": crash,
                "timeout": timed,
            })
            print("%-28s fmt=%s crash=%s timeout=%s prot=%s" % (lab["name"], fmt, crash, timed, prot))
        finally:
            shutil.rmtree(wd, ignore_errors=True)
    out = os.path.join(HERE, "detect-manifest.json")
    json.dump(cases, open(out, "w"), indent=2)
    print("\nwrote %s (%d cases)" % (out, len(cases)))


if __name__ == "__main__":
    main()
