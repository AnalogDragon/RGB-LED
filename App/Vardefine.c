#include "Vardefine.h"

u16 TimeBase = 0;

//u16 DMABUF[ADC_BUF_Lg];       //ADC L1缓存
u16 AdcTable[ADC_Lg];               //ADC滤波值
u16 AdcFilter;


Time_def Time;  //时间基

u8 LED1,LED2;       //LED显示值

u8 OutputState = 0;     //输出状态
u8 OutputIndex = 0;     //输出帧
u8 MaxIndex = 0;        //帧数
u16 OutputFrame = 0;    //当前帧时间
u8 Speed = 0;
u8 Gain = 0;

u8 PowerMode;
//u8 DispPage;            //显示页
u8 BatPct = 0;          //电量

u8 Shutdown;            //关机标记

//Color_REG ColorTable[MaxLength];

AKeyState_REG Key[4];

/*---------------------------------*/
u8 NowList;
Frame_REG* pFrame;
u8 NowPoint;

Frame_REG UserFrame[MaxUserLeng];  //用户设置的
Frame_REG SiglFrame[2];

const u8 FrameLength[10] = {2,2,2, 2,2,2, 2,2, 2,3};

const Frame_REG FactFrame0[2]={
  {{0,0,FullVal},2,4000},{{0,0,FullVal/20},2,4000},
};
const Frame_REG FactFrame1[2]={
  {{0,FullVal,0},2,4000},{{0,FullVal/20,0},2,4000},
};
const Frame_REG FactFrame2[2]={
  {{FullVal,0,0},2,4000},{{FullVal/20,0,0},2,4000},
};

const Frame_REG FactFrame3[2]={
  {{0,FullVal,FullVal},2,4000},{{0,FullVal/20,FullVal/20},2,4000},
};
const Frame_REG FactFrame4[2]={
  {{FullVal,FullVal,0},2,4000},{{FullVal/20,FullVal/20,0},2,4000},
};
const Frame_REG FactFrame5[2]={
  {{FullVal,0,FullVal},2,4000},{{FullVal/20,0,FullVal/20},2,4000},
};

const Frame_REG FactFrame6[6]={
  {{FullVal,0,0},2,4000},{{0,FullVal,0},2,4000},
};

const Frame_REG FactFrame7[6]={
  {{0,FullVal,0},2,4000},{{0,0,FullVal},2,4000},
};

const Frame_REG FactFrame8[6]={
  {{0,0,FullVal},2,4000},{{FullVal,0,0},2,4000},
};

const Frame_REG FactFrame9[3]={
  {{FullVal,0,0},2,8000},
  {{0,FullVal,0},2,8000},
  {{0,0,FullVal},2,8000},
};

//const Frame_REG FactFrame7[3]={
//  {{0,0,FullVal},2,8000},
//  {{0,FullVal,0},2,8000},
//  {{FullVal,0,0},2,8000},
//};

Frame_REG *NowAct;

/*---------------------------------*/

union ColorPoint_UNI ColorPoint;
