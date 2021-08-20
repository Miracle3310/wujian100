/******************************************************************************
  * @file     example_spi.c
  * @brief    the main function for the SPI driver
  * @version  V1.0
  * @date     02. June 2017
  ******************************************************************************/

#include "drv_gpio.h"
#include "drv_spi.h"
#include "drv_usart.h"
#include "drv_dmac.h"
#include "soc.h"
#include "stdio.h"
#include <pin.h>
#include <string.h>
#include <csi_kernel.h>

// #include "jfif.h"
// #include "bmp.h"
#include "img_params.h"
#include "Video_get.h"
#include "Halfsqueezenet.h"

#define SPI_IDX 1 //select USI1
#define UART_IDX 2
#define MY_SPI_CLK_RATE 9000000

// #define SPITEST
// #define JPEGTEST


#define K_API_PARENT_PRIO    5
#define APP_START_TASK_STK_SIZE 2048	

k_task_handle_t t_main_task;
// extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
extern void mdelay(int32_t time);
static spi_handle_t spi_t;
static usart_handle_t uart_t;
ElementType spi_img_data[TOTALBYTE] = {0};
ElementType acc_result[5] = {0xFF,0xFF,0xFF,0xFF,0xFF};

static void print_data(ElementType *data, uint16_t n)
{
    uint16_t j;
    for (j = 0; j < n; j++)
    {
        if ((j % LENGTH) == 0 && j != 0)
        {
            printf("\r\n");
        }
        printf("%x", data[j]);
    }
    printf("\r\n");
}

static ElementType TransType(int32_t pos)
{
    if (pos < 0)
        return 0;
    else if (pos > 224)
        return 224;
    return (ElementType)pos;
}

#ifdef JPEGTEST
static void wujian100_jpeg_test(void *args)
{
    BMP bmp = {0};
    void *jfif = NULL;
    int count = 0;
    while (1)
    {
        bmp_create(&bmp, LENGTH, LENGTH);
        Spidata_get(bmp.pdata, 1);
        jfif = jfif_encode(&bmp);
        bmp_free(&bmp);
        // jfif_save(jfif, "encode.jpg");
        jfif_free(jfif);
        printf("%d\n", count++);
    }
}
#endif

// static void spi_event_cb_fun(int32_t idx, spi_event_e event)
// {
//     // printf("\nspi_event_cb_fun:%d\n",event);
// }

// void example_pin_spi_init(void)
// {
//     drv_pinmux_config(EXAMPLE_PIN_SPI_MISO, EXAMPLE_PIN_SPI_MISO_FUNC);
//     drv_pinmux_config(EXAMPLE_PIN_SPI_MOSI, EXAMPLE_PIN_SPI_MOSI_FUNC);
//     drv_pinmux_config(EXAMPLE_PIN_SPI_SCK, EXAMPLE_PIN_SPI_SCK_FUNC);
//     drv_pinmux_config(EXAMPLE_PIN_SPI_CS, EXAMPLE_PIN_SPI_CS_FUNC);
// }

static int wujian100_spi_init(int32_t idx) //idx->SPI_IDX
{
    int32_t ret;

    //spi_handle_t spi_t;

    // example_pin_spi_init(); //spi pin config

    // spi_t = csi_spi_initialize(idx, spi_event_cb_fun);
    spi_t = csi_spi_initialize(idx, NULL);

    if (spi_t == NULL)
    {
        printf(" csi_spi_initialize failed\n");
        return -1;
    }

    ret = csi_spi_config(spi_t, MY_SPI_CLK_RATE, SPI_MODE_MASTER,
                         SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
                         SPI_SS_MASTER_SW, sizeof(ElementType));

    ret = csi_spi_config_block_mode(spi_t, 1);

    if (ret != 0)
    {
        printf("%s(), %d spi config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

    return 0;
}

static int wujian100_uart_init(int32_t idx){
    int32_t ret;
    uart_t = csi_usart_initialize(idx, NULL);
    if (uart_t == NULL)
    {
        printf(" csi_usart_initialize failed\n");
        return -1;
    }
    ret = csi_usart_config(uart_t, 9600, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
    if (ret != 0)
    {
        printf("%s(), %d usart config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

    return 0;

}

static void wujian100_spi_send()
{
    /***send spi_img_data and acc_result***/
    spi_handle_t handle = spi_t;
    int32_t ret;
    uint32_t i, j;
    ElementType spi_single[NBYTE + CBYTE] = {0};

#ifdef SPITEST
        Spidata_get(spi_img_data);
#else
        Videopass_get(spi_img_data);
#endif

    // frame beginning
    spi_single[NBYTE + CBYTE - 1] = 0xFF;
    memcpy(spi_single, acc_result, 5);

    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
    ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
    csi_spi_ss_control(handle, SPI_SS_INACTIVE);

    // frame 
    spi_single[NBYTE + CBYTE - 1] = 0xFE;
    for (j = 0; j < (TOTALBYTE / NBYTE); j++)
    {
        memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);
        spi_single[NBYTE] = j % 0xFF;
        // print_data(spi_single, NBYTE + 1);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        for (i = 0; i < 0; i++)
        {
            csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        }
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
        // if (j % 100 == 0)
        //     mdelay(1);
        if (ret < 0)
        {
            printf("send fail\r\n");
            mdelay(10000);
        }
    }

    // frame end
    for (i = 0; i < NBYTE + CBYTE; i++)
    {
        spi_single[i] = 0xFD;
    }
    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
    ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
    csi_spi_ss_control(handle, SPI_SS_INACTIVE);
}

static void wujian100_get_acc_result()
{
    int32_t results[35];
    int last_class = -1;
    int32_t x_min_224, y_min_224, x_max_224, y_max_224 = 0;
    while (((SDMA->state) & 0x00000002) == 0)
        ;
    Detect();
    SDMA->state = ((SDMA->state) & 0xfffffffd);
    //        printf("detecting done\r\n");

    ACC_get_result(results);
    for (int i = 0; i < 5; ++i)
    {
        float float_result_0 = ((results[i * 7 + 0] / (float)(1 << 22)) * 0x00882065) / (float)(1 << 22);
        float float_result_1 = ((results[i * 7 + 1] / (float)(1 << 22)) * 0x00882065) / (float)(1 << 22);
        float float_result_2 = ((results[i * 7 + 2] / (float)(1 << 22)) * 0x00882065) / (float)(1 << 22);
        float float_result_3 = ((results[i * 7 + 3] / (float)(1 << 22)) * 0x00882065) / (float)(1 << 22);
        float float_result_4 = (float)results[i * 7 + 5];
        int32_t obj_h = (int32_t)(float_result_4 / 14);
        int32_t obj_w = (int32_t)(results[i * 7 + 5] % 14);
        //            x_min_224 = (int32_t)(float_result_0 + obj_w * 16);
        //            y_min_224 = (int32_t)(float_result_1 + obj_h * 16);
        //            x_max_224 = (int32_t)(float_result_2 + obj_w * 16);
        //            y_max_224 = (int32_t)(float_result_3 + obj_h * 16);

        int32_t x_min = (int32_t)(float_result_0 + obj_w * 16) * (640 / (float)224);
        int32_t y_min = (int32_t)(float_result_1 + obj_h * 16) * (360 / (float)224) + 60;
        int32_t x_max = (int32_t)(float_result_2 + obj_w * 16) * (640 / (float)224);
        int32_t y_max = (int32_t)(float_result_3 + obj_h * 16) * (360 / (float)224) + 60;
        //				printf("The coordinates of the upper left corner is :%d,%d\n",x_min,y_min);
        //				printf("The coordinates of the lower right corner is :%d,%d\n",x_max,y_max);
        results[i * 7 + 0] = x_min;
        results[i * 7 + 1] = y_min;
        results[i * 7 + 2] = x_max;
        results[i * 7 + 3] = y_max;
    }

    int select_result[14];
    int frame2_state = 0;

    for (int i = 0; i < 7; ++i)
        select_result[i] = results[i];
    last_class = print_class(select_result[6], last_class);
    for (int i = 1; i < 5; ++i)
    {
        if (frame2_state)
            break;
        if (intersection(results[0], results[2], results[i * 7 + 0], results[i * 7 + 2]) && intersection(results[1], results[3], results[i * 7 + 1], results[i * 7 + 3]))
            continue;
        else
        {
            if (results[i * 7 + 4] > 15728640 * (-1))
            {
                frame2_state = 1;
                for (int j = 0; j < 7; ++j)
                    select_result[j + 7] = results[i * 7 + j];
            }
            else
                continue;
        }
    }

    draw_rectangle(select_result, frame2_state);

    //get acc result
    x_min_224 = (int32_t)select_result[frame2_state * 7 + 0];
    y_min_224 = (int32_t)select_result[frame2_state * 7 + 1];
    x_max_224 = (int32_t)select_result[frame2_state * 7 + 2];
    y_max_224 = (int32_t)select_result[frame2_state * 7 + 3];
    // printf("select: %d %d %d %d\n\r", x_min_224, y_min_224, x_max_224, y_max_224);
    x_min_224 = (int32_t)x_min_224 * ((float)224 / 640);
    y_min_224 = (int32_t)(y_min_224 - 60) * ((float)224 / 360);
    x_max_224 = (int32_t)x_max_224 * ((float)224 / 640);
    y_max_224 = (int32_t)(y_max_224 - 60) * ((float)224 / 360);
    // printf("224:   %d %d %d %d\n\r", x_min_224, y_min_224, x_max_224, y_max_224);
    acc_result[0] = TransType(x_min_224);
    acc_result[1] = TransType(y_min_224);
    acc_result[2] = TransType(x_max_224);
    acc_result[3] = TransType(y_max_224);
    acc_result[4] = (ElementType)select_result[6];
}

static void wujian100_uart_send(){
    int32_t ret;
    // uint8_t send_data[] = {0x7E, 0x03, 0x10, 0x13, 0xEF};
    uint8_t send_data[] = {'h','e','l','l','o','\n'};
    uint8_t recv_data[20] = {0};
    // printf("start uart send\n");
    ret = csi_usart_send(uart_t, send_data, sizeof(send_data));
    ret = csi_usart_receive(uart_t, recv_data, sizeof(recv_data));
    // csi_usart_transfer(uart_t, send_data, recv_data, sizeof(send_data));
    if (ret < 0)
    {
        printf("send fail\n");
    }
    print_data(recv_data, 20);
    // for (int i = 0; i < sizeof(recv_data); i++){
    //     printf("%s", recv_data[i]);
    // }
    // printf("\n");
}

int t_main(void)
{
#ifdef JPEGTEST
    wujian100_jpeg_test(0);
#endif

    int frame_num = 0;
    while (1)
    {
        printf("%d\n", frame_num++);
        wujian100_get_acc_result();
        wujian100_spi_send();
        // wujian100_uart_send();
        // mdelay(1000);
    }
    return 0;
}

int main(void)
{
    printf("\n******************\n");
    printf("wujian100 startup!\n");
    wujian100_spi_init(SPI_IDX);
    wujian100_uart_init(UART_IDX);

    csi_kernel_init();

    csi_kernel_task_new((k_task_entry_t)t_main, "t_main",
                        0, K_API_PARENT_PRIO, 0, 0, APP_START_TASK_STK_SIZE, &t_main_task);

    csi_kernel_start();

    return 0;
}