#!/bin/python3.11
"""
  Copyright (C) 2024, Advanced Micro Devices. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. Neither the name of the copyright holder nor the names of its contributors
     may be used to endorse or promote products derived from this software
  without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
"""

import sys
import subprocess
import json

def run_qemu(architecture = "x86_64", cpu_model = "EPYC", binary_path ="", test_name =""):
    qemu_cmd = [
            f"qemu-"+ architecture,
            "-cpu", cpu_model,
            binary_path,
            "--gtest_filter=*"+ test_name + "*",
            "--gtest_output=json:result.json",
            "--gtest_also_run_disabled_tests"
            ]
    qemu_process = subprocess.run(qemu_cmd, capture_output=True, timeout = 1)
    if qemu_process.stdout :
       print(qemu_process.stdout.decode())
       result  = open('result.json')
       data = json.load(result)
       return False if data["failures"] else True
    else:
        print("Test failed due to infrastructure error")
        return False

if __name__ == "__main__":
    if len(sys.argv) >= 3:
        cpu_model, test_name = sys.argv[1], sys.argv[2]
        print("SUCCESS" if run_qemu(cpu_model=cpu_model,binary_path
                                    ="./Release/core_CpuidTest", test_name=test_name) else "FAILURE")
    else:
        print("Usage: python script.py  <cpu_model> <test_name> from the build folder")

