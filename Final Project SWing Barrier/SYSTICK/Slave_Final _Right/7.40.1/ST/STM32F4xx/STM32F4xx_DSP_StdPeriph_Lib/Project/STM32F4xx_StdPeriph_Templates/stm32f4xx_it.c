/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.0RC2
  * @date    20-February-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  uint8_t rx_array[4];
  int is_complete;
  //vars for storing the commands sent by MASTER BOARD!
  int8_t free,speed,direction = -1;
  int8_t fire;
  int enc_pos;
  int8_t curr_pos_of_gate = 0;//initial position
  int flag = 0;
  uint8_t id;
  int x = 0; //index of the receptioon array
  int is_gate_moving = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  //check for errors and mishandling
  //function to check encoder values
  Encoder_Logic();
}

void USART2_IRQHandler(void)
{
if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  {
    /*PLEASE SEE THE FRAME FORMAT*/

    //<Identifier byte><data><data>
    if( x <= 2 )
      rx_array[x++] = USART_ReceiveData(UART5);
  }
  if(x == 3)
  {
    id = rx_array[0];
    if(id == 1)
      free = rx_array[1];
    else if (id == 2)
    {
      speed = rx_array[1];
      direction = rx_array[2];
    }
    else if (id == 3)
      fire = rx_array[1];
    
    if(fire == 1)
    {
      OpenGates();
    }
    else if(free == 0)//should not execute this if the if case is true above..CHECK THIS
    {
      if((curr_pos_of_gate == 0) && (direction == 1))
      {
        curr_pos_of_gate = 1;
        flag = 1;
      }
      else if((curr_pos_of_gate == 1) && (direction == 0))
      {
        curr_pos_of_gate = 0;
        flag = 1;
      }
      else if((curr_pos_of_gate == 0) && (direction == 0))
      {
        curr_pos_of_gate = -1;
        flag  = 1;
      }
      else if((curr_pos_of_gate == -1) && (direction == 1))
      {
        curr_pos_of_gate = 0;
        flag = 1;
      }
      
      if (flag == 1)
      {
        if(is_gate_moving == 1)
          Emergency_PWM(speed);
        else if  ( is_gate_moving == 0)
          Config_PWM(speed);
        Motor_Logic(direction);
        flag = 0;
        is_gate_moving = 1;
      }
    } 
    x = 0;
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
