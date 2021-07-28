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

#define VIDEO ((VIDEOPASSType *)VIDEO_BASE)
#define ACC ((HALFSQUEEZENETType *)ACC_BASE)
#define SDMA ((SDMAType *)SDMA_BASE)

#define ACC_BASE (0x40100000UL)
#define SDMA_BASE (0x40020000UL)
#define VIDEO_BASE (0x40010000UL)
typedef struct
{
    volatile uint32_t CONTROL_ADDR_AP_CTRL;                        //0x00
                                                                   //	  volatile uint8_t RESERVED[3];//0x04
    volatile uint32_t CONTROL_ADDR_GIE;                            //0x04
    volatile uint32_t CONTROL_ADDR_IER;                            //0x08
    volatile uint32_t CONTROL_ADDR_ISR;                            //0x0c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_DIN_DATA;               //0x10
    volatile uint32_t RESERVED0;                                   //0x14
    volatile uint32_t CONTROL_ADDR_SQUEEZE_CIN_DATA;               //0x18
    volatile uint32_t RESERVED1;                                   //0x1c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_COUT_DATA;              //0x20
    volatile uint32_t RESERVED2;                                   //0x24
    volatile uint32_t CONTROL_ADDR_SQUEEZE_WEIGHT_ITERATIONS_DATA; //0x28
    volatile uint32_t RESERVED3;                                   //0x2c
    volatile uint32_t CONTROL_ADDR_SQUEEZE_FACTOR_ITERATIONS_DATA; //0x30
    volatile uint32_t RESERVED4;                                   //0x34
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_DATA;                //0x38
    volatile uint32_t RESERVED5;                                   //0x3c
    volatile uint32_t CONTROL_ADDR_EXPAND_DIN_AFTERPOOL_DATA;      //0x40
    volatile uint32_t RESERVED6;                                   //0x44
    volatile uint32_t CONTROL_ADDR_EXPAND_CIN_DATA;                //0x48
    volatile uint32_t RESERVED7;                                   //0x4c
    volatile uint32_t CONTROL_ADDR_EXPAND_COUT_DATA;               //0x50
    volatile uint32_t RESERVED8;                                   //0x54
    volatile uint32_t CONTROL_ADDR_EXPAND_WEIGHT_ITERATIONS_DATA;  //0x58
    volatile uint32_t RESERVED9;                                   //0x5c
    volatile uint32_t CONTROL_ADDR_EXPAND_FACTOR_ITERATIONS_DATA;  //0x60
    volatile uint32_t RESERVED10;                                  //0x64
    volatile uint32_t CONTROL_ADDR_WHICHFIRE_DATA;                 //0x68
    volatile uint32_t RESERVED11;                                  //0x6c
    volatile uint32_t CONTROL_ADDR_NUMREPS_DATA;                   //0x70
    volatile uint32_t RESERVED12;                                  //0x74
} HALFSQUEEZENETType;
typedef struct
{
    volatile int32_t result[35];
    volatile uint32_t reserved;
    volatile uint32_t csr;
    volatile uint32_t state;
    volatile uint32_t number;
    volatile uint32_t start;
    volatile uint32_t coord[8];
    volatile uint32_t rect_en;
    volatile uint32_t frame_select;
} SDMAType;
typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t IR;
    volatile uint32_t OR;

} VIDEOPASSType;

#define MY_USI_IDX 1 //select USI1
#define MY_SPI_CLK_RATE 1000000

#define ElementType uint8_t
#define LENGTH 112
#define NBYTE (112)
#define NCHANNEL 1
#define TOTALBYTE (LENGTH * LENGTH * NCHANNEL)

extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
extern void mdelay(int32_t time);
static spi_handle_t spi_t;
ElementType spi_img_data[TOTALBYTE + 1] = {0};
ElementType spi_single[NBYTE + 1] = {0};

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

//SDMA AND ACC CONFIG
void Halfsqueezenet_Start()
{
    //    uint32_t Data;

    //    Data = (ACC-> CONTROL_ADDR_AP_CTRL) & 0x80;
    //    ACC-> CONTROL_ADDR_AP_CTRL = Data | 0x01;
    ACC->CONTROL_ADDR_AP_CTRL = 0x01;
}

void Halfsqueezenet_Reset()
{
    //    uint32_t Data;
    //		printf("start");
    //    Data = (ACC-> CONTROL_ADDR_AP_CTRL) & 0x80;
    //    ACC-> CONTROL_ADDR_AP_CTRL = Data;
    ACC->CONTROL_ADDR_AP_CTRL = 0x00;
}

void Halfsqueezenet_Set_squeeze_Din(uint32_t Data)
{
    ACC->CONTROL_ADDR_SQUEEZE_DIN_DATA = Data;
}

void Halfsqueezenet_Set_squeeze_Cin(uint32_t Data)
{
    ACC->CONTROL_ADDR_SQUEEZE_CIN_DATA = Data;
}

void Halfsqueezenet_Set_squeeze_Cout(uint32_t Data)
{
    ACC->CONTROL_ADDR_SQUEEZE_COUT_DATA = Data;
}

void Halfsqueezenet_Set_squeeze_weight_iterations(uint32_t Data)
{
    ACC->CONTROL_ADDR_SQUEEZE_WEIGHT_ITERATIONS_DATA = Data;
}

void Halfsqueezenet_Set_squeeze_factor_iterations(uint32_t Data)
{
    ACC->CONTROL_ADDR_SQUEEZE_FACTOR_ITERATIONS_DATA = Data;
}

void Halfsqueezenet_Set_expand_Din(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_DIN_DATA = Data;
}

void Halfsqueezenet_Set_expand_Din_afterpool(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_DIN_AFTERPOOL_DATA = Data;
}

void Halfsqueezenet_Set_expand_Cin(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_CIN_DATA = Data;
}

void Halfsqueezenet_Set_expand_Cout(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_COUT_DATA = Data;
}

void Halfsqueezenet_Set_expand_weight_iterations(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_WEIGHT_ITERATIONS_DATA = Data;
}

void Halfsqueezenet_Set_expand_factor_iterations(uint32_t Data)
{
    ACC->CONTROL_ADDR_EXPAND_FACTOR_ITERATIONS_DATA = Data;
}

void Halfsqueezenet_Set_whichFire(uint32_t Data)
{
    ACC->CONTROL_ADDR_WHICHFIRE_DATA = Data;
}

void Halfsqueezenet_Set_numReps(uint32_t Data)
{
    ACC->CONTROL_ADDR_NUMREPS_DATA = Data;
}

void ACC_transfer_weights()
{
    SDMA->csr = 1;
}

void ACC_transfer_stream()
{
    SDMA->csr = 2;
}

void ACC_set_whichIteration(uint32_t Data)
{
    SDMA->number = Data;
}

void ACC_transfer_start()
{
    SDMA->start = 1;
}

void ACC_get_result(int32_t *get_results)
{
    for (int i = 0; i < 35; i++)
    {
        get_results[i] = SDMA->result[i];
        //	printf("%u\t",get_results[i]);
    }
}

void weightsfactors_transfer(uint32_t iteration)
{
    //	printf("start");
    Halfsqueezenet_Reset();
    //	  printf("start");
    Halfsqueezenet_Set_whichFire(13);
    Halfsqueezenet_Set_numReps(0);
    Halfsqueezenet_Start();

    ACC_transfer_weights();
    ACC_set_whichIteration(iteration);
    //	printf("%u\t",iteration);
    ACC_transfer_start();
    while (((SDMA->state) & 0x00000001) == 0)
        ;
}

void fire(uint32_t squeeze_din, uint32_t squeeze_cin, uint32_t squeeze_cout, uint32_t expand_din, uint32_t expand_din_afterpool, uint32_t expand_cin, uint32_t expand_cout, uint32_t whichfire, uint32_t iteration)
{
    Halfsqueezenet_Reset();
    Halfsqueezenet_Set_squeeze_Din(squeeze_din);
    Halfsqueezenet_Set_squeeze_Cin(squeeze_cin);
    Halfsqueezenet_Set_squeeze_Cout(squeeze_cout);
    Halfsqueezenet_Set_squeeze_weight_iterations(0);
    Halfsqueezenet_Set_squeeze_factor_iterations(0);
    Halfsqueezenet_Set_expand_Din(expand_din);
    Halfsqueezenet_Set_expand_Din_afterpool(expand_din_afterpool);
    Halfsqueezenet_Set_expand_Cin(expand_cin);
    Halfsqueezenet_Set_expand_Cout(expand_cout);
    Halfsqueezenet_Set_expand_weight_iterations(0);
    Halfsqueezenet_Set_expand_factor_iterations(0);
    Halfsqueezenet_Set_whichFire(whichfire);
    Halfsqueezenet_Set_numReps(1);
    Halfsqueezenet_Start();

    ACC_transfer_stream();
    ACC_set_whichIteration(iteration);
    ACC_transfer_start();
    while (((SDMA->state) & 0x00000001) == 0)
        ;
}

void Detect()
{
    weightsfactors_transfer(0);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(56, 32, 32, 56, 28, 32, 96, 1, 0);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(1);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(28, 96, 32, 28, 14, 32, 96, 2, 1);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(2);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(14, 96, 32, 14, 14, 32, 96, 3, 2);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(3);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(14, 96, 32, 14, 14, 32, 96, 4, 3);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(4);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(14, 96, 32, 14, 14, 32, 96, 5, 4);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(5);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(14, 96, 32, 14, 14, 32, 96, 6, 5);
    SDMA->state = ((SDMA->state) & 0xfffffffe);

    weightsfactors_transfer(6);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
    fire(14, 96, 32, 14, 14, 32, 96, 7, 6);
    SDMA->state = ((SDMA->state) & 0xfffffffe);
}

void draw_rectangle_enable()
{
    SDMA->rect_en = 1;
}

void draw_rectangle_disable()
{
    SDMA->rect_en = 0;
}

void draw_rectangle(int32_t *final_results, uint32_t frame_sel)
{
    for (int i = 0; i < 4; i++)
    {
        SDMA->coord[i] = final_results[i];
        SDMA->coord[i + 4] = final_results[i + 7];
    }

    draw_rectangle_enable();

    SDMA->frame_select = frame_sel;
}

int intersection(int x1_min, int x1_max, int x2_min, int x2_max)
{
    if ((x1_min >= x2_min && x1_min < x2_max) || (x2_min >= x1_min && x2_min < x1_max))
        return 1;
    else
        return 0;
}

int print_class(int class_number, int last_class)
{
    if (class_number != last_class)
    {
        switch (class_number)
        {
        case 0:
            printf("Class : boat\n");
            break;
        case 1:
            printf("Class : building\n");
            break;
        case 2:
            printf("Class : car\n");
            break;
        case 3:
            printf("Class : drone\n");
            break;
        case 4:
            printf("Class : group\n");
            break;
        case 5:
            printf("Class : horseride\n");
            break;
        case 6:
            printf("Class : paraglider\n");
            break;
        case 7:
            printf("Class : person\n");
            break;
        case 8:
            printf("Class : riding\n");
            break;
        case 9:
            printf("Class : truck\n");
            break;
        case 10:
            printf("Class : wakeboard\n");
            break;
        case 11:
            printf("Class : whale\n");
            break;
        default:
            printf("class error\n");
            break;
        }
        return class_number;
    }
    return last_class;
}

void print_data(ElementType *data, uint16_t n)
{
    uint16_t j;
    for (j = 0; j < n; j++)
    {
        if ((j % LENGTH) == 0)
        {
            printf("\r\n");
        }
        printf("%d ", data[j]);
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

    while (1)
    {
        // get data
        Videopass_get(spi_img_data);
        //		print_data(spi_img_data, NBYTE);

        // send
        for (i = 0; i <= NBYTE; i++)
        {
            spi_single[i] = 0xFF;
        }

        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        mdelay(5);

        ret = csi_spi_send(spi_t, spi_single, NBYTE + 1);
        //		printf("%x\n\r",spi_single[NBYTE]);
        mdelay(5);

        spi_single[NBYTE] = 0xFE;
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
            memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);

            //			print_data(spi_single,NBYTE+1);
            ret = csi_spi_send(spi_t, spi_single, NBYTE + 1);
            mdelay(6);
            if (ret < 0)
            {
                printf("send fail\r\n");
                mdelay(10000);
            }
        }
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    }
}

int main(void)
{
    //	printf("test");
    wujian100_spi_init(MY_USI_IDX);
    spi_handle_t handle = spi_t;
    int32_t ret;
    // wujian100_spi_test(0);

    uint32_t i,j;
    int32_t results[35];
    int last_class = -1;
//    int number = -1;
    ElementType x_min_224, y_min_224, x_max_224, y_max_224 = 0;
    printf("******************\n");
    printf("wujian100 startup!\n");

    while (1)
    {
        while (((SDMA->state) & 0x00000002) == 0)
            ;
        Detect();
        SDMA->state = ((SDMA->state) & 0xfffffffd);
        printf("detecting done\r\n");

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
            x_min_224 = (int32_t)(float_result_0 + obj_w * 16);
            y_min_224 = (int32_t)(float_result_1 + obj_h * 16);
            x_max_224 = (int32_t)(float_result_2 + obj_w * 16);
            y_max_224 = (int32_t)(float_result_3 + obj_h * 16);
            
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
        //			printf("%u\t",select_result[6]);
        //			printf("%u\t",1);
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
		
//		printf("%x%x%x%x\n\r",x_min_224,y_min_224,x_max_224,y_max_224);

        //get whole img
        Videopass_get(spi_img_data);

        //get acc result
        for (i = 0; i <= NBYTE; i++)
        {
            spi_single[i] = 0xFF;
        }
        spi_single[0] = (ElementType)x_min_224;
        spi_single[1] = (ElementType)y_min_224;
        spi_single[2] = (ElementType)x_max_224;
        spi_single[3] = (ElementType)y_max_224;
        spi_single[4] = (ElementType)select_result[6];
//		printf("uint8: ");
        print_data(spi_single, 5);

        //spi send
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        mdelay(5);

        ret = csi_spi_send(spi_t, spi_single, NBYTE + 1);
        //		printf("%x\n\r",spi_single[NBYTE]);
        mdelay(5);

        spi_single[NBYTE] = 0xFE;
        for (j = 0; j < (TOTALBYTE / NBYTE); j++)
        {
            memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);

            //			print_data(spi_single,NBYTE+1);
            ret = csi_spi_send(spi_t, spi_single, NBYTE + 1);
            mdelay(6);
            if (ret < 0)
            {
                printf("send fail\r\n");
                mdelay(10000);
            }
        }
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    }
}
