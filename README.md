# ![Elements-Logo](docs/assets/images/elements.png) Elements C++ GUI library

[![CMake Build Matrix](https://github.com/cycfi/elements/workflows/Build/badge.svg)](https://github.com/cycfi/elements/actions?query=workflow%3ABuild)

![alt Photon Sampler](docs/assets/images/photon_sampler.jpg)

## Introduction

Elements is a lightweight, fine-grained, resolution independent, modular GUI library. Elements is designed with these requirements in mind:

1. It should be open source with a liberal, non-viral license.
2. It should be usable in any application and should play well with other GUI
   libraries and frameworks.
3. Corollary to the second requirement is that it can also be used to develop
   plugins. It should not own the event loop and should be able to co-exist
   with components within a plugin host such as VST and AU.
4. It should be resolution independent and allow for HDPI displays.
5. Should not rely on a “visual” GUI editor or code generator.
6. It should have a declarative API using modern C++. Declarative C++ code
   tells you *what* rather than *how* (imperative). The GUI should be
   declared in C++ code.

Elements, is extremely lightweight… and modular. You compose very
fine-grained, flyweight “elements” to form deep element hierarchies using a
declarative interface with heavy emphasis on reuse.

## News

- 26 May 2020: Introducing the [Artist 2D Canvas
  Library](https://github.com/cycfi/artist). The Artist library will
  eventually replace Element's 2d canvas implementation.
- 23 May 2020: Work in progress `notebook` gallery factory (`notebook` branch).
- 8 April 2020: Simplified installation (Infra and Json are now header-only submodules)
- 11 March 2020: Added [Layout documentation](http://cycfi.github.io/elements/layout).
- 3 March 2020: Added [Design Aspects documentation](http://cycfi.github.io/elements/aspects).
- 23 February 2020: Setup and Installation guide updated for Windows and Linux.
- 22 February 2020: Improved font support
- 01 September 2019: Linux port completed
- 12 August 2019: Windows port completed

## Documentation

Documentation is work in progress. Stay tuned...

1. [Gallery](http://cycfi.github.io/elements/gallery)
2. [Setup and Installation](http://cycfi.github.io/elements/setup)
3. [Design Aspects](http://cycfi.github.io/elements/aspects)
4. [Layout](http://cycfi.github.io/elements/layout)

## <a name="jdeguzman"></a>About the Author

Joel got into electronics and programming in the 80s because almost
everything in music, his first love, is becoming electronic and digital.
Since then, he builds his own guitars, effect boxes and synths. He enjoys
playing distortion-laden rock guitar, composes and produces his own music in
his home studio.

Joel de Guzman is the principal architect and engineer at [Cycfi
Research](https://www.cycfi.com/) and a consultant at [Ciere
Consulting](https://ciere.com/). He is a software engineer specializing in
advanced C++ and an advocate of Open Source. He has authored a number of
highly successful Open Source projects such as
[Boost.Spirit](http://tinyurl.com/ydhotlaf),
[Boost.Phoenix](http://tinyurl.com/y6vkeo5t) and
[Boost.Fusion](http://tinyurl.com/ybn5oq9v). These libraries are all part of
the [Boost Libraries](http://tinyurl.com/jubgged), a well respected,
peer-reviewed, Open Source, collaborative development effort.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*
