/*
 * Flash.c
 *
 *  Created on: 2019-1-9
 *      Author: hp
 */
#include "Flash.h"


void FlashInit(void){
    if(SET == FLASH_OB_GetRDP())
      return;
    Flash_Check = 0xBB;
    if(Flash_Check == 0xBB)
        Flash_Check = 0xCDEF;
    Flash_Unlock_Check();
    FLASH_OB_RDPConfig(OB_RDP_Level_1);
    FLASH_OB_Lock();
    FLASH_Lock();
    FLASH_OB_Launch();
}


void ErasFlash(u8 page){
    Uint8 i;
    u32 Size;
   
    Flash_Check = 0xAA;
    Size = APPSize;
    if(page)
      Size += DATASize;
    
    if(Flash_Check == 0xAA)
        Flash_Check = 0xFEDC;
    Flash_Unlock_Check();
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
    
    for(i=0;i<Size/PageSize;i++)
        if (FLASH_ErasePage(StartAddress+i*PageSize)!= FLASH_COMPLETE){
            FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
            FLASH_Lock(); 
            SendState(CMD,Failed);
            return;
        }
    
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
    FLASH_Lock(); 
    
    SendState(CMD,Success);
    FlashClearState = 1;
}


/*
 * 写flash格式
 * Mode  Address Length data
 * 1byet 4byte   1bytes xbytes
 *
 * Mode: 0x03
 * Address: 00 30 00 08 : Address = 0x00300008
 * Length: data size in 16bit //
 * data: data to write:x = 2*Length
 *
 * */

Uint16 FlashWriteBuf[128] = {0};
Uint16 FlashWriteBufSize = 0;
Uint32 FlashWriteAddress = 0;

void FlashProg(void){
    Uint8 i;
    Flash_Check = 0xAA;

    if(FlashClearState == 0){	//未擦除，禁止写入
        SendState(DAT,Failed);
        return;
    }

    if(SciBuf.RxLeng < 6){	//数据长度错误
        SendState(DAT,Failed);
        return;
    }

    FlashWriteAddress = 0;
    FlashWriteAddress |= (Uint32)SciBuf.RxBuf[1]<<24;
    FlashWriteAddress |= (Uint32)SciBuf.RxBuf[2]<<16;
    FlashWriteAddress |= (Uint32)SciBuf.RxBuf[3]<<8;
    FlashWriteAddress |= (Uint32)SciBuf.RxBuf[4];
    FlashWriteBufSize = SciBuf.RxBuf[5];

    if((FlashWriteAddress < StartAddress) || 
       ((FlashWriteAddress+FlashWriteBufSize) > StartAddress+APPSize)){//地址限位
        SendState(DAT,Failed);
        return;
    }

    if(FlashWriteBufSize*2+6 != SciBuf.RxLeng){	//数据长度错误
        SendState(DAT,Failed);
        return;
    }

    for(i = 0 ; i < FlashWriteBufSize ; i++){
        FlashWriteBuf[i] = SciBuf.RxBuf[i*2 +7]<<8 | SciBuf.RxBuf[i*2 +6];
        if((FlashWriteBuf[i] != 0) && (FlashWriteBuf[i] != RAN_FLASH))
                FlashWriteBuf[i] ^= RAN_FLASH;
    }

    if(Flash_Check == 0xAA)
        Flash_Check = 0xFEDC;
    Flash_Unlock_Check();
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
    
    for(i=0;i<FlashWriteBufSize;i++)
      if(FLASH_ProgramHalfWord(FlashWriteAddress + 2*i,FlashWriteBuf[i]) != FLASH_COMPLETE){
          FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
          FLASH_Lock(); 
          SendState(DAT,Failed);
          return;
      }    
    
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
    FLASH_Lock(); 
    
    SendState(DAT,Success);

}


void FlashCheck(void){
	Uint8 i;
	Uint32 Addr;
	Uint16 CRCData = 0xFFFF;
	Uint16 SUM = 0;

	for(Addr = StartAddress ; Addr < StartAddress+APPSize ; Addr++){
		SUM += *(Uint16 *)Addr;
		CRCData ^= *(Uint16 *)Addr;
		for(i=0;i<8;i++){
	  		if(CRCData & 0x01)
	  			CRCData = (CRCData>>1)^0xa001;
	   		else
	   			CRCData = CRCData>>1;
		}
	}

	SUM ^= (Uint16)SciBuf.RxBuf[1]<<8;
	SUM ^= (Uint16)SciBuf.RxBuf[2];
	CRCData ^= (Uint16)SciBuf.RxBuf[3]<<8;
	CRCData ^= (Uint16)SciBuf.RxBuf[4];

	if(SUM || CRCData){				//failed
		//校验失败，擦除flash后回复。
		SendState(CHECK,Failed);
	}
	else{
		SendState(CHECK,Success);
	}

}

void Flash_Unlock_Check(void){
    if(Flash_Check == 0xFEDC){
        FLASH_Unlock();
    }else if(Flash_Check == 0xCDEF){
        FLASH_Unlock();
        FLASH_OB_Unlock();
    }
    Flash_Check = 0xFFFF;
}



