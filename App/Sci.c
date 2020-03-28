#include "Sci.h"

UART_DATA_Def UART_DATA = {0};


void UartIntTask(void){
  u8 temp = 0;
  if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){
    temp = USART_ReceiveData(USART1);
    UART_DATA.RecBuf[UART_DATA.RecPoint] = temp;
    UART_DATA.RecPoint++;
    if(UART_DATA.RecPoint >= MaxUartLen){ //�������
      UART_DATA.RecPoint = 0;
    }
    UART_DATA.RecTime = Time.Cnt1ms;
  }
  if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET){
    temp = USART_ReceiveData(USART1);
    USART_ClearFlag(USART1, USART_FLAG_ORE);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) != RESET){
    if(UART_DATA.SendPoint < UART_DATA.SendLen){
      USART_SendData(USART1,UART_DATA.SendBuf[UART_DATA.SendPoint]);
      UART_DATA.SendPoint++;
    }
    else{
      UART_DATA.SendLen = 0;
      UART_DATA.SendPoint = 0;
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }
}

void SendData(u8 Len){
  if(Len > MaxUartLen || Len == 0)
    return;
  
  UART_DATA.SendLen = Len;
  UART_DATA.SendPoint = 0;
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}


u8 GetNum(Frame_REG *Framebuf){
  u8 step = 0;
  u8 flag = 0;
  u32 temp = 0;
  for(u8 i=0;i<UART_DATA.RecLen-2;i++){
    if(UART_DATA.RecBuf[i]>='0' && UART_DATA.RecBuf[i]<='9'){
      temp = temp*10 + (UART_DATA.RecBuf[i]-'0');
      flag = 1;
    }
    else if(UART_DATA.RecBuf[i] == ',' || UART_DATA.RecBuf[i] == ' '){
      switch(step++){
      case 0:if(temp<=450)Framebuf->Color.R=temp;else return 1;break;
      case 1:if(temp<=450)Framebuf->Color.G=temp;else return 1;break;
      case 2:if(temp<=450)Framebuf->Color.B=temp;else return 1;break;
      case 3:if(temp<=2)Framebuf->Mode=temp;else return 1;break;
      case 4:if(temp<60000)Framebuf->Time=temp;else return 1;break;
      default:return 1;
      }
      flag = temp = 0;
    }
    else return 1;//Error
  }
  if(flag && step == 4 && temp<60000){
    Framebuf->Time=temp;
    return 0;
  }
  if(step == 5 && flag == 0)return 0;
  return 1;
}

u8 GetNum2(void){
  u32 temp = 0;
  for(u8 i=0;i<UART_DATA.RecLen-2;i++){
    if(UART_DATA.RecBuf[i]>='0' && UART_DATA.RecBuf[i]<='9'){
      temp = temp*10 + (UART_DATA.RecBuf[i]-'0');
    }
    else return 1;
  }
  if((temp>8400) || (temp<7600))
    return 1;
  
  TimeBase = temp;
  return 0;
}



const u8 Table_Data[23] = {"[000,000,000,0,00000]\r\n"};
const u8 Table_Data2[8] = {"[8000]\r\n"};
void RepeatAll(void){
  for(u8 i=0;i<10;i++){
    if(UserFrame[i].Mode == 0 || UserFrame[i].Time == 0)break;
    while(UART_DATA.SendLen);
    memcpy(UART_DATA.SendBuf,Table_Data,sizeof(Table_Data));
    UART_DATA.SendBuf[1] ='0'+UserFrame[i].Color.R/100%10;
    UART_DATA.SendBuf[2] ='0'+UserFrame[i].Color.R/10%10;
    UART_DATA.SendBuf[3] ='0'+UserFrame[i].Color.R%10;
    UART_DATA.SendBuf[5] ='0'+UserFrame[i].Color.G/100%10;
    UART_DATA.SendBuf[6] ='0'+UserFrame[i].Color.G/10%10;
    UART_DATA.SendBuf[7] ='0'+UserFrame[i].Color.G%10;
    UART_DATA.SendBuf[9] ='0'+UserFrame[i].Color.B/100%10;
    UART_DATA.SendBuf[10]='0'+UserFrame[i].Color.B/10%10;
    UART_DATA.SendBuf[11]='0'+UserFrame[i].Color.B%10;
    UART_DATA.SendBuf[13]='0'+UserFrame[i].Mode%10;
    UART_DATA.SendBuf[15]='0'+UserFrame[i].Time/10000%10;
    UART_DATA.SendBuf[16]='0'+UserFrame[i].Time/1000%10;
    UART_DATA.SendBuf[17]='0'+UserFrame[i].Time/100%10;
    UART_DATA.SendBuf[18]='0'+UserFrame[i].Time/10%10;
    UART_DATA.SendBuf[19]='0'+UserFrame[i].Time%10;
    SendData(sizeof(Table_Data));
  }
  while(UART_DATA.SendLen);
  memcpy(UART_DATA.SendBuf,Table_Data2,sizeof(Table_Data2));
  UART_DATA.SendBuf[1] ='0'+TimeBase/1000%10;
  UART_DATA.SendBuf[2] ='0'+TimeBase/100%10;
  UART_DATA.SendBuf[3] ='0'+TimeBase/10%10;
  UART_DATA.SendBuf[4] ='0'+TimeBase%10;
  SendData(sizeof(Table_Data2));
}

const u8 Table_Hello[9] = {"Hello\r\n\r\n"};
const u8 Table_Hello2[10] = {"OscSet\r\n\r\n"};
const u8 Table_PointAsk0[27] = {"�ؼ�֡0#[R,G,B,Mode,Time]\r\n"};
const u8 Table_PointAsk1[23] = {"RGB:0~450(����:��~��)\r\n"};
const u8 Table_PointAsk2[28] = {"Mode:0/1/2(֡β,����,����)\r\n"};
const u8 Table_PointAsk3[21] = {"Time:<60000(����)\r\n\r\n"};

const u8 Table_ClarAsk[15] = {"\r\n����?[Y/N]:\r\n"};
const u8 Table_SaveAsk[15] = {"\r\n����?[Y/N]:\r\n"};
const u8 Table_Rest[9] = {"����~\r\n\r\n"};
const u8 Table_Succ[9] = {"�ɹ�!\r\n\r\n"};

const u8 Table_Osc[18] = {"ʱ��[7600~8400]:\r\n"};

const u8 Table_Err[11] = {"�㲻�Ծ�!\r\n"};

void SendMessage(const u8 *pData,u8 Leng){
  while(UART_DATA.SendLen);
  memcpy(UART_DATA.SendBuf,pData,Leng);
  SendData(Leng);
}

void SendAskPoint(u8 num){
  if(num==0){
    SendMessage(Table_PointAsk1,sizeof(Table_PointAsk1));
    SendMessage(Table_PointAsk2,sizeof(Table_PointAsk2));
    SendMessage(Table_PointAsk3,sizeof(Table_PointAsk3));
  }
  while(UART_DATA.SendLen);
  memcpy(UART_DATA.SendBuf,Table_PointAsk0,sizeof(Table_PointAsk0));
  UART_DATA.SendBuf[6] = num+'0';
  SendData(sizeof(Table_PointAsk0));
}

void WaitReset(void){
  while(UART_DATA.SendLen);
  NVIC_SystemReset();
  while(1);
}

//bytes to send
void DoSciData(void){
  static u8 step = 0;
  static u8 Flame = 0;
  Frame_REG Framebuf;
  
  switch(step){
    
  case 0:
    if(memcmp(UART_DATA.RecBuf,Table_Hello,7) == 0){
      SendMessage(Table_Hello,sizeof(Table_Hello));
      if(NowList){      //to GetData
        Flame = 0;
        memset(UserFrame,0,sizeof(UserFrame));
        SendAskPoint(Flame);
        step = 1;       //Input
      }
      else{             //to Clear ROM
        SendMessage(Table_ClarAsk,sizeof(Table_ClarAsk));
        step = 2;       //Clear
      }
    }
    else if(memcmp(UART_DATA.RecBuf,Table_Hello2,8) == 0){
      SendMessage(Table_Hello2,sizeof(Table_Hello2));
      SendMessage(Table_Osc,sizeof(Table_Osc));
      step = 4;
    }
    break;
    
  case 1:
    OutputState = 0;//Sing Color
    if(GetNum(&Framebuf)){
      SendMessage(Table_Err,sizeof(Table_Err));
    }
    else{
      OutputState = 2;//Sing Color
      SetPwm(Framebuf.Color.R,Framebuf.Color.G,Framebuf.Color.B);
      UserFrame[Flame++] = Framebuf;
      if((Flame != 0)&&(Flame>=10 || Framebuf.Mode==0 || Framebuf.Time == 0)){  //End!
        UserFrame[Flame-1].Color.R = UserFrame[Flame-1].Color.G = UserFrame[Flame-1].Color.B = 0;
        UserFrame[Flame-1].Mode = UserFrame[Flame-1].Time = 0;
        NowPoint = NowList = 0;
        if(GetAList(NowList) == 0){
          SendMessage(Table_SaveAsk,sizeof(Table_SaveAsk));
          step = 3;
          OutputState = 1;
          pFrame = &UserFrame[0];
        }
        else{   //���δͨ��
          Flame = 0;
          memset(UserFrame,0,sizeof(UserFrame));
          SendAskPoint(Flame);
          step = 1;       //Input
        }
        break;
      }
      SendAskPoint(Flame);
    }
    break;
      
  case 2://Ask Clear
    if(UART_DATA.RecLen == 3){
      if(UART_DATA.RecBuf[0] == 'Y' || UART_DATA.RecBuf[0] == 'y'){
        memset(UserFrame,0,sizeof(UserFrame));
        SaveAll();//clear
        SendMessage(Table_Succ,sizeof(Table_Succ));
        Flame = 0;
        SendAskPoint(Flame);
        step = 1;       //Input
        break;
      }
      else if(UART_DATA.RecBuf[0] == 'N' || UART_DATA.RecBuf[0] == 'n'){
        RepeatAll();
        SendMessage(Table_Rest,sizeof(Table_Rest));
        WaitReset();
      }
    }
    SendMessage(Table_Err,sizeof(Table_Err));
    break;
    
  case 3:       //Ask Save
    if(UART_DATA.RecLen == 3){
      if(UART_DATA.RecBuf[0] == 'Y' || UART_DATA.RecBuf[0] == 'y'){
        SaveAll();//clear
        SendMessage(Table_Succ,sizeof(Table_Succ));
        RepeatAll();
        SendMessage(Table_Rest,sizeof(Table_Rest));
        WaitReset();
      }
      else if(UART_DATA.RecBuf[0] == 'N' || UART_DATA.RecBuf[0] == 'n'){
        Flame = 0;
        memset(UserFrame,0,sizeof(UserFrame));
        SendAskPoint(Flame);
        step = 1;       //Input
        break;
      }
    }
    SendMessage(Table_Err,sizeof(Table_Err));
    break;
    
  case 4:
    if(GetNum2() == 0){//Succ
      SaveAll();//clear
      SendMessage(Table_Succ,sizeof(Table_Succ));
      RepeatAll();
      SendMessage(Table_Rest,sizeof(Table_Rest));
      WaitReset();
    }
    else{
      SendMessage(Table_Err,sizeof(Table_Err));
    }
    break;
    
  default:
    while(UART_DATA.SendLen);
    NVIC_SystemReset();
  }
}


void SciMode(void){
  
  if(UART_DATA.SendLen)return;
  
  if(UART_DATA.RecPoint>=2 && UART_DATA.RecBuf[UART_DATA.RecPoint-1]=='\n' && UART_DATA.RecBuf[UART_DATA.RecPoint-2] == '\r'){
    UART_DATA.RecLen = UART_DATA.RecPoint;
    UART_DATA.RecPoint = 0;
  }
  else if(UART_DATA.RecPoint && GetDtTime(UART_DATA.RecTime,Time.Cnt1ms) > 10){//10ms TimeOut
    UART_DATA.RecPoint = 0;
    UART_DATA.RecLen = 0;
    for(u8 i=0;i<MaxUartLen;i++)UART_DATA.RecBuf[i] = 0;
    return;
  }
  else return;
  //Stop Rec
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  
  //Do Link
  if(UART_DATA.RecLen > 2)
    DoSciData();
  FlashLED(1,1,1);
  
  //Clear
  UART_DATA.RecLen = 0;
  for(u8 i=0;i<MaxUartLen;i++)UART_DATA.RecBuf[i] = 0;
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


