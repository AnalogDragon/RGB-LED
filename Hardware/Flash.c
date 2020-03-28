#include "Flash.h"


#define FLASH_START_ADDR        ((uint32_t)0x08003C00)   /* Start @ of user Flash area */
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */

u8 table[200];

void SaveAll(void){
  u8 i;
  u8 *p = (u8*)UserFrame;
  for(i=0;i<sizeof(UserFrame);i++)
    table[i] = p[i];
  table[i] = (TimeBase>>8)&0xFF;
  table[i+1] = TimeBase&0xFF;
  FlashWrite((Uint16*)table,0,sizeof(table)/2);
}

void LoadAll(void){
  u8 i;
  u8 *p = (u8*)UserFrame;
  FlashRead((Uint16*)table,0,sizeof(table)/2);
  TimeBase = table[sizeof(UserFrame)+1]|(table[sizeof(UserFrame)]<<8);
  if((TimeBase>8400) || (TimeBase<7600)){
    TimeBase = 8000;
    SaveAll();
    return;
  }
  for(i=0;i<sizeof(UserFrame);i++)
    p[i] = table[i];
}


/*

*Data:  数组
Addr:   地址
Length: 长度

*/
void FlashWrite(Uint16* Data,Uint8 Addr,u16 Length){
  u16 i;
  u32 Address;
  Address = (u32)Addr;
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  FLASH_ErasePage(FLASH_START_ADDR+(Address*FLASH_PAGE_SIZE));
  for(i=0;i<Length;i++){
      FLASH_ProgramHalfWord(FLASH_START_ADDR+(Address*FLASH_PAGE_SIZE)+i*2,*Data);
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
void FlashRead(Uint16* Data,Uint8 Addr,u16 Length){
    Uint32 Address;
    Uint32 Buf;
    u16 i;
    Address = (Uint32)Addr;
    for(i=0;i<Length/2;i++){
        Buf = (*((Uint32 *)(FLASH_START_ADDR + (Address*FLASH_PAGE_SIZE) + i*4)));
        *Data = Buf&0xFFFF;
        Data++;
        *Data = (Buf>>16)&0xFFFF;
        Data++;
    }
}

