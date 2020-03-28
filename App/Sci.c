#include "Sci.h"

/*
 * 数据格式
 * head  Addr  length data   check
 * 1byte 1byte 1byte  xbyte  2byte
 * head = 0xAA
 * length = x + 2
 * Addr 广播帧 = 0无回复/0xFF回复 = LocAddr 本机回复
 * check = CRC(head + length + data)
 *
 * */

/*
 * 输入char
 * 输入值：串口数据指针
 * 返回值：0成功 1失败
 * */
Uint32 SciDelayBuf;
Uint8 UsartDataReady = 0;
Uint8 UsartData;
Uint8 SciDelay;
Uint16 ErrTime;

Uint8 GetByte(Uint8 *data){
    SciDelayBuf = 0;
    while(UsartDataReady == 0){
        SciDelayBuf++;
        if(SciDelayBuf>110000)return 1;    //1.05s超时
    }
        UsartDataReady = 0;
    *data = UsartData;
    return 0;
}

Uint8 handed = 0;//是否成功握手标记

/*
 * 输入一帧
 * 返回值：0成功 1失败
 * */
Uint8 GetPacket(void){
    Uint8 i,buf,ComAddrBuf,time=0;
    Uint16 CRCData = 0xffff;

    do{
        if(GetByte(&buf)){
            //超时处理
            time++;
            ErrTime++;
            if(handed){
              if(time>=10){
                time = 0;
                SendState(IDLE,Success);//发送自己空闲
              }
            }else{
                JumpToApp(1);
            }
        }
        if(buf != 0xAA){
          ErrTime++;
          if(ErrTime>600)
            JumpToApp(0);
        }
    }
    while(buf != 0xAA);//等待数据包头
    SciDelay = SciDelayBuf&0xFF;

    handed = 1;//成功收到过一个AA就判断为握手成功
    CRCCheck(&CRCData,0xAA);
    
    //获取地址
    if( GetByte(&buf) ) return 1;
    CRCCheck(&CRCData,buf);
    ComAddrBuf = buf;  //地址
    
    if( GetByte(&buf) ) return 1;//数据长度
    CRCCheck(&CRCData,buf);
    SciBuf.RxLeng = buf-2;        //减去CRC

    for(i = 0 ; i < SciBuf.RxLeng ; i++){//接收数据，最后2Byte为CRC
        if( GetByte(&buf) ) return 1;
        CRCCheck(&CRCData,buf);
        SciBuf.RxBuf[i] = buf;
    }
    
    if(GetByte(&buf))return 1;        //2Byte CRC,LSB
    CRCData ^= buf;
    if(GetByte(&buf))return 1;        //MSB
    CRCData ^= buf<<8;

#ifdef IsTest
    if(buf == 0){        //debug
        ErrTime = 0;
        if(ComAddrBuf){
            if(ComAddrBuf == 0xFF)
                ComAddrOnce = 1;
            else
                ComAddr = ComAddrBuf;
        }
        if(ComAddr == LocAddr || ComAddrBuf == 0 || ComAddrBuf == 0xFF){
            UnlockData();
            delay(2500);
            return 0;
        }
    }
#endif

    if(CRCData == 0){
        ErrTime = 0;
        if(ComAddrBuf){
            if(ComAddrBuf == 0xFF)
                ComAddrOnce = 1;
            else
                ComAddr = ComAddrBuf;
        }
        if(ComAddr == LocAddr || ComAddrBuf == 0 || ComAddrBuf == 0xFF){
            UnlockData();
            return 0;
        }
    }
    ErrTime++;
    return 1;
}


/*
 * 数据解密
 * 调用前需验证CRC
 * */
void UnlockData(void){
    Uint8 i,j,buf0;
    Uint8 buf[16];
    Uint8 buf1[16];
    if(SciBuf.RxLeng < 5){                //长度小于5时仅异或B
        for(i=0;i<SciBuf.RxLeng;i++){
            SciBuf.RxBuf[i] ^= BXOR;
        }
    }
    else{                                //长度大于5时，先用A2计算位移，再异或A1
        for(i = 0 ; i < SciBuf.RxLeng/16 ; i++){
            strcopy(&buf[0],&SciBuf.RxBuf[i*16],16);
            for(j = 0 ; j < (Ran&0xF) ; j++){            //操作次数 2-5
                //先循环左移3再换位
                strcopy(&buf1[0],&buf[3],13);
                strcopy(&buf1[13],&buf[0],3);
                buf0 = buf1[0];
                buf1[0] = buf1[(Ran>>4)&0xf];
                buf1[(Ran>>4)&0xf] = buf0;
                strcopy(&buf[0],&buf1[0],16);
            }
            strcopy(&SciBuf.RxBuf[i*16],&buf[0],16);
        }

        for(i=0;i<SciBuf.RxLeng;i++){    //异或
            if(i%2)
                SciBuf.RxBuf[i] ^= AXORL;    //奇数
            else
                SciBuf.RxBuf[i] ^= AXORH;
        }
    }
}
/*
 * 数据加密
 * 调用后需计算CRC
 * */
/*void LockData(void){
    Uint8 i,j,buf0;
    Uint8 buf[16];
    Uint8 buf1[16];
    if(SciBuf.TxLeng < 5){                //长度小于5时仅异或B
        for(i=0;i<SciBuf.TxLeng;i++){
            SciBuf.TxBuf[i] ^= BXOR;
        }
    }
    else{                                //长度大于5时，先用A2计算位移，再异或A1        
        for(i=0;i<SciBuf.TxLeng;i++){    //异或
            if(i%2)
                SciBuf.TxBuf[i] ^= AXORL;    //奇数
            else
                SciBuf.TxBuf[i] ^= AXORH;
        }
        for(i = 0 ; i < SciBuf.TxLeng/16 ; i++){
            strcopy(&buf[0],&SciBuf.TxBuf[i*16],16);
            for(j = 0 ; j < (Ran&0xF) ; j++){            //操作次数 2-5
                //先换位再右移
                buf0 = buf[0];
                buf[0] = buf[(Ran>>4)&0xf];
                buf[(Ran>>4)&0xf] = buf0;
                strcopy(&buf1[0],&buf[13],3);
                strcopy(&buf1[3],&buf[0],13);
                strcopy(&buf[0],&buf1[0],16);
            }
            strcopy(&SciBuf.TxBuf[i*16],&buf[0],16);
        }


    }
}*/

/*
 * 发送一帧
 * 调用发送TxBuf内的值
 * */
void SendPacket(void){
    Uint8 i;
    if(SciBuf.TxLeng == 0 || SciBuf.TxLeng>10) 
        return;//判断发送长度合法
        
    PAout(1) = 1;    //占用总线（485 only）
    delay(50);
    for(i = 0 ; i < SciBuf.TxLeng ; i++){
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//等待发送完成
        USART_SendData(USART2,SciBuf.TxBuf[i]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//等待发送完成
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完成
    
    PAout(1) = 0;
    SciBuf.TxLeng = 0;
}

/*
 * CRC校验函数
 * 输入量 ：( CRC输出累计值指针，需要计进的值）
 * */
void CRCCheck(Uint16 *Output,Uint8 Input)
{
    Uint8 i;
    *Output ^= Input;
    for(i=0;i<8;i++){
          if(*Output & 0x01)
              *Output = (*Output>>1)^0xa001;
           else
               *Output = *Output>>1;
    }
}


/*
 * 发送状态
 * 输入量：(功能码，成功状态)
 * */
void SendState(Uint8 step,Uint8 state){
    Uint16 CRCData = 0xffff;
    Uint8 i;
    if((ComAddr != LocAddr)&&(ComAddrOnce != 1)){  //广播帧或者回复帧
        ComAddrOnce = 0;
        return;
    }
    ComAddrOnce = 0;
    SciBuf.TxBuf[0] = 0xAA;
    SciBuf.TxBuf[1] = LocAddr;
    SciBuf.TxBuf[2] = 0x04;
    SciBuf.TxBuf[3] = step^BXOR;
    SciBuf.TxBuf[4] = state^BXOR;

    SciBuf.TxLeng = 7;

    for(i=0;i<SciBuf.TxLeng-2;i++)
        CRCCheck(&CRCData,SciBuf.TxBuf[i]);

    SciBuf.TxBuf[5] = CRCData&0xff;            //LSB
    SciBuf.TxBuf[6] = (CRCData>>8)&0xff;    //MSB

    SendPacket();
}


/*
 * 字符串复制
 * str2复制到str1的位置
 *
 * */
void strcopy(Uint8 *str1,Uint8 *str2,Uint16 Length){
    Uint16 i;
    for(i=0;i<Length;i++){
        *str1 = *str2;
        str1++;
        str2++;
    }
}

void delay(u16 num){
  for(num=num;num>0;num--);
}

