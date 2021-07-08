/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     example_spi.c
 * @brief    the main function for the SPI driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
 /*
#include "drv_spi.h"
#include "stdio.h"
#include "soc.h"
#include <string.h>
#include <pin.h>
#include "drv_gpio.h"
#include "w25q64fv.h"

#define OPERATE_ADDR    0x0
#define OPERATE_LEN     256
#define SPIFLASH_BASE_VALUE 0x0
static uint8_t erase_read_flag = 0;
static uint8_t program_read_flag = 0;

extern int32_t w25q64flash_read_id(spi_handle_t handle, uint32_t *id_num);
extern int32_t w25q64flash_erase_sector(spi_handle_t handle, uint32_t addr);
extern int32_t w25q64flash_erase_chip(spi_handle_t handle);
extern int32_t w25q64flash_program_data(spi_handle_t handle, uint32_t addr, const void *data, uint32_t cnt);
extern int32_t w25q64flash_read_data(spi_handle_t handle, uint32_t addr, void *data, uint32_t cnt);
extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
static void spi_event_cb_fun(int32_t idx, spi_event_e event)
{
    //printf("\nspi_event_cb_fun:%d\n",event);
}

void example_pin_spi_init(void)
{
    drv_pinmux_config(EXAMPLE_PIN_SPI_MISO, EXAMPLE_PIN_SPI_MISO_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_MOSI, EXAMPLE_PIN_SPI_MOSI_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_SCK, EXAMPLE_PIN_SPI_SCK_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_SPI_CS, EXAMPLE_PIN_SPI_CS_FUNC);
}

static uint8_t input[OPERATE_LEN] = {0};
static uint8_t output[OPERATE_LEN] = {0};
static int test_spi_eeprom(int32_t idx)
{
    uint8_t id[5] = {0x11, 0x11};

    int i;
    int32_t ret;
    spi_handle_t spi_handle_t;

    example_pin_spi_init();

    spi_handle_t = csi_spi_initialize(idx, spi_event_cb_fun);

    if (spi_handle_t == NULL) {
        printf(" csi_spi_initialize failed\n");
        return -1;
    }

    ret = csi_spi_config(spi_handle_t, W25Q64FV_CLK_RATE, SPI_MODE_MASTER,
                         SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
                         SPI_SS_MASTER_SW, 8);

    if (ret != 0) {
        printf("%s(), %d spi config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

    ret = w25q64flash_read_id(spi_handle_t, (uint32_t *)&id);

    if (ret < 0) {
        printf(" flash_read_id failed\n");
        return -1;
    }

    printf("the spiflash id is %x %x\r\n", id[3], id[4]);

    ret = w25q64flash_erase_sector(spi_handle_t, OPERATE_ADDR);

    if (ret < 0) {
        printf(" flash_erase_sector failed\n");
        return -1;
    }


    ret = w25q64flash_read_data(spi_handle_t, OPERATE_ADDR, output, OPERATE_LEN);


    if (ret < 0) {
        printf(" flash_read_data failed\n");
        return -1;
    }

    printf("erase sector and then read\n");

    for (i = 0; i < OPERATE_LEN; i++) {
        if ((i % 10) == 0) {
            printf("output[%d]", i);
        }

        printf("%x ", output[i]);

        if (((i + 1) % 10) == 0) {
            printf("\n");
        }

        if (output[i] != 0xff) {
            erase_read_flag = 1;
            break;
        }
    }

    printf("\n");

    if (erase_read_flag == 1) {
        printf("flash erase check and read check failed\n");
        return -1;
    }

    for (i = 0; i < OPERATE_LEN; i++) {
        input[i] = i + SPIFLASH_BASE_VALUE;
    }


    printf("flash erase sector passed\n");
    memset(output, 0x00, sizeof(output));

    ret = w25q64flash_program_data(spi_handle_t, OPERATE_ADDR, input, OPERATE_LEN);

    if (ret == -1) {
        printf("%s, %d ,flash program data error\n", __func__, __LINE__);
    }

    ret = w25q64flash_read_data(spi_handle_t, OPERATE_ADDR, output, OPERATE_LEN);

    if (ret == -1) {
        printf("%s, %d ,flash read error\n", __func__, __LINE__);
    }

    printf("program data and then read\n");

    for (i = 0; i < OPERATE_LEN; i++) {
        if ((i % 10) == 0) {
            printf("output[%d]", i);
        }

        printf("%d ", output[i]);

        if (((i + 1) % 10) == 0) {
            printf("\n");
        }

        if (output[i] != input[i]) {
            program_read_flag = 1;
            break;
        }
    }

    printf("\n");

    if (program_read_flag == 1) {
        printf("flash program and read check failed\n");
        return -1;
    }

    printf("flash program data passed\n");

    ret = csi_spi_uninitialize(spi_handle_t);
    return 0;

}

int example_spi(int32_t idx)
{
    int ret;
    ret = test_spi_eeprom(idx);

    if (ret < 0) {
        printf("test spi eeprom failed\n");
        return -1;
    }

    printf("test spi eeprom successfully\n");

    return 0;
}

int main(void)
{
    return example_spi(EXAMPLE_SPI_IDX);
}
*/
/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


 /******************************************************************************
  * @file     example_spi.c
  * @brief    the main function for the SPI driver
  * @version  V1.0
  * @date     02. June 2017
  ******************************************************************************/
#include "drv_spi.h"
#include "stdio.h"
#include "soc.h"
#include <string.h>
#include <pin.h>
#include "drv_gpio.h"
#include "w25q64fv.h"

#define OPERATE_ADDR    0x0
#define OPERATE_LEN     256
#define SPIFLASH_BASE_VALUE 0x0
#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 40000000
#define TEST_SPI_TIMEOUT 50
#define ElementType uint8
#define ElementBit 8

extern int32_t w25q64flash_read_id(spi_handle_t handle, uint32_t* id_num);
extern int32_t w25q64flash_erase_sector(spi_handle_t handle, uint32_t addr);
extern int32_t w25q64flash_erase_chip(spi_handle_t handle);
extern int32_t w25q64flash_program_data(spi_handle_t handle, uint32_t addr, const void* data, uint32_t cnt);
extern int32_t w25q64flash_read_data(spi_handle_t handle, uint32_t addr, void* data, uint32_t cnt);
extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);

static spi_handle_t spi_t;

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

static void test_sleep(uint32_t k)
{
    uint32_t i, j;

    for (i = 0; i < 100000000; i++) {
        for (j = 0; j < k; j++);
    }
}

static int wujian100_spi_init(int32_t idx)//idx->MY_USI_IDX
{
    int32_t ret;
    //spi_handle_t spi_t;

    example_pin_spi_init();//spi pin config

    spi_t = csi_spi_initialize(idx, spi_event_cb_fun);

    if (spi_t == NULL) {
        printf(" csi_spi_initialize failed\n");
        return -1;
    }

    ret = csi_spi_config(spi_t, MY_SPI_CLK_RATE, SPI_MODE_MASTER,
        SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
        SPI_SS_MASTER_SW, ElementBit);

    ret = csi_spi_config_block_mode(spi_t, 1);

    if (ret != 0) {
        printf("%s(), %d spi config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

	return 0;
}



static void wujian100_spi_test(void *args){
    printf("start FPGA spi test.\n");
    spi_handle_t handle = spi_t;
    ElementType data_test = 255;
//	uint8_t recv=0;
    int32_t ret;
    while(1){
		// test_sleep(1000000);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
		
		 
//         ret=csi_spi_receive(spi_t, &data_test, 1);
//		 printf("recv:%d\n", data_test);
        // if(ret<0){
        //    printf("recv failed.\n");
        // }
        
		printf("send:%d\n", data_test);
	    ret=csi_spi_send(spi_t, &data_test, 1);
//         while(csi_spi_get_status(handle).busy);
	    // if(ret<0){
		//    printf("send failed.\n");
	    // }
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
		data_test-=1;
        mdelay(1000);
        }
}



int main(void)
{
	// int p[100];//test data
    // for (int j = 0; j < 100; j++)
    // {
    //     p[j] = j;
    // }
	// uint32_t timecount = 0;
	// uint32_t i=0;
	printf("test");
	wujian100_spi_init(MY_USI_IDX);

    wujian100_spi_test(0);

    // while(1)
	// {
	// spi_handle_t handle = spi_t;
    // csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    // csi_spi_ss_control(handle, SPI_SS_ACTIVE);
	
    // csi_spi_send(handle, p+i, 1);//p->register storing data,1->one byte
    // while (csi_spi_get_status(handle).busy)
    // {
    //     timecount++;
	// 	printf("timeout: %d\n",timecount);
    //     if (timecount >= TEST_SPI_TIMEOUT)//TEST_SPI_TIMEOUT not defined
    //     {
    //         break;
    //     }
    // }
    // csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    // timecount = 0;
	// i=i+1;
	// if(i==100) i=0;
	// }
}
