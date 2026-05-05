#!/usr/bin/env python3
# Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
"""
SimNow CPUID Data Generator

This script reads actual CPUID data from the current machine and generates
a SimNow-compatible data file for use in the aocl-utils mock testing framework.

Usage:
    python scripts/generate_simnow.py -o ./output_cpuid_data
    python scripts/generate_simnow.py -o ./Library/Tests/Cpuid/Mock/simnowdata/MyMachine/MyMachine

Security note:
    The Linux/macOS path allocates an executable mmap region (PROT_EXEC) and
    the Windows path uses VirtualAlloc with PAGE_EXECUTE_READWRITE so that
    a small piece of x86-64 machine code (CPUID + 4 register stores + RET)
    can be invoked via ctypes. This is required to read raw CPUID without
    a kernel module. Run only in trusted developer environments; some
    hardened systems (W^X enforcement, SELinux, exec-mem guards) will block
    the executable mapping.

Author: AOCL-Utils Team
"""

import argparse
import ctypes
import ctypes.util
import mmap
import os
import platform
import struct
import sys
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

# Number of additional characters that ANSI escape sequences add to a
# colorized key label (one wrapping pair: "\033[33m" + "\033[0m" = 4 + 8 = 12).
# Used to compensate the format-spec width when aligning colorized columns.
_ANSI_KEY_PADDING = 12

# ANSI Color codes for terminal output
class Colors:
    """ANSI color codes for terminal output."""
    RESET = "\033[0m"
    BOLD = "\033[1m"
    DIM = "\033[2m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    CYAN = "\033[36m"
    RED = "\033[31m"
    BRIGHT_BLUE = "\033[94m"
    BRIGHT_BLACK = "\033[90m"


def supports_color() -> bool:
    """Check if the terminal supports colors."""
    if not sys.stdout.isatty():
        return False
    if os.environ.get("NO_COLOR"):
        return False
    if os.environ.get("TERM") == "dumb":
        return False
    return True


USE_COLORS = supports_color()


def colorize(text: str, *colors: str) -> str:
    """Apply colors to text if colors are enabled."""
    if not USE_COLORS:
        return text
    color_str = "".join(colors)
    return f"{color_str}{text}{Colors.RESET}"


@dataclass
class CpuidResult:
    """Result of a CPUID instruction."""
    eax: int
    ebx: int
    ecx: int
    edx: int


class CpuidExecutor:
    """
    Executes CPUID instruction on x86/x86-64 systems.

    Uses ctypes with mmap to create executable memory containing
    CPUID shellcode.
    """

    def __init__(self):
        self._setup_cpuid()

    def _setup_cpuid(self):
        """Set up the CPUID execution method."""
        # The shellcode in both _setup_mmap_method and _setup_windows_method is
        # x86-64 only. Refuse early on any other architecture so we don't
        # SIGILL or, worse, silently emit zero-filled CPUID dumps.
        machine = platform.machine().lower()
        if machine not in ('x86_64', 'amd64'):
            raise RuntimeError(
                f"generate_simnow.py only supports x86_64/AMD64; got '{machine}'"
            )

        # Try different methods in order of preference

        # Method 1: Use ctypes with inline assembly via mmap (Linux/macOS)
        if sys.platform in ('linux', 'linux2', 'darwin'):
            try:
                self._setup_mmap_method()
                self._method = 'mmap'
                return
            except Exception as e:
                print(colorize(f"Warning: mmap method failed: {e}", Colors.YELLOW))

        # Method 2: Try Windows-specific method
        if sys.platform == 'win32':
            try:
                self._setup_windows_method()
                self._method = 'windows'
                return
            except Exception as e:
                print(colorize(f"Warning: Windows method failed: {e}", Colors.YELLOW))

        # No working method - bail out instead of silently producing a
        # zero-filled SimNow file that the mock harness would accept as
        # legitimate CPU data.
        raise RuntimeError(
            "Could not establish a working CPUID execution method on this "
            "platform (sys.platform=" + sys.platform + ")."
        )

    def _setup_mmap_method(self):
        """Set up CPUID execution using mmap for executable memory."""
        # CPUID shellcode for x86-64:
        # mov eax, [rdi]       ; eax = leaf (first argument on Linux x86-64)
        # mov ecx, [rdi+4]     ; ecx = subleaf
        # cpuid
        # mov [rsi], eax       ; store results
        # mov [rsi+4], ebx
        # mov [rsi+8], ecx
        # mov [rsi+12], edx
        # ret

        # x86-64 Linux calling convention: rdi = first arg (input), rsi = second arg (output)
        shellcode_x64 = bytes([
            0x8b, 0x07,                     # mov eax, [rdi]
            0x8b, 0x4f, 0x04,               # mov ecx, [rdi+4]
            0x0f, 0xa2,                     # cpuid
            0x89, 0x06,                     # mov [rsi], eax
            0x89, 0x5e, 0x04,               # mov [rsi+4], ebx
            0x89, 0x4e, 0x08,               # mov [rsi+8], ecx
            0x89, 0x56, 0x0c,               # mov [rsi+12], edx
            0xc3,                           # ret
        ])

        # Allocate executable memory.
        # MAP_ANONYMOUS exists on Linux; macOS / *BSDs spell it MAP_ANON.
        if hasattr(mmap, 'MAP_ANONYMOUS'):
            anon_flag = mmap.MAP_ANONYMOUS
        elif hasattr(mmap, 'MAP_ANON'):
            anon_flag = mmap.MAP_ANON
        else:
            raise RuntimeError("Anonymous mmap not supported on this platform")

        self._code_size = len(shellcode_x64)
        self._code_buffer = mmap.mmap(
            -1,  # anonymous mapping
            self._code_size,
            prot=mmap.PROT_READ | mmap.PROT_WRITE | mmap.PROT_EXEC,
            flags=mmap.MAP_PRIVATE | anon_flag
        )

        # Copy shellcode to executable memory
        self._code_buffer.write(shellcode_x64)
        self._code_buffer.seek(0)

        # Create ctypes function pointer
        # Define function type: void cpuid(uint32_t* input, uint32_t* output)
        CPUID_FUNC = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.c_uint32))

        # Get address of the executable buffer
        buf_addr = ctypes.addressof(ctypes.c_char.from_buffer(self._code_buffer))
        self._cpuid_func = CPUID_FUNC(buf_addr)

    def _setup_windows_method(self):
        """Set up CPUID execution for Windows."""
        # On Windows, we can use kernel32 VirtualAlloc with executable permissions
        kernel32 = ctypes.windll.kernel32

        # CPUID shellcode for x86-64 Windows:
        # Windows x64 calling convention: rcx = first arg, rdx = second arg
        shellcode_x64 = bytes([
            0x8b, 0x01,                     # mov eax, [rcx]
            0x4c, 0x8b, 0xc2,               # mov r8, rdx
            0x8b, 0x49, 0x04,               # mov ecx, [rcx+4]
            0x0f, 0xa2,                     # cpuid
            0x41, 0x89, 0x00,               # mov [r8], eax
            0x41, 0x89, 0x58, 0x04,         # mov [r8+4], ebx
            0x41, 0x89, 0x48, 0x08,         # mov [r8+8], ecx
            0x41, 0x89, 0x50, 0x0c,         # mov [r8+12], edx
            0xc3,                           # ret
        ])

        # VirtualAlloc with PAGE_EXECUTE_READWRITE
        MEM_COMMIT = 0x1000
        MEM_RESERVE = 0x2000
        PAGE_EXECUTE_READWRITE = 0x40

        self._code_buffer = kernel32.VirtualAlloc(
            None,
            len(shellcode_x64),
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE
        )

        if not self._code_buffer:
            raise OSError("VirtualAlloc failed")

        # Copy shellcode
        ctypes.memmove(self._code_buffer, shellcode_x64, len(shellcode_x64))

        # Create function pointer
        CPUID_FUNC = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.c_uint32))
        self._cpuid_func = CPUID_FUNC(self._code_buffer)

    def cpuid(self, leaf: int, subleaf: int = 0) -> CpuidResult:
        """
        Execute CPUID instruction with given leaf and subleaf.

        Args:
            leaf: CPUID leaf (EAX input)
            subleaf: CPUID subleaf (ECX input)

        Returns:
            CpuidResult with EAX, EBX, ECX, EDX values
        """
        # Create input/output arrays
        input_arr = (ctypes.c_uint32 * 2)(leaf, subleaf)
        output_arr = (ctypes.c_uint32 * 4)(0, 0, 0, 0)

        # Execute CPUID
        self._cpuid_func(input_arr, output_arr)

        return CpuidResult(
            eax=output_arr[0],
            ebx=output_arr[1],
            ecx=output_arr[2],
            edx=output_arr[3]
        )

    def __del__(self):
        """Clean up allocated memory.

        Guarded against partially-initialised instances: if _setup_cpuid
        raises before _method or _code_buffer exists (e.g. unsupported
        architecture), the destructor must not itself raise AttributeError.
        """
        if not hasattr(self, '_method'):
            return
        if not getattr(self, '_code_buffer', None):
            return
        if sys.platform == 'win32':
            kernel32 = ctypes.windll.kernel32
            MEM_RELEASE = 0x8000
            kernel32.VirtualFree(self._code_buffer, 0, MEM_RELEASE)
        else:
            self._code_buffer.close()


class SimNowGenerator:
    """Generates SimNow-compatible CPUID data files."""

    # CPUID leaves to query (based on X86RawData.cc)
    CPUID_LEAVES = [
        # Standard leaves
        (0x00000000, 0),  # Vendor string, max standard leaf
        (0x00000001, 0),  # Feature identifiers
        (0x00000006, 0),  # Thermal and power management
        (0x00000007, 0),  # Structured extended feature flags
        (0x00000007, 1),  # Structured extended feature flags subleaf 1
        (0x0000000D, 0),  # XSAVE features
        (0x0000000D, 1),  # XSAVE features subleaf 1

        # Extended leaves (AMD)
        (0x80000000, 0),  # Max extended leaf
        (0x80000001, 0),  # Extended feature identifiers
        (0x80000007, 0),  # Advanced power management
        (0x80000008, 0),  # Address sizes, physical cores
        (0x8000000A, 0),  # SVM features
        (0x8000001D, 0),  # Cache topology (L1 data)
        (0x8000001D, 1),  # Cache topology (L1 instruction)
        (0x8000001D, 2),  # Cache topology (L2)
        (0x8000001D, 3),  # Cache topology (L3)
        (0x80000021, 0),  # AMD Extended Feature Identification 2 (AVX512_BMM, ...)

        # VIA/Centaur leaves
        (0xC0000000, 0),  # Max Centaur leaf
        (0xC0000001, 0),  # Centaur features
    ]

    def __init__(self):
        self.cpuid_executor = CpuidExecutor()
        self.cpuid_data: Dict[Tuple[int, int], CpuidResult] = {}
        self.vendor = "Unknown"
        self.family = 0
        self.model = 0
        self.stepping = 0

    def collect_cpuid_data(self):
        """Collect CPUID data from the current processor."""
        print(colorize("\nCollecting CPUID data from current processor...", Colors.CYAN, Colors.BOLD))
        print()

        for leaf, subleaf in self.CPUID_LEAVES:
            result = self.cpuid_executor.cpuid(leaf, subleaf)
            self.cpuid_data[(leaf, subleaf)] = result

            # Print progress
            leaf_str = f"0x{leaf:08X}"
            subleaf_str = f"0x{subleaf:02X}"
            print(colorize(f"  Queried {leaf_str} subleaf {subleaf_str}: ", Colors.DIM) +
                  f"EAX=0x{result.eax:08X} EBX=0x{result.ebx:08X} " +
                  f"ECX=0x{result.ecx:08X} EDX=0x{result.edx:08X}")

        # Decode processor info
        self._decode_processor_info()

    def _decode_processor_info(self):
        """Decode vendor, family, model, stepping from collected data."""
        # Vendor from leaf 0
        if (0x00000000, 0) in self.cpuid_data:
            result = self.cpuid_data[(0x00000000, 0)]
            vendor_bytes = (
                result.ebx.to_bytes(4, 'little') +
                result.edx.to_bytes(4, 'little') +
                result.ecx.to_bytes(4, 'little')
            )
            self.vendor = vendor_bytes.decode('ascii', errors='replace')

        # Family, Model, Stepping from leaf 1
        if (0x00000001, 0) in self.cpuid_data:
            result = self.cpuid_data[(0x00000001, 0)]
            eax = result.eax

            base_family = (eax >> 8) & 0xF
            ext_family = (eax >> 20) & 0xFF
            base_model = (eax >> 4) & 0xF
            ext_model = (eax >> 16) & 0xF
            self.stepping = eax & 0xF

            self.family = base_family + ext_family
            self.model = (ext_model << 4) | base_model

    def generate_simnow_file(self, output_path: str, as_directory: bool = False) -> Tuple[bool, str]:
        """
        Generate SimNow-compatible data file.

        Args:
            output_path: Path to output file or directory
            as_directory: If True, create a directory and put data file inside with same name

        Returns:
            Tuple of (success, actual_file_path)
        """
        try:
            if as_directory:
                # Create directory and put file inside with same name as directory
                output_dir = output_path
                dir_name = os.path.basename(output_path)
                actual_file_path = os.path.join(output_dir, dir_name)

                if not os.path.exists(output_dir):
                    os.makedirs(output_dir)
            else:
                # Create parent directory if needed
                output_dir = os.path.dirname(output_path)
                if output_dir and not os.path.exists(output_dir):
                    os.makedirs(output_dir)
                actual_file_path = output_path

            with open(actual_file_path, 'w') as f:
                for (leaf, subleaf), result in sorted(self.cpuid_data.items()):
                    # Format: {eax_in,ebx_in,ecx_in,edx_in}:{eax_out,ebx_out,ecx_out,edx_out}
                    line = (
                        f"{{0x{leaf:x},0x0,0x{subleaf:x},0x0}}:"
                        f"{{0x{result.eax:x},0x{result.ebx:x},0x{result.ecx:x},0x{result.edx:x}}}\n"
                    )
                    f.write(line)

            return True, actual_file_path
        except Exception as e:
            print(colorize(f"Error writing file: {e}", Colors.RED, Colors.BOLD))
            return False, ""

    def generate_flags_files(self, output_dir: str) -> Tuple[bool, List[str]]:
        """
        Generate FlagsT.txt and FlagsF.txt files.

        Args:
            output_dir: Directory to output files

        Returns:
            Tuple of (success, list of created file paths)
        """
        # Import flag definitions from decode_simnow
        try:
            # Get the directory of this script
            script_dir = os.path.dirname(os.path.abspath(__file__))

            # Try to import from decode_simnow
            import importlib.util
            spec = importlib.util.spec_from_file_location(
                "decode_simnow",
                os.path.join(script_dir, "decode_simnow.py")
            )
            decode_module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(decode_module)

            CPUID_FLAGS = decode_module.CPUID_FLAGS
        except Exception as e:
            print(colorize(f"Warning: Could not import flag definitions: {e}", Colors.YELLOW))
            return False, []

        present_flags = []
        absent_flags = []

        for flag_def in CPUID_FLAGS:
            key = (flag_def.leaf, flag_def.subleaf)
            if key in self.cpuid_data:
                result = self.cpuid_data[key]

                # CpuidResult fields are named eax/ebx/ecx/edx, matching the
                # register name in flag_def.register exactly.
                reg_value = getattr(result, flag_def.register)

                # Check if flag is set
                if reg_value & flag_def.bit_mask:
                    present_flags.append(flag_def.name)
                else:
                    absent_flags.append(flag_def.name)
            else:
                absent_flags.append(flag_def.name)

        try:
            # Create directory if needed
            if not os.path.exists(output_dir):
                os.makedirs(output_dir)

            created_files = []

            # Write FlagsT.txt (present flags)
            flags_t_path = os.path.join(output_dir, "FlagsT.txt")
            with open(flags_t_path, 'w') as f:
                for flag in sorted(present_flags):
                    f.write(f"{flag}\n")
            created_files.append(flags_t_path)

            # Write FlagsF.txt (absent flags)
            flags_f_path = os.path.join(output_dir, "FlagsF.txt")
            with open(flags_f_path, 'w') as f:
                for flag in sorted(absent_flags):
                    f.write(f"{flag}\n")
            created_files.append(flags_f_path)

            return True, created_files
        except Exception as e:
            print(colorize(f"Error writing flags files: {e}", Colors.RED, Colors.BOLD))
            return False, []


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


def print_key_value(key: str, value: str, key_width: int = 20):
    """Print a key-value pair."""
    key_str = colorize(f"  {key}:", Colors.YELLOW)
    print(f"{key_str:<{key_width + _ANSI_KEY_PADDING}} {value}")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Generate SimNow-compatible CPUID data from current processor",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python scripts/generate_simnow.py -o ./my_cpu_data
  python scripts/generate_simnow.py -o ./Library/Tests/Cpuid/Mock/simnowdata/MyMachine/MyMachine
  python scripts/generate_simnow.py -o ./output --with-flags
        """
    )

    parser.add_argument(
        "-o", "--output",
        required=True,
        help="Path to output file (SimNow data format)"
    )

    parser.add_argument(
        "--with-flags",
        action="store_true",
        help="Also generate FlagsT.txt and FlagsF.txt in the same directory"
    )

    parser.add_argument(
        "--no-color",
        action="store_true",
        help="Disable colored output"
    )

    args = parser.parse_args()

    # Handle color override
    global USE_COLORS
    if args.no_color:
        USE_COLORS = False

    # Print header
    print_header("SimNow CPUID Data Generator")

    # Create generator and collect data
    generator = SimNowGenerator()
    generator.collect_cpuid_data()

    # Print processor info
    print_section("Detected Processor")
    print_key_value("Vendor", generator.vendor)
    print_key_value("Family", f"0x{generator.family:02X} ({generator.family})")
    print_key_value("Model", f"0x{generator.model:02X} ({generator.model})")
    print_key_value("Stepping", f"0x{generator.stepping:X} ({generator.stepping})")

    # Generate output file
    print_section("Generating Output Files")

    # If --with-flags, create output as a directory with data file inside
    as_directory = args.with_flags
    success, actual_file_path = generator.generate_simnow_file(args.output, as_directory=as_directory)

    if success:
        print(colorize(f"  ✓ Created: {actual_file_path}", Colors.GREEN))
    else:
        print(colorize(f"  ✗ Failed to create: {args.output}", Colors.RED))
        sys.exit(1)

    # Generate flags files if requested
    if args.with_flags:
        output_dir = args.output  # Now args.output is the directory

        success, created_files = generator.generate_flags_files(output_dir)
        if success:
            for f in created_files:
                print(colorize(f"  ✓ Created: {f}", Colors.GREEN))
        else:
            print(colorize(f"  ✗ Failed to create flags files", Colors.RED))

    # Summary
    print_section("Summary")
    print(f"  {colorize('CPUID leaves collected:', Colors.YELLOW)} {len(generator.cpuid_data)}")
    print(f"  {colorize('Output path:', Colors.GREEN)} {args.output}")
    print()
    print(colorize("  Use decode_simnow.py to verify the generated data:", Colors.DIM))
    print(colorize(f"  python scripts/decode_simnow.py -f {actual_file_path}", Colors.DIM))
    print()


if __name__ == "__main__":
    main()
