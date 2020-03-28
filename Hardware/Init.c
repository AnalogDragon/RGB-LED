#include "Init.h"

void GPIO_config(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF,ENABLE);
  
  //PWM输出TIM3 CH4 PB4
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //PWM输出TIM3 CH4 PB1 PB2 : PA6，PA7
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //key PA0 PA4 PA5 PA10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //ADC采样 PA1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //LED显示 PA2 PA3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_2 | GPIO_Pin_3);
  //Power EN PA9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,GPIO_Pin_9);
  //PF0 PF1 拉低保护
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOF,GPIO_Pin_0 | GPIO_Pin_1);
}

void NVIC_Config(void){
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Tim1Init(u16 per,u16 psc){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = per;
  TIM_TimeBaseStructure.TIM_Prescaler = psc;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM1,TIM_FLAG_Update);
  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM1, ENABLE);
}


void Tim3InitPWM(u16 per,u16 psc){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = per;
  TIM_TimeBaseStructure.TIM_Prescaler = psc;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //CH1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_Cmd(TIM3, ENABLE);
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
}


void Tim3InitCAP(u16 per,u16 psc){
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period=per;
  TIM_TimeBaseStructure.TIM_Prescaler=psc;
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,
                             TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter=10;
  TIM_ICInit(TIM3,&TIM_ICInitStructure);
  TIM_ClearFlag(TIM3,TIM_FLAG_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

  TIM_SetCounter(TIM3,0x7fff);
  TIM_Cmd(TIM3,ENABLE);
}


void ADC_Config(void){
  ADC_InitTypeDef     ADC_InitStructure;
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);  
   /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStructure); 

  /* Convert the ADC1 Channel11 and channel10 with 55.5 Cycles as sampling time */ 
  
  ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_41_5Cycles); 
//  ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_55_5Cycles); 
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
  DMA_Config();
}


void DMA_Config(void){
  DMA_InitTypeDef   DMA_InitStructure;
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMABUF;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_Lg;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void SysTickConfig(void){
  while(SysTick_Config(TimeBase-1));
}


void InitAll(void){
  SystemInit();         //系统时钟
  GPIO_config();        //GPIO初始化
  ADC_Config();         //ADC引脚初始化
}

void InitTimer(void){
  SysTickConfig();      //1ms SysTick
  Tim1Init(TimeBase-1,1-1);    //定时器中断，用于LED刷新1khz
  Tim3InitPWM(FullVal-1,1-1);//LED PWM 17.7Khz
  NVIC_Config();
}




