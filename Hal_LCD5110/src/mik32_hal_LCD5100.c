#include "mik32_hal_LCD5100.h"
#include "mik32_hal_lcd5110_bitmaps.h"

///////////////////////////////////////////функция работы с spi//////////////////////////////////////////////////////////

HAL_StatusTypeDef HAL_LCD5110_Write(LCD5110_HandleTypeDef* LCD, uint8_t data[], uint32_t DataSize, uint8_t mode){
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

/////////////////////////////////////////инициализация дисплея////////////////////////////////////////////////////////////

HAL_StatusTypeDef _lcd5110_MspInit(LCD5110_HandleTypeDef* LCD){
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

void _LCD5110_Reset(LCD5110_HandleTypeDef* LCD){
    HAL_GPIO_WritePin(LCD->pinRST.GPIO ,LCD->pinRST.PIN.Pin, GPIO_PIN_LOW);
    for (volatile int i = 0; i < 100000; i++);
    HAL_GPIO_WritePin(LCD->pinRST.GPIO ,LCD->pinRST.PIN.Pin, GPIO_PIN_HIGH);
}

void _LCD5110_DefoltOption(LCD5110_HandleTypeDef* LCD){
    LCD->option.Offset  =   DEFAULT_OFFSET;
    LCD->option.Row     =   DEFAULT_ROW;
    LCD->option.Rowmax  =   DEFAULT_ROWMAX;
    LCD->option.Rowmin  =   DEFAULT_ROWMIN;
    LCD->option.Xcoord  =   DEFAULT_XCOORD;
    LCD->option.Xmax    =   DEFAULT_XMAX;
    LCD->option.Xmin    =   DEFAULT_XMIN;
    LCD->option.Yoffset =   DEFAULT_YOFFSET;
}

void _LCD5110_DefoltSettings(LCD5110_HandleTypeDef* LCD){
    LCD->settings.bias = 0;
    LCD->settings.control = chip_active;
    LCD->settings.display = normal_mode;
    LCD->settings.temperature = TC_0;
    LCD->settings.entry = horizontal_addressing;
    LCD->settings.contrast = 80;
}

HAL_StatusTypeDef _LCD5110_Basic_Instruction(LCD5110_HandleTypeDef* LCD){
    uint8_t mas_instruction[4];
    mas_instruction[0] = PCD8544_FUNCTIONSET | LCD->settings.entry | LCD->settings.control;
    mas_instruction[1] = PCD8544_DISPLAYCONTROL | LCD->settings.display;
    mas_instruction[2] = PCD8544_SETXADDR | LCD->option.Xcoord;
    mas_instruction[3] = PCD8544_SETYADDR | LCD->option.Row;
   if(HAL_LCD5110_Write(LCD, mas_instruction, 4, LCD_COMMAND)!= HAL_OK){
        xprintf("No basic instruction set\n");
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef _LCD5110_Extended_Instruction(LCD5110_HandleTypeDef* LCD){
    uint8_t mas_instruction[5];
    if (LCD->settings.contrast > max_contrast){
        LCD->settings.contrast = max_contrast;
    } else if(LCD->settings.contrast < min_contrast){
        LCD->settings.contrast = min_contrast;
    }
    mas_instruction[0] = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION | LCD->settings.entry | LCD->settings.control;
    mas_instruction[1] = PCD8544_SETTEMP | LCD->settings.temperature;
    mas_instruction[2] = PCD8544_SETBIAS | LCD->settings.bias;
    mas_instruction[3] = PCD8544_SETVOP | LCD->settings.contrast;
    mas_instruction[4] = PCD8544_FUNCTIONSET | LCD->settings.entry | LCD->settings.control;
   if(HAL_LCD5110_Write(LCD, mas_instruction, 5, LCD_COMMAND)!= HAL_OK){
        xprintf("No extended instruction set\n");
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_LCD5110_Reset(LCD5110_HandleTypeDef* LCD){
    /*задаем начальные значения переменным*/
    _LCD5110_DefoltOption(LCD);
    _LCD5110_DefoltSettings(LCD);
    /*перезагружыю дисплей*/
    _LCD5110_Reset(LCD);
    // доносим до контроллера настройки
    if(_LCD5110_Extended_Instruction(LCD)!= HAL_OK){
        xprintf("Stop LCD5110 init\n");
        return HAL_ERROR;
    }
    if(_LCD5110_Basic_Instruction(LCD)!= HAL_OK){
        xprintf("Stop LCD5110 init\n");
        return HAL_ERROR;
    }
    HAL_LCD5110_Clear(LCD);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_LCD5110_Init(LCD5110_HandleTypeDef* LCD){
    if(LCD == NULL){
        xprintf("struct == NULL\nStop LCD5110 init\n");
        return HAL_ERROR;
    }
    if(LCD->Spi->Instance != SPI_0 && LCD->Spi->Instance != SPI_1 ){
        xprintf("No SPI\nStop LCD5110 init\n");
        return HAL_ERROR;
    }
    // /*инициализмрую порты ввода вывода и spi*/
    if(_lcd5110_MspInit(LCD)!= HAL_OK){
        xprintf("Stop LCD5110 init\n");
        return HAL_ERROR;
    }
    
    if(HAL_LCD5110_Reset(LCD)!= HAL_OK){
        xprintf("no rese\n");
        return HAL_ERROR;
    }

    return HAL_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void LCD5110_Sleep(LCD5110Struct* LCD)
// {
//     LCD->settings|=PCD8544_POWERDOWN;
//     //LCD5110_Clear(LCD);
//     LCD5110_SPI_ON(LCD);
//     LCD5110_Inverse_Mode(LCD, display_blank);
//     LCD5110_Usually_Settings(LCD);
//     LCD5110_SPI_OFF(LCD);
// }
 
// void LCD5110_Wake(LCD5110Struct* LCD)
// {
//     LCD->settings&=~PCD8544_POWERDOWN;
//     LCD5110_SPI_ON(LCD);
//     LCD5110_Inverse_Mode(LCD, normal_mode);
//     LCD5110_Usually_Settings(LCD);
//     LCD5110_SPI_OFF(LCD);
// }

void _Check_X_Coord(LCD5110_HandleTypeDef* LCD){
    if (LCD->option.Xcoord > LCD->option.Xmax){
        LCD->option.Xcoord = LCD->option.Xmin;
        //++LCD->option.Row; 
        // xprintf("violation of the boundaries of the array x = 0; ++Row\n");
    }else if(LCD->option.Xcoord < LCD->option.Xmin){
        LCD->option.Xcoord = LCD->option.Xmax;
        // xprintf("violation of the boundaries of the array x = %d;\n" , LCD->option.Xcoord);
    }
}

void _Check_Row_Coord(LCD5110_HandleTypeDef* LCD){
    if (LCD->option.Row > LCD->option.Rowmax){
        LCD->option.Row = LCD->option.Rowmin;
        // xprintf("violation of the boundaries of the array Row = 0;\n");
    }else if(LCD->option.Row < LCD->option.Rowmin){
        LCD->option.Row = LCD->option.Rowmax;
        // xprintf("violation of the boundaries of the array Row = 0;\n");
    }
}

void _Check_Y_Offset(LCD5110_HandleTypeDef* LCD){
    if(LCD->option.Yoffset > 7){
        LCD->option.Yoffset = 7;
        // xprintf("Yoffset > 7 -> Yoffset = 0\n");
    } else if(LCD->option.Yoffset < -7){
        LCD->option.Yoffset = -7;
        // xprintf("Yoffset < 0 -> Yoffset = 0\n");
    }
}

void _Write_Coord_LCD5110(LCD5110_HandleTypeDef* LCD){
    uint8_t mas_instruction[2];
    mas_instruction[0] = PCD8544_SETXADDR | LCD->option.Xcoord;
    mas_instruction[1] = PCD8544_SETYADDR | LCD->option.Row;
    if(HAL_LCD5110_Write(LCD, mas_instruction, 2, LCD_COMMAND)!= HAL_OK){
        xprintf("No set coord\n");
    }
}


void HAL_LCD5110_Set_Coord(LCD5110_HandleTypeDef* LCD, uint8_t x, uint8_t row, ...){
    va_list variables;
    va_start(variables, row);
    LCD->option.Xcoord = x;
    LCD->option.Row = row;
    // xprintf("x = %d, y = %d\n", LCD->option.Xcoord, LCD->option.Row);
    _Check_X_Coord(LCD);
    _Check_Row_Coord(LCD);
     
    if(LCD->buffer == NULL){
        xprintf("x = %d, y = %d\n", LCD->option.Xcoord, LCD->option.Row);
        _Write_Coord_LCD5110(LCD);
        return;
    }
    LCD->option.Yoffset = va_arg(variables, int);
    _Check_Y_Offset(LCD);
    va_end(variables);
    xprintf("x = %d, y = %d Y offset = %d\n", LCD->option.Xcoord, LCD->option.Row , LCD->option.Yoffset);

}

void _Checking_Boundaries(LCD5110_HandleTypeDef* LCD){
    if (LCD->option.Xmax > DEFAULT_XMAX || LCD->option.Xmax < DEFAULT_XMIN){
        LCD->option.Xmax = DEFAULT_XMAX;
        // xprintf("LCD boundaries xmax error, boundaries x = DEFAULT_XMAX \n");
    }
    if (LCD->option.Xmin > DEFAULT_XMAX || LCD->option.Xmin < DEFAULT_XMIN){
        LCD->option.Xmin = DEFAULT_XMIN;
        // xprintf("LCD boundaries xmin error, boundaries x = DEFAULT_XMIN \n");
    }
        if (LCD->option.Xmin >= LCD->option.Xmax ){
        LCD->option.Xmin = DEFAULT_XMIN;
        LCD->option.Xmax = DEFAULT_XMAX;
        // xprintf("xmin >= xmax !!, Xmin = DEFAULT_XMI, Xmax = DEFAULT_XMAX \n");
    }
    if (LCD->option.Rowmax > DEFAULT_ROWMAX || LCD->option.Rowmax < DEFAULT_ROWMIN ){
        LCD->option.Rowmax = DEFAULT_ROWMAX;
        // xprintf("LCD boundaries row error, boundaries row = DEFAULT_ROWMAX \n");
    }
    if (LCD->option.Rowmin > DEFAULT_ROWMAX || LCD->option.Rowmin < DEFAULT_ROWMIN ){
        LCD->option.Rowmin = DEFAULT_ROWMIN;
        // xprintf("LCD boundaries row error, boundaries row = DEFAULT_ROWMIN \n");
    }
    if (LCD->option.Rowmin >= LCD->option.Rowmax ){
        LCD->option.Rowmax = DEFAULT_ROWMAX;
        LCD->option.Rowmin = DEFAULT_ROWMIN;
        // xprintf("Rowmin >= Rowmax !! Rowmax = DEFAULT_ROWMAX, Rowmin = DEFAULT_ROWMIN\n");
    }
}

void HAL_LCD5110_Set_Boundaries(LCD5110_HandleTypeDef* LCD, uint8_t x_min, uint8_t row_min, uint8_t x_max, uint8_t row_max){
    LCD->option.Rowmax = row_max;
    LCD->option.Rowmin = row_min;
    LCD->option.Xmax = x_max;
    LCD->option.Xmin = x_min;
    _Checking_Boundaries(LCD);
    HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row, LCD->option.Yoffset);
}


/////////////////////////////////////////////////////////////////основные действия с буфером//////////////////////////////////////////////////////////////////////////////////

void HAL_LCD5110_updete(LCD5110_HandleTypeDef* LCD){
    if(LCD->buffer!=NULL){
        for(uint8_t i = 0 ; i < 6 ; ++i){
            HAL_LCD5110_Set_Coord(LCD, LCD->option.Xmin, LCD->option.Rowmin + i);
            HAL_LCD5110_Write(LCD, (LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord),LCD->option.Xmax - LCD->option.Xmin, LCD_DATA);
        }
        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xmin, LCD->option.Rowmin);
    }
}

void HAL_LCD5110_Fill(LCD5110_HandleTypeDef* LCD){
    if(LCD->buffer == NULL){
        uint8_t mas[DEFAULT_XMAX];
        for(uint8_t i = 0; i < DEFAULT_XMAX; i++){
            mas[i] = 0xff;
        }
        for(uint8_t i = 0; i <= DEFAULT_ROWMAX; ++i){
            if(HAL_LCD5110_Write(LCD, mas, DEFAULT_XMAX, LCD_DATA)!= HAL_OK){
            xprintf("error LCD Fill\n");
            }
        }
    }else{
        for (size_t i = 0; i <= DEFAULT_ROWMAX ; i++)
        {
            for (int8_t j = 0; j <= DEFAULT_XMAX ; j++)
            {
                *(LCD->buffer + i*DEFAULT_XMAX + j) = 0xff;
            }
        } 
        HAL_LCD5110_updete(LCD);
    }
}

void HAL_LCD5110_Clear(LCD5110_HandleTypeDef* LCD){
    if(LCD->buffer == NULL){
        uint8_t mas[DEFAULT_XMAX] = {0};
        for(uint8_t i = 0; i <= DEFAULT_ROWMAX; ++i){
            if(HAL_LCD5110_Write(LCD, mas, DEFAULT_XMAX, LCD_DATA)!= HAL_OK){
            xprintf("error LCD Clear\n");
            }
        }
    }else{
        for (size_t i = 0; i <= DEFAULT_ROWMAX ; i++)
        {
            for (int8_t j = 0; j <= DEFAULT_XMAX ; j++)
            {
                *(LCD->buffer + i*DEFAULT_XMAX + j) = 0x00;
            }
        } 
        HAL_LCD5110_updete(LCD);
    }
}

//////////////////////////////////////////////////////////////функции связанные с выводом на дисплей//////////////////////////////////////////////////////////////////////////////////

void _work_with_LCD_print_row(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t size_mas, uint8_t p_or_d){
    if(LCD->buffer == 0){
        HAL_LCD5110_Write( LCD, mas, size_mas, LCD_DATA);
    }else{
        uint8_t start_x = LCD->option.Xcoord;
        uint8_t start_row = LCD->option.Row;
        xprintf("start print\n");
        for(;LCD->option.Xcoord < start_x + size_mas; ++LCD->option.Xcoord){
            //HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row, LCD->option.Yoffset);
            if(LCD->option.Yoffset == 0){
                if(p_or_d == LCD_PRINT){  
                    *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) |= *mas ;
                }
                else{
                    *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) &= ~(*mas);
                }
            }
            else{
                xprintf("smeshenie\n");
                if(LCD->option.Yoffset > 0){
                    if(p_or_d == LCD_PRINT){
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) |= *mas << LCD->option.Yoffset;
                        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row + 1, LCD->option.Yoffset);
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) |= *mas  >> (8 - LCD->option.Yoffset);
                    }else{
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) &= ~(*mas << LCD->option.Yoffset);
                        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row + 1, LCD->option.Yoffset);
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) &= ~(*mas  >> (8 - LCD->option.Yoffset));
                    }
                    HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row - 1, LCD->option.Yoffset);
                }
                else{
                    if(p_or_d == LCD_PRINT){
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) |= *mas  >> -LCD->option.Yoffset;
                        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row - 1, LCD->option.Yoffset);
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) |= *mas  << (8 + LCD->option.Yoffset);
                    }else{
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) &= ~(*mas  >> -LCD->option.Yoffset);
                        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row - 1, LCD->option.Yoffset);
                        *(LCD->buffer + LCD->option.Row * DEFAULT_XMAX + LCD->option.Xcoord) &= ~(*mas  << (8 + LCD->option.Yoffset));
                    }
                    HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row + 1, LCD->option.Yoffset);
                }
            }
            ++mas;
        }
        HAL_LCD5110_Set_Coord(LCD, start_x, start_row, LCD->option.Yoffset);
        xprintf("end print\n");
    }
    
}

bool _print_row(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t size_mas, uint8_t t_or_m, uint8_t p_or_d){
    if(LCD->transfer & transfer_x_on && LCD->transfer & transfer_x_and_step_y){
        LCD->transfer &= ~transfer_x_on;
    }
    uint8_t delta = LCD->option.Xmax - LCD->option.Xcoord;
    if(delta > size_mas){
        _work_with_LCD_print_row(LCD, mas, size_mas , p_or_d);
        HAL_LCD5110_Set_Coord(LCD, 
        (t_or_m == LCD_PRINT_TEXT)? LCD->option.Xcoord + size_mas + LCD->option.Offset : LCD->option.Xcoord + size_mas, 
        LCD->option.Row, LCD->option.Yoffset);
    }else{
        uint8_t rou_befor = LCD->option.Row;
        xprintf("out mas\n");
        _work_with_LCD_print_row(LCD, mas, delta, p_or_d);
        size_mas -= delta;
        mas += delta;
        delta = LCD->option.Xmax - LCD->option.Xmin;
        HAL_LCD5110_Set_Coord(LCD, LCD->option.Xmin, (LCD->transfer & transfer_x_and_step_y)?LCD->option.Row + 1:LCD->option.Row, LCD->option.Yoffset); 
        if(!(LCD->transfer & transfer_x_on) && !(LCD->transfer & transfer_x_and_step_y)) return 1;
        xprintf("good 1 \n");
        if(!(LCD->transfer & transfer_y_on) && LCD->option.Row == 0 && rou_befor != 0) return 1;
        xprintf("good 2 \n");
        while(size_mas - delta > 0){
            _work_with_LCD_print_row(LCD, mas, delta, p_or_d);
            HAL_LCD5110_Set_Coord(LCD, LCD->option.Xmin, (LCD->transfer & transfer_x_and_step_y)?LCD->option.Row + 1:LCD->option.Row, LCD->option.Yoffset); 
            if(!(LCD->transfer & transfer_y_on) && LCD->option.Row == 0) return 1;
        }
        _work_with_LCD_print_row(LCD, mas, size_mas, p_or_d);
        HAL_LCD5110_Set_Coord(LCD, 
        (t_or_m == LCD_PRINT_TEXT)? LCD->option.Xmin + size_mas + LCD->option.Offset:LCD->option.Xmin + size_mas, 
        LCD->option.Row, LCD->option.Yoffset);
    }
    return 0;
}

void _Print_mas(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t columns, uint16_t size_mas, uint8_t t_or_m, uint8_t p_or_d){
    uint8_t start_X = LCD->option.Xcoord;
    uint8_t befor_row;

    while(size_mas - columns > 0){
        befor_row = LCD->option.Row;
        _print_row(LCD, mas, columns, t_or_m, p_or_d);
        if(!(LCD->transfer & transfer_y_on) && LCD->option.Row == 0) return;
        HAL_LCD5110_Set_Coord(LCD, start_X, (LCD->option.Row != befor_row)?LCD->option.Row: LCD->option.Row + 1, LCD->option.Yoffset);
        if(!(LCD->transfer & transfer_y_on) && LCD->option.Row == 0) return; 
        size_mas-=columns;
        mas+=columns;
    }
    _print_row(LCD, mas, size_mas, t_or_m, p_or_d);
}

void HAL_LCD5110_Print_mas(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t columns, uint16_t size_mas){
    _Print_mas(LCD,mas , columns, size_mas, LCD_PRINT_MAS, LCD_PRINT);
}

void HAL_LCD5110_Delete_mas(LCD5110_HandleTypeDef* LCD, uint8_t  *mas, uint16_t columns, uint16_t size_mas){
    _Print_mas(LCD,mas , columns, size_mas, LCD_PRINT_MAS, LCD_DELETE);
}

void _Working_With_Display(LCD5110_HandleTypeDef* LCD, uint8_t c_or_p, char * format, va_list variables){
    HAL_LCD5110_Set_Coord(LCD, LCD->option.Xcoord, LCD->option.Row, LCD->option.Yoffset);
    int number; 
    while(*format!='\0'){
        if(*format!='%'){
            if(*format == '\n'){
                HAL_LCD5110_Set_Coord(LCD, LCD->option.Xmin, LCD->option.Row + 1, LCD->option.Yoffset);
                ++format;
                continue;
            }
            xprintf("print char = %C\n", *format);
            if(_print_row(LCD, Char_font1[*format - charOffset], sizeof(Char_font1[*format - charOffset]), LCD_PRINT_TEXT, c_or_p)) return;
            ++format;
            continue;
        }
        ++format;
        if(*format == 'd'){
            number = va_arg(variables, int);
            char buff[20];
            itoa(number, buff, 10);
            for(uint8_t i = 0; buff[i]!='\0'; ++i){
            if(_print_row(LCD, Char_font1[buff[i] - charOffset], sizeof(Char_font1[buff[i] - charOffset]), LCD_PRINT_TEXT, c_or_p)) return;
            }
            ++format;
            continue;
        }
    }

}

void HAL_LCD5110_Print(LCD5110_HandleTypeDef* LCD, char * str, ...){
    va_list variables;
    va_start(variables, str);
    _Working_With_Display(LCD, LCD_PRINT, str, variables);
    va_end(variables);
}

void HAL_LCD5110_Delete(LCD5110_HandleTypeDef* LCD, char * str, ...){
    va_list variables;
    va_start(variables, str);
    _Working_With_Display(LCD, LCD_DELETE, str, variables);
    va_end(variables);
}

/////////////////////////////////////////////////вывод массива/////////////////////////////////////////////////////////////////////

