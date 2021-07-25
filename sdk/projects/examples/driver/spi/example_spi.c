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

#define VIDEO            ((VIDEOPASSType*)VIDEO_BASE)
#define ACC              ((HALFSQUEEZENETType*)ACC_BASE)
#define SDMA             ((SDMAType*)SDMA_BASE)

#define ACC_BASE                (0x40100000UL)
#define SDMA_BASE               (0x40020000UL)
#define VIDEO_BASE               (0x40010000UL)
typedef struct{
    volatile uint32_t CONTROL_ADDR_AP_CTRL;//0x00
//	  volatile uint8_t RESERVED[3];//0x04
    volatile uint32_t CONTROL_ADDR_GIE;//0x04
    volatile uint32_t CONTROL_ADDR_IER;//0x08
    volatile uint32_t CONTROL_ADDR_ISR;//0x0c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_DIN_DATA;//0x10
    volatile uint32_t RESERVED0;//0x14
    volatile uint32_t CONTROL_ADDR_SQUEEZE_CIN_DATA;//0x18
    volatile uint32_t RESERVED1;//0x1c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_COUT_DATA;//0x20
    volatile uint32_t RESERVED2;//0x24
    volatile uint32_t CONTROL_ADDR_SQUEEZE_WEIGHT_ITERATIONS_DATA;//0x28
    volatile uint32_t RESERVED3;//0x2c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_FACTOR_ITERATIONS_DATA;//0x30
    volatile uint32_t RESERVED4;//0x34
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_DATA;//0x38
    volatile uint32_t RESERVED5;//0x3c
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_AFTERPOOL_DATA;//0x40
    volatile uint32_t RESERVED6;//0x44
    volatile uint32_t CONTROL_ADDR_EXPAND_CIN_DATA;//0x48
    volatile uint32_t RESERVED7;//0x4c
    volatile uint32_t CONTROL_ADDR_EXPAND_COUT_DATA;//0x50
    volatile uint32_t RESERVED8;//0x54
    volatile uint32_t CONTROL_ADDR_EXPAND_WEIGHT_ITERATIONS_DATA;//0x58
    volatile uint32_t RESERVED9;//0x5c
    volatile uint32_t CONTROL_ADDR_EXPAND_FACTOR_ITERATIONS_DATA;//0x60
    volatile uint32_t RESERVED10;//0x64
    volatile uint32_t CONTROL_ADDR_WHICHFIRE_DATA;//0x68
    volatile uint32_t RESERVED11;//0x6c
    volatile uint32_t CONTROL_ADDR_NUMREPS_DATA;//0x70
    volatile uint32_t RESERVED12;//0x74
}HALFSQUEEZENETType;
typedef struct{
    volatile  int32_t result[35];
    volatile uint32_t reserved;
    volatile uint32_t csr;
    volatile uint32_t state;
    volatile uint32_t number;
    volatile uint32_t start;
    volatile uint32_t coord[8];
    volatile uint32_t rect_en;
    volatile uint32_t frame_select;
}SDMAType;
typedef struct{
    volatile uint32_t SR;
    volatile uint32_t IR;
    volatile uint32_t OR;

}VIDEOPASSType;

// #define OPERATE_ADDR    0x0
// #define OPERATE_LEN     256
// #define SPIFLASH_BASE_VALUE 0x0
#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 10000000
// #define TEST_SPI_TIMEOUT 50
#define ElementType uint8_t
#define ElementBit 8
#define LENGTH 56
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

void Videopass_get(ElementType *pass_data){
	uint16_t i,j;
	uint32_t temp_data;
	for(j=0;j<(LENGTH*LENGTH);++j){
		VIDEO-> IR=0x01;
		temp_data=VIDEO-> OR;
//		for(i=0;i<4;i++){
//			pass_data[j*4+i]=(temp_data>>(4*i)) & 0XFF;
//		}
		pass_data[j]=temp_data & 0XFF;
		VIDEO-> IR=0x00;
//		printf("%u",VIDEO-> OR,"/n");
		// printf("%u",123,"/n");
//	}
	}
}


void print_data(ElementType *data, uint16_t n){
	uint16_t j;
	for(j=0;j<n;j++){
        if((j % LENGTH) == 0){
            printf("\r\n");
        }
       printf("%02x",data[j]);
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
    printf("\r\nstart FPGA spi test.\n");
    spi_handle_t handle = spi_t;
    // ElementType data_test = 200;
    static ElementType data_test[NBYTE] = {0};
//    ElementType nbyte = NBYTE;
//    uint32_t frame_num = 0;
//	ElementType request[NBYTE]={0};
//   	ElementType ack[NBYTE+1]={0};
    int32_t ret;
	uint32_t i,j;
    // ElementType begin = 255;
    // ElementType end = 0;

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
		
        // get data
        Videopass_get(data_test);

        // send
	    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
//            print_data(data_test, NBYTE);
            // mdelay(100);
            ret = csi_spi_send(spi_t, data_test, NBYTE);
            //			while (csi_spi_get_status(handle).busy);
		}
		// mdelay(100);
		// printf("test\r\n");
		csi_spi_ss_control(handle, SPI_SS_INACTIVE);
		mdelay(500);
        }
}


int main(void)
{
//	printf("test");
	wujian100_spi_init(MY_USI_IDX);
    wujian100_spi_test(0);
}
