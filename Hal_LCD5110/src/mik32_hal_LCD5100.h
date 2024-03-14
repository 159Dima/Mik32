#pragma once 

#include "mik32_hal_spi.h"
#include "mik32_hal_gpio.h"
#include <malloc.h>
#include <stdarg.h>
#include "uart_lib.h"
#include "xprintf.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_COMMAND                 0           // Режим команды
#define LCD_DATA                    1           // Режим данных
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEFAULT_OFFSET               1           // расстояние между симвотами при пробеле
#define DEFAULT_XMIN                 0           // минимальная координата по x слева от экрана
#define DEFAULT_XMAX                 84          // максимальная координата по x справа от экрана
#define DEFAULT_ROWMAX               6           // максимальная координата по y снизу экрана
#define DEFAULT_ROWMIN               0           // минимальная координата по y сверху экрана
#define DEFAULT_XCOORD               0           //
#define DEFAULT_YOFFSET              0           //
#define DEFAULT_ROW                  0           //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define charOffset                  0x20         // Смещение по таблице символов ASCII   
#define intOffset                   0x10            
 

typedef struct __HAL_LCD5110_Option{
    int8_t Xcoord;/* стартовая координата записи по x*/
    int8_t Yoffset;/* смещение по y*/
    int8_t Row;/* строка*/
    int8_t Offset;/*пробел между буквами*/
    int8_t Xmin;/*ограничение Xmin*/
    int8_t Xmax;/*ограничение Xmax*/
    int8_t Rowmin;/*ограничение Ymin*/
    int8_t Rowmax;/*ограничение Ymax*/
}HAL_LCD5110_Option;

typedef struct __string_parametor{
    uint8_t * statr_str;
    uint32_t size_str;
}string_parametor;

typedef enum __HAL_LCD5110_Addressing
{
	HAL_LCD5110_Horizonta = 0b00, /**< горизонтальная аддресация дисплея. */
	HAL_LCD5110_Vertical = 0b10, /**< вертикальная аддресация дисплея. */
}HAL_LCD5110_Addressing;

typedef struct __HAL_LCD5110_Port{
    GPIO_TypeDef *GPIO; 
    GPIO_InitTypeDef PIN;
}HAL_LCD5110_Port;

typedef struct LCD5110_HandleTypeDef
{
    HAL_LCD5110_Option option;
    HAL_LCD5110_Addressing addressing;
    int8_t *buffer;/* указатль на массив*/
    SPI_HandleTypeDef *Spi;/* указатель на SPI*/
    HAL_LCD5110_Port pinCS;/* выбор устройства*/
    HAL_LCD5110_Port pinDC;/* пин режима комада/запись в память*/
    HAL_LCD5110_Port pinRST;/* перезагрузка*/

}LCD5110_HandleTypeDef;


HAL_StatusTypeDef HAL_LCD5110_Init(LCD5110_HandleTypeDef* LCD);
void String_to_Display(LCD5110_HandleTypeDef* LCD, char * format, ...);
HAL_StatusTypeDef HAL_LCD5110_PrintMas(LCD5110_HandleTypeDef* LCD,  uint8_t* data, uint32_t sizedata);
void HAL_LCD5110_Fill(LCD5110_HandleTypeDef* LCD);
void HAL_LCD5110_Clear(LCD5110_HandleTypeDef* LCD);