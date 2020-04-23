/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.2.0RC2
  * @date    20-February-2013
  * @brief   Header for main.c module
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "flash.h"

void ErrorCheckEncoder(void);
void TM_LEDS_Init(void);
void TM_TIMER_Init(void);
void TM_PWM_Init(void);
void Encoder_Logic(void);
void Motor_Logic(uint8_t x);
void Config_PWM(uint8_t x);
void GPIO_Config(void);
void Init_Sequence(void);
void OpenGates(void);
void Emergency_PWM(uint8_t s);
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
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
