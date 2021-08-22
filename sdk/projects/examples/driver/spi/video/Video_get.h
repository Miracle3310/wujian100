#ifndef __video__
#define __video__
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "drv_irq.h"
#include "../cookies.h"

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

//++ for fixed point
#define FIXQ          11
#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
//-- for fixed point
#endif