/*
 * hb_drivers.c
 *
 *  Created on: 30-Aug-2026
 *      Author: HP
 */

#include <motor_control.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

static MotorState_t motorState = MOTOR_STOP;
static MotorState_t requestedState = MOTOR_STOP;
static uint32_t motorSpeed = 0U;
static volatile uint8_t deadTimeExpire = 0U;

static void Motor_PWM_AllOff(void)
{
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

static void Motor_Start_DeadTime(MotorState_t nextState)
{
	Motor_PWM_AllOff();
	requestedState = nextState;
	deadTimeExpire = 0U;
	motorState = MOTOR_DEADTIME;

	__HAL_TIM_SET_COUNTER(&htim2, 0U);

	__HAL_TIM_SET_AUTORELOAD(&htim2, MOTOR_DEAD_TIME_MS - 1);

	HAL_TIM_Base_Start_IT(&htim2);
}

void Motor_Init(void)
{
	Motor_PWM_AllOff();

	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void Motor_Set_State(MotorState_t state)
{
	if((state == motorState) && (state != MOTOR_DEADTIME))
	{
		return;
	}

	switch(state)
	{
	case MOTOR_FORWARD:
		if(motorState == MOTOR_REVERSE)
		{
			Motor_Start_DeadTime(MOTOR_FORWARD);
			return;
		}

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorSpeed);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);

		motorState = MOTOR_FORWARD;

		break;

	case MOTOR_REVERSE:
		if(motorState == MOTOR_FORWARD)
		{
			Motor_Start_DeadTime(MOTOR_REVERSE);
			return;
		}

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorSpeed);

		motorState = MOTOR_REVERSE;

		break;

	case MOTOR_STOP:
		Motor_PWM_AllOff();

		motorState = MOTOR_STOP;

		break;

	case MOTOR_DEADTIME:
		Motor_PWM_AllOff();

		motorState = MOTOR_DEADTIME;

		break;

	default:
		Motor_PWM_AllOff();

		motorState = MOTOR_STOP;

		break;
	}
}

void Motor_Set_Speed(uint32_t speed)
{
	if(speed <= __HAL_TIM_GET_AUTORELOAD(&htim1))
	{
		motorSpeed = speed;
		if(motorState == MOTOR_FORWARD)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorSpeed);
		}
		else if(motorState == MOTOR_REVERSE)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorSpeed);
		}
	}
}

void Motor_Process(void)
{
	if(motorState != MOTOR_DEADTIME)
	{
		return;
	}

	if(deadTimeExpire == 0U)
	{
		return;
	}

	deadTimeExpire = 0U;

	Motor_Set_State(requestedState);
}

MotorState_t Motor_Get_State(void)
{
	return motorState;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim2)
{
	if(htim2->Instance == TIM2)
	{
		HAL_TIM_Base_Stop_IT(&htim);
		deadTimeExpire = 1U;
	}
}

















