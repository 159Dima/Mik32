#ifndef MCU32_HAL_WDT
#define MCU32_HAL_WDT

#include "mcu32_memory_map.h"
#include "wdt.h"
#include "wdt_bus.h"
#include "power_manager.h"
#include "mik32_hal_def.h"
#include "stddef.h"

#define CLOCK32K 32768
#define CLOCK32M 32000000
#define MAX_TIME_LOAD 100

typedef enum{
    PRESCALE_1,
    PRESCALE_2,
    PRESCALE_4,
    PRESCALE_16,
    PRESCALE_64,
    PRESCALE_255,
    PRESCALE_1024,
    PRESCALE_4096
}Frequency_Divider;

typedef enum{
    OSC32M,
    HSI32M,
    OSC32K,
    HSI32K
}WDT_Clocks;


typedef struct
{
    WDT_Clocks Clock; //назначение источника тактирования
    
    Frequency_Divider Prescaler; // ДЕЛИТЕЛЬ wdt PRESCALE_x = 1/2/4/16/64/255/1024/4096

    uint16_t TimeOut;

    uint32_t Reload; // время перезагрузки WDT

}WDT_InitTypeDef;


HAL_StatusTypeDef HAL_WDT_Init(WDT_InitTypeDef *WDT_);
HAL_StatusTypeDef HAL_WDT_Refresh();
HAL_StatusTypeDef HAL_WDT_Stop();
HAL_StatusTypeDef HAL_WDT_Start();

uint8_t Wdt_enabled();
uint8_t Wdt_reset_flag();
uint8_t Wdt_loading();
uint16_t WDT_time();

#endif