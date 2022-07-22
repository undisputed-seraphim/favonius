# Favonius
A set of templated C++ libraries for Zephyr

Zephyr is a real-time operating system that exposes a C API. This library intends to provide a statically typed, RAII-enabled wrapper around the C API for ease and convenience of programming.

The source code also aims to be IEC 61508-compliant; however, users must seek their own certification. Out-of-the-box certification is NOT provided!

## Features

Master branch aims to be compatible only with the headers of the current LTS version of Zephyr that is also IEC 61508 certified, which is v2.7.0.

- Written in pure C++ language constructs only.
- No dependency on C or C++ standard libraries, template or otherwise.
- No dependency on POSIX.
- Dynamic allocations are backed by statically allocated memory.
- No exceptions are thrown.
- Aims to be compatible with the certification requirements of IEC 61508.
- Aims to be *mostly* `std` compatible.

`std` compatibility is a best-effort basis and full compatibility is not a goal. Notably, as an embedded real-time OS, dynamic memory allocation is highly restricted; consequently, any `std` features that require dynamic memory allocation will be either missing or heavily altered. Same goes for exceptions.

Some return types and function parameters may be slightly different. Users will have to check the return values of allocating functions to ensure that objects are properly constructed in containers. Functions that typically `throw` in standard C++ will return a value or nullptr instead.

Additionally, the Zephyr public API exposes more functionality for some concepts than `std` offers (e.g. cpu pinning for threads). Those functions will also be available under the same constructs as `std`, but with a different capitalization style to indicate that those functions do not originate from `std`.

## Usage

Minimum compiler requirement is C++14.

There are two namespaces:
- `ztd`, which represents `std` compatible classes and functions, but are backed by functions provided by Zephyr OS.
- `fav`, which represents all other constructs and primitives from the Zephyr OS that don't fit in `ztd`.

## Development

Users are encouraged to file an issue if any code is in conflict with IEC 61508.

Zephyr v3.x have slightly different header paths. The modified code for this will live in v3-dev.

It will be merged to master if and only if Zephyr v3.x becomes IEC 61508 certified. That is currently not on the horizon.

## License

Apache 2.0
