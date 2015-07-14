#include <stdlib.h>
#include "defines.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_pwm.h"
#include "stm32f4xx_dma.h"
#include "string.h"
#include "misc.h"
#include "main.h"
#include "motors.h"
#include "uart.h"



int main(void)
{
    SystemInit();

    initUSART(USART_BAUDRATE);

    logToUSART("SmartCar ver. 1.0\r\n");
    logToUSART("Initializing...\r\n");

    TM_PWM_InitTimer(TIM2, &TIM2_Data, MOTOR_PWM_FREQ);
    TM_PWM_InitTimer(TIM3, &TIM3_Data, MOTOR_PWM_FREQ);
    logToUSART("Timers initialized...\r\n");
    initMotors();
	logToUSART("Motors initialized...\r\nDone\r\n");
    printJSONToUSART("\"ready\"");

    printJSONToUSART("\"mode\":\"soft_start\"");
    /*SoftStart(1);
    Delay(500);
    FwdLeft(1);
    Delay(1000);
    printJSONToUSART("\"mode\":\"stop\"");
    Stop();
    Delay(500);
    printJSONToUSART("\"mode\":\"backward\"");
    Back(1);
    Delay(1000);
    Stop();*/
    //char str1[20];
    //sprintf(str1, "%d\r\n", TIM2_Data.Period);
    //logToUSART(str1);
    SoftStart(1);
    Delay(2000);
    Stop();
    Delay(500);
    Back(1);
    Delay(2000);
    Stop();
   /* Motor1FwdOn();
    Motor1Speed(1);
    Delay(2000);
    Motor1Speed(2);
    Delay(2000);
    Motor1Speed(3);
    Delay(2000);
    Motor1Speed(4);
    Delay(2000);
    Motor1Speed(5);
    Delay(2000);
    Stop();*/





    while (1) {
    	__NOP();
    }
}




































void Delay(uint32_t ms)
{
        volatile uint32_t nCount;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);

        nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
        for (; nCount!=0; nCount--);
}


void DelayMC(uint32_t mc)
{
        volatile uint32_t nCount;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);

        nCount=(RCC_Clocks.HCLK_Frequency/10000000)*mc;
        for (; nCount!=0; nCount--);
}
