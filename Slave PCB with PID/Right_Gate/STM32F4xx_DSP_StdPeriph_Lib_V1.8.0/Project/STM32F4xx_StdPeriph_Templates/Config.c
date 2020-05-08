#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

GPIO_InitTypeDef  GPIO_InitStructure;
extern arm_pid_instance_f32 PID;
int width_pwm;
void GPIO_Config(void)
{
  /*********** Initialize the GPIO structure with default values **************/
  /* Enabling the peripheral clock to the GPIOA, GPIOB to GPIOF IO - modules */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* terminal for Optical Incremental Encoder */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//Terminals A and B
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* terminal for clutch */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Terminal for bicolor LED Strips */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Digital Input pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//check
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//check
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /*Digital Output Pins (RELAY) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Motor Driver Direction & Brake Output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /*Motor Driver nSLEEP output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /************ Alternating functions for pins *********************************/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
  
  /* Set pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    
  /******UART configuration ********/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  // Initialization of UART5
  
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_Init(UART5, &USART_InitStruct);
  
  //nVIC enable
  
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  USART_Cmd(UART5, ENABLE);
  
  /*********************** Configure SWD ****************************/
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_SWJ);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_SWJ);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SWJ);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SWJ);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SWJ);
  
}

/**
  * @brief  Initializes and configures the Timer for PWM generation
  * @param  None
  * @retval None
  */
void TM_TIMER_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    TIM_BaseStruct.TIM_Prescaler = 0;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    //period = clock ( 50MHz )/((frequency needed(10kHz))(Prescalar + 1)) APB1 peripheral at 50MHz
    TIM_BaseStruct.TIM_Period = 4999; /* 10kHz PWM */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM12, &TIM_BaseStruct);
}

/**
  * @brief  Initializes and Configures the PWM 
  * @param  None
  * @retval None
  */

void TM_PWM_Init(void) {
    TIM_OCInitTypeDef TIM_OCStruct;
    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
   
    TIM_OCStruct.TIM_Pulse = 449; /*  10% duty cycle */
    TIM_OC1Init(TIM12, &TIM_OCStruct);
    TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
}

/**
  * @brief  Configures the PWM on time for modifying speed
  * @param  speed value from master via uart from 1 to 9
  * @retval None
  */
void Config_PWM(uint8_t x, uint8_t f)
{    
//  //configure for count value
//    if( (x >= 1) && (x <= 4))
//      enc_pos = 180 - (x*5);
//    else if((x >= 5) && (x <= 9))
//      enc_pos = 200 - (x*10);
//    else if (x == 7)
//      enc_pos = 125;
  float m;
  switch( x )
  {
  case 1:m = 36;
  break;
  case 2:m = 18;
  break;
  case 3:m = 12;
  break;
  case 4:m = 9;
  break;
  case 5:m = 6;
  break;
  case 6:m = 4.5;
  break;
  case 7:m = 3.3;
  break;
  case 8:m = 3;
  break;
  case 9:m = 2.6;
  break;
  default:break;//insert an error
  }
  if((is_gate_moving == 1) || (f == 1))
  {
    GPIO_WriteBit(GPIOE, GPIO_Pin_14, Bit_RESET);//engage brakes
    uwTimingDelay = 200;
    while(uwTimingDelay != 0);//inserting delay
    ReadFromFlash();
    int temp = Flash_Data_Read_Buffer[0];// get the count
    if(f == 1)
      enc_pos = temp;// - ((temp - 180)/ m));//180 is the position we need
    else 
      enc_pos = temp;// - (temp / m));
  }
  else if((is_gate_moving == 0) && (f == 0))
    enc_pos =180;// - (180 / m));//temp gives current psoition of the gate, required only during emergency stopping or fire mode 
  TIM_OCInitTypeDef TIM_OCStruct; 
  TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCStruct.TIM_Pulse = 249 + (x * 250);//10% to 50% PWM pulse width
  TIM_OC1Init(TIM12, &TIM_OCStruct);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
  width_pwm = 249 + (x * 250);
}

/**
  * @brief  Initializes the PID structure.
  * @param  None
  * @retval None
  */
void init_pid(void)
{
  PID.A0 = PID.Kd + PID.Ki + PID.Kp;
  PID.A1 = - (PID.Kp) - (2 * PID.Kd);
  PID.A2 = PID.Kd;
  PID.state[0] = 0;
  PID.state[1] = 0;
  PID.state[2] = 0;
}

/**
  * @brief  Reconfiguration of PWM based on PID output
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void adjust_pwm(float x)
{
  TIM_Cmd(TIM12, DISABLE);
  int temp = (int)x;
  TIM_OCInitTypeDef TIM_OCStruct; 
  TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCStruct.TIM_Pulse = temp;
  TIM_OC1Init(TIM12, &TIM_OCStruct);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
  TIM_Cmd(TIM12, ENABLE);
}


/**
  * @brief  Applies brakes to the motor
  * @param  None
  * @retval None
  */
void EngageBrakes(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_14, Bit_RESET);
}

/**
  * @brief  Removes brakes to the motor
  * @param  None
  * @retval None
  */
void DisEngageBrakes(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_14, Bit_SET);
}

/**
  * @brief  Motor rotates Clockwise
  * @param  None
  * @retval None
  */
void Dir_Clkwise(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_SET);
}

/**
  * @brief  Motor rotates ANTI_Clockwise
  * @param  None
  * @retval None
  */
void Dir_AntiClkwise(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_RESET);
}
 
/**
  * @brief  Clutch gets activated, halting the motor
  * @param  None
  * @retval None
  */
void EngageClutch(void)
{
  GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_SET);
}

/**
  * @brief  Clutch gets deactivated, freeing the motor
  * @param  None
  * @retval None
  */
void DisEngageClutch(void)
{
  GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_RESET);
}

/**
  * @brief  lED to indicate no entry
  * @param  None
  * @retval None
  */
void StopEntry(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET);//RED is ON
  GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET);//GREEN is OFF
}

/**
  * @brief  lED to indicate entry
  * @param  None
  * @retval None
  */
void GoEntry(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET);//RED is OFF
  GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_SET);//GREEN is ON
}

/**
  * @brief  lED to indicate no exit
  * @param  None
  * @retval None
  */
void StopExit(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);//RED is ON
  GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET);//GREEN is OFF
}

/**
  * @brief  lED to indicate exit
  * @param  None
  * @retval None
  */
void GoExit(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);//RED is OFF
  GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET);//GREEN is ON
}

/**
  * @brief  lED to indicate gate closed
  * @param  None
  * @retval None
  */
void StopGate(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);//RED is ON
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);//GREEN is OFF
}

/**
  * @brief  lED to indicate gate open
  * @param  None
  * @retval None
  */
void GoGate(void)
{
  GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_RESET);//RED is OFF
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);//GREEN is ON
}


void Emergency_PWM(uint8_t s)
{
  float x;
  switch( s )
  {
  case 1:x = 36;
  break;
  case 2:x = 18;
  break;
  case 3:x = 12;
  break;
  case 4:x = 9;
  break;
  case 5:x = 6;
  break;
  case 6:x = 4.5;
  break;
  case 7:x = 3.3;
  break;
  case 8:x = 3;
  break;
  case 9:x = 2.6;
  break;
  default:break;
  }
  GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//engage brakes
  uwTimingDelay = 200;
  while(uwTimingDelay != 0);//inserting delay
  ReadFromFlash();
  int temp = Flash_Data_Read_Buffer[0];// get the count
  enc_pos = (int)(temp - (temp / x));
  TIM_OCInitTypeDef TIM_OCStruct;
    
  TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCStruct.TIM_Pulse = 249 + (s * 250);//10% to 50% PWM pulse width
  TIM_OC1Init(TIM12, &TIM_OCStruct);
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
}
