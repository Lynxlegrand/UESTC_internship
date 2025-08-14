/*
 * mainloop.c
 *
 *  Created on: Jul 25, 2025
 *      Author: hugoc
 */

#include "mainloop.h"

float motor_power_values[4];

void mainloop_drone_control(void) {
    static float pitch_offset = 0;
    static float roll_offset = 0;
    static float persistent_base_power = MOTORS_TAKEOFF_POWER;

    static float FR_power = MOTORS_TAKEOFF_POWER;
    static float FL_power = MOTORS_TAKEOFF_POWER;
    static float BR_power = MOTORS_TAKEOFF_POWER;
    static float BL_power = MOTORS_TAKEOFF_POWER;

    if (buttons.EMERGENCY_STOP_BUTTON) {
        flags.EMERGENCY_STOP = true;
        buttons.EMERGENCY_STOP_BUTTON = false;
        return;
    }
    if(flags.EMERGENCY_STOP){
    	emergency_stop();
    	flags.MOTORS_ON = false;
        flags.BLE_HAS_BEEN_DISCONNECTED = false;
        motor_power_values[0] = 0;
        motor_power_values[1] = 0;
        motor_power_values[2] = 0;
        motor_power_values[3] = 0;
    	if(buttons.FRONT_OFFSET_BUTTON){
    		flags.EMERGENCY_STOP = false;
    	}
    	return;
    }

    static uint32_t ble_disconnect_time_ms = 0;

    if (flags.BLE_HAS_BEEN_DISCONNECTED && flags.MOTORS_ON) {
            if (ble_disconnect_time_ms == 0) {
                ble_disconnect_time_ms = HAL_GetTick(); // première détection
            } else if (HAL_GetTick() - ble_disconnect_time_ms >= BLE_TIMEOUT_MS) {
                flags.EMERGENCY_STOP = true;
                flags.BLE_HAS_BEEN_DISCONNECTED = false; // on évite de relancer plusieurs fois
                ble_disconnect_time_ms = 0;
                return;
            }
        } else {
            // BLE reconnectée, on réinitialise le timer de déconnexion
            ble_disconnect_time_ms = 0;
        }



    if (!(flags.MOTORS_ON) && buttons.FRONT_OFFSET_BUTTON) {
        decollage();
        motor_power_values[0] = MOTORS_TAKEOFF_POWER;
        motor_power_values[1] = MOTORS_TAKEOFF_POWER;
        motor_power_values[2] = MOTORS_TAKEOFF_POWER;
        motor_power_values[3] = MOTORS_TAKEOFF_POWER;
        flags.MOTORS_ON = true;
        flags.EMERGENCY_STOP = false;
        buttons.FRONT_OFFSET_BUTTON = false;
        buttons.BACK_OFFSET_BUTTON = false;
        buttons.LEFT_OFFSET_BUTTON = false;
        buttons.EMERGENCY_STOP_BUTTON = false;
        buttons.RIGHT_OFFSET_BUTTON = false;
        return;
    }

    if (flags.RAMPE_EN_COURS) {
        return;
    }

    if (flags.MOTORS_ON && !(flags.EMERGENCY_STOP)) {
        // --- Gestion des offsets fixes ---
        if (buttons.FRONT_OFFSET_BUTTON) {
            pitch_offset += 1;
            buttons.FRONT_OFFSET_BUTTON = false;
        }
        if (buttons.BACK_OFFSET_BUTTON) {
            pitch_offset -= 1;
            buttons.BACK_OFFSET_BUTTON = false;
        }
        if (buttons.RIGHT_OFFSET_BUTTON) {
            roll_offset += 1;
            buttons.RIGHT_OFFSET_BUTTON = false;
        }
        if (buttons.LEFT_OFFSET_BUTTON) {
            roll_offset -= 1;
            buttons.LEFT_OFFSET_BUTTON = false;
        }

        // On borne les offsets pour qu'il n'y ait pas trop de déséquilibres
        const float max_offset_number = MAX_OFFSET_NUMBER;

        pitch_offset = fminf(fmaxf(pitch_offset, -max_offset_number), max_offset_number);
        roll_offset = fminf(fmaxf(roll_offset, -max_offset_number), max_offset_number);


        // Coefficients
        const float k_z = HEIGHT_SENSIBILITY;
        const float k_translation = TRANSLATION_SENSITIVITY; // par exemple 200.0f
        const float k_yaw = YAW_SENSITIVITY;                 // par exemple 150.0f
        const float k_offset = OFFSET_SENSITIVITY;

        const int center_z = CENTER_Z;
        const int center_yaw = CENTER_YAW;
        const int center_x = CENTER_X;
        const int center_y = CENTER_Y;

        const int deadzone_z = DEADZONE_Z;
        const int deadzone_yaw = DEADZONE_YAW;
        const int deadzone_x = DEADZONE_X;
        const int deadzone_y = DEADZONE_Y;

        // Lecture des joysticks avec zones mortes
        float z_input = normalize_with_deadzone(adcData_2[0], center_z, deadzone_z);
        float yaw = normalize_with_deadzone(adcData_2[1], center_yaw, deadzone_yaw);
        float trans_x = normalize_with_deadzone(adcData_2[3], center_x, deadzone_x); // avant/arrière
        float trans_y = normalize_with_deadzone(adcData_2[2], center_y, deadzone_y); // droite/gauche

        // Changements de sens (si nécessaires)
        z_input = -z_input;
        trans_x = -trans_x;
        // yaw et trans_y restent tels quels

        // Mise à jour de la base de puissance persistante
        if (fabsf(z_input) > 0.01f) {
            persistent_base_power += z_input * k_z;

            // Clamp entre 0 et 100
            if (persistent_base_power > 100.0f) persistent_base_power = 100.0f;
            if (persistent_base_power < 0.0f) persistent_base_power = 0.0f;
        }

        float base_power = persistent_base_power;

        // Calcul des contributions temporaires (déséquilibres joystick)
        float FR_temp = -trans_x * k_translation + trans_y * k_translation - yaw * k_yaw;
        float FL_temp = -trans_x * k_translation - trans_y * k_translation + yaw * k_yaw;
        float BR_temp = +trans_x * k_translation + trans_y * k_translation + yaw * k_yaw;
        float BL_temp = +trans_x * k_translation - trans_y * k_translation - yaw * k_yaw;

        // Limiter les déséquilibres temporaires
        const float max_temp_imbalance = MAX_TEMPORARY_IMBALANCE;

        FR_temp = fminf(fmaxf(FR_temp, -max_temp_imbalance), max_temp_imbalance);
        FL_temp = fminf(fmaxf(FL_temp, -max_temp_imbalance), max_temp_imbalance);
        BR_temp = fminf(fmaxf(BR_temp, -max_temp_imbalance), max_temp_imbalance);
        BL_temp = fminf(fmaxf(BL_temp, -max_temp_imbalance), max_temp_imbalance);

        // --- Puissance des moteurs ---
        FR_power = base_power + FR_temp + pitch_offset * k_offset - roll_offset * k_offset;
        FL_power = base_power + FL_temp + pitch_offset * k_offset + roll_offset * k_offset;
        BR_power = base_power + BR_temp - pitch_offset * k_offset - roll_offset * k_offset;
        BL_power = base_power + BL_temp - pitch_offset * k_offset + roll_offset * k_offset;

        // Moyenne des puissances
        float avg_power = round_to_2_decimals((FR_power + FL_power + BR_power + BL_power) / 4.0f);

        // Équilibrage autour de la moyenne
        const float max_diff = MAX_DIFF;

        FR_power = fminf(fmaxf(FR_power, avg_power - max_diff), avg_power + max_diff);
        FL_power = fminf(fmaxf(FL_power, avg_power - max_diff), avg_power + max_diff);
        BR_power = fminf(fmaxf(BR_power, avg_power - max_diff), avg_power + max_diff);
        BL_power = fminf(fmaxf(BL_power, avg_power - max_diff), avg_power + max_diff);

        // Clamp final [0 ; 100]
        FR_power = fminf(fmaxf(FR_power, 0.0f), 100.0f);
        FL_power = fminf(fmaxf(FL_power, 0.0f), 100.0f);
        BR_power = fminf(fmaxf(BR_power, 0.0f), 100.0f);
        BL_power = fminf(fmaxf(BL_power, 0.0f), 100.0f);

        // Mémorisation pour affichage/debug
        motor_power_values[0] = FR_power;
        motor_power_values[1] = FL_power;
        motor_power_values[2] = BR_power;
        motor_power_values[3] = BL_power;

        // Envoi des commandes aux moteurs
        DC_Motor_SetDuty(MOTEUR_AVANT_DROIT, FR_power);
        DC_Motor_SetDuty(MOTEUR_AVANT_GAUCHE, FL_power);
        DC_Motor_SetDuty(MOTEUR_ARRIERE_DROIT, BR_power);
        DC_Motor_SetDuty(MOTEUR_ARRIERE_GAUCHE, BL_power);
        return;
    }
}


SystemMotorOffsets motorOffsets = {0};

void manage_motor_offsets(){
	if(buttons.BACK_OFFSET_BUTTON){

	}
}


void decollage(void){
	for(int i = 0; i < 4; i++){
		//DC_Motor_StartRamp(i+1,0,MOTORS_TAKEOFF_POWER, MOTORS_STEP_PERCENT, MOTORS_INTERVAL_MS);
		DC_Motor_SetDuty(i, MOTORS_TAKEOFF_POWER);
	}
}

float normalize_with_deadzone(int raw, int center, int deadzone) {
    int delta = raw - center;

    if (abs(delta) <= deadzone)
        return 0.0f;

    return round_to_2_decimals((float)delta / 2048.0f);  // garde la même échelle [-1, 1]
}

float round_to_2_decimals(float value) {
    return roundf(value * 100.0f) / 100.0f;
}

