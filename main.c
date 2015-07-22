/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Pavel Lobyrin, 2015
 * | SmartCar v. 1.0
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

#include "main.h"
#include "motors.h"
#include "usart.h"
#include "pwm.h"
#include "delay.h"

int main(void)
{
    SystemInit();

    initUSART(USART_BAUDRATE);

    logToUSART("SmartCar ver. 1.0\r\n");
    logToUSART("Initializing...\r\n");

    initMotors();
	logToUSART("Motors initialized...\r\nDone\r\n");
    printJSONToUSART("\"ready\"");

	GPIO_InitTypeDef PORT;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	GPIO_StructInit(&PORT);
	PORT.GPIO_Pin = (GPIO_Pin_12 | GPIO_Pin_13);
	PORT.GPIO_OType = GPIO_OType_PP;
	PORT.GPIO_PuPd = GPIO_PuPd_NOPULL;
	PORT.GPIO_Mode = GPIO_Mode_OUT;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &PORT);

    while (1) {
    	__NOP();
    	 GPIOD->ODR ^=(GPIO_Pin_12 | GPIO_Pin_13);
    	 Delay(1000);
    }
}

