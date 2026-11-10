#include "engine.h"
#include "rf_engine.h"
#include "lab_engine.h"
#include "lora_engine.h"
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#define PI 3.14159265358979323846
#define C0 299792458.0
#define N(a) (sizeof(a)/sizeof((a)[0]))
static const Unit volts[]={{"V","V",1},{"mV","mV",1e-3}};
static const Unit amps[]={{"A","A",1},{"mA","mA",1e-3},{"uA","μA",1e-6}};
static const Unit ohms[]={{"ohm","Ω",1},{"kohm","kΩ",1e3},{"Mohm","MΩ",1e6}};
static const Unit watts[]={{"W","W",1},{"mW","mW",1e-3}};
static const Unit dbm[]={{"dBm","dBm",1}};
static const Unit hz[]={{"Hz","Hz",1},{"kHz","kHz",1e3},{"MHz","MHz",1e6},{"GHz","GHz",1e9}};
static const Unit metres[]={{"m","m",1},{"cm","cm",.01},{"km","km",1000}};
static const Unit henry[]={{"H","H",1},{"mH","mH",1e-3},{"uH","μH",1e-6},{"nH","nH",1e-9}};
static const Unit farad[]={{"F","F",1},{"uF","μF",1e-6},{"nF","nF",1e-9},{"pF","pF",1e-12}};
static const Unit decibel[]={{"dB","dB",1}};
static const Unit joules[]={{"J","J",1},{"mJ","mJ",.001},{"Wh","Wh",3600},{"mWh","mWh",3.6}};
static const Unit coulombs[]={{"C","C",1},{"Ah","Ah",3600},{"mAh","mAh",3.6}};
static const Unit seconds[]={{"s","s",1},{"ms","ms",1e-3},{"us","μs",1e-6},{"min","min",60},{"h","h",3600}};
static const Unit ratio[]={{"fraction","1",1},{"percent","%",.01}};
static const Unit radians[]={{"rad","rad",1},{"deg","°",0.01745329251994329577}};
static const Unit dbw[]={{"dBW","dBW",1}};
static const Unit dbi[]={{"dBi","dBi",1}};
static const Unit dbd[]={{"dBd","dBd",1}};
static const Unit kelvin[]={{"K","K",1}};
static const Unit speed[]={{"m/s","m/s",1}};
static const Unit signedratio[]={{"1","1",1}};
static const Unit pcb_ohms[]={{"ohm","Ω",1},{"milliohm","mΩ",.001}};
static const Unit geometry[]={{"m","m",1},{"mm","mm",.001},{"um","μm",1e-6},{"mil","mil",.0000254}};
static const Unit resistivity[]={{"ohm m","Ωm",1},{"nano ohm m","nΩm",1e-9}};
static const Unit rise[]={{"kelvin rise","K",1},{"celsius rise","Δ°C",1}};
static const Unit foil[]={{"kg/m2","kg/m²",1},{"oz/ft2","oz/ft²",.30515172727434975}};
static const Unit scalar[]={{"1","1",1}};
static const Unit symbols[]={{"symbols","sym",1}};
static const Unit symbolrate[]={{"symbols/s","sym/s",1},{"ksymbols/s","ksym/s",1000}};
static const Unit bitrate[]={{"bits/s","bit/s",1},{"kbits/s","kbit/s",1000}};
static const Unit packetcount[]={{"packets","pkt",1}};
#define P(s,u) {s,u,N(u)}
#define OUT(u) u,N(u)
const Calculation calculations[CALC_COUNT]={
 {"Ohm: voltage","V = IR","V: volts; I: amperes; R: ohms","DC or purely resistive load.",2,{P("I",amps),P("R",ohms)},OUT(volts)},
 {"Ohm: current","I = V/R","I: amperes; V: volts; R: ohms","DC or purely resistive load.",2,{P("V",volts),P("R",ohms)},OUT(amps)},
 {"Ohm: resistance","R = V/I","R: ohms; V: volts; I: amperes","Nonnegative passive resistance.",2,{P("V",volts),P("I",amps)},OUT(ohms)},
 {"Electrical power","P = VI","P: watts; V: volts; I: amperes","DC; AC requires unity power factor.",2,{P("V",volts),P("I",amps)},OUT(watts)},
 {"mW to dBm","dBm = 10 log10(P/1mW)","P: watts; dBm: power re 0.001W","Absolute RF power conversion.",1,{P("P",watts)},OUT(dbm)},
 {"dBm to mW","P = 1mW × 10^(dBm/10)","P: watts internally; dBm: re 1mW","Select mW for milliwatt output.",1,{P("dBm",dbm)},OUT(watts)},
 {"Wavelength","λ = c/f","λ: metres; f: Hz; c: 299792458 m/s","Vacuum/free-space wavelength.",1,{P("f",hz)},OUT(metres)},
 {"Quarter-wave antenna","l = λ/4 = c/(4f)","l: metres; f: Hz; c: light speed","Ideal length; no end/velocity factor.",1,{P("f",hz)},OUT(metres)},
 {"LC resonance","f₀ = 1/(2π√(LC))","f₀: Hz; L: henries; C: farads","Ideal lumped LC, no parasitics.",2,{P("L",henry),P("C",farad)},OUT(hz)},
 {"Required capacitance","C = 1/((2πf)²L)","C: farads; f: Hz; L: henries","Ideal LC resonance design.",2,{P("f",hz),P("L",henry)},OUT(farad)},
 {"Inductive reactance","X_L = 2πfL","X_L: ohms; f: Hz; L: henries","Z_L = jX_L; ideal inductor.",2,{P("f",hz),P("L",henry)},OUT(ohms)},
 {"Capacitive reactance","X_C = 1/(2πfC)","X_C: ohms; f: Hz; C: farads","Magnitude; Z_C = -jX_C.",2,{P("f",hz),P("C",farad)},OUT(ohms)},
 {"Free-space path loss","L = 20 log10(4πdf/c)","L: dB; d: metres; f: Hz","Far field, isotropic, clear path.",2,{P("d",metres),P("f",hz)},OUT(decibel)},
 {"First Fresnel radius","r₁ = √(λd₁d₂/(d₁+d₂))","r₁,d₁,d₂: metres; f: Hz","Radius at point; full first zone.",3,{P("f",hz),P("d₁",metres),P("d₂",metres)},OUT(metres)},
 {"Required inductance","L = 1/((2πf)²C)","L: henries; f: Hz; C: farads","Ideal LC resonance inverse.",2,{P("f",hz),P("C",farad)},OUT(henry)},
 {"Resistor power: I²R","P = I²R","I: Current (A); R: Resistance (Ω); P: Linear power (W)","Compute heat dissipated by a resistor from current and resistance.",2,{P("I",amps),P("R",ohms)},OUT(watts)},
 {"Resistor power: V²/R","P = V²/R","V: Voltage (V); R: Resistance (Ω); P: Linear power (W)","Compute resistor dissipation from the voltage across it.",2,{P("V",volts),P("R",ohms)},OUT(watts)},
 {"Series resistance","R = R₁ + R₂","R₁: First resistance (Ω); R₂: Second resistance (Ω); R: Resistance (Ω)","Reduce two series resistors to one equivalent resistance; repeat for a longer series chain.",2,{P("R₁",ohms),P("R₂",ohms)},OUT(ohms)},
 {"Parallel resistance","R = R₁R₂/(R₁+R₂)","R₁: First resistance (Ω); R₂: Second resistance (Ω); R: Resistance (Ω)","Reduce two parallel resistors. Apply repeatedly to additional parallel branches.",2,{P("R₁",ohms),P("R₂",ohms)},OUT(ohms)},
 {"Voltage divider","V_o = V_i R₂/(R₁+R₂)","V_i: Input voltage (V); R₁: First resistance (Ω); R₂: Second resistance (Ω); V_o: Output voltage (V)","Find the unloaded voltage across the lower resistor R₂.",3,{P("V_i",volts),P("R₁",ohms),P("R₂",ohms)},OUT(volts)},
 {"Current divider","I₁ = I_s R₂/(R₁+R₂)","I_s: Total current (A); R₁: First resistance (Ω); R₂: Second resistance (Ω); I₁: First branch current (A)","Find current in branch R₁ of two parallel resistors.",3,{P("I_s",amps),P("R₁",ohms),P("R₂",ohms)},OUT(amps)},
 {"KVL: remaining voltage","V₃ = V_s - V₁ - V₂","V_s: Source voltage (V); V₁: First voltage drop (V); V₂: Second voltage drop (V); V₃: Remaining voltage (V)","Balance one loop: source equals three signed voltage drops. A negative residual reverses the assumed polarity.",3,{P("V_s",volts),P("V₁",volts),P("V₂",volts)},OUT(volts)},
 {"KCL: remaining current","I₃ = I_s - I₁ - I₂","I_s: Total current (A); I₁: First branch current (A); I₂: Second branch current (A); I₃: Remaining current (A)","Balance a node: entering current equals three signed outgoing currents. Negative residual means opposite direction.",3,{P("I_s",amps),P("I₁",amps),P("I₂",amps)},OUT(amps)},
 {"LED series resistor","R = (V_s - V_f)/I","V_s: Source voltage (V); V_f: LED forward voltage (V); I: Current (A); R: Resistance (Ω)","Estimate a current-limiting series resistor for one LED using a specified forward voltage.",3,{P("V_s",volts),P("V_f",volts),P("I",amps)},OUT(ohms)},
 {"Capacitor stored energy","E = CV²/2","C: Capacitance (F); V: Voltage (V); E: Energy (J)","Compute electrostatic energy stored in an ideal capacitor.",2,{P("C",farad),P("V",volts)},OUT(joules)},
 {"RC capacitor charging","V_c = V_s(1-e^(-t/(RC)))","V_s: Source voltage (V); R: Resistance (Ω); C: Capacitance (F); t: Elapsed time (s); V_c: Capacitor voltage (V)","Find capacitor voltage after a DC step from zero initial voltage.",4,{P("V_s",volts),P("R",ohms),P("C",farad),P("t",seconds)},OUT(volts)},
 {"RC capacitor discharging","V_c = V₀e^(-t/(RC))","V₀: Initial voltage (V); R: Resistance (Ω); C: Capacitance (F); t: Elapsed time (s); V_c: Capacitor voltage (V)","Find capacitor voltage after disconnecting the source and discharging through R.",4,{P("V₀",volts),P("R",ohms),P("C",farad),P("t",seconds)},OUT(volts)},
 {"Series RLC impedance","|Z| = √(R²+(X_L-X_C)²)","R: Resistance (Ω); L: Inductance (H); C: Capacitance (F); f: Frequency (Hz); |Z|: Impedance magnitude (Ω)","Find series impedance Z = R + j(X_L-X_C); the primary result is magnitude.",4,{P("R",ohms),P("L",henry),P("C",farad),P("f",hz)},OUT(ohms)},
 {"Series RLC phase","φ = atan2(X_L-X_C,R)","R: Resistance (Ω); L: Inductance (H); C: Capacitance (F); f: Frequency (Hz); φ: Impedance phase (rad)","Find phase of voltage relative to current for a series RLC network.",4,{P("R",ohms),P("L",henry),P("C",farad),P("f",hz)},OUT(radians)},
 {"RC time constant","τ = RC","R: Resistance (Ω); C: Capacitance (F); τ: Time constant (s)","Find the characteristic RC charging/discharging time scale.",2,{P("R",ohms),P("C",farad)},OUT(seconds)},
 {"RL time constant","τ = L/R","L: Inductance (H); R: Resistance (Ω); τ: Time constant (s)","Find the characteristic current rise/decay time of a first-order RL circuit.",2,{P("L",henry),P("R",ohms)},OUT(seconds)},
 {"Ah to Wh energy","E = QV","Q: Charge capacity (C); V: Voltage (V); E: Energy (J)","Convert charge capacity and representative battery voltage into estimated energy.",2,{P("Q",coulombs),P("V",volts)},OUT(joules)},
 {"Wh to Ah capacity","Q = E/V","E: Energy (J); V: Voltage (V); Q: Charge capacity (C)","Infer equivalent charge capacity from energy and representative voltage.",2,{P("E",joules),P("V",volts)},OUT(coulombs)},
 {"Average current","I = Q/t","Q: Charge capacity (C); t: Elapsed time (s); I: Current (A)","Find average discharge current from charge consumed over an interval.",2,{P("Q",coulombs),P("t",seconds)},OUT(amps)},
 {"Estimated battery runtime","t = kQ/I","Q: Charge capacity (C); I: Current (A); k: Usable fraction (1); t: Elapsed time (s)","Estimate runtime from rated charge, usable fraction and average BATTERY current.",3,{P("Q",coulombs),P("I",amps),P("k",ratio)},OUT(seconds)},
 {"Duty-cycle average power","P = DP_on + (1-D)P_off","P_on: Active power (W); P_off: Inactive power (W); D: Duty fraction (1); P: Linear power (W)","Average a repeating two-state power profile by its active-time fraction.",3,{P("P_on",watts),P("P_off",watts),P("D",ratio)},OUT(watts)},
 {"LDO power dissipation","P = (V_i-V_o)I","V_i: Input voltage (V); V_o: Output voltage (V); I: Current (A); P: Linear power (W)","Estimate linear-regulator heat from voltage drop and load current.",3,{P("V_i",volts),P("V_o",volts),P("I",amps)},OUT(watts)},
 {"DC-DC input power","P_i = P_o/η","P_o: Output power (W); η: Efficiency (1); P_i: Input power (W)","Relate input/output average power and conversion efficiency at one operating point.",2,{P("P_o",watts),P("η",ratio)},OUT(watts)},
 {"DC-DC output power","P_o = ηP_i","P_i: Input power (W); η: Efficiency (1); P_o: Output power (W)","Relate input/output average power and conversion efficiency at one operating point.",2,{P("P_i",watts),P("η",ratio)},OUT(watts)},
 {"DC-DC efficiency","η = P_o/P_i","P_o: Output power (W); P_i: Input power (W); η: Efficiency (1)","Relate input/output average power and conversion efficiency at one operating point.",2,{P("P_o",watts),P("P_i",watts)},OUT(ratio)},
 {"Resistor power rating","P_r = P/k","P: Linear power (W); k: Allowed load fraction (1); P_r: Minimum rated power (W)","Find a minimum nominal resistor rating for a chosen allowed loading fraction.",2,{P("P",watts),P("k",ratio)},OUT(watts)},
 {"dBW to dBm","P_m = P_w + 30","P: Power level dBW (dBW); P: Power level (dBm)","Change absolute power reference between 1W (dBW) and 1mW (dBm). W/mW conversions use the existing power tools.",1,{P("P",dbw)},OUT(dbm)},
 {"dBm to dBW","P_w = P_m - 30","P: Power level (dBm); P: Power level dBW (dBW)","Change absolute power reference between 1W (dBW) and 1mW (dBm). W/mW conversions use the existing power tools.",1,{P("P",dbm)},OUT(dbw)},
 {"Power ratio to dB","G_d = 10 log10(G)","G: Power gain ratio (1); G: Power gain dB (dB)","Convert power gain or attenuation to/from a logarithmic ratio, or infer gain from measured powers.",1,{P("G",ratio)},OUT(decibel)},
 {"dB to power ratio","G = 10^(G_d/10)","G: Power gain dB (dB); G: Power gain ratio (1)","Convert power gain or attenuation to/from a logarithmic ratio, or infer gain from measured powers.",1,{P("G",decibel)},OUT(ratio)},
 {"Amplitude ratio to dB","G_d = 20 log10(A)","A: Amplitude ratio (1); G: Power gain dB (dB)","Convert amplitude/voltage transfer magnitude to/from dB. This is distinct from the power-ratio factor of ten.",1,{P("A",ratio)},OUT(decibel)},
 {"dB to amplitude ratio","A = 10^(G_d/20)","G: Power gain dB (dB); A: Amplitude ratio (1)","Convert amplitude/voltage transfer magnitude to/from dB. This is distinct from the power-ratio factor of ten.",1,{P("G",decibel)},OUT(ratio)},
 {"RF measured power ratio","G = P_o/P_i","P_o: Output power (W); P_i: Input power (W); G: Power gain ratio (1)","Convert power gain or attenuation to/from a logarithmic ratio, or infer gain from measured powers.",2,{P("P_o",watts),P("P_i",watts)},OUT(ratio)},
 {"RF power to RMS voltage","V = √(PR)","P: Linear power (W); R: Resistance (Ω); V: Voltage (V)","Recover RMS voltage from RF power in a real resistive termination; inverse of the existing V²/R power tool.",2,{P("P",watts),P("R",ohms)},OUT(volts)},
 {"Cascaded gain and loss","G = G₁+G₂+G₃","G₁: Stage 1 gain dB (dB); G₂: Stage 2 gain dB (dB); G₃: Stage 3 gain dB (dB); G: Power gain dB (dB)","Combine three RF stages. Gain entries are signed; insertion-loss entries are positive losses. Repeat the reduction for a longer chain.",3,{P("G₁",decibel),P("G₂",decibel),P("G₃",decibel)},OUT(decibel)},
 {"Cascaded insertion loss","L = L₁+L₂+L₃","L₁: First loss (dB); L₂: Second loss (dB); L₃: Third loss (dB); L: Insertion loss (dB)","Combine three RF stages. Gain entries are signed; insertion-loss entries are positive losses. Repeat the reduction for a longer chain.",3,{P("L₁",decibel),P("L₂",decibel),P("L₃",decibel)},OUT(decibel)},
 {"EIRP calculation","EIRP = P_t-L_t+G_t","P_t: TX power dBm (dBm); L_t: TX cable loss (dB); G_t: TX gain dBi (dBi); EIRP: Isotropic radiated power (dBm)","Compute effective isotropic radiated power from conducted TX power, feed loss and isotropic antenna gain.",3,{P("P_t",dbm),P("L_t",decibel),P("G_t",dbi)},OUT(dbm)},
 {"EIRP to ERP","ERP = EIRP-2.15","EIRP: Isotropic radiated power (dBm); ERP: Dipole radiated power (dBm)","Change isotropic/dipole reference using the documented 2.15dB relationship. ERP is dipole-referenced, EIRP isotropic-referenced.",1,{P("EIRP",dbm)},OUT(dbm)},
 {"ERP to EIRP","EIRP = ERP+2.15","ERP: Dipole radiated power (dBm); EIRP: Isotropic radiated power (dBm)","Change isotropic/dipole reference using the documented 2.15dB relationship. ERP is dipole-referenced, EIRP isotropic-referenced.",1,{P("ERP",dbm)},OUT(dbm)},
 {"dBd to dBi","G_i = G_d+2.15","G_d: Gain dBd (dBd); G_i: Gain dBi (dBi)","Change isotropic/dipole reference using the documented 2.15dB relationship. ERP is dipole-referenced, EIRP isotropic-referenced.",1,{P("G_d",dbd)},OUT(dbi)},
 {"dBi to dBd","G_d = G_i-2.15","G_i: Gain dBi (dBi); G_d: Gain dBd (dBd)","Change isotropic/dipole reference using the documented 2.15dB relationship. ERP is dipole-referenced, EIRP isotropic-referenced.",1,{P("G_i",dbi)},OUT(dbd)},
 {"Thermal receiver noise W","N = k_B B(T+T₀(10^(NF/10)-1))","T: Temperature K (K); B: Noise bandwidth (Hz); NF: Noise figure dB (dB); N: Noise power (W); k_B = 1.380649E-23 J/K; T₀ = 290 K","Find input-referred receiver noise from source temperature and equivalent receiver noise temperature T_e=(F-1)T₀.",3,{P("T",kelvin),P("B",hz),P("NF",decibel)},OUT(watts)},
 {"Receiver noise floor dBm","N_m = 10 log10(k_B B(T+T₀(10^(NF/10)-1))/0.001)","T: Temperature K (K); B: Noise bandwidth (Hz); NF: Noise figure dB (dB); N_m: Noise floor dBm (dBm); k_B = 1.380649E-23 J/K; T₀ = 290 K","Find input-referred receiver noise from source temperature and equivalent receiver noise temperature T_e=(F-1)T₀.",3,{P("T",kelvin),P("B",hz),P("NF",decibel)},OUT(dbm)},
 {"Measured SNR","SNR = 10 log10(S/N)","S: Signal power (W); N: Noise power (W); SNR: SNR in dB (dB)","Relate measured signal and integrated noise power in the same bandwidth/reference plane.",2,{P("S",watts),P("N",watts)},OUT(decibel)},
 {"Noise from signal and SNR","N = S/10^(SNR/10)","S: Signal power (W); SNR: SNR in dB (dB); N: Noise power (W)","Relate measured signal and integrated noise power in the same bandwidth/reference plane.",2,{P("S",watts),P("SNR",decibel)},OUT(watts)},
 {"Noise figure to factor","F = 10^(NF/10)","NF: Noise figure dB (dB); F: Linear noise factor (1)","Convert linear noise factor F and noise figure NF in dB; they are not the same numerical quantity.",1,{P("NF",decibel)},OUT(ratio)},
 {"Noise factor to figure","NF = 10 log10(F)","F: Linear noise factor (1); NF: Noise figure dB (dB)","Convert linear noise factor F and noise figure NF in dB; they are not the same numerical quantity.",1,{P("F",ratio)},OUT(decibel)},
 {"Friis cascade noise factor","F = F₁+(F₂-1)/G₁+(F₃-1)/(G₁G₂)","F₁: Stage 1 noise factor (1); F₂: Stage 2 noise factor (1); F₃: Stage 3 noise factor (1); G₁: Stage 1 power gain (1); G₂: Stage 2 power gain (1); F: Linear noise factor (1)","Combine three stage noise factors using F₁+(F₂-1)/G₁+(F₃-1)/(G₁G₂). Gains and factors are linear internally.",5,{P("F₁",ratio),P("F₂",ratio),P("F₃",ratio),P("G₁",ratio),P("G₂",ratio)},OUT(ratio)},
 {"Friis cascade noise figure","NF = 10 log10(F₁+(F₂-1)/G₁+(F₃-1)/(G₁G₂))","F₁: Stage 1 noise factor (1); F₂: Stage 2 noise factor (1); F₃: Stage 3 noise factor (1); G₁: Stage 1 power gain (1); G₂: Stage 2 power gain (1); NF: Noise figure dB (dB)","Combine three stage noise factors using F₁+(F₂-1)/G₁+(F₃-1)/(G₁G₂). Gains and factors are linear internally.",5,{P("F₁",ratio),P("F₂",ratio),P("F₃",ratio),P("G₁",ratio),P("G₂",ratio)},OUT(decibel)},
 {"Required receiver sensitivity","S_min = N_m+SNR","N_m: Noise floor dBm (dBm); SNR: SNR in dB (dB); S_min: Sensitivity dBm (dBm)","Estimate required input signal level as input-referred noise floor plus user-specified required SNR.",2,{P("N_m",dbm),P("SNR",decibel)},OUT(dbm)},
 {"Half-wave electrical length","l = c/(2f)","f: Frequency (Hz); l: Antenna length (m); c = 299792458 m/s","Find an ideal half-wave electrical length using vacuum c=299792458m/s. Existing quarter-wave and wavelength tools share this model.",1,{P("f",hz)},OUT(metres)},
 {"Lossless dielectric velocity","v = c/√ε_r","ε_r: Relative permittivity (1); v: Phase velocity (m/s); c = 299792458 m/s","Find phase velocity or wavelength in a homogeneous lossless, nonmagnetic dielectric.",1,{P("ε_r",ratio)},OUT(speed)},
 {"Dielectric wavelength","λ = c/(f√ε_r)","f: Frequency (Hz); ε_r: Relative permittivity (1); λ: Wavelength (m); c = 299792458 m/s","Find phase velocity or wavelength in a homogeneous lossless, nonmagnetic dielectric.",2,{P("f",hz),P("ε_r",ratio)},OUT(metres)},
 {"Full RF link budget","P_r = P_t-L_t+G_t+G_r-L_p-L_r","P_t: TX power dBm (dBm); L_t: TX cable loss (dB); G_t: TX gain dBi (dBi); G_r: RX gain dBi (dBi); L_p: Path loss (dB); L_r: RX cable loss (dB); P_r: Received power dBm (dBm)","Calculate received power from TX power, TX/RX feed losses, isotropic antenna gains and a supplied path loss.",6,{P("P_t",dbm),P("L_t",decibel),P("G_t",dbi),P("G_r",dbi),P("L_p",decibel),P("L_r",decibel)},OUT(dbm)},
 {"Receiver link margin","M = P_r-S_min","P_r: Received power dBm (dBm); S_min: Sensitivity dBm (dBm); M: Link margin (dB)","Compare predicted/measured received power with user-specified sensitivity. Positive margin is a technical surplus, not guaranteed delivery.",2,{P("P_r",dbm),P("S_min",dbm)},OUT(decibel)},
 {"Fresnel clearance percentage","q = h/r₁","h: Available clearance (m); r₁: Fresnel radius (m); q: Clearance fraction (1)","Express available obstacle clearance as a fraction/percentage of the full first Fresnel radius at the same point.",2,{P("h",metres),P("r₁",metres)},OUT(ratio)},
 {"Effective-Earth radio horizon","d = √(2kR_E h₁)+√(2kR_E h₂)","h₁: TX height (m); h₂: RX height (m); k: Effective radius factor (1); d: Path distance (m); R_E = 6371000 m","Approximate two-terminal radio horizon with a user-specified effective-Earth-radius multiplier k.",3,{P("h₁",metres),P("h₂",metres),P("k",ratio)},OUT(metres)},
 {"Linear polarization loss","L = -20 log10(cos θ)","θ: Relative polarization angle (rad); L: Polarization loss (dB)","Find ideal mismatch loss between two linearly polarized antennas at relative angle θ.",1,{P("θ",radians)},OUT(decibel)},
 {"Electrical phase length","φ = 2πl/λ","l: Antenna length (m); λ: Wavelength (m); φ: Electrical phase (rad)","Find unwrapped electrical length, or the corresponding negative propagation phase shift under exp(-jφ).",2,{P("l",metres),P("λ",metres)},OUT(radians)},
 {"Propagation phase shift","φ = -2πl/λ","l: Antenna length (m); λ: Wavelength (m); φ: Electrical phase (rad)","Find unwrapped electrical length, or the corresponding negative propagation phase shift under exp(-jφ).",2,{P("l",metres),P("λ",metres)},OUT(radians)},
 {"RC cutoff: low and high pass","f_c = 1/(2πRC)","R: Resistance (Ω); C: Capacitance (F); f_c: Cutoff frequency (Hz)","Find the -3dB corner for either an ideal RC low-pass or RC high-pass circuit; both share one cutoff equation.",2,{P("R",ohms),P("C",farad)},OUT(hz)},
 {"First-order low-pass magnitude","A = 1/√(1+(f/f_c)²)","f: Frequency (Hz); f_c: Cutoff frequency (Hz); A: Amplitude ratio (1)","Find ideal unity-passband first-order LP/HP voltage-transfer magnitude at f using its known cutoff.",2,{P("f",hz),P("f_c",hz)},OUT(ratio)},
 {"First-order high-pass magnitude","A = (f/f_c)/√(1+(f/f_c)²)","f: Frequency (Hz); f_c: Cutoff frequency (Hz); A: Amplitude ratio (1)","Find ideal unity-passband first-order LP/HP voltage-transfer magnitude at f using its known cutoff.",2,{P("f",hz),P("f_c",hz)},OUT(ratio)},
 {"Series resonator Q","Q = √(L/C)/R","R: Resistance (Ω); L: Inductance (H); C: Capacitance (F); Q: Series resonator Q (1)","Find Q or current-response half-power bandwidth of an ideal SERIES RLC resonator.",3,{P("R",ohms),P("L",henry),P("C",farad)},OUT(ratio)},
 {"Series resonator bandwidth","B = R/(2πL)","R: Resistance (Ω); L: Inductance (H); B: Resonator bandwidth (Hz)","Find Q or current-response half-power bandwidth of an ideal SERIES RLC resonator.",2,{P("R",ohms),P("L",henry)},OUT(hz)},
 {"Complex load reflection","ρ = |(R_L+jX_L-Z₀)/(R_L+jX_L+Z₀)|","R_L: Load resistance (Ω); X_L: Load reactance (Ω); Z₀: Real reference impedance (Ω); ρ: Reflection magnitude (1)","Convert passive complex load impedance and reflection coefficient relative to a positive real Z₀. Complex details accompany the primary scalar result.",3,{P("R_L",ohms),P("X_L",ohms),P("Z₀",ohms)},OUT(ratio)},
 {"VSWR from reflection","VSWR = (1+ρ)/(1-ρ)","ρ: Reflection magnitude (1); VSWR: Standing-wave ratio (1)","Convert passive reflection magnitude to voltage standing-wave ratio.",1,{P("ρ",ratio)},OUT(ratio)},
 {"Return loss","RL = -20 log10(ρ)","ρ: Reflection magnitude (1); RL: Return loss (dB)","Express reflection magnitude as positive return loss using -20log10(ρ).",1,{P("ρ",ratio)},OUT(decibel)},
 {"Reflection to impedance","Z = Z₀(1+Γ)/(1-Γ)","Γ_r: Reflection real part (1); Γ_i: Reflection imaginary part (1); Z₀: Real reference impedance (Ω); R_L: Load resistance (Ω)","Convert passive complex load impedance and reflection coefficient relative to a positive real Z₀. Complex details accompany the primary scalar result.",3,{P("Γ_r",signedratio),P("Γ_i",signedratio),P("Z₀",ohms)},OUT(ohms)},
 {"Low-pass resistive L-match","L_s = Q R_lo/(2πf)","R_s: Source resistance (Ω); R_L: Load resistance (Ω); f: Frequency (Hz); L_s: Inductance (H); Q = √(R_hi/R_lo - 1); R_hi = max(R_s,R_L); R_lo = min(R_s,R_L); C_p = Q/(2πfR_hi) [F]","Design an ideal LP or HP two-element L-match between unequal REAL positive resistances. Shunt goes at the HIGH-resistance port.",3,{P("R_s",ohms),P("R_L",ohms),P("f",hz)},OUT(henry)},
 {"High-pass resistive L-match","C_s = 1/(2πf Q R_lo)","R_s: Source resistance (Ω); R_L: Load resistance (Ω); f: Frequency (Hz); C_s: Capacitance (F); Q = √(R_hi/R_lo - 1); R_hi = max(R_s,R_L); R_lo = min(R_s,R_L); L_p = R_hi/(2πfQ) [H]","Design an ideal LP or HP two-element L-match between unequal REAL positive resistances. Shunt goes at the HIGH-resistance port.",3,{P("R_s",ohms),P("R_L",ohms),P("f",hz)},OUT(farad)},
 {"W to dBW","P_w = 10 log10(P/1W)","P: Linear power (W); P_w: Power level dBW (dBW)","Convert absolute RF power between watts/milliwatts and dBW, using 1W as the reference.",1,{P("P",watts)},OUT(dbw)},
 {"dBW to W","P = 1W × 10^(P_w/10)","P: Power level dBW (dBW); P_out: Linear power (W)","Convert absolute RF power between watts/milliwatts and dBW, using 1W as the reference.",1,{P("P",dbw)},OUT(watts)},
 {"Copper trace resistance","R = ρl/(wt)","ρ: Conductor resistivity [Ωm]; l: Conductor length [m]; w: Trace width [m]; t: Copper thickness [m]; R: Resistance [Ω]","Compute uniform conductor DC resistance from actual metal cross-section and resistivity.",4,{P("ρ",resistivity),P("l",geometry),P("w",geometry),P("t",geometry)},OUT(pcb_ohms)},
 {"IPC-2221 external estimate","I = k ΔT^0.44 (wt/mil²)^0.725","w: Trace width [m]; t: Copper thickness [m]; ΔT: Allowed temperature rise [K increment]; I: Current [A]","Estimate trace current using the legacy IPC-2221 empirical fit documented publicly by TI: I=k ΔT^0.44 A_mil²^0.725.",3,{P("w",geometry),P("t",geometry),P("ΔT",rise)},OUT(amps)},
 {"IPC-2221 internal estimate","I = k ΔT^0.44 (wt/mil²)^0.725","w: Trace width [m]; t: Copper thickness [m]; ΔT: Allowed temperature rise [K increment]; I: Current [A]","Estimate trace current using the legacy IPC-2221 empirical fit documented publicly by TI: I=k ΔT^0.44 A_mil²^0.725.",3,{P("w",geometry),P("t",geometry),P("ΔT",rise)},OUT(amps)},
 {"Copper weight to thickness","t = m_A/8960","m_A: Copper areal mass [kg/m²]; t: Copper thickness [m]","Convert copper mass per area to equivalent uniform thickness, or invert it.",1,{P("m_A",foil)},OUT(geometry)},
 {"Thickness to copper weight","m_A = 8960t","t: Copper thickness [m]; m_A: Copper areal mass [kg/m²]","Convert copper mass per area to equivalent uniform thickness, or invert it.",1,{P("t",geometry)},OUT(foil)},
 {"Plated via resistance","R = ρl/(πt(d+t))","ρ: Conductor resistivity [Ωm]; l: Conductor length [m]; d: Finished inner diameter [m]; t: Copper thickness [m]; R: Resistance [Ω]","Compute uniform conductor DC resistance from actual metal cross-section and resistivity.",4,{P("ρ",resistivity),P("l",geometry),P("d",geometry),P("t",geometry)},OUT(pcb_ohms)},
 {"Microstrip effective ε","Hammerstad-Jensen zero-thickness model","w: Trace width [m]; h: Substrate height [m]; ε_r: Relative permittivity [1]; ε_e: Effective permittivity [1]","Estimate quasi-static microstrip impedance and effective relative permittivity using the zero-thickness Hammerstad-Jensen model.",3,{P("w",geometry),P("h",geometry),P("ε_r",ratio)},OUT(ratio)},
 {"Microstrip impedance","Hammerstad-Jensen zero-thickness model","w: Trace width [m]; h: Substrate height [m]; ε_r: Relative permittivity [1]; Z₀: Real reference impedance [Ω]","Estimate quasi-static microstrip impedance and effective relative permittivity using the zero-thickness Hammerstad-Jensen model.",3,{P("w",geometry),P("h",geometry),P("ε_r",ratio)},OUT(ohms)},
 {"Guided propagation delay","τ = l√ε_e/c","l: Conductor length [m]; ε_e: Effective permittivity [1]; τ: Elapsed time [s]","Find propagation delay along a lossless line from its effective permittivity.",2,{P("l",geometry),P("ε_e",ratio)},OUT(seconds)},
 {"Reflected power fraction","p_r = ρ_g²","ρ_g: Reflection magnitude [1]; p_r: Reflected power fraction [1]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("ρ_g",ratio)},OUT(ratio)},
 {"Delivered power fraction","p_d = 1-ρ_g²","ρ_g: Reflection magnitude [1]; p_d: Delivered power fraction [1]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("ρ_g",ratio)},OUT(ratio)},
 {"Mismatch loss","ML = -10log10(1-ρ_g²)","ρ_g: Reflection magnitude [1]; ML: Mismatch loss [dB]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("ρ_g",ratio)},OUT(decibel)},
 {"Return loss to reflection","ρ_g = 10^(-RL/20)","RL: Return loss [dB]; ρ_g: Reflection magnitude [1]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("RL",decibel)},OUT(ratio)},
 {"S21 dB to insertion loss","IL = -S21_dB","S21: S21 signed dB [dB]; IL: Insertion loss [dB]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("S21",decibel)},OUT(decibel)},
 {"S21 linear insertion loss","IL = -20log10(|S21|)","S21: S21 wave magnitude [1]; IL: Insertion loss [dB]","Evaluate manually entered calibrated VNA readings using explicit wave/power conventions. This app does not communicate with NanoVNA SAA-2 or LiteVNA.",1,{P("S21",ratio)},OUT(decibel)},
 {"Complex impedance magnitude","|Z|=√(R²+X²)","R: Load resistance [Ω]; X: Load reactance [Ω]; |Z|: Complex impedance magnitude [Ω]","Convert a user-entered complex impedance R+jX to magnitude or phase.",2,{P("R",ohms),P("X",ohms)},OUT(ohms)},
 {"Complex impedance phase","φ=atan2(X,R)","R: Load resistance [Ω]; X: Load reactance [Ω]; φ: Complex impedance phase [rad]","Convert a user-entered complex impedance R+jX to magnitude or phase.",2,{P("R",ohms),P("X",ohms)},OUT(radians)},
 {"E12 nearest resistor","R_out = nearest_E(R)","R: Resistance [Ω]; R: Resistance [Ω]","Choose the nearest nominal resistor on the E12 or E24 preferred series using logarithmic distance.",1,{P("R",ohms)},OUT(ohms)},
 {"E24 nearest resistor","R_out = nearest_E(R)","R: Resistance [Ω]; R: Resistance [Ω]","Choose the nearest nominal resistor on the E12 or E24 preferred series using logarithmic distance.",1,{P("R",ohms)},OUT(ohms)},
 {"SMD 3 digit code","R = significant_digits × 10^last_digit","D: Numeric marking [1 (common-unit normalized)]; R: Resistance [Ω]","Decode conventional numeric three-/four-digit resistor markings: significant digits times ten to the final digit.",1,{P("D",scalar)},OUT(ohms)},
 {"SMD 4 digit code","R = significant_digits × 10^last_digit","D: Numeric marking [1 (common-unit normalized)]; R: Resistance [Ω]","Decode conventional numeric three-/four-digit resistor markings: significant digits times ten to the final digit.",1,{P("D",scalar)},OUT(ohms)},
 {"Resistor tolerance limits","y_min = y(1-t); y_max = y(1+t)","R: Resistor tolerance limits [Ω]; t: Tolerance fraction [1]; R: Resistor tolerance limits [Ω]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",2,{P("R",ohms),P("t",ratio)},OUT(ohms)},
 {"Capacitor tolerance limits","y_min = y(1-t); y_max = y(1+t)","C: Capacitor tolerance limits [F]; t: Tolerance fraction [1]; C: Capacitor tolerance limits [F]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",2,{P("C",farad),P("t",ratio)},OUT(farad)},
 {"Inductor tolerance limits","y_min = y(1-t); y_max = y(1+t)","L: Inductor tolerance limits [H]; t: Tolerance fraction [1]; L: Inductor tolerance limits [H]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",2,{P("L",henry),P("t",ratio)},OUT(henry)},
 {"Divider worst-case minimum","V = V_i R2(1±t2)/(R1(1∓t1)+R2(1±t2))","V_i: Input voltage [V]; R1: First resistance [Ω]; R2: Second resistance [Ω]; t1: First tolerance [1]; t2: Second tolerance [1]; V: Output voltage [V]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",5,{P("V_i",volts),P("R1",ohms),P("R2",ohms),P("t1",ratio),P("t2",ratio)},OUT(volts)},
 {"Divider worst-case maximum","V = V_i R2(1±t2)/(R1(1∓t1)+R2(1±t2))","V_i: Input voltage [V]; R1: First resistance [Ω]; R2: Second resistance [Ω]; t1: First tolerance [1]; t2: Second tolerance [1]; V: Output voltage [V]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",5,{P("V_i",volts),P("R1",ohms),P("R2",ohms),P("t1",ratio),P("t2",ratio)},OUT(volts)},
 {"RC worst-case minimum","τ = RC(1±t_R)(1±t_C)","R: Resistance [Ω]; C: Capacitance [F]; t_R: First tolerance [1]; t_C: Second tolerance [1]; τ: Time constant [s]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",4,{P("R",ohms),P("C",farad),P("t_R",ratio),P("t_C",ratio)},OUT(seconds)},
 {"RC worst-case maximum","τ = RC(1±t_R)(1±t_C)","R: Resistance [Ω]; C: Capacitance [F]; t_R: First tolerance [1]; t_C: Second tolerance [1]; τ: Time constant [s]","Calculate specified nominal component limits or exact worst-case corners of an ideal positive circuit.",4,{P("R",ohms),P("C",farad),P("t_R",ratio),P("t_C",ratio)},OUT(seconds)},
 {"Absolute scalar error","e = |x-r|","x: Measured scalar [1 (common-unit normalized)]; r: Reference scalar [1 (common-unit normalized)]; e: Absolute scalar error [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",2,{P("x",scalar),P("r",scalar)},OUT(scalar)},
 {"Relative scalar error","e_r = |x-r|/|r|","x: Measured scalar [1 (common-unit normalized)]; r: Reference scalar [1 (common-unit normalized)]; e_r: Relative scalar error [1]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",2,{P("x",scalar),P("r",scalar)},OUT(ratio)},
 {"Mean: 4 readings","m = Σx_i/4","x1: Scalar reading 1 [1 (common-unit normalized)]; x2: Scalar reading 2 [1 (common-unit normalized)]; x3: Scalar reading 3 [1 (common-unit normalized)]; x4: Scalar reading 4 [1 (common-unit normalized)]; m: Mean: 4 readings [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",4,{P("x1",scalar),P("x2",scalar),P("x3",scalar),P("x4",scalar)},OUT(scalar)},
 {"Population SD: 4 readings","σ = √(Σ(x_i-m)²/4)","x1: Scalar reading 1 [1 (common-unit normalized)]; x2: Scalar reading 2 [1 (common-unit normalized)]; x3: Scalar reading 3 [1 (common-unit normalized)]; x4: Scalar reading 4 [1 (common-unit normalized)]; σ: Population SD: 4 readings [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",4,{P("x1",scalar),P("x2",scalar),P("x3",scalar),P("x4",scalar)},OUT(scalar)},
 {"Sample SD: 4 readings","s = √(Σ(x_i-m)²/3)","x1: Scalar reading 1 [1 (common-unit normalized)]; x2: Scalar reading 2 [1 (common-unit normalized)]; x3: Scalar reading 3 [1 (common-unit normalized)]; x4: Scalar reading 4 [1 (common-unit normalized)]; s: Sample SD: 4 readings [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",4,{P("x1",scalar),P("x2",scalar),P("x3",scalar),P("x4",scalar)},OUT(scalar)},
 {"Mean standard uncertainty","u_m = s/2","x1: Scalar reading 1 [1 (common-unit normalized)]; x2: Scalar reading 2 [1 (common-unit normalized)]; x3: Scalar reading 3 [1 (common-unit normalized)]; x4: Scalar reading 4 [1 (common-unit normalized)]; u_m: Mean standard uncertainty [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",4,{P("x1",scalar),P("x2",scalar),P("x3",scalar),P("x4",scalar)},OUT(scalar)},
 {"Rectangular standard u","u = a/√3","a: Symmetric half-width [1 (common-unit normalized)]; u: Standard uncertainty [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",1,{P("a",scalar)},OUT(scalar)},
 {"Independent combined u","u_c = √(u1²+u2²+u3²)","u1: Standard contribution 1 [1 (common-unit normalized)]; u2: Standard contribution 2 [1 (common-unit normalized)]; u3: Standard contribution 3 [1 (common-unit normalized)]; u_c: Combined uncertainty [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",3,{P("u1",scalar),P("u2",scalar),P("u3",scalar)},OUT(scalar)},
 {"Expanded uncertainty","U = ku_c","u_c: Combined uncertainty [1 (common-unit normalized)]; k: Coverage factor [1 (common-unit normalized)]; U: Expanded uncertainty [1 (common-unit normalized)]","Evaluate dimensionless/common-unit scalar laboratory data; preserve the same normalization for every reading.",2,{P("u_c",scalar),P("k",scalar)},OUT(scalar)},
 {"Significant digits / SI","y = round_significant(x,n)","x: Measured scalar [1 (common-unit normalized)]; n: Significant digits [1 (common-unit normalized)]; y: Rounded scalar [1 (common-unit normalized)]","Round a scalar to a chosen number of significant digits and show scientific/engineering notation without changing the original input.",2,{P("x",scalar),P("n",scalar)},OUT(scalar)},
 {"LoRa symbol duration","Ts=2^SF/BW","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; T_s: Symbol duration [s]","Calculate nominal legacy SX127x LoRa symbol timing, symbol rate or coded physical bit rate.",2,{P("SF",scalar),P("BW",hz)},OUT(seconds)},
 {"LoRa symbol rate","Rs=BW/2^SF","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; R_s: Symbol rate [sym/s]","Calculate nominal legacy SX127x LoRa symbol timing, symbol rate or coded physical bit rate.",2,{P("SF",scalar),P("BW",hz)},OUT(symbolrate)},
 {"LoRa nominal bit rate","Rb=SF BW 4/(2^SF(4+CR))","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; CR: Coding index (1-4) [1]; R_b: Nominal coded bit rate [bit/s]","Calculate nominal legacy SX127x LoRa symbol timing, symbol rate or coded physical bit rate.",3,{P("SF",scalar),P("BW",hz),P("CR",scalar)},OUT(bitrate)},
 {"LoRa coding fraction","r=4/(4+CR)","CR: Coding index (1-4) [1]; r: Coding fraction [1]","Convert the legacy modem coding index to its information/code-bit fraction, or invert that discrete mapping.",1,{P("CR",scalar)},OUT(ratio)},
 {"LoRa coding index","CR=4/r-4","r: Coding fraction [1]; CR: Coding index (1-4) [1]","Convert the legacy modem coding index to its information/code-bit fraction, or invert that discrete mapping.",1,{P("r",ratio)},OUT(scalar)},
 {"LoRa packet airtime","Tair=(Npre+4.25+Npay)2^SF/BW","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; CR: Coding index (1-4) [1]; PL: Physical payload bytes [1]; IH: Header: 0 explicit, 1 implicit [1]; CRC: CRC: 0 off, 1 on [1]; N_pre: Programmed preamble symbols [sym]; DE: LDRO: 0 off, 1 on [1]; T_air: Packet airtime [s]; q: ceiling argument; N_pay: payload symbols; T_s: 2^SF/BW seconds","Calculate the complete standard SX127x LoRa packet, preamble or payload duration from explicitly entered modem settings.",8,{P("SF",scalar),P("BW",hz),P("CR",scalar),P("PL",scalar),P("IH",scalar),P("CRC",scalar),P("N_pre",symbols),P("DE",scalar)},OUT(seconds)},
 {"LoRa preamble duration","Tpre=(Npre+4.25)2^SF/BW","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; N_pre: Programmed preamble symbols [sym]; T_pre: Preamble duration [s]; q: ceiling argument; N_pay: payload symbols; T_s: 2^SF/BW seconds","Calculate the complete standard SX127x LoRa packet, preamble or payload duration from explicitly entered modem settings.",3,{P("SF",scalar),P("BW",hz),P("N_pre",symbols)},OUT(seconds)},
 {"LoRa payload duration","Tpay=Npay 2^SF/BW","SF: Spreading factor (7-12) [1]; BW: LoRa bandwidth [Hz]; CR: Coding index (1-4) [1]; PL: Physical payload bytes [1]; IH: Header: 0 explicit, 1 implicit [1]; CRC: CRC: 0 off, 1 on [1]; DE: LDRO: 0 off, 1 on [1]; T_pay: Payload duration [s]; q: ceiling argument; N_pay: payload symbols; T_s: 2^SF/BW seconds","Calculate the complete standard SX127x LoRa packet, preamble or payload duration from explicitly entered modem settings.",7,{P("SF",scalar),P("BW",hz),P("CR",scalar),P("PL",scalar),P("IH",scalar),P("CRC",scalar),P("DE",scalar)},OUT(seconds)},
 {"LoRa payload symbols","Npay=8+max(ceil(q)(CR+4),0)","SF: Spreading factor (7-12) [1]; CR: Coding index (1-4) [1]; PL: Physical payload bytes [1]; IH: Header: 0 explicit, 1 implicit [1]; CRC: CRC: 0 off, 1 on [1]; DE: LDRO: 0 off, 1 on [1]; N_pay: Payload symbol count [sym]; q: ceiling argument; N_pay: payload symbols; T_s: 2^SF/BW seconds","Calculate the complete standard SX127x LoRa packet, preamble or payload duration from explicitly entered modem settings.",6,{P("SF",scalar),P("CR",scalar),P("PL",scalar),P("IH",scalar),P("CRC",scalar),P("DE",scalar)},OUT(symbols)},
 {"Packet duty fraction","D=Tair/Tinterval","T_air: Measured/model TX airtime [s]; T_int: Packet start interval [s]; D: Occupied airtime fraction [1]","Estimate occupied-airtime fraction or floor-rounded theoretical packet count from a user-specified time budget.",2,{P("T_air",seconds),P("T_int",seconds)},OUT(ratio)},
 {"Packet budget count","N=floor(Twindow Dbudget/Tair)","T_air: Measured/model TX airtime [s]; T_w: Budget window duration [s]; D: Entered duty budget [1]; N: Theoretical packet count [pkt]","Estimate occupied-airtime fraction or floor-rounded theoretical packet count from a user-specified time budget.",3,{P("T_air",seconds),P("T_w",seconds),P("D",ratio)},OUT(packetcount)},
 {"Battery TX energy","E=Pdraw t","P_draw: Battery-side mean draw power [W]; T_air: Measured/model TX airtime [s]; E: Energy [J]","Estimate battery-side energy consumed during a specified transmission interval using measured mean draw power.",2,{P("P_draw",watts),P("T_air",seconds)},OUT(joules)},
};
static const VariableId input_ids[CALC_COUNT][CALC_MAX_INPUTS]={
 {VAR_I,VAR_R},{VAR_V,VAR_R},{VAR_V,VAR_I},{VAR_V,VAR_I},{VAR_POWER},{VAR_DBM},
 {VAR_F},{VAR_F},{VAR_L,VAR_C},{VAR_F,VAR_L},{VAR_F,VAR_L},{VAR_F,VAR_C},
 {VAR_DISTANCE,VAR_F},{VAR_F,VAR_D1,VAR_D2},{VAR_F,VAR_C},
 {VAR_I,VAR_R},
 {VAR_V,VAR_R},
 {VAR_R1,VAR_R2},
 {VAR_R1,VAR_R2},
 {VAR_VIN,VAR_R1,VAR_R2},
 {VAR_IS,VAR_R1,VAR_R2},
 {VAR_VS,VAR_V1,VAR_V2},
 {VAR_IS,VAR_I1,VAR_I2},
 {VAR_VS,VAR_VF,VAR_I},
 {VAR_C,VAR_V},
 {VAR_VS,VAR_R,VAR_C,VAR_TIME},
 {VAR_V0,VAR_R,VAR_C,VAR_TIME},
 {VAR_R,VAR_L,VAR_C,VAR_F},
 {VAR_R,VAR_L,VAR_C,VAR_F},
 {VAR_R,VAR_C},
 {VAR_L,VAR_R},
 {VAR_Q,VAR_V},
 {VAR_ENERGY,VAR_V},
 {VAR_Q,VAR_TIME},
 {VAR_Q,VAR_I,VAR_USABLE},
 {VAR_PON,VAR_POFF,VAR_DUTY},
 {VAR_VIN,VAR_VOUT,VAR_I},
 {VAR_POUT,VAR_ETA},
 {VAR_PIN,VAR_ETA},
 {VAR_POUT,VAR_PIN},
 {VAR_POWER,VAR_DERATE},
 {VAR_DBW},
 {VAR_DBM},
 {VAR_GAIN},
 {VAR_GAIN_DB},
 {VAR_AMP},
 {VAR_GAIN_DB},
 {VAR_POUT,VAR_PIN},
 {VAR_POWER,VAR_R},
 {VAR_GDB1,VAR_GDB2,VAR_GDB3},
 {VAR_LOSS1,VAR_LOSS2,VAR_LOSS3},
 {VAR_TX,VAR_TXL,VAR_TXG},
 {VAR_EIRP},
 {VAR_ERP},
 {VAR_DBD},
 {VAR_DBI},
 {VAR_TEMP,VAR_BAND,VAR_NF},
 {VAR_TEMP,VAR_BAND,VAR_NF},
 {VAR_SIGNAL,VAR_NOISE_W},
 {VAR_SIGNAL,VAR_SNR},
 {VAR_NF},
 {VAR_FACTOR},
 {VAR_F1,VAR_F2,VAR_F3,VAR_G1,VAR_G2},
 {VAR_F1,VAR_F2,VAR_F3,VAR_G1,VAR_G2},
 {VAR_NOISE_DBM,VAR_SNR},
 {VAR_F},
 {VAR_ER},
 {VAR_F,VAR_ER},
 {VAR_TX,VAR_TXL,VAR_TXG,VAR_RXG,VAR_PATHL,VAR_RXL},
 {VAR_RX,VAR_SENS},
 {VAR_CLEAR,VAR_RADIUS},
 {VAR_H1,VAR_H2,VAR_EARTHK},
 {VAR_ANGLE},
 {VAR_LENGTH,VAR_WAVELENGTH},
 {VAR_LENGTH,VAR_WAVELENGTH},
 {VAR_R,VAR_C},
 {VAR_F,VAR_FC},
 {VAR_F,VAR_FC},
 {VAR_R,VAR_L,VAR_C},
 {VAR_R,VAR_L},
 {VAR_RLOAD,VAR_XLOAD,VAR_Z0},
 {VAR_GMAG},
 {VAR_GMAG},
 {VAR_GR,VAR_GI,VAR_Z0},
 {VAR_RS,VAR_RLOAD,VAR_F},
 {VAR_RS,VAR_RLOAD,VAR_F},
 {VAR_POWER},{VAR_DBW},
 {VAR_RESISTIVITY,VAR_TRLEN,VAR_WIDTH,VAR_THICK},
 {VAR_WIDTH,VAR_THICK,VAR_RISE},
 {VAR_WIDTH,VAR_THICK,VAR_RISE},
 {VAR_FOIL},
 {VAR_THICK},
 {VAR_RESISTIVITY,VAR_TRLEN,VAR_DIAM,VAR_THICK},
 {VAR_WIDTH,VAR_HEIGHT,VAR_ER},
 {VAR_WIDTH,VAR_HEIGHT,VAR_ER},
 {VAR_TRLEN,VAR_EEFF},
 {VAR_GMAG},
 {VAR_GMAG},
 {VAR_GMAG},
 {VAR_RLOSS},
 {VAR_S21DB},
 {VAR_S21LIN},
 {VAR_RLOAD,VAR_XLOAD},
 {VAR_RLOAD,VAR_XLOAD},
 {VAR_R},
 {VAR_R},
 {VAR_CODE},
 {VAR_CODE},
 {VAR_R,VAR_TOL},
 {VAR_C,VAR_TOL},
 {VAR_L,VAR_TOL},
 {VAR_VIN,VAR_R1,VAR_R2,VAR_TOL1,VAR_TOL2},
 {VAR_VIN,VAR_R1,VAR_R2,VAR_TOL1,VAR_TOL2},
 {VAR_R,VAR_C,VAR_TOL1,VAR_TOL2},
 {VAR_R,VAR_C,VAR_TOL1,VAR_TOL2},
 {VAR_MEAS,VAR_REF},
 {VAR_MEAS,VAR_REF},
 {VAR_DATA1,VAR_DATA2,VAR_DATA3,VAR_DATA4},
 {VAR_DATA1,VAR_DATA2,VAR_DATA3,VAR_DATA4},
 {VAR_DATA1,VAR_DATA2,VAR_DATA3,VAR_DATA4},
 {VAR_DATA1,VAR_DATA2,VAR_DATA3,VAR_DATA4},
 {VAR_BOUND},
 {VAR_UNC1,VAR_UNC2,VAR_UNC3},
 {VAR_UNC,VAR_COVERAGE},
 {VAR_MEAS,VAR_DIGITS},
 {VAR_LORA_SF,VAR_LORA_BW},
 {VAR_LORA_SF,VAR_LORA_BW},
 {VAR_LORA_SF,VAR_LORA_BW,VAR_LORA_CR},
 {VAR_LORA_CR},
 {VAR_LORA_RATE},
 {VAR_LORA_SF,VAR_LORA_BW,VAR_LORA_CR,VAR_LORA_PL,VAR_LORA_IH,VAR_LORA_CRC,VAR_LORA_NPRE,VAR_LORA_DE},
 {VAR_LORA_SF,VAR_LORA_BW,VAR_LORA_NPRE},
 {VAR_LORA_SF,VAR_LORA_BW,VAR_LORA_CR,VAR_LORA_PL,VAR_LORA_IH,VAR_LORA_CRC,VAR_LORA_DE},
 {VAR_LORA_SF,VAR_LORA_CR,VAR_LORA_PL,VAR_LORA_IH,VAR_LORA_CRC,VAR_LORA_DE},
 {VAR_LORA_TAIR,VAR_LORA_INTERVAL},
 {VAR_LORA_TAIR,VAR_LORA_WINDOW,VAR_LORA_BUDGET},
 {VAR_BAT_DRAW,VAR_LORA_TAIR},
};
static const VariableId output_ids[CALC_COUNT]={VAR_V,VAR_I,VAR_R,VAR_POWER,VAR_DBM,VAR_POWER,VAR_WAVELENGTH,VAR_LENGTH,VAR_F,VAR_C,VAR_XL,VAR_XC,VAR_LOSS,VAR_RADIUS,VAR_L,VAR_POWER,VAR_POWER,VAR_R,VAR_R,VAR_VOUT,VAR_I1,VAR_V3,VAR_I3,VAR_R,VAR_ENERGY,VAR_VC,VAR_VC,VAR_ZMAG,VAR_PHASE,VAR_TAU,VAR_TAU,VAR_ENERGY,VAR_Q,VAR_I,VAR_TIME,VAR_POWER,VAR_POWER,VAR_PIN,VAR_POUT,VAR_ETA,VAR_PRATED,VAR_DBM,VAR_DBW,VAR_GAIN_DB,VAR_GAIN,VAR_GAIN_DB,VAR_AMP,VAR_GAIN,VAR_V,VAR_GAIN_DB,VAR_LOSS_TOTAL,VAR_EIRP,VAR_ERP,VAR_EIRP,VAR_DBI,VAR_DBD,VAR_NOISE_W,VAR_NOISE_DBM,VAR_SNR,VAR_NOISE_W,VAR_FACTOR,VAR_NF,VAR_FACTOR,VAR_NF,VAR_SENS,VAR_LENGTH,VAR_SPEED,VAR_WAVELENGTH,VAR_RX,VAR_MARGIN,VAR_CLEAR_PCT,VAR_DISTANCE,VAR_POLLOSS,VAR_ELPH,VAR_ELPH,VAR_FC,VAR_AMP,VAR_AMP,VAR_RESONATOR_Q,VAR_RBAND,VAR_GMAG,VAR_VSWR,VAR_RLOSS,VAR_RLOAD,VAR_L,VAR_C,VAR_DBW,VAR_POWER,VAR_R,VAR_I,VAR_I,VAR_THICK,VAR_FOIL,VAR_R,VAR_EEFF,VAR_Z0,VAR_TIME,VAR_PREFL,VAR_PDEL,VAR_MLOSS,VAR_GMAG,VAR_ILOSS,VAR_ILOSS,VAR_ZMAG,VAR_PHASE,VAR_R,VAR_R,VAR_R,VAR_R,VAR_R,VAR_C,VAR_L,VAR_VOUT,VAR_VOUT,VAR_TAU,VAR_TAU,VAR_ERR,VAR_RELERR,VAR_AVG4,VAR_POP4,VAR_SAMPLE4,VAR_MEANU4,VAR_UNC,VAR_UNC,VAR_EXP_U,VAR_ROUNDED,VAR_LORA_TS,VAR_LORA_RS,VAR_LORA_RB,VAR_LORA_RATE,VAR_LORA_CR,VAR_LORA_TAIR,VAR_LORA_TPRE,VAR_LORA_TPAY,VAR_LORA_NPAY,VAR_LORA_DUTY,VAR_LORA_COUNT,VAR_ENERGY};
VariableId calc_input_id(CalcId id,unsigned i){return (unsigned)id<CALC_COUNT&&i<calculations[id].count?input_ids[id][i]:VAR_NONE;}
VariableId calc_output_id(CalcId id){return (unsigned)id<CALC_COUNT?output_ids[id]:VAR_NONE;}
const char *variable_meaning(VariableId id){
 static const char *names[VAR_COUNT]={"Voltage","Current","Resistance","Linear power","Power level","Frequency","Inductance","Capacitance","Path distance","First path segment","Second path segment","Inductive reactance","Capacitive reactance","Wavelength","Antenna length","Path loss","Fresnel radius","First resistance","Second resistance","Input voltage","Output voltage","Source voltage","LED forward voltage","Initial voltage","Capacitor voltage","First voltage drop","Second voltage drop","Remaining voltage","Total current","First branch current","Second branch current","Remaining current","Energy","Elapsed time","Time constant","Charge capacity","Usable fraction","Duty fraction","Active power","Inactive power","Input power","Output power","Efficiency","Allowed load fraction","Minimum rated power","Impedance magnitude","Impedance phase","Power level dBW","Power gain ratio","Power gain dB","Amplitude ratio","Stage 1 gain dB","Stage 2 gain dB","Stage 3 gain dB","First loss","Second loss","Third loss","Insertion loss","TX power dBm","TX cable loss","TX gain dBi","Isotropic radiated power","Dipole radiated power","Gain dBd","Gain dBi","Temperature K","Noise bandwidth","Noise figure dB","Noise power","Noise floor dBm","Signal power","SNR in dB","Linear noise factor","Stage 1 noise factor","Stage 2 noise factor","Stage 3 noise factor","Stage 1 power gain","Stage 2 power gain","Sensitivity dBm","Relative permittivity","Phase velocity","RX gain dBi","Path loss","RX cable loss","Received power dBm","Link margin","Available clearance","Clearance fraction","TX height","RX height","Effective radius factor","Relative polarization angle","Polarization loss","Electrical phase","Cutoff frequency","Series resonator Q","Resonator bandwidth","Load resistance","Load reactance","Real reference impedance","Reflection magnitude","Standing-wave ratio","Return loss","Reflection real part","Reflection imaginary part","Source resistance","Conductor resistivity","Conductor length","Trace width","Copper thickness","Allowed temperature rise","Copper areal mass","Finished inner diameter","Substrate height","Effective permittivity","Reflected power fraction","Delivered power fraction","Mismatch loss","S21 signed dB","Insertion loss","S21 wave magnitude","Numeric marking","Tolerance fraction","First tolerance","Second tolerance","Measured scalar","Reference scalar","Absolute scalar error","Relative scalar error","Scalar reading 1","Scalar reading 2","Scalar reading 3","Scalar reading 4","Mean: 4 readings","Population SD: 4 readings","Sample SD: 4 readings","Mean standard uncertainty","Symmetric half-width","Standard uncertainty","Standard contribution 1","Standard contribution 2","Standard contribution 3","Coverage factor","Expanded uncertainty","Significant digits","Rounded scalar","Spreading factor (7-12)","LoRa bandwidth","Symbol duration","Symbol rate","Coding index (1-4)","Nominal coded bit rate","Coding fraction","Physical payload bytes","Header: 0 explicit, 1 implicit","CRC: 0 off, 1 on","Programmed preamble symbols","LDRO: 0 off, 1 on","Packet airtime","Preamble duration","Payload duration","Payload symbol count","Packet start interval","Occupied airtime fraction","Budget window duration","Entered duty budget","Theoretical packet count","Battery-side mean draw power"};
 return (unsigned)id<VAR_COUNT?names[id]:"Constant";
}
const char *variable_symbol(VariableId id){static const char *symbols[VAR_COUNT]={"V","I","R","P","dBm","f","L","C","d","d₁","d₂","X_L","X_C","λ","l","L_fs","r₁","R₁","R₂","V_i","V_o","V_s","V_f","V₀","V_c","V₁","V₂","V₃","I_s","I₁","I₂","I₃","E","t","τ","Q","k","D","P_on","P_off","P_i","P_o","η","k","P_r","|Z|","φ","P","G","G","A","G₁","G₂","G₃","L₁","L₂","L₃","L","P_t","L_t","G_t","EIRP","ERP","G_d","G_i","T","B","NF","N","N_m","S","SNR","F","F₁","F₂","F₃","G₁","G₂","S_min","ε_r","v","G_r","L_p","L_r","P_r","M","h","q","h₁","h₂","k","θ","L","φ","f_c","Q","B","R_L","X_L","Z₀","ρ","VSWR","RL","Γ_r","Γ_i","R_s","ρ","l","w","t","ΔT","m_A","d","h","ε_e","p_r","p_d","ML","S21","IL","S21","D","t","t1","t2","x","r","e","e_r","x1","x2","x3","x4","m","σ","s","u_m","a","u","u1","u2","u3","k","U","n","y","SF","BW","T_s","R_s","CR","R_b","r","PL","IH","CRC","N_pre","DE","T_air","T_pre","T_pay","N_pay","T_int","D","T_w","D","N","P_draw"};return (unsigned)id<VAR_COUNT?symbols[id]:"";}
int parameter_valid(CalcId id,unsigned i,double v){
 if((unsigned)id>=CALC_COUNT||i>=calculations[id].count||!isfinite(v))return 0;
 if(id>=LORA_FIRST_ID)return lora_parameter_valid(id,i,v);
 if(id>=PCB_FIRST_ID)return lab_parameter_valid(id,i,v);
 if(id>=RF_FIRST_ID)return rf_parameter_valid(id,i,v);
 switch(id){
 case POWER_IR:return i==0||v>0;
 case POWER_VR:return i==0||v>0;
 case R_SERIES:return v>=0;
 case V_DIVIDER:case I_DIVIDER:return i==0||v>0;
 case KVL_REMAINDER:case KCL_REMAINDER:return 1;
 case LED_RESISTOR:return i==2?v>0:v>=0;
 case CAP_ENERGY:return i==1||v>0;
 case RC_CHARGE:case RC_DISCHARGE:return i==0||(i==3?v>=0:v>0);
 case RLC_MAG:case RLC_PHASE:return i==0?v>=0:v>0;
 case AVG_CURRENT:return i==0?v>=0:v>0;
 case BAT_RUNTIME:return v>0&&(i!=2||v<=1);
 case DUTY_POWER:return v>=0&&(i!=2||v<=1);
 case LDO_LOSS:return v>=0;
 case DCDC_IN:case DCDC_OUT:return i==0?v>=0:(v>0&&v<=1);
 case DCDC_EFF:return i==0?v>=0:v>0;
 case RES_RATING:return i==0?v>=0:(v>0&&v<=1);
 case VOLTAGE:return i==0||v>=0;
 case CURRENT:return i==0||v>0;
 case RESISTANCE:return i==0||v!=0;
 case POWER:case DBM_MW:return 1;
 default:return v>0;
 }
}
static Status expanded_calculate(CalcId id,const double *x,double *out);
Status calculate(CalcId id,const double *x,double *out) {
 double y=0; unsigned i;
 if(!x||!out||(unsigned)id>=CALC_COUNT)return INVALID;
 for(i=0;i<calculations[id].count;i++) {
  if(!parameter_valid(id,i,x[i]))return INVALID;
 }
 if(id>=LORA_FIRST_ID)return lora_calculate(id,x,out);
 if(id>=PCB_FIRST_ID)return lab_calculate(id,x,out);
 if(id>=RF_FIRST_ID)return rf_calculate(id,x,out);
 if(id>=LEGACY_CALC_COUNT)return expanded_calculate(id,x,out);
 /* Signed DC quantities; passive R is nonnegative, and divisors are nonzero. */
 switch(id) {
 case VOLTAGE:y=x[0]*x[1];break;
 case CURRENT:case RESISTANCE:y=x[0]/x[1];break;
 case POWER:y=x[0]*x[1];break;
 case MW_DBM:y=10*(log10(x[0])+3);break;
 case DBM_MW:y=pow(10,x[0]/10-3);break;
 case WAVELENGTH:y=C0/x[0];break;
 case QUARTER_WAVE:y=(C0/4)/x[0];break;
 case LC_FREQ:y=exp(-log(2*PI)-.5*(log(x[0])+log(x[1])));break;
 case LC_CAP:case LC_INDUCTANCE:y=exp(-2*(log(2*PI)+log(x[0]))-log(x[1]));break;
 case XL:y=exp(log(2*PI)+log(x[0])+log(x[1]));break;
 case XC:y=exp(-log(2*PI)-log(x[0])-log(x[1]));break;
 case FSPL:y=20*(log10(4*PI/C0)+log10(x[0])+log10(x[1]));break;
 case FRESNEL: {
  double lo=fmin(x[1],x[2]),hi=fmax(x[1],x[2]);
  y=exp(.5*(log(C0)-log(x[0])+log(lo)-log(1+lo/hi)));break;
 }
 default:return INVALID;
 }
 if(id==RESISTANCE&&y<0)return INVALID;
 if(!isfinite(y))return RANGE;
 if(y==0&&id!=MW_DBM&&id!=FSPL){
  int exact_zero=(id==VOLTAGE||id==POWER)?(x[0]==0||x[1]==0):(id==CURRENT||id==RESISTANCE)?x[0]==0:0;
  if(!exact_zero)return RANGE;
 }
 *out=y;return OK;
}
Status parse_number(const char *s,double scale,double *si) {
 char *end; double v;
 if(!s||!si||!isfinite(scale)||scale<=0)return INVALID;
 errno=0; v=strtod(s,&end);
 if(end==s||errno==ERANGE||!isfinite(v))return INVALID;
 while(isspace((unsigned char)*end))end++;
 if(*end)return INVALID;
 *si=v*scale;
 if(!isfinite(*si)||(v!=0&&*si==0))return RANGE;
 return OK;
}
Status convert_output(double si,double scale,double *v) {
 if(!v||!isfinite(si)||!isfinite(scale)||scale<=0)return INVALID;
 *v=si/scale;
 return (!isfinite(*v)||(si!=0&&*v==0))?RANGE:OK;
}
const char *status_text(Status s) {
 return s==OK?"OK":s==INVALID?"Invalid input or passive-domain violation":"Numeric overflow/underflow";
}

/* Phase 3E-1: SI-only extensions; the original calculation cases stay above. */
/* Stable bounded kernels also supported by the existing Windows CRT.
   Stage floating-point helper calls: the installed TinyCC miscompiles some
   nested multi-double calls; host and ARM/GCC use the same staged expressions. */
static double log_one_plus(double x){double term=x,sum=x;unsigned k;if(x>=.001)return log(1+x);for(k=2;k<=8;k++){term*=-x;sum+=term/k;}return sum;}
static double one_minus_exp_neg(double x){double term=x,sum=x;unsigned k;if(x>=.01)return 1-exp(-x);for(k=2;k<=12;k++){term*=-x/k;sum+=term;}return sum;}
static double signed_exp(double logarithm,double sign){double magnitude=exp(logarithm);return sign<0?-magnitude:magnitude;}
static double log_sum_positive(double a,double b){double hi=fmax(a,b),lo=fmin(a,b),correction;correction=log_one_plus(lo/hi);return log(hi)+correction;}
static double balance3(double a,double b,double c){
 double m=fabs(a),s=0,correction=0,v[3],t;unsigned i;
 if(fabs(b)>m){m=fabs(b);}
 if(fabs(c)>m){m=fabs(c);}
 if(m==0)return 0;
 v[0]=a/m;v[1]=-b/m;v[2]=-c/m;
 for(i=0;i<3;i++){t=s+v[i];correction+=fabs(s)>=fabs(v[i])?(s-t)+v[i]:(v[i]-t)+s;s=t;}
 return (s+correction)*m;
}
Status series_rlc(const double *si,double *reactance,double *magnitude,double *phase){
 double a[2],b[2],xl,xc,x,z;unsigned i;
 if(!si||si[0]<0||!isfinite(si[0])||!reactance||!magnitude||!phase)return INVALID;
 for(i=1;i<4;i++)if(!isfinite(si[i])||si[i]<=0)return INVALID;
 a[0]=b[0]=si[3];a[1]=si[1];b[1]=si[2];
 if(calculate(XL,a,&xl)!=OK||calculate(XC,b,&xc)!=OK)return RANGE;
 x=xl-xc;z=hypot(si[0],x);if(!isfinite(x)||!isfinite(z))return RANGE;
 *reactance=x;*magnitude=z;*phase=z==0?NAN:atan2(x,si[0]);return OK;
}
static Status expanded_calculate(CalcId id,const double *x,double *out){
 double y=0,a=0,b=0,z=0,phi=0;int exact_zero=0;Status status;
 switch(id){
 case POWER_IR:y=x[0]==0?0:exp(2*log(fabs(x[0]))+log(x[1]));exact_zero=x[0]==0;break;
 case POWER_VR:y=x[0]==0?0:exp(2*log(fabs(x[0]))-log(x[1]));exact_zero=x[0]==0;break;
 case R_SERIES:y=x[0]+x[1];exact_zero=x[0]==0&&x[1]==0;break;
 case R_PARALLEL:a=fmin(x[0],x[1]);b=fmax(x[0],x[1]);y=a/(1+a/b);break;
 case V_DIVIDER:case I_DIVIDER:if(x[0]==0)y=0;else{a=log_sum_positive(x[1],x[2]);b=log(fabs(x[0]))+log(x[2])-a;y=signed_exp(b,x[0]);}exact_zero=x[0]==0;break;
 case KVL_REMAINDER:case KCL_REMAINDER:y=balance3(x[0],x[1],x[2]);exact_zero=1;break;
 case LED_RESISTOR:if(x[0]<=x[1])return INVALID;y=exp(log(x[0]-x[1])-log(x[2]));break;
 case CAP_ENERGY:y=x[1]==0?0:exp(log(x[0])+2*log(fabs(x[1]))-log(2));exact_zero=x[1]==0;break;
 case RC_CHARGE:case RC_DISCHARGE:
  exact_zero=x[0]==0||(id==RC_CHARGE&&x[3]==0);
  if(x[0]==0)y=0;
  else if(x[3]==0)y=id==RC_CHARGE?0:x[0];
  else{a=log(x[3])-log(x[1])-log(x[2]);
   if(id==RC_CHARGE){if(a<-40){b=log(fabs(x[0]))+a;y=signed_exp(b,x[0]);}else if(a>700)y=x[0];else{b=one_minus_exp_neg(exp(a));y=x[0]*b;}}
   else{if(a>700)return RANGE;b=log(fabs(x[0]))-exp(a);y=signed_exp(b,x[0]);}
  }break;
 case RLC_MAG:case RLC_PHASE:
  status=series_rlc(x,&a,&z,&phi);if(status!=OK)return status;
  if(id==RLC_PHASE&&!isfinite(phi))return INVALID;
  y=id==RLC_MAG?z:phi;exact_zero=1;break;
 case RC_TAU:y=exp(log(x[0])+log(x[1]));break;
 case RL_TAU:y=exp(log(x[0])-log(x[1]));break;
 case AH_WH:y=exp(log(x[0])+log(x[1]));break;
 case WH_AH:y=exp(log(x[0])-log(x[1]));break;
 case AVG_CURRENT:y=x[0]==0?0:exp(log(x[0])-log(x[1]));exact_zero=x[0]==0;break;
 case BAT_RUNTIME:y=exp(log(x[0])+log(x[2])-log(x[1]));break;
 case DUTY_POWER:y=x[2]*x[0]+(1-x[2])*x[1];exact_zero=(x[0]==0||x[2]==0)&&(x[1]==0||x[2]==1);break;
 case LDO_LOSS:if(x[1]>x[0])return INVALID;exact_zero=x[0]==x[1]||x[2]==0;y=exact_zero?0:exp(log(x[0]-x[1])+log(x[2]));break;
 case DCDC_IN:y=x[0]==0?0:exp(log(x[0])-log(x[1]));exact_zero=x[0]==0;break;
 case DCDC_OUT:y=x[0]==0?0:exp(log(x[0])+log(x[1]));exact_zero=x[0]==0;break;
 case DCDC_EFF:if(x[0]>x[1])return INVALID;y=x[0]/x[1];exact_zero=x[0]==0;break;
 case RES_RATING:y=x[0]==0?0:exp(log(x[0])-log(x[1]));exact_zero=x[0]==0;break;
 default:return INVALID;
 }
 if(!isfinite(y)||(!exact_zero&&y==0))return RANGE;
 *out=y;return OK;
}
