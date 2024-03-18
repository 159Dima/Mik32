#include "mik32_hal_PCC.h"
#include "mik32_hal_LCD5100.h"
#include "uart_lib.h"
#include "xprintf.h"

SPI_HandleTypeDef hspi0;
LCD5110_HandleTypeDef LCD1;

void SystemClock_Config(void);
static void SPI0_Init(void);
void LCD5110_Init(void);

void test_1_Fill_Clear(){
    xprintf("Tese 1\n");
    xprintf("Fill LCD\n");
    HAL_LCD5110_Fill(&LCD1);
    for(volatile int i = 0; i < 10000000; ++i);
    xprintf("Clear LCD\n");
    HAL_LCD5110_Clear(&LCD1);
    for(volatile int i = 0; i < 10000000; ++i);
    xprintf("Creat Buffer\n");
    uint8_t buffer[6][84] = {0};
    LCD1.buffer= &buffer[0][0]; 
    xprintf("Fill Buffer and Print\n");
    HAL_LCD5110_Fill(&LCD1);
    for(volatile int i = 0; i < 10000000; ++i);
    xprintf("Clear Buffer and Print\n");
    HAL_LCD5110_Clear(&LCD1);
    for(volatile int i = 0; i < 10000000; ++i);
    LCD1.buffer = NULL;
}

void test_2_set_coord(){
    HAL_LCD5110_Set_Coord(&LCD1, 0, 0);
    xprintf("1) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);

    HAL_LCD5110_Set_Coord(&LCD1, 5, 3);
    xprintf("2) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);

    HAL_LCD5110_Set_Coord(&LCD1, 84, 5);
    xprintf("3) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);

    HAL_LCD5110_Set_Coord(&LCD1, 86, 6);
    xprintf("4) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);

    HAL_LCD5110_Set_Coord(&LCD1, -1, -1);
    xprintf("5) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);

    HAL_LCD5110_Set_Coord(&LCD1, 0, 0, 4);
    xprintf("6) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);
    uint8_t buffer[6][84] = {0};
    LCD1.buffer= &buffer[0][0]; 
    HAL_LCD5110_Set_Coord(&LCD1, 5, 3, 4);
    xprintf("7) x coord = %d, row = %d, y offset = %d\n", LCD1.option.Xcoord,LCD1.option.Row, LCD1.option.Yoffset);
    LCD1.buffer = NULL;
}

void prinnt_buffer(){
    uint8_t buffer[6][84] = {0};
    LCD1.buffer= &buffer[0][0]; 
    // LCD1.transfer = transfer_off;
    // LCD1.transfer = transfer_x_on;
    // LCD1.transfer = transfer_y_on ;
    // LCD1.transfer = transfer_x_and_step_y;
    // LCD1.transfer = transfer_x_and_step_y | transfer_y_on;
    // LCD1.transfer = transfer_x_on | transfer_y_on;
    // LCD1.transfer = transfer_x_on | transfer_y_on | transfer_x_and_step_y ;
    HAL_LCD5110_Set_Coord(&LCD1, 0 , 2 , -7);
    HAL_LCD5110_Print(&LCD1, "hollo world");
    HAL_LCD5110_updete(&LCD1);
    HAL_LCD5110_Set_Coord(&LCD1, 0 , 2 , -1);
    HAL_LCD5110_Print(&LCD1, "hollo world");
    HAL_LCD5110_updete(&LCD1);
    //HAL_LCD5110_Fill(&LCD1);
}

void prinnt_LCD(){
    LCD1.transfer = transfer_x_and_step_y|transfer_y_on ;
    HAL_LCD5110_Set_Coord(&LCD1, 60, 5);
    HAL_LCD5110_Print(&LCD1, "hollo world");
}

void test_3_print(){
    xprintf("Print LCD\n");

    // prinnt_LCD();
    prinnt_buffer();

    // HAL_LCD5110_Set_Coord(&LCD1, 5, 3);
    // HAL_LCD5110_Print(&LCD1, "I am LCD5110");
    // HAL_LCD5110_Set_Coord(&LCD1, 3, 4);
    // HAL_LCD5110_Print(&LCD1, "Pi=%d,%d", 3, 14);
    // HAL_LCD5110_Set_Coord(&LCD1, 54, 5);
    // HAL_LCD5110_Print(&LCD1, "Pere-nos");
    // for(volatile int i = 0; i < 10000000; ++i);
    // xprintf("Delete LCD\n");
    // HAL_LCD5110_Set_Coord(&LCD1, 4, 2);
    // HAL_LCD5110_Delete(&LCD1, "hollo");
}

void test_4_print_mas(){
    HAL_LCD5110_Clear(&LCD1);
    uint8_t mas[] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00,
                    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00};
    // LCD1.transfer = transfer_off;
    // LCD1.transfer = transfer_x_on;
    // LCD1.transfer = transfer_y_on ;
    // LCD1.transfer = transfer_x_and_step_y;
    // LCD1.transfer = transfer_x_and_step_y | transfer_y_on;
    // LCD1.transfer = transfer_x_on | transfer_y_on;
    // LCD1.transfer = transfer_x_on | transfer_y_on | transfer_x_and_step_y ;
    HAL_LCD5110_Set_Coord(&LCD1, 82, 3 );
    HAL_LCD5110_Print_mas(&LCD1,mas, 9, sizeof(mas));

    // LCD1.transfer = transfer_off;
    // HAL_LCD5110_Set_Coord(&LCD1, 80, 5 );
    // HAL_LCD5110_Print_mas(&LCD1,mas, 9, 18);
    // HAL_LCD5110_Set_Coord(&LCD1, LCD1.option.Xcoord + 1, LCD1.option.Row);
    // HAL_LCD5110_Print_mas(&LCD1,mas, 10, 10);
}

int main()
{
    
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    SPI0_Init();
    LCD5110_Init();

    //test_1_Fill_Clear();
    //test_2_set_coord();
    test_3_print();
    // test_4_print_mas();

    while (1){

    }
}

static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;
    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi0.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}

void LCD5110_Init(void){
    ////////////////////////////////////////////////////
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_LCD5110_Init = {0};
    GPIO_LCD5110_Init.Pin = GPIO_PIN_10;
    GPIO_LCD5110_Init.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_LCD5110_Init.Pull = HAL_GPIO_PULL_NONE;
    ////////////////////////////////////////////////////
    LCD1.pinCS.GPIO = NULL;
    //LCD1.pinCS.PIN = GPIO_LCD5110_Init;
    //GPIO_LCD5110_Init.Pin = GPIO_PIN_4;
    LCD1.pinDC.GPIO = GPIO_0;
    LCD1.pinDC.PIN = GPIO_LCD5110_Init;
    GPIO_LCD5110_Init.Pin = GPIO_PIN_9;
    LCD1.pinRST.GPIO = GPIO_0;
    LCD1.pinRST.PIN = GPIO_LCD5110_Init;
    LCD1.buffer= NULL; 
    /////////////////////////////////////////////////////
    LCD1.Spi = &hspi0; 
    if (HAL_LCD5110_Init(&LCD1) != HAL_OK)
    {
        xprintf("LCD5110_Init_Error\n");
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}