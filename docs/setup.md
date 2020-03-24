# Setup and Installation

## Table of Contents
* [Requirements](#requirements)
* [MacOS Installation](#macos)
* [Windows Installation](#windows)
* [Windows Installation (Gtk)](#windows-gtk)
* [Linux Installation](#linux)
* [Building and Running the examples](#building-and-running-the-examples)

-------------------------------------------------------------------------------

## Requirements

Here are the basic requirements and dependencies that you need to satisfy in
order to use the library:

1. A C++17 compiler
2. Git
3. [CMake](https://cmake.org/) 3.9.6 or higher
4. [Boost](https://www.boost.org/) 1.68 or higher
5. [Cairo](https://cairographics.org/)
6. The [Cycfi infra library](https://github.com/cycfi/infra/)
7. The [Cycfi json library](https://github.com/cycfi/json/)
8. [fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)
9. [freetype2](https://www.freetype.org/) (Windows and Linux only).

### C++17

Elements currently supports the MacOS, Windows and Linux. In the Mac, we
support both [XCode](https://developer.apple.com/xcode/) and
[CLion](https://www.jetbrains.com/clion/) IDEs. Elements is tested with XCode
10 and XCode 11. In Windows, we support Windows 10 with [Visual Studio
2019](https://visualstudio.microsoft.com/vs/), although it will probably also
work with older versions of the Visual Studio IDE. In Linux, we support both
[Clang](https://clang.llvm.org/) and [g++](https://gcc.gnu.org/) Get the
latest version with a C++17 compiler.

### Git

Elements C++ GUI library, plus the Cycfi Infra and JSON libraries:

```
git clone https://github.com/cycfi/elements.git
git clone https://github.com/cycfi/infra.git
git clone https://github.com/cycfi/json.git
```

Infra provides some basic groundwork common to Cycfi libraries, including
Elements. JSON provides Strict, type-safe, c++ to JSON I/O.  It's easiest to
place your clone of the infra and json libraries in the same directory as
you have the Elements C++ GUI library.

If you wish to place them somewhere else, then you need to set the cmake
variables CYCFI_INFRA_ROOT, and CYCFI_JSON_ROOT later (see below).

### CMake

Make sure you have [CMake](https://cmake.org/) 3.9.6 or higher. Follow the
installation procedure for your platform, or follow the instructions below
for specific environments.

### Required Libraries

Elements requires [Cairo](https://www.cairographics.org/) 1.16 or higher, and
the [Boost Libraries](https://www.boost.org/) 1.68 or higher,
[fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) and
[freetype2](https://www.freetype.org/) (on Windows and Linux). Specific
instructions are provided below for specific environments.

-------------------------------------------------------------------------------

## MacOS

### Install required libraries using [Homebrew](https://brew.sh/):

```
brew install cairo
brew install boost
brew install fontconfig
```

### Install CMake

```
brew install cmake
```

### Generating the Project using CMake

There are multiple ways to generate a project file using CMake depending on
your platform and desired IDE, but here are some examples for the MacOS:

### Using [XCode](https://developer.apple.com/xcode/):

1. CD to the elements library.
2. Make a build directory inside the elements directory.
3. CD to the build directory.
4. invoke cmake.

```
cd elements
mkdir build
cd build
cmake -GXcode ../
```

If successful, cmake will generate an XCode project in the build directory.
Open the project file elements.xcodeproj and build all. You should see a
couple of example applications.

### Using [CLion](https://www.jetbrains.com/clion/):

Simply open the CMakeLists.txt file using CLion and build the project.

-------------------------------------------------------------------------------

## Windows

### Install required libraries

The Windows port comes with the cairo, fontconfig and freetype2 binaries as
part of the installation in the `lib/external` directory, so there is no need
to have these libraries installed. CMake will take care of the dependencies.

The only thing you have to install is Boost:

#### Download and build Boost

From (https://www.boost.org/users/download/), get the latest Boost version.
If you have it already, make sure you have version 1.68 or higher. Many Boost
libraries are header-only. You only need to build some of the non-header-only
libraries:

1. CD to boost directory
2. Bootstrap Boost
3. Build Boost

```
cd path/to/boost
bootstrap.bat
b2 -j+8 --with-filesystem --with-system --with-date_time --with-regex link=static stage
```

*Replace path/to/boost with the directory where you installed boost.*

### Install CMake

Follow the instructions provided here: https://cmake.org/install/

### Generating the Project using CMake

Assuming you have [Visual Studio
2019](https://visualstudio.microsoft.com/vs/) installed:

1. CD to the elements library.
2. Make a build directory inside the elements directory.
3. CD to the build directory.
4. invoke cmake.

```
cd elements
mkdir build
cd build
cmake -G"Visual Studio 16 2019" -DBOOST_ROOT=path/to/boost ..//
```

*Replace path/to/boost with the directory where you installed boost.*

If successful, cmake will generate a Visual Studio solution in the build
directory. Open the project file elements.sln and build all. You should see a
couple of example applications.

-------------------------------------------------------------------------------

## Windows (Gtk)

### Install MSYS2 toolchain and required libraries
> MSYS2 is a software distro and building platform for Windows

Download MSYS2 from its [official website](https://www.msys2.org/) and install it. Its installation guide is on the [home page](https://www.msys2.org/). 

Open `MSYS2 MinGW 64-bit` or `MSYS2 MinGW 32-bit` from your start menu. Install tools and libraries:
```
pacman -S ${MINGW_PACKAGE_PREFIX}-toolchains
pacman -S ${MINGW_PACKAGE_PREFIX}-boost
pacman -S ${MINGW_PACKAGE_PREFIX}-cairo
pacman -S ${MINGW_PACKAGE_PREFIX}-gtk3
pacman -S make
```

### Install CMake

```
pacman -S ${MINGW_PACKAGE_PREFIX}-cmake
```

### Generating the Project using CMake

There are multiple ways to generate a project file using CMake depending on
your platform and desired IDE, but here are some examples for MSYS2:

### Using UNIX makefiles:

1. CD to the elements library.
2. Make a build directory inside the elements directory.
3. CD to the build directory.
4. invoke cmake. -DHOST_UI_LIBRARY=gtk tells cmake to build with Gtk.

```
cd elements
mkdir build
cd build
cmake ../ -G "Unix Makefiles" -DHOST_UI_LIBRARY=gtk
```

Note that GCC on MinGW may come across a multiple reference bug if you turn on LTO.
To disable LTO, you should change
```
INTERPROCEDURAL_OPTIMIZATION TRUE
```
in `CMakeMain.txt` and `lib/CMakeLists.txt` to
```
INTERPROCEDURAL_OPTIMIZATION FALSE
```

If successful, cmake will generate Unix Make files in the build directory.

### Using [CLion](https://www.jetbrains.com/clion/):

Simply open the CMakeLists.txt file using CLion and build the project.


-------------------------------------------------------------------------------

## Linux

### Install required libraries using [apt-get](https://linux.die.net/man/8/apt-get) (requires `sudo`).

In addition to the requirements listed in the [requirements](#requirements)
section, the [GTK3 library](https://www.gtk.org/) is also required by the
Linux port of Elements.

```
sudo apt-get install libcairo2-dev
sudo apt-get install libgtk-3-dev
sudo apt-get install libboost-all-dev
```

### Install CMake

```
sudo apt-get -y install cmake
```

### Generating the Project using CMake

There are multiple ways to generate a project file using CMake depending on
your platform and desired IDE, but here are some examples for Linux:

### Using UNIX makefiles:

1. CD to the elements library.
2. Make a build directory inside the elements directory.
3. CD to the build directory.
4. invoke cmake.

```
cd elements
mkdir build
cd build
cmake -G "Unix Makefiles" ../
```

If successful, cmake will generate Unix Make files in the build directory.

### Using [CLion](https://www.jetbrains.com/clion/):

Simply open the CMakeLists.txt file using CLion and build the project.

-------------------------------------------------------------------------------

## Building and Running the examples

<div class = "img-align-right" style="width:35%">
  <img src="{{ site.url }}/elements/assets/images/examples.png" alt="Examples">
  <p>CLion Project Tool Window</p>
</div>

If successful, cmake will generate a project file or makefiles in the build
directory. Build the library and example programs using the generated
makefiles or open the project file using your IDE and build all.

You should see a couple of example applications in there that you can run.
These examples are provided as starting points to get you up to speed in
learning how to use the library. For simplicity, each example is contained in
a single `main.cpp` source file.

Feel free to inspect and mess with the examples. Each example demonstrates
different aspects of the Elements library, from layout to control. You'd
probably want to start with the “hello_universe” example, which is the
Element's hello-world counterpart. You can copy the “empty” example and use
it as a boilerplate for your own project.

Tips:
1. Resize the window and see the elements fluidly resize
2. Use two-finger swipe to move the sliders and knobs

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*