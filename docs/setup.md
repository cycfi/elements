# ![Elements-Logo](images/elements.png) Elements C++ GUI library

## Setup and Installation

You are a modern c++ programmer. You are smart. You know your way around and
you do not need any hand-holding :smiley: So here are the basic requirements
and dependencies that you need to satisfy in order to use the library:

1. A C++17 compiler
2. Git
3. [CMake](https://cmake.org/) 3.9.6 or higher
4. [Boost](https://www.boost.org/) 1.68 or higher
5. [Cairo](https://cairographics.org/)
6. The [Cycfi infra library](https://github.com/cycfi/infra/)
7. The [Cycfi json library](https://github.com/cycfi/json/)
8. [fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)
9. [freetype2](https://www.freetype.org/) (Windows and Linux only).

Nuff said? :stuck_out_tongue: Well OK, here are some more info, just for
completeness' sake...

## Requirements

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

### Install required libraries using Brew:

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

### Using [CLion](https://www.jetbrains.com/clion/):

Simply open the CMakeLists.txt file using CLion and build the project.

### Building and Running the examples

If successful, cmake will generate an XCode project in the build directory.
Open the project file elements.xcodeproj and build all. You should see a
couple of example applications.

Tips:
1. Resize the window and see the elements fluidly resize
2. Use two-finger swipe to move the sliders and knobs

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

### Building and Running the examples

If successful, cmake will generate a Visual Studio solution in the build
directory. Open the project file elements.sln and build all. You should see a
couple of example applications.

Tips:
1. Resize the window and see the elements fluidly resize
2. Use two-finger swipe to move the sliders and knobs

-------------------------------------------------------------------------------

## Linux