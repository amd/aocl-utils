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
        pass


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
        pass


def stop_docker_container(container_name):
    cmd = "docker stop " + container_name
    run_cmd(cmd)


def remove_docker_container(container_name):
    cmd = "docker rm " + container_name
    run_cmd(cmd)


def remove_docker_image(image_name):
    cmd = "docker rmi " + image_name
    run_cmd(cmd)


def show_progress_bar():
    for _ in tqdm(range(100), desc="Running sanity test"):
        time.sleep(.8)  # Update the progress bar every second

def main():
    dockerfile = "Tests/Dockerfile"
    C_COMPILER = ["gcc-9", "gcc-10", "gcc-11", "gcc-12", "gcc-13", "gcc-14",
                  "clang-12", "clang-13", "clang-14", "clang-15", "clang-16", "clang-17"]
    C_COMPILER = ["clang-14", "clang-15", "clang-16", "clang-17"]
    CXX_COMPILER = ["clang++-14", "clang++-15", "clang++-16", "clang++-17"]
    for c_compiler, cxx_compiler in zip(C_COMPILER, CXX_COMPILER):
        print("C Compiler: " + c_compiler)
        print("CXX Compiler: " + cxx_compiler)
        image_name = "build_sanity_test_" + c_compiler
        create_docker_image(image_name, dockerfile, c_compiler, cxx_compiler)
        # RUN DOCKER CONTAINER FOR THE LIST OF CMAKE VERSIONS
        CMAKE_VERSIONS = ["3.22", "3.23", "3.24", "3.25",
                          "3.26", "3.27", "3.28", "3.29", "3.30"]
        for cmake_version in CMAKE_VERSIONS:
            container_name = image_name + "_" + cmake_version
            cmd = "-e CMAKE_VERSION=" + cmake_version + \
                  " -e C_COMPILER=" + c_compiler + " -e CXX_COMPILER=" + cxx_compiler
            #run_docker_container(image_name, container_name, cmd)
            process_thread = threading.Thread(target=run_docker_container, args=(image_name, container_name, cmd))
            progress_thread = threading.Thread(target=show_progress_bar)
            process_thread.start()
            progress_thread.start()
            process_thread.join()
            progress_thread.join()
            # Get the container status
            status = get_docker_container_status(container_name)
            print("Container status: " + status.decode('utf-8'))

            # Get the container exit code
            exit_code = get_docker_container_exit_code(container_name)
            print("Container exit code: " + exit_code.decode('utf-8'))
            if exit_code.decode('utf-8').strip() != "0":
                print("Sanity test failed")
            else:
                print("Sanity test passed")
            print("Cleaning up...")
            print("Stopping container...")
            stop_docker_container(container_name)
            print("Removing container...")
            remove_docker_container(container_name)

        run_cmd(cmd)
        print("Removing image...")
        remove_docker_image(image_name)

if __name__ == "__main__":
    main()
