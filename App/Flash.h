/*
 * Flash.h
 *
 *  Created on: 2019-1-9
 *      Author: hp
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "head.h"
   
   
void FlashInit(void);
extern Uint16 CsmUnlock(void);
void ErasFlash(u8 page);
void UnlockFlash(void);
void LockFlash(void);
void FlashProg(void);
void FlashCheck(void);
void Flash_Unlock_Check(void);

#endif /* FLASH_H_ */
