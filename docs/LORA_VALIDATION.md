# LoRa implementation and independent validation — retained 1.0.2 model evidence

Legacy SX127x scope: SF7–12; BW125/250/500kHz; CR index1–4 for4/5..4/8; physical PL0–255; explicit/implicit header; CRC; programmed preamble6–65535; explicit LDRO. SF5/6, narrower bandwidths, SX126x-specific behavior and other generations are outside the verified model. A configured modem is not assumed legal.

The production module `src/lora_engine.c` uses independent bounded integer ceiling/max arithmetic and SI double timing. All eight packet parameters are entered using the existing validated UI. DE1 is required when Ts>16ms; manual short-symbol DE1 is supported and independently checked. Nominal Rb=SF BW 4/(2^SF(4+CR)) excludes LDRO reduction and packet/protocol overhead, so it is not application throughput. PL includes MAC/application bytes and excludes modem-inserted header/CRC. Airtime excludes radio ramp, RX, retry, MCU and regulatory scheduling effects.

## Actual vendor oracles

1. [Pinned current Semtech SX1276 source](https://github.com/Lora-net/LoRaMac-node/blob/cf779a46f52f9b22395a207c6853f3efb47f0e5b/src/radio/sx1276/sx1276.c): the actual unchanged pure numerator function was compiled in a host-only harness. **4,320 exact rational matches** span6 SF×3 BW×4 CR×2 header×2 CRC×5 PL×3 preamble cases, including PL0/255 and preamble6/65535. This function auto-selects LDRO.
2. [Pinned older Semtech driver](https://github.com/Lora-net/LoRaMac-node/blob/f42be67be402a40b3586724800771bfe13fb18e6/src/radio/sx1276/sx1276.c): actual unchanged `SX1276GetTimeOnAir`, compiled with GCC and minimal settings stubs, matches **1,056 millisecond results**, including **480 short-symbol LDRO-enabled cases**. The unused FSK hardware calls are stubbed and never executed. This driver returns floor(1000T+0.999), so its agreement is explicitly millisecond agreement, not an invented exact sub-millisecond claim.

Both oracle sets also compare directly with the production C engine in `tests/test_lora.c`. Separate 70-digit Decimal references cover all12 targets and independently modified input values. The driver source is cached under ignored `build/reference`, is not copied into the native module and is not linked into the RC. [Official Semtech SX1276 product reference](https://www.semtech.com/products/wireless-rf/lora-connect/sx1276) identifies the modem family; the Salesforce-hosted datasheet was not reliably machine-readable here, so no claim is made that its inaccessible PDF was inspected. Verification relies on the pinned official implementations and explicitly stated equations.

Replay: `py -3.13 tools/verify-semtech-reference.py`; add `--fetch` only to retrieve the pinned official public source if cache is absent. Provenance/source hashes are in `tools/semtech-reference-provenance.json`. No radio or calculator is accessed.

## Key-event acceptance

| Entered settings | Verified result |
|---|---|
| SF7/BW125k/CR1/PL20/IH0/CRC1/pre8/DE0 | Ts1.024ms; pre12.544ms; payload44.032ms; packet56.576ms; Npay43 |
| Same, SF12/DE0 | Invalid configuration with LDRO guidance |
| Same, SF12/DE1 | Packet1.318912s |
| Same SF7, manual DE1 | Packet66.816ms |
| SF7/BW125k/CR1 nominal PHY | Rs976.5625sym/s; Rb5468.75bit/s; coding fraction80% |
| 56.576ms packet /10s interval | Occupied fraction0.56576% |
| 3600s window, entered1% personal budget | 636 theoretical packets, not a legal preset |
| Battery-side165mW×56.576ms | 9.33504mJ |

Boundary cases include noninteger/unsupported SF/BW/CR, PL256, nonbinary flags, invalid preamble, long-symbol DE0, code-fraction mismatch, zero interval, airtime beyond interval, overflow/underflow and counts above2^53−1. Max supported integer packet count renders all digits instead of rounding to10 significant digits. Counts close to a budget boundary retain finite-double limitations.

The new suite executes **68,127 assertions /17,118 rendered frames**, including real input and editing for every new target, all units, output switching, Handbook, languages and visible known-limit notices. Assertion count is not mathematical proof: independent-reference scope, equations, units and model assumptions are the correctness evidence. No physical radio timing or calculator verification is claimed.
