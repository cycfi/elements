# ![Elements-Logo](https://cycfi.github.io/assets/img/elements/elements.png) Elements C++ GUI library

[![CMake Build Matrix](https://github.com/cycfi/elements/workflows/Build/badge.svg)](https://github.com/cycfi/elements/actions?query=workflow%3ABuild)

[![Build, by platform and backend](https://cycfi.github.io/elements/status/build.svg)](https://github.com/cycfi/elements/actions/workflows/build.yml)


## Introduction

![alt Elements Sampler](https://cycfi.github.io/assets/img/elements/photon_sampler.jpg)

Elements is a lightweight, fine-grained, resolution-independent, extremely modular GUI library. The library is written using modern C++20 language features. It has a declarative interface with a sensible and easy-to-use Domain Specific Embedded Languages (DSEL) syntax for constructing the GUI elements. A declarative description of the GUI is written exclusively in C++, and therefore, there is no need for an external visual GUI editor or code generator.

Elements is embeddable, meaning you can embed it in any application, and it will play well with other GUI libraries and frameworks. This also implies that you can utilize it for the creation of plugins. It does not own the event loop and is able to co-exist with components within a plugin host, such as [VST](https://en.wikipedia.org/wiki/Virtual_Studio_Technology) and [AU](https://en.wikipedia.org/wiki/Audio_Units). Porting to a new host target is straightforward, requiring the porting of only a few files.

Elements is distributed under the liberal and non-viral MIT License.

> **Status:** `master` tracks the latest stable release (currently v1.0.1). The next release, **v1.1**, is developed on `develop`, where the docs stay in sync as changes land. See [NEWS.md](NEWS.md) for what is new in each release.

## Documentation

The documentation is published at <https://cycfi.github.io/elements/>.

* [Gallery](https://cycfi.github.io/elements/elements/v1.0/gallery.html)
* [Setup and Installation](https://cycfi.github.io/elements/elements/v1.0/setup.html)
* [Tutorials](https://cycfi.github.io/elements/elements/v1.0/tutorials.html)
* [Reference](https://cycfi.github.io/elements/elements/v1.0/reference.html)

## <a name="jdeguzman"></a>The Author

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

## Discord

Join the [Discord channel](https://discord.gg/NJkadSx) to discuss Elements and chat with the developers.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2026 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*
