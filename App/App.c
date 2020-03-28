#include "App.h"


//const u8 LvTab[5]={100,95,90,80,70};

//设置PWM输出 0-100
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


//获取ADC值
u8 GetAdcValue(void){
  u8 i;
  u32 buf0;
  static u8 index = 0;
  static u8 InitFlag = 0;
  float buf2;
  
//  buf0 = 0;
//  for(i=0;i<ADC_BUF_Lg;i++){//DMA缓存均值滤波1
//    buf0+=DMABUF[i];
//  }
  AdcTable[index] = ReadAdc();
  index++;
  if(index>=ADC_Lg){
    index = 0;
    buf0 = 0;
    for(i=0;i<ADC_Lg;i++){  //缓存均值滤波
      buf0 += AdcTable[i];
    }
    buf2 = (float)buf0*3.3f/ADC_Lg/4096;
    
    if(InitFlag){
      AdcFilter = AdcFilter*0.99+buf2*0.01;  //一阶滤波
    }
    else{
      InitFlag = 1;     //首次直接赋值
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
    if(Shutdown >= 12000){//12秒强制关机
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
  
//电压值转换为电量百分比0-100
//AdcValue：输入ADC值  AdcRef：2.5V基准
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

//获取时间差
u16 GetDtTime(u16 TimeBuf,u16 TimeBase){
  if(TimeBuf > TimeBase)
    return TimeBase + 60000 - TimeBuf;
  else
    return TimeBase - TimeBuf;
}

//电量获取
void BatTask(void){
  if(Time.Flag1ms == 0)return; //ADC滤波，1ms进入一次
  
  if(GetAdcValue())//ADC值转换，100ms进入一次
    BatPct = GetPower();//3103
}

//开关机响应
void PowerTask(void){
  if((Key[0].Flag.bit.KeyOut == 2)||(Key[0].Flag.bit.KeyOut == 4))
    Shutdown = 1;//长按关机
  if(Shutdown){
    SetPwm(0,0,0);      //关闭输出
    PullLED(400,1,0);
    PWRIO = 1;       //电源关
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

//全局变量初始化
void ValueInit(void){
  OutputState = 0;      //输出
  DispPage = 0;         //显示电量
  Shutdown = 0;         //清除关机标记
  PowerMode = 0;
  Speed = 0;
  NowList = sizeof(UserFrame);
  while(GetAList(NowList))NowList++;
  NowPoint = 0;
  pFrame = &UserFrame[0];
}

//ms软件延时
void delay_ms(u16 n){
  u16 i;
  while(n--){
    i = 855;
    while(i--);
  }
}

void PowerOn(void){
  u8 i,buf;
  PWRIO = 1;
  ValueInit();           //值初始化
  
  i = 0;
  while(1){     //按住1s
    if(KEY0)i++;
    if(i>=50)break;
    delay_ms(20);
  }
  PWRIO = 0;            //电源开
L1:
  FlashLED(2,1,0);
  
  buf = i = 0;
  while(KEY0){     //继续按住
    i++;
    if(i>=100){
      buf = 1;
      break;
    }
    IWDG_ReloadCounter();
    delay_ms(20);
  }
  if(buf == 0)return;//正常开机
  FlashLED(3,1,0);
  
  buf = i = 0;
  while(KEY0){     //继续按住
    i++;
    if(i>=100){
      buf = 1;
      break;
    }
    IWDG_ReloadCounter();
    delay_ms(20);
  }
  if(buf == 0){//模式2 SetColor
    PowerMode = 1;
    return;
  }
  FlashLED(4,1,0);//
  
  buf = i = 0;
  while(KEY0){     //继续按住
    i++;
    if(i>=100){
      buf = 1;
      break;
    }
    IWDG_ReloadCounter();
    delay_ms(20);
  }
  if(buf == 0){//模式3 Link
    PowerMode = 2;
    return;
  }
  goto L1;
}

////改变某个值
//void ChangeVal(AKeyState_REG *Key,u8 mode){
////  static u8 i = 0;
////  if(Key->Flag.bit.KeyOut == 3){
////    *Value
////  }
//}
#if (HardVer == 100)

void OscInit(void){
  static u8 flag = 0;
  if(flag == 0){        //初始化通信引脚
    SWDInit_Master();
    flag = 1;
  }
}


void OscLink(void){
  static u8 SlaveFlag = 0;
  if(SlaveFlag == 0){   //主机，检测从机
  }
  else{ //从机，接收数据
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

void NormMode(void){
  if(Time.Flag1s && ((Time.Cnt1s%3)==0)){
    if(BatPct)
      FlashLED(2,0,1);
    else
      FlashLED(3,1,0);
  }//Flash 2Times
  
  if(Key[1].Flag.bit.KeyOut == 3){//UP short press
    do{NowList++;
    }while(GetAList(NowList));
  }
  
  if(Key[2].Flag.bit.KeyOut == 2){//校准时间
    OutputIndex = OutputFrame = 0;
  }
  else if(Key[2].Flag.bit.KeyOut == 3){
    Speed++;
    if(Speed>4)Speed = 0;
  }
  
  if(Key[3].Flag.bit.KeyOut == 3){//Down short press
    do{NowList--;
    }while(GetAList(NowList));
  }
  
  ClearKeySta();
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
}

void SigMode(void){
  static u8 Sta = 0;//当前设置项 0~2
  static u8 Key1Flg = 0;
  static u8 Key2Flg = 0;
  static u8 Key3Flg = 0;
  static u8 ChangeFlag = 0xFF;
  static union ColorPoint_UNI ColorPointBak;
  static u16 ChangeTime;
  
  u8 Max,Step,Data,Cycl;
  
  if(ChangeFlag > 1){
    ChangeFlag = 0;
    ColorPointBak.all = ColorPoint.all;
  }
  
  if(Time.Flag1s && ((Time.Cnt1s%3)==0)){
    if(BatPct){
      FlashLED(1,0,1);
      delay_ms(150);
      FlashLED(1,1,0);
    }
    else
      FlashLED(3,1,0);
  }//Flash 2Times
  
  if(Key[2].Flag.bit.KeyOut == 2){
    if(Key2Flg==0)ColorPoint.bit.PlayMode++;
    if(ColorPoint.bit.PlayMode>2)ColorPoint.bit.PlayMode = 0;
    Key2Flg = 1;
    ChangeFlag = 1;
    ChangeTime = Time.Cnt1s;
  }
  else{
    Key2Flg = 0;
    if(Key[2].Flag.bit.KeyOut == 3){
      Sta++;
      if(Sta>2)Sta = 0;
    }
  }
  
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

  if(Key[1].Flag.bit.KeyOut == 1){
    if(Key1Flg == 0){
      if(Data < Max)Data += Step;
      else if(Cycl)Data = 0;
      ChangeFlag = 1;
      ChangeTime = Time.Cnt1s;
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
      ChangeFlag = 1;
      ChangeTime = Time.Cnt1s;
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

  switch(Sta){
  case 0:ColorPoint.bit.Hue = Data;break;
  case 1:ColorPoint.bit.Sat = Data;break;
  case 2:ColorPoint.bit.Brt = Data;break;
  }
  
  GetHSB();
  
  if((GetDtTime(ChangeTime,Time.Cnt1s) > 30)&&(ChangeFlag == 1)){//30s to save
    ChangeFlag = 0;
    if(ColorPointBak.all != ColorPoint.all){
      ColorPointBak.all = ColorPoint.all;
      SaveAll();
    }
  }
  
  ClearKeySta();
}

void MainApp(void){
  switch(PowerMode){
    
//  case 1:       //时钟同步模式
//    LinkMode();
//    break;
  case 1:       //单色模式
    OutputState = 3;    //不变化
    SigMode();
    break;
    
  case 2:       //颜色设置模式
    SciMode();
    break;
    
  default:      //正常模式
    OutputState = 1;      //正常输出
    NormMode();
  }
}




