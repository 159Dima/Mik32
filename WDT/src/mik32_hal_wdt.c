#include "mik32_hal_wdt.h"

uint16_t TimeOut = 20;
uint16_t WDT_prescale[] = {
    1, 2, 4, 16, 64, 255, 1024, 4096 
};
/*
Бит активности таймера:
1 – таймер запущен;
0 – таймер остановлен
*/
uint8_t Wdt_enabled(){

    if (WDT->STA & WDT_STA_ENABLED_M) return 1;
    else return 0;
}
/*
Бит перезагрузки значения:
1 – выполняется перезагрузка значения в таймере;
0 – перезагрузка значения таймера в данный момент не выполняется (за-вершена)*/
uint8_t Wdt_loading(){
    if (WDT->STA & WDT_STA_LOADING_M) return 1;
    else return 0;
}
/*
Флаг генерации сброса сторожевым таймером.
Сбрасывается в 0 только при снятии и последующей подаче пи-тания
*/
uint8_t Wdt_reset_flag(){
    if (WDT->STA & WDT_STA_RST_FLAG_M) return 1;
    else return 0;
}

uint16_t WDT_time(){
    return WDT->CON & 0xFFF;
}

HAL_StatusTypeDef HAL_WDT_Stop(){
    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->KEY = WDT_KEY_STOP;

    if(Wdt_enabled()){
        return HAL_ERROR;        
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_WDT_Start(){
    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->KEY = WDT_KEY_START;

    if(!Wdt_enabled()){
        return HAL_ERROR;        
    }

    return HAL_OK;
}

HAL_StatusTypeDef WDT_enable_(){
    uint8_t timeout = 0; 
    while(++ timeout < MAX_TIME_LOAD){
        if(!Wdt_loading())
        {
            return HAL_OK;
        }
    }
    return  HAL_TIMEOUT;
}

uint16_t Millis_in_clock_(WDT_InitTypeDef *WDT_){
    uint32_t clck;

    clck = (WDT_->Clock < 2) ? CLOCK32M : CLOCK32K;

    uint32_t tick = 4095 - ((clck/(WDT_prescale[WDT_->Prescaler]))*WDT_->Reload)/1000;
    
    if (tick > 4095 || tick == 0){
        return 0;        
    }

    return tick;
}

HAL_StatusTypeDef HAL_WDT_Init(WDT_InitTypeDef *WDT_){

    if(WDT_ == NULL){
        return HAL_ERROR;
    }

    if(WDT_->TimeOut < 16){
        return HAL_ERROR;
    }
    TimeOut = WDT_->TimeOut;
    uint16_t tact = Millis_in_clock_(WDT_);
    if(tact == 0) {
        return HAL_ERROR;
    }

    if(HAL_WDT_Stop() !=  HAL_OK){
        return HAL_ERROR;
    }
        
    PM->WDT_CLK_MUX = WDT_->Clock;
    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->CON= (WDT_->Prescaler<< WDT_CON_PRESCALE_S) | WDT_CON_PRELOAD(tact);

    if(WDT_enable_() != HAL_OK){
        return HAL_TIMEOUT;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_WDT_Refresh(){
    if(HAL_WDT_Start() != HAL_OK){
        return HAL_ERROR;        
    }
    return  WDT_enable_();   
}



