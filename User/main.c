#include "main.h"



void main(void){
  InitAll();
  LoadAll();    //�����û�����
  InitTimer();  //��ʱ����ʼ��
  PowerOn();    //�ϵ�
  BatTask();
  
/*main loop*/
  while(1){
    GetFlag();
    BatTask();
    KeyTsak();
    MainApp();
    PowerTask();
    ClearFlag();
  }

}



