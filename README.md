# Zakero's C++ Header Libraries

This is a collection of libraries in C++ header file form.  All the header 
libraries can be found in the `include/` directory.  Due to their nature, the 
libraries are not stand-alone _Single Header Libraries_.  The reason for this 
is that there maybe dependencies between the headers.  Refer to each library 
to learn what they depend on.  Or just add all of them to your project and 
not worry about it.  :-)

The documentation is available locally, if cloned, at `html/index.html` as 
well as [online](https://zhl.zakero.com/doc/trunk/html/index.html).  The 
official website for the project is [zhl.zakero.com](https://zhl.zakero.com).

| Library             | Version | Description                                                       |
|---------------------|:-------:|-------------------------------------------------------------------|
| Zakero_Base.h       |  0.9.1  | A collection of helper functions, macros, and templates           |
| Zakero_MemoryPool.h |  0.9.0  | An expandable memory pool that is based on Unix File Descriptors  |
| Zakero_Profiler.h   |  0.9.0  | Generate profiling data that can be visualized in Chrome/Chromium |
| Zakero_Yetani.h     |  0.6.0  | A class that makes working with Wayland much easier               |
| _More coming soon_  |         |                                                                   |

# License

All libraries are licensed using the [Mozilla Public License v2](https://www.mozilla.org/en-US/MPL/2.0/) 
([FAQ](https://www.mozilla.org/en-US/MPL/2.0/FAQ/)).  The key point of this 
license is that the license applies __only__ to the header file, not what 
includes it or what it is compiled into (not viral).

