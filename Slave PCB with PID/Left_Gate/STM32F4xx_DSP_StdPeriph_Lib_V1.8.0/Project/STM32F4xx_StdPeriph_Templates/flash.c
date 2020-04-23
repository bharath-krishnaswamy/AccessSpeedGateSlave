#include "stm32f4xx_flash.h"
#include "flash.h"

uint32_t uwStartSector = 0;
uint32_t uwEndSector = 0;
uint32_t uwAddress = 0;
uint32_t uwSectorCounter = 0;

__IO uint32_t uwData32 = 0;
__IO uint32_t uwMemoryProgramStatus = 0;

int32_t Flash_Data_Read_Buffer[FLASH_DATA_BUFF_SIZE];
int32_t Flash_Data_Write_Buffer[FLASH_DATA_BUFF_SIZE];
void WriteToFlash(void)
{
  uwAddress = 0;
  /* Unlock the Flash *********************************************************/
  FLASH_Unlock();
  
  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
  
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
  
  /* Get the number of the start and end sectors */
  uwStartSector = FLASH_Sector_4;
  uwEndSector = FLASH_Sector_5;
  
  /* Strat the erase operation */
  uwSectorCounter = uwStartSector;
  while (uwSectorCounter <= uwEndSector) 
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */ 
    if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
    { 
      /* Error occurred while sector erase. 
      User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
    /* jump to the next sector */
    if (uwSectorCounter == FLASH_Sector_11)
    {
      uwSectorCounter += 40;
    } 
    else 
    {
      uwSectorCounter += 8;
    }
  }
  
  /* Program the user Flash area word by word ********************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
  
  uwAddress = FLASH_USER_START_ADDR;
 
  for(uint8_t index=0;index<FLASH_DATA_BUFF_SIZE;index++)
  {  
    if (uwAddress < FLASH_USER_END_ADDR)
    {
      if (FLASH_ProgramWord(uwAddress, Flash_Data_Write_Buffer[index]) == FLASH_COMPLETE)
      {
        uwAddress = uwAddress + 4;
      }
      else
      { 
        /* Error occurred while writing data in Flash memory. 
        User can add here some code to deal with this error */
        while (1)
        {
        }
      }
    }
    
  }
  
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock();
}

void ReadFromFlash(void)
{
  uwAddress = 0;
  
  /* Unlock the Flash to enable the flash control register access *************/   
  FLASH_Unlock();
  
  /* Clear pending flags (if any) */  
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
  
  uwAddress = FLASH_USER_START_ADDR;  
  
  for(uint8_t index=0;index<FLASH_DATA_BUFF_SIZE;index++)
  {
    if (uwAddress <= FLASH_USER_END_ADDR)
    {
      // Data is copied to "Flash_Data_Read_Buffer"
      Flash_Data_Read_Buffer[index] = *(__IO uint32_t *)uwAddress;
      uwAddress = uwAddress + 4;
    }
  }
  FLASH_Lock();
}
