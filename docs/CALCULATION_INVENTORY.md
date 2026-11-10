# Calculation inventory — 1.0 Classic Edition

Audit date: 2026-11-10. **138 interactive targets in 64 documented model/workflow families**, including one formatting utility. Alternative outputs, topology, language and display modes are not independently counted as mathematical families. All 126 accepted metadata rows, 756 Handbook prose fields and 1,065 translations remain unchanged. Original RF/lab/DC numerical implementations and Eagle assets are preserved.

Evidence flags below describe executed host scenarios and available independent benchmark sources, not proof of mathematical correctness over every real number or physical validity of ideal models. Reference quality and model limitations matter more than assertion count. Every row has actual key-event input/edit/recalculation, unit selection, contextual Handbook return and English/Turkce ASCII coverage (retained Unicode resource checks) through the retained suites or the new LoRa suite.

| ID / key | Target | Visual mode | Output selector | Independent reference |
|---|---|---|---|---|
| 0 / `VOLTAGE` | Ohm: voltage | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 1 / `CURRENT` | Ohm: current | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 2 / `RESISTANCE` | Ohm: resistance | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 3 / `POWER` | Electrical power | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 4 / `MW_DBM` | mW to dBm | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 5 / `DBM_MW` | dBm to mW | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 6 / `WAVELENGTH` | Wavelength | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 7 / `QUARTER_WAVE` | Quarter-wave antenna | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 8 / `LC_FREQ` | LC resonance | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 9 / `LC_CAP` | Required capacitance | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 10 / `XL` | Inductive reactance | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 11 / `XC` | Capacitive reactance | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 12 / `FSPL` | Free-space path loss | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 13 / `FRESNEL` | First Fresnel radius | Formula + List | Not applicable | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 14 / `LC_INDUCTANCE` | Required inductance | Formula + List | Yes | Core hand-derived/Python vectors: tests/test_host.c, tests/interactive_vectors.h, tests/formula_editor_refs.h |
| 15 / `POWER_IR` | Resistor power: I²R | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 16 / `POWER_VR` | Resistor power: V²/R | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 17 / `R_SERIES` | Series resistance | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 18 / `R_PARALLEL` | Parallel resistance | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 19 / `V_DIVIDER` | Voltage divider | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 20 / `I_DIVIDER` | Current divider | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 21 / `KVL_REMAINDER` | KVL: remaining voltage | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 22 / `KCL_REMAINDER` | KCL: remaining current | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 23 / `LED_RESISTOR` | LED series resistor | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 24 / `CAP_ENERGY` | Capacitor stored energy | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 25 / `RC_CHARGE` | RC capacitor charging | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 26 / `RC_DISCHARGE` | RC capacitor discharging | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 27 / `RLC_MAG` | Series RLC impedance | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 28 / `RLC_PHASE` | Series RLC phase | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 29 / `RC_TAU` | RC time constant | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 30 / `RL_TAU` | RL time constant | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 31 / `AH_WH` | Ah to Wh energy | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 32 / `WH_AH` | Wh to Ah capacity | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 33 / `AVG_CURRENT` | Average current | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 34 / `BAT_RUNTIME` | Estimated battery runtime | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 35 / `DUTY_POWER` | Duty-cycle average power | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 36 / `LDO_LOSS` | LDO power dissipation | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 37 / `DCDC_IN` | DC-DC input power | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 38 / `DCDC_OUT` | DC-DC output power | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 39 / `DCDC_EFF` | DC-DC efficiency | Formula + List | Yes | Phase 3E-1 Decimal vectors |
| 40 / `RES_RATING` | Resistor power rating | Formula + List | Not applicable | Phase 3E-1 Decimal vectors |
| 41 / `RF_DBW_DBM` | dBW to dBm | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 42 / `RF_DBM_DBW` | dBm to dBW | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 43 / `RF_GAIN_DB` | Power ratio to dB | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 44 / `RF_GAIN_RATIO` | dB to power ratio | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 45 / `RF_AMP_DB` | Amplitude ratio to dB | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 46 / `RF_AMP_RATIO` | dB to amplitude ratio | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 47 / `RF_POWER_RATIO` | RF measured power ratio | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 48 / `RF_VRMS` | RF power to RMS voltage | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 49 / `RF_GAIN_SUM` | Cascaded gain and loss | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 50 / `RF_LOSS_SUM` | Cascaded insertion loss | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 51 / `RF_EIRP` | EIRP calculation | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 52 / `RF_ERP` | EIRP to ERP | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 53 / `RF_FROM_ERP` | ERP to EIRP | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 54 / `RF_DBI` | dBd to dBi | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 55 / `RF_DBD` | dBi to dBd | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 56 / `RF_NOISE_W` | Thermal receiver noise W | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 57 / `RF_NOISE_DBM` | Receiver noise floor dBm | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 58 / `RF_SNR` | Measured SNR | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 59 / `RF_NOISE_SNR` | Noise from signal and SNR | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 60 / `RF_NF_FACTOR` | Noise figure to factor | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 61 / `RF_FACTOR_NF` | Noise factor to figure | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 62 / `RF_FRIIS_F` | Friis cascade noise factor | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 63 / `RF_FRIIS_NF` | Friis cascade noise figure | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 64 / `RF_SENS` | Required receiver sensitivity | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 65 / `RF_HALF` | Half-wave electrical length | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 66 / `RF_SPEED` | Lossless dielectric velocity | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 67 / `RF_DIEL_WAVE` | Dielectric wavelength | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 68 / `RF_LINK` | Full RF link budget | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 69 / `RF_MARGIN` | Receiver link margin | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 70 / `RF_CLEAR` | Fresnel clearance percentage | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 71 / `RF_HORIZON` | Effective-Earth radio horizon | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 72 / `RF_POL` | Linear polarization loss | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 73 / `RF_PHASE` | Electrical phase length | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 74 / `RF_DELAY` | Propagation phase shift | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 75 / `RF_RC_CUTOFF` | RC cutoff: low and high pass | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 76 / `RF_LP` | First-order low-pass magnitude | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 77 / `RF_HP` | First-order high-pass magnitude | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 78 / `RF_SERIES_Q` | Series resonator Q | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 79 / `RF_SERIES_BW` | Series resonator bandwidth | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 80 / `RF_REFLECT` | Complex load reflection | List + visual equation | Not applicable | Phase 3E-2 Decimal vectors |
| 81 / `RF_VSWR` | VSWR from reflection | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 82 / `RF_RETURN` | Return loss | Formula + List | Not applicable | Phase 3E-2 Decimal vectors |
| 83 / `RF_IMPEDANCE` | Reflection to impedance | List + visual equation | Not applicable | Phase 3E-2 Decimal vectors |
| 84 / `RF_LMATCH_LP` | Low-pass resistive L-match | List + visual equation | Not applicable | Phase 3E-2 Decimal vectors |
| 85 / `RF_LMATCH_HP` | High-pass resistive L-match | List + visual equation | Not applicable | Phase 3E-2 Decimal vectors |
| 86 / `RF_W_DBW` | W to dBW | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 87 / `RF_DBW_W` | dBW to W | Formula + List | Yes | Phase 3E-2 Decimal vectors |
| 88 / `PCB_TRACE_R` | Copper trace resistance | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 89 / `PCB_IPC_EXT` | IPC-2221 external estimate | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 90 / `PCB_IPC_INT` | IPC-2221 internal estimate | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 91 / `PCB_FOIL_T` | Copper weight to thickness | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 92 / `PCB_FOIL_M` | Thickness to copper weight | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 93 / `PCB_VIA_R` | Plated via resistance | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 94 / `PCB_MS_EPS` | Microstrip effective ε | List + visual equation | Yes | Phase 3E-3 Decimal vectors |
| 95 / `PCB_MS_Z` | Microstrip impedance | List + visual equation | Yes | Phase 3E-3 Decimal vectors |
| 96 / `PCB_DELAY` | Guided propagation delay | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 97 / `VNA_REFLECTED` | Reflected power fraction | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 98 / `VNA_DELIVERED` | Delivered power fraction | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 99 / `VNA_MISMATCH` | Mismatch loss | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 100 / `VNA_RL_GAMMA` | Return loss to reflection | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 101 / `VNA_S21_DB` | S21 dB to insertion loss | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 102 / `VNA_S21_LIN` | S21 linear insertion loss | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 103 / `VNA_Z_MAG` | Complex impedance magnitude | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 104 / `VNA_Z_PHASE` | Complex impedance phase | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 105 / `LAB_E12` | E12 nearest resistor | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 106 / `LAB_E24` | E24 nearest resistor | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 107 / `LAB_SMD3` | SMD 3 digit code | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 108 / `LAB_SMD4` | SMD 4 digit code | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 109 / `LAB_TOL_R` | Resistor tolerance limits | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 110 / `LAB_TOL_C` | Capacitor tolerance limits | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 111 / `LAB_TOL_L` | Inductor tolerance limits | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 112 / `LAB_DIV_MIN` | Divider worst-case minimum | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 113 / `LAB_DIV_MAX` | Divider worst-case maximum | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 114 / `LAB_RC_MIN` | RC worst-case minimum | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 115 / `LAB_RC_MAX` | RC worst-case maximum | Formula + List | Yes | Phase 3E-3 Decimal vectors |
| 116 / `LAB_ABS_ERROR` | Absolute scalar error | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 117 / `LAB_REL_ERROR` | Relative scalar error | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 118 / `LAB_MEAN` | Mean: 4 readings | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 119 / `LAB_POP_SD` | Population SD: 4 readings | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 120 / `LAB_SAMPLE_SD` | Sample SD: 4 readings | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 121 / `LAB_MEAN_U` | Mean standard uncertainty | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 122 / `LAB_RECT_U` | Rectangular standard u | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 123 / `LAB_COMBINED_U` | Independent combined u | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 124 / `LAB_EXPANDED_U` | Expanded uncertainty | Formula + List | Not applicable | Phase 3E-3 Decimal vectors |
| 125 / `LAB_FORMAT` | Significant digits / SI | List + visual equation | Not applicable | Phase 3E-3 Decimal vectors |
| 126 / `LORA_SYMBOL_T` | LoRa symbol duration | Formula + List | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 127 / `LORA_SYMBOL_RATE` | LoRa symbol rate | Formula + List | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 128 / `LORA_BITRATE` | LoRa nominal bit rate | Formula + List | Not applicable | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 129 / `LORA_CODE_RATE` | LoRa coding fraction | Formula + List | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 130 / `LORA_CODE_INDEX` | LoRa coding index | Formula + List | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 131 / `LORA_AIRTIME` | LoRa packet airtime | List + visual equation | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 132 / `LORA_PREAMBLE_T` | LoRa preamble duration | Formula + List | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 133 / `LORA_PAYLOAD_T` | LoRa payload duration | List + visual equation | Yes | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 134 / `LORA_PAYLOAD_SYMBOLS` | LoRa payload symbols | List + visual equation | Not applicable | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 135 / `LORA_DUTY` | Packet duty fraction | Formula + List | Not applicable | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 136 / `LORA_PACKET_COUNT` | Packet budget count | Formula + List | Not applicable | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |
| 137 / `LORA_TX_ENERGY` | Battery TX energy | Formula + List | Not applicable | Phase 3F Decimal vectors; packet benchmark additionally uses two pinned Semtech implementations |

## Families

| Model/workflow family | Output target IDs |
|---|---|
| Ohm V/I/R | 0, 1, 2 |
| Resistive electrical power / RMS | 3, 15, 16, 48 |
| Absolute power references | 4, 5, 41, 42, 86, 87 |
| Free-space wavelength / electrical length | 6, 7, 65 |
| Ideal LC resonance and inverses | 8, 9, 14 |
| Inductive reactance | 10 |
| Capacitive reactance | 11 |
| Free-space path loss | 12 |
| First Fresnel radius | 13 |
| Series resistance | 17 |
| Parallel resistance | 18 |
| Unloaded voltage divider | 19 |
| Two-branch current divider | 20 |
| KVL residual | 21 |
| KCL residual | 22 |
| LED resistor | 23 |
| Capacitor energy | 24 |
| Ideal RC transient voltage | 25, 26 |
| Series RLC magnitude/phase/Q/bandwidth | 27, 28, 78, 79 |
| RC time constant / cutoff | 29, 75 |
| RL time constant | 30 |
| Battery charge / energy | 31, 32 |
| Charge-current-time / runtime estimate | 33, 34 |
| Two-state average power | 35 |
| LDO dissipation | 36 |
| DC-DC power / efficiency | 37, 38, 39 |
| Resistor rating | 40 |
| Power/amplitude gain and ratios | 43, 44, 45, 46, 47, 102 |
| Logarithmic RF cascade / radiated / receive budget / margin | 49, 50, 51, 68, 69, 101 |
| Isotropic / dipole reference | 52, 53, 54, 55 |
| Input-referred thermal receiver noise | 56, 57 |
| SNR / noise / sensitivity | 58, 59, 64 |
| Noise figure / factor | 60, 61 |
| Three-stage Friis cascade | 62, 63 |
| Lossless dielectric velocity / wavelength | 66, 67, 96 |
| Fresnel clearance fraction | 70 |
| Effective-Earth horizon | 71 |
| Ideal linear-polarization mismatch | 72 |
| Electrical phase length / delay | 73, 74 |
| First-order RC LP/HP response | 76, 77 |
| Passive complex impedance / reflection transform | 80, 83, 103, 104 |
| VSWR | 81 |
| Return loss | 82, 100 |
| Real-resistance L-match LP/HP | 84, 85 |
| Uniform conductor geometry resistance | 88, 93 |
| Legacy IPC-2221 current estimate | 89, 90 |
| Copper mass per area / thickness | 91, 92 |
| Zero-thickness microstrip | 94, 95 |
| Reflection power balance / mismatch | 97, 98, 99 |
| Preferred E12/E24 lookup | 105, 106 |
| Numeric SMD decode | 107, 108 |
| Nominal component tolerance bounds | 109, 110, 111 |
| Unloaded divider tolerance corners | 112, 113 |
| RC tolerance corners | 114, 115 |
| Reference error | 116, 117 |
| Four-reading statistics / Type A mean uncertainty | 118, 119, 120, 121 |
| Rectangular Type B uncertainty | 122 |
| Independent uncertainty combination | 123 |
| Expanded uncertainty | 124 |
| Significant-digit formatting utility | 125 |
| LoRa symbol/rate/coding workflow | 126, 127, 128, 129, 130 |
| Legacy SX127x packet workflow | 131, 132, 133, 134 |
| Entered airtime budget workflow | 135, 136 |
| Battery energy per transmission | 137 |

## Disposition of limitations and terminology

- Zero-thickness microstrip, legacy IPC advisory estimates, four-reading normalized statistics and numeric-only SMD remain bounded models. Relevant input/results screens now expose these limits directly; their existing numeric/domain tests are retained.
- Nominal LoRa bit rate excludes LDRO reduction and packet/protocol overhead. Packet payload length includes MAC/application bytes but excludes modem-inserted header and CRC. Header/CRC/LDRO are explicit flags.
- Handbook topic 2 is now labelled RF / VNA / LoRa reference so the new material is discoverable without changing numeric routes. Discrete/auxiliary List views no longer falsely say their formulas are too large.
- Numeric aliases across categories reuse existing IDs and are not extra calculators. No unintended per-menu duplicate targets are accepted by traversal tests.
- Packet-budget integers render without 10-digit rounding; physical duty legality is not inferred. Source-safe bounded editors remain unchanged.
