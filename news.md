# News

**March 28, 2024:**
I decided to bring the Cairo-based backend of Elements back into the fold. It will be the master branch once again. The Skia backend version is still very much in active development, but it still needs a lot of testing and work, especially with the way Skia is integrated (more on that below). The APIs of elements with Skia and Cairo-based backends are essentially in sync right now. There are some necessary API changes, but I made sure, as best as I could, to apply deprecation warnings so as not to cause too much disruption when upgrading to the latest code.

The latest code that uses Cairo is in the `cairo_2024` branch. This will become the new master. The current master will become the `skia_2024` branch. I am now trying to avoid prebuilt libraries, as I find them too unwieldy to work with and difficult to maintain. The `cairo_2024` branch, for example, now uses `pkg-config`, which obviously requires a package manager that supports it, with integration with CMake. In my development work, I use VCPKG on Windows. I find that it only requires `pkgconf`, `cairo`, and `libwebp` to get the `cairo_2024` branch building. Installing Cairo already brings in the other dependencies.

Integrating Skia, on the other hand, is a mess! Currently, prebuilt binaries are automatically installed via CMake. But this strategy has proven to be VERY difficult to maintain. Windows, for example, requires binaries for x86-64 (forget 32-bit!) and arm64 in both debug and release modes, as well as Clang and MSVC! And the situation is fragile! For example, a prebuilt binary for VS 2022 may not necessarily link properly with prebuilt binary for VS 2019. And then multiply that with the OSes supported, and it becomes a combinatorial explosion nightmare! I intend to find a solution to this madness. One solution is to rely on VCPKG, as it is the only package manager that supports Skia on all platforms.

The longer-term plan is to complete the port of the Cairo backend into the Artist library, find a reasonable packaging solution for Skia, fix the instabilities of Elements on Wayland, and unify the `skia_2024` and `cairo_2024` (soon to be master) branches. The develop branch will cease to exist, BTW. Development will happen on feature branches, with frequent merges into the `master` branch.

**December 5, 2023:**
The master branch now contains the code base using the Artist library for 2D canvas
drawing (https://github.com/cycfi/artist).

For those who have not been following recently, the Artist 2D Canvas Library is an abstraction layer with an API
inspired by the HTML5 canvas API. The library presents a lean API modeled after the HTML Canvas 2D Context
specification. The API is a not-so-thin layer above various 2D platform-specific and cross-platform 2D
"backend" graphics libraries, such as Skia and Quartz-2D.

The artist library replaces the original cairo based  canvas implementation used by elements. The classic
cairo implementation is now in the `cairo_2023` branch. At this point, as long as you are using the examples
as a starting point and usage guide, code cairo_2023 is fully compatible with the artist based code, which
is now in the `master` branch.

**19 January 2021:** Added Child Windows to develop (will merge to master in due time). [See Example](https://bit.ly/3iLMJt2).

**25 September 2020:** Added [thumbwheels](https://github.com/cycfi/elements/issues/231).

**12 September 2020:** Linux [Artist 2D Canvas Library](https://github.com/cycfi/artist) port feature complete.

**10 September 2020:** Windows [Artist 2D Canvas Library](https://github.com/cycfi/artist) port feature complete.

**7 September 2020: MacOS** [Artist 2D Canvas Library](https://github.com/cycfi/artist) port feature complete.

**22 June 2020:** Removed dependency on Boost. This requires some API changes to the `app` with additional arguments. See examples.

**26 May 2020:** Introducing the [Artist 2D Canvas Library](https://github.com/cycfi/artist). The Artist library will eventually replace Element's 2d canvas implementation.

**23 May 2020: Work in progress** `notebook` gallery factory (`notebook` branch).

**8 April 2020: Simplified installation** (Infra and Json are now header-only submodules)

**11 March 2020:** Added [Layout documentation](http://cycfi.github.io/elements/layout).

**3 March 2020: Added** [Design Aspects documentation](http://cycfi.github.io/elements/aspects).

**23 February 2020:** Setup and Installation guide updated for Windows and Linux.

**22 February 2020:** Improved font support

**01 September 2019:** Linux port completed

**12 August 2019:** Windows port completed

