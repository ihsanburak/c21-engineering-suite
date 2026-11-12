> Current release: **1.0.3**. Startup animation added; physical timing/execution remain unverified. Current binary and test evidence: [validation](VALIDATION.md) and [manifest](../dist/build-manifest.json). Earlier 1.0.2 size/test figures below are historical.

# Known limitations — 1.0 Classic Edition

| Area | Scope and limit |
|---|---|
| Hardware | Original monochrome Touchpad non-CAS, OS 3.9.0.463, working compatible Ndless. Other models/OS versions are unverified. Final build 1.0.2 center-click/language confirmation is pending. |
| Physical evidence | Owner-reported launch plus LC resonance, Ohm and mW-to-dBm examples. Not every calculation has been physically verified. RAM/timing and emulator acceptance are not certified. |
| LoRa | Legacy SX127x SF7–12, BW125/250/500 kHz. No SF5/6, narrow-band or other-generation-specific model. DE1 required when Ts>16 ms. Nominal bit rate excludes LDRO/packet overhead; application throughput is different. |
| RF legality | No legal band/power/duty preset, compliance certificate or communication-distance promise. Enter actual parameters and independently check applicable requirements. |
| Microstrip | Zero-thickness, lossless/quasi-static model in declared w/h and εr bounds. No solder-mask, dispersion, thermal or stripline correction. |
| Trace current | Legacy IPC-style empirical advisory estimate, not IPC-2152 or a safety rating. |
| Statistics | Exactly four normalized common-unit readings; no arbitrary-unit tracking, covariance, systematic-bias or confidence-factor inference. |
| SMD | Numeric three/four-digit and zero-jumper formats only. R/EIA-96/manufacturer formats are unsupported. Preferred E12/E24 values do not guarantee stock or tolerance. |
| Numerical | Finite double precision. A packet budget near a boundary may shift the last packet. Explicit formatter rejects subnormals. Some long models use List Mode or compact worked-example formulas. |
| Storage | Optional application-owned language sidecar. Legacy Unicode Turkish normalizes to ASCII on read, without an automatic file write; next explicit save persists it. |
| Instruments | Manual measurement entry. No VNA/radio control, measurement import or wireless communication. |

Check each calculator's H entry for assumptions, instrument requirements and common mistakes. Host checks do not establish physical model validity or component safety.
