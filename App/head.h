#ifndef HEAD_H_
#define HEAD_H_

#include "stm32f0xx.h"
#include "VarDefine.h"
#include "Init.h"
#include "App.h"
#include "Key.h"
#include "Led.h"
#include "Flash.h"
#include "main.h"
#include "Sci.h"


typedef struct {
    uint16_t bit0 : 1;
    uint16_t bit1 : 1;
    uint16_t bit2 : 1;
    uint16_t bit3 : 1;
    uint16_t bit4 : 1;
    uint16_t bit5 : 1;
    uint16_t bit6 : 1;
    uint16_t bit7 : 1;
    uint16_t bit8 : 1;
    uint16_t bit9 : 1;
    uint16_t bit10 : 1;
    uint16_t bit11 : 1;
    uint16_t bit12 : 1;
    uint16_t bit13 : 1;
    uint16_t bit14 : 1;
    uint16_t bit15 : 1;
} Bits_16_TypeDef;


#define PAout(n) ((Bits_16_TypeDef *)(&(GPIOA->ODR)))->bit##n 
#define PBout(n) ((Bits_16_TypeDef *)(&(GPIOB->ODR)))->bit##n 
#define PCout(n) ((Bits_16_TypeDef *)(&(GPIOC->ODR)))->bit##n 
#define PDout(n) ((Bits_16_TypeDef *)(&(GPIOD->ODR)))->bit##n 
#define PEout(n) ((Bits_16_TypeDef *)(&(GPIOE->ODR)))->bit##n 
#define PFout(n) ((Bits_16_TypeDef *)(&(GPIOF->ODR)))->bit##n 


#define PAin(n) ((Bits_16_TypeDef *)(&(GPIOA->IDR)))->bit##n 
#define PBin(n) ((Bits_16_TypeDef *)(&(GPIOB->IDR)))->bit##n  
#define PCin(n) ((Bits_16_TypeDef *)(&(GPIOC->IDR)))->bit##n  
#define PDin(n) ((Bits_16_TypeDef *)(&(GPIOD->IDR)))->bit##n  
#define PEin(n) ((Bits_16_TypeDef *)(&(GPIOE->IDR)))->bit##n  
#define PFin(n) ((Bits_16_TypeDef *)(&(GPIOF->IDR)))->bit##n 




#endif /* HEAD_H_ */
