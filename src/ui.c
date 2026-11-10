#include "ui.h"
#include "rf_engine.h"
#include "lab_engine.h"
#include "lora_engine.h"
#include "version.h"
#include <math.h>
#include "display.h"
#include "handbook.h"
#include "math_layout.h"
#include "locale.h"
#include "settings.h"
#include <stdio.h>
#include <string.h>
#include "c21_logo.h"
typedef struct { const char *name; int child,calc; } Entry;
typedef struct { const Entry *entries; unsigned count; } Menu;
#define SUB(s,n) {s,n,-1}
#define CAL(s,n) {s,-1,n}
#define SYM {"Engineering symbols",-2,-1}
static const Entry root[]={SUB("RF CALCULATOR",1),SUB("BASIC ELECTRONICS",2),SUB("RLC & IMPEDANCE",3),SUB("POWER & BATTERY",4),SUB("FILTER DESIGN",5),SUB("PCB ENGINEERING",6),SUB("ANTENNA & PROPAGATION",7),SUB("MEASUREMENT LAB",8),SUB("ENGINEERING TOOLS",9),SUB("ENGINEERING HANDBOOK",17)};
static const Entry rf[]={SUB("RF power conversions",10),SUB("Propagation",11),SUB("RF levels",36),SUB("Gain and EIRP",37),SUB("Receiver noise",38),SUB("LoRa PHY and coding",67),SUB("LoRa packet timing",68),SUB("LoRa budget and energy",69)};
static const Entry basic[]={SUB("DC circuits",12),SUB("Resistor networks",22),SUB("Kirchhoff & LED",23),SUB("Capacitors & RC",24)};
static const Entry rlc[]={SUB("Resonance",13),SUB("Reactance",14),SUB("Series RLC",25),SUB("Time constants",26)};
static const Entry power[]={SUB("DC power",15),SUB("Power conversions",10),SUB("Battery energy",27),SUB("Battery runtime",28),SUB("Regulators & converters",29)};
static const Entry filter[]={SUB("Ideal LC building blocks",13),SUB("RC and resonators",41),SUB("Matching networks",42)};
static const Entry pcb[]={SUB("DC trace load basics",12),SUB("Copper geometry",51),SUB("Microstrip and delay",52),SUB("Trace dissipation",15)};
static const Entry antenna[]={SUB("Antenna length",16),SUB("Propagation",11),SUB("Dielectric and length",39),SUB("Link analysis",40)};
static const Entry lab[]={SUB("Reactance checks",14),SUB("Power conversions",10),SUB("VNA reflection",53),SUB("VNA transmission",54),SUB("Statistics / uncertainty",57)};
static const Entry engineering[]={SYM,SUB("DC reference calculations",12),SUB("Preferred values / codes",55),SUB("Tolerance and corners",56),SUB("Statistics / uncertainty",57)};
static const Entry conversion[]={CAL("mW to dBm",MW_DBM),CAL("dBm to mW",DBM_MW)};
static const Entry propagation[]={CAL("Free-space path loss",FSPL),CAL("First Fresnel radius",FRESNEL)};
static const Entry dc[]={CAL("Ohm: voltage",VOLTAGE),CAL("Ohm: current",CURRENT),CAL("Ohm: resistance",RESISTANCE),CAL("Electrical power",POWER)};
static const Entry resonance[]={CAL("LC resonance frequency",LC_FREQ),CAL("Required capacitance",LC_CAP),CAL("Required inductance",LC_INDUCTANCE)};
static const Entry reactance[]={CAL("Inductive reactance",XL),CAL("Capacitive reactance",XC)};
static const Entry dc_power[]={CAL("Electrical power",POWER),CAL("Resistor power: I²R",POWER_IR),CAL("Resistor power: V²/R",POWER_VR),CAL("Resistor power rating",RES_RATING)};
static const Entry lengths[]={CAL("Frequency to wavelength",WAVELENGTH),CAL("Quarter-wave length",QUARTER_WAVE)};
#define BOOK(s,n) {s,-4,n}
static const Entry handbook_index[]={SUB("DC circuits & power",18),SUB("RF / VNA / LoRa reference",19),SUB("Waves & propagation",20),SUB("RLC & reactance",21),SUB("Resistor networks",30),SUB("Kirchhoff & LED",31),SUB("Capacitors & RC",32),SUB("Battery energy",33),SUB("Battery runtime",34),SUB("Regulators & converters",35)};
static const Entry book_dc[]={BOOK("Ohm: voltage",VOLTAGE),BOOK("Ohm: current",CURRENT),BOOK("Ohm: resistance",RESISTANCE),BOOK("Electrical power",POWER),BOOK("Resistor power: I²R",POWER_IR),BOOK("Resistor power: V²/R",POWER_VR),BOOK("Resistor power rating",RES_RATING),SUB("Statistics / uncertainty",64)};
static const Entry book_rf[]={BOOK("mW to dBm",MW_DBM),BOOK("dBm to mW",DBM_MW),SUB("RF levels",43),SUB("Gain and EIRP",44),SUB("Receiver noise",45),SUB("VNA measurement tools",66),SUB("LoRa PHY and coding",70),SUB("LoRa packet timing",71),SUB("LoRa budget and energy",72)};
static const Entry book_waves[]={BOOK("Wavelength",WAVELENGTH),BOOK("Quarter-wave antenna",QUARTER_WAVE),BOOK("Free-space path loss",FSPL),BOOK("First Fresnel radius",FRESNEL),SUB("Dielectric and length",46),SUB("Link analysis",47)};
static const Entry book_rlc[]={BOOK("LC resonance",LC_FREQ),BOOK("Required capacitance",LC_CAP),BOOK("Inductive reactance",XL),BOOK("Capacitive reactance",XC),BOOK("Required inductance",LC_INDUCTANCE),BOOK("Series RLC impedance",RLC_MAG),BOOK("Series RLC phase",RLC_PHASE),BOOK("RC time constant",RC_TAU),BOOK("RL time constant",RL_TAU),SUB("RF networks",50)};
static const Entry networks[]={CAL("Series resistance",R_SERIES),CAL("Parallel resistance",R_PARALLEL),CAL("Voltage divider",V_DIVIDER),CAL("Current divider",I_DIVIDER)};
static const Entry laws[]={CAL("KVL: remaining voltage",KVL_REMAINDER),CAL("KCL: remaining current",KCL_REMAINDER),CAL("LED series resistor",LED_RESISTOR)};
static const Entry capacitors[]={CAL("Capacitor stored energy",CAP_ENERGY),CAL("RC capacitor charging",RC_CHARGE),CAL("RC capacitor discharging",RC_DISCHARGE)};
static const Entry series_rlc_menu[]={CAL("Series RLC impedance",RLC_MAG),CAL("Series RLC phase",RLC_PHASE)};
static const Entry time_constants[]={CAL("RC time constant",RC_TAU),CAL("RL time constant",RL_TAU)};
static const Entry battery_energy[]={CAL("Ah to Wh energy",AH_WH),CAL("Wh to Ah capacity",WH_AH),CAL("Average current",AVG_CURRENT)};
static const Entry battery_runtime[]={CAL("Estimated battery runtime",BAT_RUNTIME),CAL("Duty-cycle average power",DUTY_POWER)};
static const Entry regulators[]={CAL("LDO power dissipation",LDO_LOSS),CAL("DC-DC input power",DCDC_IN),CAL("DC-DC output power",DCDC_OUT),CAL("DC-DC efficiency",DCDC_EFF)};
static const Entry book_networks[]={BOOK("Series resistance",R_SERIES),BOOK("Parallel resistance",R_PARALLEL),BOOK("Voltage divider",V_DIVIDER),BOOK("Current divider",I_DIVIDER),SUB("Preferred values / codes",62),SUB("Tolerance and corners",63)};
static const Entry book_laws[]={BOOK("KVL: remaining voltage",KVL_REMAINDER),BOOK("KCL: remaining current",KCL_REMAINDER),BOOK("LED series resistor",LED_RESISTOR)};
static const Entry book_capacitors[]={BOOK("Capacitor stored energy",CAP_ENERGY),BOOK("RC capacitor charging",RC_CHARGE),BOOK("RC capacitor discharging",RC_DISCHARGE)};
static const Entry book_battery_energy[]={BOOK("Ah to Wh energy",AH_WH),BOOK("Wh to Ah capacity",WH_AH),BOOK("Average current",AVG_CURRENT)};
static const Entry book_battery_runtime[]={BOOK("Estimated battery runtime",BAT_RUNTIME),BOOK("Duty-cycle average power",DUTY_POWER)};
static const Entry book_regulators[]={BOOK("LDO power dissipation",LDO_LOSS),BOOK("DC-DC input power",DCDC_IN),BOOK("DC-DC output power",DCDC_OUT),BOOK("DC-DC efficiency",DCDC_EFF),SUB("PCB engineering tools",65)};
static const Entry new_levels[]={CAL("dBW to dBm",RF_DBW_DBM),CAL("dBm to dBW",RF_DBM_DBW),CAL("Power ratio to dB",RF_GAIN_DB),CAL("dB to power ratio",RF_GAIN_RATIO),CAL("Amplitude ratio to dB",RF_AMP_DB),CAL("dB to amplitude ratio",RF_AMP_RATIO),CAL("RF measured power ratio",RF_POWER_RATIO),CAL("RF power to RMS voltage",RF_VRMS),CAL("W to dBW",RF_W_DBW),CAL("dBW to W",RF_DBW_W)};
static const Entry new_gain[]={CAL("Cascaded gain and loss",RF_GAIN_SUM),CAL("Cascaded insertion loss",RF_LOSS_SUM),CAL("EIRP calculation",RF_EIRP),CAL("EIRP to ERP",RF_ERP),CAL("ERP to EIRP",RF_FROM_ERP),CAL("dBd to dBi",RF_DBI),CAL("dBi to dBd",RF_DBD)};
static const Entry new_noise[]={CAL("Thermal receiver noise W",RF_NOISE_W),CAL("Receiver noise floor dBm",RF_NOISE_DBM),CAL("Measured SNR",RF_SNR),CAL("Noise from signal and SNR",RF_NOISE_SNR),CAL("Noise figure to factor",RF_NF_FACTOR),CAL("Noise factor to figure",RF_FACTOR_NF),CAL("Friis cascade noise factor",RF_FRIIS_F),CAL("Friis cascade noise figure",RF_FRIIS_NF),CAL("Required receiver sensitivity",RF_SENS)};
static const Entry new_antenna[]={CAL("Half-wave electrical length",RF_HALF),CAL("Lossless dielectric velocity",RF_SPEED),CAL("Dielectric wavelength",RF_DIEL_WAVE)};
static const Entry new_propagation[]={CAL("Full RF link budget",RF_LINK),CAL("Receiver link margin",RF_MARGIN),CAL("Fresnel clearance percentage",RF_CLEAR),CAL("Effective-Earth radio horizon",RF_HORIZON),CAL("Linear polarization loss",RF_POL),CAL("Electrical phase length",RF_PHASE),CAL("Propagation phase shift",RF_DELAY)};
static const Entry new_filters[]={CAL("RC cutoff: low and high pass",RF_RC_CUTOFF),CAL("First-order low-pass magnitude",RF_LP),CAL("First-order high-pass magnitude",RF_HP),CAL("Series resonator Q",RF_SERIES_Q),CAL("Series resonator bandwidth",RF_SERIES_BW)};
static const Entry new_matching[]={CAL("Complex load reflection",RF_REFLECT),CAL("VSWR from reflection",RF_VSWR),CAL("Return loss",RF_RETURN),CAL("Reflection to impedance",RF_IMPEDANCE),CAL("Low-pass resistive L-match",RF_LMATCH_LP),CAL("High-pass resistive L-match",RF_LMATCH_HP)};
static const Entry book_new_levels[]={BOOK("dBW to dBm",RF_DBW_DBM),BOOK("dBm to dBW",RF_DBM_DBW),BOOK("Power ratio to dB",RF_GAIN_DB),BOOK("dB to power ratio",RF_GAIN_RATIO),BOOK("Amplitude ratio to dB",RF_AMP_DB),BOOK("dB to amplitude ratio",RF_AMP_RATIO),BOOK("RF measured power ratio",RF_POWER_RATIO),BOOK("RF power to RMS voltage",RF_VRMS),BOOK("W to dBW",RF_W_DBW),BOOK("dBW to W",RF_DBW_W)};
static const Entry book_new_gain[]={BOOK("Cascaded gain and loss",RF_GAIN_SUM),BOOK("Cascaded insertion loss",RF_LOSS_SUM),BOOK("EIRP calculation",RF_EIRP),BOOK("EIRP to ERP",RF_ERP),BOOK("ERP to EIRP",RF_FROM_ERP),BOOK("dBd to dBi",RF_DBI),BOOK("dBi to dBd",RF_DBD)};
static const Entry book_new_noise[]={BOOK("Thermal receiver noise W",RF_NOISE_W),BOOK("Receiver noise floor dBm",RF_NOISE_DBM),BOOK("Measured SNR",RF_SNR),BOOK("Noise from signal and SNR",RF_NOISE_SNR),BOOK("Noise figure to factor",RF_NF_FACTOR),BOOK("Noise factor to figure",RF_FACTOR_NF),BOOK("Friis cascade noise factor",RF_FRIIS_F),BOOK("Friis cascade noise figure",RF_FRIIS_NF),BOOK("Required receiver sensitivity",RF_SENS)};
static const Entry book_new_antenna[]={BOOK("Half-wave electrical length",RF_HALF),BOOK("Lossless dielectric velocity",RF_SPEED),BOOK("Dielectric wavelength",RF_DIEL_WAVE)};
static const Entry book_new_propagation[]={BOOK("Full RF link budget",RF_LINK),BOOK("Receiver link margin",RF_MARGIN),BOOK("Fresnel clearance percentage",RF_CLEAR),BOOK("Effective-Earth radio horizon",RF_HORIZON),BOOK("Linear polarization loss",RF_POL),BOOK("Electrical phase length",RF_PHASE),BOOK("Propagation phase shift",RF_DELAY)};
static const Entry book_new_filters[]={BOOK("RC cutoff: low and high pass",RF_RC_CUTOFF),BOOK("First-order low-pass magnitude",RF_LP),BOOK("First-order high-pass magnitude",RF_HP),BOOK("Series resonator Q",RF_SERIES_Q),BOOK("Series resonator bandwidth",RF_SERIES_BW)};
static const Entry book_new_matching[]={BOOK("Complex load reflection",RF_REFLECT),BOOK("VSWR from reflection",RF_VSWR),BOOK("Return loss",RF_RETURN),BOOK("Reflection to impedance",RF_IMPEDANCE),BOOK("Low-pass resistive L-match",RF_LMATCH_LP),BOOK("High-pass resistive L-match",RF_LMATCH_HP)};
static const Entry book_rf_networks[]={SUB("RC and resonators",48),SUB("Matching networks",49)};
static const Entry e3_copper[]={CAL("Copper trace resistance",PCB_TRACE_R),CAL("IPC-2221 external estimate",PCB_IPC_EXT),CAL("IPC-2221 internal estimate",PCB_IPC_INT),CAL("Copper weight to thickness",PCB_FOIL_T),CAL("Thickness to copper weight",PCB_FOIL_M),CAL("Plated via resistance",PCB_VIA_R)};
static const Entry e3_line[]={CAL("Microstrip effective ε",PCB_MS_EPS),CAL("Microstrip impedance",PCB_MS_Z),CAL("Guided propagation delay",PCB_DELAY),CAL("Guided wavelength",RF_DIEL_WAVE),CAL("Electrical phase length",RF_PHASE)};
static const Entry e3_vna_ref[]={CAL("Reflected power fraction",VNA_REFLECTED),CAL("Delivered power fraction",VNA_DELIVERED),CAL("Mismatch loss",VNA_MISMATCH),CAL("Return loss to reflection",VNA_RL_GAMMA),CAL("Complex load reflection",RF_REFLECT),CAL("Reflection to impedance",RF_IMPEDANCE),CAL("VSWR from reflection",RF_VSWR),CAL("Return loss",RF_RETURN)};
static const Entry e3_vna_trans[]={CAL("S21 dB to insertion loss",VNA_S21_DB),CAL("S21 linear insertion loss",VNA_S21_LIN),CAL("Complex impedance magnitude",VNA_Z_MAG),CAL("Complex impedance phase",VNA_Z_PHASE),CAL("Amplitude ratio to dB",RF_AMP_DB),CAL("dB to amplitude ratio",RF_AMP_RATIO)};
static const Entry e3_parts[]={CAL("E12 nearest resistor",LAB_E12),CAL("E24 nearest resistor",LAB_E24),CAL("SMD 3 digit code",LAB_SMD3),CAL("SMD 4 digit code",LAB_SMD4),CAL("Series resistance",R_SERIES),CAL("Parallel resistance",R_PARALLEL)};
static const Entry e3_tolerance[]={CAL("Resistor tolerance limits",LAB_TOL_R),CAL("Capacitor tolerance limits",LAB_TOL_C),CAL("Inductor tolerance limits",LAB_TOL_L),CAL("Divider worst-case minimum",LAB_DIV_MIN),CAL("Divider worst-case maximum",LAB_DIV_MAX),CAL("RC worst-case minimum",LAB_RC_MIN),CAL("RC worst-case maximum",LAB_RC_MAX)};
static const Entry e3_data[]={CAL("Absolute scalar error",LAB_ABS_ERROR),CAL("Relative scalar error",LAB_REL_ERROR),CAL("Mean: 4 readings",LAB_MEAN),CAL("Population SD: 4 readings",LAB_POP_SD),CAL("Sample SD: 4 readings",LAB_SAMPLE_SD),CAL("Mean standard uncertainty",LAB_MEAN_U),CAL("Rectangular standard u",LAB_RECT_U),CAL("Independent combined u",LAB_COMBINED_U),CAL("Expanded uncertainty",LAB_EXPANDED_U),CAL("Significant digits / SI",LAB_FORMAT)};
static const Entry book_e3_copper[]={BOOK("Copper trace resistance",PCB_TRACE_R),BOOK("IPC-2221 external estimate",PCB_IPC_EXT),BOOK("IPC-2221 internal estimate",PCB_IPC_INT),BOOK("Copper weight to thickness",PCB_FOIL_T),BOOK("Thickness to copper weight",PCB_FOIL_M),BOOK("Plated via resistance",PCB_VIA_R)};
static const Entry book_e3_line[]={BOOK("Microstrip effective ε",PCB_MS_EPS),BOOK("Microstrip impedance",PCB_MS_Z),BOOK("Guided propagation delay",PCB_DELAY),BOOK("Guided wavelength",RF_DIEL_WAVE),BOOK("Electrical phase length",RF_PHASE)};
static const Entry book_e3_vna_ref[]={BOOK("Reflected power fraction",VNA_REFLECTED),BOOK("Delivered power fraction",VNA_DELIVERED),BOOK("Mismatch loss",VNA_MISMATCH),BOOK("Return loss to reflection",VNA_RL_GAMMA),BOOK("Complex load reflection",RF_REFLECT),BOOK("Reflection to impedance",RF_IMPEDANCE),BOOK("VSWR from reflection",RF_VSWR),BOOK("Return loss",RF_RETURN)};
static const Entry book_e3_vna_trans[]={BOOK("S21 dB to insertion loss",VNA_S21_DB),BOOK("S21 linear insertion loss",VNA_S21_LIN),BOOK("Complex impedance magnitude",VNA_Z_MAG),BOOK("Complex impedance phase",VNA_Z_PHASE),BOOK("Amplitude ratio to dB",RF_AMP_DB),BOOK("dB to amplitude ratio",RF_AMP_RATIO)};
static const Entry book_e3_parts[]={BOOK("E12 nearest resistor",LAB_E12),BOOK("E24 nearest resistor",LAB_E24),BOOK("SMD 3 digit code",LAB_SMD3),BOOK("SMD 4 digit code",LAB_SMD4),BOOK("Series resistance",R_SERIES),BOOK("Parallel resistance",R_PARALLEL)};
static const Entry book_e3_tolerance[]={BOOK("Resistor tolerance limits",LAB_TOL_R),BOOK("Capacitor tolerance limits",LAB_TOL_C),BOOK("Inductor tolerance limits",LAB_TOL_L),BOOK("Divider worst-case minimum",LAB_DIV_MIN),BOOK("Divider worst-case maximum",LAB_DIV_MAX),BOOK("RC worst-case minimum",LAB_RC_MIN),BOOK("RC worst-case maximum",LAB_RC_MAX)};
static const Entry book_e3_data[]={BOOK("Absolute scalar error",LAB_ABS_ERROR),BOOK("Relative scalar error",LAB_REL_ERROR),BOOK("Mean: 4 readings",LAB_MEAN),BOOK("Population SD: 4 readings",LAB_POP_SD),BOOK("Sample SD: 4 readings",LAB_SAMPLE_SD),BOOK("Mean standard uncertainty",LAB_MEAN_U),BOOK("Rectangular standard u",LAB_RECT_U),BOOK("Independent combined u",LAB_COMBINED_U),BOOK("Expanded uncertainty",LAB_EXPANDED_U),BOOK("Significant digits / SI",LAB_FORMAT)};
static const Entry book_e3_pcb[]={SUB("Copper geometry",58),SUB("Microstrip and delay",59)};
static const Entry book_e3_vna[]={SUB("VNA reflection",60),SUB("VNA transmission",61)};
static const Entry lora_phy[]={CAL("LoRa symbol duration",LORA_SYMBOL_T),CAL("LoRa symbol rate",LORA_SYMBOL_RATE),CAL("LoRa nominal bit rate",LORA_BITRATE),CAL("LoRa coding fraction",LORA_CODE_RATE),CAL("LoRa coding index",LORA_CODE_INDEX)};
static const Entry lora_packet[]={CAL("LoRa packet airtime",LORA_AIRTIME),CAL("LoRa preamble duration",LORA_PREAMBLE_T),CAL("LoRa payload duration",LORA_PAYLOAD_T),CAL("LoRa payload symbols",LORA_PAYLOAD_SYMBOLS)};
static const Entry lora_budget[]={CAL("Packet duty fraction",LORA_DUTY),CAL("Packet budget count",LORA_PACKET_COUNT),CAL("Battery TX energy",LORA_TX_ENERGY)};
static const Entry book_lora_phy[]={BOOK("LoRa symbol duration",LORA_SYMBOL_T),BOOK("LoRa symbol rate",LORA_SYMBOL_RATE),BOOK("LoRa nominal bit rate",LORA_BITRATE),BOOK("LoRa coding fraction",LORA_CODE_RATE),BOOK("LoRa coding index",LORA_CODE_INDEX)};
static const Entry book_lora_packet[]={BOOK("LoRa packet airtime",LORA_AIRTIME),BOOK("LoRa preamble duration",LORA_PREAMBLE_T),BOOK("LoRa payload duration",LORA_PAYLOAD_T),BOOK("LoRa payload symbols",LORA_PAYLOAD_SYMBOLS)};
static const Entry book_lora_budget[]={BOOK("Packet duty fraction",LORA_DUTY),BOOK("Packet budget count",LORA_PACKET_COUNT),BOOK("Battery TX energy",LORA_TX_ENERGY)};
#define M(a) {a,sizeof(a)/sizeof((a)[0])}
static const Menu menus[]={M(root),M(rf),M(basic),M(rlc),M(power),M(filter),M(pcb),M(antenna),M(lab),M(engineering),M(conversion),M(propagation),M(dc),M(resonance),M(reactance),M(dc_power),M(lengths),M(handbook_index),M(book_dc),M(book_rf),M(book_waves),M(book_rlc),M(networks),M(laws),M(capacitors),M(series_rlc_menu),M(time_constants),M(battery_energy),M(battery_runtime),M(regulators),M(book_networks),M(book_laws),M(book_capacitors),M(book_battery_energy),M(book_battery_runtime),M(book_regulators),M(new_levels),M(new_gain),M(new_noise),M(new_antenna),M(new_propagation),M(new_filters),M(new_matching),M(book_new_levels),M(book_new_gain),M(book_new_noise),M(book_new_antenna),M(book_new_propagation),M(book_new_filters),M(book_new_matching),M(book_rf_networks),M(e3_copper),M(e3_line),M(e3_vna_ref),M(e3_vna_trans),M(e3_parts),M(e3_tolerance),M(e3_data),M(book_e3_copper),M(book_e3_line),M(book_e3_vna_ref),M(book_e3_vna_trans),M(book_e3_parts),M(book_e3_tolerance),M(book_e3_data),M(book_e3_pcb),M(book_e3_vna),M(lora_phy),M(lora_packet),M(lora_budget),M(book_lora_phy),M(book_lora_packet),M(book_lora_budget)};
static const char *localized_meaning(VariableId id){return tr(variable_meaning(id));}
void app_init(App *a){loc_set(LANG_EN);memset(a,0,sizeof(*a));a->running=1;a->screen=S_SPLASH;}
static void open_calc(App *a,int id){unsigned i;a->calc=(CalcId)id;a->handbook_origin=0;a->screen=S_FORM;a->row=0;a->error=OK;a->input_error=NUM_OK;a->result_valid=0;a->edit_cursor=0;a->input_mode=INPUT_LIST;a->mode_fallback=0;memset(a->stored_valid,0,sizeof(a->stored_valid));memset(a->stored_values,0,sizeof(a->stored_values));memset(a->stored_units,0,sizeof(a->stored_units));a->output_unit=(id==DBM_MW?1:0);for(i=0;i<CALC_MAX_INPUTS;i++){a->units[i]=0;a->values[i][0]=0;}if(id==MW_DBM)a->units[0]=1;
 if(id==AH_WH){a->units[0]=1;a->output_unit=2;}
 if(id==WH_AH){a->units[0]=2;a->output_unit=1;}
 if(id==AVG_CURRENT){a->units[0]=2;a->units[1]=4;a->output_unit=1;}
 if(id==BAT_RUNTIME){a->units[0]=2;a->units[1]=1;a->units[2]=1;a->output_unit=4;}
 if(id==DUTY_POWER)a->units[2]=1;
 if(id==DCDC_IN||id==DCDC_OUT)a->units[1]=1;
 if(id==DCDC_EFF||id==RLC_PHASE)a->output_unit=1;
}
static Screen input_screen(const App *a){return a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM;}
int app_request_formula(App *a,int w,int h){
 if(!formula_interactive_fits(a->calc,w,h)){a->input_mode=INPUT_LIST;a->screen=S_FORM;a->mode_fallback=1;return 0;}
 a->input_mode=INPUT_FORMULA;a->screen=S_FORMULA;a->mode_fallback=0;if(a->row>=calculations[a->calc].count)a->row=0;return 1;
}
static unsigned targets(CalcId id,CalcId out[3]){
 if(id==LORA_CODE_RATE||id==LORA_CODE_INDEX){out[0]=LORA_CODE_RATE;out[1]=LORA_CODE_INDEX;return 2;}
 if(id==LORA_SYMBOL_T||id==LORA_SYMBOL_RATE){out[0]=LORA_SYMBOL_T;out[1]=LORA_SYMBOL_RATE;return 2;}
 if(id==LORA_AIRTIME||id==LORA_PAYLOAD_T||id==LORA_PREAMBLE_T){out[0]=LORA_AIRTIME;out[1]=LORA_PAYLOAD_T;out[2]=LORA_PREAMBLE_T;return 3;}

 if(id==PCB_FOIL_T||id==PCB_FOIL_M){out[0]=PCB_FOIL_T;out[1]=PCB_FOIL_M;return 2;}
 if(id==PCB_MS_EPS||id==PCB_MS_Z){out[0]=PCB_MS_EPS;out[1]=PCB_MS_Z;return 2;}
 if(id==LAB_DIV_MIN||id==LAB_DIV_MAX){out[0]=LAB_DIV_MIN;out[1]=LAB_DIV_MAX;return 2;}
 if(id==LAB_RC_MIN||id==LAB_RC_MAX){out[0]=LAB_RC_MIN;out[1]=LAB_RC_MAX;return 2;}

 if(id==RF_GAIN_DB||id==RF_GAIN_RATIO){out[0]=RF_GAIN_DB;out[1]=RF_GAIN_RATIO;return 2;}
 if(id==RF_AMP_DB||id==RF_AMP_RATIO){out[0]=RF_AMP_DB;out[1]=RF_AMP_RATIO;return 2;}
 if(id==RF_NF_FACTOR||id==RF_FACTOR_NF){out[0]=RF_NF_FACTOR;out[1]=RF_FACTOR_NF;return 2;}
 if(id==RF_NOISE_W||id==RF_NOISE_DBM){out[0]=RF_NOISE_W;out[1]=RF_NOISE_DBM;return 2;}
 if(id==RF_FRIIS_F||id==RF_FRIIS_NF){out[0]=RF_FRIIS_F;out[1]=RF_FRIIS_NF;return 2;}
 if(id==RF_W_DBW||id==RF_DBW_W){out[0]=RF_W_DBW;out[1]=RF_DBW_W;return 2;}
 if(id==RF_SNR||id==RF_NOISE_SNR){out[0]=RF_SNR;out[1]=RF_NOISE_SNR;return 2;}

 if(id==RLC_MAG||id==RLC_PHASE){out[0]=RLC_MAG;out[1]=RLC_PHASE;return 2;}
 if(id==AH_WH||id==WH_AH){out[0]=AH_WH;out[1]=WH_AH;return 2;}
 if(id==DCDC_IN||id==DCDC_OUT||id==DCDC_EFF){out[0]=DCDC_IN;out[1]=DCDC_OUT;out[2]=DCDC_EFF;return 3;}
 if(id==VOLTAGE||id==CURRENT||id==RESISTANCE){out[0]=VOLTAGE;out[1]=CURRENT;out[2]=RESISTANCE;return 3;}
 if(id==LC_FREQ||id==LC_CAP||id==LC_INDUCTANCE){out[0]=LC_FREQ;out[1]=LC_INDUCTANCE;out[2]=LC_CAP;return 3;}
 if(id==MW_DBM||id==DBM_MW){out[0]=MW_DBM;out[1]=DBM_MW;return 2;}
 return 0;
}
static void switch_target(App *a,CalcId id){
 const Calculation *old=&calculations[a->calc],*next=&calculations[id];unsigned i;VariableId v;
 if(id==a->calc){a->screen=a->target_return;return;}
 for(i=0;i<old->count;i++){v=calc_input_id(a->calc,i);strcpy(a->stored_values[v],a->values[i]);a->stored_units[v]=a->units[i];a->stored_valid[v]=1;}
 v=calc_output_id(a->calc);a->stored_units[v]=a->output_unit;a->stored_valid[v]=1;
 if(a->result_valid)snprintf(a->stored_values[v],32,"%.17g",a->result/old->outputs[a->output_unit].scale);
 a->calc=id;a->row=0;a->result_valid=0;a->error=OK;a->input_error=NUM_OK;a->mode_fallback=0;
 for(i=0;i<next->count;i++){v=calc_input_id(id,i);if(a->stored_valid[v]){strcpy(a->values[i],a->stored_values[v]);a->units[i]=a->stored_units[v];}else{a->values[i][0]=0;a->units[i]=(id==MW_DBM?1:0);}}
 v=calc_output_id(id);a->output_unit=a->stored_valid[v]?a->stored_units[v]:(id==DBM_MW?1:0);
 a->screen=input_screen(a);if(a->input_mode==INPUT_FORMULA)app_request_formula(a,310,58);
}
static void open_target(App *a){CalcId options[3];unsigned i,n=targets(a->calc,options);if(!n)return;a->target_return=a->screen;a->target_cursor=0;for(i=0;i<n;i++)if(options[i]==a->calc)a->target_cursor=i;a->screen=S_TARGET;}
static void open_editor(App *a){a->edit_return=a->screen;strcpy(a->edit,a->values[a->row]);a->edit_cursor=(unsigned)strlen(a->edit);a->input_error=NUM_OK;a->error=OK;a->error_field=a->row;a->screen=S_EDIT;}
static void open_units(App *a,int output){a->units_return=a->screen;a->units_focus=a->row;if(output)a->row=calculations[a->calc].count;a->unit_cursor=a->row<calculations[a->calc].count?a->units[a->row]:a->output_unit;a->screen=S_UNITS;}
static void evaluate(App *a){
 double x[CALC_MAX_INPUTS]={0};unsigned i;const Calculation *c=&calculations[a->calc];a->result_valid=0;a->error=OK;a->input_error=NUM_OK;
 for(i=0;i<c->count;i++){
  a->error_field=i;a->input_error=numeric_parse(a->values[i],c->inputs[i].units[a->units[i]].scale,&x[i]);
  if(a->input_error!=NUM_OK){a->error=INVALID;a->row=i;return;}
  if(!parameter_valid(a->calc,i,x[i])){
   a->input_error=x[i]==0&&(a->calc==CURRENT||a->calc==RESISTANCE)?NUM_ZERO:NUM_DOMAIN;
   a->error=INVALID;a->row=i;return;
  }
 }
 a->error=calculate(a->calc,x,&a->result);
 if(a->error==OK){double displayed;a->error=convert_output(a->result,c->outputs[a->output_unit].scale,&displayed);if(a->error==OK){a->result_valid=1;a->screen=S_RESULT;}}
 else if(a->error==INVALID){a->input_error=NUM_DOMAIN;a->error_field=c->count;}
}
static const char *model_notice(CalcId id){
 if(id>=LORA_FIRST_ID&&id<CALC_COUNT)return "LoRa model; check local RF limits";
 if(id==PCB_MS_EPS||id==PCB_MS_Z)return "Zero-thickness microstrip; H limits";
 if(id==PCB_IPC_EXT||id==PCB_IPC_INT)return "IPC-2221 estimate; not a safety rating";
 if(id==LAB_MEAN||id==LAB_POP_SD||id==LAB_SAMPLE_SD||id==LAB_MEAN_U)return "4 normalized readings; H assumptions";
 if(id==LAB_SMD3||id==LAB_SMD4)return "Numeric codes only; R/EIA-96 excluded";
 return 0;
}
static void input_error_message(const App *a,int y){
 char msg[96];
 if(a->input_error!=NUM_OK){
  const Calculation *c=&calculations[a->calc];const char *reason=tr(numeric_error_text(a->input_error));
  if(a->error_field>=c->count)snprintf(msg,sizeof(msg),"%s",tr(a->calc==LORA_AIRTIME||a->calc==LORA_PAYLOAD_T?"Check LoRa flags: Ts>16ms needs DE=1":a->calc==LORA_DUTY?"TX airtime must not exceed interval":a->calc==RESISTANCE?"Result: passive resistance V/I must be >=0":"Check model limits and related inputs"));
  else{
   if(a->input_error==NUM_DOMAIN)reason=a->calc>=LEGACY_CALC_COUNT?tr("Outside valid input domain; see H Info"):a->calc==VOLTAGE?tr("Resistance must be >=0"):tr("Must be greater than zero");
   snprintf(msg,sizeof(msg),"%s: %s",c->inputs[a->error_field].symbol,reason);
  }
  wrapped_text(13,y,294,2,0,msg,1);
 }
 else if(a->error!=OK)wrapped_text(13,y,294,2,0,tr(status_text(a->error)),1);
}
static void rf_detail(const App *a){
 const Calculation *c=&calculations[a->calc];double si[CALC_MAX_INPUTS],re,im;unsigned i;char b[128];
 if(!a->result_valid||!rf_has_details(a->calc))return;
 for(i=0;i<c->count;i++)if(numeric_parse(a->values[i],c->inputs[i].units[a->units[i]].scale,&si[i])!=NUM_OK)return;
 if(a->calc==RF_REFLECT&&rf_reflection(si,&re,&im)==OK)snprintf(b,sizeof(b),"Γ = %.6g %+.6gj",re,im);
 else if(a->calc==RF_IMPEDANCE&&rf_impedance(si,&re,&im)==OK)snprintf(b,sizeof(b),"Z = %.6g %+.6gj Ω",re,im);
 else if((a->calc==RF_LMATCH_LP||a->calc==RF_LMATCH_HP)&&rf_lmatch(si,a->calc==RF_LMATCH_HP,&re,&im)==OK){
  const char *side=tr(si[0]>si[1]?"Source shunt":"Load shunt");
  snprintf(b,sizeof(b),"%s %s = %.6g %s",side,a->calc==RF_LMATCH_LP?"C":"L",im/(a->calc==RF_LMATCH_LP?1e-12:1e-9),a->calc==RF_LMATCH_LP?"pF":"nH");
 }else return;
 text_fit(13,205,294,b,1);
}
static void lab_detail(const App *a){
 const Calculation *c=&calculations[a->calc];double x[CALC_MAX_INPUTS],lo,hi,shown,mant;unsigned i;int e,nd;char b[144],sci[40],eng[48];const char *prefix;
 if(!a->result_valid||!lab_details(a->calc))return;
 for(i=0;i<c->count;i++)if(numeric_parse(a->values[i],c->inputs[i].units[a->units[i]].scale,&x[i])!=NUM_OK)return;
 if(a->calc==PCB_MS_EPS||a->calc==PCB_MS_Z){if(lab_microstrip(x,&lo,&hi)!=OK)return;snprintf(b,sizeof(b),"ε_e = %.6g; Z0 = %.6g Ω",lo,hi);}
 else if(a->calc==LAB_FORMAT){
  nd=(int)x[1];shown=a->result;e=shown==0?0:(int)(3*floor(log10(fabs(shown))/3));
  mant=shown==0?0:exp(log(fabs(shown))-e*log(10.0));if(shown<0)mant=-mant;
  prefix=e==-12?"p":e==-9?"n":e==-6?"μ":e==-3?"m":e==0?"":e==3?"k":e==6?"M":e==9?"G":e==12?"T":0;
  snprintf(sci,sizeof(sci),"%.*E",nd-1,shown);
  if(prefix&&*prefix)snprintf(eng,sizeof(eng),"%.*g E%+d(%s)",nd,mant,e,prefix);else if(prefix)snprintf(eng,sizeof(eng),"%.*g E%+d",nd,mant,e);else snprintf(eng,sizeof(eng),"%.*g E%+d",nd,mant,e);
  snprintf(b,sizeof(b),"%s | %s",sci,eng);
 }else{
  hi=x[0]*(1+x[1]);if(convert_output(hi,c->outputs[a->output_unit].scale,&shown)!=OK){text_fit(13,205,294,tr("Upper limit exceeds display range"),1);return;}
  snprintf(b,sizeof(b),tr("Upper limit: %.8g %s"),shown,c->outputs[a->output_unit].unit);
 }
 text_fit(13,205,294,b,1);
}
static void open_info(App *a,int from_index){a->info_return=a->screen;a->handbook_origin=from_index;a->info_page=0;a->info_scroll=0;a->screen=S_INFO;}
static void select_entry(App *a){
 const Entry *e=&menus[a->menu].entries[a->selected];
 if(e->child>=0&&a->depth<8){a->parents[a->depth]=a->menu;a->selections[a->depth++]=a->selected;a->menu=e->child;a->selected=0;}
 else if(e->child==-2)a->screen=S_SYMBOLS;
 else if(e->child==-4){a->calc=(CalcId)e->calc;open_info(a,1);}
 else if(e->calc>=0)open_calc(a,e->calc);
}
void app_key(App *a,int k){
 const Calculation *c=&calculations[a->calc];
 if(!a->running||k==K_NONE)return;
 if(k==K_SETTINGS&&a->screen!=S_SETTINGS&&a->screen!=S_LANGUAGE&&a->screen!=S_SPLASH){a->settings_return=a->screen;a->screen=S_SETTINGS;a->language_saved=-1;return;}
 if(a->screen==S_SETTINGS){
  if(k==K_ESC){a->screen=a->settings_return;if(a->screen==S_INFO){unsigned n=app_info_lines(a);if(a->info_scroll>(n>11?n-11:0))a->info_scroll=n>11?n-11:0;}}
  if(k==K_ENTER||k==K_DIGIT0+1){a->language_cursor=loc_get()==LANG_EN?0:1;a->screen=S_LANGUAGE;}
  return;
 }
 if(a->screen==S_LANGUAGE){
  if(k==K_ESC){a->screen=S_SETTINGS;return;}
  if(k==K_UP||k==K_DOWN||k==K_LEFT||k==K_RIGHT||k==K_TAB)a->language_cursor=(a->language_cursor+1)%2;
  if(k==K_ENTER||k==K_DIGIT0+1||k==K_DIGIT0+2){
   if(k!=K_ENTER)a->language_cursor=(unsigned)(k-K_DIGIT0-1);
   loc_set(a->language_cursor==0?LANG_EN:LANG_TR_ASCII);a->language_saved=settings_save(loc_get());
  }
  return;
 }
 if(a->screen==S_SPLASH){if(k==K_ESC)a->running=0;else a->screen=S_MENU;return;}
 if(k==K_INFO&&(a->screen==S_FORMULA||a->screen==S_FORM||a->screen==S_EDIT||a->screen==S_UNITS||a->screen==S_RESULT)){open_info(a,0);return;}
 if(k==K_MODE&&(a->screen==S_FORM||a->screen==S_FORMULA||a->screen==S_RESULT)){
  if(a->input_mode==INPUT_FORMULA){a->input_mode=INPUT_LIST;a->screen=S_FORM;}else app_request_formula(a,310,58);return;
 }
 if(k==K_OUTPUT_UNITS&&(a->screen==S_FORM||a->screen==S_FORMULA||(a->screen==S_RESULT&&a->input_mode==INPUT_FORMULA))){open_units(a,1);return;}
 if(k==K_TARGET&&(a->screen==S_FORM||a->screen==S_FORMULA||a->screen==S_RESULT)){open_target(a);return;}
 if(a->screen==S_TARGET){
  CalcId options[3];unsigned n=targets(a->calc,options);
  if(k==K_ESC){a->screen=a->target_return;return;}
  if(k==K_UP||k==K_LEFT)a->target_cursor=(a->target_cursor+n-1)%n;
  if(k==K_DOWN||k==K_RIGHT||k==K_TAB)a->target_cursor=(a->target_cursor+1)%n;
  if(k==K_ENTER)switch_target(a,options[a->target_cursor]);
  else if(k>K_DIGIT0&&k<=K_DIGIT0+(int)n)switch_target(a,options[k-K_DIGIT0-1]);
  return;
 }
 if(a->screen==S_RESULT&&a->input_mode==INPUT_FORMULA&&k!=K_ESC)a->screen=S_FORMULA;
 if(a->screen==S_FORMULA){
  if(k==K_LEFT)a->row=(a->row+c->count-1)%c->count;
  if(k==K_RIGHT||k==K_TAB)a->row=(a->row+1)%c->count;
  if(k==K_ENTER)open_editor(a);
  if(k==K_UNITS)open_units(a,0);
  if(k==K_CALCULATE)evaluate(a);
  if(k==K_ESC)a->screen=S_MENU;
  return;
 }
 if(a->screen==S_MENU){const Menu *m=&menus[a->menu];
  if(k==K_UP)a->selected=(a->selected+(int)m->count-1)%(int)m->count;
  if(k==K_DOWN)a->selected=(a->selected+1)%(int)m->count;
  if(k==K_ESC){if(a->depth){a->depth--;a->menu=a->parents[a->depth];a->selected=a->selections[a->depth];}else a->running=0;}
  if(k==K_ENTER)select_entry(a);
  if(k>=K_DIGIT0&&k<K_DIGIT0+10){int item=k-K_DIGIT0;item=item==0?10:item;if(item<=(int)m->count){a->selected=item-1;select_entry(a);}}
  if(k==K_INFO){const Entry *e=&m->entries[a->selected];if(e->calc>=0){a->calc=(CalcId)e->calc;open_info(a,1);}}
 }else if(a->screen==S_FORM){
  if(k==K_CALCULATE){evaluate(a);return;}
  if(k==K_ESC)a->screen=S_MENU;
  if(k==K_UP)a->row=(a->row+c->count+2)%(c->count+3);
  if(k==K_DOWN)a->row=(a->row+1)%(c->count+3);
  if(k==K_LEFT||k==K_RIGHT){if(a->row<c->count){open_units(a,0);}else if(a->row==c->count){open_units(a,0);}}
  if(k==K_ENTER){if(a->row<c->count){open_editor(a);}else if(a->row==c->count){open_units(a,0);}else if(a->row==c->count+1)evaluate(a);else open_info(a,0);}
 }else if(a->screen==S_EDIT){size_t n=strlen(a->edit);char ch=0;
  if(k==K_ESC){a->screen=a->edit_return;a->input_error=NUM_OK;a->error=OK;}
  else if(k==K_ENTER){double v;
   a->input_error=numeric_parse(a->edit,c->inputs[a->row].units[a->units[a->row]].scale,&v);
   if(a->input_error==NUM_OK&&!parameter_valid(a->calc,a->row,v))a->input_error=v==0&&(a->calc==CURRENT||a->calc==RESISTANCE)?NUM_ZERO:NUM_DOMAIN;
   if(a->input_error==NUM_OK){strcpy(a->values[a->row],a->edit);a->result_valid=0;a->error=OK;a->screen=a->edit_return;}else a->error=INVALID;
  }
  else if(k==K_LEFT){if(a->edit_cursor)a->edit_cursor--;}
  else if(k==K_RIGHT){if(a->edit_cursor<n)a->edit_cursor++;}
  else if(k==K_HOME)a->edit_cursor=0;
  else if(k==K_END)a->edit_cursor=(unsigned)n;
  else if(k==K_CLEAR){a->edit[0]=0;a->edit_cursor=0;a->input_error=NUM_OK;a->error=OK;}
  else if(k==K_DELETE){numeric_backspace(a->edit,&a->edit_cursor);a->input_error=NUM_OK;a->error=OK;}
  else if(k==K_FORWARD_DELETE){numeric_delete(a->edit,a->edit_cursor);a->input_error=NUM_OK;a->error=OK;}
  else{
   if(k>=K_DIGIT0&&k<K_DIGIT0+10)ch=(char)('0'+k-K_DIGIT0);
   if(k==K_DOT)ch='.';
   if(k==K_SIGN)ch='-';
   if(k==K_PLUS)ch='+';
   if(k==K_EXP)ch='E';
   if(ch){a->input_error=numeric_insert(a->edit,sizeof(a->edit),&a->edit_cursor,ch)?NUM_OK:NUM_FULL;a->error=a->input_error==NUM_OK?OK:INVALID;}
  }
 }else if(a->screen==S_UNITS){unsigned count=a->row<c->count?c->inputs[a->row].count:c->output_count;
  if(k==K_UP)a->unit_cursor=(a->unit_cursor+count-1)%count;
  if(k==K_DOWN)a->unit_cursor=(a->unit_cursor+1)%count;
  if(k==K_ESC){a->screen=a->units_return;a->row=a->units_focus;}
  if(k==K_ENTER){if(a->row<c->count)a->units[a->row]=a->unit_cursor;else a->output_unit=a->unit_cursor;a->result_valid=0;a->error=OK;a->input_error=NUM_OK;a->screen=a->units_return;a->row=a->units_focus;}
  if(k>K_DIGIT0&&k<K_DIGIT0+10&&(unsigned)(k-K_DIGIT0)<=count){a->unit_cursor=(unsigned)(k-K_DIGIT0-1);if(a->row<c->count)a->units[a->row]=a->unit_cursor;else a->output_unit=a->unit_cursor;a->result_valid=0;a->error=OK;a->input_error=NUM_OK;a->screen=a->units_return;a->row=a->units_focus;}
 }else if(a->screen==S_RESULT){if(k==K_ESC)a->screen=input_screen(a);if(k==K_ENTER)open_info(a,0);}
 else if(a->screen==S_INFO){
  unsigned old=a->info_page,total=app_info_lines(a);
  if(k==K_ESC||k==K_INFO)a->screen=a->info_return;
  else if(k==K_ENTER){if(a->handbook_origin)open_calc(a,a->calc);else a->screen=a->info_return;}
  else if(k==K_LEFT)a->info_page=(a->info_page+HANDBOOK_PAGES-1)%HANDBOOK_PAGES;
  else if(k==K_RIGHT)a->info_page=(a->info_page+1)%HANDBOOK_PAGES;
  else if(k==K_UP){if(a->info_scroll)a->info_scroll--;}
  else if(k==K_DOWN){if(total>11&&a->info_scroll<total-11)a->info_scroll++;}
  else if(k>K_DIGIT0&&k<=K_DIGIT0+HANDBOOK_PAGES)a->info_page=(unsigned)(k-K_DIGIT0-1);
  if(old!=a->info_page)a->info_scroll=0;
 }
 else if(a->screen==S_SYMBOLS&&k==K_ESC)a->screen=S_MENU;
}
static void logo_pixel(int x,int y,int black){rect(x,y,1,1,black);}
static void panel(int x,int y,int w,int h){rect(x,y,w,h,0);frame(x,y,w,h,1);}
static void header(const char *title){
 rect(0,0,320,28,1);text(7,10,tr("C21 LABS / ENGINEERING"),0);
 rect(291,0,29,28,0);c21_draw_mono(294,2,C21_CORNER_W,C21_CORNER_H,c21_corner_bits,logo_pixel);
 text_fit(7,34,306,title,1);rect(6,46,308,1,1);
}
static void footer(const char *s){rect(0,221,320,1,1);text_fit(6,228,308,s,1);}
static void line(int y,const char *s,int selected){
 if(selected){rect(7,y-3,306,15,1);text_fit(13,y,294,s,0);}
 else text_fit(13,y,294,s,1);
}
static void input_box(int y,const char *symbol,const char *label,const char *value,const char *unit,int focus){
 char b[96];snprintf(b,sizeof(b),"%s - %s",symbol,label);text_fit(12,y,292,b,1);
 panel(12,y+12,234,18);panel(251,y+12,57,18);
 text_fit(18,y+17,222,value&&*value?value:tr("Enter value"),1);text_fit(257,y+17,45,unit,1);
 if(focus){frame(10,y+10,238,22,1);rect(5,y+15,3,10,1);}
}
static void result_panel(const App *a,int y,int h){
 const Calculation *c=&calculations[a->calc];char b[96];double shown;
 panel(7,y,306,h);rect(8,y+1,304,12,1);text(13,y+3,tr("RESULT"),0);
 if(a->result_valid&&convert_output(a->result,c->outputs[a->output_unit].scale,&shown)==OK)
  {if(a->calc==LORA_PACKET_COUNT)snprintf(b,sizeof(b),"N = %.0f pkt",shown);else if(a->calc==LAB_TOL_R||a->calc==LAB_TOL_C||a->calc==LAB_TOL_L)snprintf(b,sizeof(b),tr("Lower limit: %.10g %s"),shown,c->outputs[a->output_unit].unit);else snprintf(b,sizeof(b),"%s = %.10g %s",variable_symbol(calc_output_id(a->calc)),shown,c->outputs[a->output_unit].unit);}
 else snprintf(b,sizeof(b),"%s  --  %s",variable_symbol(calc_output_id(a->calc)),c->outputs[a->output_unit].unit);
 text_fit(13,y+17,294,b,1);
}
static void formula_view(const App *a){
 const Calculation *c=&calculations[a->calc];MathInteraction paint;unsigned row=a->row<c->count?a->row:0,i;char b[96];
 panel(7,51,306,78);text(13,55,tr("FORMULA     O Output   C Calculate"),1);
 formula_interactive(a->calc,calc_input_id(a->calc,row),5,68,310,58,&paint);
 input_box(134,c->inputs[row].symbol,localized_meaning(calc_input_id(a->calc,row)),a->values[row],c->inputs[row].units[a->units[row]].unit,1);
 for(i=0;i<c->count;i++){int x=8+(int)i*(304/(int)c->count);snprintf(b,sizeof(b),"%s:%s%s",c->inputs[i].symbol,a->values[i][0]?a->values[i]:"?",c->inputs[i].units[a->units[i]].unit);text_fit(x,171,304/(int)c->count-4,b,1);}
 if(a->error!=OK||a->input_error!=NUM_OK){panel(7,186,306,31);input_error_message(a,189);}
 else result_panel(a,186,31);
 footer(tr(model_notice(a->calc)?model_notice(a->calc):a->result_valid&&rf_regulatory(a->calc)?"RF: check local limits; not certification":"←→/Tab Field Enter Edit F List H Info"));
}
static const char *info_body(const App *a){
 const HandbookEntry *h=&handbook[a->calc];const Calculation *c=&calculations[a->calc];
 switch(a->info_page){case 1:return tr(c->definitions);case 2:return tr(h->purpose);case 3:return tr(h->measurements);case 4:return tr(h->laboratory);case 5:return tr(h->limits);case 7:return tr(h->mistakes);default:return "";}
}
static void info_text(const App *a,char body[1024]){
 const char *extra=a->info_page==1?tr("\nc = 299792458 m/s; π = 3.141592654\nj = √(-1); λ = c/f; log10: base 10"):a->info_page==3?tr("\nInput SI = number × unit multiplier.\nOutput = SI / unit multiplier.\nChoose each unit in the calculator."):"";
 snprintf(body,1024,"%s%s",info_body(a),extra);
 if(a->info_page==3){const Calculation *c=&calculations[a->calc];unsigned i,j;size_t used=strlen(body);
  for(i=0;i<c->count;i++)for(j=0;j<c->inputs[i].count;j++){
   if(used<1024){
    if(calc_input_id(a->calc,i)==VAR_DBM)snprintf(body+used,1024-used,"%s",tr("\ndBm: logarithmic input, no SI prefix."));
    else snprintf(body+used,1024-used,"\n%s: 1 %s = %.8g SI",c->inputs[i].symbol,c->inputs[i].units[j].unit,c->inputs[i].units[j].scale);
   }
   used=strlen(body);
  }
 }
}
unsigned app_info_lines(const App *a){char body[1024];if(a->info_page==0||a->info_page==6)return 0;info_text(a,body);return wrapped_text(0,0,282,0,0,body,1);}
void app_draw(const App *a){const Calculation *c=&calculations[a->calc];char b[96];unsigned i;clear_screen();
 if(a->screen==S_SPLASH){c21_draw_mono(0,0,C21_SPLASH_W,C21_SPLASH_H,c21_splash_bits,logo_pixel);if(loc_get()!=LANG_EN){const char *s=tr("ENGINEERING SUITE");rect(40,199,240,16,0);text((320-text_width(s))/2,202,s,1);}return;}
 if(a->screen==S_SETTINGS||a->screen==S_LANGUAGE){
  header(a->screen==S_SETTINGS?tr("SETTINGS"):tr("LANGUAGE"));panel(7,53,306,161);
  if(a->screen==S_SETTINGS){line(65,tr("1. LANGUAGE"),1);wrapped_text(13,99,288,3,0,tr("English is the default language."),1);text(13,151,loc_get()==LANG_EN?"English":"Turkce ASCII",1);snprintf(b,sizeof(b),tr("Version: %s"),C21_VERSION);text_fit(13,185,294,b,1);snprintf(b,sizeof(b),tr("Build date: %s"),C21_BUILD_DATE);text_fit(13,201,294,b,1);}
  else{line(65,"1. English",a->language_cursor==0);line(87,"2. Turkce ASCII",a->language_cursor==1);wrapped_text(13,135,288,2,0,tr("Choose language; calculator is preserved."),1);
   if(a->language_saved>=0)wrapped_text(13,166,288,3,0,tr(a->language_saved?"Language saved for next launch.":"Session only: settings unavailable."),1);}
  footer(tr("Enter Select ESC Return"));return;
 }
 if(a->screen==S_MENU){const Menu *m=&menus[a->menu];header(a->menu==0?tr("MAIN MENU"):(a->menu>=17&&a->menu<=21)||(a->menu>=30&&a->menu<=35)||(a->menu>=43&&a->menu<=50)||(a->menu>=58&&a->menu<=66)||a->menu>=70?tr("ENGINEERING HANDBOOK"):tr("CALCULATION SELECTION"));panel(7,51,306,165);for(i=0;i<m->count;i++){snprintf(b,sizeof(b),"%u. %s",i+1,tr(m->entries[i].name));line(57+(int)i*16,b,(int)i==a->selected);}footer(a->menu==0?tr("1-9 Select 0=10 S Settings"):tr("Digits Select ↑↓ Enter S Settings"));return;}
 header(a->screen==S_SYMBOLS?tr("ENGINEERING SYMBOLS"):tr(c->name));
 if(a->screen==S_TARGET){panel(7,52,306,156);text(13,60,tr("CHOOSE CALCULATED OUTPUT"),1);{CalcId options[3];unsigned n=targets(a->calc,options);for(i=0;i<n;i++){snprintf(b,sizeof(b),"%u. %s",i+1,localized_meaning(calc_output_id(options[i])));line(86+(int)i*22,b,i==a->target_cursor);}}footer(tr("Digits/Enter Select ESC Cancel"));return;}
 if(a->screen==S_FORMULA||(a->screen==S_RESULT&&a->input_mode==INPUT_FORMULA&&!rf_has_details(a->calc)&&!lab_details(a->calc))){formula_view(a);return;}
 if(a->screen==S_FORM){
  unsigned visible=c->count>3?3:c->count,first=c->count>6?(a->row<3?0:a->row>=c->count?c->count-3:a->row-2):(c->count>3&&a->row>=3?c->count-3:0);
  for(i=first;i<first+visible;i++)input_box(53+35*(int)(i-first),c->inputs[i].symbol,localized_meaning(calc_input_id(a->calc,i)),a->values[i],c->inputs[i].units[a->units[i]].unit,a->row==i);
  snprintf(b,sizeof(b),tr("Output unit: %s"),c->outputs[a->output_unit].unit);line(58+35*(int)visible,b,a->row==c->count);
  {int y=76+35*(int)visible;
   panel(12,y-3,145,16);panel(163,y-3,145,16);
   if(a->row==c->count+1)rect(13,y-2,143,14,1);
   if(a->row==c->count+2)rect(164,y-2,143,14,1);
   text(20,y,tr("CALCULATE"),a->row!=c->count+1);text(171,y,tr("INFO (H)"),a->row!=c->count+2);
  }
  if(a->error!=OK||a->input_error!=NUM_OK){rect(7,195,306,24,0);input_error_message(a,195);}
  else if(model_notice(a->calc))text_fit(12,208,296,tr(model_notice(a->calc)),1);
  else if(a->mode_fallback)text_fit(12,208,296,tr(lab_list_only(a->calc)?"List mode: model/lookup details in H":"Formula too large: LIST mode"),1);
  if(c->count<=2&&a->error==OK&&a->input_error==NUM_OK&&!a->mode_fallback){int y=96+35*(int)visible;formula_draw(a->calc,12,y,296,215-y);}
  if(c->count>3){rect(7,30,307,15,0);text_fit(7,34,210,c->name?tr(c->name):"",1);snprintf(b,sizeof(b),tr("Fields %u-%u/%u"),first+1,first+visible,c->count);text_fit(222,34,91,b,1);}
  footer(tr("Enter Edit ←→ Units C Calc F Mode O Out H Info"));return;
 }
 if(a->screen==S_EDIT){
  panel(7,53,306,156);input_box(63,c->inputs[a->row].symbol,localized_meaning(calc_input_id(a->calc,a->row)),a->edit,c->inputs[a->row].units[a->units[a->row]].unit,1);
  rect(18+(int)a->edit_cursor*6,92,5,1,1);
  text(13,109,tr("Decimal / sign / E scientific notation"),1);text(13,126,tr("←→ Cursor   Backspace Erase"),1);text(13,143,tr("Clear: Ctrl+DEL (PC Ctrl+Backspace)"),1);input_error_message(a,168);footer(tr("Enter Save H Info ESC Cancel"));return;
 }
 if(a->screen==S_UNITS){unsigned n=a->row<c->count?c->inputs[a->row].count:c->output_count;const Unit *u=a->row<c->count?c->inputs[a->row].units:c->outputs;panel(7,53,306,155);text(13,62,tr("SELECT ENGINEERING UNIT"),1);for(i=0;i<n;i++){snprintf(b,sizeof(b),"%u. %s",i+1,u[i].unit);line(86+(int)i*23,b,i==a->unit_cursor);}footer(tr("Digits/Enter Save H Info ESC Cancel"));return;}
 if(a->screen==S_RESULT){result_panel(a,55,37);panel(7,100,306,100);formula_draw(a->calc,12,106,296,87);if(a->calc==RLC_MAG&&a->result_valid){double si[CALC_MAX_INPUTS],x,z,phi;for(i=0;i<c->count;i++)numeric_parse(a->values[i],c->inputs[i].units[a->units[i]].scale,&si[i]);if(series_rlc(si,&x,&z,&phi)==OK){snprintf(b,sizeof(b),"Z = %.6g %+.6gj Ω",si[0],x);text_fit(13,205,294,b,1);}}rf_detail(a);lab_detail(a);footer(tr(model_notice(a->calc)?model_notice(a->calc):rf_regulatory(a->calc)?"RF: check local limits; not certification":"H/Enter Info ESC Inputs F Mode O Output"));return;}
 if(a->screen==S_INFO){
  static const char *titles[]={"FORMULA","SYMBOLS / SI UNITS","PURPOSE / PHYSICS","MEASUREMENTS / CONVERSIONS","LABORATORY USAGE","ASSUMPTIONS / LIMITS","WORKED EXAMPLE (STATIC)","COMMON MISTAKES"};
  const HandbookEntry *h=&handbook[a->calc];snprintf(b,sizeof(b),"%u/%u %s",a->info_page+1,HANDBOOK_PAGES,tr(titles[a->info_page]));rect(7,52,306,14,1);text_fit(12,55,296,b,0);panel(7,69,306,148);
  if(a->info_page==0)formula_draw(a->calc,12,77,296,128);
  else if(a->info_page==6){
   for(i=0;i<c->count;i++){snprintf(b,sizeof(b),"%s = %.8g %s",c->inputs[i].symbol,h->example_inputs[i],c->inputs[i].units[0].unit);text_fit(c->count>3?13+(int)(i%2)*149:13,76+(int)(c->count>3?i/2:i)*12,c->count>3?142:294,b,1);}
   if(c->count>3){int fy=82+(int)((c->count+1)/2)*12;if(!formula_draw_values(a->calc,h->example_inputs,12,fy,296,180-fy)&&!formula_draw(a->calc,12,fy,296,180-fy))formula_draw_primary(a->calc,12,fy,296,180-fy);}
   else if(!formula_draw_values(a->calc,h->example_inputs,12,114,296,66)){formula_draw(a->calc,12,114,296,66);}
   snprintf(b,sizeof(b),tr("Result: %.10g %s"),h->example_result,c->outputs[0].unit);text_fit(13,187,294,b,1);text_fit(13,202,294,tr(c->count>3?"Inputs above; ideal-model example.":"Rounded SI substitution; static example."),1);
  }else{char body[1024];unsigned total;info_text(a,body);total=wrapped_text(13,77,282,11,a->info_scroll,body,1);if(total>11){int thumb=130*11/(int)total;frame(303,76,5,133,1);rect(304,77+(int)a->info_scroll*(130-thumb)/(int)(total-11),3,thumb,1);}}
  footer(tr("←→/1-8 Pages ↑↓ Scroll Enter/ESC Return"));return;
 }
 if(a->screen==S_SYMBOLS){
  panel(1,50,318,168);
  text(5,54,"Ω μ π λ Δ Σ ε φ θ α β η τ ω ρ γ",1);
  text(5,69,"√ ± × ÷ ° ² ³ ⁻ ← → ↑ ↓  & < >",1);
  text(5,84,"₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉",1);
  formula_draw(LC_FREQ,5,103,150,38);formula_draw(WAVELENGTH,165,103,150,38);
  formula_draw(XL,5,146,150,40);formula_draw(XC,165,146,150,40);
  formula_draw(FORMULA_Z,5,198,125,18);formula_draw(POWER,135,198,70,18);formula_draw(FORMULA_DELTA_V,215,198,100,18);
  footer(tr("ESC Back"));
 }
}
