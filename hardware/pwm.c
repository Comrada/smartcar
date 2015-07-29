/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Pavel Lobyrin, 2015
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

#include "pwm.h"
#include "delay.h"

void initMotorPin(GPIO_TypeDef* GPIOx, uint16_t Pin, uint8_t PinSource, uint8_t GPIO_AF)
{
	if (GPIOx == GPIOA)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	if (GPIOx == GPIOB)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef Port;
	Port.GPIO_Mode = GPIO_Mode_AF;
	Port.GPIO_OType = GPIO_OType_PP;
	Port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Port.GPIO_Pin = Pin;
	Port.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOx, &Port);
	GPIO_PinAFConfig(GPIOx, PinSource, GPIO_AF);
}

void initPWM(TIM_TypeDef* TIMx, uint32_t Period, uint16_t Prescaler, uint8_t Channel)
{
	if (TIMx == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	if (TIMx == TIM3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef Timer;
	Timer.TIM_CounterMode = TIM_CounterMode_Up;
	Timer.TIM_Period = Period - 1;
	Timer.TIM_Prescaler = Prescaler - 1;
	Timer.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIMx, &Timer);

	TIM_OCInitTypeDef PWM;
	PWM.TIM_Pulse = Period - 1;
	PWM.TIM_OCMode = TIM_OCMode_PWM2;
	PWM.TIM_OutputState =TIM_OutputState_Enable;
	PWM.TIM_OCPolarity = TIM_OCPolarity_Low;
	if (Channel == 1)
		TIM_OC1Init(TIMx, &PWM);
	if (Channel == 2)
		TIM_OC2Init(TIMx, &PWM);
	if (Channel == 3)
		TIM_OC3Init(TIMx, &PWM);
	if (Channel == 4)
		TIM_OC4Init(TIMx, &PWM);
	TIM_Cmd(TIMx, ENABLE);
}


void calcPWMClocks(TIM_TypeDef* TIMx, PWM_Data * ClocksData, uint32_t Frequency)
{
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);
	ClocksData->IPB1_Clock = RCC_ClocksStruct.PCLK2_Frequency;	//Frequency of APB1
	ClocksData->PWM_Frequency = Frequency;
	if (TIMx == TIM2)
	{
		ClocksData->Period = (uint32_t)(ClocksData->IPB1_Clock / Frequency);
		ClocksData->Prescaler = 1;
	} else if (TIMx == TIM3)
	{
		ClocksData->Period = (uint32_t)(ClocksData->IPB1_Clock / Frequency / 2);	//TIM3 is 16bit, divide in half
		ClocksData->Prescaler = 2;
	}
	ClocksData->MicrosecPerPeriod = (uint32_t)(1000000 / Frequency);
}
