#include "Key.h"


void GetAKey(AKeyState_REG *KeyBuf,u8 IOState);

void KeyTsak(void){
  if(Time.Flag10ms == 0)return;
  GetAKey(&Key[0],KEY0);
  GetAKey(&Key[1],KEY1);
  GetAKey(&Key[2],KEY2);
  GetAKey(&Key[3],KEY3);
}


void GetAKey(AKeyState_REG *KeyBuf,u8 IOState){
  u16 TimeBuf = Time.Cnt10ms;

  KeyBuf->Flag.bit.IOLast = KeyBuf->Flag.bit.IOActu;		//ת���ϴ�״̬
  KeyBuf->Flag.bit.IOActu = (IOState != 0);			//��¼����״̬

  if(KeyBuf->Flag.bit.KeyErr)return;  //��������

  if(KeyBuf->Flag.bit.IOActu){		//�����а���
    if(KeyBuf->Flag.bit.IOLast){	//�ϴΰ���
      if(KeyBuf->Flag.bit.IOFilt == 0)
        KeyBuf->Flag.bit.KeyOut = 1;		//�������ΰ�����Ч
      KeyBuf->Flag.bit.IOFilt = 1;		//�˲�IO
    }
    KeyBuf->HoldTime = GetDtTime(KeyBuf->PressTime,TimeBuf);
    if(KeyBuf->HoldTime >= 40)
      KeyBuf->Flag.bit.KeyOut = 2;		//400ms�����ж�Ϊ�̰�
    if(KeyBuf->HoldTime > 3000){	//30s����������
      KeyBuf->Flag.all = 0;
      KeyBuf->Flag.bit.KeyErr = 1;
    }
  }else{
    KeyBuf->PressTime = TimeBuf;		//��¼δ���µ�ʱ��
    if(KeyBuf->Flag.bit.IOLast){		//�״ε���,�����Ƕ�����
      ;
    }else{
      if(KeyBuf->Flag.bit.KeyOut == 1)		//����δ����ȡ
          KeyBuf->Flag.bit.KeyOut = 3;		//400ms�����ж�Ϊ�̰�
      else if(KeyBuf->Flag.bit.KeyOut == 2)		//����δ����ȡ
          KeyBuf->Flag.bit.KeyOut = 4;
      KeyBuf->Flag.bit.IOFilt = 0;		//�˲�IO
      KeyBuf->HoldTime = 0;	//���ʱ��
    }
  }
}







