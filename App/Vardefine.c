#include "Vardefine.h"

u16 TimeBase = 0;

u16 DMABUF[ADC_BUF_Lg];       //ADC L1����
float AdcFilter;
u16 AdcTable[ADC_Lg];               //ADC�˲�ֵ


Time_def Time;  //ʱ���

u8 LED1,LED2;       //LED��ʾֵ

u8 OutputState = 0;     //���״̬
u8 OutputIndex = 0;     //���֡
u8 MaxIndex = 0;        //֡��
u16 OutputFrame = 0;    //��ǰ֡ʱ��

u8 PowerMode;
u8 DispPage;            //��ʾҳ
u8 BatPct = 0;          //����

u8 Shutdown;            //�ػ����


//Color_REG ColorTable[MaxLength];

AKeyState_REG Key[4];

/*---------------------------------*/

Frame_REG UserFrame[10];  //�û����õ�

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







