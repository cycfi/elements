# ![Elements-Logo](https://cycfi.github.io/assets/img/elements/elements.png) Elements C++ GUI library

[![CMake Build Matrix](https://github.com/cycfi/elements/workflows/Build/badge.svg)](https://github.com/cycfi/elements/actions?query=workflow%3ABuild)

![alt Photon Sampler](https://cycfi.github.io/assets/img/elements/photon_sampler.jpg)

## Introduction

Elements is a lightweight, fine-grained, resolution independent, modular GUI library.
Elements is designed with these requirements in mind:

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
7. Keep it simple. Make it easy to use right out of the box, with a lot of examples.
8. Porting to a new host target should be as easy as possible, requiring
   porting of only a few files.

Elements, is extremely lightweight… and modular. You compose very
fine-grained, flyweight “elements” to form deep element hierarchies using a
declarative interface with heavy emphasis on reuse.

## Status

Please take note that Elements is still very much in flux as we are inching closer towards a version 1.0 release. The API and code is still undergoing continuous changes, and for that matter, Elements is not yet "production ready". But that should not prevent you from using Elements now! It is already in a very usable form, and more people using it will ultimately make it more robust when bugs are reported and fixed. API and usability matters and I very much welcome suggestions and contributions. Hey, this is an open source project! If you like the design, concepts and ideas behind Elements, I very much welcome collaboration.

## News

**March 28, 2024:**
I decided to bring the Cairo-based backend of Elements back into the fold. It will be the master branch once again. The Skia backend version is still very much in active development, but it still needs a lot of testing and work, especially with the way Skia is integrated.

Follow [this link](docs/news.md) for details.

## Discord

Feel free to join the [discord channel](https://discord.gg/NJkadSx) for discussion and chat with the developers.

## Documentation

Documentation is work in progress. Stay tuned...

1. [Gallery](http://cycfi.github.io/elements/gallery)
2. [Setup and Installation](docs/setup.md)
3. [Design Aspects](http://cycfi.github.io/elements/aspects)
4. [Layout](http://cycfi.github.io/elements/layout)

## <a name="jdeguzman"></a>About the Author

<img align="right" src="https://github.com/cycfi/elements/blob/assets/images/joel.jpg" width="200">

Joel got into electronics and programming in the 80s because almost
everything in music, his first love, is becoming electronic and digital.
Since then, he builds his own guitars, effect boxes and synths. He enjoys
playing distortion-laden rock guitar, composes and produces his own music in
his home studio.

Joel de Guzman is the principal architect and engineer at [Cycfi
Research](https://www.cycfi.com/). He is a software engineer specializing in
advanced C++ and an advocate of Open Source. He has authored a number of
highly successful Open Source projects such as
[Boost.Spirit](http://tinyurl.com/ydhotlaf),
[Boost.Phoenix](http://tinyurl.com/y6vkeo5t) and
[Boost.Fusion](http://tinyurl.com/ybn5oq9v). These libraries are all part of
the [Boost Libraries](http://tinyurl.com/jubgged), a well respected,
peer-reviewed, Open Source, collaborative development effort.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2024 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*
