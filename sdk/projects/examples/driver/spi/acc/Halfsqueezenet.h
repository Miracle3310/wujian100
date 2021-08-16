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
void Halfsqueezenet_Start();
void Halfsqueezenet_Reset();
void Halfsqueezenet_Set_squeeze_Din(uint32_t Data);
void Halfsqueezenet_Set_squeeze_Cin(uint32_t Data);
void Halfsqueezenet_Set_squeeze_Cout(uint32_t Data);
void Halfsqueezenet_Set_squeeze_weight_iterations(uint32_t Data);
void Halfsqueezenet_Set_squeeze_factor_iterations(uint32_t Data);
void Halfsqueezenet_Set_expand_Din(uint32_t Data);
void Halfsqueezenet_Set_expand_Din_afterpool(uint32_t Data);
void Halfsqueezenet_Set_expand_Cin(uint32_t Data);
void Halfsqueezenet_Set_expand_Cout(uint32_t Data);
void Halfsqueezenet_Set_expand_weight_iterations(uint32_t Data);
void Halfsqueezenet_Set_expand_factor_iterations(uint32_t Data);
void Halfsqueezenet_Set_whichFire(uint32_t Data);
void Halfsqueezenet_Set_numReps(uint32_t Data);
void ACC_transfer_weights();
void ACC_transfer_stream();
void ACC_set_whichIteration(uint32_t Data);
void ACC_transfer_start();
void ACC_get_result(int32_t *get_results);
void weightsfactors_transfer(uint32_t iteration);
void fire(uint32_t squeeze_din, uint32_t squeeze_cin, uint32_t squeeze_cout, uint32_t expand_din, uint32_t expand_din_afterpool, uint32_t expand_cin, uint32_t expand_cout, uint32_t whichfire, uint32_t iteration);
void Detect();
void draw_rectangle_enable();
void draw_rectangle_disable();
void draw_rectangle(int32_t *final_results, uint32_t frame_sel);
int intersection(int x1_min, int x1_max, int x2_min, int x2_max);
int print_class(int class_number, int last_class);