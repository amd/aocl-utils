# python script to run sanity test on build
# Creat docker image, run docker container, run sanity test
# and clean up the docker container

import os
import sys
import subprocess
import time
import re


def run_cmd(cmd):
    print("Running command: " + cmd)
    try:
        output = subprocess.check_output(cmd, shell=True)
        print("Output: " + output.decode('utf-8') )
        return output
    except subprocess.CalledProcessError as e:
        print("Error: " + e.output.decode('utf-8'))
        pass
def get_docker_image_id(image_name):
    cmd = "docker images | grep " + image_name + " | awk '{print $3}'"
    image_id = run_cmd(cmd)
    return image_id

def get_docker_container_id(container_name):
    cmd = "docker ps -a | grep " + container_name + " | awk '{print $1}'"
    container_id = run_cmd(cmd)
    return container_id

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
    logs = run_cmd(cmd)
    return logs

def create_docker_image(image_name, dockerfile, c_compiler, cxx_compiler):
    cmd = "docker buildx build -t " + image_name + " -f " \
          + dockerfile + " .  --network=host --build-arg C_COMPILER=" + c_compiler + \
          " --build-arg CXX_COMPILER=" + cxx_compiler + " --build-arg CXX_COMPILER=" + cxx_compiler
    try:
        run_cmd(cmd)
    except subprocess.CalledProcessError as e:
        print("Error: " + e.output.decode('utf-8'))
        remove_docker_image(image_name)
        pass
def run_docker_container(image_name, container_name, cmd):
    cmd = "docker run -e WORKDIR=/usr/src/app " + cmd + \
          " --mount src=" + os.curdir +",target=/usr/src/app,type=bind --network=host -t" + \
          " --name " + container_name + " " + image_name
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

def main():
    dockerfile = "Tests/Dockerfile"
    C_COMPILER = ["gcc-9", "gcc-10", "gcc-11", "gcc-12", "gcc-13", "gcc-14", "clang-12", "clang-13", "clang-14", "clang-15", "clang-16", "clang-17"]
    CXX_COMPILER = ["g++-9", "g++-10", "g++-11", "g++-12", "g++-13", "g++-14", "clang++-12", "clang++-13", "clang++-14", "clang++-15", "clang++-16", "clang++-17"]
    for c_compiler, cxx_compiler in zip(C_COMPILER, CXX_COMPILER):
        print("C Compiler: " + c_compiler)
        print("CXX Compiler: " + cxx_compiler)
        image_name = "build_sanity_test_" + c_compiler
        create_docker_image(image_name, dockerfile, c_compiler, cxx_compiler)
        # RUN DOCKER CONTAINER FOR THE LIST OF CMAKE VERSIONS
        CMAKE_VERSIONS = ["3.22", "3.23", "3.24", "3.25", "3.26", "3.27", "3.28", "3.29", "3.30"]
        for cmake_version in CMAKE_VERSIONS:
            container_name = image_name +"_" + cmake_version
            cmd = "-e CMAKE_VERSION=" + cmake_version + \
                  " -e C_COMPILER=" + c_compiler + " -e CXX_COMPILER=" + cxx_compiler
            run_docker_container(image_name, container_name, cmd)

            # Wait for the container to finish
            time.sleep(5)

            # Get the container status
            status = get_docker_container_status(container_name)
            print("Container status: " + status.decode('utf-8'))

            # Get the container exit code
            exit_code = get_docker_container_exit_code(container_name)
            print("Container exit code: " + exit_code.decode('utf-8'))

            # Get the container logs
            logs = get_docker_container_logs(container_name)
            print("Container logs: " + logs.decode('utf-8'))

            stop_docker_container(container_name)
            remove_docker_container(container_name)
            if exit_code.decode() != "0":
                print("Sanity test failed")
            else:
                print("Sanity test passed")

        remove_docker_image(image_name)

if __name__ == "__main__":
    main()
