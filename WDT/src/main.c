#include "mik32_hal_timer16.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_wdt.h"
#include "uart_lib.h"
#include "xprintf.h"

Timer16_HandleTypeDef htimer16_1;
SCR1_TIMER_HandleTypeDef hscr1_timer;
WDT_InitTypeDef wdt;

static void Scr1_Timer_Init(void);

void SystemClock_Config(void);


void WDT_Init(){
  wdt.Clock = OSC32K;
  wdt.Prescaler = PRESCALE_16;
  wdt.Reload = 1000;
  wdt.TimeOut = 100;
  HAL_WDT_Init(&wdt);
}
void WDT_NEW_Time(){
  wdt.Reload = 500;
  HAL_WDT_Init(&wdt); 
  HAL_WDT_Start();
}

int main()
{    

  SystemClock_Config();
  Scr1_Timer_Init();
  WDT_Init();

  UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
  
  xprintf("\nstart 1\n"); 
  HAL_WDT_Start(); 
  HAL_DelayMs(&hscr1_timer, 1000);
  HAL_WDT_Refresh();
  xprintf("stop 1");


  xprintf("\nstart 2\n"); 
  WDT_NEW_Time();
  HAL_DelayMs(&hscr1_timer, 500);
  HAL_WDT_Refresh();
  xprintf("stop 2\n");

  HAL_WDT_Stop();

  while (1)
  {   
    HAL_DelayMs(&hscr1_timer, 1050);
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

static void Scr1_Timer_Init(void)
{
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL;    /* Источник тактирования */
    hscr1_timer.Divider = 0;       /* Делитель частоты 10-битное число */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}


