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
"""Generate mockcpuid_profiles.h from the simnowdata FlagsT.txt corpus.

Each model directory under Library/Tests/Cpuid/Mock/simnowdata/ ships a
FlagsT.txt listing the features that model has. Those files are already the
golden data for the CPUID mock tests, so the named profiles the shim offers are
derived from them rather than hand-curated -- a new model directory becomes an
available profile with no further edit.

Flag names are resolved against AU_CPUID_FLAG_LIST in cpuid_flags.h, which is
the single source of truth for the enum values. An unrecognised name is a hard
error: silently dropping it would produce a profile that masks less than the
user asked for, which is exactly the quiet-wrong-answer failure this feature is
meant to eliminate.
"""

import argparse
import re
import sys
from pathlib import Path

# Short aliases for the names people actually ask for ("mask down to Milan").
# Anything not listed is still reachable by its full directory name.
ALIASES = {
    "milan": "EPYC-Milan-v1",
    "rome": "EPYC-Rome-v1",
    "genoa": "EPYC-Genoa-v1",
    "turin": "EPYC-Turin-v1",
    "turin-dense": "EPYC-Turin-Dense-v1",
    "bergamo": "Bergamo-v1",
    "naples": "EPYC-v1",
    "siena": "Siena-v1",
    "venice": "Venice-v1",
}


def read_flag_order(flags_header: Path) -> list:
    """Return flag names in AU_CPUID_FLAG_LIST order (enum value = index + 1)."""
    text = flags_header.read_text()
    m = re.search(r"#define AU_CPUID_FLAG_LIST\(X\)(.*?)\n\n", text, re.S)
    if not m:
        sys.exit(f"error: AU_CPUID_FLAG_LIST not found in {flags_header}")
    names = re.findall(r"X\((\w+)\)", m.group(1))
    if not names:
        sys.exit(f"error: AU_CPUID_FLAG_LIST in {flags_header} is empty")
    return names


def read_identity(model_dir: Path) -> tuple:
    """Return (leaf1_eax, vendor_ebx, vendor_ecx, vendor_edx) from the raw dump.

    Leaf 1 EAX carries family/model/stepping and leaf 0 EBX/ECX/EDX the vendor
    string, so a profile can present the CPU's identity and not merely its
    features. The raw dump is the same file the CPUID mock replays. A model
    without a dump gets zeros, which the shim reads as "leave identity alone".
    """
    dump = model_dir / model_dir.name
    if not dump.exists():
        return (0, 0, 0, 0)

    leaf1, vendor = None, None
    for line in dump.read_text().splitlines():
        m = re.match(
            r"\{([^,]+),([^,]+),[^,]+,[^}]+\}:\{([^,]+),([^,]+),([^,]+),([^}]+)\}",
            line.strip(),
        )
        if not m:
            continue
        leaf, sub = int(m.group(1), 16), int(m.group(2), 16)
        regs = [int(m.group(i), 16) for i in (3, 4, 5, 6)]
        if leaf == 0x1 and sub == 0x0 and leaf1 is None:
            leaf1 = regs[0]
        elif leaf == 0x0 and sub == 0x0 and vendor is None:
            vendor = regs[1:]
    if leaf1 is None or vendor is None:
        return (0, 0, 0, 0)
    return (leaf1, vendor[0], vendor[1], vendor[2])


def collect_profiles(simnow_dir: Path, known: dict) -> list:
    """Build (name, flag indices, identity) for every model with FlagsT.txt."""
    profiles = []
    for model_dir in sorted(p for p in simnow_dir.iterdir() if p.is_dir()):
        flags_file = model_dir / "FlagsT.txt"
        if not flags_file.exists():
            continue
        indices = []
        for lineno, line in enumerate(flags_file.read_text().splitlines(), 1):
            name = line.strip()
            if not name:
                continue
            if name not in known:
                sys.exit(
                    f"error: {flags_file}:{lineno}: '{name}' is not in "
                    f"AU_CPUID_FLAG_LIST. Add it to cpuid_flags.h or fix the "
                    f"data file; a profile must not silently drop a feature."
                )
            indices.append(known[name])
        profiles.append(
            (model_dir.name, sorted(set(indices)), read_identity(model_dir))
        )
    if not profiles:
        sys.exit(f"error: no FlagsT.txt found under {simnow_dir}")
    return profiles


def bitmap_words(indices: list, nwords: int) -> list:
    words = [0] * nwords
    for i in indices:
        words[i // 64] |= 1 << (i % 64)
    return words


def emit(out: Path, flag_names: list, profiles: list, nwords: int) -> None:
    lines = []
    add = lines.append
    add("/*")
    add(" * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.")
    add(" *")
    add(" * SPDX-License-Identifier: BSD-3-Clause")
    add(" */")
    add("")
    add("/* GENERATED FILE -- DO NOT EDIT.")
    add(" * Produced by scripts/tools/gen_mock_profiles.py from")
    add(" * Library/Tests/Cpuid/Mock/simnowdata/<Model>/FlagsT.txt.")
    add(" * Edit the data or the generator, never this file. */")
    add("")
    add("#ifndef __AU_MOCKCPUID_PROFILES_H__")
    add("#define __AU_MOCKCPUID_PROFILES_H__")
    add("")
    add('#include "Capi/au/cpuid/cpuid_core.h"')
    add("")
    add("#include <stdio.h>")
    add("#include <string.h>")
    add("")
    add("typedef struct au_mock_profile")
    add("{")
    add("    const char*   name;")
    add("    au_flag_set_t flags;")
    add("    /* Leaf 1 EAX (family/model/stepping) and leaf 0 EBX/ECX/EDX")
    add("     * (vendor string) from the model's raw dump. All zero when the")
    add("     * model ships no dump, meaning 'leave identity alone'. */")
    add("    uint32_t      leaf1_eax;")
    add("    uint32_t      vendor_ebx;")
    add("    uint32_t      vendor_ecx;")
    add("    uint32_t      vendor_edx;")
    add("} au_mock_profile_t;")
    add("")
    add("/* Local ASCII case-insensitive compare: strcasecmp is POSIX and")
    add(" * _stricmp is MSVC, and this header must build as plain C on both. */")
    add("static inline int")
    add("au_mock_strcasecmp(const char* a, const char* b)")
    add("{")
    add("    while (*a != '\\0' && *b != '\\0') {")
    add("        int ca = (unsigned char)*a;")
    add("        int cb = (unsigned char)*b;")
    add("")
    add("        if (ca >= 'A' && ca <= 'Z')")
    add("            ca += 'a' - 'A';")
    add("        if (cb >= 'A' && cb <= 'Z')")
    add("            cb += 'a' - 'A';")
    add("        if (ca != cb)")
    add("            return ca - cb;")
    add("        a++;")
    add("        b++;")
    add("    }")
    add("    return (unsigned char)*a - (unsigned char)*b;")
    add("}")
    add("")

    # Flag-index -> name, for refusal diagnostics.
    add("/* Flag index to name, for diagnostics. Index 0 is AU_FLAG_MIN. */")
    add("static const char* const au_mock_flag_names[] = {")
    add('    "<none>",')
    for name in flag_names:
        add(f'    "{name}",')
    add("};")
    add("")
    add("static inline const char*")
    add("au_mock_flag_name(unsigned flag)")
    add("{")
    add("    if (flag == 0")
    add("        || flag >= sizeof(au_mock_flag_names)")
    add("                       / sizeof(au_mock_flag_names[0]))")
    add('        return "<unknown>";')
    add("    return au_mock_flag_names[flag];")
    add("}")
    add("")

    add(f"/* {len(profiles)} profiles derived from the simnowdata corpus. */")
    add("static const au_mock_profile_t au_mock_profiles[] = {")
    for name, indices, ident in profiles:
        words = bitmap_words(indices, nwords)
        packed = ", ".join(f"0x{w:016x}ULL" for w in words)
        idt = ", ".join(f"0x{v:08x}u" for v in ident)
        add(f'    {{ "{name}", {{ {{ {packed} }} }}, {idt} }},')
    add("};")
    add("")

    add("/* Short aliases for the common asks. */")
    add("static const struct")
    add("{")
    add("    const char* alias;")
    add("    const char* target;")
    add("} au_mock_profile_aliases[] = {")
    known_names = {p[0] for p in profiles}
    for alias, target in sorted(ALIASES.items()):
        if target in known_names:
            add(f'    {{ "{alias}", "{target}" }},')
    add("};")
    add("")

    add("/* Case-insensitive lookup, aliases first. NULL when unknown. */")
    add("static inline const au_mock_profile_t*")
    add("au_mock_profile_find(const char* name)")
    add("{")
    add("    size_t i;")
    add("")
    add("    if (name == NULL)")
    add("        return NULL;")
    add("")
    add("    for (i = 0; i < sizeof(au_mock_profile_aliases)")
    add("                        / sizeof(au_mock_profile_aliases[0]);")
    add("         i++) {")
    add("        if (au_mock_strcasecmp(au_mock_profile_aliases[i].alias, name)")
    add("            == 0) {")
    add("            name = au_mock_profile_aliases[i].target;")
    add("            break;")
    add("        }")
    add("    }")
    add("")
    add("    for (i = 0; i < sizeof(au_mock_profiles) / sizeof(au_mock_profiles[0]);")
    add("         i++) {")
    add("        if (au_mock_strcasecmp(au_mock_profiles[i].name, name) == 0)")
    add("            return &au_mock_profiles[i];")
    add("    }")
    add("    return NULL;")
    add("}")
    add("")
    add("static inline void")
    add("au_mock_profile_list(FILE* fp)")
    add("{")
    add("    size_t i;")
    add("")
    add('    fprintf(fp, "AU_CPUID_MOCK_PROFILE: %zu profiles available.\\n",')
    add("            sizeof(au_mock_profiles) / sizeof(au_mock_profiles[0]));")
    add('    fprintf(fp, "  aliases:");')
    add("    for (i = 0; i < sizeof(au_mock_profile_aliases)")
    add("                        / sizeof(au_mock_profile_aliases[0]);")
    add("         i++)")
    add('        fprintf(fp, " %s", au_mock_profile_aliases[i].alias);')
    add('    fprintf(fp, "\\n");')
    add("    for (i = 0; i < sizeof(au_mock_profiles) / sizeof(au_mock_profiles[0]);")
    add("         i++)")
    add('        fprintf(fp, "  %s\\n", au_mock_profiles[i].name);')
    add("}")
    add("")
    add("#endif /* __AU_MOCKCPUID_PROFILES_H__ */")
    add("")

    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(lines))


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--flags-header", type=Path, required=True,
                    help="Path to SDK/Include/Capi/au/cpuid/cpuid_flags.h")
    ap.add_argument("--simnow-dir", type=Path, required=True,
                    help="Path to Library/Tests/Cpuid/Mock/simnowdata")
    ap.add_argument("--output", type=Path, required=True,
                    help="Generated header path")
    args = ap.parse_args()

    if not args.flags_header.exists():
        sys.exit(f"error: no such file: {args.flags_header}")
    if not args.simnow_dir.is_dir():
        sys.exit(f"error: no such directory: {args.simnow_dir}")

    flag_names = read_flag_order(args.flags_header)
    # Enum framing is MIN=0 then flags 1..K, so value = position + 1.
    known = {name: i + 1 for i, name in enumerate(flag_names)}
    # AU_CPUID_BITMAP_WORDS = (AU_FLAG_MAX / 64) + 1, with MAX = len + 1.
    nwords = ((len(flag_names) + 1) // 64) + 1

    profiles = collect_profiles(args.simnow_dir, known)
    emit(args.output, flag_names, profiles, nwords)
    print(f"gen_mock_profiles: {len(profiles)} profiles -> {args.output}")


if __name__ == "__main__":
    main()
