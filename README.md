![MIET](https://avatars0.githubusercontent.com/u/20048671?s=80)

# Computer Vision Classes 2019

[![CircleCI](https://circleci.com/gh/cvlabmiet/cvclasses19/tree/master.svg?style=svg)](https://circleci.com/gh/cvlabmiet/cvclasses19/tree/master)

## Overview

This is a Computer Vision classes prepared for the 2nd-year M.D. students.
Course includes topics covering basic areas of computer vision.
It takes 16 seminars-discussion lessons and 8 practical lessons.
Most practical lessons will be based on using of the latest [OpenCV](https://github.com/opencv/opencv) library.
All materials and tasks will be hosted in this repository and available for all students.
All program code is written using c++17.

## Required Knowledge

* Basics of Digital Signal Processing
* Basics of Digital Image Processing
* Git experience
* C++ experience

## Course Plan

Course consist of
* 26 seminars/practical lessons.
* 7 individual programming tasks.

[2019 plan](https://docs.google.com/spreadsheets/d/e/2PACX-1vQED4IwdOIDpr_h6e-k1BKvmH_Xq2l4FGPDfiJa8EPCVm5a7nWc1S1EMpOC5FbutaKBmVkuWzSLPiC8/pubhtml?gid=319680911&single=true)

## Repository Overview

The structure of repository is described below:

```
.
├── cmake       | Contains cmake helper functions for checkstyle and ctest
├── cvlib       | Simple Computer Vision Library to be developed in this course
│   ├── include | Public API
│   ├── src     | Internal sources
│   └── tests   | Unit-Tests based on Catch2 framework
└── demo        | Demo Application based on algorithms of cvlib
```

## Tools Overview

Please ensure you have following instruments and settings to start the work:
* [Git](https://git-scm.com/)
* [CMake >=3.9](https://cmake.org/)
* [OpenCV latest](http://opencv.org/downloads.html)
* gcc>=7.3 / Visual Studio 15 2017 Win64
* Camera to execute demo applications

## Build Steps

1. Register github account if you haven't it yet.
2. Meet with [git](https://git-scm.com/book/ru/v1) and practice locally.
3. Fork current [repository](https://github.com/cvlabmiet/cvclasses18).
4. [Configure git locally](https://git-scm.com/book/en/v2/Getting-Started-First-Time-Git-Setup).

### Linux

Prerequesties:

```sh
$ sudo apt install gcc-7
$ sudo apt install libopencv-dev
```

#### Build

Stay in root folder of the repository:

```sh
$ mkdir build && cd build
$ cmake ..
$ make checkstyle
$ make
$ ctest
```

#### Run

Run Unit-Tests:
```sh
build/ $ ./cvlib/cvlib_tests
```

Run Demo:

```sh
build/ $ ./demo/cvlib_demo
```

### Windows

Prerequesties:
1. Install latest [CMake x64](https://cmake.org)
2. Download and unpack binaries of OpenCV.
3. Add system environment variables, example:
    ```
    OPENCV_DIR="C:\Users\roman\Downloads\opencv\build"
    PATH="%PATH%;%OPENCV_DIR%\x64\vc15\bin"
    ```

4. Install Visual Studio 2017.

#### Build
Stay in root folder of the repository:

```sh
$ mkdir build && cd build
build/ $ cmake .. -G "Visual Studio 15 2017 Win64"
build/ $ cmake --build . --config Release
```

Note: checkstyle is disabled in WIN32 environment.

Run Demo:

```sh
build/ $ ./demo/Release/cvlib_demo.exe
```

## Contacts

Any issues/suggestions/questions may be:
* asked directly by email <golovanov@org.miet.ru>;
* created as separate [issue in this repository](https://github.com/cvlabmiet/cvclasses18/issues).
