## C++ WebTraveler 

This project travels web.

## Build
### For Linux
```
mkdir build; cd build;
conan install ..
cmake ..
make
```

### For Windows
Note: It is successfully compiled with mingw on Linux.
```
export MXE_LIB_DIRS="Your mxe path"
mkdir winbuild; cd winbuild
conan install .. -pr toolchain
$YOUR_MXE_CMAKE .. -DUSE_MXE=true
make

