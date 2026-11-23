# Zip Bomb Creator

A C++ ZIP writer that zips a folder and replaces one
file with a **bomb file**, or embeds an **overlapping bomb payload** inside the archive.
Ordinary files are compressed with standard **DEFLATE**, while the bomb entry is built with a
**dynamic Huffman table** tuned to reach DEFLATE's theoretical ceiling of roughly **1032:1**
compression.

> **Educational use only.** This is a learning project for understanding the ZIP format,
> DEFLATE, CRC-32, and ZIP64 internals.

## Dependencies

- **CMake** 3.20 or newer
- A **C++20** compiler (GCC, Clang, or MSVC)

## Build

From the project root, configure and build a Release executable:

```powershell
cmake -B build -S .
cmake --build build --config Release
```

The executable is named **`zbc`**. Depending on your generator it lands in:

- `build\Release\zbc.exe` (Visual Studio / MSVC)
- `build\zbc.exe` (MinGW, Ninja, or single-config generators)

## Add to PATH

So you can run `zbc` from any directory, add the folder containing `zbc.exe` to your system
**PATH** environment variable:

1. Press `Win`, search for **"Edit the system environment variables"**, open it.
2. Click **Environment Variables…**
3. Under **System variables** (or **User variables**), select **Path** → **Edit** → **New**.
4. Paste the full path to the build output folder, e.g. `C:\Users\You\source\repos\zip-bomb-creator\build\Release`.
5. **OK** out of every dialog and open a new terminal.

Verify:

```powershell
zbc -help
```

## Flags 
> **Flags** are case-sensitive, all flag variables should be lower-cased.


| Flag                  | Value         | Description                                                                                                                                |
|-----------------------|---------------|--------------------------------------------------------------------------------------------------------------------------------------------|
| `-help`               | —             | Displays the available flags and their descriptions.                                                                                       |
| `-dir`                | path          | Directory of the folder to be zipped. If omitted, the current working directory is zipped.                                                 |
| `-name`               | filename      | Name of the produced zip file. Default: `zipBomb.zip`.                                                                                     |
| `-target`             | relative path | Sets a specific file in the folder to be the bomb file (instead of a random one).                                                          |
| `-mb`                 | number        | Sets the bomb size in MB. Default is 1 GB.                                                                                                 |
| `-gb`                 | number        | Sets the bomb size in GB. Default is 1 GB.                                                                                                 |
| `-tb`                 | number        | Sets the bomb size in TB. Default is 1 GB.                                                                                                 |
| `-overlapName`        | name          | Builds an overlapping bomb and sets its name. Default: `overlapBomb`. Put a `*folderName*/` in front of the file name to postion the bomb. |
| `-overlappingHeaders` | number        | Number of overlapping headers in the overlap bomb. Default: `1000`.                                                                        |
| `-detailed`           | —             | Displays all log messages in full detail. Reduces performance.                                                                             |

When no target is given, the bomb replaces a **random** file in the folder. Supplying either
`-overlapName` or `-overlappingHeaders` switches the build to the overlapping-bomb mode and
leaves the ordinary zipped files intact.

Example:

```powershell
zbc -dir .\sample_folder -name demo.zip -gb 2 -target notes.txt
```

## What are ZipBombs
Historically, **zip bombs** were a form of malware that exploited antivirus tools and file
scanners which naively unzip archives for inspection. Decompressing one unloads a payload of
petabytes of garbage data, exhausting the target's memory and disk, which is a form of **denial of
service**.

The earliest zip bombs were **recursive**: decompressors would endlessly unzip archives nested
inside other archives. The classic example is [`42.zip`](https://en.wikipedia.org/wiki/Zip_bomb#Overview),
which expands to roughly 4.5 PB. A more modern technique is the **overlapping zip bomb** — the
approach this program implements, which reaches extreme ratios *without* recursion.

> [!NOTE]
> Zip bombs are largely harmless today. **Modern** decompressors and scanners set limits and checks
> on compression ratios, recursion depth, and output size, flagging anything suspicious before
> it can do damage.

> Again the keyword here is **Modern**.

> [!NOTE]
> The current implementation allows the user to hide the bomb in a targeted directory, so unsuspecting individuals might accidentally unzip it
## How to make a petabyte bomb
The overlappingHeaders flag can be treated as a multiplier for the bomb size.
Given that `1 PB` = `1,000 TB` and `1 TB` = `1,000 GB`, you can set the bomb 
size to `100 GB` and overlappingHeaders to 100,000.
>Always prioritize a higher overlappingHeaders value rather than increasing the 
bomb size, as significantly increasing the bomb size can reduce the overall 
compression ratio.

The given example creates a zip file of sized ~100 MB which unzips to 1 PB, a better ratio could be found but this is solely for display purposes

<img width="1002" height="52" alt="image" src="https://github.com/user-attachments/assets/aeb81238-5a9c-4127-a4a9-3b5fd321d6b2" />
<img width="1123" height="935" alt="image" src="https://github.com/user-attachments/assets/5a22d2b1-6e59-4f5a-b20f-c54e2736ccf9" />

>This is 1000 overlapping headers for a single GB bomb size, which results in a ~1000 KB file unzipping to a TB
>
<img width="965" height="25" alt="image" src="https://github.com/user-attachments/assets/eb6e90ba-2676-4f8e-a3f2-74161751d256" />
<img width="1097" height="740" alt="image" src="https://github.com/user-attachments/assets/e16f00e3-557e-412c-b0aa-05d87c1070cc" />


## Author's Note

For some reason I thought this was going to be a quick funny project. It was not quick
and it was painful UwU.


## References

- [The ZIP file format](https://medium.com/@felixstridsberg/the-zip-file-format-6c8a160d1c34) — Felix Stridsberg's walkthrough of the ZIP structure (local file headers, central directory, EOCD).
- [A better zip bomb](https://www.bamsoftware.com/hacks/zipbomb/) — David Fifield's research on non-recursive, overlapping zip bombs.
