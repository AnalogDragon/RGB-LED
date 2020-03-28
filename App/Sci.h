/*
 * sci.h
 *
 *  Created on: 2019-1-9
 *      Author: hp
 */

#ifndef SCI_H_
#define SCI_H_

#include "head.h"




void SCIA_Init(Uint32 Band);
Uint8 GetByte(Uint8 *data);
Uint8 GetPacket(void);
void UnlockData(void);
void LockData(void);
void SendPacket(void);
void CRCCheck(Uint16 *Output,Uint8 Input);
void SendState(Uint8 step,Uint8 state);
void RTS_Output(Uint8 state);
void SendByte(Uint8 data);
void strcopy(Uint8 *str1,Uint8 *str2,Uint16 Length);
void delay(u16 num);


#endif /* SCI_H_ */
