#!/usr/bin/env python3
# Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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

"""
Pre-commit hook: reject staged files that contain internal Jira ticket IDs.

Patterns blocked:
  CPUPL-<digits>   (CPU Performance Library project)
  SWLCSG-<digits>  (Software CSG project)

These are AMD-internal identifiers and must not appear in source code,
comments, or documentation committed to the public repository.
"""

import os
import re
import subprocess
import sys

# Patterns that identify internal Jira tickets.
# Negative lookbehind/lookahead for alphanumerics only (no underscore) so that
# underscore-adjacent IDs are caught while run-on identifiers are not.
# Boundary semantics (KEY = project key, N = digits):
#   underscore_KEY-N       -> match  (underscore is not alphanumeric)
#   KEY-N_underscore       -> match  (underscore is not alphanumeric)
#   aKEY-N / KEY-Nx        -> no match (alphanumeric immediately adjacent)
_TICKET_RE = re.compile(r'(?<![A-Za-z0-9])(CPUPL|SWLCSG)-\d+(?![A-Za-z0-9])')


def staged_files():
    """
    Retrieve the list of staged files from git.

    Returns:
        list: A list of file paths for files currently staged in git.
    """
    result = subprocess.run(
        ["git", "diff", "--cached", "--name-only", "-z"],
        capture_output=True,
    )
    if result.returncode != 0:
        return []
    return [f for f in result.stdout.decode("utf-8", errors="replace").split("\0") if f]


def scan(path):
    """Return list of line numbers where a ticket ID is found, or None on read error."""
    hits = []
    try:
        with open(path, 'r', encoding='utf-8', errors='replace') as fh:
            for lineno, line in enumerate(fh, start=1):
                if _TICKET_RE.search(line):
                    hits.append(lineno)
    except OSError:
        return None
    return hits


def main():
    """
    Scan staged files for internal Jira ticket IDs and block commit if found.

    Files to scan are taken from command-line arguments (as pre-commit passes
    them), falling back to ``git diff --cached --name-only`` when invoked
    directly without arguments.  Exits with status 1 if any internal ticket
    ID matching CPUPL-* or SWLCSG-* is detected, or if a staged file cannot
    be read (fail-safe: block rather than silently skip).
    """
    files = sys.argv[1:] if len(sys.argv) > 1 else staged_files()
    found = False

    for path in files:
        if not os.path.isfile(path):
            continue
        hits = scan(path)
        if hits is None:
            found = True
            print(f"\n  {path}: could not be read — blocking commit (fail-safe)")
        elif hits:
            found = True
            lines = ", ".join(str(n) for n in hits)
            print(f"\n  {path}: line(s) {lines}")

    if found:
        print(
            "\nCommit blocked: internal Jira ticket IDs (CPUPL-*, SWLCSG-*) "
            "found in staged files.\n"
            "Remove or replace these references before committing."
        )
        sys.exit(1)


if __name__ == "__main__":
    main()
