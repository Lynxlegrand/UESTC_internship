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
void buffer_to_char(uint8_t* buffer, char* char_buff, int max_len);
void add_char_to_buff(char ch, char* buff);
void fill_char_buff(char*buff, uint8_t* dma_buff, uint16_t offset);
void shorten_after_last_crlf(char* str);
void process_trame(char* receive_buffer);

void handle_BLE_CONN(void);
void handle_BLE_DISC(void);


#endif /* INC_MYCODE_H_ */
