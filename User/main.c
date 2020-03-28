#include "main.h"



void main(void){
  InitAll();
  LoadAll();
  InitTimer();
  PowerOn();
  BatTask();
  
/*main loop*/
  while(1){
    GetFlag();
    BatTask();
    KeyTsak();
    MainApp();
    ClearFlag();
  }
//  while(1){
//    PwmTask();
//    DispTask();
//    PowerTask();
//  }
}




