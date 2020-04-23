/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.2.0RC2
  * @date    20-February-2013
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
uint32_t count = 0;
//uint16_t count_A = 0;
//uint16_t count_B = 0;
uint32_t oldcount;
uint8_t FLAG = 0;
int lastcounter = 0;
int aLastState = 0;
int aState = 0;
int counter = 0;
int counter_buf = 0;
int position = 0;
int val = 0;
int flash_flag = 1;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);
RCC_ClocksTypeDef RCC_Clocks;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  /* Initialize system */
  SystemInit();
  SysTick_Config(SystemCoreClock /1000);
  TM_TIMER_Init();
  TM_PWM_Init();
  GPIO_Config();
  oldcount = count;
  aLastState = GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_10);
  GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);
  
  while (1)
  {
    Encoder_Logic();   
    if(flash_flag == 1)
    {
      Init_Sequence();
      flash_flag = 0;
    }
    if(flash_flag == 0)
      ErrorCheckEncoder();
  }
}

/**
  * @brief  Starting and stopping of motor using encoder states
  * @param  None
  * @retval None
  */
void Motor_Logic(uint8_t x)
{
  if(flash_flag == 0)
  {
    if(x == 0)//anti-clockwise
    {
      GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_RESET);
      position = counter_buf;
      counter = counter_buf;
      GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET); //brake set
      TIM_Cmd(TIM4, ENABLE);
      if((position == -enc_pos))
      {
        GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//brake reset
        Delay(500);//check here
        oldcount = count;
        TIM_Cmd(TIM4, DISABLE);
        is_gate_moving = 0;
      }
      
    }
    else if(x == 1)
    {
      GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);
      position = counter_buf;
      counter = counter_buf;
      GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET); //brake set
      TIM_Cmd(TIM4, ENABLE);//start the timer to move in the set direction
      if((position == enc_pos))
      {
        GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//brake reset
        Delay(500);//check here
        oldcount = count;
        TIM_Cmd(TIM4, DISABLE);
        is_gate_moving = 0;
      }
    }
  }
}

/**
  * @brief  Implements the Encoder basic functionality to decode the direction and degrees traversed
  * @param  None
  * @retval None
  */

void Encoder_Logic(void)
{
  aState = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);

  if (aState != aLastState)
  {     
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) != aLastState) 
    { 
      lastcounter = counter;
      counter ++;
      position = counter;
      if((free == 1) && (flash_flag == 0))
        counter_buf ++;
    }
    else 
    {
      lastcounter = counter;
      counter --;
      position = counter;
      if((free == 1) && (flash_flag == 0))
        counter_buf --;
    }
    Flash_Data_Write_Buffer[0] = counter;
    WriteToFlash();
  }
  
  aLastState = aState;
  
  if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) == 1)
  {
    FLAG = 1;
  }
  
  if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) == 0)
  {
    if(FLAG == 1)
    {
      count++;
      FLAG=0;
    }
  }
}

/**
  * @brief  Initial power up sequence to check if the gate is in closed position
  * @param  None
  * @retval None
  */

void Init_Sequence(void)
{
  /*reading value of counter from flash memory */
  ReadFromFlash();
  int temp = Flash_Data_Read_Buffer[0];
  if ((temp != 0) || (temp != 1) || (temp != -1))
  {
    if ( temp < -2 )
    {
      GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);//go clockwise
      position = temp;
      counter = temp;//change on 19th march...
      GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET); //brake set
      TIM_Cmd(TIM4, ENABLE);
      if((position == 0))
      {
        GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//brake reset
        TIM_Cmd(TIM4, DISABLE);
      }
    }
    else if ( temp > 2 )
    {
      GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_RESET);//go anti-clockwise
      position = temp;
      counter = temp;//change on 19th march...
      GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET); //brake set
      TIM_Cmd(TIM4, ENABLE);
      if((position == 0))
      {
        GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//brake reset
        TIM_Cmd(TIM4, DISABLE);
      }
    }
  }
}

/**
  * @brief  Checks for any errors like intrusion of the system
  * @param  None
  * @retval None
  */

void ErrorCheckEncoder(void)
{
  if((free == 0) && (is_gate_moving == 0)); // should not be activated in free mode at all!!!
  {
    if ( oldcount == (count-2) )//2 degrees of tolerance before activating the clutch.. count value only increases. so we can track the magnitude only..
    {
      GPIO_SetBits(GPIOB,GPIO_Pin_11); //activate the clutch
      Delay(3000);//3 sec delay
      GPIO_ResetBits(GPIOB,GPIO_Pin_11); //deactivates the clutch
      oldcount = count;
      enc_pos = enc_pos - 2;// to cancel out theerror caused
    }
  }
  // check for forceful entry in system while moving
  if((free == 0) && (is_gate_moving == 1) && (direction == 0))
  {
    if(lastcounter < counter) //if at all there is a intrusion
    {
      TIM_Cmd(TIM4, DISABLE);
      GPIO_SetBits(GPIOB,GPIO_Pin_11); //activate the clutch
      Delay(3000);//3 sec delay
      GPIO_ResetBits(GPIOB,GPIO_Pin_11); //deactivates the clutch
      TIM_Cmd(TIM4, ENABLE);
    }
  }
  else if((free == 0) && (is_gate_moving == 1) && (direction == 1))
  {
    if(lastcounter > counter) //if at all there is a intrusion
    {
      TIM_Cmd(TIM4, DISABLE);
      GPIO_SetBits(GPIOB,GPIO_Pin_11); //activate the clutch
      Delay(3000);//3 sec delay
      GPIO_ResetBits(GPIOB,GPIO_Pin_11); //deactivates the clutch
      TIM_Cmd(TIM4, ENABLE);
    }
  }
}

/**
  * @brief  Opens the gate in the exit direction
  * @param  None
  * @retval None
  */


void OpenGates(void)
{
  ReadFromFlash();
  int temp = Flash_Data_Read_Buffer[0];
  if( temp - 20 < 180 )
  {
    GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);//go clockwise
    position = temp;
    counter = temp;
    GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET); //brake set
    TIM_Cmd(TIM4, ENABLE);
    if((position == 180))
    {
      GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);//brake reset
      TIM_Cmd(TIM4, DISABLE);
    }
  }

}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */

void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */

void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
