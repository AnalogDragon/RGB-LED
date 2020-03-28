#include "main.h"



void main(void){
  InitAll();
  LoadAll();    //加载用户配置
  InitTimer();  //定时器初始化
  PowerOn();    //上电
  
/*main loop*/
  while(1){
    GetFlag();
    BatTask();
    KeyTsak();
    MainApp();
    LEDTask();
    PowerTask();
    ClearFlag();
  }

}



