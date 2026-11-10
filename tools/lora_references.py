"""Independent 70-digit Decimal/rational LoRa references; no C engine calls."""
from decimal import Decimal as D,getcontext,ROUND_CEILING,ROUND_FLOOR
getcontext().prec=70
def symbols(sf,cr,pl,ih,crc,de):
 a=(8*pl-4*sf+28+16*crc-20*ih)/(4*(sf-2*de));return 8+max(a.to_integral_value(rounding=ROUND_CEILING)*(cr+4),0)
def reference(k,values):
 x=[D(str(v)) for v in values];a=x[0];b=x[1] if len(x)>1 else D(0)
 if k=='LORA_SYMBOL_T':return D(2)**a/b
 if k=='LORA_SYMBOL_RATE':return b/D(2)**a
 if k=='LORA_BITRATE':return a*b/D(2)**a*4/(4+x[2])
 if k=='LORA_CODE_RATE':return 4/(4+a)
 if k=='LORA_CODE_INDEX':return (4/a-4).to_integral_value()
 if k=='LORA_PREAMBLE_T':return (x[2]+D('4.25'))*D(2)**a/b
 if k=='LORA_PAYLOAD_SYMBOLS':return symbols(*x)
 if k in ('LORA_AIRTIME','LORA_PAYLOAD_T'):
  de=x[7] if k=='LORA_AIRTIME' else x[6];n=symbols(a,x[2],x[3],x[4],x[5],de);return (n+(x[6]+D('4.25') if k=='LORA_AIRTIME' else 0))*D(2)**a/b
 if k=='LORA_DUTY':return a/b
 if k=='LORA_PACKET_COUNT':return (b*x[2]/a).to_integral_value(rounding=ROUND_FLOOR)
 if k=='LORA_TX_ENERGY':return a*b
 raise ValueError(k)
