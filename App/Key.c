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

  KeyBuf->Flag.bit.IOLast = KeyBuf->Flag.bit.IOActu;		//转移上次状态
  KeyBuf->Flag.bit.IOActu = (IOState != 0);			//记录本次状态

  if(KeyBuf->Flag.bit.KeyErr)return;  //按键报错

  if(KeyBuf->Flag.bit.IOActu){		//本次有按下
    if(KeyBuf->Flag.bit.IOLast){	//上次按下
      if(KeyBuf->Flag.bit.IOFilt == 0)
        KeyBuf->Flag.bit.KeyOut = 1;		//按键单次按下有效
      KeyBuf->Flag.bit.IOFilt = 1;		//滤波IO
    }
    KeyBuf->HoldTime = GetDtTime(KeyBuf->PressTime,TimeBuf);
    if(KeyBuf->HoldTime >= 40)
      KeyBuf->Flag.bit.KeyOut = 2;		//400ms以内判断为短按
    if(KeyBuf->HoldTime > 3000){	//30s持续，报错
      KeyBuf->Flag.all = 0;
      KeyBuf->Flag.bit.KeyErr = 1;
    }
  }else{
    KeyBuf->PressTime = TimeBuf;		//记录未按下的时间
    if(KeyBuf->Flag.bit.IOLast){		//首次弹起,可能是抖动，
      ;
    }else{
      if(KeyBuf->Flag.bit.KeyOut == 1)		//按键未被读取
          KeyBuf->Flag.bit.KeyOut = 3;		//400ms以内判断为短按
      else if(KeyBuf->Flag.bit.KeyOut == 2)		//按键未被读取
          KeyBuf->Flag.bit.KeyOut = 4;
      KeyBuf->Flag.bit.IOFilt = 0;		//滤波IO
      KeyBuf->HoldTime = 0;	//清空时间
    }
  }
}







