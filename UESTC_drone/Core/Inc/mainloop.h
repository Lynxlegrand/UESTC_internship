/*
 * mainloop.h
 *
 *  Created on: Jul 25, 2025
 *      Author: hugoc
 */

#ifndef INC_MAINLOOP_H_
#define INC_MAINLOOP_H_

#include "mycode.h"
#include <math.h>


//Configurations du mode du drone

#define MODE_NORMAL 1
#define MODE_TEST_JOYSTICKS 2
#define MODE_TEST_20_PERCENT 3

// Choix du mode
#define MODE_CHOICE	2


// Takeoff parameters
#define MOTORS_TAKEOFF_POWER 20  //% de la puissance des moteurs
#define MOTORS_STEP_PERCENT 0.1
#define MOTORS_INTERVAL_MS 20			//6 secondes pr décoller


#define MOTEUR_AVANT_DROIT 1
#define MOTEUR_AVANT_GAUCHE 3
#define MOTEUR_ARRIERE_DROIT 4
#define MOTEUR_ARRIERE_GAUCHE 2
        // Coefficients
#define OFFSET_SENSITIVITY 1.0f //% de la puissance des moteurs
#define SIGNAL_LOST_TIMEOUT 3000 //ms
#define HEIGHT_SENSIBILITY 0.01f //facteur qui va régler la sensibilité des joysticks
#define TRANSLATION_SENSITIVITY 20.0f // par exemple 200.0f
#define YAW_SENSITIVITY 10.0f       // par exemple 150.0f

#define CENTER_Z 2053
#define CENTER_YAW 2080
#define CENTER_X 2013
#define CENTER_Y 2048

#define DEADZONE_Z 100
#define DEADZONE_YAW 100
#define DEADZONE_X 100
#define DEADZONE_Y 100

#define MAX_TEMPORARY_IMBALANCE  20.0f
#define MAX_DIFF                 30.0f

#define MAX_OFFSET_NUMBER 3		//nombre de fois qu'on peut ajuster l'offset dans un sens et une direction


typedef struct {
	float MOTEUR_AVANT_DROIT_OFFSET;
	float MOTEUR_AVANT_GAUCHE_OFFSET;
	float MOTEUR_ARRIERE_DROIT_OFFSET;
	float MOTEUR_ARRIERE_GAUCHE_OFFSET;
} SystemMotorOffsets;

extern SystemMotorOffsets motorOffsets;


void mainloop_drone_control(void);
void testloop_drone_control(void);

void decollage(void);
float normalize_with_deadzone(int raw, int center, int deadzone);
float round_to_2_decimals(float value);

#endif /* INC_MAINLOOP_H_ */
