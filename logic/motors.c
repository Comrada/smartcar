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

#include "tm_stm32f4_pwm.h"
#include "motors.h"
#include "main.h"

MotorState Motor1, Motor2, Motor3, Motor4;

void initMotors(void)
{
	initMotorStruct(&Motor1);
	initMotorStruct(&Motor2);
	initMotorStruct(&Motor3);
	initMotorStruct(&Motor4);
	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_1, TM_PWM_PinsPack_1); // 1 Motor FWD
	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_2, TM_PWM_PinsPack_1); // 2 Motor FWD
	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_3, TM_PWM_PinsPack_1); // 3 Motor FWD
	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_4, TM_PWM_PinsPack_1); // 4 Motor FWD

	TM_PWM_InitChannel(&TIM3_Data, TM_PWM_Channel_1, TM_PWM_PinsPack_1); // 1 Motor REAR
	TM_PWM_InitChannel(&TIM3_Data, TM_PWM_Channel_2, TM_PWM_PinsPack_1); // 2 Motor REAR
	TM_PWM_InitChannel(&TIM3_Data, TM_PWM_Channel_3, TM_PWM_PinsPack_1); // 3 Motor REAR
	TM_PWM_InitChannel(&TIM3_Data, TM_PWM_Channel_4, TM_PWM_PinsPack_1); // 4 Motor REAR
	initSpeeds();
}

void initMotorStruct(MotorState* Motor_Data)
{
	Motor_Data->Mode = Mode_Stop;
	Motor_Data->Direction = Direction_None;
	Motor_Data->Speed = 0;
}

void initSpeeds(void)
{
	speeds[0] = TIM2_Data.Period;
	speeds[1] = (uint32_t)((TIM2_Data.Period - 1) * 500) / MOTOR_PWM_FREQ;
	speeds[2] = (uint32_t)((TIM2_Data.Period - 1) * 375) / MOTOR_PWM_FREQ;
	speeds[3] = (uint32_t)((TIM2_Data.Period - 1) * 250) / MOTOR_PWM_FREQ;
	speeds[4] = (uint32_t)((TIM2_Data.Period - 1) * 125) / MOTOR_PWM_FREQ;
	speeds[5] = 0;
}

/*
 * increasing speed for 'duration' in sec
 */
void SoftStart(uint32_t duration)
{
	uint32_t speed, delay;
	uint16_t speed_count = sizeof(speeds) / sizeof(uint16_t);
	delay = floor(duration * 1000 / speed_count);
	Motor1FwdOn();
	Motor2FwdOn();
	Motor3FwdOn();
	Motor4FwdOn();
	for (speed=1;speed<=speed_count;speed++)
	{
		TIM2->CCR1 = speeds[speed];
		TIM2->CCR2 = speeds[speed];
		TIM2->CCR3 = speeds[speed];
		TIM2->CCR4 = speeds[speed];
		Delay(delay);
	}
}

void SoftStop(uint32_t duration)
{
	uint32_t speed, delay;
	uint16_t speed_count = sizeof(speeds) / sizeof(uint16_t);
	delay = floor(duration * 1000 / speed_count);
	for (speed=speed_count;speed>=0;speed--)
	{
		TIM2->CCR1 = speeds[speed];
		TIM2->CCR2 = speeds[speed];
		TIM2->CCR3 = speeds[speed];
		TIM2->CCR4 = speeds[speed];
		Delay(delay);
	}
	Motor1Stop();
	Motor2Stop();
	Motor3Stop();
	Motor4Stop();
}

void Stop(void)
{
	Motor1Stop();
	Motor2Stop();
	Motor3Stop();
	Motor4Stop();
}

void Forward(uint8_t speed)
{
	Stop();
	SoftStart(1);
}

void Back(uint8_t speed)
{

	Motor1BackOn();
	Motor2BackOn();
	Motor3BackOn();
	Motor4BackOn();
	if (speed != 1)
	{
		Motor1Speed(1);
		Motor3Speed(1);
		Motor2Speed(1);
		Motor4Speed(1);
	}
	Delay(MOTOR_DELAY_AFTER_START);
	Motor4Speed(speed);
	Motor2Speed(speed);
	Motor3Speed(speed);
	Motor1Speed(speed);
}

void FwdLeft(uint8_t angle)
{
	if (Motor1.Direction == Direction_Forward)
	{
		Motor1Stop();
		Motor4Stop();
		Delay(1000);
		Motor1FwdOn();Motor1Speed(1);
		Motor4FwdOn();Motor4Speed(1);
	}
}

void FwdRight(uint8_t angle)
{
	if (Motor1.Direction == Direction_Forward)
	{
		Motor2Stop();
		Motor3Stop();
		Delay(1000);
		Motor2FwdOn();Motor2Speed(1);
		Motor3FwdOn();Motor3Speed(1);
	}
}

void Motor1FwdOn(void)
{
	Motor1.Mode = Mode_Active;
	Motor1.Direction = Direction_Forward;
	Motor1.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM2_Data, TM_PWM_Channel_1, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor1Speed(uint16_t speed)
{
	if ((Motor1.Mode == Mode_Stop) || (Motor1.Direction == Direction_None)) return;
	Motor1.Speed = speed;
	if (Motor1.Direction == Direction_Forward)
		TIM2->CCR1 = speeds[speed];
	else if (Motor1.Direction == Direction_Backward)
		TIM3->CCR1 = speeds[speed];
}

void Motor1BackOn(void)
{
	Motor1.Mode = Mode_Active;
	Motor1.Direction = Direction_Backward;
	Motor1.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM3_Data, TM_PWM_Channel_1, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor1Stop(void)
{
	Motor1.Mode = Mode_Stop;
	Motor1.Direction = Direction_None;
	Motor1.Speed = 0;
	TIM2->CCR1 = speeds[0];

	TIM3->CCR1 = speeds[0];
}

void Motor2FwdOn(void)
{
	Motor2.Mode = Mode_Active;
	Motor2.Direction = Direction_Forward;
	Motor2.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM2_Data, TM_PWM_Channel_2, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor2Speed(uint16_t speed)
{
	if ((Motor2.Mode == Mode_Stop) || (Motor2.Direction == Direction_None)) return;
	Motor2.Speed = speed;
	if (Motor2.Direction == Direction_Forward)
		TIM2->CCR2 = speeds[speed];
	else if (Motor2.Direction == Direction_Backward)
		TIM3->CCR2 = speeds[speed];
}

void Motor2BackOn(void)
{
	Motor2.Mode = Mode_Active;
	Motor2.Direction = Direction_Backward;
	Motor2.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM3_Data, TM_PWM_Channel_2, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor2Stop(void)
{
	Motor2.Mode = Mode_Stop;
	Motor2.Direction = Direction_None;
	Motor2.Speed = 0;
	TIM2->CCR2 = speeds[0];

	TIM3->CCR2 = speeds[0];
}

void Motor3FwdOn(void)
{
	Motor3.Mode = Mode_Active;
	Motor3.Direction = Direction_Forward;
	Motor3.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM2_Data, TM_PWM_Channel_3, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor3Speed(uint16_t speed)
{
	if ((Motor3.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	Motor3.Speed = speed;
	if (Motor3.Direction == Direction_Forward)
		TIM2->CCR3 = speeds[speed];
	else if (Motor3.Direction == Direction_Backward)
		TIM3->CCR3 = speeds[speed];
}

void Motor3BackOn(void)
{
	Motor3.Mode = Mode_Active;
	Motor3.Direction = Direction_Backward;
	Motor3.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM3_Data, TM_PWM_Channel_3, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor3Stop(void)
{
	Motor3.Mode = Mode_Stop;
	Motor3.Direction = Direction_None;
	Motor3.Speed = 0;
	TIM2->CCR3 = speeds[0];

	TIM3->CCR3 = speeds[0];
}

void Motor4FwdOn(void)
{
	Motor4.Mode = Mode_Active;
	Motor4.Direction = Direction_Forward;
	Motor4.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM2_Data, TM_PWM_Channel_4, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor4Speed(uint16_t speed)
{
	if ((Motor4.Mode == Mode_Stop) || (Motor4.Direction == Direction_None)) return;
	Motor4.Speed = speed;
	if (Motor4.Direction == Direction_Forward)
		TIM2->CCR4 = speeds[speed];
	else if (Motor4.Direction == Direction_Backward)
		TIM4->CCR1 = speeds[speed];
}

void Motor4BackOn(void)
{
	Motor4.Mode = Mode_Active;
	Motor4.Direction = Direction_Backward;
	Motor4.Speed = 1;
	TM_PWM_SetChannelMicros(&TIM3_Data, TM_PWM_Channel_4, MOTOR_PWM_MAX_PULS_WIDTH);
}

void Motor4Stop(void)
{
	Motor4.Mode = Mode_Stop;
	Motor4.Direction = Direction_None;
	Motor4.Speed = 0;
	TIM2->CCR4 = speeds[0];

	TIM3->CCR4 = speeds[0];
}
