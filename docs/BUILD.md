# Build and reproduce

The source builds application **1.0.3**, including the animated opening screen. Calculation engines are retained from 1.0.2.

## Native ARM

Use a compatible Linux development environment (including an existing WSL development distribution on Windows), the official Ndless SDK at commit `37d61aa22762171ffc9b0abf2565f13d09aec70f`, ARM GCC **14.2.0**, Binutils **2.44** and Newlib **4.5.0.20241231**. The SDK includes nSDL 1.1.1. Read [dependency notices](../THIRD_PARTY_NOTICES.md) before distributing a rebuilt executable. No provisioning or firmware action is automated here.

Set `NDLESS_SDK` to your own SDK directory and run `bash tools/build-native.sh`. The portable wrapper adds SDK wrappers/compiler tools to PATH, runs the original Makefile and verifies the ARM ELF and exact official-loader/Zehn packaging. Outputs remain under ignored `build/`. You may also place the genuine SDK tools on PATH and run `make` directly.

The Makefile compiles C in ARM mode, links with nSDL/runtime and libm, produces an ELF, runs `genzehn` then `make-prg`. It never creates a placeholder document or renames arbitrary data into an executable. Firmware/Ndless installation is not part of this build.

## Host regression

Linux: `make test HOST_CC=gcc`. Sanitizers: `bash tools/test-sanitized.sh` using GCC with ASan/UBSan. Windows: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/test-host.ps1 -Compiler <your-tcc.exe>` with an existing TinyCC toolchain. A compiler is not bundled. Python 3 is used for asset/resource scripts; Pillow is used for previews and publication artwork conversion only, not the application.

All thirteen test sources and their vector headers are included. The original bitmap font generator does not extract a system font. Source translations remain Unicode while ASCII is applied at display time. Test-only SDK key constants and SDL/OS stubs compile the real native input adapter; these do not emulate Touchpad electronics or replace genuine SDK headers in an ARM build.

## Reproducibility scope

The preparation report records whether a clean build from this staged source, using the installed pinned toolchain, matches the published 1.0.3 executable. That is a same-environment comparison, not a claim of arbitrary-machine or future-toolchain reproducibility. Pinned component source/relink/notices are included under third_party/ and relink/; preserve these when redistributing. Consult `dist/build-manifest.json`, docs/REPRODUCIBILITY.json and third_party/REDISTRIBUTION.md for actual hashes/evidence. No CI auto-publication, network credentials or calculator communication is configured.
