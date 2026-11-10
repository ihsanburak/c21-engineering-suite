#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${NDLESS_SDK:?Set NDLESS_SDK to your official Ndless SDK directory}"
export PATH="$NDLESS_SDK/toolchain/install/bin:$NDLESS_SDK/bin:$PATH"
mkdir -p build
make
arm-none-eabi-readelf -h build/c21-engineering-suite.elf > build/arm-elf-header.txt
arm-none-eabi-readelf -A build/c21-engineering-suite.elf > build/arm-attributes.txt
arm-none-eabi-size build/c21-engineering-suite.elf > build/arm-size.txt
genzehn --info --input build/c21-engineering-suite.tns.zehn > build/zehn-info.txt
python3 tools/verify-native.py --loader "$(nspire-tools _zehn_loader_path '')"
