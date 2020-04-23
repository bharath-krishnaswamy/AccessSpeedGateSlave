/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Header for main.c module
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "flash.h"
#include "arm_math.h"
void ErrorCheckEncoder(void);
void TM_LEDS_Init(void);
void TM_TIMER_Init(void);
void TM_PWM_Init(void);
void Encoder_Logic(void);
void Motor_Logic(uint8_t x);
void Config_PWM(uint8_t x, uint8_t f);
void GPIO_Config(void);
void Init_Sequence(void);
void OpenCloseGates(void);
void Emergency_PWM(uint8_t s);
void EngageBrakes(void);
void DisEngageBrakes(void);
void EngageClutch(void);
void DisEngageClutch(void);
void Dir_Clkwise(void);
void Dir_AntiClkwise(void);
void StopEntry(void);
void GoEntry(void);
void StopGate(void);
void GoGate(void);
void StopExit(void);
void GoExit(void);
void init_pid(void);
void adjust_pwm(float x);
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern uint8_t rx_array[4];
extern int is_complete;
extern  int enc_pos;
extern  int8_t curr_pos_of_gate;//initial position
//variables for turning the motor in intended direction and also for storing the previous commands to overcome bottlenecks
extern int8_t free,speed,direction ;
extern int flag;
extern int is_gate_moving;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
extern int32_t Flash_Data_Write_Buffer[FLASH_DATA_BUFF_SIZE];
extern int32_t Flash_Data_Read_Buffer[FLASH_DATA_BUFF_SIZE];
extern int width_pwm;
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
