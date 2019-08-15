# sso_string

sso_string is a header only, mutable string library in C that uses the short string optimization to avoid unnecessary allocations at the cost of a small amount of speed. It was primarily modelled after the libc++ std::string class, and it's api mostly mirrors the C++ std::string api. It has been tested using MSVC and gcc (via WSL).

# Using

It's very simple to add this to your project. It consists of a single header, `include/sso_string.h` which you can easily copy into your project, or just subproject this repo.

# Building

The only thing to build is the test suite. The project uses the meson build system by default, but it shouldn't be hard to switch to cmake or another build tool if desired. It also uses check to perform the unit tests. On windows, you'll most likely need to specify `check`s location (usually "C:\Program Files (86)\check"). On \*nix OS', you'll either need to have check installed (most likely) or you can specify its location, provided it has a directory structure like the following:

```
check/
    include/
        check.h
    lib/
        libcheck.lib
        libcompat.lib
```

This is an example of how to build the project using meson on windows.

```sh
C:\\sso> mkdir build
C:\\sso> cd build
C:\\sso> meson .. --buildtype=release
C:\\sso\\build> meson configure "-Dcheck_location=C:\\Program Files (86)\\check"
C:\\sso\\build> ninja test
```

# API

Todo...