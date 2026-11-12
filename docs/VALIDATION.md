# Validation status — 1.0.3

2026-11-12: **569,912 checks per platform**, Windows TinyCC and GCC ASan/UBSan, 13 suites. Includes 1,909 splash animation checks and 495 native input-loop checks. All suites passed; sanitizer run completed without reported errors. Logs: [Windows](test-evidence/v103-windows.log), [GCC ASan/UBSan](test-evidence/v103-sanitized.log).

The 56-frame animation was checked in all three internal language modes for deterministic rendering, framebuffer bounds, original bitmap preservation and final suffix placement. Input tests run the real native adapter/main loop with mocked OS/SDL boundaries. These are host tests, not hardware emulation.

The LoRa implementation retains its 5,376 pinned Semtech reference cases. See [LoRa validation](LORA_VALIDATION.md). Model assumptions and input-domain limits still apply.

## Native build

ELF32 ARM and the official 492-byte Ndless loader plus Zehn packaging were verified. A forced rebuild in the same pinned installed SDK/toolchain produced the same SHA-256: `61730cf61148e830245a9e7948187053985d92f19d20e9ed9b68abd5d6b36006` (628,560 bytes). This is same-environment reproducibility, not arbitrary-machine reproduction.

## Physical status

Physical execution and animation timing of **1.0.3 remain unverified**. Earlier owner-reported launch, LC resonance, Ohm's law and mW-to-dBm examples apply to earlier builds. All 138 targets, peak physical RAM/timing and every hardware/OS configuration have not been physically verified.

No firmware or Ndless installer is included. Read [known limitations](KNOWN_LIMITATIONS.md) and [redistribution/relink instructions](../third_party/REDISTRIBUTION.md).
