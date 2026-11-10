# Validation status

## Host verified

Accepted build **1.0.2**: **567,737 checks per platform**, Windows and GCC ASan/UBSan, twelve suites. The first eleven retained suites account for 565,478 checks; two-choice language/migration adds 2,259. Independent numerical references, key-event input/edit/recalculation, units, Formula/List modes, Handbook state, translations and framebuffer bounds are covered. Sanitizer results are desktop evidence, not a measurement of calculator RAM use.

The LoRa implementation was compared against **5,376 cases** from two pinned actual Semtech SX1276 reference implementations. See [LoRa validation](LORA_VALIDATION.md) for model scope and reference commits. High check counts alone do not establish mathematical correctness outside documented cases/domains.

Native-input tests include the real adapter and main loop with mocked OS/SDL boundaries; held/release dispatch behavior is modeled, not physical I2C emulation. The accepted packet/memory/resource observations are summarized in [technical specifications](TECHNICAL_SPECIFICATIONS.md).

## ARM compiled

The genuine ELF32 ARM build and official 492-byte Ndless compatibility-loader plus Zehn packaging were verified. The public edition package must match the accepted V1.0.2 SHA-256 exactly. The local publication handoff records the clean-build comparison and toolchain scope. No TI firmware or Ndless installer is included.

## Physical owner feedback

- Application launch on the owner's original classic TI-Nspire: **passed, owner reported**.
- LC resonance, Ohm's law and mW-to-dBm: **three owner-assisted numerical examples passed**.
- V1.0.2 center-click and simplified English/Turkce ASCII language behavior: **final owner confirmation pending**.
- All 138 targets, peak physical RAM/timing, all storage/error flows and other hardware/OS configurations: **not fully physically verified**.

Do not present this as hardware certification or universal verification. Device results are owner reports; no photographs/raw logs of those three tests were supplied in this preparation. Record the tested binary hash, actual key sequence, values/units and observations in the final acceptance sheet.
