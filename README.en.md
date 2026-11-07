<div align="center">
  <img src="resources/icons/otne.svg" alt="Otne" width="160" height="160">

  <h1>Otne Programming Language</h1>

  <p>An open source interpreted language</p>

  <p>
    <a href="README.md">简体中文</a> •
    <a href="README.en.md">English</a> •
    <a href="CHANGELOG.md">Changelog</a> •
    <a href="docs/otne-language.md">Language reference</a>
  </p>
</div>

## Introduction

Otne is a programming language designed by Uxnt and uxnt-naxe since March 6, 2022. Its syntax follows Java and Go.

Version 0.0.2 is a **tree-walking interpreter** written in C++17: lexer, recursive-descent parser, runtime environment and builtins — it actually executes `.otne` programs.

## Quick Start

### Requirements

| Dependency | Version |
| --- | --- |
| CMake | 3.16 or newer |
| C++ compiler | C++17 support (GCC / Clang / MSVC / MinGW-w64) |

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The executable is generated at `build/bin/otne`.

A convenience Makefile (wrapping CMake) is also provided:

```bash
make                                              # Build (Release)
make debug                                        # Debug build
make rebuild                                      # Clean rebuild
make run                                          # Run the default sample
make run FILE=resources/code/Fibonacci.otne       # Run a given file
make tokens FILE=resources/code/HelloWorld.otne   # Dump tokens
make test                                         # Smoke tests
make ctest                                        # Run tests via CTest
make clean                                        # Clean build output
make help                                         # List all targets
```

### Run

```bash
# Runs the default sample: resources/code/HelloWorld.otne
./build/bin/otne

# Runs the given source file
./build/bin/otne resources/code/Fibonacci.otne

# Dumps tokens only
./build/bin/otne --tokens resources/code/HelloWorld.otne
```

> The default sample path is relative to the current directory, so run from the project root.

### Options

| Option | Description |
| --- | --- |
| `-h`, `--help` | Show help |
| `-v`, `--version` | Show version |
| `-t`, `--tokens` | Dump tokens instead of executing |

## Example

```otne
// HelloWorld.otne
import stdio;

func main() {
    println("Hello, World!");
}
```

```text
$ otne resources/code/HelloWorld.otne
Hello, World!
```

## Features

- **Concise syntax**: Java / Go style, with English-only keywords
- **Optional type annotations**: `let n: int = 1`, `func half(v: float): float`
- **Dynamic values**: `int`, `float`, `string`, `bool`, `null`; `+` concatenates strings
- **Full control flow**: `if` / `elif` / `else`, `while`, C-style `for`, `break` / `continue` / `return`
- **First-class functions**: recursion and assignment to variables
- **Precise diagnostics**: lexical, syntax and runtime errors all report `file:line:column`

See the [language reference](docs/otne-language.md) for the complete grammar, or the machine-readable [`docs/otne.bnf`](docs/otne.bnf).

## Project Layout

```text
otne/
├── CMakeLists.txt          Top-level build script
├── Makefile                Convenience build script (wraps CMake)
├── otne/
│   ├── CMakeLists.txt
│   └── src/                lexer, parser, AST, values, interpreter, CLI
├── docs/otne-language.md   Language reference and grammar
├── docs/otne.bnf           BNF grammar (tokens and syntax)
├── tests/smoke.sh          Smoke tests
├── resources/
│   ├── code/               Sample programs
│   └── icons/              Icon assets
├── CHANGELOG.md
├── README.en.md
└── LICENSE
```

## Testing

```bash
cmake --build build
./tests/smoke.sh                 # uses build/bin/otne by default
./tests/smoke.sh build/bin/otne  # explicit binary path
```

Or simply run `make test` (or `make ctest`).

The smoke tests cover every sample plus lexical, syntax and runtime errors.

## Contributing

Issues and pull requests are welcome. Before submitting, make sure that:

1. The project builds: `cmake --build build`
2. Smoke tests pass: `./tests/smoke.sh`
3. New samples live under `resources/code/` and are listed in the README
4. Syntax changes update `docs/otne-language.md` and `docs/otne.bnf`

## License

Released under the [GNU General Public License v3.0](LICENSE).
