<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# Build the Sphinx documentation

The repository's Sphinx site is generated from `docs/index.rst`. Doxygen runs
first and supplies XML to the API pages through Breathe.

## Prerequisites

Install:

- CMake 3.26 or newer
- Doxygen
- Graphviz (`dot`)
- Sphinx
- `myst-parser`
- `packaging`

The `breathe` Python package and `rocm-docs-core` are optional:

- Install `breathe` to render the generated Doxygen API members.
- Install `rocm-docs-core` to use the ROCm documentation theme.
- Without either package, the build uses an Alabaster theme and clearly marks
  the unavailable generated API sections.

## Configure and build

From the repository root:

```sh
cmake -S . -B build-docs \
    -DAU_BUILD_DOCS=ON \
    -DAU_BUILD_TESTS=OFF \
    -DAU_BUILD_EXAMPLES=OFF
cmake --build build-docs --target Doxygen
cmake --build build-docs --target Sphinx
```

The combined target is also available:

```sh
cmake --build build-docs --target Doxygen Sphinx
```

The generated entry points are:

- `build-docs/docs/doxygen/html/index.html`
- `build-docs/docs/sphinx/index.html`

The Sphinx target automatically uses the Doxygen XML generated in the same
build tree.

## Direct Sphinx invocation

After building Doxygen, the equivalent direct command is:

```sh
sphinx-build -E -a -b html -W \
    -D breathe_projects.aoclutils=build-docs/docs/doxygen/xml \
    docs build-docs/docs/sphinx
```

`-W` promotes documentation warnings to errors. `-E -a` forces a clean
environment and rebuild, which is useful after changing navigation or API
directives.

## Documentation references

Use the local generated Sphinx pages for AOCL-Utils APIs and the internal
`docs/SupportedPackageMatrix.md` for support claims. It is acceptable to link
to AMD's published documentation when referring to a public AMD API or an
external AMD package.

To clean the isolated documentation build:

```sh
rm -rf build-docs
```
