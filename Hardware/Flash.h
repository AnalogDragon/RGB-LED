#ifndef FLASH_H_
#define FLASH_H_

#include "head.h"


void FlashWrite(Uint16* Data,Uint8 Addr,u16 Length);
void FlashRead(Uint16* Data,Uint8 Addr,u16 Length);

void SaveAll(void);
void LoadAll(void);

void SaveTask(u8 Flag);

#endif 