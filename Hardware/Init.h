#ifndef _INIT_H_
#define _INIT_H_

#include "head.h"

#define ADC1_Address    0x40012440





void GPIO_config(void);
void NVIC_Config(void);
void Tim1Init(void);
void Tim14InitPWM(void);
//void Tim3InitCAP(u16 per,u16 psc);
void ADC_Config(void);
void DMA_Config(void);
void SysTickConfig(void);


void InitAll(void);
void InitTimer(void);

void SWDInit_Master(void);
void SWDInit_Slave(void);

u16 ReadAdc(void);


#endif