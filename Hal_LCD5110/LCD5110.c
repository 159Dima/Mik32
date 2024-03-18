#include "LCD5110.h"
#include "mik32_hal_lcd5110_Bitmaps.h"


HAL_StatusTypeDef LCD5110_Write(LCD5110Struct *LCD, uint8_t data, unsigned char mode)
{   
    // Передача и прием данных 
    HAL_StatusTypeDef error_code = HAL_OK;

    if (LCD->Spi->Init.ThresholdTX == 0)
    {
        error_code = HAL_ERROR;
        return error_code;
    }

    /* Запись байта в буфер */
    if ((error_code = HAL_SPI_WaitTxNotFull(LCD->Spi, SPI_TIMEOUT_DEFAULT)) != HAL_OK)
    {
        return error_code;
    }
        /* Запись байта */
    HAL_GPIO_PinWrite(LCD->pinDC, mode);
    LCD->Spi->Instance->TxData = data;
    return error_code;
}

void LCD5110_Reset(LCD5110Struct* LCD)
{
    HAL_GPIO_PinWrite(LCD->pinRST, GPIO_PIN_LOW);
    for (volatile int i = 0; i < 100000; i++);
    HAL_GPIO_PinWrite(LCD->pinRST, GPIO_PIN_HIGH); 
}

void LCD5110_Reset_Settings(LCD5110Struct* LCD)
{
    LCD->x_coord =  0;
    LCD->y_coord =  0;
    LCD->row =      0;
    LCD->rE =       6;
    LCD->rS =       0;
    LCD->xE =       84;
    LCD->xS =       0;
    LCD->offset =   1; 
}

void LCD5110_Sleep(LCD5110Struct* LCD)
{
    LCD->settings|=PCD8544_POWERDOWN;
    //LCD5110_Clear(LCD);
    LCD5110_SPI_ON(LCD);
    LCD5110_Inverse_Mode(LCD, display_blank);
    LCD5110_Usually_Settings(LCD);
    LCD5110_SPI_OFF(LCD);
}
 
void LCD5110_Wake(LCD5110Struct* LCD)
{
    LCD->settings&=~PCD8544_POWERDOWN;
    LCD5110_SPI_ON(LCD);
    LCD5110_Inverse_Mode(LCD, normal_mode);
    LCD5110_Usually_Settings(LCD);
    LCD5110_SPI_OFF(LCD);
}

void HAL_LCD5110_SetBorder(LCD5110Struct* LCD, uint8_t xS, uint8_t xE, uint8_t rS, uint8_t rE) 
{
    if(rE > LCD_RMAX){rE = LCD_RMAX;}
    if(xE > LCD_XMAX){xE = LCD_XMAX;}
    if(rS > rE){rS = rE;}
    if(xS > xE){xS = xE - 5;}
    LCD->xS = xS;
    LCD->xE = xE;
    LCD->rS = rS;
    LCD->rE = rE;
}


void LCD5110_Init(LCD5110Struct* LCD)
{
    LCD5110_Reset_Settings(LCD);
    if(LCD->buffer != NULL) LCD5110_Clear_Buffer(LCD);

    HAL_PadConfig_PinMode(LCD->pinRST, PIN_MODE1);
    HAL_GPIO_PinDirection(LCD->pinRST, GPIO_PIN_OUTPUT);
    HAL_PadConfig_PinMode(LCD->pinDC, PIN_MODE1);
    HAL_GPIO_PinDirection(LCD->pinDC, GPIO_PIN_OUTPUT);
    HAL_PadConfig_PinMode(LCD->pinCS, PIN_MODE1);
    HAL_GPIO_PinDirection(LCD->pinCS, GPIO_PIN_OUTPUT);

	// //RESET
    LCD5110_Reset(LCD);

    LCD5110_SPI_ON(LCD);
    LCD5110_Advanced_Settings(LCD);
    LCD5110_Temperature_Correction(LCD ,LCD_TEMP);
    LCD5110_Level_Offset(LCD, LCD_BIAS);
    LCD5110_Setting_Contrast(LCD, 100);
    LCD5110_Usually_Settings(LCD);
    LCD5110_Inverse_Mode(LCD, normal_mode);  
    LCD5110_Clear(LCD);
    LCD5110_SPI_OFF(LCD);
}


void LCD5110_Mode(LCD5110Struct * LCD, Display_Configuration DisCon)
{
    LCD5110_SPI_ON(LCD);
    LCD5110_Inverse_Mode(LCD, DisCon);
    LCD5110_SPI_OFF(LCD); 
}

void LCD5110_Contrast(LCD5110Struct * LCD, uint8_t contrast)
{
    if (contrast > 127 )contrast = 127;
    if (contrast < 0 )contrast = 0;
    LCD5110_SPI_ON(LCD);
    LCD5110_Advanced_Settings(LCD);
    LCD5110_Setting_Contrast(LCD, contrast);
    LCD5110_Usually_Settings(LCD);
    LCD5110_SPI_OFF(LCD); 
}

void LCD5110_SPI_ON(LCD5110Struct * LCD)
{
	if(LCD->Spi->Init.ManualCS == SPI_MANUALCS_ON)
    {
        HAL_SPI_Enable(LCD->Spi);
        HAL_GPIO_PinWrite(LCD->pinCS, GPIO_PIN_LOW);
    }
}

void LCD5110_SPI_OFF(LCD5110Struct * LCD)
{
    for (volatile int8_t i = 0; i < 10; i++);
    if(LCD->Spi->Init.ManualCS == SPI_MANUALCS_ON)
    {
        HAL_SPI_CS_Disable(LCD->Spi);
        HAL_GPIO_PinWrite(LCD->pinCS, GPIO_PIN_HIGH);
    }
}
////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef LCD5110_Advanced_Settings(LCD5110Struct * LCD)
{
    return LCD5110_Write(LCD, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION | ((LCD->settings & PCD8544_HORIZONTADD) << 1) | ((LCD->settings & PCD8544_POWERDOWN) << 1), LCD_COMMAND);
}

HAL_StatusTypeDef LCD5110_Temperature_Correction(LCD5110Struct * LCD, uint8_t TC)
{
    return LCD5110_Write(LCD,PCD8544_SETTEMP | TC, LCD_COMMAND);
}

HAL_StatusTypeDef LCD5110_Level_Offset(LCD5110Struct * LCD, uint8_t Level)
{
    return LCD5110_Write(LCD, PCD8544_SETBIAS | Level, LCD_COMMAND);
}

HAL_StatusTypeDef LCD5110_Setting_Contrast(LCD5110Struct * LCD, uint8_t contrast)
{
    return LCD5110_Write(LCD, PCD8544_SETVOP | contrast, LCD_COMMAND);
}
////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef LCD5110_Usually_Settings(LCD5110Struct * LCD)
{
    return LCD5110_Write(LCD, PCD8544_FUNCTIONSET | ((LCD->settings & PCD8544_HORIZONTADD) << 1) | ((LCD->settings & PCD8544_POWERDOWN) << 1), LCD_COMMAND );
}

HAL_StatusTypeDef LCD5110_Inverse_Mode(LCD5110Struct * LCD, Display_Configuration DisCon)
{
    return LCD5110_Write(LCD, PCD8544_DISPLAYCONTROL |  DisCon, LCD_COMMAND);
}

HAL_StatusTypeDef LCD5110_Set_X_coord(LCD5110Struct * LCD, int8_t X)
{
    return LCD5110_Write(LCD, PCD8544_SETXADDR | X, LCD_COMMAND);
}

HAL_StatusTypeDef LCD5110_Set_Y_coord(LCD5110Struct * LCD, int8_t Y)
{
    return LCD5110_Write(LCD, PCD8544_SETYADDR | Y, LCD_COMMAND);
}
///////////////////////////////////////////////////////////////////////////////////////////


void LCD5110_Coord_SPI(LCD5110Struct * LCD, int8_t x, int8_t y)
{   
    LCD5110_Set_X_coord(LCD, x);
    LCD5110_Set_Y_coord(LCD, y);
}

void LCD5110_Coord(LCD5110Struct * LCD, int8_t x, int8_t y)
{   
    if( x < 0 )  x = 0;
    else if( x > LCD_XMAX ) x = LCD_RMAX;
    LCD->x_coord = x;
    if( y < 0 )  y = 0;
    else if( y > LCD_RMAX ) y= LCD_RMAX;
    LCD->row = y;
    if(LCD->buffer == NULL)LCD->y_coord = 0;
}


void LCD5110_Fill(LCD5110Struct * LCD)
{
    LCD5110_SPI_ON(LCD);
    LCD5110_Coord_SPI(LCD, 0, 0);
    for (int16_t c=0; c < 504; c++){LCD5110_Write(LCD, 0xFF, LCD_DATA);}
    LCD5110_SPI_OFF(LCD);
}

void LCD5110_Clear(LCD5110Struct * LCD)
{
    LCD5110_SPI_ON(LCD);
    LCD5110_Coord_SPI(LCD, 0, 0);
    for (int16_t c=0; c<504; c++){LCD5110_Write(LCD, 0x00, LCD_DATA);}
    LCD5110_SPI_OFF(LCD);
}

void LCD5110_Print_Char(LCD5110Struct * LCD, char Char_Data[])
{
    uint8_t DATA[504]={0};
    uint16_t j = 0;
    for(uint16_t i = 0; *Char_Data !='\0'; Char_Data++, i++)
    {
        for(int cy = 0 ;cy<5; cy++, j++)
        {
           DATA[j] =  Char_font1[*Char_Data-charOffset][cy];
        }
    }  
    LCD5110_Print_mas(LCD, &DATA[0], 84-LCD->x_coord, j);
}
///////////////////////////////////////////////////////////////////////////////////

void LCD5110_Clear_Buffer(LCD5110Struct * LCD)
{
    for (size_t i = 0; i < 6 ; i++)
    {
        for (int8_t j = 0; j < 84; j++)
        {
            *(LCD->buffer + i*LCD_XMAX + j) = 0x00;
        }
    } 
}

void LCD5110_Fill_Buffer(LCD5110Struct * LCD)
{
    for (size_t i = 0; i < 6 ; i++)
    {
        for (int8_t j = 0; j < 84; j++)
        {
            *(LCD->buffer + i*LCD_XMAX + j) = 0xff;
        }
    } 
}

void LCD5110_Buffer_Update(LCD5110Struct * LCD)
{   
    LCD5110_SPI_ON(LCD);
    LCD5110_Coord_SPI(LCD, 0, 0);
    if(LCD->buffer != NULL)
    {
        for(int8_t i = 0; i < 6;i++)
        {
            for(int8_t j = 0; j<84;j++)
            {
             LCD5110_Write(LCD, *(LCD->buffer + i*LCD_XMAX + j) , LCD_DATA);
            }
        }

    }
    LCD5110_SPI_OFF(LCD);
}

void LCD5110_Set_Pixel(LCD5110Struct * LCD, int8_t X, int8_t Y)
{
    if (X > 84)X = 84;
    else if (X < 0)X = 0;
    if(Y > 48) Y = 48;
    else if(Y < 0) Y = 0;
    LCD->row = Y / 8;
    if(LCD->buffer == NULL)LCD->y_coord = 0;
    else LCD->y_coord = Y % 8;
    LCD->x_coord  = X;
}

void LCD5110_Print_Int(LCD5110Struct * LCD, int Int_Data)
{
    char buff[20];
    LCD5110_Print_Char(LCD, itoa(Int_Data, buff, 10));
}

void LCD5110_Print_Float(LCD5110Struct * LCD, float Float_Data, uint8_t after_point)
{
    char buff[20];
    if(after_point>0)
    {
    uint16_t point = 1;
    for(int8_t i = 0; i < after_point; i++)
    {
       point*=10;
    }
    if( Float_Data < 0 )
    {
        LCD5110_Print_Char(LCD, "-");
        Float_Data *=(-1);
    }
    int16_t a = Float_Data * point;
    int16_t b = a / point;
    LCD5110_Print_Char(LCD, itoa(b, buff, 10));
    LCD5110_Print_Char(LCD, ".");
    b =(a % point); 
    LCD5110_Print_Char(LCD, itoa( b, buff, 10));
    LCD5110_Print_Char(LCD, " ");
    }
    else
    {
        if( Float_Data < 0 )
        {
        LCD5110_Print_Char(LCD, "-");
        Float_Data *=(-1);
        }
        int16_t a = Float_Data;
        if(Float_Data - a >=0.5)
        {
            a++;
        }
        LCD5110_Print_Char(LCD, itoa(a, buff, 10));
        LCD5110_Print_Char(LCD, " ");
    }

}

void LCD5110_Print_mas(LCD5110Struct *LCD,uint8_t  *mas, uint8_t x_, uint16_t size_)
{
    int8_t y = LCD->y_coord;
    int8_t row = LCD->row;
    int8_t x = LCD->x_coord;
    int8_t line = 0;
    if(LCD->buffer == NULL)
    {
        LCD5110_SPI_ON(LCD);
        LCD5110_Coord_SPI(LCD, LCD->x_coord, LCD->row);
        do
        {
            for(int8_t i = 0; i < x_ && (size_ - line*x_ - i)>0 ;i++)
            {
                LCD5110_Write(LCD, *(mas+line*x_+i), LCD_DATA);
                x++;
                if(x>LCD->xE)break;
            }
            
            line++;
            if((size_ - line*x_)>0)
            {
                row++;
                x = LCD->x_coord;
            }
            if(row>LCD->rE)break;  
            LCD5110_Coord_SPI(LCD, LCD->x_coord, row);
        }while((size_ - line*x_)>0);
        LCD5110_SPI_OFF(LCD);
    }
    else
    {
 
    }
    if(y>0)row--;
    LCD->y_coord = y;
    LCD->row = row;
    LCD->x_coord = x;
}


void LCD5110_Delete_mas(LCD5110Struct *LCD,uint8_t  *mas, uint8_t x_, uint16_t size_)
{
    if(LCD->buffer != NULL)  
    {
    int8_t y = LCD->y_coord;
    int8_t row = LCD->row;
    int8_t x = LCD->x_coord;
    int8_t line = 0;   
    do
    {
        x = LCD->x_coord;
        for(int8_t i = 0; i < x_ && (size_ - line*x_ - i)>0 ;i++)
        {
            *(LCD->buffer + row*84 + x) &= ~(*(mas+line*x_+i) << y );
            x++;
            if(x>LCD->xE)break;
        }
            if(y>0)
        {
            row++;
            if(row>LCD->rE)break;
            x = LCD->x_coord;
            for(int8_t i = 0; i< x_ && (size_ - line*x_ - i)>0 ;i++)
            {
            *(LCD->buffer + row*84 + x) &= ~((*(mas+line*x_ + i) >> (8-y)));
            x++;
            if(x>LCD->xE)break;
            }
            line++;
            
        }
        else
        {  
        line++;
        if((size_ - line*x_)>0)  row++;
        if(row>LCD->rE)break;
        }
        
        
    }while((size_ - line*x_)>0);  
    if(y>0)row--;    
    LCD->y_coord = y;
    LCD->row = row;
    LCD->x_coord = x;
    }      
}