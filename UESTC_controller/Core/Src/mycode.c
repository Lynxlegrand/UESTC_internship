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

// Booléens
bool BLE_CONNECTED = false;
bool BLE_REPONDED = false;

// Dictionnaires

#define COMMAND_COUNT 2
CommandEntry command_table_rx[COMMAND_COUNT] = {
    {"\r\nBLE_DISC\r\n", handle_BLE_DISC},
    {"\r\nBLE_CONN\r\n", handle_BLE_CONN}
};



////////////////////////////////////////////////////////////////// CODE RX

void clear_buffer(uint8_t *buffer, int size){
    for (int i = 0; i < size; i++) {
        buffer[i] = 0;
    }
}

char* buffer_to_char(uint8_t* buffer, int start_index, int length){
    char* result = (char*) malloc((length + 1) * sizeof(char));	//alloue dynamiquement une chaîne de length + 1 caractères (+1 pour le \0 final).
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        result[i] = (char) buffer[start_index + i];
        buffer[start_index + i] = 0;
    }

    result[length] = '\0'; // nécessaire pour terminer une chaine de caractère
    return result;
}

void shorten_after_last_crlf(char* str) {
    if (str == NULL) return;

    char* last_cr = NULL;
    char* ptr = str;

    // Parcours la chaîne à la recherche de \r\n
    while ((ptr = strstr(ptr, "\r\n")) != NULL) {
        last_cr = ptr;        // Sauvegarde la position de la dernière occurrence
        ptr += 2;             // Continue après \r\n
    }

    // Si \r\n trouvé, coupe la chaîne juste après
    if (last_cr != NULL) {
        last_cr[2] = '\0';     // Coupe juste après le \n
    }
}


// fonction utilisée dans le main.c : HAL_UARTEx_RxEventCallback
void process_rx_buffer(char ch){
	static uint32_t strlen = 0;
	switch (ch){
	case '\0' :
		receive_buffer[0] = '\0';
		strlen = 0;
		break;
	default:
		if (strlen < RX_BUFFER_SIZE){
			strncat((char*)receive_buffer,&ch,1);
			++strlen;
		}


	}






}


void handle_BLE_DISC(void){
	BLE_CONNECTED = false;
}

void handle_BLE_CONN(void){
	BLE_CONNECTED = true;
}


////////////////////////////////////////////////////////////////// CODE TX






















