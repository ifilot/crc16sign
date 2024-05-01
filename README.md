# CRC16 Sign

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/crc16sign?label=version)
[![C/C++ CI](https://github.com/ifilot/crc16sign/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/crc16sign/actions/workflows/build.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Purpose
CRC-16 checksum signing tool to create binaries that yield a CRC-16 checksum
of `0x0000`.

## Compilation

```bash
mkdir build
cd build
cmake ../src
make -j
```

## Usage

```bash
crc16sign -i <INPUT> -o <OUTPUT> -s
```