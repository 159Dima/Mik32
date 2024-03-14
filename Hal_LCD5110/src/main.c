#include "mik32_hal_PCC.h"
#include "mik32_hal_LCD5100.h"
#include "uart_lib.h"
#include "xprintf.h"

SPI_HandleTypeDef hspi0;
LCD5110_HandleTypeDef LCD1;

void SystemClock_Config(void);
static void SPI0_Init(void);
void LCD5110_Init(void);

int main()
{
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    
    SPI0_Init();
    LCD5110_Init();
    //HAL_LCD5110_PrintMas(&LCD1, a, sizeof(a));
    //String_to_Display("dhfj");
    //for(volatile int i = 0; i < 1000000; ++i);
    //HAL_LCD5110_Fill(&LCD1);
    HAL_LCD5110_Clear(&LCD1);
    for(volatile int i = 0; i < 1000000; ++i);
    String_to_Display(&LCD1, "Pi = %d,%d", 3, 14);
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