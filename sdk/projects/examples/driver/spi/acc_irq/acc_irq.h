#ifndef __acc_irq__
#define __acc_irq__
#include <stdint.h>
#include <stdio.h>
#include "drv_irq.h"
#include "Halfsqueezenet.h"
#include "Video_get.h"
#include "../cookies.h"
int acc_done_flag;
void acc_irq_handle(void *args);
void wujian100_irq_init(uint32_t idx);
ElementType TransType(int32_t pos);
void wujian100_get_acc_result();
#endif