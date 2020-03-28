#include "App.h"


//const u8 LvTab[5]={100,95,90,80,70};

//����PWM��� 0-100
void SetPwm(u16 R,u16 G,u16 B){
  if(R > FullVal)R = FullVal;
  if(G > FullVal)G = FullVal;
  if(B > FullVal)B = FullVal;
  if(BatPct == 0){
    TIM3->CCR4 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCR1 = 0;
    return;
  }
  TIM3->CCR4 = R;//*LvTab[BatPct-1]/100;
  TIM3->CCR2 = G;//*LvTab[BatPct-1]/100;
  TIM3->CCR1 = B;//*LvTab[BatPct-1]/100;
}


//��ȡADCֵ
u8 GetAdcValue(void){
  u8 i;
  u32 buf0;
  static u8 index = 0;
  static u8 InitFlag = 0;
  float buf2;
  
//  buf0 = 0;
//  for(i=0;i<ADC_BUF_Lg;i++){//DMA�����ֵ�˲�1
//    buf0+=DMABUF[i];
//  }
  AdcTable[index] = ReadAdc();
  index++;
  if(index>=ADC_Lg){
    index = 0;
    buf0 = 0;
    for(i=0;i<ADC_Lg;i++){  //�����ֵ�˲�
      buf0 += AdcTable[i];
    }
    buf2 = (float)buf0*3.3f/ADC_Lg/4096;
    
    if(InitFlag){
      AdcFilter = AdcFilter*0.99+buf2*0.01;  //һ���˲�
    }
    else{
      InitFlag = 1;     //�״�ֱ�Ӹ�ֵ
      AdcFilter = buf2;
    }
    return 1;
  }
  return 0;
}


void TimeBaseTask(void){
  static u16 Shutdown = 0;
  Time.Cnt1ms++;
  if(60000 == Time.Cnt1ms)
    Time.Cnt1ms = 0;
  if(0 == Time.Cnt1ms%10){
    Time.Cnt10ms++;
    if(60000 == Time.Cnt10ms)
      Time.Cnt10ms = 0;
    if(0 == Time.Cnt10ms%10){
      Time.Cnt100ms++;
      if(60000 == Time.Cnt100ms)
        Time.Cnt100ms = 0;
      if(0 == (Time.Cnt100ms%10)){
        Time.Cnt1s++;
        if(60000 == Time.Cnt1s)
          Time.Cnt1s = 0;
        if(0 == Time.Cnt1s%60)
          Time.Cnt1min ++;
      }
    }
  }
  if(KEY0){
    Shutdown++;
    if(Shutdown >= 12000){//12��ǿ�ƹػ�
      StateLed(1,1);
      while(KEY0);
      PWRIO = 1;
      NVIC_SystemReset();
      while(1);
    }
  }
  else Shutdown = 0;
}

void GetFlag(void){
  static Time_def TimeBuf;
  if(TimeBuf.Cnt1ms != Time.Cnt1ms){
    Time.Flag1ms = 1;
    TimeBuf.Cnt1ms = Time.Cnt1ms;
  }
  if(TimeBuf.Cnt10ms != Time.Cnt10ms){
    Time.Flag10ms = 1;
    TimeBuf.Cnt10ms = Time.Cnt10ms;
  }
  if(TimeBuf.Cnt100ms != Time.Cnt100ms){
    Time.Flag100ms = 1;
    TimeBuf.Cnt100ms = Time.Cnt100ms;
  }
  if(TimeBuf.Cnt1s != Time.Cnt1s){
    Time.Flag1s = 1;
    TimeBuf.Cnt1s = Time.Cnt1s;
  }
  if(TimeBuf.Cnt1min != Time.Cnt1min){
    Time.Flag1min = 1;
    TimeBuf.Cnt1min = Time.Cnt1min;
  }
}

void ClearFlag(void){
  Time.Flag1ms = 0;
  Time.Flag10ms = 0;
  Time.Flag100ms = 0;
  Time.Flag1s = 0;
  Time.Flag1min = 0;
  IWDG_ReloadCounter();
}
  
//��ѹֵת��Ϊ�����ٷֱ�0-100
//AdcValue������ADCֵ  AdcRef��2.5V��׼
u8 GetPower(void){
  if(AdcFilter < 1.5)
    return 0;
  else if(AdcFilter < 1.8)
    return 1;
  else if(AdcFilter < 1.85)
    return 2;
  else if(AdcFilter < 1.9)
    return 3;
  else if(AdcFilter < 1.95)
    return 4;
  else
    return 5;
}

//��ȡʱ���
u16 GetDtTime(u16 TimeBuf,u16 TimeBase){
  if(TimeBuf > TimeBase)
    return TimeBase + 60000 - TimeBuf;
  else
    return TimeBase - TimeBuf;
}

//������ȡ
void BatTask(void){
  if(Time.Flag1ms == 0)return; //ADC�˲���1ms����һ��
  
  if(GetAdcValue())//ADCֵת����100ms����һ��
    BatPct = GetPower();//3103
}

//���ػ���Ӧ
void PowerTask(void){
  if((Key[0].Flag.bit.KeyOut == 2)||(Key[0].Flag.bit.KeyOut == 4))
    Shutdown = 1;//�����ػ�
  if(Shutdown){
    SetPwm(0,0,0);      //�ر����
    PullLED(400,1,0);
    PWRIO = 1;       //��Դ��
    NVIC_SystemReset();
    while(1);
  }
}


u8 GetAList(u8 num){
  OutputIndex = OutputFrame = 0;
  if(num == 0){
    if(UserFrame[0].Mode == 1 || UserFrame[0].Mode == 2){
      NowAct = UserFrame;
      MaxIndex = 10;
      return 0;
    }
  }
  else if(num < 11){
    MaxIndex = FrameLength[num-1];
    switch(num){
    case 1:NowAct = (Frame_REG *)FactFrame0;break;
    case 2:NowAct = (Frame_REG *)FactFrame1;break;
    case 3:NowAct = (Frame_REG *)FactFrame2;break;
    case 4:NowAct = (Frame_REG *)FactFrame3;break;
    case 5:NowAct = (Frame_REG *)FactFrame4;break;
    case 6:NowAct = (Frame_REG *)FactFrame5;break;
    case 7:NowAct = (Frame_REG *)FactFrame6;break;
    case 8:NowAct = (Frame_REG *)FactFrame7;break;
    case 9:NowAct = (Frame_REG *)FactFrame8;break;
    case 10:NowAct = (Frame_REG *)FactFrame9;break;
    }
    return 0;
  }
  return 1;
}

//ȫ�ֱ�����ʼ��
void ValueInit(void){
  OutputState = 0;      //���
  Shutdown = 0;         //����ػ����
  PowerMode = 0;
  NowList = 0;
  while(GetAList(NowList))NowList++;
  NowPoint = 0;
  pFrame = &UserFrame[0];
}

void ValueInitSigl(void){
  NowAct = &SiglFrame[0];
  OutputIndex = OutputFrame = 0;
  OutputState = 3;    //���仯
}

//ms�����ʱ
void delay_ms(u16 n){
  u16 i;
  while(n--){
    i = 855;
    while(i--);
  }
}

  u8 i,buf1,buf2,buf;
void PowerOn(void){
  PWRIO = 1;
  ValueInit();           //ֵ��ʼ��
  
  buf1 = 0;
  buf2 = 0;
  while(1){
    GetFlag();
    KeyTsak();
    
    if(Key[0].Flag.bit.KeyOut && (Key[0].Flag.bit.KeyOut < 3)){
      if(Key[0].HoldTime>120)
        buf1 = ((Key[0].HoldTime-120)/120)%3+1;
    }
    else if(Time.Cnt1s)
      break;
    
    if(buf2 != buf1){
      PWRIO = 0;
      buf2 = buf1;
      FlashLED(buf1+1,1,0);
    }
    
    ClearFlag();
  }
  
  PowerMode = buf1-1;
  
  switch(buf1){
  case 1:
    OutputState = 1;      //�������
    break;
    
  case 2:
    ValueInitSigl();
    break;
    
  case 3:
    break;
    
  default:
    NVIC_SystemReset();
    while(1);
  }
  Key[0].Flag.all = 0;
}

////�ı�ĳ��ֵ
//void ChangeVal(AKeyState_REG *Key,u8 mode){
////  static u8 i = 0;
////  if(Key->Flag.bit.KeyOut == 3){
////    *Value
////  }
//}
#if (HardVer == 100)

void OscInit(void){
  static u8 flag = 0;
  if(flag == 0){        //��ʼ��ͨ������
    SWDInit_Master();
    flag = 1;
  }
}


void OscLink(void){
  static u8 SlaveFlag = 0;
  if(SlaveFlag == 0){   //���������ӻ�
  }
  else{ //�ӻ�����������
  }
}

void LinkMode(void){
  OscInit();
  OscLink();
}

#elif (HardVer == 101)

//void LinkMode(void){
//}
#endif

void ClearKeySta(void){
  if(Key[1].Flag.bit.KeyOut > 2)Key[1].Flag.bit.KeyOut = 0;
  if(Key[2].Flag.bit.KeyOut > 2)Key[2].Flag.bit.KeyOut = 0;
  if(Key[3].Flag.bit.KeyOut > 2)Key[3].Flag.bit.KeyOut = 0;
}

void CheckPress2(u8 flag,u8* pData1,u8* pData2,u8 Max1,u8 Max2){
  static u8 Press = 0;
  static u16 PressTime = 0;    //˫���ж�
  
  if(flag){
    Press++;
    PressTime = Time.Cnt100ms;
    return;
  }
  
  if(Press){
    if(GetDtTime(PressTime,Time.Cnt100ms) > 3){
      if(Press == 1){  //����
        (*pData1)++;
        if((*pData1) > Max1)(*pData1) = 0;
      }
      else if(Press == 2){     //˫��
        (*pData2)++;
        if((*pData2) > Max2)(*pData2) = 0;
      }
      Press = 0;
    }
  }
  
}

void NormMode(void){
  
  if(Time.Flag1s && ((Time.Cnt1s%3)==0)){
    if(BatPct)
      FlashLED(2,0,1);
    else
      FlashLED(3,1,0);
  }//Flash 2Times
  
  //����
  if(Key[2].Flag.bit.KeyOut == 2){//У׼ʱ��
    OutputIndex = OutputFrame = 0;
  }
  else if(Key[2].Flag.bit.KeyOut == 3){
    CheckPress2(1,&Gain,&Speed,4,4);
  }
  CheckPress2(0,&Gain,&Speed,4,4);
  
  
  //������
  if(Key[1].Flag.bit.KeyOut == 3){//UP short press
    do{NowList++;
    }while(GetAList(NowList));
  }
  
  if(Key[3].Flag.bit.KeyOut == 3){//Down short press
    do{NowList--;
    }while(GetAList(NowList));
  }
  
}



void GetHSB(void){
  u16 temp;
  u16 Rtemp;
  u16 Gtemp;
  u16 Btemp;
  
  temp = ColorPoint.bit.Hue;
  if (temp < 12) {//1,+.0
    Btemp = 450;
    Gtemp = temp*37;
    Rtemp = 0;
  }
  else if (temp < 24) {//-,1,0
    Btemp = (24-temp)*37;
    Gtemp = 450;
    Rtemp = 0;
  }
  else if (temp < 36) {//0,1,+
    Btemp = 0;
    Gtemp = 450;
    Rtemp = (temp-24)*37;
  }
  else if (temp < 48) {//0,-,1
    Btemp = 0;
    Gtemp = (48-temp)*37;
    Rtemp = 450;
  }
  else if (temp < 60) {//+,0,1
    Btemp = (temp-48)*37;
    Gtemp = 0;
    Rtemp = 450;
  }
  else {//1,0,-
    Btemp = 450;
    Gtemp = 0;
    Rtemp = (72-temp)*37;
  }
  
  temp = (31-ColorPoint.bit.Sat)%32;
  
  Rtemp += temp*450/32;
  Gtemp += temp*450/32;
  Btemp += temp*450/32;
  
  if(Rtemp>450)Rtemp=450;
  if(Gtemp>450)Gtemp=450;
  if(Btemp>450)Btemp=450;
  
  temp = ColorPoint.bit.Brt%32;
  
  Rtemp = Rtemp*temp/31;
  Gtemp = Gtemp*temp/31;
  Btemp = Btemp*temp/31;
  
  SiglFrame[0].Color.R = Rtemp;
  SiglFrame[0].Color.G = Gtemp;
  SiglFrame[0].Color.B = Btemp;
  
  SiglFrame[1].Color.R = Rtemp/20;
  SiglFrame[1].Color.G = Gtemp/20;
  SiglFrame[1].Color.B = Btemp/20;
}

void SigMode(void){
  static u8 Sta = 0;//��ǰ������ 0~2
  static u8 Key1Flg = 0;
  static u8 Key2Flg = 0;
  static u8 Key3Flg = 0;
  
  u8 Max,Step,Data,Cycl;
  
  if(Time.Flag1s && ((Time.Cnt1s%3)==0)){
    if(BatPct){
      FlashLED(1,0,1);
      delay_ms(150);
      FlashLED(Sta+1,1,0);
    }
    else
      FlashLED(3,1,0);
  }//Flash 2Times
  
  
  //����
  if(Key[2].Flag.bit.KeyOut == 2){
    if(Key2Flg==0)ColorPoint.bit.PlayMode++;
    if(ColorPoint.bit.PlayMode>2)ColorPoint.bit.PlayMode = 0;
    Key2Flg = 1;
    SaveTask(1);
  }
  else{
    Key2Flg = 0;
    if(Key[2].Flag.bit.KeyOut == 3){
      CheckPress2(1,&Sta,&Speed,2,4);
      SaveTask(1);
    }
  }
  CheckPress2(0,&Sta,&Speed,2,4);
  
  //������������ֵ
  switch(Sta){
  case 0:
    Data = ColorPoint.bit.Hue;
    Max = 71;
    Step = 1;
    Cycl = 1;
    break;
  case 1:
    Data = ColorPoint.bit.Sat;
    Max = 31;
    Step = 1;
    Cycl = 0;
    break;
  case 2:
    Data = ColorPoint.bit.Brt;
    Max = 31;
    Step = 1;
    Cycl = 0;
    break;
  }
  
  //������
  if(Key[1].Flag.bit.KeyOut == 1){
    if(Key1Flg == 0){
      if(Data < Max)Data += Step;
      else if(Cycl)Data = 0;
      SaveTask(1);
    }
    Key1Flg = 1;
  }
  else if(Key[1].Flag.bit.KeyOut == 2){
    if(Time.Flag100ms){
      if(Data < Max)Data += Step;
      else if(Cycl)Data = 0;
    }
  }
  else Key1Flg = 0;
  
  if(Key[3].Flag.bit.KeyOut == 1){
    if(Key3Flg == 0){
      if(Data)Data -= Step;
      else if(Cycl)Data = Max;
      SaveTask(1);
    }
    Key3Flg = 1;
  }
  else if(Key[3].Flag.bit.KeyOut == 2){
    if(Time.Flag100ms){
      if(Data)Data -= Step;
      else if(Cycl)Data = Max;
    }
  }
  else Key3Flg = 0;
  
  //��������ֵ
  switch(Sta){
  case 0:ColorPoint.bit.Hue = Data;break;
  case 1:ColorPoint.bit.Sat = Data;break;
  case 2:ColorPoint.bit.Brt = Data;break;
  }
  
  //��ȡ��ɫ
  GetHSB();
}

void MainApp(void){
  switch(PowerMode){
    
//  case 1:       //ʱ��ͬ��ģʽ
//    LinkMode();
//    break;
  case 1:       //��ɫģʽ
    SigMode();
    SaveTask(0);
    break;
    
  case 2:       //��ɫ����ģʽ
    SciMode();
    break;
    
  default:      //����ģʽ
    NormMode();
  }
  ClearKeySta();
}


void GetID(uint32_t *id){
  id[0]=*(uint32_t*)(0x1FFFF7AC);
  id[1]=*(uint32_t*)(0x1FFFF7AC+4);
  id[2]=*(uint32_t*)(0x1FFFF7AC+8); 
}

