#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
: "${NDLESS_SDK:?Set NDLESS_SDK to the official pinned SDK}"
: "${NSDL_ARCHIVE:?Set NSDL_ARCHIVE to your rebuilt or modified libSDL.a}"
export PATH="$NDLESS_SDK/toolchain/install/bin:$NDLESS_SDK/bin:$PATH"
test ! -e output || { echo 'Choose a fresh relink folder; output exists'; exit 1; }
mkdir output
units=(splash_anim main engine rf_engine lab_engine lora_engine display ui handbook math_layout numeric_input locale settings)
own=(); for unit in "${units[@]}"; do own+=("objects/$unit.o"); done
nspire-ld "${own[@]}" "$NSDL_ARCHIVE" -o output/c21-relinked.elf -lm -Wl,-Map,output/c21-relinked.map
genzehn --input output/c21-relinked.elf --output output/c21-relinked.zehn --name "C21 Engineering Suite"
make-prg output/c21-relinked.zehn output/c21-relinked.tns
sha256sum output/c21-relinked.tns
echo 'Locally relinked derivative; not the owner-accepted public executable. Physical testing remains separate.'
