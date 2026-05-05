#!/usr/bin/env python3
# Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
"""
SimNow CPUID Data Decoder

This script decodes CPUID information from SimNow data files used in
the aocl-utils mock testing framework.

Usage:
    python scripts/decode_simnow.py -f ./Library/Tests/Cpuid/Mock/simnowdata/EPYC-Genoa-v1/EPYC-Genoa-v1

Author: AOCL-Utils Team
Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
"""

import argparse
import os
import re
import sys
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

# ANSI Color codes for terminal output
class Colors:
    """ANSI color codes for terminal output."""
    RESET = "\033[0m"
    BOLD = "\033[1m"
    DIM = "\033[2m"
    UNDERLINE = "\033[4m"

    # Foreground colors
    BLACK = "\033[30m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"
    WHITE = "\033[37m"

    # Bright foreground colors
    BRIGHT_BLACK = "\033[90m"
    BRIGHT_RED = "\033[91m"
    BRIGHT_GREEN = "\033[92m"
    BRIGHT_YELLOW = "\033[93m"
    BRIGHT_BLUE = "\033[94m"
    BRIGHT_MAGENTA = "\033[95m"
    BRIGHT_CYAN = "\033[96m"
    BRIGHT_WHITE = "\033[97m"

    # Background colors
    BG_BLACK = "\033[40m"
    BG_RED = "\033[41m"
    BG_GREEN = "\033[42m"
    BG_YELLOW = "\033[43m"
    BG_BLUE = "\033[44m"
    BG_MAGENTA = "\033[45m"
    BG_CYAN = "\033[46m"
    BG_WHITE = "\033[47m"


def supports_color() -> bool:
    """Check if the terminal supports colors."""
    if not sys.stdout.isatty():
        return False
    if os.environ.get("NO_COLOR"):
        return False
    if os.environ.get("TERM") == "dumb":
        return False
    return True


# Global color enable flag
USE_COLORS = supports_color()


def colorize(text: str, *colors: str) -> str:
    """Apply colors to text if colors are enabled."""
    if not USE_COLORS:
        return text
    color_str = "".join(colors)
    return f"{color_str}{text}{Colors.RESET}"


@dataclass
class CpuidEntry:
    """Represents a single CPUID request/response pair."""
    eax_in: int
    ebx_in: int
    ecx_in: int
    edx_in: int
    eax_out: int
    ebx_out: int
    ecx_out: int
    edx_out: int


@dataclass
class FlagDefinition:
    """Definition of a CPU feature flag."""
    name: str
    leaf: int
    subleaf: int
    register: str  # 'eax', 'ebx', 'ecx', 'edx'
    bit_mask: int
    description: str = ""


# CPUID flag definitions based on X86RawData.cc
CPUID_FLAGS: List[FlagDefinition] = [
    # CPUID 0x00000001 ECX flags
    FlagDefinition("sse3", 0x00000001, 0, "ecx", 0x00000001, "SSE3 Instructions"),
    FlagDefinition("pclmulqdq", 0x00000001, 0, "ecx", 0x00000002, "PCLMULQDQ Instruction"),
    FlagDefinition("dtes64", 0x00000001, 0, "ecx", 0x00000004, "64-bit DS Area"),
    FlagDefinition("monitor", 0x00000001, 0, "ecx", 0x00000008, "MONITOR/MWAIT"),
    FlagDefinition("dscpl", 0x00000001, 0, "ecx", 0x00000010, "CPL Qualified Debug Store"),
    FlagDefinition("vmx", 0x00000001, 0, "ecx", 0x00000020, "Virtual Machine Extensions"),
    FlagDefinition("smx", 0x00000001, 0, "ecx", 0x00000040, "Safer Mode Extensions"),
    FlagDefinition("est", 0x00000001, 0, "ecx", 0x00000080, "Enhanced SpeedStep"),
    FlagDefinition("tm2", 0x00000001, 0, "ecx", 0x00000100, "Thermal Monitor 2"),
    FlagDefinition("ssse3", 0x00000001, 0, "ecx", 0x00000200, "SSSE3 Instructions"),
    FlagDefinition("cid", 0x00000001, 0, "ecx", 0x00000400, "L1 Context ID"),
    FlagDefinition("fma", 0x00000001, 0, "ecx", 0x00001000, "Fused Multiply-Add"),
    FlagDefinition("cx16", 0x00000001, 0, "ecx", 0x00002000, "CMPXCHG16B Instruction"),
    FlagDefinition("xtpr", 0x00000001, 0, "ecx", 0x00004000, "xTPR Update Control"),
    FlagDefinition("pdcm", 0x00000001, 0, "ecx", 0x00008000, "Perfmon and Debug Capability"),
    FlagDefinition("pcid", 0x00000001, 0, "ecx", 0x00020000, "Process-context Identifiers"),
    FlagDefinition("dca", 0x00000001, 0, "ecx", 0x00040000, "Direct Cache Access"),
    FlagDefinition("sse4_1", 0x00000001, 0, "ecx", 0x00080000, "SSE4.1 Instructions"),
    FlagDefinition("sse4_2", 0x00000001, 0, "ecx", 0x00100000, "SSE4.2 Instructions"),
    FlagDefinition("x2apic", 0x00000001, 0, "ecx", 0x00200000, "x2APIC"),
    FlagDefinition("movbe", 0x00000001, 0, "ecx", 0x00400000, "MOVBE Instruction"),
    FlagDefinition("popcnt", 0x00000001, 0, "ecx", 0x00800000, "POPCNT Instruction"),
    FlagDefinition("tsc_deadline", 0x00000001, 0, "ecx", 0x01000000, "TSC Deadline"),
    FlagDefinition("aes", 0x00000001, 0, "ecx", 0x02000000, "AES Instructions"),
    FlagDefinition("xsave", 0x00000001, 0, "ecx", 0x04000000, "XSAVE/XRSTOR"),
    FlagDefinition("osxsave", 0x00000001, 0, "ecx", 0x08000000, "OSXSAVE"),
    FlagDefinition("avx", 0x00000001, 0, "ecx", 0x10000000, "AVX Instructions"),
    FlagDefinition("f16c", 0x00000001, 0, "ecx", 0x20000000, "16-bit FP Conversion"),
    FlagDefinition("rdrand", 0x00000001, 0, "ecx", 0x40000000, "RDRAND Instruction"),
    FlagDefinition("hypervisor", 0x00000001, 0, "ecx", 0x80000000, "Hypervisor Present"),

    # CPUID 0x00000001 EDX flags
    FlagDefinition("fpu", 0x00000001, 0, "edx", 0x00000001, "x87 FPU"),
    FlagDefinition("vme", 0x00000001, 0, "edx", 0x00000002, "Virtual 8086 Mode"),
    FlagDefinition("de", 0x00000001, 0, "edx", 0x00000004, "Debugging Extensions"),
    FlagDefinition("pse", 0x00000001, 0, "edx", 0x00000008, "Page Size Extension"),
    FlagDefinition("tsc", 0x00000001, 0, "edx", 0x00000010, "Time Stamp Counter"),
    FlagDefinition("msr", 0x00000001, 0, "edx", 0x00000020, "Model Specific Registers"),
    FlagDefinition("pae", 0x00000001, 0, "edx", 0x00000040, "Physical Address Extension"),
    FlagDefinition("mce", 0x00000001, 0, "edx", 0x00000080, "Machine Check Exception"),
    FlagDefinition("cx8", 0x00000001, 0, "edx", 0x00000100, "CMPXCHG8B Instruction"),
    FlagDefinition("apic", 0x00000001, 0, "edx", 0x00000200, "APIC On-Chip"),
    FlagDefinition("sep", 0x00000001, 0, "edx", 0x00000800, "SYSENTER/SYSEXIT"),
    FlagDefinition("mtrr", 0x00000001, 0, "edx", 0x00001000, "Memory Type Range Registers"),
    FlagDefinition("pge", 0x00000001, 0, "edx", 0x00002000, "Page Global Enable"),
    FlagDefinition("mca", 0x00000001, 0, "edx", 0x00004000, "Machine Check Architecture"),
    FlagDefinition("cmov", 0x00000001, 0, "edx", 0x00008000, "Conditional Move Instructions"),
    FlagDefinition("pat", 0x00000001, 0, "edx", 0x00010000, "Page Attribute Table"),
    FlagDefinition("pse36", 0x00000001, 0, "edx", 0x00020000, "36-bit Page Size Extension"),
    FlagDefinition("pn", 0x00000001, 0, "edx", 0x00040000, "Processor Serial Number"),
    FlagDefinition("clflush", 0x00000001, 0, "edx", 0x00080000, "CLFLUSH Instruction"),
    FlagDefinition("ds", 0x00000001, 0, "edx", 0x00200000, "Debug Store"),
    FlagDefinition("acpi", 0x00000001, 0, "edx", 0x00400000, "Thermal Monitor and Clock Ctrl"),
    FlagDefinition("mmx", 0x00000001, 0, "edx", 0x00800000, "MMX Technology"),
    FlagDefinition("fxsr", 0x00000001, 0, "edx", 0x01000000, "FXSAVE/FXRSTOR"),
    FlagDefinition("sse", 0x00000001, 0, "edx", 0x02000000, "SSE Instructions"),
    FlagDefinition("sse2", 0x00000001, 0, "edx", 0x04000000, "SSE2 Instructions"),
    FlagDefinition("ss", 0x00000001, 0, "edx", 0x08000000, "Self Snoop"),
    FlagDefinition("ht", 0x00000001, 0, "edx", 0x10000000, "Hyper-Threading"),
    FlagDefinition("tm", 0x00000001, 0, "edx", 0x20000000, "Thermal Monitor"),
    FlagDefinition("ia64", 0x00000001, 0, "edx", 0x40000000, "IA64 Processor"),
    FlagDefinition("pbe", 0x00000001, 0, "edx", 0x80000000, "Pending Break Enable"),

    # CPUID 0x00000006 EAX flags
    FlagDefinition("arat", 0x00000006, 0, "eax", 0x00000004, "Always Running APIC Timer"),

    # CPUID 0x00000007 EBX flags (Structured Extended Feature Flags)
    FlagDefinition("fsgsbase", 0x00000007, 0, "ebx", 0x00000001, "FSGSBASE Instructions"),
    FlagDefinition("tsc_adjust", 0x00000007, 0, "ebx", 0x00000002, "TSC_ADJUST MSR"),
    FlagDefinition("bmi1", 0x00000007, 0, "ebx", 0x00000008, "Bit Manipulation Instruction Set 1"),
    FlagDefinition("hle", 0x00000007, 0, "ebx", 0x00000010, "Hardware Lock Elision"),
    FlagDefinition("avx2", 0x00000007, 0, "ebx", 0x00000020, "AVX2 Instructions"),
    FlagDefinition("smep", 0x00000007, 0, "ebx", 0x00000080, "Supervisor Mode Execution Prevention"),
    FlagDefinition("bmi2", 0x00000007, 0, "ebx", 0x00000100, "Bit Manipulation Instruction Set 2"),
    FlagDefinition("erms", 0x00000007, 0, "ebx", 0x00000200, "Enhanced REP MOVSB/STOSB"),
    FlagDefinition("invpcid", 0x00000007, 0, "ebx", 0x00000400, "INVPCID Instruction"),
    FlagDefinition("rtm", 0x00000007, 0, "ebx", 0x00000800, "Restricted Transactional Memory"),
    FlagDefinition("mpx", 0x00000007, 0, "ebx", 0x00004000, "Memory Protection Extensions"),
    FlagDefinition("avx512f", 0x00000007, 0, "ebx", 0x00010000, "AVX-512 Foundation"),
    FlagDefinition("avx512dq", 0x00000007, 0, "ebx", 0x00020000, "AVX-512 Doubleword and Quadword"),
    FlagDefinition("rdseed", 0x00000007, 0, "ebx", 0x00040000, "RDSEED Instruction"),
    FlagDefinition("adx", 0x00000007, 0, "ebx", 0x00080000, "Multi-Precision Add-Carry Instructions"),
    FlagDefinition("smap", 0x00000007, 0, "ebx", 0x00100000, "Supervisor Mode Access Prevention"),
    FlagDefinition("avx512ifma", 0x00000007, 0, "ebx", 0x00200000, "AVX-512 Integer Fused Multiply-Add"),
    FlagDefinition("pcommit", 0x00000007, 0, "ebx", 0x00400000, "PCOMMIT Instruction"),
    FlagDefinition("clflushopt", 0x00000007, 0, "ebx", 0x00800000, "CLFLUSHOPT Instruction"),
    FlagDefinition("clwb", 0x00000007, 0, "ebx", 0x01000000, "CLWB Instruction"),
    FlagDefinition("avx512pf", 0x00000007, 0, "ebx", 0x04000000, "AVX-512 Prefetch Instructions"),
    FlagDefinition("avx512er", 0x00000007, 0, "ebx", 0x08000000, "AVX-512 Exponential and Reciprocal"),
    FlagDefinition("avx512cd", 0x00000007, 0, "ebx", 0x10000000, "AVX-512 Conflict Detection"),
    FlagDefinition("sha_ni", 0x00000007, 0, "ebx", 0x20000000, "SHA Extensions"),
    FlagDefinition("avx512bw", 0x00000007, 0, "ebx", 0x40000000, "AVX-512 Byte and Word Instructions"),
    FlagDefinition("avx512vl", 0x00000007, 0, "ebx", 0x80000000, "AVX-512 Vector Length Extensions"),

    # CPUID 0x00000007 ECX flags
    FlagDefinition("avx512vbmi", 0x00000007, 0, "ecx", 0x00000002, "AVX-512 Vector Bit Manipulation"),
    FlagDefinition("umip", 0x00000007, 0, "ecx", 0x00000004, "User-Mode Instruction Prevention"),
    FlagDefinition("pku", 0x00000007, 0, "ecx", 0x00000008, "Memory Protection Keys for User-mode"),
    FlagDefinition("ospke", 0x00000007, 0, "ecx", 0x00000010, "PKU Enabled by OS"),
    FlagDefinition("avx512vbmi2", 0x00000007, 0, "ecx", 0x00000040, "AVX-512 Vector Bit Manipulation 2"),
    FlagDefinition("vaes", 0x00000007, 0, "ecx", 0x00000200, "Vector AES"),
    FlagDefinition("vpclmulqdq", 0x00000007, 0, "ecx", 0x00000400, "Vector PCLMULQDQ"),
    FlagDefinition("avx512_vnni", 0x00000007, 0, "ecx", 0x00000800, "AVX-512 Vector Neural Network"),
    FlagDefinition("avx512_bitalg", 0x00000007, 0, "ecx", 0x00001000, "AVX-512 Bit Algorithms"),
    FlagDefinition("avx512_vpopcntdq", 0x00000007, 0, "ecx", 0x00004000, "AVX-512 VPOPCNTDQ"),
    FlagDefinition("la57", 0x00000007, 0, "ecx", 0x00010000, "5-level Paging"),
    FlagDefinition("rdpid", 0x00000007, 0, "ecx", 0x00400000, "RDPID Instruction"),
    FlagDefinition("movdiri", 0x00000007, 0, "ecx", 0x08000000, "MOVDIRI Instruction"),
    FlagDefinition("movdir64b", 0x00000007, 0, "ecx", 0x10000000, "MOVDIR64B Instruction"),

    # CPUID 0x00000007 EDX flags
    FlagDefinition("avx512_4vnniw", 0x00000007, 0, "edx", 0x00000004, "AVX-512 4-register Neural Network"),
    FlagDefinition("avx512_4fmaps", 0x00000007, 0, "edx", 0x00000008, "AVX-512 4-register Multiply Accumulation"),
    FlagDefinition("avx512_vpintersect", 0x00000007, 0, "edx", 0x00000100, "AVX-512 VP2INTERSECT"),
    FlagDefinition("avx512_fp16", 0x00000007, 0, "edx", 0x00800000, "AVX-512 FP16 Instructions"),

    # CPUID 0x00000007 subleaf 1 EAX flags
    FlagDefinition("avx512_bf16", 0x00000007, 1, "eax", 0x00000020, "AVX-512 BF16 Instructions"),
    FlagDefinition("avxvnni", 0x00000007, 1, "eax", 0x00000010, "AVX-VNNI Instructions"),
    FlagDefinition("avx_ifma", 0x00000007, 1, "eax", 0x00800000, "AVX IFMA Instructions"),

    # CPUID 0x00000007 subleaf 1 EDX flags
    FlagDefinition("avx_vnni_int8", 0x00000007, 1, "edx", 0x00000010, "AVX VNNI INT8 Instructions"),
    FlagDefinition("avx_ne_convert", 0x00000007, 1, "edx", 0x00000020, "AVX NE Convert Instructions"),
    FlagDefinition("avx_vnni_int16", 0x00000007, 1, "edx", 0x00000400, "AVX VNNI INT16 Instructions"),
    FlagDefinition("avx10", 0x00000007, 1, "edx", 0x00080000, "AVX10 Converged Vector ISA"),

    # CPUID 0x0000000D subleaf 1 EAX flags (XSAVE)
    FlagDefinition("xsaveopt", 0x0000000D, 1, "eax", 0x00000001, "XSAVEOPT Available"),
    FlagDefinition("xsavec", 0x0000000D, 1, "eax", 0x00000002, "XSAVEC Available"),
    FlagDefinition("xgetbv1", 0x0000000D, 1, "eax", 0x00000004, "XGETBV with ECX=1"),
    FlagDefinition("xsaves", 0x0000000D, 1, "eax", 0x00000008, "XSAVES/XRSTORS"),

    # CPUID 0x80000001 ECX flags (AMD Extended)
    FlagDefinition("lahf_lm", 0x80000001, 0, "ecx", 0x00000001, "LAHF/SAHF in Long Mode"),
    FlagDefinition("cmp_legacy", 0x80000001, 0, "ecx", 0x00000002, "Core Multi-Processing Legacy"),
    FlagDefinition("svm", 0x80000001, 0, "ecx", 0x00000004, "Secure Virtual Machine"),
    FlagDefinition("extapic", 0x80000001, 0, "ecx", 0x00000008, "Extended APIC Space"),
    FlagDefinition("cr8legacy", 0x80000001, 0, "ecx", 0x00000010, "CR8 in 32-bit Mode"),
    FlagDefinition("abm", 0x80000001, 0, "ecx", 0x00000020, "Advanced Bit Manipulation"),
    FlagDefinition("sse4a", 0x80000001, 0, "ecx", 0x00000040, "SSE4a Instructions"),
    FlagDefinition("misalignsse", 0x80000001, 0, "ecx", 0x00000080, "Misaligned SSE Mode"),
    FlagDefinition("_3dnowprefetch", 0x80000001, 0, "ecx", 0x00000100, "3DNow! PREFETCH/PREFETCHW"),
    FlagDefinition("osvw", 0x80000001, 0, "ecx", 0x00000200, "OS Visible Workaround"),
    FlagDefinition("ibs", 0x80000001, 0, "ecx", 0x00000400, "Instruction Based Sampling"),
    FlagDefinition("xop", 0x80000001, 0, "ecx", 0x00000800, "XOP Instructions"),
    FlagDefinition("skinit", 0x80000001, 0, "ecx", 0x00001000, "SKINIT/STGI Instructions"),
    FlagDefinition("wdt", 0x80000001, 0, "ecx", 0x00002000, "Watchdog Timer"),
    FlagDefinition("lwp", 0x80000001, 0, "ecx", 0x00008000, "Light Weight Profiling"),
    FlagDefinition("fma4", 0x80000001, 0, "ecx", 0x00010000, "4-operand FMA Instructions"),
    FlagDefinition("tce", 0x80000001, 0, "ecx", 0x00020000, "Translation Cache Extension"),
    FlagDefinition("nodeid_msr", 0x80000001, 0, "ecx", 0x00080000, "NodeId MSR"),
    FlagDefinition("tbm", 0x80000001, 0, "ecx", 0x00200000, "Trailing Bit Manipulation"),
    FlagDefinition("topoext", 0x80000001, 0, "ecx", 0x00400000, "Topology Extensions"),
    FlagDefinition("perfctr_core", 0x80000001, 0, "ecx", 0x00800000, "Core Performance Counter Extensions"),
    FlagDefinition("perfctr_nb", 0x80000001, 0, "ecx", 0x01000000, "NB Performance Counter Extensions"),

    # CPUID 0x80000001 EDX flags
    FlagDefinition("syscall", 0x80000001, 0, "edx", 0x00000800, "SYSCALL/SYSRET"),
    FlagDefinition("nxxd", 0x80000001, 0, "edx", 0x00100000, "Execute Disable Bit"),
    FlagDefinition("mmxext", 0x80000001, 0, "edx", 0x00400000, "AMD Extensions to MMX"),
    FlagDefinition("fxsr_opt", 0x80000001, 0, "edx", 0x02000000, "FXSAVE/FXRSTOR Optimizations"),
    FlagDefinition("pdpe1gb", 0x80000001, 0, "edx", 0x04000000, "1GB Pages"),
    FlagDefinition("rdtscp", 0x80000001, 0, "edx", 0x08000000, "RDTSCP Instruction"),
    FlagDefinition("lmi64", 0x80000001, 0, "edx", 0x20000000, "Long Mode"),
    FlagDefinition("_3dnowext", 0x80000001, 0, "edx", 0x40000000, "AMD 3DNow! Extensions"),
    FlagDefinition("_3dnow", 0x80000001, 0, "edx", 0x80000000, "3DNow! Instructions"),

    # CPUID 0x80000007 EDX flags
    FlagDefinition("invtsc", 0x80000007, 0, "edx", 0x00000100, "Invariant TSC"),

    # CPUID 0x8000000A EDX flags (SVM Features)
    FlagDefinition("npt", 0x8000000A, 0, "edx", 0x00000001, "Nested Page Tables"),
    FlagDefinition("lbrv", 0x8000000A, 0, "edx", 0x00000002, "LBR Virtualization"),
    FlagDefinition("svm_lock", 0x8000000A, 0, "edx", 0x00000004, "SVM Lock"),
    FlagDefinition("nrip_save", 0x8000000A, 0, "edx", 0x00000008, "NRIP Save"),
    FlagDefinition("tsc_scale", 0x8000000A, 0, "edx", 0x00000010, "TSC Rate MSR"),
    FlagDefinition("vmcb_clean", 0x8000000A, 0, "edx", 0x00000020, "VMCB Clean Bits"),
    FlagDefinition("flushbyasid", 0x8000000A, 0, "edx", 0x00000040, "Flush by ASID"),
    FlagDefinition("decodeassists", 0x8000000A, 0, "edx", 0x00000080, "Decode Assists"),
    FlagDefinition("pause_filter", 0x8000000A, 0, "edx", 0x00000400, "Pause Filter"),
    FlagDefinition("pfthreshold", 0x8000000A, 0, "edx", 0x00001000, "Pause Filter Threshold"),
    FlagDefinition("x2avic", 0x8000000A, 0, "edx", 0x00080000, "x2AVIC"),

    # CPUID 0xC0000001 EDX flags (VIA/Centaur)
    FlagDefinition("xstore", 0xC0000001, 0, "edx", 0x00000004, "VIA PadLock RNG"),
    FlagDefinition("xstore_en", 0xC0000001, 0, "edx", 0x00000008, "VIA PadLock RNG Enabled"),
    FlagDefinition("xcrypt", 0xC0000001, 0, "edx", 0x00000040, "VIA PadLock AES"),
    FlagDefinition("xcrypt_en", 0xC0000001, 0, "edx", 0x00000080, "VIA PadLock AES Enabled"),
    FlagDefinition("ace2", 0xC0000001, 0, "edx", 0x00000100, "VIA PadLock ACE2"),
    FlagDefinition("ace2_en", 0xC0000001, 0, "edx", 0x00000200, "VIA PadLock ACE2 Enabled"),
    FlagDefinition("phe", 0xC0000001, 0, "edx", 0x00000400, "VIA PadLock PHE"),
    FlagDefinition("phe_en", 0xC0000001, 0, "edx", 0x00000800, "VIA PadLock PHE Enabled"),
    FlagDefinition("pmm", 0xC0000001, 0, "edx", 0x00001000, "VIA PadLock PMM"),
    FlagDefinition("pmm_en", 0xC0000001, 0, "edx", 0x00002000, "VIA PadLock PMM Enabled"),

    # CPUID 0x80000008 EBX flags
    FlagDefinition("clzero", 0x80000008, 0, "ebx", 0x00000001, "CLZERO Instruction"),

    # CPUID 0x80000021 EAX flags (AMD Extended Feature Identification 2)
    FlagDefinition("avx512_bmm", 0x80000021, 0, "eax", 0x00800000, "AVX-512 Bit Matrix Multiply"),
]


# Family and Model mappings for microarchitecture detection
FAMILY_NAMES = {
    0x17: "Zen/Zen+/Zen2",
    0x19: "Zen3/Zen4",
    0x1A: "Zen5/Zen6",
}

UARCH_DETECTION = {
    # Family 0x17 (Zen, Zen+, Zen2)
    0x17: {
        "name": "Zen Family",
        "models": {
            (0x00, 0x1f): "Zen",
            (0x20, 0x2f): "Zen/Zen2 (detection based on CLWB flag)",
            (0x30, 0xff): "Zen2",
        }
    },
    # Family 0x19 (Zen3, Zen4)
    0x19: {
        "name": "Zen3/Zen4 Family",
        "models": {
            (0x00, 0x0f): "Zen3",
            (0x10, 0x1f): "Zen4",
            (0x20, 0x5f): "Zen3",
            (0x60, 0xaf): "Zen4",
            (0xb0, 0xff): "Zen3/Zen4 (detection based on AVX512F flag)",
        }
    },
    # Family 0x1A (Zen5, Zen6) - model ranges per GCC get_amd_cpu / X86RawData.hh
    0x1A: {
        "name": "Zen5/Zen6 Family",
        "models": {
            (0x00, 0x4f): "Zen5",
            (0x50, 0x5f): "Zen6",
            (0x60, 0x77): "Zen5",
            (0x78, 0x7f): "Zen5/Zen6 (detection based on AVX512_BMM flag)",
            (0x80, 0xcf): "Zen6",
            (0xd0, 0xd7): "Zen5",
            (0xd8, 0xe7): "Zen6",
            (0xe8, 0xff): "Zen5/Zen6 (detection based on AVX512_BMM flag)",
        }
    },
}


class SimNowDecoder:
    """Decoder for SimNow CPUID data files."""

    def __init__(self, filepath: str):
        self.filepath = filepath
        self.cpuid_data: Dict[Tuple[int, int, int, int], Tuple[int, int, int, int]] = {}
        self.vendor = "Unknown"
        self.family = 0
        self.model = 0
        self.stepping = 0
        self.uarch = "Unknown"
        self.present_flags: List[str] = []
        self.absent_flags: List[str] = []

    def parse_file(self) -> bool:
        """Parse the SimNow data file."""
        if not os.path.exists(self.filepath):
            print(colorize(f"Error: File not found: {self.filepath}", Colors.RED, Colors.BOLD))
            return False

        try:
            with open(self.filepath, 'r') as f:
                for line_num, line in enumerate(f, 1):
                    line = line.strip()
                    if not line:
                        continue

                    # Parse format: {eax_in,ebx_in,ecx_in,edx_in}:{eax_out,ebx_out,ecx_out,edx_out}
                    match = re.match(
                        r'\{(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+)\}:'
                        r'\{(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+),(0x[0-9a-fA-F]+)\}',
                        line
                    )

                    if match:
                        eax_in = int(match.group(1), 16)
                        ebx_in = int(match.group(2), 16)
                        ecx_in = int(match.group(3), 16)
                        edx_in = int(match.group(4), 16)
                        eax_out = int(match.group(5), 16)
                        ebx_out = int(match.group(6), 16)
                        ecx_out = int(match.group(7), 16)
                        edx_out = int(match.group(8), 16)

                        key = (eax_in, ebx_in, ecx_in, edx_in)
                        value = (eax_out, ebx_out, ecx_out, edx_out)
                        self.cpuid_data[key] = value
                    else:
                        print(colorize(f"Warning: Could not parse line {line_num}: {line}",
                                      Colors.YELLOW))

            return True
        except (OSError, UnicodeDecodeError, ValueError) as e:
            print(colorize(f"Error reading file: {e}", Colors.RED, Colors.BOLD))
            return False

    def get_cpuid(self, leaf: int, subleaf: int = 0) -> Optional[Tuple[int, int, int, int]]:
        """Get CPUID output for a given leaf and subleaf."""
        # Fast path: SimNow rows are typically emitted with EBX_in=EDX_in=0,
        # so a direct dict lookup catches the common case in O(1).
        direct = self.cpuid_data.get((leaf, 0, subleaf, 0))
        if direct is not None:
            return direct
        # Fall back to a scan for rows that carry non-zero EBX_in/EDX_in,
        # then for any row matching the leaf with a different subleaf.
        for key, value in self.cpuid_data.items():
            if key[0] == leaf and key[2] == subleaf:
                return value
        for key, value in self.cpuid_data.items():
            if key[0] == leaf:
                return value
        return None

    def decode_vendor(self):
        """Decode vendor string from CPUID leaf 0."""
        cpuid0 = self.get_cpuid(0x00000000)
        if cpuid0:
            ebx, ecx, edx = cpuid0[1], cpuid0[2], cpuid0[3]

            # Vendor string is in EBX, EDX, ECX (in that order)
            vendor_bytes = (
                ebx.to_bytes(4, 'little') +
                edx.to_bytes(4, 'little') +
                ecx.to_bytes(4, 'little')
            )
            self.vendor = vendor_bytes.decode('ascii', errors='replace')

            # Identify vendor
            if ebx == 0x68747541 and ecx == 0x444d4163 and edx == 0x69746e65:
                return "AMD"
            elif ebx == 0x756e6547 and ecx == 0x6c65746e and edx == 0x49656e69:
                return "Intel"
            else:
                return self.vendor
        return "Unknown"

    def decode_family_model_stepping(self):
        """Decode family, model, and stepping from CPUID leaf 1."""
        cpuid1 = self.get_cpuid(0x00000001)
        if cpuid1:
            eax = cpuid1[0]

            # Extract fields from EAX
            base_family = (eax >> 8) & 0xF
            ext_family = (eax >> 20) & 0xFF
            base_model = (eax >> 4) & 0xF
            ext_model = (eax >> 16) & 0xF
            self.stepping = eax & 0xF

            # Compute actual family and model
            self.family = base_family + ext_family
            self.model = (ext_model << 4) | base_model

            return True
        return False

    def detect_uarch(self):
        """Detect microarchitecture based on family/model and flags."""
        if self.family in UARCH_DETECTION:
            uarch_info = UARCH_DETECTION[self.family]
            for (model_min, model_max), uarch_name in uarch_info["models"].items():
                if model_min <= self.model <= model_max:
                    # Special case: check for flag-based detection
                    if "CLWB" in uarch_name:
                        if "clwb" in self.present_flags:
                            self.uarch = "Zen2"
                        else:
                            self.uarch = "Zen"
                    elif "AVX512F" in uarch_name:
                        if "avx512f" in self.present_flags:
                            self.uarch = "Zen4"
                        else:
                            self.uarch = "Zen3"
                    elif "AVX512_BMM" in uarch_name:
                        # Mirror X86RawData.hh fallback for unknown 0x1A models:
                        # AVX512_VPINTERSECT -> Zen5, AVX512_BMM -> Zen6, else Zen5
                        if "avx512_vpintersect" in self.present_flags:
                            self.uarch = "Zen5"
                        elif "avx512_bmm" in self.present_flags:
                            self.uarch = "Zen6"
                        else:
                            self.uarch = "Zen5"
                    else:
                        self.uarch = uarch_name
                    return

        # Fallback for future AMD families beyond current Max (0x1A): treat as Zen6.
        # Mirrors X86RawData.hh "Assuming Family increases each generation".
        if self.family > 0x1A:
            self.uarch = f"Zen6 (future family 0x{self.family:02X})"
            return

        # Fallback: Unknown
        self.uarch = f"Unknown (Family 0x{self.family:02X}, Model 0x{self.model:02X})"

    def decode_flags(self):
        """Decode all CPU feature flags."""
        self.present_flags = []
        self.absent_flags = []

        for flag_def in CPUID_FLAGS:
            cpuid_data = self.get_cpuid(flag_def.leaf, flag_def.subleaf)
            if cpuid_data:
                # Get the appropriate register value
                reg_map = {'eax': 0, 'ebx': 1, 'ecx': 2, 'edx': 3}
                reg_value = cpuid_data[reg_map[flag_def.register]]

                # Check if flag is set
                if reg_value & flag_def.bit_mask:
                    self.present_flags.append(flag_def.name)
                else:
                    self.absent_flags.append(flag_def.name)
            else:
                # CPUID leaf not available, flag is absent
                self.absent_flags.append(flag_def.name)

    def decode_all(self):
        """Decode all information."""
        if not self.parse_file():
            return False

        self.decode_vendor()
        self.decode_family_model_stepping()
        self.decode_flags()
        self.detect_uarch()
        return True


def print_header(text: str, width: int = 80):
    """Print a styled header."""
    print()
    print(colorize("═" * width, Colors.CYAN))
    padding = (width - len(text) - 2) // 2
    header_text = f"{'═' * padding} {text} {'═' * (width - padding - len(text) - 2)}"
    print(colorize(header_text, Colors.CYAN, Colors.BOLD))
    print(colorize("═" * width, Colors.CYAN))


def print_section(title: str, width: int = 80):
    """Print a section divider."""
    print()
    print(colorize(f"─── {title} ", Colors.BRIGHT_BLUE, Colors.BOLD) +
          colorize("─" * (width - len(title) - 5), Colors.BRIGHT_BLACK))


_ANSI_KEY_PADDING = 12  # Compensates the format-spec width for invisible
                        # ANSI escape sequences wrapping the colorized key.


def print_key_value(key: str, value: str, key_width: int = 20):
    """Print a key-value pair."""
    key_str = colorize(f"  {key}:", Colors.YELLOW)
    print(f"{key_str:<{key_width + _ANSI_KEY_PADDING}} {value}")


def print_flags_grid(flags: List[str], present: bool, columns: int = 6):
    """Print flags in a grid layout."""
    if not flags:
        status = colorize("  (none)", Colors.DIM)
        print(status)
        return

    # Sort flags
    sorted_flags = sorted(flags)

    # Calculate column width
    max_len = max(len(f) for f in sorted_flags) if sorted_flags else 10
    col_width = max_len + 2

    # Print in grid
    for i, flag in enumerate(sorted_flags):
        if present:
            flag_str = colorize(f"✓ {flag}", Colors.GREEN)
        else:
            flag_str = colorize(f"✗ {flag}", Colors.RED, Colors.DIM)

        print(f"  {flag_str:<{col_width + 10}}", end="")

        if (i + 1) % columns == 0:
            print()

    # Final newline if needed
    if len(sorted_flags) % columns != 0:
        print()


def print_raw_cpuid(cpuid_data: Dict[Tuple[int, int, int, int], Tuple[int, int, int, int]]):
    """Print raw CPUID data in a formatted table."""
    print()
    header = colorize(
        f"  {'LEAF':<12} {'SUBLEAF':<10} {'EAX':<12} {'EBX':<12} {'ECX':<12} {'EDX':<12}",
        Colors.BRIGHT_CYAN, Colors.BOLD
    )
    print(header)
    print(colorize("  " + "─" * 72, Colors.BRIGHT_BLACK))

    # Sort by leaf, then subleaf
    sorted_keys = sorted(cpuid_data.keys(), key=lambda x: (x[0], x[2]))

    for key in sorted_keys:
        leaf, _, subleaf, _ = key
        eax, ebx, ecx, edx = cpuid_data[key]

        leaf_str = f"0x{leaf:08X}"
        subleaf_str = f"0x{subleaf:02X}"

        row = f"  {leaf_str:<12} {subleaf_str:<10} "
        row += f"0x{eax:08X}   0x{ebx:08X}   0x{ecx:08X}   0x{edx:08X}"
        print(colorize(row, Colors.WHITE))


def print_x86_levels(decoder: SimNowDecoder):
    """Print x86-64 microarchitecture level compatibility."""
    # x86-64-v2 requirements
    v2_flags = ['cx16', 'lahf_lm', 'popcnt', 'sse4_1', 'sse4_2', 'ssse3']
    v2_support = all(f in decoder.present_flags for f in v2_flags)

    # x86-64-v3 requirements (in addition to v2)
    v3_flags = ['avx', 'avx2', 'bmi1', 'bmi2', 'f16c', 'fma', 'abm', 'movbe', 'xsave']
    v3_support = v2_support and all(f in decoder.present_flags for f in v3_flags)

    # x86-64-v4 requirements (in addition to v3)
    v4_flags = ['avx512f', 'avx512bw', 'avx512cd', 'avx512dq', 'avx512vl']
    v4_support = v3_support and all(f in decoder.present_flags for f in v4_flags)

    print_section("x86-64 Microarchitecture Levels")

    levels = [
        ("x86-64-v2", v2_support, v2_flags),
        ("x86-64-v3", v3_support, v3_flags),
        ("x86-64-v4", v4_support, v4_flags),
    ]

    for level_name, supported, required_flags in levels:
        if supported:
            status = colorize("✓ Supported", Colors.GREEN, Colors.BOLD)
        else:
            status = colorize("✗ Not Supported", Colors.RED)
            missing = [f for f in required_flags if f not in decoder.present_flags]
            if missing:
                status += colorize(f" (missing: {', '.join(missing)})", Colors.DIM)

        print(f"  {colorize(level_name, Colors.YELLOW)}: {status}")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Decode CPUID information from SimNow data files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python scripts/decode_simnow.py -f ./Library/Tests/Cpuid/Mock/simnowdata/EPYC-Genoa-v1/EPYC-Genoa-v1
  python scripts/decode_simnow.py -f ./Library/Tests/Cpuid/Mock/simnowdata/EPYC-Milan-v1/EPYC-Milan-v1 --raw
        """
    )

    parser.add_argument(
        "-f", "--file",
        required=True,
        help="Path to the SimNow CPUID data file"
    )

    parser.add_argument(
        "--raw",
        action="store_true",
        help="Show raw CPUID data table"
    )

    parser.add_argument(
        "--no-color",
        action="store_true",
        help="Disable colored output"
    )

    parser.add_argument(
        "--absent",
        action="store_true",
        help="Show absent (unsupported) flags"
    )

    args = parser.parse_args()

    # Handle color override
    global USE_COLORS
    if args.no_color:
        USE_COLORS = False

    # Create decoder and process file
    decoder = SimNowDecoder(args.file)

    if not decoder.decode_all():
        sys.exit(1)

    # Get filename for display
    filename = os.path.basename(args.file)
    dirname = os.path.basename(os.path.dirname(args.file))

    # Print results
    print_header(f"SimNow CPUID Decoder - {dirname}/{filename}")

    # Processor Information
    print_section("Processor Information")

    vendor_display = decoder.decode_vendor()
    if vendor_display == "AMD":
        vendor_colored = colorize("AMD", Colors.GREEN, Colors.BOLD)
    elif vendor_display == "Intel":
        vendor_colored = colorize("Intel", Colors.BLUE, Colors.BOLD)
    else:
        vendor_colored = colorize(vendor_display, Colors.YELLOW)

    print_key_value("Vendor", f"{vendor_colored} ({decoder.vendor})")
    print_key_value("Family", f"0x{decoder.family:02X} ({decoder.family})")
    print_key_value("Model", f"0x{decoder.model:02X} ({decoder.model})")
    print_key_value("Stepping", f"0x{decoder.stepping:X} ({decoder.stepping})")
    print_key_value("Microarchitecture", colorize(decoder.uarch, Colors.MAGENTA, Colors.BOLD))

    # x86-64 Levels
    print_x86_levels(decoder)

    # Present Flags
    print_section(f"Present Flags ({len(decoder.present_flags)} flags)")
    print_flags_grid(decoder.present_flags, present=True)

    # Absent Flags (optional)
    if args.absent:
        print_section(f"Absent Flags ({len(decoder.absent_flags)} flags)")
        print_flags_grid(decoder.absent_flags, present=False)

    # Raw CPUID data (optional)
    if args.raw:
        print_section("Raw CPUID Data")
        print_raw_cpuid(decoder.cpuid_data)

    # Summary
    print_section("Summary")
    total_flags = len(decoder.present_flags) + len(decoder.absent_flags)
    print(f"  {colorize('Total flags checked:', Colors.YELLOW)} {total_flags}")
    print(f"  {colorize('Present:', Colors.GREEN)} {len(decoder.present_flags)}")
    print(f"  {colorize('Absent:', Colors.RED)} {len(decoder.absent_flags)}")
    print()


if __name__ == "__main__":
    main()
