/******************************************************************************
  * @file     example_spi.c
  * @brief    the main function for the SPI driver
  * @version  V1.0
  * @date     02. June 2017
  ******************************************************************************/

#include "drv_gpio.h"
#include "drv_spi.h"
#include "soc.h"
#include "stdio.h"
#include <pin.h>
#include <string.h>
#include "Halfsqueezenet.h"

#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 9000000

#define ElementType uint8_t
#define LENGTH 112
#define NBYTE 4
#define CBYTE 2 // check byte
#define NCHANNEL 1
#define TOTALBYTE (LENGTH * LENGTH * NCHANNEL)

#define SPITEST
// #define SPITEST2

extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
extern void mdelay(int32_t time);
static spi_handle_t spi_t;
void print_data(ElementType *data, uint16_t n);
ElementType spi_img_data[TOTALBYTE + 1] = {0};
ElementType spi_single[NBYTE + CBYTE] = {0};

void Spidata_get(ElementType *pass_data, int frame_num)
{
    // uint16_t i;
    // ElementType line_color = 0xFF;
    // for (i = 0; i < LENGTH; i++)
    // {
    //     pass_data[i * LENGTH + frame_num] = line_color;
    //     if (frame_num == 0)
    //     {
    //         pass_data[i * LENGTH + LENGTH - 1] = frame_num;
    //     }
    //     else
    //     {
    //         pass_data[i * LENGTH + frame_num - 1] = frame_num;
    //     }
    // }

    uint16_t i, j;
    ElementType line_color = 0xAA;
    static ElementType back_color = 0x10;
    back_color += 0x40;
    for (i = 0; i < LENGTH; i++){
        for (j = 0; j < LENGTH; j++){
            pass_data[i * LENGTH + j] = back_color;
        }
        pass_data[i * LENGTH + frame_num] = line_color;
    }
}

void Videopass_get(ElementType *pass_data)
{
    //	uint16_t row=56;
    //	uint16_t col=224;
    uint16_t i, j, k;
    uint32_t temp_data;
    VIDEO->SR = 0x01;

    switch (LENGTH)
    {
    case (56):
        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 56; j++)
            {
                VIDEO->IR = i * 4 * 56 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                pass_data[i * LENGTH + j] = (temp_data & 0XFF);
            }
        }
        break;
    case (112):
        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 56; j++)
            {
                VIDEO->IR = i * 2 * 56 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                for (k = 0; k < 2; k++)
                {
                    pass_data[i * LENGTH + j * 2 + k] = (temp_data >> (16 * k)) & 0XFF;
                }
            }
        }
        break;
    case (224):
        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 56; j++)
            {
                VIDEO->IR = i * 56 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                for (k = 0; k < 4; k++)
                {
                    pass_data[i * LENGTH + j * 4 + k] = (temp_data >> (8 * k)) & 0XFF;
                }
            }
        }
        break;
    default:
        printf("Not supported length\r\n");
    }

    VIDEO->SR = 0x00;
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

static int wujian100_spi_init(int32_t idx) //idx->MY_USI_IDX
{
    int32_t ret;

    //spi_handle_t spi_t;

    example_pin_spi_init(); //spi pin config

    spi_t = csi_spi_initialize(idx, spi_event_cb_fun);

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

static void wujian100_spi_test(void *args)
{
    spi_handle_t handle = spi_t;
    int32_t ret;
    uint32_t i, j;
    int frame_num = 0;

    while (1)
    {
        mdelay(50);
        // get data
        Videopass_get(spi_img_data);
        // Spidata_get(spi_img_data, frame_num % LENGTH);
        frame_num++;
        printf("%d\n", frame_num);

        // print_data(spi_img_data, TOTALBYTE);

        // frame beginning
        for (i = 0; i < NBYTE + CBYTE; i++)
        {
            spi_single[i] = 0xFF;
        }

        // send
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);

        spi_single[NBYTE+CBYTE-1] = 0xFE;
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
            memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);
            spi_single[NBYTE] = j % 0xFF;
            // print_data(spi_single, NBYTE + 1);
            csi_spi_ss_control(handle, SPI_SS_ACTIVE);
            ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
            csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            for (i = 0; i < 0; i++)
            {
                csi_spi_ss_control(handle, SPI_SS_ACTIVE);
                csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            }
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
        // print_data(spi_single, NBYTE + 1);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    }
}

static void wujian100_spi_test_2(void *args)
{
    spi_handle_t handle = spi_t;
    int32_t ret;
    uint32_t i, j;
    int frame_num = 0;

    while (1)
    {
        mdelay(50);
        // get data
        // Videopass_get(spi_img_data);
        Spidata_get(spi_img_data, frame_num % LENGTH);
        frame_num++;
        printf("%d\n", frame_num);

        // print_data(spi_img_data, TOTALBYTE);

        // frame beginning
        for (i = 0; i < NBYTE + CBYTE; i++)
        {
            spi_single[i] = 0xFF;
        }

        // send
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);

        // spi_single[NBYTE+CBYTE-1] = 0xFE;
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
            memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);
            // spi_single[NBYTE] = j % 0xFF;
            // print_data(spi_single, NBYTE + 1);
            csi_spi_ss_control(handle, SPI_SS_ACTIVE);
            ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
            csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            for (i = 0; i < 20; i++)
            {
                csi_spi_ss_control(handle, SPI_SS_ACTIVE);
                csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            }
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
        // print_data(spi_single, NBYTE + 1);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    }
}

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

int main(void)
{
		printf("\n******************\n");
    printf("wujian100 startup!\n");
    wujian100_spi_init(MY_USI_IDX);

#ifdef SPITEST
    wujian100_spi_test(0);
#endif

#ifdef SPITEST2
    wujian100_spi_test_2(0);
#endif

    spi_handle_t handle = spi_t;
    int32_t ret;
    uint32_t i, j;
    int32_t results[35];
    int last_class = -1;
    int32_t x_min_224, y_min_224, x_max_224, y_max_224 = 0;

    while (1)
    {
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

        // draw_rectangle(select_result, frame2_state);

        //get whole img
        Videopass_get(spi_img_data);

        // frame beginning
        for (i = 0; i < NBYTE + CBYTE; i++)
        {
            spi_single[i] = 0xFF;
        }

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
        spi_single[0] = TransType(x_min_224);
        spi_single[1] = TransType(y_min_224);
        spi_single[2] = TransType(x_max_224);
        spi_single[3] = TransType(y_max_224);
        spi_single[4] = (ElementType)select_result[6];
        print_data(spi_single, 5);

        // send
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);

        spi_single[NBYTE+CBYTE-1] = 0xFE;
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
            memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);
            spi_single[NBYTE] = j % 0xFF;
            csi_spi_ss_control(handle, SPI_SS_ACTIVE);
            ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
            csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            for (i = 0; i < 1; i++)
            {
                csi_spi_ss_control(handle, SPI_SS_ACTIVE);
                csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            }
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
        // print_data(spi_single, NBYTE + 1);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    }
}
