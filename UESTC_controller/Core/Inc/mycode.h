/*
 * mycode.h
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */

#ifndef INC_MYCODE_H_
#define INC_MYCODE_H_
// Defines
#define huart_BLE huart1
// Includes
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


////////////////////////////////////////////////////////////////// CODE RX

//Répertoire des flags
typedef struct {
	bool BLE_CONNECTED;
	bool BLE_REPONDED;
	bool UNKNOW_COMMAND;
	bool OK;
	bool ERR_CMD;
} SystemFlags;

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



////////////////////////////////////////////////////////////////// CODE TX


//Structures
typedef struct {
    void (*ping)(void);
} BLE_Interface_t;


// Prototypes

// Prototypes des fonctions BLE
void BLE_ping(void);



#endif /* INC_MYCODE_H_ */
