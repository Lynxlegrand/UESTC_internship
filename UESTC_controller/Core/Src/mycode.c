/*
 * mycode.c
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */


#include "mycode.h"

////////////////////////////////////////////////////////////////// CODE RX

// flags
SystemFlags flags = {0};

// Dictionnaires
#define COMMAND_COUNT_RX 4
CommandEntry command_table_rx[COMMAND_COUNT_RX] = {
    {"\r\nBLE_DISC\r\n", handle_BLE_DISC},
	{"\r\nOK\r\n", handle_OK},
	{"\r\nERR_CMD\r\n", handle_ERR_CMD},
    {"\r\nBLE_CONN\r\n", handle_BLE_CONN}
};

// Handles

void handle_BLE_DISC(void){
	flags.BLE_CONNECTED = false;
}

void handle_BLE_CONN(void){
	flags.BLE_CONNECTED = true;
}

void handle_OK(void){
	flags.BLE_CONNECTED = true;
}

void handle_ERR_CMD(void){
	flags.ERR_CMD = true;
}



// fonctions
void process_trame_rx(char* receive_buffer) {
	if(receive_buffer[0] == '\0'){
		return;
	}
	else {
		for (int i = 0; i < COMMAND_COUNT_RX; ++i) {
			if (strcmp(receive_buffer, command_table_rx[i].command) == 0) {
				command_table_rx[i].handler(); // Exécute le handler associé
				flags.UNKNOW_COMMAND = false;
				return; // Optionnel : on quitte après exécution
			}
		}
		flags.UNKNOW_COMMAND = true;
		return;
		}
}




////////////////////////////////////////////////////////////////// CODE TX

//Interface
BLE_Interface_t BLE = {
    .ping    = BLE_ping
};

//fonctions

void BLE_ping(void) {
	extern UART_HandleTypeDef huart_BLE;
    const char* at_cmd = "AT\r\n";
    HAL_UART_Transmit(&huart_BLE, (uint8_t*)at_cmd, strlen(at_cmd), HAL_MAX_DELAY);
}


















