/*
 * mycode.c
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */

// Includes
#include "mycode.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


// Booléens
bool BLE_CONNECTED = false;
bool BLE_REPONDED = false;
bool UNKNOW_COMMAND = false;

// Dictionnaires

#define COMMAND_COUNT 2
CommandEntry command_table_rx[COMMAND_COUNT] = {
    {"\r\nBLE_DISC\r\n", handle_BLE_DISC},
    {"\r\nBLE_CONN\r\n", handle_BLE_CONN}
};



////////////////////////////////////////////////////////////////// CODE RX



void process_trame(char* receive_buffer) {
	if(receive_buffer[0] == '\0'){
		return;
	}
	else {
		for (int i = 0; i < COMMAND_COUNT; ++i) {
			if (strcmp(receive_buffer, command_table_rx[i].command) == 0) {
				command_table_rx[i].handler(); // Exécute le handler associé
				UNKNOW_COMMAND = false;
				return; // Optionnel : on quitte après exécution
			}
		}
		UNKNOW_COMMAND = true;
		return;
		}
}


void handle_BLE_DISC(void){
	BLE_CONNECTED = false;
}

void handle_BLE_CONN(void){
	BLE_CONNECTED = true;
}


////////////////////////////////////////////////////////////////// CODE TX






















