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

MotorState Motor1, Motor2, Motor3, Motor4;
PWM_Data PWM_Data_Motor_Fwd, PWM_Data_Motor_Back;

void calcSpeeds(void)
{
	uint32_t NullSpeed = (uint32_t)(1000000 / MOTOR_PWM_FREQ);
	speeds[0] = NullSpeed;	// 100%duty cycle, constant high level, motors stopped
	speeds[1] = MOTOR_PWM_MAX_PULSE_WIDTH;	//~500us	50% duty cycle, maximum pulse length, when motors is starts
	speeds[2] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 3);	//maximum pulse length divided by 4	~375us
	speeds[3] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 2);	//~250us
	speeds[4] = (uint32_t)(MOTOR_PWM_MAX_PULSE_WIDTH / 4 * 1);	//~125us
	speeds[5] = MOTOR_PWM_MIN_PULSE_WIDTH;	//0% duty cycle, constant low level, full speed
}

/*
 * increasing speed for 'duration' in sec
 */
void SoftStart(uint32_t Direction, uint32_t duration)
{
	uint32_t speed, delay;
	uint16_t speed_count = sizeof(speeds) / sizeof(uint32_t);
	delay = floor(duration * 1000 / speed_count);
	Motor1Start(Direction, 1);
	Motor2Start(Direction, 1);
	Motor3Start(Direction, 1);
	Motor4Start(Direction, 1);
	for (speed=1;speed<=speed_count;speed++)
	{
		Motor1Speed(speed);
		Motor2Speed(speed);
		Motor3Speed(speed);
		Motor4Speed(speed);
		Delay(delay);
	}
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

void Left(uint8_t angle)
{
	uint32_t motor1_speed = Motor1.Speed, motor4_speed = Motor4.Speed;
	if ((Motor2.Direction == Direction_Forward) && (Motor3.Direction == Direction_Forward))
	{
		Motor1Stop();
		Motor4Stop();
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor1Start(Direction_Forward, motor1_speed);
		Motor4Start(Direction_Forward, motor4_speed);
	} else if ((Motor2.Direction == Direction_Backward) && (Motor3.Direction == Direction_Backward))
	{
		Motor1Stop();
		Motor4Stop();
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor1Start(Direction_Backward, motor1_speed);
		Motor4Start(Direction_Backward, motor4_speed);
	}
}

void Right(uint8_t angle)
{
	uint32_t motor2_speed = Motor1.Speed, motor3_speed = Motor4.Speed;
	if ((Motor1.Direction == Direction_Forward) && (Motor4.Direction == Direction_Forward))
	{
		Motor2Stop();
		Motor3Stop();
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor2Start(Direction_Forward, motor2_speed);
		Motor3Start(Direction_Forward, motor3_speed);
	} else if ((Motor1.Direction == Direction_Backward) && (Motor4.Direction == Direction_Backward))
	{
		Motor2Stop();
		Motor3Stop();
		Delay(MOTOR_DELAY_FOR_TURNING);
		Motor2Start(Direction_Backward, motor2_speed);
		Motor3Start(Direction_Backward, motor3_speed);
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
	initPWMClocks(TIM2, &PWM_Data_Motor_Fwd, MOTOR_PWM_FREQ);
	initPWMClocks(TIM3, &PWM_Data_Motor_Back, MOTOR_PWM_FREQ);

	initMotor1();
	initMotor2();
	initMotor3();
	initMotor4();
}

void initMotorStruct(MotorState* Motor_Data)
{
	Motor_Data->Mode = Mode_Stop;
	Motor_Data->Direction = Direction_None;
	Motor_Data->Speed = 0;
}

void initPWMClocks(TIM_TypeDef* TIMx, PWM_Data * ClocksData, uint32_t Frequency)
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

void initMotor1(void)
{
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_0, GPIO_PinSource0, GPIO_AF_TIM2);
	initPWM(TIM2, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 1);
	//Backward
	initMotorPin(GPIOA, GPIO_Pin_6, GPIO_PinSource6, GPIO_AF_TIM3);
	initPWM(TIM3, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 1);
}

void Motor1Start(uint32_t Direction, uint8_t speed)
{
	if (Motor1.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor1.Mode = Mode_Active;
		Motor1.Direction = Direction_Forward;
		Motor1.Speed = speed;
		TIM2->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor1.Mode = Mode_Active;
		Motor1.Direction = Direction_Backward;
		Motor1.Speed = speed;
		TIM3->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor1Speed(uint8_t speed)
{
	if ((Motor1.Mode == Mode_Stop) || (Motor1.Direction == Direction_None)) return;
	Motor1.Speed = speed;
	if (Motor1.Direction == Direction_Forward)
	{
		Motor1.Speed = speed;
		TIM2->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor1.Direction == Direction_Backward)
	{
		Motor1.Speed = speed;
		TIM3->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor1Stop(void)
{
	if ((Motor1.Mode == Mode_Stop) || (Motor1.Direction == Direction_None)) return;
	if (Motor1.Direction == Direction_Forward)
		TIM2->CCR1 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor1.Direction == Direction_Backward)
		TIM3->CCR1 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor1.Mode = Mode_Stop;
	Motor1.Direction = Direction_None;
	Motor1.Speed = 0;
}

void initMotor2(void)
{
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_1, GPIO_PinSource1, GPIO_AF_TIM2);
	initPWM(TIM2, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 2);
	//Backward
	initMotorPin(GPIOA, GPIO_Pin_7, GPIO_PinSource7, GPIO_AF_TIM3);
	initPWM(TIM3, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 2);
}

void Motor2Start(uint32_t Direction, uint8_t speed)
{
	if (Motor2.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor2.Mode = Mode_Active;
		Motor2.Direction = Direction_Forward;
		Motor2.Speed = speed;
		TIM2->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor2.Mode = Mode_Active;
		Motor2.Direction = Direction_Backward;
		Motor2.Speed = speed;
		TIM3->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor2Speed(uint8_t speed)
{
	if ((Motor2.Mode == Mode_Stop) || (Motor2.Direction == Direction_None)) return;
	Motor2.Speed = speed;
	if (Motor2.Direction == Direction_Forward)
	{
		Motor2.Speed = speed;
		TIM2->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor2.Direction == Direction_Backward)
	{
		Motor2.Speed = speed;
		TIM3->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor2Stop(void)
{
	if ((Motor2.Mode == Mode_Stop) || (Motor2.Direction == Direction_None)) return;
	if (Motor2.Direction == Direction_Forward)
		TIM2->CCR2 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor2.Direction == Direction_Backward)
		TIM3->CCR2 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor2.Mode = Mode_Stop;
	Motor2.Direction = Direction_None;
	Motor2.Speed = 0;
}

void initMotor3(void)
{
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_2, GPIO_PinSource2, GPIO_AF_TIM2);
	initPWM(TIM2, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 3);
	//Backward
	initMotorPin(GPIOB, GPIO_Pin_0, GPIO_PinSource0, GPIO_AF_TIM3);
	initPWM(TIM3, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 3);
}

void Motor3Start(uint32_t Direction, uint8_t speed)
{
	if (Motor3.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor3.Mode = Mode_Active;
		Motor3.Direction = Direction_Forward;
		Motor3.Speed = speed;
		TIM2->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor3.Mode = Mode_Active;
		Motor3.Direction = Direction_Backward;
		Motor3.Speed = speed;
		TIM3->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor3Speed(uint8_t speed)
{
	if ((Motor3.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	Motor3.Speed = speed;
	if (Motor3.Direction == Direction_Forward)
	{
		Motor3.Speed = speed;
		TIM2->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor3.Direction == Direction_Backward)
	{
		Motor3.Speed = speed;
		TIM3->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor3Stop(void)
{
	if ((Motor3.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	if (Motor3.Direction == Direction_Forward)
		TIM2->CCR3 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor3.Direction == Direction_Backward)
		TIM3->CCR3 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor3.Mode = Mode_Stop;
	Motor3.Direction = Direction_None;
	Motor3.Speed = 0;
}

void initMotor4(void)
{
	//Forward
	initMotorPin(GPIOA, GPIO_Pin_3, GPIO_PinSource3, GPIO_AF_TIM2);
	initPWM(TIM2, PWM_Data_Motor_Fwd.Period, PWM_Data_Motor_Fwd.Prescaler, 4);
	//Backward
	initMotorPin(GPIOB, GPIO_Pin_1, GPIO_PinSource1, GPIO_AF_TIM3);
	initPWM(TIM3, PWM_Data_Motor_Back.Period, PWM_Data_Motor_Back.Prescaler, 4);
}

void Motor4Start(uint32_t Direction, uint8_t speed)
{
	if (Motor4.Mode == Mode_Active) return;
	if (Direction == Direction_Forward)
	{
		Motor4.Mode = Mode_Active;
		Motor4.Direction = Direction_Forward;
		Motor4.Speed = speed;
		TIM2->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	} else if (Direction == Direction_Backward)
	{
		Motor4.Mode = Mode_Active;
		Motor4.Direction = Direction_Backward;
		Motor4.Speed = speed;
		TIM3->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor4Speed(uint8_t speed)
{
	if ((Motor4.Mode == Mode_Stop) || (Motor3.Direction == Direction_None)) return;
	Motor4.Speed = speed;
	if (Motor4.Direction == Direction_Forward)
	{
		Motor4.Speed = speed;
		TIM2->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
	else if (Motor4.Direction == Direction_Backward)
	{
		Motor4.Speed = speed;
		TIM3->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[speed]) / MOTOR_PWM_FREQ;
	}
}

void Motor4Stop(void)
{
	if ((Motor4.Mode == Mode_Stop) || (Motor4.Direction == Direction_None)) return;
	if (Motor4.Direction == Direction_Forward)
		TIM2->CCR4 = (uint32_t)((PWM_Data_Motor_Fwd.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	else if (Motor4.Direction == Direction_Backward)
		TIM3->CCR4 = (uint32_t)((PWM_Data_Motor_Back.Period - 1) * speeds[0]) / MOTOR_PWM_FREQ;
	Motor4.Mode = Mode_Stop;
	Motor4.Direction = Direction_None;
	Motor4.Speed = 0;
}
