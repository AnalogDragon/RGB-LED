#include "Led.h"


Color_REG GetNextColor(void);


//刷新中断函数
void DispLedFresh(void){
  Color_REG Color;
  if(OutputState == 1){         //正常输出
    Color = GetNextColor();
    SetPwm(Color.R,Color.G,Color.B);
  }
  else if(OutputState == 2){    //不由中断控制
    ;
  }
  else if(OutputState == 3){ //单色模式   //校准结束绿色灯
//    if(SaveData.ColorPoint.bit.PlayMode == 1){
//    }
//    else if(SaveData.ColorPoint.bit.PlayMode == 2){
//      MaxIndex = 10;
//      OutputIndex = OutputFrame = 0;
//    }
//    else{
//      SetPwm(SiglFrame[0].Color.R,SiglFrame[0].Color.G,SiglFrame[0].Color.B);
//    }
    Color = GetNextColor();
    SetPwm(Color.R,Color.G,Color.B);
  }
  else{ //关闭显示
    OutputIndex = 0;
    OutputFrame = 0;
    SetPwm(0,0,0);
  }
}


void StateLed(u8 temp){
  if(temp&1)//R
    LED1IO = 1;
  else
    LED1IO = 0;
  
  if(temp&2)//B
    LED2IO = 1;
  else
    LED2IO = 0;
}

#define LEDFlash_TableLen 10
u8 LEDFlash_Table[LEDFlash_TableLen] = {0};

void FlashLED(u8 Color,u8 Time){
  if(Time == 0)return;
  for(u8 i=0;i<LEDFlash_TableLen;i++){
    if(LEDFlash_Table[i] == 0){
      LEDFlash_Table[i] = Color;
      if(Time == 1)return;
      Time--;
    }
  }
}

u8 InPutPoint(void){
  if(LEDFlash_Table[0]){
    StateLed(LEDFlash_Table[0]);
    memcpy(&LEDFlash_Table[0],&LEDFlash_Table[1],LEDFlash_TableLen-1);
    LEDFlash_Table[9] = 0;
    return 1;
  }
  return 0;
}

void LEDTask(void){
  static u8 Sta = 0;
  static u16 TimeBuf = 0;
  
  if(Sta == 0){
    if(InPutPoint()){
      Sta = 1;//Enable
      TimeBuf = Time.Cnt1ms;
    }
  }
  else if(Sta == 1){
    if(GetDtTime(TimeBuf,Time.Cnt1ms) > 15){
      StateLed(0);
      Sta = 2;//Disable
      TimeBuf = Time.Cnt10ms;
    }
  }
  else if(Sta == 2){
    if(GetDtTime(TimeBuf,Time.Cnt10ms) > 15){
      if(InPutPoint()){
        Sta = 1;//Enable
        TimeBuf = Time.Cnt1ms;
      }
      else
        Sta = 0;
    }
  }
}

void PullLED(u8 temp,u16 Time){
  StateLed(temp);
  delay_ms(Time);
  StateLed(0);
}

extern u8 OutputIndex;     //输出帧
extern u8 MaxIndex;        //帧数
extern u16 OutputFrame;    //当前帧时间

Color_REG GetNextColor(void){  
  Color_REG Color;
  Color_REG Color1;
  Color_REG Color2;
  
  if(OutputFrame > NowAct[OutputIndex].Time){//时间到，帧+1
    OutputFrame = 0;
    OutputIndex ++;
    if((OutputIndex >= MaxIndex)||(NowAct[OutputIndex].Mode == 0))//结尾，返回首帧
      OutputIndex = 0;
  }
  
  Color1 = NowAct[OutputIndex].Color;
  if(NowAct[OutputIndex+1].Mode && (OutputIndex+1) < MaxIndex)
    Color2 = NowAct[OutputIndex+1].Color;
  else
    Color2 = NowAct[0].Color;
  
  if(NowAct[OutputIndex].Mode == 2){
    Color.R = ((OutputFrame*Color2.R) + ((NowAct[OutputIndex].Time-OutputFrame)*Color1.R))
              /NowAct[OutputIndex].Time;
    Color.G = ((OutputFrame*Color2.G) + ((NowAct[OutputIndex].Time-OutputFrame)*Color1.G))
              /NowAct[OutputIndex].Time;
    Color.B = ((OutputFrame*Color2.B) + ((NowAct[OutputIndex].Time-OutputFrame)*Color1.B))
              /NowAct[OutputIndex].Time;
  }
  else
    Color =  Color1;
  
  OutputFrame += (1+Speed);
  Color.R = Color.R/(Gain+1);
  Color.G = Color.G/(Gain+1);
  Color.B = Color.B/(Gain+1);
  return Color;
}



