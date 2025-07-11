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
	BLE.Reset();                 // Redémarre pour appliquer
	wait_until_flag(&flags.RSTING,BLE_TIMEOUT_MS);
	BLE.EnterATMode();           // AT>9
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetRole(BLE_ROLE);              // CLIENT
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetTargetUUID(BLE_UUID);   // UUID du service du drone
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.
	BLE.SetAutoConnect(BLE_MAC_DRONE);  // MAC du drone
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetName(NOM_DE_LA_MANETTE);
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.Reset();                 // Redémarre pour appliquer
	wait_until_flag(&flags.RSTING,BLE_TIMEOUT_MS);
	wait_until_flag(&flags.CONNECTING,BLE_TIMEOUT_MS);
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


////////////////////////////////////////////////////////////////// CODE TRAME

#define TRAME_SIZE (1 + 2 * ADC_NUM_CONVERSIONS + GPIO_NUM_CONVERSIONS + 1)
uint16_t adcData[ADC_NUM_CONVERSIONS];
int gpioData[GPIO_NUM_CONVERSIONS];

#define HEADER_BYTE 0xAA
#define FOOTER_BYTE 0x55

bool MUST_SEND_TRAME = false;
uint8_t trame[TRAME_SIZE];

void send_trame_if_necessary(void){
	if(flags.BLE_CONNECTED && MUST_SEND_TRAME){
		build_drone_trame(trame);
		process_trame(trame); //debug
		BLE.SendTrame(trame);
		MUST_SEND_TRAME = false;
	}
}

void build_drone_trame(uint8_t* buffer) {			//code la trame (plus opti que du texte : taille trame constante et rapidité)
    uint8_t idx = 0;

    buffer[idx++] = HEADER_BYTE;  // Header (convention)

    // ADC data (2 octets chacun)
    for (int i = 0; i < ADC_NUM_CONVERSIONS; i++) {
        buffer[idx++] = (adcData[i] >> 8) & 0xFF;  // MSB
        buffer[idx++] = adcData[i] & 0xFF;         // LSB
    }

    // GPIO data (1 octet chacun)
    for (int i = 0; i < GPIO_NUM_CONVERSIONS; i++) {
        buffer[idx++] = gpioData[i] ? 1 : 0;
    }

    buffer[idx++] = FOOTER_BYTE;  // Footer (convention)
}



void uint8_to_readable_char(const uint8_t trame[], char trame_char[]) {
    for (int i = 0; i < TRAME_SIZE; i++) {
        if (isprint(trame[i])) {
            trame_char[i] = (char)trame[i];  // Caractère lisible
        } else {
            trame_char[i] = '.';             // Remplacement des caractères non imprimables
        }
    }
}




////////////////////////////////////////////////////////////////// CODE TRAME DECODAGE


uint16_t adcData_2[ADC_NUM_CONVERSIONS];
int gpioData_2[GPIO_NUM_CONVERSIONS];

void process_trame(uint8_t* trame) {
    if (trame[0] != HEADER_BYTE || trame[TRAME_SIZE - 1] != FOOTER_BYTE)
        return;

    uint8_t idx = 1;  // On saute le header

    for (int i = 0; i < ADC_NUM_CONVERSIONS; i++) {
        uint16_t value = (trame[idx] << 8) | trame[idx + 1];
        adcData_2[i] = value;
        idx += 2;
    }

    for (int i = 0; i < GPIO_NUM_CONVERSIONS; i++) {
        gpioData_2[i] = trame[idx++];
    }
}

