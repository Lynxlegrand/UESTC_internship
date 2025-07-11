/*
 * DC_motor.c
 *
 *  Created on: Jul 11, 2025
 *      Author: hugoc
 */


#include "stm32f1xx_hal.h"
#include "DC_motor.h"

// Extern : handles HAL timer, déclarés dans main.c ou ailleurs
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

typedef struct {
    float current_duty;
    float target_duty;
    float step;
    uint32_t interval_ms;
    uint32_t counter_ms;
    uint8_t active;
} Ramp_t;

static Ramp_t ramps[4] = {0};

void DC_Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);

    for (int i = 0; i < 4; i++) {
        ramps[i].active = 0;
    }
}

void DC_Motor_SetDuty(uint8_t motor_id, float duty)
{
    if (motor_id < 1 || motor_id > 4) return;

    if (duty < 0) duty = 0;
    if (duty > 100) duty = 100;

    // ARR = 9 → période PWM = 10 ticks (0..9)
    uint32_t pulse = (uint32_t)((duty * 10) / 100);

    switch (motor_id) {
        case 1: TIM1->CCR1 = pulse; break;
        case 2: TIM2->CCR3 = pulse; break;
        case 3: TIM3->CCR1 = pulse; break;
        case 4: TIM4->CCR3 = pulse; break;
    }
}

void DC_Motor_StartRamp(uint8_t motor_id, float start, float target, float step, uint32_t interval_ms)
{
    if (motor_id < 1 || motor_id > 4) return;
    if (step <= 0) return;

    Ramp_t *r = &ramps[motor_id - 1];
    r->current_duty = start;
    r->target_duty = target;
    r->step = step;
    r->interval_ms = interval_ms;
    r->counter_ms = 0;
    r->active = 1;

    DC_Motor_SetDuty(motor_id, start);
}

void DC_Motor_RampTick(void)
{
    for (int i = 0; i < 4; i++) {
        Ramp_t *r = &ramps[i];
        if (!r->active) continue;

        r->counter_ms++;
        if (r->counter_ms >= r->interval_ms) {
            r->counter_ms = 0;

            if (r->current_duty < r->target_duty) {
                r->current_duty += r->step;
                if (r->current_duty > r->target_duty)
                    r->current_duty = r->target_duty;
            }
            else if (r->current_duty > r->target_duty) {
                r->current_duty -= r->step;
                if (r->current_duty < r->target_duty)
                    r->current_duty = r->target_duty;
            }
            else {
                r->active = 0; // fin rampe
            }

            DC_Motor_SetDuty(i + 1, r->current_duty);
        }
    }
}
