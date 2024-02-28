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

----------------------------
Doxygen document generation:
----------------------------
Tools needed: Doxygen 1.9.6 or above
Include "-DALCI_DOXYGEN_DOC=ON" along with above commands during compilation.

Example:
cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -T ClangCl -DALCI_DOXYGEN_DOC=ON

After successful command execution, document will be generated in HTML format.
Open docs/doxygen/html/index.html in any browser to view documentation.

CMake will use the existing Doxygen if already installed. Else user need to install Doxygen and try again.

NOTES:
1. cmake flag "CMAKE_INSTALL_PREFIX" can be used to avoid lib installation into default path and to install in a user specified path.
   cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -DCMAKE_INSTALL_PREFIX="custom_path" -T ClangCl
2. In order to use Clang v16, Need to install MS Visual Studio 2019 and below plugin:
   LLVM 2019 plugin: https://marketplace.visualstudio.com/items?itemName=MarekAniola.mangh-llvm2019
   Change the command as follows:
   cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -T LLVM
3. Few warnings are expected in Windows build with Clang.
4. If BUILD_SHARED_LIBS is OFF, then only static library binary is generated.
   By default, BUILD_SHARED_LIBS is ON and both static, shared library binaries are generated.
   Example: cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=OFF -B build -T ClangCl
5. Use -j option along with make/cmake build command to use multiple processes parallely to build the library.
   Example:In linux, use "make -j" and in Windows, use "cmake --build ./build --config Release -j"
