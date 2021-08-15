#include <stdint.h>

#define VIDEO ((VIDEOPASSType *)VIDEO_BASE)
#define ACC ((HALFSQUEEZENETType *)ACC_BASE)
#define SDMA ((SDMAType *)SDMA_BASE)

#define ACC_BASE (0x40100000UL)
#define SDMA_BASE (0x40020000UL)
#define VIDEO_BASE (0x40010000UL)

typedef struct
{
    volatile uint32_t CONTROL_ADDR_AP_CTRL;                        //0x00
                                                                   //	  volatile uint8_t RESERVED[3];//0x04
    volatile uint32_t CONTROL_ADDR_GIE;                            //0x04
    volatile uint32_t CONTROL_ADDR_IER;                            //0x08
    volatile uint32_t CONTROL_ADDR_ISR;                            //0x0c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_DIN_DATA;               //0x10
    volatile uint32_t RESERVED0;                                   //0x14
    volatile uint32_t CONTROL_ADDR_SQUEEZE_CIN_DATA;               //0x18
    volatile uint32_t RESERVED1;                                   //0x1c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_COUT_DATA;              //0x20
    volatile uint32_t RESERVED2;                                   //0x24
    volatile uint32_t CONTROL_ADDR_SQUEEZE_WEIGHT_ITERATIONS_DATA; //0x28
    volatile uint32_t RESERVED3;                                   //0x2c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_FACTOR_ITERATIONS_DATA; //0x30
    volatile uint32_t RESERVED4;                                   //0x34
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_DATA;                //0x38
    volatile uint32_t RESERVED5;                                   //0x3c
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_AFTERPOOL_DATA;      //0x40
    volatile uint32_t RESERVED6;                                   //0x44
    volatile uint32_t CONTROL_ADDR_EXPAND_CIN_DATA;                //0x48
    volatile uint32_t RESERVED7;                                   //0x4c
    volatile uint32_t CONTROL_ADDR_EXPAND_COUT_DATA;               //0x50
    volatile uint32_t RESERVED8;                                   //0x54
    volatile uint32_t CONTROL_ADDR_EXPAND_WEIGHT_ITERATIONS_DATA;  //0x58
    volatile uint32_t RESERVED9;                                   //0x5c
    volatile uint32_t CONTROL_ADDR_EXPAND_FACTOR_ITERATIONS_DATA;  //0x60
    volatile uint32_t RESERVED10;                                  //0x64
    volatile uint32_t CONTROL_ADDR_WHICHFIRE_DATA;                 //0x68
    volatile uint32_t RESERVED11;                                  //0x6c
    volatile uint32_t CONTROL_ADDR_NUMREPS_DATA;                   //0x70
    volatile uint32_t RESERVED12;                                  //0x74
} HALFSQUEEZENETType;
typedef struct
{
    volatile int32_t result[35];
    volatile uint32_t reserved;
    volatile uint32_t csr;
    volatile uint32_t state;
    volatile uint32_t number;
    volatile uint32_t start;
    volatile uint32_t coord[8];
    volatile uint32_t rect_en;
    volatile uint32_t frame_select;
} SDMAType;
typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t IR;
    volatile uint32_t OR;

} VIDEOPASSType;

