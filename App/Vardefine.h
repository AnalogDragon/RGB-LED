#ifndef VARDEFINE_H_
#define VARDEFINE_H_

#include "head.h"

#define Version 100

#define ADC_BUF_Lg      40
#define ADC_Lg          20

#define MaxLength 500;

extern u16 DMABUF[ADC_BUF_Lg];
extern float AdcFilter;
extern u16 AdcTable[ADC_Lg];


typedef struct{
  u8 Flag1ms;
  u8 Flag10ms;
  u8 Flag100ms;
  u8 Flag1s;
  u8 Flag1min;

  u16 Cnt1ms;
  u16 Cnt10ms;
  u16 Cnt100ms;
  u16 Cnt1s;
  u16 Cnt1min;
}Time_def;

extern Time_def Time;


extern u16 TimeBase;

extern u8 LED1,LED2;       //LED��ʾֵ

extern u8 OutputState;     //���״̬
extern u8 OutputIndex;     //���֡
extern u8 MaxIndex;        //֡��
extern u16 OutputFrame;    //��ǰ֡ʱ��

extern u8 PowerMode;

extern u8 DispPage;            //��ʾҳ
extern u8 BatPct;          //����

extern u8 Shutdown;            //�ػ����

/*---------------------------------*/
typedef struct{
  u32 R:10;
  u32 G:10;
  u32 B:10;
  u32 Z:2;
}Color_REG;

//extern Color_REG ColorTable[MaxLength];

/*---------------------------------*/
typedef struct{
  u16 IOActu:1; //ʵʱ
  u16 IOLast:1; //�ϴ�״̬�������˲�
  u16 IOFilt:1; //�˲����ֵ
  u16 KeyOut:3; //���δ���λ   //1:���¶̰� 2:���³��� 3:�̰����� 4:��������
  u16 KeyErr:1; //������������
} KeyFlag_BITS;

union KeyFlag_REG{
  u16   all;
  KeyFlag_BITS  bit;
};

typedef struct{
  union KeyFlag_REG Flag;
  u16 PressTime;
  u16 HoldTime;
}AKeyState_REG;

extern AKeyState_REG Key[4];

/*---------------------------------*/
typedef struct{
  Color_REG Color;      //��ɫ
  u8    Mode;           //�仯����  0:��Ч�� 1:����  2:����
  u32   Time;           //��ʱ
}Frame_REG;


extern u8 NowList;
extern Frame_REG* pFrame;
extern u8 NowPoint;
extern Frame_REG UserFrame[10];
extern const u8 FrameLength[];
extern const Frame_REG FactFrame0[];
extern const Frame_REG FactFrame1[];
extern const Frame_REG FactFrame2[];
extern const Frame_REG FactFrame3[];
extern const Frame_REG FactFrame4[];
extern const Frame_REG FactFrame5[];
extern const Frame_REG FactFrame6[];
extern const Frame_REG FactFrame7[];
extern const Frame_REG FactFrame8[];
extern const Frame_REG FactFrame9[];

extern Frame_REG *NowAct;


/*---------------------------------*/

#endif /* VARDEFINE_H_ */
