#ifndef __params__
#define __params__
#define ElementType uint8_t
#define LENGTH 112 
#define NBYTE 8
#define CBYTE 3 // check byte
#define NCHANNEL 2
#define TOTALBYTE (LENGTH * LENGTH * NCHANNEL)
#define SPI_IDX 1
#define UART_IDX 2
#define MY_SPI_CLK_RATE 8500000
#define IRQ_ACC 43
uint8_t acc_result[5]; // first 4 bytes: coordinate; last byte: class

// #define SPITEST

extern void mdelay(int32_t time);
void print_data(ElementType *data, uint16_t n);
#endif