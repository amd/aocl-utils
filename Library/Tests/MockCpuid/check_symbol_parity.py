#!/usr/bin/env python3
#
# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
"""Fail if libaoclutils and the mock shim export different CPUID APIs.

A consumer swaps one library for the other, so the shim must export every
au_cpuid_* / alci_* entry point the real library does. When it does not, the
consumer fails to link -- late, confusingly, and in someone else's build. This
turns that into a build-time failure here.

Missing symbols are always an error. Extra ones are too: an entry point the shim
exports and the real library does not means the two have diverged, and the
divergence should be explained rather than accumulate.
"""

import argparse
import re
import subprocess
import sys

# The surface a consumer can link against. Internal static-inline helpers from
# the header-only core never reach .dynsym, so this needs no exclusion list.
API_RE = re.compile(r"^(au_cpuid_|alci_)")


def exported(lib, nm):
    """Public defined function symbols, via nm -D --defined-only."""
    try:
        out = subprocess.run(
            [nm, "-D", "--defined-only", str(lib)],
            capture_output=True, text=True, check=True).stdout
    except FileNotFoundError:
        sys.exit(f"error: '{nm}' not found; cannot check symbol parity")
    except subprocess.CalledProcessError as e:
        sys.exit(f"error: {nm} failed on {lib}: {e.stderr.strip()}")

    syms = set()
    for line in out.splitlines():
        parts = line.split()
        # "<addr> <type> <name>", or "<type> <name>" for undefined-address rows.
        if len(parts) >= 3 and parts[1] in ("T", "W", "i"):
            name = parts[2]
        elif len(parts) == 2 and parts[0] in ("T", "W", "i"):
            name = parts[1]
        else:
            continue
        # Strip any version suffix: libfoo.so exports "sym@@VER".
        name = name.split("@")[0]
        if API_RE.match(name):
            syms.add(name)
    return syms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--real", required=True, help="libaoclutils shared object")
    ap.add_argument("--shim", required=True, help="shim shared object")
    ap.add_argument("--nm", default="nm", help="nm binary to use")
    args = ap.parse_args()

    real = exported(args.real, args.nm)
    shim = exported(args.shim, args.nm)

    if not real:
        sys.exit(f"error: no au_cpuid_*/alci_* symbols found in {args.real}; "
                 f"the check would pass vacuously, so treat it as a failure")

    missing = sorted(real - shim)
    extra = sorted(shim - real)

    print(f"libaoclutils exports {len(real)} CPUID C entry points; "
          f"shim exports {len(shim)}.")

    if missing:
        print(f"\nMISSING from the shim ({len(missing)}):")
        for s in missing:
            print(f"  {s}")
        print("\nA consumer linking the shim will fail to link. The shim "
              "re-includes cpuid.h/cpuid_legacy.h, so an entry point declared "
              "there is emitted automatically -- a gap here means the new API "
              "lives somewhere those headers do not reach.")
    if extra:
        print(f"\nEXTRA in the shim ({len(extra)}):")
        for s in extra:
            print(f"  {s}")

    if missing or extra:
        return 1
    print("Symbol parity OK: the two libraries export the same CPUID surface.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
