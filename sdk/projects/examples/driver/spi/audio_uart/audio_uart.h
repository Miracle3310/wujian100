#ifndef __audio__
#define __audio__
#include <stdint.h>
#include <stdio.h>
#include "drv_usart.h"
usart_handle_t uart_t;
static volatile uint8_t tx_async_flag;
static volatile uint8_t rx_async_flag;
uint8_t audio_command[10][10];
int32_t usart_send_async(usart_handle_t usart, const void *data, uint32_t num);
void usart_event_cb(int32_t idx, uint32_t event);
int wujian100_uart_init(int32_t idx);
void wujian100_uart_send(uint8_t cls);
#endif