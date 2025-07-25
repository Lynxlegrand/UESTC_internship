/*
 * mycode.c
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */


#include "mycode.h"

////////////////////////////////////////////////////////////////// BUFFERS
char receive_buffer[RX_BUFFER_SIZE];
//uint8_t receive_buffer_int8[RX_BUFFER_SIZE];
char unknow_command[RX_BUFFER_SIZE];

////////////////////////////////////////////////////////////////// CODE RX

// flags
SystemFlags flags = {0};

// Dictionnaires
#define COMMAND_COUNT_RX 7
CommandEntry command_table_rx[COMMAND_COUNT_RX] = {
    {"BLE_DISC\r\n", handle_BLE_DISC},			//NB : les trammes réelles commencent et se terminent toutes par \r\n
	{"OK\r\n", handle_OK},
	{"ERR_CMD\r\n", handle_ERR_CMD},
	{"RESETTING!\r\n", handle_RSTING},
	{"CONNECTING......\r\n", handle_CONNECTING},
	{"RESTORING......\r\n", handle_RESTORING},
    {"BLE_CONN\r\n", handle_BLE_CONN}
};

// Handles

void handle_BLE_DISC(void){
	flags.BLE_CONNECTED = false;
	flags.BLE_HAS_BEEN_DISCONNECTED = true;

}

void handle_BLE_CONN(void){
	flags.BLE_CONNECTED = true;
	flags.BLE_HAS_BEEN_DISCONNECTED = false;
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
void handle_RESTORING(void){
	flags.RESTORING = true;
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
void config_BLE_manette(void){
	BLE.RestoreDefaults();
	wait_until_flag(&flags.RESTORING,BLE_TIMEOUT_MS);
	HAL_Delay(500);
	BLE.EnterATMode();
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetName(NAME);
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetRole(BLE_ROLE);
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.SetAutoConnect(BLE_MAC_SERVEUR);  // MAC du drone
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.ScanStart();
	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
	BLE.Reset();                 // Redémarre pour appliquer
	wait_until_flag(&flags.RSTING,BLE_TIMEOUT_MS);

}
//void config_BLE_drone(void){
//	BLE.RestoreDefaults();
//	wait_until_flag(&flags.RESTORING,BLE_TIMEOUT_MS);
//	HAL_Delay(500);
//	BLE.EnterATMode();
//	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
//	BLE.SetName(NAME);
//	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
//	BLE.SetRole(BLE_ROLE);
//	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
//	BLE.SetBLEMAC(BLE_MAC_SERVEUR);
//	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
//	BLE.SetSecurity(SECURITY);
//	wait_until_flag(&flags.OK,BLE_TIMEOUT_MS);
//	BLE.Reset();                 // Redémarre pour appliquer
//	wait_until_flag(&flags.RSTING,BLE_TIMEOUT_MS);
//}

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

#define TRAME_SIZE 32
uint16_t adcData[ADC_NUM_CONVERSIONS];
int gpioData[GPIO_NUM_CONVERSIONS];

//#define HEADER_BYTE 0xAA
//#define FOOTER_BYTE 0x7E

bool MUST_SEND_TRAME = false;
char trame[TRAME_SIZE];

void send_trame_if_necessary(void){
	if(MUST_SEND_TRAME){//flags.BLE_CONNECTED &&
		build_drone_trame_char(trame,TRAME_SIZE);
		BLE.SendCommand(trame);
		MUST_SEND_TRAME = false;

	}
}


void build_drone_trame_char(char* buffer, size_t buffer_size) {
    // Initialisation du buffer
    memset(buffer, 0, buffer_size);

    // Ajoute début de trame
    strncat(buffer, "\r\n", buffer_size - strlen(buffer) - 1);

    // Permet au drone de détecter que c'est une trame de l'IHM
    strncat(buffer, "$", buffer_size - strlen(buffer) - 1);

    // Ajoute les données ADC (en texte lisible)
    for (int i = 0; i < ADC_NUM_CONVERSIONS; i++) {
        char tmp[10];
        snprintf(tmp, sizeof(tmp), "%u/", adcData[i]);  // Convertit en string
        strncat(buffer, tmp, buffer_size - strlen(buffer) - 1);
    }

    // Ajoute les données GPIO
    for (int i = 0; i < GPIO_NUM_CONVERSIONS; i++) {
        char tmp[5];
        snprintf(tmp, sizeof(tmp), "%u", gpioData[i]);
        strncat(buffer, tmp, buffer_size - strlen(buffer) - 1);

        // Réinitialise si différent de 0
        if (gpioData[i] != 0) {
            gpioData[i] = 0;
        }
    }

    // Ajoute fin de trame
    strncat(buffer, "\r\n", buffer_size - strlen(buffer) - 1);
}

//void build_drone_trame(uint8_t* buffer) {			//code la trame (plus opti que du texte : taille trame constante et rapidité)
//    uint8_t idx = 0;
//
//    buffer[idx++] = HEADER_BYTE;  // Header (convention)
//
//    // ADC data (2 octets chacun)
//    for (int i = 0; i < ADC_NUM_CONVERSIONS; i++) {
//        buffer[idx++] = (adcData[i] >> 8) & 0xFF;  // MSB
//        buffer[idx++] = adcData[i] & 0xFF;         // LSB
//    }
//
//    // GPIO data (1 octet chacun)
//    for (int i = 0; i < GPIO_NUM_CONVERSIONS; i++) {
//        buffer[idx++] = gpioData[i];
//        if (gpioData[i]!=0){
//        	gpioData[i]=0;
//        }
//    }
//
//    buffer[idx++] = FOOTER_BYTE;  // Footer (convention)
//}


//void uint8_to_readable_char(const uint8_t trame[], char trame_char[]) {
//    for (int i = 0; i < TRAME_SIZE; i++) {
//        if (isprint(trame[i])) {
//            trame_char[i] = (char)trame[i];  // Caractère lisible
//        } else {
//            trame_char[i] = '.';             // Remplacement des caractères non imprimables
//        }
//    }
//}




////////////////////////////////////////////////////////////////// CODE TRAME DECODAGE


uint16_t adcData_2[ADC_NUM_CONVERSIONS];
int gpioData_2[GPIO_NUM_CONVERSIONS];

void parse_drone_trame_char(const char* buffer) {
    // Copie de la trame pour traitement (strtok modifie la chaîne)
    char temp[strlen(buffer) + 1];
    strcpy(temp, buffer);

    // Recherche du marqueur '$'
    char* start = strchr(temp, '$');
    if (start == NULL) return;  // Trame invalide

    start++; // Avance après le '$'

    // Séparer les ADC et GPIO : ADC sont suivis de '/' ; GPIO sans séparateur
    int adc_index = 0;
    while (adc_index < ADC_NUM_CONVERSIONS) {
        char* token = strtok(start, "/");
        if (token == NULL) return;
        adcData_2[adc_index++] = (uint16_t)atoi(token);
        start = NULL;  // Après le premier appel, strtok utilise NULL
    }

    // Ce qu'il reste : les GPIO, un par caractère numérique
    int gpio_index = 0;
    char* token = strtok(NULL, "\r\n");
    while (token != NULL && gpio_index < GPIO_NUM_CONVERSIONS) {
        gpioData_2[gpio_index++] = token[0] - '0';  // Convertit '1' -> 1
        token++;
    }
}


//uint16_t adcData_2[ADC_NUM_CONVERSIONS];
//int gpioData_2[GPIO_NUM_CONVERSIONS];
//
//void process_trame(uint8_t* trame) {
//    if (trame[0] != HEADER_BYTE || trame[TRAME_SIZE - 1] != FOOTER_BYTE)
//        return;
//
//    uint8_t idx = 1;  // On saute le header
//
//    for (int i = 0; i < ADC_NUM_CONVERSIONS; i++) {
//        uint16_t value = (trame[idx] << 8) | trame[idx + 1];
//        adcData_2[i] = value;
//        idx += 2;
//    }
//
//    for (int i = 0; i < GPIO_NUM_CONVERSIONS; i++) {
//        gpioData_2[i] = trame[idx++];
//    }
//}


