#include "App.h"

//设置PWM输出 0-100 -> 1ms-2ms
void SetPwm(u16 R,u16 G,u16 B){
  if(R > FullVal)R = FullVal;
  if(G > FullVal)G = FullVal;
  if(B > FullVal)B = FullVal;
  
  TIM3->CCR1 = B;
  TIM3->CCR2 = G;
  TIM3->CCR4 = R;
}


//获取ADC值
void GetAdcValue(void){
  u8 i;
  u32 buf0;
  static u8 index = 0;
  static u8 InitFlag = 0;
  float buf2;
  
  buf0 = 0;
  for(i=0;i<ADC_BUF_Lg;i++){//DMA缓存均值滤波1
    buf0+=DMABUF[i];
  }
  AdcTable[index] = buf0/ADC_BUF_Lg;
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
  }
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
    if(Shutdown >= 8000){//8秒强制关机
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
}
  
//电压值转换为电量百分比0-100
//AdcValue：输入ADC值  AdcRef：2.5V基准
u8 GetPower(void){
  if(AdcFilter < 1.75)
    return 0;
  return 100;
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
  if(Time.Flag1ms == 0) //ADC滤波，1ms进入一次
    return;
  GetAdcValue();
  
  if(Time.Flag100ms == 0)//ADC值转换，100ms进入一次
    return;
  BatPct = GetPower();//3103
}

//开关机响应
void PowerTask(void){
  if(Shutdown | (BatPct == 0)){
    SetPwm(0,0,0);      //关闭输出  
    FlashLED(2);
    PWRIO = 1;       //电源关
    NVIC_SystemReset();
    while(1);
  }
}


//全局变量初始化
void ValueInit(void){
  OutputState = 0;      //输出
  DispPage = 0;         //显示电量
  Shutdown = 0;         //清除关机标记
  PowerMode = 0;
  if(UserFrame[0].Mode == 1 || UserFrame[0].Mode == 2){
    NowAct = UserFrame;
    MaxIndex = 10;
  }
  else{
    NowAct = (Frame_REG *)FactFrame0;
    MaxIndex = FrameLength[0];
  }
}

//ms软件延时
void delay_ms(Uint16 n){
  Uint16 i;
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
    if(KEY0)
      i++;
    else
      i=0;
    if(i>=50)break;
    delay_ms(20);
  }
  PWRIO = 0;            //电源开
  FlashLED(2);
  
  buf = i = 0;
  while(KEY0){     //继续按住
    i++;
    if(i>=100){
      buf = 1;
      break;
    }
    delay_ms(20);
  }
  if(buf == 0)return;//正常开机
  FlashLED(3);
  
  buf = i = 0;
  while(KEY0){     //继续按住
    i++;
    if(i>=100){
      buf = 1;
      break;
    }
    delay_ms(20);
  }
  if(buf == 0){//模式2 SetColor
    PowerMode = 1;
    return;
  }
  FlashLED(4);//

  PowerMode = 2;  //模式3 SetOsc
}


void MainApp(void){
  switch(PowerMode){
  case 1:       //颜色设置模式
    break;
  case 2:       //时钟同步模式
    break;
  default:      //正常模式
    ;
  }
}




