# zip-bomb-creator

A ZIP writer in C++ that produces a compressed zip bomb

<img width="829" height="83" alt="image" src="https://github.com/user-attachments/assets/b4b3b1df-cb68-4f21-a283-1e358c5dc358" />

> **Educational use only.** This is a learning project, meant to be built and tested
> on your own machine. Don't distribute the output or point it at anyone else's system.

## How it works

The payload is a single byte (`'a'`) followed by DEFLATE **LZ77 back-references** with
distance 1 and length 256. Because DEFLATE copies overlap, each reference re-copies the
byte that was just written, so one byte unfolds into gigabytes at decompression time.
The archive wraps this in hand-built ZIP records.

## Dependencies

- **CMake** 3.20 or newer
- A **C++20** compiler (GCC, Clang, or MSVC)

## Build & run

Built with CMake. From the project root:

```bash
cmake --build cmake-build-debug
```

Running the executable writes `Test.zip` to the working directory.
