/*
 * hb_drivers.h
 *
 *  Created on: 30-Aug-2026
 *      Author: HP
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#include "main.h"

#define MOTOR_DEAD_TIME_MS 10U

typedef enum
{
	MOTOR_STOP,
	MOTOR_FORWARD,
	MOTOR_DEADTIME,
	MOTOR_REVERSE
}MotorState_t;

void Motor_init(void);
void Motor_Set_State(MotorState_t state);
void Motor_Set_Speed(uint32_t speed);

void Motor_Process(void);

MotorState_t motor_GetState(void);

#endif /* INC_MOTOR_CONTROL_H_ */
