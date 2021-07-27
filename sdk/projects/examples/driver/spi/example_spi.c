 /******************************************************************************
  * @file     example_spi.c
  * @brief    the main function for the SPI driver
  * @version  V1.0
  * @date     02. June 2017
  ******************************************************************************/
#include "drv_spi.h"
#include "stdio.h"
#include "soc.h"
#include <string.h>
#include <pin.h>
#include "drv_gpio.h"
//#include "w25q64fv.h"

#define VIDEO            ((VIDEOPASSType*)VIDEO_BASE)
#define ACC              ((HALFSQUEEZENETType*)ACC_BASE)
#define SDMA             ((SDMAType*)SDMA_BASE)

#define ACC_BASE                (0x40100000UL)
#define SDMA_BASE               (0x40020000UL)
#define VIDEO_BASE               (0x40010000UL)
typedef struct{
    volatile uint32_t CONTROL_ADDR_AP_CTRL;//0x00
//	  volatile uint8_t RESERVED[3];//0x04
    volatile uint32_t CONTROL_ADDR_GIE;//0x04
    volatile uint32_t CONTROL_ADDR_IER;//0x08
    volatile uint32_t CONTROL_ADDR_ISR;//0x0c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_DIN_DATA;//0x10
    volatile uint32_t RESERVED0;//0x14
    volatile uint32_t CONTROL_ADDR_SQUEEZE_CIN_DATA;//0x18
    volatile uint32_t RESERVED1;//0x1c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_COUT_DATA;//0x20
    volatile uint32_t RESERVED2;//0x24
    volatile uint32_t CONTROL_ADDR_SQUEEZE_WEIGHT_ITERATIONS_DATA;//0x28
    volatile uint32_t RESERVED3;//0x2c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_FACTOR_ITERATIONS_DATA;//0x30
    volatile uint32_t RESERVED4;//0x34
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_DATA;//0x38
    volatile uint32_t RESERVED5;//0x3c
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_AFTERPOOL_DATA;//0x40
    volatile uint32_t RESERVED6;//0x44
    volatile uint32_t CONTROL_ADDR_EXPAND_CIN_DATA;//0x48
    volatile uint32_t RESERVED7;//0x4c
    volatile uint32_t CONTROL_ADDR_EXPAND_COUT_DATA;//0x50
    volatile uint32_t RESERVED8;//0x54
    volatile uint32_t CONTROL_ADDR_EXPAND_WEIGHT_ITERATIONS_DATA;//0x58
    volatile uint32_t RESERVED9;//0x5c
    volatile uint32_t CONTROL_ADDR_EXPAND_FACTOR_ITERATIONS_DATA;//0x60
    volatile uint32_t RESERVED10;//0x64
    volatile uint32_t CONTROL_ADDR_WHICHFIRE_DATA;//0x68
    volatile uint32_t RESERVED11;//0x6c
    volatile uint32_t CONTROL_ADDR_NUMREPS_DATA;//0x70
    volatile uint32_t RESERVED12;//0x74
}HALFSQUEEZENETType;
typedef struct{
    volatile  int32_t result[35];
    volatile uint32_t reserved;
    volatile uint32_t csr;
    volatile uint32_t state;
    volatile uint32_t number;
    volatile uint32_t start;
    volatile uint32_t coord[8];
    volatile uint32_t rect_en;
    volatile uint32_t frame_select;
}SDMAType;
typedef struct{
    volatile uint32_t SR;
    volatile uint32_t IR;
    volatile uint32_t OR;

}VIDEOPASSType;

// #define OPERATE_ADDR    0x0
// #define OPERATE_LEN     256
// #define SPIFLASH_BASE_VALUE 0x0
#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 1000000
// #define TEST_SPI_TIMEOUT 50
#define ElementType uint8_t
#define ElementBit 8
#define LENGTH 112
#define NBYTE (112)
#define NCHANNEL 1
#define TOTALBYTE (LENGTH*LENGTH*NCHANNEL)

//  extern int32_t w25q64flash_read_id(spi_handle_t handle, uint32_t* id_num);
//  extern int32_t w25q64flash_erase_sector(spi_handle_t handle, uint32_t addr);
//  extern int32_t w25q64flash_erase_chip(spi_handle_t handle);
//  extern int32_t w25q64flash_program_data(spi_handle_t handle, uint32_t addr, const void* data, uint32_t cnt);
//  extern int32_t w25q64flash_read_data(spi_handle_t handle, uint32_t addr, void* data, uint32_t cnt);
extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
extern void mdelay(int32_t time);

static spi_handle_t spi_t;

//ElementType data_test[TOTALBYTE] = {0x80,0x7a,0x51,0x39,0x47,0x49,0x49,0x4a,0x54,0x5a,0x5e,0x61,0x63,0x64,0x64,0x66,0x62,0x65,0x65,0x62,0x63,0x62,0x62,0x62,0x62,0x62,0x63,0x65,0x63,0x61,0x60,0x5e,0x5e,0x5d,0x5a,0x51,0x4d,0x6d,0x80,0x7d,0x7d,0x7d,0x7e,0x7a,0x7b,0x77,0x74,0x93,0xd1,0xa7,0x41,0x4f,0x56,0x58,0x58,0x5e,0x7c,0x79,0x52,0x3a,0x45,0x47,0x45,0x47,0x52,0x58,0x5b,0x5e,0x5f,0x61,0x61,0x62,0x60,0x62,0x62,0x62,0x60,0x5d,0x5d,0x5d,0x5d,0x5f,0x61,0x62,0x61,0x5f,0x5e,0x5d,0x5d,0x5c,0x58,0x50,0x4d,0x6a,0x77,0x7a,0x7b,0x7a,0x7a,0x7a,0x7b,0x77,0x72,0x67,0xb0,0xd3,0x7e,0x40,0x52,0x56,0x5c,0x39,0x7b,0x78,0x51,0x36,0x42,0x48,0x44,0x44,0x52,0x56,0x5a,0x5b,0x5d,0x5e,0x5e,0x5f,0x5e,0x5f,0x5f,0x5e,0x5c,0x56,0x57,0x57,0x50,0x53,0x59,0x5b,0x5f,0x61,0x5f,0x5d,0x5b,0x5b,0x56,0x4d,0x4f,0x61,0x70,0x76,0x78,0x77,0x78,0x76,0x78,0x74,0x6f,0x6b,0x78,0xcd,0xcd,0x4d,0x45,0x58,0x37,0x1e,0x7c,0x77,0x53,0x33,0x41,0x46,0x47,0x47,0x50,0x56,0x59,0x5c,0x5d,0x60,0x60,0x61,0x5d,0x5f,0x5f,0x60,0x66,0x6b,0x7d,0x85,0x89,0x7c,0x67,0x4f,0x53,0x60,0x5d,0x5d,0x5c,0x58,0x53,0x4e,0x4c,0x62,0x6d,0x6d,0x73,0x75,0x75,0x72,0x72,0x6e,0x6a,0x6c,0x65,0x97,0xcf,0xa9,0x42,0x34,0x1d,0x21,0x7e,0x78,0x54,0x32,0x40,0x44,0x46,0x4a,0x53,0x58,0x5b,0x5c,0x5b,0x5c,0x5e,0x5f,0x66,0x61,0x60,0x69,0x6c,0x71,0x7f,0x89,0x89,0x8a,0x9a,0xa3,0x74,0x49,0x59,0x5a,0x5b,0x57,0x52,0x4c,0x49,0x62,0x6c,0x6a,0x6f,0x73,0x70,0x6e,0x6e,0x6c,0x67,0x69,0x6d,0x62,0xba,0xd8,0x5d,0x18,0x23,0x24,0x7f,0x78,0x52,0x2e,0x3c,0x41,0x41,0x44,0x4d,0x52,0x57,0x5a,0x5b,0x5a,0x67,0x5b,0x58,0x56,0x5c,0x5e,0x63,0x69,0x74,0x80,0x8d,0x96,0x96,0x99,0xa9,0x9a,0x52,0x47,0x52,0x55,0x52,0x4c,0x48,0x63,0x72,0x6e,0x6b,0x6d,0x6c,0x6c,0x6e,0x6c,0x6b,0x69,0x6c,0x67,0x84,0xaa,0x1e,0x20,0x25,0x24,0x7d,0x76,0x50,0x2c,0x3b,0x3f,0x40,0x42,0x4b,0x51,0x54,0x58,0x58,0x5d,0x5c,0x53,0x52,0x57,0x5b,0x5f,0x63,0x64,0x6f,0x7b,0x8b,0x92,0x9c,0xa2,0xa0,0x99,0xa7,0x7f,0x41,0x4d,0x4b,0x4a,0x46,0x65,0x78,0x73,0x64,0x55,0x68,0x6e,0x70,0x6e,0x6d,0x6d,0x6f,0x70,0x6c,0x19,0x1c,0x23,0x21,0x22,0x80,0x76,0x51,0x2c,0x3b,0x42,0x41,0x44,0x49,0x52,0x56,0x56,0x59,0x5b,0x51,0x51,0x56,0x55,0x55,0x5d,0x62,0x63,0x6d,0x77,0x90,0x99,0x92,0x94,0xab,0xb8,0xbc,0xc8,0xb5,0x29,0x46,0x43,0x46,0x64,0x77,0x77,0x65,0x3a,0x4d,0x73,0x71,0x6c,0x6a,0x6b,0x6c,0x73,0x28,0x1f,0x23,0x23,0x20,0x21,0x80,0x76,0x50,0x2d,0x3a,0x42,0x41,0x44,0x4a,0x52,0x54,0x56,0x5a,0x4b,0x4c,0x53,0x52,0x50,0x59,0x62,0x64,0x65,0x6c,0x75,0x8e,0x84,0x98,0xb9,0xba,0xae,0xb3,0xb4,0xcc,0x92,0x24,0x3e,0x46,0x64,0x76,0x75,0x69,0x35,0x2d,0x63,0x71,0x6c,0x69,0x6d,0x79,0x46,0x1b,0x20,0x22,0x23,0x23,0x22,0x80,0x75,0x4f,0x2e,0x3b,0x42,0x41,0x44,0x4a,0x52,0x55,0x57,0x52,0x4e,0x51,0x52,0x4f,0x5a,0x5f,0x67,0x6a,0x67,0x66,0x72,0x78,0xa7,0xb4,0xad,0xa7,0xac,0xb2,0xb3,0xb6,0xc4,0x9a,0x29,0x41,0x63,0x78,0x75,0x68,0x39,0x23,0x43,0x70,0x6c,0x66,0x6e,0x60,0x19,0x21,0x23,0x20,0x23,0x21,0x22,0x7c,0x75,0x51,0x2c,0x39,0x40,0x40,0x44,0x49,0x52,0x4f,0x68,0x44,0x4e,0x51,0x54,0x56,0x5c,0x65,0x65,0x68,0x59,0x59,0x87,0xa3,0xa3,0x9e,0xa0,0xaf,0xb5,0xb1,0xb6,0xb3,0xb3,0xcb,0x9e,0x28,0x63,0x76,0x75,0x68,0x37,0x20,0x25,0x42,0x78,0x66,0x65,0x20,0x21,0x26,0x24,0x22,0x23,0x21,0x33,0x7a,0x74,0x50,0x2a,0x36,0x3c,0x3e,0x40,0x49,0x4e,0x4e,0x7d,0x3f,0x4d,0x50,0x54,0x5c,0x64,0x61,0x61,0x50,0x60,0x9a,0x93,0x98,0x91,0x9f,0xa7,0xa5,0xac,0xab,0xae,0xaf,0xb5,0xb4,0xda,0x44,0x5a,0x75,0x77,0x69,0x39,0x1d,0x17,0x96,0xbc,0xc9,0x8b,0x14,0x21,0x22,0x22,0x26,0x25,0x1f,0x6a,0x7b,0x75,0x4f,0x29,0x38,0x3e,0x3e,0x3f,0x49,0x48,0x71,0x6e,0x3d,0x4e,0x52,0x58,0x5c,0x50,0x5d,0x4f,0x6b,0x92,0x82,0x8c,0x8d,0x9c,0x9e,0x9b,0x9b,0xa6,0xaa,0xaa,0xb3,0xb0,0xaf,0xb6,0xb1,0x43,0x77,0x79,0x6a,0x27,0x3b,0xa3,0xb6,0xbf,0xb1,0xc8,0xf,0x24,0x24,0x24,0x25,0x1e,0x5b,0x6e,0x80,0x78,0x53,0x2b,0x3a,0x42,0x42,0x44,0x4c,0x47,0x8c,0x5a,0x40,0x49,0x53,0x57,0x56,0x60,0x4f,0x70,0x7f,0x7e,0x88,0x94,0x8d,0x98,0x8e,0x91,0xa0,0xa4,0xa8,0xab,0x93,0xa5,0xb2,0xad,0xb2,0xb0,0x6b,0x71,0x61,0x84,0xaf,0xbb,0xb5,0xa5,0xb3,0xbb,0x15,0x27,0x24,0x27,0x23,0x30,0x78,0x73,0x82,0x7f,0x57,0x2e,0x3b,0x42,0x43,0x44,0x4b,0x42,0x9a,0x51,0x45,0x43,0x4b,0x5a,0x55,0x52,0x71,0x74,0x6d,0x7c,0x88,0x84,0x8e,0x83,0x8e,0x93,0x8c,0x98,0x97,0x97,0x93,0xa3,0x9f,0xa1,0x9c,0xa3,0x7f,0x8f,0xab,0xaa,0xb4,0xa7,0xaf,0xa8,0xba,0x9f,0x15,0x25,0x25,0x26,0x25,0x6c,0x70,0x78,0x84,0x80,0x58,0x2e,0x3b,0x42,0x43,0x42,0x48,0x38,0xb2,0x5f,0x4b,0x49,0x4f,0x56,0x55,0x6b,0x6d,0x63,0x71,0x74,0x83,0x87,0x7c,0x8a,0x8f,0x88,0x88,0x94,0x94,0x96,0xa2,0x98,0x8b,0x91,0x81,0x94,0x9d,0xaa,0xb4,0xa4,0xa3,0xaa,0xaa,0x95,0xb8,0x6b,0x1b,0x23,0x22,0x1f,0x4f,0x73,0x79,0x77,0x83,0x80,0x58,0x2d,0x3b,0x42,0x44,0x42,0x47,0x3c,0xac,0x70,0x5c,0x45,0x4d,0x4e,0x65,0x67,0x5e,0x70,0x6e,0x74,0x76,0x7e,0x89,0x85,0x83,0x82,0x7f,0x7f,0x8a,0x93,0x90,0x88,0x8a,0x81,0x96,0xa0,0xaf,0xa2,0xa1,0xa7,0xaa,0xb3,0x7d,0x72,0x9b,0x2a,0x1e,0x25,0x23,0x31,0x73,0x70,0x79,0x76,0x82,0x7e,0x56,0x2c,0x3a,0x41,0x42,0x42,0x46,0x44,0xa5,0x77,0x66,0x4a,0x47,0x6a,0x61,0x56,0x6f,0x67,0x6b,0x68,0x81,0x7c,0x83,0x71,0x7a,0x7b,0x85,0x84,0x86,0x81,0x7f,0x79,0x8a,0x94,0xa2,0x9e,0x9e,0xa4,0xaa,0xa8,0xb5,0x9c,0x72,0x58,0x99,0x11,0x23,0x23,0x1b,0x5d,0x70,0x76,0x76,0x74,0x84,0x81,0x59,0x2d,0x3a,0x41,0x42,0x45,0x47,0x48,0xb9,0x84,0x76,0x5b,0x5f,0x5f,0x56,0x6b,0x61,0x5a,0x66,0x75,0x56,0x52,0x49,0x70,0x62,0x67,0x66,0x6a,0x61,0x47,0x7f,0x8c,0xa1,0xa6,0x9b,0x9f,0xa4,0xa9,0xae,0xba,0xa8,0x52,0x2d,0x98,0x49,0x1d,0x22,0x22,0x33,0x74,0x74,0x76,0x76,0x75,0x86,0x83,0x5a,0x2e,0x3a,0x40,0x42,0x44,0x45,0x3b,0xbe,0x96,0x5c,0x5c,0x51,0x50,0x66,0x5a,0x5a,0x5f,0x68,0x5c,0x5b,0x47,0x3a,0x41,0x4a,0x34,0x2f,0x22,0x11,0x4c,0x88,0xa9,0xa3,0x99,0x9e,0xa0,0xaa,0xb5,0x91,0x49,0x28,0x3a,0x91,0x6a,0x15,0x25,0x23,0x1a,0x59,0x6e,0x76,0x76,0x76,0x74,0x88,0x84,0x5c,0x2e,0x3a,0x3f,0x44,0x42,0x43,0x36,0xa6,0xa2,0x60,0x6a,0x4a,0x67,0x57,0x5b,0x5d,0x68,0x5a,0x58,0x34,0x2b,0x1f,0x42,0x2f,0x3e,0x2a,0x22,0x62,0x91,0x98,0x97,0x94,0x95,0x9a,0xab,0x96,0x54,0x4c,0x59,0x6f,0x9b,0x4f,0x17,0x25,0x23,0x25,0x2d,0x72,0x72,0x7a,0x77,0x75,0x74,0x86,0x82,0x5f,0x2a,0x38,0x3f,0x42,0x46,0x45,0x40,0x75,0xb2,0x82,0x55,0x60,0x5b,0x5b,0x53,0x61,0x4e,0x42,0x2b,0x29,0x26,0x2d,0x2c,0x34,0x38,0x28,0x6c,0x9b,0x9c,0x90,0x8a,0x8a,0x94,0x92,0x60,0x29,0x44,0x5d,0x85,0x9c,0x4c,0x16,0x24,0x24,0x22,0x1e,0x4d,0x6c,0x78,0x79,0x79,0x77,0x75,0x85,0x83,0x62,0x2b,0x37,0x40,0x43,0x42,0x47,0x4f,0x38,0xaa,0x93,0x5a,0x56,0x57,0x57,0x5c,0x52,0x35,0x2d,0x28,0x30,0x25,0x29,0x2a,0x2e,0x35,0x52,0x92,0x9d,0x8d,0x8e,0x86,0x8e,0x97,0xa3,0x7e,0x34,0x3c,0x58,0x7c,0x26,0x18,0x23,0x25,0x25,0x24,0x24,0x55,0x75,0x7f,0x7c,0x7c,0x7a,0x77,0x88,0x83,0x62,0x2e,0x36,0x40,0x42,0x41,0x47,0x4f,0x46,0x88,0xae,0x56,0x4e,0x50,0x61,0x4f,0x34,0x24,0x2c,0x27,0x25,0x3b,0x21,0x1b,0x29,0x40,0x79,0x94,0x86,0x81,0x89,0x87,0xa4,0xab,0xad,0x99,0x3d,0x20,0x50,0x55,0x1b,0x23,0x26,0x25,0x22,0x20,0x47,0x5b,0x77,0x7c,0x7b,0x77,0x79,0x79,0x87,0x83,0x62,0x2d,0x36,0x40,0x42,0x42,0x4c,0x53,0x49,0x7b,0x8c,0x43,0x4d,0x5a,0x46,0x30,0x28,0x23,0x24,0x2e,0x2e,0x3c,0x22,0x27,0x6b,0x85,0x76,0x98,0x7d,0x82,0x86,0x9f,0xac,0xb0,0xb8,0xb7,0x53,0x1a,0x3f,0x59,0x1e,0x26,0x28,0x25,0x23,0x1c,0x60,0x6e,0x71,0x6e,0x6f,0x73,0x78,0x79,0x85,0x85,0x62,0x2e,0x37,0x43,0x43,0x44,0x4c,0x50,0x57,0x47,0x84,0x53,0x5b,0x51,0x2c,0x1f,0x27,0x24,0x22,0x23,0x33,0x33,0x31,0x67,0x8d,0x92,0x76,0x95,0x7c,0x81,0x93,0xa3,0xaf,0xb0,0xb9,0xbe,0x74,0x23,0x26,0x65,0x18,0x25,0x27,0x22,0x22,0x37,0x66,0x77,0x78,0x72,0x70,0x6f,0x66,0x69,0x85,0x86,0x65,0x2e,0x37,0x42,0x42,0x45,0x4c,0x54,0x56,0x5a,0x63,0x5b,0x5a,0x2a,0x27,0x23,0x26,0x2d,0x20,0x2a,0x29,0x1d,0x5a,0x85,0x7c,0x9d,0x7d,0x6d,0x87,0x96,0x97,0x9c,0xa8,0xa7,0xb3,0xc5,0x94,0x26,0x1b,0x69,0x1f,0x25,0x26,0x22,0x23,0x55,0x6a,0x7c,0x78,0x78,0x7a,0x78,0x6c,0x66,0x88,0x88,0x65,0x2f,0x36,0x42,0x43,0x44,0x49,0x4e,0x60,0x57,0x18,0x6f,0x2e,0x2f,0x31,0x28,0x25,0x21,0x1e,0x2e,0x23,0x4c,0x81,0x97,0x8d,0x8d,0x62,0x63,0x42,0x4e,0x8a,0x8e,0xa1,0xa5,0x99,0x6a,0x48,0x2d,0x1d,0x64,0x22,0x27,0x24,0x26,0x28,0x5f,0x74,0x78,0x74,0x71,0x73,0x72,0x73,0x71,0x86,0x86,0x68,0x2f,0x36,0x43,0x45,0x44,0x43,0x63,0x78,0x6d,0x4f,0x2f,0x36,0x2d,0x2f,0x2b,0x21,0x34,0x26,0x21,0x30,0x82,0x96,0x97,0x5c,0x32,0x36,0x27,0x2e,0x5c,0x66,0x80,0xa6,0xa4,0x41,0x23,0x2a,0x29,0x1b,0x60,0x29,0x24,0x25,0x1f,0x3a,0x67,0x78,0x75,0x73,0x72,0x72,0x71,0x71,0x6f,0x89,0x88,0x6b,0x30,0x3b,0x43,0x41,0x3d,0x6b,0x7d,0x6f,0x42,0x1e,0x2c,0x25,0x3f,0x23,0x1f,0x34,0x21,0x2c,0x1d,0x63,0x8a,0xac,0x5f,0x22,0x2a,0x21,0x7c,0x68,0x40,0x5c,0x77,0xbc,0x45,0x15,0x63,0x2c,0x1e,0x1e,0x5c,0x31,0x22,0x22,0x1c,0x5a,0x6f,0x78,0x72,0x71,0x73,0x71,0x72,0x6f,0x6b,0x8d,0x8d,0x6b,0x31,0x40,0x48,0x43,0x36,0x5a,0xa1,0x42,0x1f,0x32,0x2a,0x2c,0x4b,0x2c,0x1f,0x26,0x24,0x1b,0x3e,0x86,0xac,0x65,0x5b,0x6c,0x4f,0x5a,0x8a,0x8d,0x5e,0x59,0x73,0xca,0x59,0x5c,0x63,0x29,0x24,0x20,0x53,0x42,0x21,0x20,0x24,0x62,0x78,0x74,0x71,0x70,0x71,0x70,0x6f,0x6c,0x65,0x8d,0x92,0x6f,0x34,0x42,0x4c,0x53,0x71,0x37,0x23,0x1a,0x47,0x3b,0x27,0x3d,0x43,0x38,0x1c,0x20,0x20,0x27,0x6d,0xa6,0x63,0x53,0x74,0x7b,0x7d,0x74,0x75,0x81,0x6d,0x67,0x6c,0xc6,0x8d,0x67,0x53,0x47,0x29,0x22,0x4c,0x4d,0x20,0x23,0x37,0x67,0x77,0x75,0x72,0x70,0x6f,0x6f,0x6d,0x64,0x61,0x89,0x8e,0x70,0x38,0x3e,0x54,0x62,0x58,0x36,0x1e,0x3a,0x42,0x27,0x28,0x3a,0x45,0x3c,0x23,0x21,0x1a,0x4d,0x9d,0x76,0x38,0x62,0x76,0x87,0x90,0x91,0x8f,0x86,0x76,0x69,0x67,0xc1,0x98,0x75,0x63,0x57,0x2b,0x24,0x44,0x5b,0x1d,0x1f,0x52,0x6f,0x76,0x73,0x71,0x6f,0x6b,0x6b,0x67,0x5d,0x95,0x87,0x8a,0x6f,0x34,0x3c,0x4a,0x45,0x32,0x30,0x2f,0x3e,0x2d,0x1c,0x21,0x26,0x1f,0x3e,0x43,0x1a,0x33,0x75,0x9b,0xf,0x4d,0x64,0x71,0x7d,0x8d,0x94,0x8f,0x80,0x74,0x65,0x64,0xb4,0x9a,0x75,0x68,0x53,0x24,0x28,0x39,0x67,0x19,0x25,0x60,0x79,0x73,0x71,0x6f,0x6d,0x69,0x67,0x5e,0x84,0xa8,0x87,0x89,0x6f,0x2e,0x4d,0x60,0x45,0x40,0x2e,0x2f,0x42,0x35,0x20,0x20,0x2a,0x24,0x5f,0x57,0x18,0x4c,0xac,0x2f,0x24,0x4b,0x61,0x6c,0x79,0x81,0x8a,0x89,0x79,0x69,0x5f,0x5c,0x9e,0xb2,0x6d,0x66,0x46,0x22,0x2c,0x29,0x74,0x1b,0x2e,0x6b,0x79,0x73,0x71,0x6d,0x6b,0x6a,0x65,0x63,0xa5,0xa2,0x87,0x8b,0x70,0x2d,0x4b,0x4c,0x38,0x3d,0x2b,0x33,0x3e,0x40,0x1a,0x1f,0x33,0x54,0x3c,0x1d,0x38,0x68,0x5f,0x13,0x32,0x45,0x61,0x6a,0x74,0x7a,0x7c,0x82,0x75,0x5b,0x6f,0x63,0x8c,0xaf,0x68,0x63,0x38,0x1f,0x2b,0x1d,0x7c,0x1f,0x50,0x73,0x74,0x72,0x70,0x6d,0x6c,0x69,0x5f,0x8a,0xab,0xa6,0x86,0x8d,0x72,0x39,0x3b,0x4c,0x30,0x42,0x28,0x37,0x33,0x52,0x2a,0x2a,0x1e,0x33,0x20,0x19,0x87,0x88,0x15,0x21,0x39,0x47,0x60,0x68,0x72,0x76,0x79,0x7e,0x77,0x62,0x4d,0x4a,0x75,0x83,0x65,0x55,0x1a,0x25,0x2a,0x1e,0x72,0x2c,0x65,0x74,0x73,0x70,0x6f,0x6d,0x6c,0x68,0x5f,0x9e,0xa7,0xb3,0x83,0x8c,0x74,0x31,0x34,0x55,0x28,0x2d,0x34,0x2b,0x3c,0x4b,0x3e,0x3c,0x2d,0x28,0x57,0x63,0x6d,0x26,0x22,0x22,0x3c,0x4d,0x5f,0x68,0x70,0x72,0x76,0x7c,0x7e,0x76,0x7a,0x96,0xad,0x7b,0x69,0x3b,0x1f,0x25,0x2e,0x21,0x5c,0x3f,0x6f,0x78,0x71,0x6d,0x6c,0x6b,0x69,0x65,0x67,0xa2,0xb3,0xb8,0x84,0x8d,0x76,0x31,0x3f,0x58,0x30,0x39,0x22,0x27,0x2a,0x33,0x50,0x34,0x26,0x4d,0x8a,0x40,0x3b,0x23,0x24,0x27,0x36,0x48,0x64,0x66,0x6e,0x6f,0x71,0x79,0x76,0x78,0x85,0xaa,0xa8,0x71,0x5f,0x1d,0x23,0x2b,0x32,0x21,0x4e,0x51,0x72,0x73,0x71,0x6d,0x6d,0x6c,0x6b,0x63,0x6a,0xab,0xbc,0xb7,0x87,0x8c,0x75,0x36,0x47,0x56,0x20,0x2e,0x23,0x32,0x3c,0x2f,0x3c,0x39,0x69,0xbc,0x3b,0x58,0x18,0x2d,0x27,0x2b,0x2f,0x3e,0x5b,0x65,0x69,0x71,0x66,0x4a,0x53,0x53,0x4f,0x65,0x4d,0x5f,0x3d,0x1d,0x24,0x2d,0x32,0x24,0x40,0x6a,0x72,0x73,0x71,0x70,0x6c,0x6b,0x6a,0x62,0x72,0xba,0xbc,0xbb,0x8a,0x8e,0x7a,0x4d,0x3c,0x53,0x2b,0x1f,0x2a,0x49,0x33,0x3d,0x2d,0x57,0x76,0x64,0x71,0x16,0x1f,0x24,0x26,0x2c,0x28,0x32,0x50,0x5d,0x5f,0x6a,0x74,0x69,0x60,0x6b,0x85,0x83,0x70,0x61,0x1b,0x22,0x22,0x2d,0x31,0x28,0x35,0x7c,0x72,0x71,0x70,0x73,0x6d,0x6c,0x6b,0x5a,0x7b,0xc8,0xbd,0xbe,0x8d,0x90,0x77,0x2a,0x4c,0x38,0x3b,0x1d,0x28,0x5b,0x46,0x2e,0x54,0x46,0x70,0x87,0x1e,0x21,0x24,0x22,0x26,0x28,0x27,0x29,0x3f,0x4d,0x59,0x67,0x6d,0x6b,0x62,0x5f,0x64,0x68,0x70,0x29,0x20,0x24,0x23,0x2c,0x36,0x2a,0x31,0x7c,0x6f,0x73,0x70,0x71,0x6d,0x68,0x65,0x55,0x7a,0xd2,0xc3,0xc2,0x89,0x8c,0x79,0x2f,0x41,0x2c,0x2a,0x23,0x22,0x43,0x46,0x51,0x39,0x49,0x5b,0x38,0x1c,0x22,0x24,0x23,0x24,0x26,0x26,0x28,0x2d,0x3b,0x51,0x64,0x6b,0x73,0x7f,0x88,0x8a,0x7e,0x56,0x1e,0x22,0x29,0x24,0x32,0x31,0x2d,0x30,0x82,0x71,0x6f,0x6d,0x70,0x6b,0x64,0x60,0x50,0x88,0xd2,0xba,0xbf,0x84,0x8b,0x7a,0x2a,0x41,0x21,0x2f,0x22,0x1e,0x4b,0x25,0x41,0x53,0x69,0x3f,0x17,0x20,0x23,0x26,0x27,0x20,0x24,0x29,0x26,0x2b,0x25,0x29,0x3f,0x53,0x63,0x77,0x83,0x82,0x6a,0x34,0x17,0x21,0x27,0x23,0x36,0x31,0x2f,0x33,0x7c,0x4f,0x5d,0x65,0x6d,0x6b,0x66,0x61,0x49,0xa9,0xc4,0xb5,0xb8,0x86,0x8b,0x6f,0x69,0x41,0x20,0x2b,0x24,0x1e,0x43,0x4b,0x23,0x50,0x4c,0x41,0x59,0x1d,0x26,0x25,0x23,0x1f,0x24,0x25,0x27,0x24,0x3a,0x56,0x68,0x6b,0x69,0x70,0x72,0x76,0x92,0xa3,0x6b,0x26,0x19,0x1f,0x36,0x31,0x2f,0x3a,0x65,0x4a,0x4c,0x49,0x48,0x4e,0x55,0x53,0x45,0xb9,0xb1,0xb3,0xa9,0x83,0x88,0x88,0x29,0x46,0x19,0x29,0x23,0x37,0x23,0x4f,0x44,0x47,0x3d,0x43,0x5b,0x43,0x22,0x24,0x26,0x21,0x23,0x24,0x26,0x25,0x38,0x58,0x60,0x61,0x6b,0x6c,0x6c,0x7b,0x98,0xa8,0xae,0xbc,0x86,0x1a,0x2b,0x2c,0x2c,0x42,0x6d,0x64,0x5f,0x52,0x49,0x3f,0x32,0x4a,0x2e,0xad,0xb2,0xb2,0x7b,0x83,0x86,0x77,0x68,0x28,0x1f,0x35,0x32,0x36,0x30,0x2a,0x3f,0x2a,0x41,0x63,0x3a,0x47,0x3b,0x1f,0x26,0x1e,0x23,0x24,0x27,0x2f,0x32,0x5a,0x5b,0x69,0x6e,0x6e,0x75,0x80,0x90,0x98,0xa0,0xae,0xbe,0xbd,0x2a,0x23,0x29,0x48,0x6c,0x67,0x67,0x65,0x5c,0x55,0x4a,0x57,0x51,0xbe,0xb8,0x9b,0x2a,0x89,0x8a,0x79,0x64,0x1d,0x1f,0x27,0x24,0x29,0x1c,0x24,0x34,0x27,0x38,0x50,0x57,0x4c,0x4b,0x18,0x25,0x1f,0x26,0x23,0x2d,0x4a,0x2a,0x55,0x67,0x6b,0x6f,0x71,0x78,0x80,0x89,0x92,0x9d,0xad,0xb3,0xc0,0xc3,0x17,0x21,0x51,0x6a,0x69,0x68,0x67,0x63,0x5c,0x52,0x5b,0x7e,0xc3,0xba,0x6e,0x2c,0x88,0x8d,0x7b,0x52,0x20,0x21,0x21,0x1d,0x3a,0x24,0x28,0x2d,0x38,0x38,0x3e,0x6b,0x24,0x74,0x1b,0x28,0x24,0x25,0x22,0x36,0x4b,0x2a,0x5f,0x66,0x6b,0x6c,0x71,0x77,0x7d,0x86,0x90,0x99,0xa4,0xb6,0xbc,0xce,0x7c,0x13,0x5f,0x69,0x6a,0x66,0x63,0x5f,0x58,0x50,0x8c,0xac,0xc1,0xac,0x42,0x3a,0x88,0x8b,0x7c,0x4b,0x1c,0x22,0x21,0x25,0x40,0x23,0x1f,0x1e,0x29,0x40,0x32,0x6a,0x31,0x5c,0x23,0x2b,0x24,0x25,0x21,0x46,0x5b,0x2e,0x62,0x6a,0x6d,0x69,0x70,0x74,0x79,0x82,0x8d,0x95,0xa3,0xb4,0xbd,0xbe,0xcb,0xd,0x63,0x6f,0x69,0x64,0x60,0x61,0x53,0x70,0xcc,0xbb,0xbf,0x7f,0x37,0x40,0x8a,0x8a,0x7d,0x41,0x1d,0x21,0x23,0x2c,0x2c,0x20,0x23,0x24,0x2b,0x2b,0x51,0x55,0x53,0x53,0x35,0x2d,0x27,0x22,0x2d,0x58,0x55,0x34,0x6a,0x6a,0x6f,0x6d,0x6d,0x70,0x76,0x7e,0x84,0x8b,0xa2,0xaf,0xb8,0xbf,0xce,0x3c,0x67,0x6d,0x67,0x64,0x63,0x5c,0x54,0x86,0xba,0xbe,0xab,0x57,0x43,0x46,0x86,0x89,0x7e,0x43,0x1d,0x1d,0x20,0x20,0x26,0x21,0x1e,0x23,0x2b,0x2a,0x49,0x44,0x35,0x1b,0x20,0x35,0x2e,0x1f,0x41,0x60,0x4a,0x43,0x6b,0x6e,0x6e,0x6c,0x6c,0x6f,0x74,0x7c,0x81,0x88,0x99,0xac,0xb4,0xbb,0xc2,0x7e,0x64,0x69,0x6b,0x66,0x64,0x5f,0x5e,0x71,0x99,0xc7,0x8d,0x49,0x43,0x3b,0x85,0x89,0x7b,0x51,0x1f,0x25,0x27,0x20,0x23,0x20,0x22,0x23,0x26,0x30,0x39,0x38,0x42,0x49,0x23,0x21,0x20,0x29,0x4e,0x5e,0x35,0x5c,0x6c,0x6f,0x6e,0x6d,0x6c,0x6d,0x71,0x79,0x7f,0x86,0x8f,0xa1,0xae,0xbb,0xc5,0xbf,0x40,0x3a,0x4b,0x5b,0x63,0x64,0x70,0x5e,0x79,0xcc,0x70,0x3b,0x3a,0x32,0x81,0x8a,0x7f,0x45,0x1e,0x1f,0x22,0x24,0x24,0x24,0x1f,0x1c,0x2e,0x52,0x2a,0x27,0x30,0x23,0x20,0x1f,0x1f,0x40,0x53,0x54,0x2d,0x6e,0x6a,0x6f,0x70,0x6f,0x6c,0x6d,0x6f,0x75,0x7c,0x83,0x8a,0x99,0xaa,0xbb,0xbd,0xd1,0x42,0x2f,0x30,0x2a,0x2c,0x38,0x61,0x7f,0x9c,0xb5,0x40,0x2f,0x33,0x3b,0x7e,0x89,0x7d,0x39,0x1d,0x1c,0x1c,0x1e,0x1d,0x21,0x1e,0x1b,0x19,0x24,0x5d,0x3d,0x26,0x1a,0x18,0x19,0x29,0x4e,0x53,0x1c,0x58,0x6a,0x69,0x6c,0x6e,0x6e,0x69,0x6b,0x68,0x6e,0x72,0x79,0x84,0x90,0xa1,0xb3,0xbd,0xc1,0x83,0x33,0x39,0x2f,0x1e,0xb,0x5d,0x9b,0xab,0x5f,0x1d,0x26,0x32,0x32,0x9b,0xa4,0x9b,0x63,0x4a,0x4d,0x4b,0x4f,0x4e,0x50,0x50,0x52,0x52,0x4f,0x59,0x6c,0x4e,0x4b,0x4b,0x54,0x68,0x70,0x51,0x6d,0x86,0x8b,0x8a,0x8c,0x8d,0x90,0x8d,0x8c,0x8a,0x8b,0x91,0x94,0x9b,0xa3,0xaf,0xbf,0xc8,0xcd,0xc2,0x56,0x6a,0x63,0x5b,0x54,0x7c,0xc2,0x72,0x4a,0x52,0x5c,0x63,0x55};
ElementType data_test[TOTALBYTE+1] = {0};


void Videopass_get(ElementType *pass_data){
//	uint16_t row=56;
//	uint16_t col=224;
	uint16_t i,j,k;
	uint32_t temp_data;
	VIDEO-> SR=0x01;

	switch(LENGTH){
		case(56):
			for(i=0;i<LENGTH;i++){
				for(j=0;j<56;j++){
					VIDEO-> IR=i*4*56+j;
					while(VIDEO->SR!=0x03);
					temp_data=VIDEO-> OR;
					pass_data[i*LENGTH+j]=(temp_data & 0XFF);
				}
			}
			break;
		case(112):
			for(i=0;i<LENGTH;i++){
				for(j=0;j<56;j++){
					VIDEO-> IR=i*2*56+j;
					while(VIDEO->SR!=0x03);
					temp_data=VIDEO-> OR;
					for(k=0;k<2;k++){
						pass_data[i*LENGTH+j*2+k]=(temp_data>>(16*k)) & 0XFF;
					}
				}
			}
			break;
		case(224):
			for(i=0;i<LENGTH;i++){
				for(j=0;j<56;j++){
					VIDEO-> IR=i*56+j;
					while(VIDEO->SR!=0x03);
					temp_data=VIDEO-> OR;
					for(k=0;k<4;k++){
						pass_data[i*LENGTH+j*4+k]=(temp_data>>(8*k)) & 0XFF;
					}
				}
			}
			break;
		default:
			printf("Not supported length\r\n");
	}

	VIDEO-> SR=0x00;
}

void print_data(ElementType *data, uint16_t n){
	uint16_t j;
	for(j=0;j<n;j++){
        if((j % LENGTH) == 0){
            printf("\r\n");
        }
       printf("%02x",data[j]);
	}
	printf("\r\n");
}

static void spi_event_cb_fun(int32_t idx, spi_event_e event)
{
    // printf("\nspi_event_cb_fun:%d\n",event);
}

void example_pin_spi_init(void)
{
    drv_pinmux_config(EXAMPLE_PIN_SPI_MISO, EXAMPLE_PIN_SPI_MISO_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_MOSI, EXAMPLE_PIN_SPI_MOSI_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_SCK, EXAMPLE_PIN_SPI_SCK_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_CS, EXAMPLE_PIN_SPI_CS_FUNC);
}

static int wujian100_spi_init(int32_t idx)//idx->MY_USI_IDX
{
    int32_t ret;
    //spi_handle_t spi_t;

    example_pin_spi_init();//spi pin config

    spi_t = csi_spi_initialize(idx, spi_event_cb_fun);

    if (spi_t == NULL) {
        printf(" csi_spi_initialize failed\n");
        return -1;
    }

    ret = csi_spi_config(spi_t, MY_SPI_CLK_RATE, SPI_MODE_MASTER,
        SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
        SPI_SS_MASTER_SW, ElementBit);

    ret = csi_spi_config_block_mode(spi_t, 1);
//	ret = csi_spi_power_control(spi_t, DRV_POWER_FULL);
    if (ret != 0) {
        printf("%s(), %d spi config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

	return 0;
}



static void wujian100_spi_test(void *args){
//    printf("\r\nstart FPGA spi test.\n");
    spi_handle_t handle = spi_t;
    // ElementType data_test = 200;
//    static ElementType data_test[NBYTE] = {0};
//    ElementType nbyte = NBYTE;
//    uint32_t frame_num = 0;
//	ElementType request[NBYTE]={0};
//   	static ElementType ack[NBYTE+1]={0};
    int32_t ret;
	uint32_t j;
    // ElementType begin = 255;
    // ElementType end = 0;

    // initial
//	for(i=0;i<NBYTE;i++){
//		request[i]=1;
//	}

    
    while(1){
		// hand shake
		// csi_spi_ss_control(handle, SPI_SS_ACTIVE);
		// mdelay(100);
        // ret = csi_spi_send(spi_t, request, NBYTE+1);
		// while (csi_spi_get_status(handle).busy);
		// print_data(request,NBYTE+1);
		// csi_spi_ss_control(handle, SPI_SS_INACTIVE);
		
        // get data
//		printf("VIDEOPASS\r\n");
        Videopass_get(data_test);
//		printf("VIDEOPASS PASS\r\n");
//		print_data(data_test, NBYTE);
//		mdelay(50000);
//		printf("printf pass\r\n");

        // send
//		printf("send spi\r\n");

		csi_spi_ss_control(handle, SPI_SS_ACTIVE);
		mdelay(5);

        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
			printf("spi send %d\r\n",j);
			

            ret = csi_spi_send(spi_t, data_test+j*NBYTE, NBYTE);
			
			
			mdelay(15);
//			print_data(data_test, NBYTE);
			if(ret<0){
				printf("send fail\r\n");
				mdelay(10000);
			}
            //			while (csi_spi_get_status(handle).busy);

//			printf("\r\nsend\r\n");
//			print_data(data_test, NBYTE);
//			printf("\r\nrecv\r\n");
//			print_data(ack, NBYTE+1);
			
		}
		csi_spi_ss_control(handle, SPI_SS_INACTIVE);
//		 mdelay(1000);
		// printf("test\r\n");
		
//		mdelay(100);
        }
}


int main(void)
{
//	printf("test");
	wujian100_spi_init(MY_USI_IDX);
    wujian100_spi_test(0);
}
