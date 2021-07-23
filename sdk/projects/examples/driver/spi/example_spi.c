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

// #define OPERATE_ADDR    0x0
// #define OPERATE_LEN     256
// #define SPIFLASH_BASE_VALUE 0x0
#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 1000000
// #define TEST_SPI_TIMEOUT 50
#define ElementType uint8_t
#define ElementBit 8
#define LENGTH 224
#define NBYTE (LENGTH*LENGTH)
#define NCHANNEL 1
#define TOTALBYTE (LENGTH*LENGTH*NCHANNEL)

//  extern int32_t w25q64flash_read_id(spi_handle_t handle, uint32_t* id_num);
//  extern int32_t w25q64flash_erase_sector(spi_handle_t handle, uint32_t addr);
//  extern int32_t w25q64flash_erase_chip(spi_handle_t handle);
//  extern int32_t w25q64flash_program_data(spi_handle_t handle, uint32_t addr, const void* data, uint32_t cnt);
//  extern int32_t w25q64flash_read_data(spi_handle_t handle, uint32_t addr, void* data, uint32_t cnt);
extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);

static spi_handle_t spi_t;

void print_data(ElementType *data, uint16_t n){
	uint16_t j;
	for(j=0;j<n;j++){
        if((j % LENGTH) == 0){
            printf("\r\n");
        }
       printf("%x",data[j]);
	}
	printf("\r\n");
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
    // ElementType data_test = 200;
    static ElementType data_test[NBYTE] = {0};
//    ElementType nbyte = NBYTE;
    uint32_t frame_num = 0;
//	ElementType request[NBYTE]={0};
//   	ElementType ack[NBYTE+1]={0};
    int32_t ret;
	uint32_t i,j;
    ElementType begin = 255;
    ElementType end = 0;

    // initial
//	for(i=0;i<NBYTE;i++){
//		request[i]=1;
//	}

    
    while(1){
		// hand shake
		// csi_spi_ss_control(handle, SPI_SS_ACTIVE);
		// mdelay(100);
        // ret = csi_spi_send(spi_t, request, NBYTE+1);
		// while (csi_spi_get_status(handle).busy);
		// print_data(request,NBYTE+1);
		// csi_spi_ss_control(handle, SPI_SS_INACTIVE);
		
		
        // generate test frame
        for (i = 0; i < LENGTH; i++)
        {
            data_test[i * LENGTH+frame_num] = begin;

            if(frame_num==0){
                data_test[i * LENGTH + LENGTH - 1] = end;
            }
            else{
                data_test[i * LENGTH + frame_num - 1] = end;
            }
        }
        frame_num++;
        if (frame_num == LENGTH)
        {
            frame_num = 0;
        }

        // send
		printf("send spi\r\n");
	    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
//            print_data(data_test, NBYTE);
            // mdelay(100);
            ret = csi_spi_send(spi_t, data_test, NBYTE + 1);
            //			while (csi_spi_get_status(handle).busy);
		}
		 mdelay(100);
		// printf("test\r\n");
		csi_spi_ss_control(handle, SPI_SS_INACTIVE);
//		mdelay(100);
        }
}


int main(void)
{
//	printf("test");
	wujian100_spi_init(MY_USI_IDX);
    wujian100_spi_test(0);
}
