/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t count = 0;
uint32_t oldcount;
uint8_t FLAG = 0;
int lastcounter = 0;
int aLastState = 0;
int aState = 0;
int counter = 0;
int counter_buf = 0;
int position = 0;
int fire_exit = 0;
int flash_flag = 1;

static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
//  GPIO_InitTypeDef GPIO_InitStructure;
// 
// /*!< At this stage the microcontroller clock setting is already configured, 
//       this is done through SystemInit() function which is called from startup
//       files before to branch to application main.
//       To reconfigure the default setting of SystemInit() function, 
//       refer to system_stm32f4xx.c file */
//
//  /* SysTick end of count event each 10ms */
//  RCC_GetClocksFreq(&RCC_Clocks);
//  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
//  
//  /* Add your application code here */
//  /* Insert 50 ms delay */
//  Delay(5);
//  
//  /* Output HSE clock on MCO1 pin(PA8) **************************************/ 
//  /* Enable the GPIOA peripheral */ 
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//  
//  /* Configure MCO1 pin(PA8) in alternate function */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//    
//  /* HSE clock selected to output on MCO1 pin(PA8)*/
//  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
//  
//  
//  /* Output SYSCLK/4 clock on MCO2 pin(PC9) *********************************/ 
//  /* Enable the GPIOACperipheral */ 
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//  
//  /* Configure MCO2 pin(PC9) in alternate function */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//    
//  /* SYSCLK/4 clock selected to output on MCO2 pin(PC9)*/
//  RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_4);
  
  SystemInit();
  SysTick_Config(SystemCoreClock /1000);
  GPIO_Config();
  aLastState = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);
  oldcount = count;
  TM_TIMER_Init();
  TM_PWM_Init();
  GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);
  GoEntry();
  GoExit();
  StopGate();
  /* Infinite loop */
  while (1)
  {
    if(flash_flag == 1)
    {
      Init_Sequence();
      flash_flag = 0;
    }
    if(flash_flag == 0)
      ErrorCheckEncoder();
  }
}

void Motor_Logic(uint8_t x)
{
  if(flash_flag == 0)
  {
    if(x == 0)//clockwise
    {
      Dir_Clkwise();
      position = counter_buf;
      DisEngageBrakes(); //brake set
      TIM_Cmd(TIM4, ENABLE);
//      if((position == enc_pos))
//      {
//        EngageBrakes();//brake reset
//        Delay(200);
//        oldcount = count;
//        TIM_Cmd(TIM4, DISABLE);
//        is_gate_moving = 0;
//      }
      while(position != enc_pos);
      EngageBrakes();//brake reset
      Delay(200);
      oldcount = count;
      TIM_Cmd(TIM4, DISABLE);
      is_gate_moving = 0;
    }
    else if(x == 1)
    {
      Dir_AntiClkwise();
      position = counter_buf;
      DisEngageBrakes(); 
      TIM_Cmd(TIM4, ENABLE);
//      if((position == -enc_pos))
//      {
//        EngageBrakes();
//        Delay(200);//check here
//        oldcount = count;
//        TIM_Cmd(TIM4, DISABLE);
//        is_gate_moving = 0;
//      }
      while(position != -enc_pos);
      EngageBrakes();
      Delay(200);//check here
      oldcount = count;
      TIM_Cmd(TIM4, DISABLE);
      is_gate_moving = 0;
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
  aState = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);

  if (aState != aLastState)
  {     
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) != aLastState) 
    { 
      lastcounter = counter;
      counter ++;//absolute position variable
      position ++;//reference variable
      if((free == 1) && (flash_flag == 0) && (fire_exit == 0))
        counter_buf ++;
    }
    else 
    {
      lastcounter = counter;
      counter --;
      position --;
      if((free == 1) && (flash_flag == 0) && (fire_exit == 0))
        counter_buf --;
    }
    Flash_Data_Write_Buffer[0] = counter;
    WriteToFlash();
  }
  
  aLastState = aState;
  
  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)) == 1)
  {
    FLAG = 1;
  }
  
  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)) == 0)
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
  /**************************** SAME FOR BOTH LEFT AND RIGHT GATE **********************/
  /*reading value of counter from flash memory */
  ReadFromFlash();
  int temp = Flash_Data_Read_Buffer[0];
  is_gate_moving = 1;
  if( temp < -10 )
  {
    direction = 1;
    enc_pos = temp + (temp / 36);//Collectively takes care of every testcase...pls check
    Dir_Clkwise();
    position = enc_pos;
    DisEngageBrakes(); 
    TIM_Cmd(TIM4, ENABLE);
    StopEntry();
    StopExit();
    GoGate();
//    if(position == 0)
//    {
//      EngageBrakes();
//      TIM_Cmd(TIM4, DISABLE);
//      Delay(200);
//      oldcount = count;
//      is_gate_moving = 0;
//      GoEntry();
//      GoExit();
//      StopGate();
//    }
    while(position != 0);
    EngageBrakes();
    TIM_Cmd(TIM4, DISABLE);
    Delay(200);
    oldcount = count;
    is_gate_moving = 0;
    GoEntry();
    GoExit();
    StopGate();
  }
  else if ( temp > 10 )
  {
    direction = 0;
    enc_pos = temp - (temp / 36);
    Dir_AntiClkwise();
    position = enc_pos;
    DisEngageBrakes();
    TIM_Cmd(TIM4, ENABLE);
    StopEntry();
    StopExit();
    GoGate();
//    if(position == 0)
//    {
//      EngageBrakes();
//      TIM_Cmd(TIM4, DISABLE);
//      Delay(200);
//      oldcount = count;
//      is_gate_moving = 0;
//      GoEntry();
//      GoExit();
//      StopGate();
//    }
    while(position != 0);
    EngageBrakes();
    TIM_Cmd(TIM4, DISABLE);
    Delay(200);
    oldcount = count;
    is_gate_moving = 0;
    GoEntry();
    GoExit();
    StopGate();
  }
  
}
/**
  * @brief  Checks for any errors like intrusion of the system
  * @param  None
  * @retval None
  */

void ErrorCheckEncoder(void)
{
  if((free == 0) && (is_gate_moving == 0)); // should not be activated in free mode at all
  {
    if ( oldcount == (count-2) )//2 degrees of tolerance before activating the clutch.. count value only increases. so we can track the magnitude only..
    {
      EngageClutch();
      Delay(3000);
      DisEngageClutch(); 
      oldcount = count;
      enc_pos = enc_pos - 2;// to cancel out theerror caused. MAYBE ERRORS POSSIBLE HERE. TEST THIS
    }
  }
  // check for forceful entry in system while moving
  if((free == 0) && (is_gate_moving == 1) && (direction == 0))
  {
    if(lastcounter < counter) //if at all there is a intrusion
    {
      TIM_Cmd(TIM12, DISABLE);
      EngageClutch(); 
      Delay(3000);
      DisEngageClutch();
      TIM_Cmd(TIM12, ENABLE);
    }
  }
  else if((free == 0) && (is_gate_moving == 1) && (direction == 1))
  {
    if(lastcounter > counter) //if at all there is a intrusion
    {
      TIM_Cmd(TIM12, DISABLE);
      EngageClutch(); 
      Delay(3000);
      DisEngageClutch();
      TIM_Cmd(TIM12, ENABLE);
    }
  }
}

/**
  * @brief  Opens the gate in the exit direction
  * @param  None
  * @retval None
  */

void OpenCloseGates(void)
{
  if(fire_exit == 0)
  {
    direction = 0;
    EngageBrakes();
    position = enc_pos;//from Config_PWM(_,_)
    Dir_AntiClkwise();
    DisEngageBrakes();
    TIM_Cmd(TIM12, ENABLE);
    is_gate_moving = 1;
    StopEntry();
    StopExit();
    GoGate();
//    if(position == -180)
//    {
//      EngageBrakes();
//      TIM_Cmd(TIM4, DISABLE);
//      oldcount = count;
//      is_gate_moving = 0;
//      GoEntry();
//      GoExit();
//      StopGate();
//    }
    while(position != -180);
    EngageBrakes();
    TIM_Cmd(TIM4, DISABLE);
    oldcount = count;
    is_gate_moving = 0;
    GoEntry();
    GoExit();
    StopGate();
    fire_exit = 1;
  }
  else if ( fire_exit == 1 )
  {
    direction = 1;
    EngageBrakes();
    position = -enc_pos;//from Config_PWM(_,_)
    Dir_Clkwise();
    DisEngageBrakes();
    TIM_Cmd(TIM12, ENABLE);
    is_gate_moving = 1;
    StopEntry();
    StopExit();
    GoGate();
//    if(position == 0)
//    {
//      EngageBrakes();
//      TIM_Cmd(TIM4, DISABLE);
//      oldcount = count;
//      is_gate_moving = 0;
//      GoEntry();
//      GoExit();
//      StopGate();
//    }
    while(position != 0 );
    EngageBrakes();
    TIM_Cmd(TIM4, DISABLE);
    oldcount = count;
    is_gate_moving = 0;
    GoEntry();
    GoExit();
    StopGate();
    fire_exit = 0;
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
