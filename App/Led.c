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


void StateLed(u8 R,u8 B){
  if(R)
    LED1IO = 1;
  else
    LED1IO = 0;
  
  if(B)
    LED2IO = 1;
  else
    LED2IO = 0;
}


void FlashLED(u8 Time,u8 R,u8 B){
  u8 i;
  for(i=0;i<Time-1;i++){
    StateLed(R,B);
    delay_ms(15);
    StateLed(0,0);
    delay_ms(150);
  }
  StateLed(R,B);
  delay_ms(15);
  StateLed(0,0);
}

void PullLED(u16 Time,u8 R,u8 B){
  StateLed(R,B);
  delay_ms(Time);
  StateLed(0,0);
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
    if(NowAct[OutputIndex+1].Mode == 0)//结尾，返回首帧
      OutputIndex = 0;
    else
      OutputIndex ++;
    if(OutputIndex >= MaxIndex)OutputIndex = 0;
  }
  
  Color1 = NowAct[OutputIndex].Color;
  if(NowAct[OutputIndex+1].Mode)
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



