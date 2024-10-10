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
import os
import re
import sys

def get_files():
    # execute command
    # git diff --cached --name-only
    # store the output

    cmd = "git diff --cached --name-only  | sort -u"
    files = os.popen(cmd).read()
    return files

def get_notice(file):
    # open the file and read the first 5 lines
    # if the line contains the word "Copyright"
    # return the line
    with open(file, 'r') as f:
        for i in range(5):
            line = f.readline()
            if "Copyright" in line:
                return line
    return None

def scan_files(files):
    # for each file in the list
    # check if it has a notice and the year 2024
    # if not, print the file name
    found = False;
    for file in files.split('\n'):
        if os.path.isfile(file):
            notice = get_notice(file)
            if notice is None:
                print(file)
                found = True;
                print("doesnot contain a notice");
            elif "2024" not in notice:
                print(file)
                found = True;
                print("doesnot contain the modified year");
    return found

def main():
    ret = scan_files(get_files())
    sys.exit(ret)

if __name__ == "__main__":
    main()
