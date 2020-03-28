#ifndef LED_H_
#define LED_H_

#include "head.h"

#define FullVal 450

void DispLedFresh(void);
void StateLed(u8 temp);
void FlashLED(u8 Color,u8 Time);
void PullLED(u8 temp,u16 Time);



void LEDTask(void);










#endif