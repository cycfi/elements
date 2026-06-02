# ![Elements-Logo](https://cycfi.github.io/assets/img/elements/elements.png) Elements C++ GUI library

[![CMake Build Matrix](https://github.com/cycfi/elements/workflows/Build/badge.svg)](https://github.com/cycfi/elements/actions?query=workflow%3ABuild)


## Introduction

![alt Elements Sampler](https://cycfi.github.io/assets/img/elements/photon_sampler.jpg)

Elements is a lightweight, fine-grained, resolution-independent, extremely modular GUI library. The library is written using modern C++20 language features. It has a declarative interface with a sensible and easy-to-use Domain Specific Embedded Languages (DSEL) syntax for constructing the GUI elements. A declarative description of the GUI is written exclusively in C++, and therefore, there is no need for an external visual GUI editor or code generator.

Elements is embeddable, meaning you can embed it in any application, and it will play well with other GUI libraries and frameworks. This also implies that you can utilize it for the creation of plugins. It does not own the event loop and is able to co-exist with components within a plugin host, such as [VST](https://en.wikipedia.org/wiki/Virtual_Studio_Technology) and [AU](https://en.wikipedia.org/wiki/Audio_Units). Porting to a new host target is straightforward, requiring the porting of only a few files.

Elements is distributed under the liberal and non-viral MIT License.

## Status

Please take note that Elements is still very much in flux as we are inching closer towards a version 1.0 release. The API and code is still undergoing continuous changes, and for that matter, Elements is not yet "production ready". But that should not prevent you from using Elements now! It is already in a very usable form, and more people using it will ultimately make it more robust when bugs are reported and fixed. API and usability matters and I very much welcome suggestions and contributions. Hey, this is an open source project! If you like the design, concepts and ideas behind Elements, I very much welcome collaboration.

## News

**March 28, 2024:**
I decided to bring the Cairo-based backend of Elements back into the fold. It will be the master branch once again. The Skia backend version is still very much in active development, but it still needs a lot of testing and work, especially with the way Skia is integrated.

Follow [this link](news.md) for details.

## Discord

Feel free to join the [discord channel](https://discord.gg/NJkadSx) for discussion and chat with the developers.

## Documentation

Documentation is work in progress. Stay tuned...

1. [Gallery](http://cycfi.github.io/elements/elements/skia_2024/gallery.html)
2. [Setup and Installation](http://cycfi.github.io/elements/elements/skia_2024/setup.html)
3. [Design Aspects](http://cycfi.github.io/elements/elements/skia_2024/aspects.html)
4. [Layout](http://cycfi.github.io/elements/elements/skia_2024/layout.html)

## <a name="jdeguzman"></a>About the Author

<img align="right" src="https://github.com/cycfi/elements/blob/assets/images/joel.jpg?raw=true" width="200">

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
