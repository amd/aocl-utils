#!/bin/python3.11
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
    print(qemu_cmd)
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

