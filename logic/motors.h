/**
 * @author  Pavel Lobyrin
 * @email   pavel@lobyrin.ru
 * @website http://lobyrin.de
 * @link    https://github.com/Comrada/SmartCar
 * @version v1.0
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
#include "tm_stm32f4_pwm.h"

typedef struct
{
	uint32_t Mode;
	uint32_t Direction;
	uint32_t Speed;
}MotorState;

#define Mode_Stop					((uint32_t)0x00000000)
#define Mode_Active					((uint32_t)0x00000001)
#define Direction_None				((uint32_t)0x00000000)
#define Direction_Forward			((uint32_t)0x00000001)
#define Direction_Backward			((uint32_t)0x02000002)


#define MOTOR_FRONT_FWD_L_PIN 1
#define MOTOR_FRONT_FWD_R_PIN 2
#define MOTOR_REAR_FWD_L_PIN 3
#define MOTOR_REAR_FWD_R_PIN 4
#define MOTOR_FRONT_BACK_L_PIN 5
#define MOTOR_FRONT_BACK_R_PIN 6
#define MOTOR_REAR_BACK_L_PIN 7
#define MOTOR_REAR_BACK_R_PIN 8

#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_MAX_PULS_WIDTH 500
#define MOTOR_PWM_MIN_PULS_WIDTH 0

#define MOTOR_DELAY_AFTER_START 200

#define MOTOR_SPEED_MIN 0
#define MOTOR_SPEED_MAX 500

static uint16_t speeds[] = {0, 0, 0, 0, 0, 0};

void initMotors(void);
void initMotorStruct(MotorState* Motor_Data);
void initSpeeds(void);
void SoftStop(uint32_t duration);
void SoftStart(uint32_t duration);
void Stop(void);

//TODO LIST
void Forward(uint8_t speed);
void Back(uint8_t speed);
void FwdLeft(uint8_t angle);
void FwdRight(uint8_t angle);
void BackLeft(uint8_t angle);
void BackRight(uint8_t angle);
void Motor1FwdOn(void);
void Motor1Speed(uint16_t speed);
void Motor1BackOn(void);
void Motor1Stop(void);
void Motor2FwdOn(void);
void Motor2Speed(uint16_t speed);
void Motor2BackOn(void);
void Motor2Stop(void);
void Motor3FwdOn(void);
void Motor3Speed(uint16_t speed);
void Motor3BackOn(void);
void Motor3Stop(void);
void Motor4FwdOn(void);
void Motor4Speed(uint16_t speed);
void Motor4BackOn(void);
void Motor4Stop(void);
//TODO END
