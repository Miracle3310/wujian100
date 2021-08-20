#include <stdint.h>
#include "../img_params.h"

#define VIDEO_BASE (0x40010000UL)
#define VIDEO ((VIDEOPASSType *)VIDEO_BASE)
typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t IR;
    volatile uint32_t OR;
} VIDEOPASSType;
void Spidata_get(ElementType *pass_data);
void Videopass_get(ElementType *pass_data);