#ifndef FLASH_H_
#define FLASH_H_

#include "head.h"


void FlashWrite(Uint16* Data,Uint8 Addr,u16 Length);
void FlashRead(Uint16* Data,Uint8 Addr,u16 Length);

void LoadAll(void);
void SaveAll(void);


#endif 