#pragma once 

#include "mik32_hal_spi.h"
#include "mik32_hal_gpio.h"
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include "uart_lib.h"
#include "xprintf.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_COMMAND                 0           // Режим команды
#define LCD_DATA                    1           // Режим данных
#define LCD_PRINT                   1           // печатать
#define LCD_DELETE                  0           // удалить
#define LCD_PRINT_TEXT              1           // напечатать текст
#define LCD_PRINT_MAS               0           // вывести массив
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEFAULT_OFFSET               1           // расстояние между симвотами при пробеле
#define DEFAULT_XMIN                 0           // минимальная координата по x слева от экрана
#define DEFAULT_XMAX                 84          // максимальная координата по x справа от экрана
#define DEFAULT_ROWMAX               5           // максимальная координата по y снизу экрана
#define DEFAULT_ROWMIN               0           // минимальная координата по y сверху экрана
#define DEFAULT_XCOORD               0           //
#define DEFAULT_YOFFSET              0           //
#define DEFAULT_ROW                  0           //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define charOffset                  0x20         // Смещение по таблице символов ASCII   
#define intOffset                   0x10            
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define max_contrast                0x7F
#define min_contrast                0x00
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Режим настройки дисплея
#define PCD8544_FUNCTIONSET			0x20		// Выбор режима питания, адресации и набора инструкций 
#define PCD8544_POWERDOWN			0x02		// Режим сна
#define PCD8544_HORIZONTADD			0x01		// Вертикальная адресация
#define PCD8544_EXTENDEDINSTRUCTION	0x01		// Расширенный набор инструкций
// Режимы работы дисплея
#define PCD8544_DISPLAYBLANK		0x00		// Режим пустого дисплея
#define PCD8544_DISPLAYNORMAL		0x04		// Нормальный режим работы
#define PCD8544_DISPLAYALLON		0x01		// Заполнить все сегменты
#define PCD8544_DISPLAYINVERTED		0x05		// Режим инверсии изображения
// Стандартный набор инструкций
#define PCD8544_DISPLAYCONTROL		0x08		// Конфигурация дисплея
#define PCD8544_SETYADDR			0x40		// Задать адрес по оси Y
#define PCD8544_SETXADDR			0x80		// Задать адрес по оси X
// Расширенный набор инструкций
#define PCD8544_SETTEMP				0x04		// Колибровка температуры
#define PCD8544_SETBIAS				0x10		// Утсановка BIAS
#define PCD8544_SETVOP				0x80		// Установка контраста
 

typedef enum{
 display_blank = 0x00,
 normal_mode = 0x04,
 all_segments_on = 0x01,
 inverse_mode = 0x05
} Display_Configuration;

typedef enum{
 TC_0 = 0x00,
 TC_1 = 0x01,
 TC_2 = 0x02,
 TC_3 = 0x03
} Temperature_Coefficient;

typedef enum{
 BS0 = 0x01,
 BS1 = 0x02,
 BS3 = 0x04,
} Bias_System;

typedef enum{
 horizontal_addressing = 0,
 vertical_addressing = 1
} Entry_Mode;

typedef enum{
    chip_active = 0,
    chip_power_down = 1
} Power_Down_Control;

typedef enum{
    transfer_off                = 0b00000000,
    transfer_x_on               = 0b00000001,
    transfer_y_on               = 0b00000010,
    transfer_x_and_step_y       = 0b00000100,
} Transfer_When_End;



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

typedef struct __HAL_LCD5110_Settings{
    Display_Configuration display;/*режим рабюоты дисплея*/
    Temperature_Coefficient temperature;/*установка температурного коэфффицента*/
    Bias_System bias;
    Entry_Mode entry;
    Power_Down_Control control;
    uint8_t contrast;
}HAL_LCD5110_Settings;

typedef struct __HAL_LCD5110_Port{
    GPIO_TypeDef *GPIO; 
    GPIO_InitTypeDef PIN;
}HAL_LCD5110_Port;

typedef struct LCD5110_HandleTypeDef
{
    HAL_LCD5110_Option option;
    HAL_LCD5110_Settings settings;
    Transfer_When_End transfer;
    uint8_t *buffer;/* указатль на массив*/
    SPI_HandleTypeDef *Spi;/* указатель на SPI*/
    HAL_LCD5110_Port pinCS;/* выбор устройства*/
    HAL_LCD5110_Port pinDC;/* пин режима комада/запись в память*/
    HAL_LCD5110_Port pinRST;/* перезагрузка*/

}LCD5110_HandleTypeDef;

/// @brief 
/// @param LCD 
/// @param data 
/// @param DataSize 
/// @param mode 
/// @return 
HAL_StatusTypeDef HAL_LCD5110_Write_(LCD5110_HandleTypeDef* LCD, uint8_t data[], uint32_t DataSize, uint8_t mode);

/// @brief 
/// @param LCD 
/// @return 
HAL_StatusTypeDef HAL_LCD5110_Reset(LCD5110_HandleTypeDef* LCD);

/// @brief 
/// @param LCD 
/// @return 
HAL_StatusTypeDef HAL_LCD5110_Init(LCD5110_HandleTypeDef* LCD);

/// @brief 
/// @param LCD 
/// @param x 
/// @param row 
/// @param  
void HAL_LCD5110_Set_Coord(LCD5110_HandleTypeDef* LCD, uint8_t x, uint8_t row, ...);

/// @brief 
/// @param LCD 
/// @param x_min 
/// @param row_min 
/// @param x_max 
/// @param row_max 
void HAL_LCD5110_Set_Boundaries(LCD5110_HandleTypeDef* LCD, uint8_t x_min, uint8_t row_min, uint8_t x_max, uint8_t row_max);

/// @brief 
/// @param LCD 
void HAL_LCD5110_updete(LCD5110_HandleTypeDef* LCD);

/// @brief 
/// @param LCD 
void HAL_LCD5110_Fill(LCD5110_HandleTypeDef* LCD);

/// @brief 
/// @param LCD 
void HAL_LCD5110_Clear(LCD5110_HandleTypeDef* LCD);

/// @brief 
/// @param LCD 
/// @param str 
/// @param  
void HAL_LCD5110_Print(LCD5110_HandleTypeDef* LCD, char * str, ...);

/// @brief 
/// @param LCD 
/// @param str 
/// @param  
void HAL_LCD5110_Delete(LCD5110_HandleTypeDef* LCD, char * str, ...);

void HAL_LCD5110_Print_mas(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t columns, uint16_t size_mas);
