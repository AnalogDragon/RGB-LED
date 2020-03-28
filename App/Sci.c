#include "Sci.h"

/*
 * ���ݸ�ʽ
 * head  Addr  length data   check
 * 1byte 1byte 1byte  xbyte  2byte
 * head = 0xAA
 * length = x + 2
 * Addr �㲥֡ = 0�޻ظ�/0xFF�ظ� = LocAddr �����ظ�
 * check = CRC(head + length + data)
 *
 * */

/*
 * ����char
 * ����ֵ����������ָ��
 * ����ֵ��0�ɹ� 1ʧ��
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
        if(SciDelayBuf>110000)return 1;    //1.05s��ʱ
    }
        UsartDataReady = 0;
    *data = UsartData;
    return 0;
}

Uint8 handed = 0;//�Ƿ�ɹ����ֱ��

/*
 * ����һ֡
 * ����ֵ��0�ɹ� 1ʧ��
 * */
Uint8 GetPacket(void){
    Uint8 i,buf,ComAddrBuf,time=0;
    Uint16 CRCData = 0xffff;

    do{
        if(GetByte(&buf)){
            //��ʱ����
            time++;
            ErrTime++;
            if(handed){
              if(time>=10){
                time = 0;
                SendState(IDLE,Success);//�����Լ�����
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
    while(buf != 0xAA);//�ȴ����ݰ�ͷ
    SciDelay = SciDelayBuf&0xFF;

    handed = 1;//�ɹ��յ���һ��AA���ж�Ϊ���ֳɹ�
    CRCCheck(&CRCData,0xAA);
    
    //��ȡ��ַ
    if( GetByte(&buf) ) return 1;
    CRCCheck(&CRCData,buf);
    ComAddrBuf = buf;  //��ַ
    
    if( GetByte(&buf) ) return 1;//���ݳ���
    CRCCheck(&CRCData,buf);
    SciBuf.RxLeng = buf-2;        //��ȥCRC

    for(i = 0 ; i < SciBuf.RxLeng ; i++){//�������ݣ����2ByteΪCRC
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
 * ���ݽ���
 * ����ǰ����֤CRC
 * */
void UnlockData(void){
    Uint8 i,j,buf0;
    Uint8 buf[16];
    Uint8 buf1[16];
    if(SciBuf.RxLeng < 5){                //����С��5ʱ�����B
        for(i=0;i<SciBuf.RxLeng;i++){
            SciBuf.RxBuf[i] ^= BXOR;
        }
    }
    else{                                //���ȴ���5ʱ������A2����λ�ƣ������A1
        for(i = 0 ; i < SciBuf.RxLeng/16 ; i++){
            strcopy(&buf[0],&SciBuf.RxBuf[i*16],16);
            for(j = 0 ; j < (Ran&0xF) ; j++){            //�������� 2-5
                //��ѭ������3�ٻ�λ
                strcopy(&buf1[0],&buf[3],13);
                strcopy(&buf1[13],&buf[0],3);
                buf0 = buf1[0];
                buf1[0] = buf1[(Ran>>4)&0xf];
                buf1[(Ran>>4)&0xf] = buf0;
                strcopy(&buf[0],&buf1[0],16);
            }
            strcopy(&SciBuf.RxBuf[i*16],&buf[0],16);
        }

        for(i=0;i<SciBuf.RxLeng;i++){    //���
            if(i%2)
                SciBuf.RxBuf[i] ^= AXORL;    //����
            else
                SciBuf.RxBuf[i] ^= AXORH;
        }
    }
}
/*
 * ���ݼ���
 * ���ú������CRC
 * */
/*void LockData(void){
    Uint8 i,j,buf0;
    Uint8 buf[16];
    Uint8 buf1[16];
    if(SciBuf.TxLeng < 5){                //����С��5ʱ�����B
        for(i=0;i<SciBuf.TxLeng;i++){
            SciBuf.TxBuf[i] ^= BXOR;
        }
    }
    else{                                //���ȴ���5ʱ������A2����λ�ƣ������A1        
        for(i=0;i<SciBuf.TxLeng;i++){    //���
            if(i%2)
                SciBuf.TxBuf[i] ^= AXORL;    //����
            else
                SciBuf.TxBuf[i] ^= AXORH;
        }
        for(i = 0 ; i < SciBuf.TxLeng/16 ; i++){
            strcopy(&buf[0],&SciBuf.TxBuf[i*16],16);
            for(j = 0 ; j < (Ran&0xF) ; j++){            //�������� 2-5
                //�Ȼ�λ������
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
 * ����һ֡
 * ���÷���TxBuf�ڵ�ֵ
 * */
void SendPacket(void){
    Uint8 i;
    if(SciBuf.TxLeng == 0 || SciBuf.TxLeng>10) 
        return;//�жϷ��ͳ��ȺϷ�
        
    PAout(1) = 1;    //ռ�����ߣ�485 only��
    delay(50);
    for(i = 0 ; i < SciBuf.TxLeng ; i++){
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//�ȴ��������
        USART_SendData(USART2,SciBuf.TxBuf[i]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//�ȴ��������
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//�ȴ��������
    
    PAout(1) = 0;
    SciBuf.TxLeng = 0;
}

/*
 * CRCУ�麯��
 * ������ ��( CRC����ۼ�ֵָ�룬��Ҫ�ƽ���ֵ��
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
 * ����״̬
 * ��������(�����룬�ɹ�״̬)
 * */
void SendState(Uint8 step,Uint8 state){
    Uint16 CRCData = 0xffff;
    Uint8 i;
    if((ComAddr != LocAddr)&&(ComAddrOnce != 1)){  //�㲥֡���߻ظ�֡
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
 * �ַ�������
 * str2���Ƶ�str1��λ��
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

