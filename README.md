# sso_string

sso_string is a mutable string library in C that uses the short string optimization to avoid unnecessary allocations of small strings at the cost of a small amount of speed. It was primarily modelled after the libc++ std::string class, and its api mostly mirrors the C++ std::string api. It has been tested using MSVC and gcc (via WSL).

sso_string aims to provide a memory efficient string representation for low-level/embedded applications, while also providing high-level string manipulation functions.


## Example

``` c
#include <sso_string.h>
#include <stdio.h>

int main(void) {
    // Create a string value on the stack and print it.
    String str = string_create("Hello, world!");
    puts(string_data(&str));

    // Still needs to free its inner resources.
    string_free_resources(&str);

    // Create a string on the heap.
    String* ref = string_create_ref("I rate this library ");

    // Append some data using printf format specifiers.
    string_format_cstr(ref, "%d/%d stars!", 5, 5);

    // Print it.
    puts(string_data(ref));

    // Free the string and its resources.
    string_free(ref); 
}
```

### Strings and C-Strings

There are many apis built around c-strings, and a string library wouldn't be worth using in most cases if it can't interface with them. sso_string provides alternative functions that accept c-strings for any function where it makes sense. It can also grab the internal c-string representation using `string_data` (`const char*`)  or `string_cstr` (`char*`). These are `NULL` terminated and can be used just like normal c-strings, as long as the caller doesn't try and resize them.

## Including

sso_string consists of a single header/source file pair, `include/sso_string.h` and `src/sso_string.c` which you can easily copy into your project (or add this repo as a submodule). It can also be added as a subproject when building with Meson.

### Meson

This is a sample Meson wrap file that can be copied to add this library as a subproject.

```
[wrap-git]

directory = sso_string
url = https://github.com/mystborn/sso_string.git
revision = master
clone-recursive = true
```

Then you can easily grab the dependency from the meson.build file:

``` meson
sso_string_proj = subproject('sso_string')
sso_string = sso_string_proj.get_variable('sso_string_dep')
```

## Documentation

Currently there is no official documentation, but the header file has thorough documentation of each function in the style of doxygen comments. For specific function usage, if it's not clear from the documentation, there is probably a unit test for it that can be used as a template.

## Building

The project uses the meson build system by default, but it shouldn't be hard to switch to cmake or another build tool if desired. It also uses check to perform the unit tests. On windows, you'll most likely need to specify `check`s location (usually "C:\Program Files (86)\check"). On \*nix OS', you'll either need to have check installed (most likely) or you can specify its location, provided it has a directory structure like the following:

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

## Todo

* API Additions
    * string_trim (High)
    * string_pad (High)
    * Custom allocation functions (Medium)
        * `sso_string_malloc`, `sso_string_calloc`, `sso_string_realloc`, `sso_string_free` 
          macros that can be defined by the user, but that use the std library versions by default). 
    * Grapheme Clusters (Low)
        * These are characters that are represented using multiple codepoints.
        * Should include functions for iterating, adding, removing, etc, grapheme clusters in a string.
        * They will start with `string_gc_*` just like the `string_u8_*` functions.
* Tests
    * There are not enough tests that check failure conditions. So as long as the input is correct, the functions should work fine, but their might be issues when the input is invalid.
* CMake Integration
* Documentation
* More Comments
    * Just need more comments throughout to explain some of the more esoteric implementations/design decisions.
* Bug Fixes
    * I am not aware of any major bugs at the moment, but there are likely some. These need to be identified and fixed.
* Improve Speed 
    * The current version of the library is more focused on usability and correctness. Future versions will start putting more
      emphasis towards execution speed.
    * Considering the main purpose of the short string optimization is memory performance, continuing to improve on this should be a major priority.