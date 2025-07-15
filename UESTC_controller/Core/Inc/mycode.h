/*
 * mycode.h
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */

#ifndef INC_MYCODE_H_
#define INC_MYCODE_H_

// Includes
#include "stm32f1xx_hal.h"
#include "ble_td5322a.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////// DEFINE ET PARAMETRAGE
#define huart_BLE huart1
#define BLE_ROLE 1 //client maitre
#define BLE_UUID "FF02"
#define BLE_MAC_SERVEUR "e73bbbee45c9"		//connection auto
#define NAME "Manette_HUGO"
#define BLE_TIMEOUT_MS 1000  // 1 seconde max

////////////////////////////////////////////////////////////////// CODE RX
// BUFFERS
#define RX_BUFFER_SIZE 64
extern char receive_buffer[RX_BUFFER_SIZE];
extern char unknow_command[RX_BUFFER_SIZE];


//Répertoire des flags
typedef struct {
	bool BLE_CONNECTED;
	bool BLE_REPONDED;
	bool UNKNOW_COMMAND;
	bool OK;
	bool ERR_CMD;
	bool RSTING;
	bool CONNECTING;
	bool RESTORING;
	bool BLE_HAS_BEEN_DISCONNECTED;
} SystemFlags;

extern SystemFlags flags;
//Structures
typedef void (*CommandHandler)(void);

typedef struct {
    const char* command;
    CommandHandler handler;
} CommandEntry;

// Prototypes
void process_trame_rx(char* receive_buffer);

// Prototypes des handles
void handle_BLE_CONN(void);
void handle_BLE_DISC(void);
void handle_OK(void);
void handle_ERR_CMD(void);
void handle_RSTING(void);
void handle_CONNECTING(void);
void handle_RESTORING(void);


////////////////////////////////////////////////////////////////// CODE TX

//Variables

extern bool flag_timeout_err;

// Prototypes
void config_BLE(void);
void wait_until_flag(volatile bool* flag, uint32_t timeout_ms);
void TIMEOUT_ERR_HANDLER(void);

////////////////////////////////////////////////////////////////// CODE TRAME
#define ADC_NUM_CONVERSIONS 4
#define GPIO_NUM_CONVERSIONS 5

extern uint16_t adcData[ADC_NUM_CONVERSIONS];
extern int gpioData[GPIO_NUM_CONVERSIONS];
extern bool MUST_SEND_TRAME;

void send_trame_if_necessary(void);
void build_drone_trame(uint8_t* buffer);
void uint8_to_readable_char(const uint8_t trame[], char trame_char[]);

////////////////////////////////////////////////////////////////// CODE TRAME DECODAGE
void process_trame(uint8_t* trame);


#endif /* INC_MYCODE_H_ */
