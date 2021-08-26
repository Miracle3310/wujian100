/******************************************************************************
  * @file     cookies.c
  * @brief    the main function for the ICContest2021 (Team: Cookies)
  * @version  V1.0
  * @date     22. Aug 2021
  ******************************************************************************/

#include "soc.h"
// #include "drv_gpio.h"
// #include "drv_spi.h"
// #include "drv_usart.h"
// #include "drv_dmac.h"
// #include <drv_irq.h>
// #include "stdio.h"
// #include <pin.h>
// #include <string.h>
// #include <csi_kernel.h>

#include "cookies.h"
#include "Video_get.h"
#include "Halfsqueezenet.h"
#include "audio_uart.h"
#include "img_spi.h"
#include "acc_irq.h"

void print_data(ElementType *data, uint16_t n)
{
    uint16_t j;
    for (j = 0; j < n; j++)
    {
        if ((j % LENGTH) == 0 && j != 0)
        {
            printf("\r\n");
        }
        printf("%02x", data[j]);
    }
    printf("\r\n");
}

int t_main(void)
{
    int frame_num = 0;
    while (1)
    {
        printf("%d\n", frame_num++);
        if (acc_done_flag){
            wujian100_get_acc_result();
        }
        if ((frame_num & 0b11111) == 0) // 16 samples
        {
            wujian100_uart_send(acc_result[4]);
        }
        wujian100_spi_send();
        // mdelay(10);
    }
    return 0;
}

int main(void)
{
    printf("\n******************\n");
    printf("wujian100 startup!\n");
    wujian100_spi_init(SPI_IDX);
    wujian100_uart_init(UART_IDX);
    wujian100_irq_init(IRQ_ACC);
    t_main();
    return 0;
}