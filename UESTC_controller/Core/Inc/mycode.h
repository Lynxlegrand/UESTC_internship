/*
 * mycode.h
 *
 *  Created on: Jun 20, 2025
 *      Author: hugoc
 */

#ifndef INC_MYCODE_H_
#define INC_MYCODE_H_
#include "main.h"

//Types
typedef void (*CommandHandler)(void);

typedef struct {
    const char* command;
    CommandHandler handler;
} CommandEntry;

// Prototypes
void clear_buffer(uint8_t *buffer, int size);
char* buffer_to_char(uint8_t* buffer, int start_index, int length);
void process_rx_buffer(char ch);
void shorten_after_last_crlf(char* str);

void handle_BLE_CONN(void);
void handle_BLE_DISC(void);


#endif /* INC_MYCODE_H_ */
