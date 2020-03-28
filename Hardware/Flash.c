#include "Flash.h"


#define FLASH_START_ADDR        ((uint32_t)0x08003C00)   /* Start @ of user Flash area */
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */

#define Addr1 0x000
#define Addr2 0x200
#define Addr3 0x220


u8 table[FLASH_PAGE_SIZE];//One Page

void SaveAll(void){
  u16 i;
  u8 *p = (u8*)UserFrame;
  for(i=0;i<sizeof(UserFrame);i++)
    table[Addr1+i] = p[i];
  p = (u8*)&ColorPoint;
  for(i=0;i<sizeof(ColorPoint);i++)
    table[Addr2+i] = p[i];
  table[Addr3] = (TimeBase>>8)&0xFF;
  table[Addr3+1] = TimeBase&0xFF;
  FlashWrite((Uint16*)table,sizeof(table)/2);
}

void LoadAll(void){
  u16 i;
  u8 *p = (u8*)UserFrame;
  FlashRead((Uint16*)table,sizeof(table)/2);
  TimeBase = table[Addr3+1]|(table[Addr3]<<8);
  if((TimeBase>8400) || (TimeBase<7600)){
    TimeBase = 8000;
    ColorPoint.all = 0x001F1F47;
    SaveAll();
    return;
  }
  for(i=0;i<sizeof(UserFrame);i++)
    p[i] = table[Addr1+i];
  p = (u8*)&ColorPoint;
  for(i=0;i<sizeof(ColorPoint);i++)
    p[i] = table[Addr2+i];
}

//存储检测
void SaveTask(u8 Flag){
  static u16 ChangeTime;
  static u8 ChangeFlag = 0xFF;
  static union ColorPoint_UNI ColorPointBak;//缓存
  
  if(ChangeFlag > 1){
    ChangeFlag = 0;
    ColorPointBak.all = ColorPoint.all;
  }
  
  if(Flag){
    ChangeFlag = 1;
    ChangeTime = Time.Cnt1s;
    return;
  }
  
  if((GetDtTime(ChangeTime,Time.Cnt1s) > 30)&&(ChangeFlag == 1)){//30s to save
    ChangeFlag = 0;
    if(ColorPoint.all != ColorPointBak.all){    //判断差异性
      ColorPointBak.all = ColorPoint.all;
      SaveAll();
    }
  }
}
/*

*Data:  数组
Addr:   地址
Length: 长度

*/
void FlashWrite(Uint16* Data,u16 Length){
  u16 i;
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  FLASH_ErasePage(FLASH_START_ADDR);
  for(i=0;i<Length;i++){
      FLASH_ProgramHalfWord(FLASH_START_ADDR + i*2,*Data);
      FLASH_WaitForLastOperation(1000);
      Data++;
  }
  FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  FLASH_Lock(); 
}

/*

*Data:  数组
Addr:   地址
Length: 长度

*/
void FlashRead(Uint16* Data,u16 Length){
    Uint32 Buf;
    u16 i;
    for(i=0;i<Length/2;i++){
        Buf = (*((Uint32 *)(FLASH_START_ADDR + i*4)));
        *Data = Buf&0xFFFF;
        Data++;
        *Data = (Buf>>16)&0xFFFF;
        Data++;
    }
}

