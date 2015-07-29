/**
 * @author  Pavel Lobyrin
 * @email   pavel@lobyrin.ru
 * @website http://lobyrin.de
 * @link    https://github.com/Comrada/SmartCar
 * @version v1.0
 * @ide     CooCox
 * @license GNU GPL v3
 * @brief   PWM initialization for SmartCar
 *
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Pavel Lobyrin, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */

#ifndef PWM_H
#define PWM_H

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_tim.h>

typedef struct
{
	uint32_t IPB1_Clock;
	uint32_t PWM_Frequency;
	uint32_t Period;
	uint32_t Prescaler;
	uint32_t MicrosecPerPeriod;    /*!< Microseconds used for one period.
	                                This is not useful in large pwm frequency, but good for controlling servos or similar,
	                                Where you need exact time of pulse high*/
}PWM_Data;

void initMotorPin(GPIO_TypeDef* GPIOx, uint16_t Pin, uint8_t PinSource, uint8_t GPIO_AF);
void initPWM(TIM_TypeDef* TIMx, uint32_t Period, uint16_t Prescaler, uint8_t Channel);
void calcPWMClocks(TIM_TypeDef* TIMx, PWM_Data * ClocksData, uint32_t Frequency);

#endif
