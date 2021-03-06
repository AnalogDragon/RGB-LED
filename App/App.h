#ifndef APP_H_
#define APP_H_

#include "head.h"


void SetPwm(u16 R,u16 G,u16 B);
u8 GetAdcValue(void);
void TimeBaseTask(void);
void GetFlag(void);
void ClearFlag(void);
u8 GetPower(void);
u16 GetDtTime(u16 TimeBuf,u16 TimeBase);
void BatTask(void);
void PowerTask(void);
void ValueInit(void);
void delay_ms(Uint16 n);
void PowerOn(void);
void MainApp(void);
u8 GetAList(u8 num);

void GetID(uint32_t *id);





#endif /* APP_H_ */
