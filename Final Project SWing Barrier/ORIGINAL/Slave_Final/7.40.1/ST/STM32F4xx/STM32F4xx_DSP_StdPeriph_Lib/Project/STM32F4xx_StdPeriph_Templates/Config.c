#include "main.h"

void GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Initialize the GPIO structure with default values */

  /* This enables the peripheral clock to the GPIOA, GPIOB, GPIOF IO - modules */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 /* terminals for A and B */
 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_10;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 /* Configure PA4 to PA6 in Digita input mode for CALCP */
 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 /*terminal for clutch*/
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 //ports for PWM generation

 
 /* Alternating functions for pins */
 GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
 GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
 GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
 GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
 
 /* Set pins */
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_Init(GPIOD, &GPIO_InitStructure);

  /********* Configure Digital Outputs ***************/

//  GPIO_WriteBit(GPIOE, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
//  GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);
  //PE8 = direction, PE9 = brake, PE10 = Sleep
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /******UART configuration ********/
      // Initialization of GPIOB
  // alternate function configuration
    
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
  
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Initialization of USART1
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_HardwareFlowControl =
            USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStruct);
    
    //nVIC enable
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}



/**
  * @brief  Initializes and configures the Timer for PWM generation
  * @param  None
  * @retval None
  */
void TM_TIMER_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_BaseStruct.TIM_Prescaler = 0;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 8399; /* 10kHz PWM */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
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
   
    TIM_OCStruct.TIM_Pulse = 2099; /*  25% duty cycle */
    TIM_OC1Init(TIM4, &TIM_OCStruct);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
//    TIM_OCStruct.TIM_Pulse = 4199; /* 50% duty cycle */
//    TIM_OC2Init(TIM4, &TIM_OCStruct);
//    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
//    
//    TIM_OCStruct.TIM_Pulse = 6299; /* 75% duty cycle */
//    TIM_OC3Init(TIM4, &TIM_OCStruct);
//    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
//    
//    TIM_OCStruct.TIM_Pulse = 8399; /* 100% duty cycle */
//    TIM_OC4Init(TIM4, &TIM_OCStruct);
//    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/**
  * @brief  Configures the PWM on time for modifying speed
  * @param  speed value from master via uart from 1 to 9
  * @retval None
  */
void Config_PWM(uint8_t x)
{    
  //configure for count value
    if( (x >= 1) && (x <= 4))
      enc_pos = 180 - (x*5);
    else if((x >= 5) && (x <= 9))
      enc_pos = 200 - (x*10);
    else if (x == 7)
      enc_pos = 125;
  
    TIM_OCInitTypeDef TIM_OCStruct;
    
    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCStruct.TIM_Pulse = 419 + (x * 420);//10% to 50% PWM pulse width
    TIM_OC1Init(TIM4, &TIM_OCStruct);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}
