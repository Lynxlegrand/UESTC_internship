/*
 * mycode.c
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */


#include "mycode.h"

////////////////////////////////////////////////////////////////// BUFFERS
char receive_buffer[RX_BUFFER_SIZE];
char unknow_command[RX_BUFFER_SIZE];

////////////////////////////////////////////////////////////////// CODE RX

// flags
SystemFlags flags = {0};

// Dictionnaires
#define COMMAND_COUNT_RX 6
CommandEntry command_table_rx[COMMAND_COUNT_RX] = {
    {"\r\nBLE_DISC\r\n", handle_BLE_DISC},
	{"\r\nOK\r\n", handle_OK},
	{"\r\nERR_CMD\r\n", handle_ERR_CMD},
	{"\r\nRESETTING!\r\n", handle_RSTING},
	{"\r\nCONNECTING......\r\n", handle_CONNECTING},
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
	flags.OK = true;
}

void handle_ERR_CMD(void){
	flags.ERR_CMD = true;
}

void handle_RSTING(void){
	flags.RSTING = true;
}
void handle_CONNECTING(void){
	flags.CONNECTING = true;
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
		strcpy(receive_buffer, unknow_command); //utile pr le debug
		return;
		}
}


////////////////////////////////////////////////////////////////// CODE TX

bool flag_timeout_err = false;


// config du module BLE
void config_BLE(void){
	BLE.EnterATMode();           // AT>9
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetRole(BLE_ROLE);              // CLIENT
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetTargetUUID(BLE_UUID);   // UUID du service du drone
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetAutoConnect(BLE_MAC_DRONE);  // MAC du drone
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetName(NOM_DE_LA_MANETTE);
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.Reset();                 // Redémarre pour appliquer
	wait_until_flag(&flags.RSTING,BLE_TIMEOUT_MS);
}

void wait_until_flag(volatile bool* flag, uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();

    while (!(*flag)) {
        if (HAL_GetTick() - start >= timeout_ms) {
            flag_timeout_err = true;      // Indique qu'un timeout s’est produit
            TIMEOUT_ERR_HANDLER();        // Fonction de traitement personnalisée
            return;                       // Sort de la fonction
        }
    }

    *flag = false;                        // Réinitialise le flag une fois traité
    flag_timeout_err = false;             // Tout s’est bien passé
}





void TIMEOUT_ERR_HANDLER(void){
	//Ecrire ça plus tard
}












