/**
 * @author  Pavel Lobyrin
 * @email   pavel@lobyrin.ru
 * @website http://lobyrin.de
 * @link    https://github.com/Comrada/SmartCar
 * @version v1.1
 * @ide     CooCox
 * @license GNU GPL v3
 * @brief   Motors functionality for SmartCar
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

#ifndef MOTORS_H
#define MOTORS_H
#endif

#include "stm32f4xx.h"

typedef struct
{
	uint32_t Mode;
	uint32_t Direction;
	uint32_t Speed;
}MotorState;

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

#define Mode_Stop					((uint32_t)0x00000000)
#define Mode_Active					((uint32_t)0x00000001)
#define Direction_None				((uint32_t)0x00000000)
#define Direction_Forward			((uint32_t)0x00000001)
#define Direction_Backward			((uint32_t)0x02000002)

#define MOTOR_PWM_FREQ 1000					// 1kHz
#define MOTOR_PWM_MAX_PULSE_WIDTH 500		// 500us
#define MOTOR_PWM_MIN_PULSE_WIDTH 0

#define MOTOR_DELAY_AFTER_START 500
#define MOTOR_DELAY_FOR_TURNING 500

static uint32_t speeds[] = {0, 0, 0, 0, 0, 0};	//Array with pulses length of PWM signal

void initMotors(void);
void initMotor1(void);
void initMotor2(void);
void initMotor3(void);
void initMotor4(void);
void initMotorStruct(MotorState* Motor_Data);
void initPWMClocks(TIM_TypeDef* TIMx, PWM_Data * ClocksData, uint32_t Frequency);
void calcSpeeds(void);

void SoftStart(uint32_t Direction, uint32_t duration);
void Forward(uint8_t speed);
void Stop(void);
void Back(uint8_t speed);
void Left(uint8_t angle);
void Right(uint8_t angle);
void BackLeft(uint8_t angle);
void BackRight(uint8_t angle);

void Motor1Start(uint32_t Direction, uint8_t speed);
void Motor1Speed(uint8_t speed);
void Motor1Stop(void);

void Motor2Start(uint32_t Direction, uint8_t speed);
void Motor2Speed(uint8_t speed);
void Motor2Stop(void);

void Motor3Start(uint32_t Direction, uint8_t speed);
void Motor3Speed(uint8_t speed);
void Motor3Stop(void);

void Motor4Start(uint32_t Direction, uint8_t speed);
void Motor4Speed(uint8_t speed);
void Motor4Stop(void);
