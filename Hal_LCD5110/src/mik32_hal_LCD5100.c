#include "mik32_hal_LCD5100.h"
#include "mik32_hal_lcd5110_bitmaps.h"

HAL_StatusTypeDef HAL_LCD5110_Write_(LCD5110_HandleTypeDef* LCD, uint8_t data[], uint32_t DataSize, uint8_t mode){
    uint8_t input[DataSize];
    HAL_GPIO_WritePin(LCD->pinDC.GPIO ,LCD->pinDC.PIN.Pin, mode);
    HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(LCD->Spi, data , input, DataSize, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status != HAL_OK)
    {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, LCD->Spi->ErrorCode & HAL_SPI_ERROR_OVR, LCD->Spi->ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(LCD->Spi);
    }
    return SPI_Status;
}

HAL_StatusTypeDef HAL_lcd5110_MspInit(LCD5110_HandleTypeDef* LCD){
    // инициализируем порты
    if(LCD->pinCS.GPIO != NULL){
        if (HAL_GPIO_Init(LCD->pinCS.GPIO, &LCD->pinCS.PIN) != HAL_OK){
            xprintf("Pin_CS_Error\n");
            return HAL_ERROR;
        }
    }

    if (HAL_GPIO_Init(LCD->pinRST.GPIO, &LCD->pinRST.PIN) != HAL_OK){
        xprintf("Pin_RST_Error\n");
        return HAL_ERROR;
    }
    
    if (HAL_GPIO_Init(LCD->pinDC.GPIO, &LCD->pinDC.PIN) != HAL_OK){
        xprintf("Pin_DC_Error\n");
        return HAL_ERROR;
    }

    // инициализируеми SPI
    return HAL_OK;
}
void HAL_LCD5110_Fill(LCD5110_HandleTypeDef* LCD){
    uint8_t mas[84];
    for(uint8_t i = 0; i < 84; ++i){
        mas[i] = 0xff;
    }
    for(uint8_t i = 0; i < 6; ++i){
        if(HAL_LCD5110_Write_(LCD, mas, 84, LCD_DATA)!= HAL_OK){
        xprintf("No writ settings\n");
        }
    }
}
void HAL_LCD5110_Clear(LCD5110_HandleTypeDef* LCD){
    uint8_t mas[DEFAULT_XMAX];
    for(uint8_t i = 0; i < DEFAULT_XMAX; ++i){
        mas[i] = 0x00;
    }
    for(uint8_t i = 0; i < DEFAULT_ROWMAX; ++i){
        if(HAL_LCD5110_Write_(LCD, mas, DEFAULT_XMAX, LCD_DATA)!= HAL_OK){
        xprintf("No writ settings\n");
        }
    }
}
void HAL_LCD5110_Reset(LCD5110_HandleTypeDef* LCD){
    HAL_GPIO_WritePin(LCD->pinRST.GPIO ,LCD->pinRST.PIN.Pin, GPIO_PIN_LOW);
    for (volatile int i = 0; i < 100000; i++);
    HAL_GPIO_WritePin(LCD->pinRST.GPIO ,LCD->pinRST.PIN.Pin, GPIO_PIN_HIGH);
}

void HAL_LCD5110_DefoltOption(LCD5110_HandleTypeDef* LCD){
    LCD->option.Offset  =   DEFAULT_OFFSET;
    LCD->option.Row     =   DEFAULT_ROW;
    LCD->option.Rowmax  =   DEFAULT_ROWMAX;
    LCD->option.Rowmin  =   DEFAULT_ROWMIN;
    LCD->option.Xcoord  =   DEFAULT_XCOORD;
    LCD->option.Xmax    =   DEFAULT_XMAX;
    LCD->option.Xmin    =   DEFAULT_XMIN;
    LCD->option.Yoffset =   DEFAULT_YOFFSET;
}

HAL_StatusTypeDef HAL_LCD5110_Init(LCD5110_HandleTypeDef* LCD){
    if(LCD == NULL){
        xprintf(" = NULL\n");
        return HAL_ERROR;
    }
    if(LCD->Spi->Instance != SPI_0 && LCD->Spi->Instance != SPI_1 ){
        xprintf("No SPI\n");
        return HAL_ERROR;
    }
    // /*инициализмрую порты ввода вывода и spi*/
    if(HAL_lcd5110_MspInit(LCD)!= HAL_OK){
        xprintf("No init port\n");
        return HAL_ERROR;
    }
    // /*задаем начальные значения переменным*/
    HAL_LCD5110_DefoltOption(LCD);
    // /*перезагружыю дисплей*/
    HAL_LCD5110_Reset(LCD);
    uint8_t mas_comand[8];  
    mas_comand[0] = 0b00100001;
    mas_comand[1] = 0b00000100;
    mas_comand[2] = 0b00010011;
    mas_comand[3] = 0b11000001;
    mas_comand[4] = 0b00100000;
    mas_comand[5] = 0b00001100;
    mas_comand[6] = 0b01000000;
    mas_comand[7] = 0b10000000;
    if(HAL_LCD5110_Write_(LCD, mas_comand, 8, LCD_COMMAND)!= HAL_OK){
        xprintf("No writ settings\n");
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_LCD5110_PrintMas(LCD5110_HandleTypeDef* LCD,  uint8_t * data, uint32_t sizedata){

    if(HAL_LCD5110_Write_(LCD, data, sizedata,LCD_DATA)!= HAL_OK){
        xprintf("No writ settings\n");
        return HAL_ERROR;
    }
    return HAL_OK;
}

void LCD5110_Print_char(LCD5110_HandleTypeDef* LCD, char a){
    HAL_LCD5110_Write_(LCD, Char_font1[a - charOffset], 5, LCD_DATA);
}
void LCD5110_Print_int(LCD5110_HandleTypeDef* LCD, int a){
    int buffer = a;
    while(buffer > 0){
        HAL_LCD5110_Write_(LCD, Char_font1[intOffset + buffer%10], 5, LCD_DATA);
        buffer/=10;
    }
}

void String_to_Display(LCD5110_HandleTypeDef* LCD, char * format, ...){
    int d; 
    char c;
    va_list factor;         // указатель на необязательный параметр
    va_start(factor, format);   // устанавливаем указатель
    for(;*format!='\0' ; ++format)
    {
        c = *format;
        if(c!='%')
        {
            LCD5110_Print_char(LCD, c);
            continue;
        }
        c = *++format;
        if(c == 'd'){
            d = va_arg(factor, int);
            LCD5110_Print_int(LCD, d);
            continue;
        }
    }
    va_end(factor);
}