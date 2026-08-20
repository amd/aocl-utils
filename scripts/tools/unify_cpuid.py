#!/usr/bin/env python3
"""
Generate single-header CPUID API by inlining local includes.
Emits deterministic Au/Config.h (version + AU_WARN_DEPRECATION only, no build-config).
"""

import argparse
import os
import re
import sys
from pathlib import Path
from typing import Set, List

# Repository URL for the provenance banner. Deterministic (no commit SHA, no
# timestamp) so the CI sync-diff stays stable across checkouts.
REPO_URL = "https://github.com/amd/aocl-utils"


def read_version(sdk_include: Path) -> str:
    """Read version.txt from the repo root (parent of parent of SDK/Include)."""
    # SDK/Include -> SDK -> repo root
    repo_root = sdk_include.resolve().parent.parent
    version_file = repo_root / "version.txt"
    try:
        return version_file.read_text(encoding="utf-8").strip()
    except OSError:
        return "unknown"


# Sentinel standing in for the Au/Config.h include. Deliberately NOT a real
# filesystem path: resolve_include() hands this back and inline_file() emits a
# synthetic block on a match, so anything that can equal a real header's
# resolved path would silently replace that header with the Config stub.
CONFIG_SENTINEL = Path("<synthetic:Au/Config.h>")


class Unifier:
    def __init__(self, sdk_include: Path, emit_line_markers: bool = True):
        self.sdk_include = sdk_include.resolve()
        self.repo_root = self.sdk_include.parent.parent
        self.emit_line_markers = emit_line_markers
        self.included_files: Set[Path] = set()
        self.output_lines: List[str] = []
        # Version is sourced from the committed version.txt (NOT from the
        # build-generated Config.h) so the baked value is reproducible in any
        # environment -- this is what keeps the CI regenerate-and-diff stable.
        self.version = read_version(self.sdk_include)

    def resolve_include(self, include_path: str, from_file: Path) -> Path | None:
        """Resolve quoted include: sibling then SDK/Include, returns None if not found."""
        # Special case: Au/Config.h. We do NOT resolve it to the build-generated
        # file -- its content is build-config-specific and would break the CI
        # regenerate-and-diff check. Instead inline_file() emits a deterministic
        # synthetic Config.h block (version + AU_WARN_DEPRECATION). Return the
        # sentinel so inline_file() recognises the marker.
        if include_path == "Au/Config.h":
            return CONFIG_SENTINEL

        # 1. Search relative to the including file's directory (sibling includes)
        sibling = from_file.parent / include_path
        if sibling.exists() and sibling.is_file():
            return sibling.resolve()

        # 2. Search relative to SDK/Include (the -I root)
        candidate = self.sdk_include / include_path
        if candidate.exists() and candidate.is_file():
            return candidate.resolve()

        return None

    def _line_marker(self, file_path: Path, lineno: int):
        """Emit a #line marker pointing at the original source, if enabled."""
        if not self.emit_line_markers:
            return
        try:
            rel = file_path.relative_to(self.repo_root)
        except ValueError:
            rel = file_path
        self.output_lines.append(f'#line {lineno} "{rel}"')

    def _emit_synthetic_config(self):
        """Emit deterministic Au/Config.h (version + AU_WARN_DEPRECATION only)."""
        self.output_lines.append(
            "/* ===== BEGIN SYNTHETIC Au/Config.h (deterministic; version from version.txt) ===== */"
        )
        self.output_lines.append(
            "/* Build-config-specific macros (AU_BUILD_TYPE_*, AU_COMPILER_IS_*, */"
        )
        self.output_lines.append(
            "/* AU_TARGET_OS_IS_*, endian, ...) are intentionally NOT baked here:  */"
        )
        self.output_lines.append(
            "/* they are unused by the header-only chain and would make this file */"
        )
        self.output_lines.append(
            "/* depend on the build config. Platform/compiler identity is derived */"
        )
        self.output_lines.append(
            "/* from the toolchain's predefined macros by au_platform.h below.     */"
        )
        # Version -- reproducible from version.txt in any environment.
        self.output_lines.append("#ifndef AU_PACKAGE_VERSION")
        self.output_lines.append(f'#define AU_PACKAGE_VERSION "{self.version}"')
        self.output_lines.append("#endif")
        # Deprecation markers: keep on by default (matches the shipped library).
        self.output_lines.append("#ifndef AU_WARN_DEPRECATION")
        self.output_lines.append("#define AU_WARN_DEPRECATION")
        self.output_lines.append("#endif")
        self.output_lines.append(
            "/* ===== END SYNTHETIC Au/Config.h ===== */"
        )

    def inline_file(self, file_path: Path, depth: int = 0):
        """Recursively inline file, processing #includes (quoted inlined, angle-bracket kept)."""
        # Compare BEFORE resolve(): the sentinel is not a real path and must not
        # be resolved against the cwd, or it would stop matching.
        is_config = file_path == CONFIG_SENTINEL
        if not is_config:
            file_path = file_path.resolve()

        # Dedup: include each file at most once
        if file_path in self.included_files:
            return
        self.included_files.add(file_path)

        # Special case: Au/Config.h. Emit a DETERMINISTIC synthetic block rather
        # than baking the build-generated file. See the module docstring for why.
        # Only the version (from version.txt) and AU_WARN_DEPRECATION are baked;
        # both are reproducible in any environment. Compiler/OS/endian and
        # build-type identity is supplied at compile time by au_platform.h and is
        # intentionally NOT baked here.
        if is_config:
            self._emit_synthetic_config()
            return

        # Read and process the file line by line. The #line marker for each
        # source points at its FIRST real content line (the first line that is
        # actually emitted below), so diagnostics map back to the true source.
        with open(file_path, "r", encoding="utf-8") as f:
            marked = False
            for lineno, line in enumerate(f, start=1):
                # Match #include "path" (quoted, project-local)
                quoted_match = re.match(r'^\s*#\s*include\s+"([^"]+)"\s*$', line)
                if quoted_match:
                    include_path = quoted_match.group(1)
                    resolved = self.resolve_include(include_path, file_path)
                    if resolved:
                        # Recursively inline; resume this file's markers after.
                        self.inline_file(resolved, depth + 1)
                        marked = False
                        continue
                    else:
                        # Couldn't resolve -- keep the #include line as a fallback
                        # (shouldn't happen if the tree is consistent)
                        if not marked:
                            self._line_marker(file_path, lineno)
                            marked = True
                        self.output_lines.append(f"/* WARNING: could not resolve: {line.strip()} */")
                        self.output_lines.append(line.rstrip())
                        continue

                # Match #include <...> (system/angle-bracket) -- keep as-is
                angle_match = re.match(r'^\s*#\s*include\s*<[^>]+>\s*$', line)
                if angle_match:
                    if not marked:
                        self._line_marker(file_path, lineno)
                        marked = True
                    self.output_lines.append(line.rstrip())
                    continue

                # Regular line: emit
                if not marked:
                    self._line_marker(file_path, lineno)
                    marked = True
                self.output_lines.append(line.rstrip())

    def generate_unified(self, entry_file: Path, output_file: Path):
        """Generate the unified header."""
        entry_file = entry_file.resolve()
        version = self.version
        # Banner -- fully deterministic (version + repo URL, no SHA/timestamp).
        self.output_lines.append("/*")
        self.output_lines.append(" * GENERATED FILE - DO NOT EDIT")
        self.output_lines.append(" * ")
        self.output_lines.append(" * This is the standalone single-file version of the AOCL-Utils CPUID")
        self.output_lines.append(" * header-only API. It is generated by scripts/tools/unify_cpuid.py")
        self.output_lines.append(" * from the multi-header sources:")
        self.output_lines.append(" * ")
        try:
            entry_rel = entry_file.relative_to(self.repo_root)
        except ValueError:
            entry_rel = entry_file.name
        self.output_lines.append(f" *   Entry: {entry_rel}")
        self.output_lines.append(" *   Au/Config.h replaced by a deterministic synthetic block")
        self.output_lines.append(" *     (version + AU_WARN_DEPRECATION only; see the generator).")
        self.output_lines.append(f" *   Version: {version}")
        self.output_lines.append(f" *   Repository: {REPO_URL}")
        self.output_lines.append(" * ")
        self.output_lines.append(" * Source of truth: the individual headers in SDK/Include/.")
        self.output_lines.append(" * To regenerate: run the unification script (see above).")
        self.output_lines.append(" */")
        self.output_lines.append("")
        self.output_lines.append("#ifndef __AU_CPUID_HEADER_ONLY_H__")
        self.output_lines.append("#define __AU_CPUID_HEADER_ONLY_H__")
        self.output_lines.append("")

        # Inline the entry file (which recursively pulls everything)
        self.inline_file(entry_file)

        # Close the header guard
        self.output_lines.append("")
        self.output_lines.append("#endif /* __AU_CPUID_HEADER_ONLY_H__ */")

        # Write output
        output_file.parent.mkdir(parents=True, exist_ok=True)
        with open(output_file, "w", encoding="utf-8") as f:
            for line in self.output_lines:
                f.write(line + "\n")

        print(f"Unified header written to: {output_file}")
        print(f"  Total lines: {len(self.output_lines)}")
        print(f"  Files inlined: {len(self.included_files)}")
        for path in sorted(self.included_files):
            try:
                rel = path.relative_to(self.repo_root)
            except ValueError:
                rel = path
            print(f"    - {rel}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate the standalone single-file CPUID header from the multi-header sources."
    )
    parser.add_argument(
        "--entry",
        type=Path,
        required=True,
        help="Entry header file (e.g., SDK/Include/Capi/au/cpuid/cpuid_inline.h)",
    )
    parser.add_argument(
        "--sdk-include",
        type=Path,
        required=True,
        help="SDK include directory (e.g., SDK/Include)",
    )
    parser.add_argument(
        "--output",
        type=Path,
        required=True,
        help="Output standalone header path (e.g., SDK/Include/Capi/au/cpuid/au_cpuid_header_only.h)",
    )
    parser.add_argument(
        "--no-line-markers",
        action="store_true",
        help="Do not emit #line markers (they are emitted by default).",
    )

    args = parser.parse_args()

    # Validate inputs
    if not args.entry.exists():
        print(f"Error: entry file does not exist: {args.entry}", file=sys.stderr)
        sys.exit(1)
    if not args.sdk_include.is_dir():
        print(f"Error: SDK include dir does not exist: {args.sdk_include}", file=sys.stderr)
        sys.exit(1)
    unifier = Unifier(
        args.sdk_include,
        emit_line_markers=not args.no_line_markers,
    )
    unifier.generate_unified(args.entry, args.output)


if __name__ == "__main__":
    main()
