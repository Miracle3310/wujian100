#ifndef __img__
#define __img__
#include <stdint.h>
#include <stdio.h>
#include "drv_spi.h"
#include "Video_get.h"
#include "../cookies.h"
spi_handle_t spi_t;
ElementType spi_img_data[TOTALBYTE];
ElementType XORCheck(ElementType *pass_data, uint32_t len);
void spi_event_cb_fun(int32_t idx, spi_event_e event);
int wujian100_spi_init(int32_t idx);
void wujian100_spi_send();
#endif