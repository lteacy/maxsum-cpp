maxsum-cpp
==========

Implementation of the Max-Sum Algorithm in C++

Dependencies
============
Currently this project has no platform specific or third-party dependencies, other than the C++ Standard Library.
However, this may change as development proceeds.

Installation
============
Install CMake (see http://www.cmake.org/) and follow the general instructions for building CMake projects for your platform.
For example, on most unix-like platforms (including Mac OS X) it is usually sufficient to run the following commands:
    cd maxsum-cpp
    cmake .
    make

Optionally, to run unit tests:
    ctest .

And to build documentation:
    make doc

If your platform has multiple cores, both make and ctest can run in parallel, by specifying the number of cores on the command line.
For example, on a 4 core machine, run:
    make -j 4
    ctest -j 4
