#include "Vardefine.h"

u16 TimeBase = 0;

//u16 DMABUF[ADC_BUF_Lg];       //ADC L1����
u16 AdcTable[ADC_Lg];               //ADC�˲�ֵ
float AdcFilter;


Time_def Time;  //ʱ���

u8 LED1,LED2;       //LED��ʾֵ

u8 OutputState = 0;     //���״̬
u8 OutputIndex = 0;     //���֡
u8 MaxIndex = 0;        //֡��
u16 OutputFrame = 0;    //��ǰ֡ʱ��
u8 Speed = 0;

u8 PowerMode;
u8 DispPage;            //��ʾҳ
u8 BatPct = 0;          //����

u8 Shutdown;            //�ػ����

//Color_REG ColorTable[MaxLength];

AKeyState_REG Key[4];

/*---------------------------------*/
u8 NowList;
Frame_REG* pFrame;
u8 NowPoint;

Frame_REG UserFrame[10];  //�û����õ�
Frame_REG SiglFrame[2];

const u8 FrameLength[10] = {3,3,3,3,3,3,7,4,4,50};

const Frame_REG FactFrame0[3]={
  {{0,0,FullVal},2,4000},{{0,0,FullVal/20},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame1[3]={
  {{0,FullVal,0},2,4000},{{0,FullVal/20,0},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame2[3]={
  {{FullVal,0,0},2,4000},{{FullVal/20,0,0},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame3[3]={
  {{0,FullVal,FullVal},2,4000},{{0,FullVal/20,FullVal/20},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame4[3]={
  {{FullVal,FullVal,0},2,4000},{{FullVal/20,FullVal/20,0},2,4000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame5[3]={
  {{FullVal,0,FullVal},2,4000},{{FullVal/20,0,FullVal/20},2,4000},
  {{0,0,0},0,0},//End
};

const Frame_REG FactFrame6[7]={
  {{FullVal,0,0},2,4000},{{FullVal/2,0,0},2,4000},
  {{0,FullVal,0},2,4000},{{0,FullVal/2,0},2,4000},
  {{0,0,FullVal},2,4000},{{0,0,FullVal/2},2,4000},
  {{0,0,0},0,0},//End
};

const Frame_REG FactFrame7[4]={
  {{FullVal,0,0},2,8000},
  {{0,FullVal,0},2,8000},
  {{0,0,FullVal},2,8000},
  {{0,0,0},0,0},//End
};
const Frame_REG FactFrame8[4]={
  {{0,0,FullVal},2,8000},
  {{0,FullVal,0},2,8000},
  {{FullVal,0,0},2,8000},
  {{0,0,0},0,0},//End
};

const Frame_REG FactFrame9[50]={
  {{0,0,0},2,1000},{{FullVal,0,0},2,1000},
  {{0,0,0},2,1000},{{0,FullVal,0},2,1000},
  {{0,0,0},2,1000},{{0,0,FullVal},2,1000},{{0,0,0},1,1},
  
  {{FullVal,0,0},2,1000},{{FullVal,FullVal,0},2,1000},{{0,FullVal,FullVal},2,1000},
  {{FullVal,0,FullVal},2,1000},{{FullVal,FullVal,0},2,1000},{{0,FullVal,FullVal},2,1000},
  {{FullVal,0,FullVal},2,1000},{{FullVal,FullVal,0},2,1000},{{0,FullVal,FullVal},2,1000},
  {{FullVal,FullVal,FullVal},1,1},
  
  {{FullVal,0,0},1,200},{{0,0,0},1,200},
  {{0,FullVal,0},1,200},{{0,0,0},1,200},
  {{0,0,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,0,0},1,200},{{0,0,0},1,200},
  {{0,FullVal,0},1,200},{{0,0,0},1,200},
  {{0,0,FullVal},1,200},{{0,0,0},1,200},
  
  {{FullVal,FullVal,0},1,200},{{0,0,0},1,200},
  {{0,FullVal,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,0,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,FullVal,0},1,200},{{0,0,0},1,200},
  {{0,FullVal,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,0,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,FullVal,0},1,200},{{0,0,0},1,200},
  {{0,FullVal,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,0,FullVal},1,200},{{0,0,0},1,200},
  {{FullVal,FullVal,FullVal},1,200},{{0,0,0},1,200},
  {{0,0,0},0,0},//End
};

Frame_REG *NowAct;

/*---------------------------------*/

union ColorPoint_UNI ColorPoint;
