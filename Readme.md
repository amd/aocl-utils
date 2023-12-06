# Project name

  AOCL-UTILS : An effort to factor out common functionality across libraries.

# Project structure

The project is structured as follows:

* `Library`: This directory contains the source code for the project.
* `SDK`: The release folder.
* `Tests`: This directory contains the necessary unit tests for the project.
* `Tools`: The necessary tools to work with the project.

# Dependencies

The project depends on the following libraries:

* CMake 3.25
* Gcc-13 or Clang-15 
* GoogleTest

# Getting started

To build the project, run the following command:

```
mkdir build
cd build
cmake ..
cmake build .
```

Note: To build with tests enabled use
``` cmake -DAU_BUILD_TESTS=TRUE -DAU_CMAKE_VERBOSE=TRUE .. ```

To run the unit tests, use the following command:
``` ctest ```


