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

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#include "motors.h"
#include "delay.h"
#include "main.h"
#include "pwm.h"

Motor Motor1, Motor2, Motor3, Motor4;
PWM_Data PWM_Data_Motor_Fwd, PWM_Data_Motor_Back;

void calcSpeeds(void)
{
	uint32_t NullSpeed = (uint32_t)(1000000 / MOTOR_PWM_FREQ);
	speeds[0] = NullSpeed;	// 100%duty cycle, constant high level, motors stopped
	speeds[1] = MOTOR_PWM_MAX_PULSE_WIDTH;	//~500us	60% duty cycle, maximum pulse length, when motors is starts
	speeds[2] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 3);	//maximum pulse length divided by 4	~375us
	speeds[3] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 2);	//~250us
	speeds[4] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 1);	//~125us
	speeds[5] = MOTOR_PWM_MIN_PULSE_WIDTH;	//0% duty cycle, constant low level, full speed
	max_speed = 5;
}

/*
 * stops all motors
 */
void Stop(void)
{
	Motor1Stop();
	Motor2Stop();
	Motor3Stop();
	Motor4Stop();
}

/*
 *starts all motors forward
 */
void Forward(uint8_t speed)
{
	if (Motor1.Direction == Direction_Forward || Motor2.Direction == Direction_Forward) return;
	Stop();
	Motor1Start(Direction_Forward, 1);
	Motor2Start(Direction_Forward, 1);
	Motor3Start(Direction_Forward, 1);
	Motor4Start(Direction_Forward, 1);
	if (speed != 1)	//if speed more then 1, make soft start
	{
		Delay(MOTOR_DELAY_AFTER_START);
		Motor1Speed(speed);
		Motor2Speed(speed);
		Motor3Speed(speed);
		Motor4Speed(speed);
	}
}

/*
 *starts all motors backward
 */
void Back(uint8_t speed)
{
	if (Motor1.Direction == Direction_Backward || Motor2.Direction == Direction_Backward) return;
	Stop();
	Motor1Start(Direction_Backward, 1);
	Motor2Start(Direction_Backward, 1);
	Motor3Start(Direction_Backward, 1);
	Motor4Start(Direction_Backward, 1);
	if (speed != 1)	//if speed more then 1, make soft start
	{
		Delay(MOTOR_DELAY_AFTER_START);
		Motor1Speed(speed);
		Motor2Speed(speed);
		Motor3Speed(speed);
		Motor4Speed(speed);
	}
}

void Left()
{
	uint8_t motor1_speed = Motor1.Speed, motor2_speed = Motor2.Speed;	//Current states
	uint8_t motor3_speed = Motor3.Speed, motor4_speed = Motor4.Speed;
	if (((Motor2.Direction == Direction_Forward) && (Motor3.Direction == Direction_Forward)) ||
		((Motor2.Direction == Direction_Backward) && (Motor3.Direction == Direction_Backward)))
	{
		Motor1Speed(1);
		Motor4Speed(1);
		Motor2Speed(max_speed);
		Motor3Speed(max_speed);
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor1Speed(motor1_speed);
		Motor2Speed(motor2_speed);
		Motor3Speed(motor3_speed);
		Motor4Speed(motor4_speed);
	} else if ((Motor2.Direction == Direction_None) || (Motor2.Mode == Mode_Stop))
	{
		Motor1Start(Direction_Backward, 1);
		Motor2Start(Direction_Forward, 1);
		Motor3Start(Direction_Forward, 1);
		Motor4Start(Direction_Backward, 1);
		Delay(MOTOR_DELAY_FOR_TURNING);
		Stop();
	}
}

void Right()
{
	uint8_t motor1_speed = Motor1.Speed, motor2_speed = Motor2.Speed;	//Current states
	uint8_t motor3_speed = Motor3.Speed, motor4_speed = Motor4.Speed;
	if (((Motor1.Direction == Direction_Forward) && (Motor4.Direction == Direction_Forward)) ||
		((Motor1.Direction == Direction_Backward) && (Motor4.Direction == Direction_Backward)))
	{
		Motor2Speed(1);
		Motor3Speed(1);
		Motor1Speed(max_speed);
		Motor4Speed(max_speed);
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor1Speed(motor1_speed);
		Motor2Speed(motor2_speed);
		Motor3Speed(motor3_speed);
		Motor4Speed(motor4_speed);
	} else if ((Motor1.Direction == Direction_None) || (Motor4.Mode == Mode_Stop))
	{
		Motor1Start(Direction_Forward, 1);
		Motor2Start(Direction_Backward, 1);
		Motor3Start(Direction_Backward, 1);
		Motor4Start(Direction_Forward, 1);
		Delay(MOTOR_DELAY_FOR_TURNING);
		Stop();
	}
}

//-------------------------------------------------------------------------------------------------------
// Low Level functions
void initMotors(void)
{
	calcSpeeds();

	initMotorStruct(&Motor1);
	initMotorStruct(&Motor2);
	initMotorStruct(&Motor3);
	initMotorStruct(&Motor4);

	// Clocks
	calcPWMClocks(MOTOR_TIMER_FWD, &PWM_Data_Motor_Fwd, MOTOR_PWM_FREQ);
	calcPWMClocks(MOTOR_TIMER_BACK, &PWM_Data_Motor_Back, MOTOR_PWM_FREQ);

	initMotor1();
	initMotor2();
	initMotor3();
	initMotor4();
}

void initMotorStruct(Motor* Motor_Data)
{
	Motor_Data->Mode = Mode_Stop;
	Motor_Data->Direction = Direction_None;
	Motor_Data->Speed = 0;
}

void initMotor1(void)
{
	Motor1.Num = 1;
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_0, GPIO_PinSource0, MOTOR_TIMER_FWD_AF);
	initPWM(MOTOR_TIMER_FWD, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 1);
	//Backward
	initMotorPin(GPIOA, GPIO_Pin_6, GPIO_PinSource6, MOTOR_TIMER_BACK_AF);
	initPWM(MOTOR_TIMER_BACK, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 1);
}

void Motor1Start(uint8_t Direction, uint8_t speed)
{
	if (Motor1.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor1.Mode = Mode_Active;
		Motor1.Direction = Direction_Forward;
		Motor1.Speed = speed;
		MOTOR_TIMER_FWD->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor1.Mode = Mode_Active;
		Motor1.Direction = Direction_Backward;
		Motor1.Speed = speed;
		MOTOR_TIMER_BACK->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor1);
}

void Motor1Speed(uint8_t speed)
{
	if ((Motor1.Mode == Mode_Stop) || (Motor1.Direction == Direction_None)) return;
	if (Motor1.Speed == speed) return;
	Motor1.Speed = speed;
	if (Motor1.Direction == Direction_Forward)
	{
		Motor1.Speed = speed;
		MOTOR_TIMER_FWD->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor1.Direction == Direction_Backward)
	{
		Motor1.Speed = speed;
		MOTOR_TIMER_BACK->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor1);
}

void Motor1Stop(void)
{
	if ((Motor1.Mode == Mode_Stop) || (Motor1.Direction == Direction_None)) return;
	if (Motor1.Direction == Direction_Forward)
		MOTOR_TIMER_FWD->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor1.Direction == Direction_Backward)
		MOTOR_TIMER_BACK->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor1.Mode = Mode_Stop;
	Motor1.Direction = Direction_None;
	Motor1.Speed = 0;
	rc_printMotorState(Motor1);
}

void initMotor2(void)
{
	Motor2.Num = 2;
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_1, GPIO_PinSource1, MOTOR_TIMER_FWD_AF);
	initPWM(MOTOR_TIMER_FWD, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 2);
	//Backward
	initMotorPin(GPIOA, GPIO_Pin_7, GPIO_PinSource7, MOTOR_TIMER_BACK_AF);
	initPWM(MOTOR_TIMER_BACK, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 2);
}

void Motor2Start(uint8_t Direction, uint8_t speed)
{
	if (Motor2.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor2.Mode = Mode_Active;
		Motor2.Direction = Direction_Forward;
		Motor2.Speed = speed;
		MOTOR_TIMER_FWD->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor2.Mode = Mode_Active;
		Motor2.Direction = Direction_Backward;
		Motor2.Speed = speed;
		MOTOR_TIMER_BACK->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor2);
}

void Motor2Speed(uint8_t speed)
{
	if ((Motor2.Mode == Mode_Stop) || (Motor2.Direction == Direction_None)) return;
	if (Motor2.Speed == speed) return;
	Motor2.Speed = speed;
	if (Motor2.Direction == Direction_Forward)
	{
		Motor2.Speed = speed;
		MOTOR_TIMER_FWD->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor2.Direction == Direction_Backward)
	{
		Motor2.Speed = speed;
		MOTOR_TIMER_BACK->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor2);
}

void Motor2Stop(void)
{
	if ((Motor2.Mode == Mode_Stop) || (Motor2.Direction == Direction_None)) return;
	if (Motor2.Direction == Direction_Forward)
		MOTOR_TIMER_FWD->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor2.Direction == Direction_Backward)
		MOTOR_TIMER_BACK->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor2.Mode = Mode_Stop;
	Motor2.Direction = Direction_None;
	Motor2.Speed = 0;
	rc_printMotorState(Motor2);
}

void initMotor3(void)
{
	Motor3.Num = 3;
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_2, GPIO_PinSource2, MOTOR_TIMER_FWD_AF);
	initPWM(MOTOR_TIMER_FWD, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 3);
	//Backward
	initMotorPin(GPIOB, GPIO_Pin_0, GPIO_PinSource0, MOTOR_TIMER_BACK_AF);
	initPWM(MOTOR_TIMER_BACK, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 3);
}

void Motor3Start(uint8_t Direction, uint8_t speed)
{
	if (Motor3.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor3.Mode = Mode_Active;
		Motor3.Direction = Direction_Forward;
		Motor3.Speed = speed;
		MOTOR_TIMER_FWD->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor3.Mode = Mode_Active;
		Motor3.Direction = Direction_Backward;
		Motor3.Speed = speed;
		MOTOR_TIMER_BACK->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor3);
}

void Motor3Speed(uint8_t speed)
{
	if ((Motor3.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	if (Motor3.Speed == speed) return;
	Motor3.Speed = speed;
	if (Motor3.Direction == Direction_Forward)
	{
		Motor3.Speed = speed;
		MOTOR_TIMER_FWD->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor3.Direction == Direction_Backward)
	{
		Motor3.Speed = speed;
		MOTOR_TIMER_BACK->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor3);
}

void Motor3Stop(void)
{
	if ((Motor3.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	if (Motor3.Direction == Direction_Forward)
		MOTOR_TIMER_FWD->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor3.Direction == Direction_Backward)
		MOTOR_TIMER_BACK->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor3.Mode = Mode_Stop;
	Motor3.Direction = Direction_None;
	Motor3.Speed = 0;
	rc_printMotorState(Motor3);
}

void initMotor4(void)
{
	Motor4.Num = 4;
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_3, GPIO_PinSource3, MOTOR_TIMER_FWD_AF);
	initPWM(MOTOR_TIMER_FWD, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 4);
	//Backward
	initMotorPin(GPIOB, GPIO_Pin_1, GPIO_PinSource1, MOTOR_TIMER_BACK_AF);
	initPWM(MOTOR_TIMER_BACK, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 4);
}

void Motor4Start(uint8_t Direction, uint8_t speed)
{
	if (Motor4.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor4.Mode = Mode_Active;
		Motor4.Direction = Direction_Forward;
		Motor4.Speed = speed;
		MOTOR_TIMER_FWD->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor4.Mode = Mode_Active;
		Motor4.Direction = Direction_Backward;
		Motor4.Speed = speed;
		MOTOR_TIMER_BACK->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor4);
}

void Motor4Speed(uint8_t speed)
{
	if ((Motor4.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	if (Motor4.Speed == speed) return;
	Motor4.Speed = speed;
	if (Motor4.Direction == Direction_Forward)
	{
		Motor4.Speed = speed;
		MOTOR_TIMER_FWD->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor4.Direction == Direction_Backward)
	{
		Motor4.Speed = speed;
		MOTOR_TIMER_BACK->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	rc_printMotorState(Motor4);
}

void Motor4Stop(void)
{
	if ((Motor4.Mode == Mode_Stop) || (Motor4.Direction == Direction_None)) return;
	if (Motor4.Direction == Direction_Forward)
		MOTOR_TIMER_FWD->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor4.Direction == Direction_Backward)
		MOTOR_TIMER_BACK->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor4.Mode = Mode_Stop;
	Motor4.Direction = Direction_None;
	Motor4.Speed = 0;
	rc_printMotorState(Motor4);
}
