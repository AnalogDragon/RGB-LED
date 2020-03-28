#ifndef main_H_
#define main_H_

#include "head.h"


#define LED1IO  PAout(2)
#define LED2IO  PAout(3)

#define PWRIO   PAout(9)
   
#define KEY0IO  PAin(10)
#define KEY1IO  PAin(0)
#define KEY2IO  PAin(4)
#define KEY3IO  PAin(5)

#define KEY0    (KEY0IO == 0)
#define KEY1    (KEY1IO == 0)
#define KEY2    (KEY2IO == 0)
#define KEY3    (KEY3IO == 0)


#endif