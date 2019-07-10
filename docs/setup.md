# ![Elemental-Logo](images/elemental.png) Elemental C++ GUI library

## Setup and Installation

You are a modern c++ programmer. You are smart. You know your way around and
you do not need any hand-holding :smiley: So here are the basic requirements
and dependencies that you need to satisfy in order to use the library:

1. A C++17 compiler
2. git
3. cmake 3.5.1 or higher
4. [Boost](https://www.boost.org/) 1.61 or higher
5. [Cairo](https://cairographics.org/)
6. The [Cycfi infra library](https://github.com/cycfi/infra/)
7. The [Cycfi json library](https://github.com/cycfi/json/)

Nuff said? :stuck_out_tongue: Well OK, here are some more info, just for
completeness' sake...

### C++17

Elemental supports the MacOS at this point. Ports for other platforms are
still in flux. In the Mac, we support both XCode and CLion IDEs. Get the
latest version with a C++17 compiler.

### Git

Elemental C++ GUI library, plus the Cycfi Infra and JSON libraries:

```
git clone https://github.com/cycfi/elemental.git
git clone https://github.com/cycfi/infra.git
git clone https://github.com/cycfi/json.git
```

Infra provides some basic groundwork common to Cycfi libraries, including
Elemental. JSON provides Strict, type-safe, c++ to JSON I/O.  It's easiest to
place your clone of the infra and json libraries in the same directory as 
you have the Elemental C++ GUI library.

If you wish to place them somewhere else, then you need to set the cmake
variables CYCFI_INFRA_ROOT, and CYCFI_JSON_ROOT on the cmake command line
invocation:

```
-DCYCFI_INFRA_ROOT=cycfi-infra-path -CYCFI_JSON_ROOT=cycfi-json-path
```

### Boost

#### Download Boost

From (https://www.boost.org/users/download/), get the latest Boost version.
If you have it already, make sure you have version 1.61 or higher.

#### Build Boost

Many Boost libraries are header-only. You only need to build some of the
non-header-only libraries:

1. CD to boost directory
2. Bootstrap Boost: ```./bootstrap.sh```
3. Build Boost: ```./b2 -j+8 --with-filesystem --with-system stage```


### CMake

Make sure you have CMake 3.7.2 or higher. Follow the installation procedure
for your platform.

For MacOS, using Homebrew:

```
brew install cmake
```
### Generating the Project using CMake

There are multiple ways to generate a project file using CMake depending on 
your platform and desired IDE, but here is an some example for MacOS 10.14:

### Using [XCode](https://developer.apple.com/xcode/):

1. CD to the elemental library: ```cd elemental```
2. Make a build directory inside the elemental directory ```mkdir build```
3. CD to the build directory: ```cd build```
4. invoke cmake: cmake -GXcode -DBOOST_ROOT=your-installation-boost-path ../

If placed the cycfi infra and json somewhere else, then you need to set the 
cmake variables CYCFI_INFRA_ROOT, and CYCFI_JSON_ROOT too:

```
-DCYCFI_INFRA_ROOT=cycfi-infra-path -CYCFI_JSON_ROOT=cycfi-json-path
```

### Using [CLion](https://www.jetbrains.com/clion/):

1. Simply open the CMakeLists.txt file using CLion.
2. Open Preferences. Under Build, Execution, Deployment/CMake, add
   -DBOOST_ROOT=your-installation-boost-path to your CMake options
   
If placed the cycfi infra and json somewhere else, then you need to add the 
cmake variables CYCFI_INFRA_ROOT, and CYCFI_JSON_ROOT on the cmake command line
invocation:

```
-DCYCFI_INFRA_ROOT=cycfi-infra-path -CYCFI_JSON_ROOT=cycfi-json-path
```

### Building and Running the examples

If successful, cmake will generate an XCode project in the build directory.
Open the project file elemental.xcodeproj and build all. You should see a
couple of example applications.

Tips:
1. Resize the window and see the elements fluidly resize
2. Use two-finger swipe to move the sliders and knobs
