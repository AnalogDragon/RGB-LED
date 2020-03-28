#ifndef SCI_H_
#define SCI_H_

#include "head.h"

#define MaxUartLen 40

typedef struct{
  
  u8 SendBuf[MaxUartLen];
  u8 SendLen;
  u8 SendPoint;
  
  u8 RecBuf[MaxUartLen];
  u8 RecLen;
  u8 RecPoint;
  u16 RecTime;
  
}UART_DATA_Def;

extern UART_DATA_Def UART_DATA;
extern UART_DATA_Def UART_DATA;



void UartIntTask(void);
void SciMode(void);




#endif
