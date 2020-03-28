#include "Vardefine.h"

u16 TimeBase = 0;

//u16 DMABUF[ADC_BUF_Lg];       //ADC L1����
u16 AdcTable[ADC_Lg];               //ADC�˲�ֵ
u16 AdcFilter;


Time_def Time;  //ʱ���

u8 LED1,LED2;       //LED��ʾֵ

u8 OutputState = 0;     //���״̬
u8 OutputIndex = 0;     //���֡
u8 MaxIndex = 0;        //֡��
u16 OutputFrame = 0;    //��ǰ֡ʱ��
u8 Speed = 0;
u8 Gain = 0;

u8 PowerMode;
//u8 DispPage;            //��ʾҳ
u8 BatPct = 0;          //����

u8 Shutdown;            //�ػ����

//Color_REG ColorTable[MaxLength];

AKeyState_REG Key[4];

/*---------------------------------*/
u8 NowList;
Frame_REG* pFrame;
u8 NowPoint;

Frame_REG UserFrame[MaxUserLeng];  //�û����õ�
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
