/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     main.c
 * @brief    hello world
 * @version  V1.0
 * @date     17. Jan 2018
 ******************************************************************************/

#include <stdio.h>

#include <stdio.h>
#include "stdio.h"
//#include "soc.h"
//#include "vtimer.h"
//#include "datatype.h"
#include <string.h>
#include <math.h>
#define WJ_CORDIC_BASE 0x40010000
#define REG32(p,i)           (*(volatile uint32_t *) ((p) + (i)))
#define CORDIC_REG32(offset)    REG32(WJ_CORDIC_BASE,offset)
#define SR 0x00000000
#define IR 0x00000004
#define OR 0x00000008

int main(void)
{   while(1){
	uint16_t rgb=*(volatile uint32_t *) 0x40010008 ;
	printf("%x\r\n",rgb);
	printf("hhh");
	mdelay(500);
}
    return 0;
}
