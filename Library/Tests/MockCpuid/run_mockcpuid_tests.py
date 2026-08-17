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
#    without specific prior written permission.
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
"""Behavioural tests for the Phase-1 Mock-CPUID profile facility.

The C and C++ consumers are each built twice from one source: against the
real library and against a mock library. The checks cover the subset contract,
profile identity, mutable apply/reset control, the reserved CPU selector,
strict mode, direct linking, LD_PRELOAD, and a dispatched kernel that actually
runs after a profile lowers the reported feature set.

--self-test deliberately feeds broken data to the checker and requires each
checker to reject it. This keeps a permanently-green assertion from becoming
an untested assertion.
"""

import argparse
import os
import platform
import subprocess
import sys

LD_PRELOAD_SUPPORTED = platform.system() == "Linux"


class Failure(Exception):
    """One behavioural check failed."""


def run(binary, env_extra=None, expect_ok=True, preload=None):
    """Run a consumer, returning (return code, stdout, stderr)."""
    env = dict(os.environ)
    for name in ("AU_CPUID_MOCK_PROFILE", "AU_CPUID_MOCK_STRICT",
                 "AU_CPUID_MOCK"):
        env.pop(name, None)
    env.pop("LD_PRELOAD", None)
    if preload:
        env["LD_PRELOAD"] = preload
    if env_extra:
        env.update(env_extra)

    process = subprocess.run([binary], env=env, capture_output=True,
                             text=True)
    if expect_ok and process.returncode != 0:
        raise Failure(
            f"{os.path.basename(binary)} exited {process.returncode} with "
            f"env {env_extra}\nstdout: {process.stdout}\n"
            f"stderr: {process.stderr}"
        )
    return process.returncode, process.stdout, process.stderr


def parse_flags(text):
    """Parse the C safety consumer into (flag map, metadata map)."""
    flags, metadata = {}, {}
    for line in text.splitlines():
        line = line.strip()
        if not line:
            continue
        if line.startswith("#"):
            key, _, value = line[1:].partition(" ")
            metadata[key] = value.strip()
            continue
        name, separator, value = line.partition("=")
        if not separator:
            raise Failure(f"malformed flag output line: {line!r}")
        flags[name] = value == "1"
    if not flags:
        raise Failure("no flags parsed from C consumer output")
    return flags, metadata


def parse_cpp(text):
    """Parse the one-line C++ consumer output."""
    line = text.strip()
    values = {}
    for item in line.split():
        name, separator, value = item.partition("=")
        if not separator:
            raise Failure(f"malformed C++ output item: {item!r}")
        values[name] = int(value)
    required = {"amd", "intel", "avx2", "avx512f", "uarch", "v3"}
    if not required.issubset(values):
        raise Failure(f"C++ output lacks fields: {sorted(required - set(values))}")
    return values


def parse_api(text):
    """Parse the API consumer's labelled state lines."""
    states = {}
    statuses = {}
    for line in text.splitlines():
        words = line.split()
        if not words:
            continue
        if words[0] in ("host", "rome", "reset"):
            values = {}
            for item in words[1:]:
                name, separator, value = item.partition("=")
                if not separator:
                    raise Failure(f"malformed API state item: {item!r}")
                values[name] = int(value)
            states[words[0]] = values
        else:
            for item in words:
                name, separator, value = item.partition("=")
                if not separator:
                    raise Failure(f"malformed API status item: {item!r}")
                statuses[name] = int(value)
    if not {"host", "rome", "reset"}.issubset(states):
        raise Failure(f"API output lacks states: {text!r}")
    return states, statuses


def parse_identity(metadata):
    """Return vendor, family, model, stepping, uarch from C metadata."""
    try:
        values = [int(part) for part in metadata["identity"].split()]
    except (KeyError, ValueError) as error:
        raise Failure(f"bad identity metadata: {metadata!r}") from error
    if len(values) != 5:
        raise Failure(f"bad identity field count: {values!r}")
    return values


def check_subset(host, reported, label):
    """The safety property: every reported flag must exist on the host."""
    extra = sorted(name for name, value in reported.items()
                   if value and not host.get(name, False))
    if extra:
        raise Failure(
            f"[{label}] reported {len(extra)} host-absent feature(s): "
            f"{', '.join(extra[:10])}"
        )


def check_equal(left, right, label):
    if left != right:
        raise Failure(f"[{label}] values differ: {left!r} != {right!r}")


def check_programmatic_abort(returncode, stderr):
    """Check that an impossible programmatic request is fatal when strict."""
    if returncode == 0:
        raise Failure("programmatic strict request returned success")
    if "strict" not in stderr.lower() or "aborting" not in stderr.lower():
        raise Failure(f"programmatic strict failure was not explained: {stderr!r}")


def check_programmatic_continue(returncode, stdout, stderr):
    """Check the strict=0 programmatic warning-and-safe-subset contract."""
    if returncode != 0:
        raise Failure(f"programmatic strict=0 request exited {returncode}")
    if "continued profile=" not in stdout or "status=" not in stdout:
        raise Failure(f"programmatic continuation was not reported: {stdout!r}")
    if "refus" not in stderr.lower() and "keeps" not in stderr.lower():
        raise Failure(f"programmatic refusal was not warned about: {stderr!r}")


def self_test():
    """Prove the important checkers reject data that violates their claims."""
    checks = []

    def expect_failure(name, function):
        try:
            function()
        except Failure:
            checks.append((name, True))
            return
        checks.append((name, False))

    host = {"avx2": True, "avx512f": False, "sse3": True}
    expect_failure(
        "subset catches invented feature",
        lambda: check_subset(host, {"avx2": True, "avx512f": True}, "self"),
    )
    expect_failure(
        "subset catches many invented features",
        lambda: check_subset(
            host, {name: True for name in list(host) + ["avx512dq"]}, "self"
        ),
    )
    expect_failure("flag parser rejects empty output", lambda: parse_flags(""))
    expect_failure(
        "C++ parser rejects incomplete output",
        lambda: parse_cpp("avx2=1"),
    )
    expect_failure(
        "programmatic strict checker catches success",
        lambda: check_programmatic_abort(0, ""),
    )
    expect_failure(
        "programmatic continuation checker catches abort",
        lambda: check_programmatic_continue(-6, "", ""),
    )

    try:
        check_subset(host, {"avx2": False, "sse3": True}, "self")
        checks.append(("subset accepts a true subset", True))
    except Failure:
        checks.append(("subset accepts a true subset", False))

    success = True
    for name, passed in checks:
        print(f"  {'PASS' if passed else 'FAIL'}  {name}")
        success = success and passed
    print()
    print("Self-test: assertions demonstrably fail on bad data."
          if success else
          "Self-test FAILED: a checker did not reject bad data.")
    return 0 if success else 1


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--safety-real", required=True)
    parser.add_argument("--safety-shim", required=True)
    parser.add_argument("--dispatch-real", required=True)
    parser.add_argument("--dispatch-shim", required=True)
    parser.add_argument("--cpp-real", required=True)
    parser.add_argument("--cpp-shim", required=True)
    parser.add_argument("--api", required=True)
    parser.add_argument("--programmatic", required=True)
    parser.add_argument("--mock-c", required=True)
    parser.add_argument("--self-test", action="store_true",
                        help="prove each checker can fail, then exit")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    failures = []
    passed = 0
    failed = 0
    skipped = []

    def case(name, function):
        nonlocal passed, failed
        try:
            function()
            print(f"  PASS  {name}")
            passed += 1
        except Failure as error:
            print(f"  FAIL  {name}: {error}")
            failures.append(name)
            failed += 1

    _, host_output, _ = run(args.safety_real)
    host_flags, host_metadata = parse_flags(host_output)
    host_identity = parse_identity(host_metadata)
    print(f"host identity: {host_metadata.get('identity', '?')} "
          f"({sum(host_flags.values())} features)")

    def test_passthrough():
        _, output, error = run(args.safety_shim)
        flags, metadata = parse_flags(output)
        check_equal(flags, host_flags, "unconfigured feature set")
        check_equal(metadata, host_metadata, "unconfigured identity")
        if error.strip():
            raise Failure(f"unconfigured mock wrote stderr: {error!r}")
    case("direct link: unconfigured mock equals real library", test_passthrough)

    def test_preload_passthrough():
        _, output, error = run(args.safety_real, preload=args.mock_c)
        flags, metadata = parse_flags(output)
        check_equal(flags, host_flags, "preload feature set")
        check_equal(metadata, host_metadata, "preload identity")
        if error.strip():
            raise Failure(f"unconfigured preload wrote stderr: {error!r}")
    if LD_PRELOAD_SUPPORTED:
        case("LD_PRELOAD: unconfigured mock is a pass-through",
             test_preload_passthrough)
    else:
        label = "ld-preload-passthrough"
        reason = "LD_PRELOAD not supported on this platform"
        skipped.append((label, reason))
        print(f"  SKIP  {label}: {reason}")

    def test_profile_subset():
        for profile in ("milan", "rome", "genoa", "turin", "Conroe-v1"):
            _, output, _ = run(
                args.safety_shim,
                {"AU_CPUID_MOCK_PROFILE": profile,
                 "AU_CPUID_MOCK_STRICT": "0"},
            )
            flags, _ = parse_flags(output)
            check_subset(host_flags, flags, f"profile:{profile}")
    case("profiles report only host-supported features", test_profile_subset)

    def test_unknown_uarch_identity():
        _, output, error = run(
            args.safety_shim,
            {"AU_CPUID_MOCK_PROFILE": "Opteron_G1-v1",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        flags, metadata = parse_flags(output)
        check_subset(host_flags, flags, "unknown-uarch profile")
        if metadata.get("identity") != host_metadata.get("identity"):
            raise Failure("unknown-uarch profile substituted an identity")
        if "profile's microarchitecture is not in the uarch registry" not in error:
            raise Failure(f"unknown-uarch refusal was not reported: {error!r}")
        if "host's microarchitecture is not in the uarch registry" in error:
            raise Failure(f"unknown-uarch refusal blamed the host: {error!r}")
    case("unknown uarch: identity substitution fails closed",
         test_unknown_uarch_identity)

    def test_rome_correctness():
        _, output, _ = run(
            args.safety_shim,
            {"AU_CPUID_MOCK_PROFILE": "rome",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        flags, metadata = parse_flags(output)
        identity = parse_identity(metadata)
        if host_flags.get("avx512f", False) and flags.get("avx512f", False):
            raise Failure("rome retained host AVX-512F")
        # The test box is Zen3, but retain a meaningful skip for older or
        # non-AMD hosts where Rome identity is not a downward request.
        if host_identity[0] == 1 and host_identity[4] > 3:
            if identity[4] != 3:
                raise Failure(f"rome uarch was {identity[4]}, expected Zen2 (3)")
            if identity[0] != 1:
                raise Failure("rome changed an AMD host to a non-AMD identity")
    case("rome masks features and presents a downward identity",
         test_rome_correctness)

    # Venice is the highest-uarch profile in the table and is therefore an
    # upward-identity probe on current hosts.
    impossible_profile = "Venice-v1"

    if host_identity[4] >= 7:
        label = "strict-upward"
        reason = "host is Zen6+, no newer profile in table"
        skipped.append((label, reason))
        print(f"  SKIP  {label}: {reason}")
    else:
        def test_strict_default():
            rc, _, error = run(
                args.safety_shim,
                {"AU_CPUID_MOCK_PROFILE": impossible_profile},
                expect_ok=False,
            )
            check_programmatic_abort(rc, error)
        case("strict default: upward identity aborts",
             test_strict_default)

        def test_strict_enabled():
            rc, _, error = run(
                args.safety_shim,
                {"AU_CPUID_MOCK_PROFILE": impossible_profile,
                 "AU_CPUID_MOCK_STRICT": "1"},
                expect_ok=False,
            )
            check_programmatic_abort(rc, error)
        case("AU_CPUID_MOCK_STRICT=1: upward identity aborts",
             test_strict_enabled)

        def test_non_strict_safe_subset():
            _, output, error = run(
                args.safety_shim,
                {"AU_CPUID_MOCK_PROFILE": impossible_profile,
                 "AU_CPUID_MOCK_STRICT": "0"},
            )
            flags, _ = parse_flags(output)
            check_subset(host_flags, flags, "non-strict impossible profile")
            if "refus" not in error.lower() and "keeps" not in error.lower():
                raise Failure(f"non-strict refusal was not reported: {error!r}")
        case("strict=0: refusal warns and applies the safe subset",
             test_non_strict_safe_subset)

        def test_strict_off_tokens():
            for token in ("0", "false", "no", "off"):
                _, output, error = run(
                    args.safety_shim,
                    {"AU_CPUID_MOCK_PROFILE": impossible_profile,
                     "AU_CPUID_MOCK_STRICT": token},
                )
                flags, _ = parse_flags(output)
                check_subset(host_flags, flags, f"strict-off token:{token}")
                if "keeps the host CPU identity" not in error:
                    raise Failure(
                        f"strict-off token {token!r} lacked identity warning"
                    )
        case("strict off tokens: 0/false/no/off warn and continue",
             test_strict_off_tokens)

        def test_strict_unknown_token():
            rc, _, error = run(
                args.safety_shim,
                {"AU_CPUID_MOCK_PROFILE": impossible_profile,
                 "AU_CPUID_MOCK_STRICT": "maybe"},
                expect_ok=False,
            )
            check_programmatic_abort(rc, error)
        case("strict unknown token: fail-safe abort", test_strict_unknown_token)

    if host_identity[4] < 7:
        def test_programmatic_strict():
            rc, _, error = run(
                args.programmatic,
                expect_ok=False,
            )
            check_programmatic_abort(rc, error)
        case("programmatic strict default: upward identity aborts",
             test_programmatic_strict)

        def test_programmatic_non_strict():
            rc, output, error = run(
                args.programmatic, {"AU_CPUID_MOCK_STRICT": "0"}
            )
            check_programmatic_continue(rc, output, error)
            if host_identity[0] == 1 and "unclassified_status=6" not in output:
                raise Failure(
                    "pre-Zen AMD profile did not return "
                    "AU_MOCK_ERR_IDENTITY_UNCLASSIFIED"
                )
        case("programmatic strict=0: warn and apply the safe subset",
             test_programmatic_non_strict)

    def test_unknown_profile():
        rc, _, error = run(
            args.safety_shim,
            {"AU_CPUID_MOCK_PROFILE": "no-such-profile",
             "AU_CPUID_MOCK_STRICT": "1"},
            expect_ok=False,
        )
        check_programmatic_abort(rc, error)
        if "unknown profile" not in error.lower():
            raise Failure(f"unknown profile was not rejected: {error!r}")
    case("unknown profile: strict=1 rejects it", test_unknown_profile)

    def test_dispatch_baseline():
        _, output, _ = run(args.dispatch_real)
        parts = output.split()
        if len(parts) < 3 or parts[2] != "ok":
            raise Failure(f"real-linked dispatch failed: {output!r}")
    case("dispatch: real-linked baseline executes", test_dispatch_baseline)

    def test_dispatch_profile():
        _, baseline, _ = run(args.dispatch_shim)
        base_kernel = baseline.split()[0]
        _, output, _ = run(
            args.dispatch_shim,
            {"AU_CPUID_MOCK_PROFILE": "Conroe-v1",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        parts = output.split()
        if len(parts) < 3:
            raise Failure(f"unparsable dispatch output: {output!r}")
        if parts[2] != "ok":
            raise Failure(f"profile-selected kernel returned {parts[2]!r}")
        if host_flags.get("avx2", False) and parts[0] == base_kernel:
            raise Failure("Conroe profile did not select a lower kernel")
    if host_flags.get("avx2", False):
        case("dispatch-and-execute: profile selects a lower running kernel",
             test_dispatch_profile)
    else:
        label = "dispatch-and-execute"
        reason = "host has no AVX2"
        skipped.append((label, reason))
        print(f"  SKIP  {label}: {reason}")

    def test_cpp_passthrough():
        _, real_output, _ = run(args.cpp_real)
        _, mock_output, _ = run(args.cpp_shim)
        check_equal(parse_cpp(real_output), parse_cpp(mock_output),
                    "C++ pass-through")
    case("C++ consumer: unconfigured X86Cpu matches real", test_cpp_passthrough)

    def test_cpp_profile():
        _, c_output, _ = run(
            args.safety_shim,
            {"AU_CPUID_MOCK_PROFILE": "rome",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        c_flags, c_metadata = parse_flags(c_output)
        _, output, _ = run(
            args.cpp_shim,
            {"AU_CPUID_MOCK_PROFILE": "rome",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        cpp = parse_cpp(output)
        if cpp["avx512f"] and not c_flags.get("avx512f", False):
            raise Failure("C++ X86Cpu reported AVX-512F absent from C result")
        if host_identity[0] == 1 and host_identity[4] > 3:
            if cpp["uarch"] != parse_identity(c_metadata)[4]:
                raise Failure("C++ and C mocked uarch identities differ")
    case("C++ consumer: X86Cpu receives the masked profile", test_cpp_profile)

    def test_cpp_preload():
        _, direct_output, _ = run(
            args.cpp_shim,
            {"AU_CPUID_MOCK_PROFILE": "rome",
             "AU_CPUID_MOCK_STRICT": "0"},
        )
        _, preload_output, _ = run(
            args.cpp_real,
            {"AU_CPUID_MOCK_PROFILE": "rome",
             "AU_CPUID_MOCK_STRICT": "0"},
            preload=os.path.abspath(
                os.path.join(os.path.dirname(args.mock_c), "libaoclutils_mock.so")
            ),
        )
        check_equal(parse_cpp(direct_output), parse_cpp(preload_output),
                    "C++ preload")
    # The C and C++ mock libraries live beside one another in the build tree.
    if LD_PRELOAD_SUPPORTED:
        case("LD_PRELOAD: C++ X86Cpu is masked", test_cpp_preload)
    else:
        label = "ld-preload-cpp-masked"
        reason = "LD_PRELOAD not supported on this platform"
        skipped.append((label, reason))
        print(f"  SKIP  {label}: {reason}")

    def test_api():
        for label, environment in (("default", {}),
                                   ("strict=0", {"AU_CPUID_MOCK_STRICT": "0"})):
            _, output, error = run(args.api, environment)
            if error.strip() and "AU_CPUID_MOCK_PROFILE" not in error:
                raise Failure(f"unexpected API-test stderr ({label}): {error!r}")
            states, statuses = parse_api(output)
            if statuses.get("sentinel_apply") != 4:
                raise Failure(f"bad sentinel apply status ({label}): {statuses!r}")
            if statuses.get("sentinel_reset") != 4:
                raise Failure(f"bad sentinel reset status ({label}): {statuses!r}")
            if statuses.get("profile_status") != 0:
                raise Failure(
                    f"plain profile was not a successful narrowing ({label}): "
                    f"{statuses!r}"
                )
            if statuses.get("reset_status") != 0:
                raise Failure(f"reset failed ({label}): {statuses!r}")
            check_equal(states["host"], states["reset"], f"API reset ({label})")
            if states["host"].get("avx512f") and states["rome"].get("avx512f"):
                raise Failure(f"API Rome profile retained AVX-512F ({label})")
    case("C API: profile, reset, and reserved CPU selector", test_api)

    print()
    print(f"MockCpuid summary: passed={passed} failed={failed} "
          f"skipped={len(skipped)}")
    for label, reason in skipped:
        print(f"MockCpuid skipped: {label} - {reason}")
    if failed != 0:
        print(f"FAILED: {len(failures)} case(s): {', '.join(failures)}")
        return 1
    print("All MockCpuid behavioural checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
