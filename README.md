# zip-bomb-creator

A from-scratch ZIP writer in C++ with **no external libraries** — every byte of the
archive, the DEFLATE stream, and the CRC-32 is produced by hand. It builds a small
compressed file that expands into a very large one, as an exercise in understanding
the ZIP and DEFLATE formats end to end.

> **Educational use only.** This is a learning project, meant to be built and tested
> on your own machine. Don't distribute the output or point it at anyone else's system.

## How it works

The payload is a single byte (`'a'`) followed by DEFLATE **LZ77 back-references** with
distance 1 and length 256. Because DEFLATE copies overlap, each reference re-copies the
byte that was just written, so one byte unfolds into gigabytes at decompression time.
The archive wraps this in hand-built ZIP records.

## What currently works

- Generates a valid ZIP entirely from scratch (no zlib / no libraries).
- Hand-written **bit writer** (LSB-first) and **fixed Huffman** DEFLATE encoding.
- **LZ77 overlapping copy** trick (length 256 / distance 1) as the expansion engine.
- **CRC-32 via matrix exponentiation** — combines the CRC of N repeated bytes in
  O(log N) instead of O(N), so the checksum of a multi-GB output is computed in
  milliseconds.
- Core ZIP structures: **local file header**, **central directory**, **EOCD**.
- **ZIP64** support — extra-field block and `0xFFFFFFFF` sentinels for uncompressed
  sizes of 4 GiB and above.

## Planned (future work)

- **Real compression** — Compressing real files and folder.
- **Dynamic Huffman tables** — build frequency-optimal codes per block rather than the
  fixed table, to improve the compression ratio of the bomb.
- **Terminal User Interface** — provide a better user interface for interacting with the programme.

## Build & run

Built with CMake (Ninja). From the project root:

```bash
cmake --build cmake-build-debug
```

Running the executable writes `Test.zip` to the working directory.
