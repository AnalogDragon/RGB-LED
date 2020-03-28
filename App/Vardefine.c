#include "Vardefine.h"

u16 TimeBase = 0;

u16 DMABUF[ADC_BUF_Lg];       //ADC L1缓存
float AdcFilter;
u16 AdcTable[ADC_Lg];               //ADC滤波值


Time_def Time;  //时间基

u8 LED1,LED2;       //LED显示值

u8 OutputState = 0;     //输出状态
u8 OutputIndex = 0;     //输出帧
u8 MaxIndex = 0;        //帧数
u16 OutputFrame = 0;    //当前帧时间

u8 PowerMode;
u8 DispPage;            //显示页
u8 BatPct = 0;          //电量

u8 Shutdown;            //关机标记


//Color_REG ColorTable[MaxLength];

AKeyState_REG Key[4];

/*---------------------------------*/

Frame_REG UserFrame[10];  //用户设置的

const u8 FrameLength[10] = {3,3,3,3,3,3,3,3,3,3};

const Frame_REG FactFrame0[3]={
  {{0,0,450},2,4000},{{0,0,20},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame1[3]={
  {{0,450,0},2,4000},{{0,20,0},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame2[3]={
  {{450,0,0},2,4000},{{20,0,0},2,4000},
  {{0,0,0},0,0},//End
};

const Frame_REG FactFrame3[2];
const Frame_REG FactFrame4[2];
const Frame_REG FactFrame5[2];
const Frame_REG FactFrame6[2];
const Frame_REG FactFrame7[2];
const Frame_REG FactFrame8[2];
const Frame_REG FactFrame9[2];

Frame_REG *NowAct;







