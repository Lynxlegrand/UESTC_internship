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

    static float FR_power = MOTORS_TAKEOFF_POWER;
    static float FL_power = MOTORS_TAKEOFF_POWER;
    static float BR_power = MOTORS_TAKEOFF_POWER;
    static float BL_power = MOTORS_TAKEOFF_POWER;

    if (buttons.EMERGENCY_STOP_BUTTON && flags.MOTORS_ON) {
        emergency_stop();
        flags.EMERGENCY_STOP = true;
        flags.MOTORS_ON = false;
        return;
    }

    if (!(flags.MOTORS_ON) && buttons.FRONT_OFFSET_BUTTON) {
        decollage();
        flags.MOTORS_ON = true;
        buttons.FRONT_OFFSET_BUTTON = false;
        buttons.BACK_OFFSET_BUTTON = false;
        buttons.LEFT_OFFSET_BUTTON = false;
        buttons.EMERGENCY_STOP_BUTTON = false;
        buttons.RIGHT_OFFSET_BUTTON = false;
    }

    if (flags.RAMPE_EN_COURS) {
        return;
    }

    if (flags.MOTORS_ON) {
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
        // Coefficients
        const float k_z           = HEIGHT_SENSIBILITY;
        const float k_translation = TRANSLATION_SENSITIVITY; // par exemple 200.0f
        const float k_yaw         = YAW_SENSITIVITY;         // par exemple 150.0f
        const float k_offset      = OFFSET_SENSIBILITY;

        const int center_z = CENTER_Z;
        const int center_yaw = CENTER_YAW;
        const int center_x = CENTER_X;
        const int center_y = CENTER_Y;

        const int deadzone_z = DEADZONE_Z;
        const int deadzone_yaw = DEADZONE_YAW;
        const int deadzone_x = DEADZONE_X;
        const int deadzone_y = DEADZONE_Y;
        // Lecture des joysticks avec zones mortes
        float z          = normalize_with_deadzone(adcData_2[0], center_z, deadzone_z);
        float yaw        = normalize_with_deadzone(adcData_2[1], center_yaw, deadzone_yaw);
        float trans_x    = normalize_with_deadzone(adcData_2[3], center_x, deadzone_x); // avant/arrière
        float trans_y    = normalize_with_deadzone(adcData_2[2], center_y, deadzone_y); // droite/gauche

        //Changments des sens
        z = -z;
        yaw = yaw;
        trans_x = -trans_x;
        trans_y = trans_y;


        // Base power
        float base_power = z * k_z;

        // Calcul des puissances avec translation et rotation
        FR_power += base_power
                 - trans_x * k_translation     // avant
                 + trans_y * k_translation     // gauche
                 - yaw * k_yaw
                 + pitch_offset * k_offset
                 - roll_offset  * k_offset;

        FL_power += base_power
                 - trans_x * k_translation
                 - trans_y * k_translation     // droite
                 + yaw * k_yaw
                 + pitch_offset * k_offset
                 + roll_offset  * k_offset;

        BR_power += base_power
                 + trans_x * k_translation     // arrière
                 + trans_y * k_translation
                 + yaw * k_yaw
                 - pitch_offset * k_offset
                 - roll_offset  * k_offset;

        BL_power += base_power
                 + trans_x * k_translation
                 - trans_y * k_translation
                 - yaw * k_yaw
                 - pitch_offset * k_offset
                 + roll_offset  * k_offset;

        // Moyenne des puissances
        float avg_power = (FR_power + FL_power + BR_power + BL_power) / 4.0f;

        // Saturation à [0 ; 100]
		FR_power = (FR_power > 100) ? 100 : (FR_power < 0) ? 0 : FR_power;
		FL_power = (FL_power > 100) ? 100 : (FL_power < 0) ? 0 : FL_power;
		BR_power = (BR_power > 100) ? 100 : (BR_power < 0) ? 0 : BR_power;
		BL_power = (BL_power > 100) ? 100 : (BL_power < 0) ? 0 : BL_power;

        // Borne chaque puissance autour de la moyenne
        FR_power = fminf(fmaxf(FR_power, avg_power - MAX_DIFF), avg_power + MAX_DIFF);
        FL_power = fminf(fmaxf(FL_power, avg_power - MAX_DIFF), avg_power + MAX_DIFF);
        BR_power = fminf(fmaxf(BR_power, avg_power - MAX_DIFF), avg_power + MAX_DIFF);
        BL_power = fminf(fmaxf(BL_power, avg_power - MAX_DIFF), avg_power + MAX_DIFF);


		motor_power_values[0] = FR_power;
		motor_power_values[1] = FL_power;
		motor_power_values[2] = BR_power;
		motor_power_values[3] = BL_power;


        // Envoi des commandes
        DC_Motor_SetDuty(MOTEUR_AVANT_DROIT, FR_power);
        DC_Motor_SetDuty(MOTEUR_AVANT_GAUCHE,  FL_power);
        DC_Motor_SetDuty(MOTEUR_ARRIERE_DROIT,  BR_power);
        DC_Motor_SetDuty(MOTEUR_ARRIERE_GAUCHE,   BL_power);
    }
}

SystemMotorOffsets motorOffsets = {0};

void manage_motor_offsets(){
	if(buttons.BACK_OFFSET_BUTTON){

	}
}


void decollage(void){
	for(int i = 0; i < 4; i++){
		DC_Motor_StartRamp(i+1,0,MOTORS_TAKEOFF_POWER, MOTORS_STEP_PERCENT, MOTORS_INTERVAL_MS);
	}
}

float normalize_with_deadzone(int raw, int center, int deadzone) {
    int delta = raw - center;

    if (abs(delta) <= deadzone)
        return 0.0f;

    return (float)delta / 2048.0f;  // garde la même échelle [-1, 1]
}
