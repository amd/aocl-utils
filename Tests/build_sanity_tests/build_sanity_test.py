#
# Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
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

# python script to run sanity test on build
# Creat docker image, run docker container, run sanity test
# and clean up the docker container

import os
import sys
import subprocess
import time
import re
from tqdm import tqdm
import threading

def run_cmd(cmd):
    try:
        output = subprocess.check_output(cmd, shell=True)
        return output
    except subprocess.CalledProcessError as e:
        print("Error: " + e.output.decode('utf-8'))
        pass

def get_docker_container_status(container_name):
    cmd = "docker inspect --format='{{.State.Status}}' " + container_name
    status = run_cmd(cmd)
    return status


def get_docker_container_exit_code(container_name):
    cmd = "docker inspect --format='{{.State.ExitCode}}' " + container_name
    exit_code = run_cmd(cmd)
    return exit_code


def get_docker_container_logs(container_name):
    cmd = "docker logs " + container_name
    print("Running command: " + cmd)
    logs = run_cmd(cmd)
    return logs


def create_docker_image(image_name, dockerfile, c_compiler, cxx_compiler):
    cmd = "docker buildx build -t " + image_name + " -f " \
          + dockerfile + " .  --network=host --build-arg C_COMPILER=" + c_compiler + \
          " --build-arg CXX_COMPILER=" + cxx_compiler + \
        " --build-arg CXX_COMPILER=" + cxx_compiler
    print("Running command: " + cmd)
    try:
        run_cmd(cmd)
    except subprocess.CalledProcessError as e:
        print("Error: " + e.output.decode('utf-8'))
        remove_docker_image(image_name)
        sys.exit(1)


def run_docker_container(image_name, container_name, cmd):
    cmd = "docker run -e WORKDIR=/usr/src/app " + cmd + \
          " --mount src=" + os.curdir + ",target=/usr/src/app,type=bind --network=host -t" + \
          " --name " + container_name + " " + image_name
    print("Running command: " + cmd)
    try:
        run_cmd(cmd)
    # catch the exception if the command fails
    except subprocess.CalledProcessError as e:
        print("Error: " + e.output.decode('utf-8'))
        remove_docker_container(container_name)
        sys.exit(1)


def stop_docker_container(container_name):
    cmd = "docker stop " + container_name
    run_cmd(cmd)


def remove_docker_container(container_name):
    cmd = "docker rm " + container_name
    run_cmd(cmd)


def remove_docker_image(image_name):
    cmd = "docker rmi " + image_name
    run_cmd(cmd)


def test_cmake_version(image_name, c_compiler, cxx_compiler, cmake_version):
    container_name = image_name + "_" + cmake_version
    cmd = "-e CMAKE_VERSION=" + cmake_version + \
          " -e C_COMPILER=" + c_compiler + " -e CXX_COMPILER=" + cxx_compiler
    run_docker_container(image_name, container_name, cmd)
    # Get the container status
    status = get_docker_container_status(container_name)
    print("Container status: " + status.decode('utf-8'))
    # Get the container exit code
    exit_code = get_docker_container_exit_code(container_name)
    print("Container exit code: " + exit_code.decode('utf-8'))
    return_code = 0
    if exit_code.decode('utf-8').strip() != "0":
        print("Sanity test failed")
        return_code = 1
    else:
        print("Sanity test passed")
    print("COmbination: " + c_compiler + " " +
          cxx_compiler + " " + cmake_version)
    print("Cleaning up...")
    print("Stopping container...")
    stop_docker_container(container_name)
    print("Removing container...")
    remove_docker_container(container_name)
    sys.exit(return_code)


def main():
    dockerfile = "Tests/Dockerfile"
    if len(sys.argv) != 4:
        print("Usage: script.py arg1 arg2 arg3")
        sys.exit(1)

    c_compiler = sys.argv[1]
    cxx_compiler = sys.argv[2]
    cmake_version = sys.argv[3]
    print("C Compiler: " + c_compiler)
    print("CXX Compiler: " + cxx_compiler)
    image_name = "build_sanity_test_" + c_compiler
    create_docker_image(image_name, dockerfile, c_compiler, cxx_compiler)
    test_cmake_version(image_name, c_compiler, cxx_compiler, cmake_version)
    time.sleep(5)
    # print("Removing image...")
    # remove_docker_image(image_name)
    sys.exit(0)

if __name__ == "__main__":
    main()
