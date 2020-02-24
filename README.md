# ![Elements-Logo](docs/images/elements.png) Elements C++ GUI library

![alt Photon Sampler](docs/images/photon_sampler.jpg)

## Introduction

> ### Perfect is the enemy of the good, but I just can’t resist it!
> *— Joel de Guzman*

Elements is a lightweight, fine-grained, HDPI capable resolution independent, modular GUI library, designed with these requirements in mind:

* It should be open source with a liberal, non-viral license.
* It should be usable in any application and should play well with other GUI libraries and frameworks.
* Corollary to the second requirement is that it can also be used to develop plugins (e.g. it should not own the event loop and can co-exist with elements within a plugin host such as VST and AU.
* It should be resolution independent and allows for HDPI displays.
* Should not rely on a “visual” GUI editor or code generator. The GUI should be declared in the code.
* Declarative API using modern C++. Declarative C++ code
tells you what rather than how (imperative).

Elements, is very lightweight… and extremely modular. You compose very
fine-grained, flyweight “elements” to form deep element hierarchies using a
declarative interface with heavy emphasis on reuse.

## Setup and Installation

The Elements C++ GUI library is cross-platform. Follow the [Setup and
Installation guide](docs/setup.md) to get started using the library.

The library is Open Source and released under the very liberal [MIT
license](http://tinyurl.com/p6pekvo).

## History

<img style="float: right;" hspace="10" width="35%" src="docs/images/hdpi.png">

Sometime in 2014, I started searching for a GUI library I can use for some of the projects I am developing given the requirements listed above.

HDPI Matters. Zoom in and notice the standard DPI icons at the top (e.g. the Power Button from the VST3 Plug-in Test Host), vs. Elements’ vector dials.
Sometime in 2014, I started searching for a GUI library I can use for some of the projects I am developing. The most important requirements to me were:

It should be open source with a liberal, non-viral license.
It should be usable in any application and should play well with other GUI libraries and frameworks.
Corollary to the second requirement is that it can also be used to develop plugins (e.g. it should not own the event loop and can co-exist with elements within a plugin host such as VST and AU.
It should be resolution independent and allows for HDPI displays.
I tried hard to find something that satisfies these requirements. I failed. I did not find any. JUCE comes close, but it did not satisfy the first requirement. iPlug is usable. I actually prototyped some plugins using it, but it did not satisfy the 4th requirement. I’m also unsure if it satisfies the 2nd requirement.

There are other requirements, such as not relying on a “visual” GUI editor or code generator. IMO, the GUI should be declared in the code. Obviously, good design and architecture is also another requirement. Most GUI C++ libraries are just too 90s for me to consider. None of that crappy OOP bleh! But, hey, I am digressing! The truth is, I am even willing to use a library with a pure C interface, such as GLFW, as long as it is well designed (GLFW is simple and clean) and can be wrapped in C++ anyway. I also tried to use NanoVG — not really a GUI library, but it makes it easier to write one (NanoVG inspired Elements’ Cairo based vector graphics canvas).

I know that perfect is the enemy of the good, but I just can’t resist it. I couldn’t stand it anymore. So at around 2016, I decided to write my own. Actually, I did not start from scratch. I had a head start: I’ve been writing GUI libraries since the 90s. One of the main projects I got involved with when I was working in Japan in the 90s was a lightweight GUI library named Pica. So I went ahead, dusted off the old code and rewrote it from the ground up using modern C++.

## News

- 12 August 2019: Windows port completed
- 01 September 2019: Linux port completed
- 22 February 2020: Improved font support
- 23 February 2020: Setup and Installation guide updated for Windows and Linux.
