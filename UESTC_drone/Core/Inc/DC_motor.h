/*
 * DC_motor.h
 *
 *  Created on: Jul 11, 2025
 *      Author: hugoc
 */

#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include <stdint.h>

// Initialise les PWM (doit être appelé après init des timers HAL)
void DC_Motor_Init(void);

// Fixe instantanément le rapport cyclique d’un moteur (0-100%)
void DC_Motor_SetDuty(uint8_t motor_id, float duty_percent);

// Lance une rampe progressive de PWM (non bloquante)
// motor_id: 1..4
// start_percent: duty de départ (0-100)
// target_percent: duty cible (0-100)
// step_percent: incrément de duty à chaque intervalle
// interval_ms: durée entre chaque incrément (en ms)
void DC_Motor_StartRamp(uint8_t motor_id, float start_percent, float target_percent, float step_percent, uint32_t interval_ms);

// Doit être appelé dans SysTick_Handler (toutes les 1 ms)
void DC_Motor_RampTick(void);

#endif
