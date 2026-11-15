# Building YAP

This document describes the current build requirements, supplied scripts, compiler options, and output locations.

## Supported build environments

| Environment | Script | Compiler |
|---|---|---|
| Linux | `build.sh` | Clang |
| Windows 64-bit | `build.bat` | Clang |

Other Unix-like platforms may be compatible with parts of the code, but no dedicated build script or verified support policy is provided for them yet.

## Requirements

### Linux

- Bash.
- Clang with C23 support.
- Standard POSIX tools used by the script, including `find` and `mkdir`.

### Windows

- 64-bit Windows.
- Clang available as `clang` on `PATH`.
- PowerShell or Command Prompt.

The scripts do not install a compiler. Verify the installation with:

```sh
clang --version
```

## Clone

```sh
git clone https://github.com/soulz3r/yap.git
cd yap
```

Run build commands from the repository root. The scripts use relative paths.

## Linux build

```sh
chmod +x build.sh
./build.sh
```

The script:

1. Finds all `.c` files under `src/` using a null-delimited file list.
2. Creates `bin/` if it does not exist.
3. Adds `include/` and `src/` as header search paths.
4. Invokes Clang in C23 mode with debug information and warnings.
5. Writes the executable to `bin/yap`.

A nonzero compiler result causes the script to print a failure message and exit with status 1.

## Windows build

PowerShell:

```powershell
.\build.bat
```

Command Prompt:

```bat
build.bat
```

The batch script recursively collects C files under `src`, creates `bin`, and writes `bin\yap.exe`. It defines `YAP_PLATFORM_WINDOWS` for platform-specific source.

## Compiler options

| Option | Meaning |
|---|---|
| `-std=c23` | Compile as C23 |
| `-g` | Emit debug information |
| `-O0` | Disable optimization |
| `-Wall` | Enable common warnings |
| `-Iinclude` | Search public headers |
| `-Isrc` | Search source/internal headers |
| `-DYAP_PLATFORM_LINUX` | Select Linux-specific code |
| `-DYAP_PLATFORM_WINDOWS` | Select Windows-specific code |

The current scripts are debug-oriented. There are not yet separate release/debug profiles.

## Additional compiler flags

Both scripts append the `compilerFlags` environment variable.

Bash:

```sh
compilerFlags="-Wextra -Wpedantic" ./build.sh
```

PowerShell:

```powershell
$env:compilerFlags = "-Wextra -Wpedantic"
.\build.bat
Remove-Item Env:compilerFlags
```

This is a build-time variable, not a YAP runtime setting.

## Compile metadata

The repository contains `compile_flags.txt` for clangd/editor tooling. It does not replace the build scripts.

If build flags change, review whether the compile metadata should change as well. The actual compiler invocation remains the source of truth.

## Cleaning build output

There is no implemented `yap clean` command yet. Remove the generated output manually from the repository root.

Linux:

```sh
rm -rf bin
```

PowerShell:

```powershell
Remove-Item -Recurse -Force .\bin
```

Check the target path before running a recursive removal command.

## Run

Linux:

```sh
./bin/yap --help
./bin/yap --version
./bin/yap tree
```

Windows:

```powershell
.\bin\yap.exe --help
.\bin\yap.exe --version
.\bin\yap.exe tree
```

The tree command currently has a Linux implementation only. See [CLI usage](usage.md).

## Troubleshooting

### Clang is not found

Install LLVM/Clang and ensure the compiler is on `PATH`. Run `clang --version` in the same terminal used for the build.

### C23 is rejected

The installed compiler may be too old. Use a Clang release with C23 support rather than silently changing the project's language standard.

### Source files are not found

Run the script from the repository root. The script expects `src/` and `include/` to exist relative to the current working directory.

### Compilation fails

Read the first compiler diagnostic first. Later diagnostics may be consequences of the initial error. Confirm that the correct platform script is being used and that Windows builds target 64-bit.
