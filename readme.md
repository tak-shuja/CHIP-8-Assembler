
# 🛠️ CHIP-8 Assembler (c8asm)

A custom CHIP-8 assembler that compiles CHIP-8 assembly programs into `.ch8` binary ROMs.
**Note:** This assembler currently does **not** support `db` directives or embedding sprites directly. It assumes sprites are pre-loaded in the emulator’s memory.

---

## Overview

This project builds upon my [CHIP-8 Emulator](https://github.com/tak-shuja/CHIP-8-Emulator.git) by adding a dedicated assembler, completing the toolchain for CHIP-8 development.

---

## Project Structure

```bash
.
├── src
│   ├── main.c
│   ├── assembler.c
│   └── include
│       └── assembler.h
├── out
│   └── c8asm            # compiled assembler executable
├── programs
│   └── LoopDraw.asm     # example CHIP-8 assembly program
└── Makefile
````

---

## Prerequisites

* **libsdl2-dev** (Linux) — required for building and running the emulator.

Install on Debian/Ubuntu with:

```bash
sudo apt-get install libsdl2-dev
```

---

## Building

Run the following command to compile the assembler:

```bash
make
```

This produces the executable `out/c8asm`.

---

## Usage

To assemble a CHIP-8 program, use:

```bash
./out/c8asm input.asm output.ch8
```

Example:

```bash
./out/c8asm programs/LoopDraw.asm programs/LoopDraw.ch8
```

---

## Example

The `programs/LoopDraw.asm` file showcases sprite drawing in a loop with delay timers using basic CHIP-8 instructions.
Feel free to review and modify this example.

---

## Roadmap

* Support for `db` directives to embed raw sprite data within assembly source.
* Enhanced error handling and extended instruction set support.

---

## Contributing & Feedback

Contributions, suggestions, and feedback are welcome! Feel free to open issues or pull requests.

---

Made with passion by Shuja Ahmad Tak
[GitHub](https://github.com/tak-shuja/)
