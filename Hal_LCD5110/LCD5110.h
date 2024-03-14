#ifndef LCD5110_ON
#define LCD5110_ON

#include <mcu32_memory_map.h>
#include <power_manager.h>
#include "mik32_hal_spi.h"
#include "mik32_hal_def.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_pad_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>



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
// Предустановки дисплея
#define LCD_BIAS					0x03	    // Диапазон: 0-7 (0x00-0x07) 
#define LCD_TEMP					0x02	    // Диапазон: 0-3 (0x00-0x03)
#define LCD_CONTRAST				0x46	    // Контраст дисплея Диапазон: 0-127 (0x00-0x7F)
// Тип передавыемых данных
#define LCD_COMMAND                 0           // Режим команды
#define LCD_DATA                    1           // Режим данных

#define charOffset                  0x20       // Смещение по таблице символов ASCII     
#define intoffset                   0x10        

#define LCD_XMAX					84	        // Диапазон: 0-7 (0x00-0x07) 
#define LCD_RMAX					6	        // Диапазон: 0-3 (0x00-0x03)

#define LCD_Sleep_S 2
#define LCD_Sleep_M (1 << LCD_Sleep_S)
#define LCD_entryMode_S 1
#define LCD_entryMode_M (1 << LCD_entryMode_S)


typedef enum
{
 display_blank = 0x00,
 normal_mode = 0x04,
 all_segments_on = 0x01,
 inverse_mode = 0x05
} Display_Configuration;


typedef struct 
{
    int8_t x_coord;/* стартовая координата записи по x*/
    int8_t row;/*строка*/
    int8_t offset;/*пробел между буквами*/
    int8_t xS;/*ограничение Xmin*/
    int8_t xE;/*ограничение Xmax*/
    int8_t rS;/*ограничение Ymin*/
    int8_t rE;/*ограничение Ymax*/
    int8_t y_coord;

    uint8_t settings;

    int8_t *buffer;/*указатль на двумерный массив*/

    SPI_HandleTypeDef *Spi;/*указатель на SPI*/
    //пины
    HAL_PinMapTypeDef pinCS;/*выбор устройства*/
    HAL_PinMapTypeDef pinDC;/*пин режима комада/запись в память*/
    HAL_PinMapTypeDef pinRST;/*перезагрузка*/

}LCD5110Struct;



//LCD5110Struct *LCD;


void LCD5110_Init(LCD5110Struct * LCD);

void LCD5110_Coord(LCD5110Struct * LCD, int8_t x, int8_t y);

void LCD5110_Fill(LCD5110Struct * LCD);
void LCD5110_Clear(LCD5110Struct * LCD);



void LCD5110_Clear_Buffer(LCD5110Struct * LCD);
void LCD5110_Fill_Buffer(LCD5110Struct * LCD);
void LCD5110_Buffer_Update(LCD5110Struct * LCD);

void LCD5110_Set_Pixel(LCD5110Struct * LCD, int8_t X, int8_t Y);

void LCD5110_Print_Char(LCD5110Struct * LCD, char Char_Data[]);
void LCD5110_Print_Int(LCD5110Struct * LCD, int Int_Data);
void LCD5110_Print_Float(LCD5110Struct * LCD, float Float_Data, uint8_t after_point);
void LCD5110_Print_mas(LCD5110Struct * LCD, uint8_t  *mas, uint8_t x, uint16_t y);
void LCD5110_Delete_mas(LCD5110Struct *LCD,uint8_t  *mas, uint8_t x_, uint16_t size_);

void LCD5110_Sleep(LCD5110Struct* LCD);
void LCD5110_Wake(LCD5110Struct* LCD);



/////////////////////////////// не используется пользователем /////////////////////////////////////////////////////
HAL_StatusTypeDef LCD5110_Advanced_Settings(LCD5110Struct * LCD);
HAL_StatusTypeDef LCD5110_Temperature_Correction(LCD5110Struct * LCD, uint8_t TC);
HAL_StatusTypeDef LCD5110_Level_Offset(LCD5110Struct * LCD, uint8_t Level);
HAL_StatusTypeDef LCD5110_Setting_Contrast(LCD5110Struct * LCD, uint8_t contrast);
HAL_StatusTypeDef LCD5110_Usually_Settings(LCD5110Struct * LCD);
HAL_StatusTypeDef LCD5110_Inverse_Mode(LCD5110Struct * LCD, Display_Configuration DisCon);
HAL_StatusTypeDef LCD5110_Set_X_coord(LCD5110Struct * LCD, int8_t X);
HAL_StatusTypeDef LCD5110_Set_Y_coord(LCD5110Struct * LCD, int8_t Y);
void LCD5110_Coord_SPI(LCD5110Struct * LCD, int8_t x, int8_t y);
HAL_StatusTypeDef LCD5110_Write(LCD5110Struct *LCD, unsigned char data, unsigned char mode);
void LCD5110_SPI_ON(LCD5110Struct * LCD);
void LCD5110_SPI_OFF(LCD5110Struct * LCD);


#endif