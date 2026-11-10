/* Layout-only SX127x formulas. Full packet expression uses stacked auxiliary definitions. */
 case LORA_SYMBOL_T:left=S(T("T"),0,T("s"));right=F(S(T("2"),V(0,"SF"),0),V(1,"BW"));break;
 case LORA_SYMBOL_RATE:left=S(T("R"),0,T("s"));right=F(V(1,"BW"),S(T("2"),V(0,"SF"),0));break;
 case LORA_BITRATE:left=S(T("R"),0,T("b"));right=F(MUL(MUL(V(0,"SF"),V(1,"BW")),T("4")),MUL(S(T("2"),V(0,"SF"),0),P(ADD(T("4"),V(2,"CR")))));break;
 case LORA_CODE_RATE:left=T("r");right=F(T("4"),ADD(T("4"),V(0,"CR")));break;
 case LORA_CODE_INDEX:left=T("CR");right=SUBTRACT(F(T("4"),V(0,"r")),T("4"));break;
 case LORA_PREAMBLE_T:left=S(T("T"),0,T("pre"));right=MUL(P(ADD(W(2,"N","pre"),T("4.25"))),F(S(T("2"),V(0,"SF"),0),V(1,"BW")));break;
 case LORA_AIRTIME:case LORA_PAYLOAD_T:case LORA_PAYLOAD_SYMBOLS:{
  int cr=id==LORA_PAYLOAD_SYMBOLS?1:2,pl=id==LORA_PAYLOAD_SYMBOLS?2:3,ih=id==LORA_PAYLOAD_SYMBOLS?3:4,crc=id==LORA_PAYLOAD_SYMBOLS?4:5,de=id==LORA_AIRTIME?7:id==LORA_PAYLOAD_T?6:5;
  const MathNode *num=SUBTRACT(ADD(ADD(SUBTRACT(MUL(T("8"),V(pl,"PL")),MUL(T("4"),V(0,"SF"))),T("28")),MUL(T("16"),V(crc,"CRC"))),MUL(T("20"),V(ih,"IH")));
  const MathNode *den=MUL(T("4"),P(SUBTRACT(V(0,"SF"),MUL(T("2"),V(de,"DE")))));
  const MathNode *qe=eq(a,T("q"),F(num,den));
  const MathNode *ceilq=R(T("ceil"),P(T("q")));
  const MathNode *blocks=MUL(ceilq,P(ADD(V(cr,"CR"),T("4"))));
  const MathNode *npe=eq(a,S(T("N"),0,T("pay")),ADD(T("8"),R(T("max"),P(R(R(blocks,T(",")),T("0"))))));
  const MathNode *aux=math_stack(a,npe,qe,2);
  if(id==LORA_PAYLOAD_SYMBOLS)return aux;
  right=S(T("N"),0,T("pay"));
  if(id==LORA_AIRTIME)right=P(ADD(ADD(W(6,"N","pre"),T("4.25")),right));
  right=MUL(right,F(S(T("2"),V(0,"SF"),0),V(1,"BW")));
  left=S(T("T"),0,T(id==LORA_AIRTIME?"air":"pay"));return math_stack(a,eq(a,left,right),aux,2);
 }
 case LORA_DUTY:left=T("D");right=F(W(0,"T","air"),W(1,"T","int"));break;
 case LORA_PACKET_COUNT:left=T("N");right=R(T("floor"),P(F(MUL(W(1,"T","w"),V(2,"D")),W(0,"T","air"))));break;
 case LORA_TX_ENERGY:left=T("E");right=MUL(W(0,"P","draw"),W(1,"T","air"));break;
