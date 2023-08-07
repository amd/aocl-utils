# Build AOCL-Utils

------------
Linux build:
------------
With GCC:
---------
Tools needed: CMAKE v3.15 or above, GCC v12 or above.

$ mkdir build
$ cd build
$ cmake .. (installs to default path)

  [cmake flag "-DCMAKE_INSTALL_PREFIX" can be used to avoid lib installation into defalut path(/home/usr/)
  and to install in a user specified path.
  
  usage: cmake -DCMAKE_INSTALL_PREFIX=<custom path> ..(to install library into custom path) ]

$ cmake -DCMAKE_INSTALL_PREFIX=<custom path> ..(to install library into custom path)

$ make          (without installing the library)
  or
  make install  (to install library)

Static library binary: libaoclutils.a
Shared/Dynamic library binary: libaoclutils.so

NOTES:
If multiple GCC versions are installed in same system, please use below command to perform build with default compilers:
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
(or)
you can export the compilers path (to be used) in CC, CXX variables before performing cmake command.
Example: export CC=/usr/bin/gcc; export CXX=/usr/bin/g++;

With AOCC:
----------
Tools needed: CMAKE v3.15  or above, AOCC v4.0
Install AOCC 4.0, go to that path and use below commands:
$ cd /home/amd/aocc-compiler-4.0.0
$ ./install.sh
$ source /home/amd/setenv_AOCC.sh
$ export CC=clang
$ export CXX=clang++

And follow the same above steps as per GCC build.

Enable testing:
---------------
This enables test suites present in library.
$ cmake .. -DALCI_TESTS=ON
$ make or make install
$ ./test_cAoclUtils ; which uses C wrappers of library.
  or
$ ./test_cppAoclUtils ; which uses C++ implementation of library.

Enable examples:
----------------
This enables examples present in library.
$ cmake .. -DALCI_EXAMPLES=ON
$ make or make install

Note: examples section will be deleted once all test cases are implemented under tests (C/C++).

--------------
Windows build:
--------------
Tools needed: CMAKE v3.15 or above, LLVM(Atleast version 15), MS Visual Studio community 2019/2022, Clang v15 or above.

With Clang:
1. cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -T ClangCl
2. cmake --build .\build --config Release
3. cmake --install .\build --config Release

Static library binary: libaoclutils_static.lib
Shared/Dynamic library binary: libaoclutils.lib, libaoclutils.dll

NOTES:
1. cmake flag "CMAKE_INSTALL_PREFIX" can be used to avoid lib installation into default path and to install in a user specified path.
   cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -DCMAKE_INSTALL_PREFIX="custom_path" -T ClangCl
2. In order to use Clang v16, Need to install MS Visual Studio 2019 and below plugin:
   LLVM 2019 plugin: https://marketplace.visualstudio.com/items?itemName=MarekAniola.mangh-llvm2019
   Change the command as follows:
   cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -T LLVM
3. Few warnings are expected in Windows build with Clang.
4. In order to enable testing, need to add -DALCI_TESTS=ON along with cmake cmd above.
   Example: cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -DALCI_TESTS=ON -B build -T ClangCl
5. In order to enable examples, need to add -DALCI_EXAMPLES=ON along with cmake cmd above.
   Example: cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -DALCI_EXAMPLES=ON -B build -T ClangCl