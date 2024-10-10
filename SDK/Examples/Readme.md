# Building

```sh
cmake . -B build -DAUROOT=<Utils install path>
cmake --build build --config Release -j
```
export LD_LIBRARY_PATH=<Utilst install path>/lib
where AUROOT is the AOCL-Utils install directory
