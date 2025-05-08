/*
 * Servo.c
 *
 *  Created on: May 1, 2025
 *      Author: HUNG
 */
#include "Servo.h"
#define MIN_PULSE_WIDTH 544
#define MAX_PULSE_WIDTH 2400
// chuyển đổi 0 -> 180 thành 0 đến 2000 ( 2ms )
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
uint8_t servo_read(Servo* servo)
{
	return servo->angle;
}
void servo_write(Servo* servo, uint8_t angle)
{
	uint16_t ccr = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
	servo->angle = angle;
	switch(servo->channel)
	{
	case TIM_CHANNEL_1:
		servo->htim->Instance->CCR1 = ccr;
		break;
	case TIM_CHANNEL_2:
		servo->htim->Instance->CCR2 = ccr;
		break;
	case TIM_CHANNEL_3:
		servo->htim->Instance->CCR3 = ccr;
		break;
	case TIM_CHANNEL_4:
		servo->htim->Instance->CCR4 = ccr;
		break;
	}
}
void servo_init(Servo* servo, TIM_HandleTypeDef *htim,uint16_t channel)
{
	servo->htim = htim;
	servo->channel = channel;
	servo->angle = 0;
	htim->Instance->ARR = 19999;
	htim->Instance->PSC = 63;
	HAL_TIM_PWM_Start(htim, channel);
}


