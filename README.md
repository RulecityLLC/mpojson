# mpojson

Very basic C++ JSON parsing library.  Currently uses the 'jansson' C library to do the heavy lifting.

## Caveat

This is a library I write for myself to use.  Therefore, backward compatibility isn't guaranteed.

This library depends on mpolib2 ( https://github.com/RulecityLLC/mpolib2 ).

## To build/install

These instructions assume you are installing to '/tmp/cmake_install/mpojson'.  For Windows, you can substitute 'c:\temp\cmake_install' for '/tmp/cmake_install'.

```
cd thirdparty/jansson-2.12
mkdir build
cd build
cmake -DCMAKE_SYSTEM_PREFIX_PATH=/tmp/cmake_install -DCMAKE_INSTALL_PREFIX=/tmp/cmake_install/jansson ..
make install
cd ../../..
mkdir build
cd build
cmake -DCMAKE_SYSTEM_PREFIX_PATH=/tmp/cmake_install -DCMAKE_INSTALL_PREFIX=/tmp/cmake_install/mpojson ..
make install
```
