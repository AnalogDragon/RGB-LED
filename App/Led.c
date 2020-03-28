#include "Led.h"

#define FullVal 450


Color_REG GetNextColor(void);


//ˢ���жϺ���
void DispLedFresh(void){
  Color_REG Color;
  if(OutputState){
    Color = GetNextColor();
    SetPwm(Color.R,Color.G,Color.B);
  }
  else{
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


void FlashLED(u8 Time){
  u8 i;
  for(i=0;i<Time-1;i++){
    StateLed(1,0);
    delay_ms(15);
    StateLed(0,0);
    delay_ms(150);
  }
  StateLed(1,0);
  delay_ms(15);
  StateLed(0,0);
}


extern u8 OutputState;     //���״̬
extern u8 OutputIndex;     //���֡
extern u8 MaxIndex;        //֡��
extern u16 OutputFrame;    //��ǰ֡ʱ��

Color_REG GetNextColor(void){  
  Color_REG Color;
  Color_REG Color1;
  Color_REG Color2;
  
  if(OutputFrame > NowAct[OutputIndex].Time){//ʱ�䵽��֡+1
    OutputFrame = 0;
    if(NowAct[OutputIndex+1].Mode == 0)//��β��������֡
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
  
  OutputFrame++;
  return Color;
}





