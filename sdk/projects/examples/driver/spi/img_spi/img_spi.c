#include "img_spi.h"
ElementType XORCheck(ElementType *pass_data, uint32_t len)
{
    ElementType res = pass_data[0];
    for (int i = 1; i < len; i++){
        res ^= pass_data[i];
    }
    return res;
}
void spi_event_cb_fun(int32_t idx, spi_event_e event)
{
    // printf("\nspi_event_cb_fun:%d\n",event);
}

int wujian100_spi_init(int32_t idx) //idx->SPI_IDX
{
    int32_t ret;

    //spi_handle_t spi_t;

    // example_pin_spi_init(); //spi pin config

    spi_t = csi_spi_initialize(idx, spi_event_cb_fun);
    // spi_t = csi_spi_initialize(idx, NULL);

    if (spi_t == NULL)
    {
        printf(" csi_spi_initialize failed\n");
        return -1;
    }

    ret = csi_spi_config(spi_t, MY_SPI_CLK_RATE, SPI_MODE_MASTER,
                         SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
                         SPI_SS_MASTER_SW, 8);

    ret = csi_spi_config_block_mode(spi_t, 1);

    if (ret != 0)
    {
        printf("%s(), %d spi config error, %d\n", __func__, __LINE__, ret);
        return -1;
    }

    return 0;
}

void wujian100_spi_send()
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
    memcpy(spi_single, acc_result, 5);
    spi_single[NBYTE + CBYTE - 2] = 0xFF;
    spi_single[NBYTE + CBYTE - 1] = XORCheck(spi_single, NBYTE + CBYTE - 1);

    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
    ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
    csi_spi_ss_control(handle, SPI_SS_INACTIVE);
    // print_data(spi_single, NBYTE + CBYTE);

    // frame
    spi_single[NBYTE + CBYTE - 2] = 0x00;
    for (j = 0; j < (TOTALBYTE / NBYTE); j++)
    {
        memcpy(spi_single, spi_img_data + j * NBYTE, sizeof(ElementType) * NBYTE);
        spi_single[NBYTE] = j & 0xFF;
        spi_single[NBYTE + CBYTE - 1] = XORCheck(spi_single, NBYTE + CBYTE - 1);
        // print_data(spi_single, NBYTE + 1);
        csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
        // print_data(spi_single, NBYTE + CBYTE);
        for (i = 0; i < 0; i++)
        {
            csi_spi_ss_control(handle, SPI_SS_INACTIVE);
            csi_spi_ss_control(handle, SPI_SS_ACTIVE);
        }
        csi_spi_ss_control(handle, SPI_SS_INACTIVE);
        if (ret < 0)
        {
            printf("send fail\r\n");
            mdelay(10000);
        }
    }
    spi_single[NBYTE + CBYTE - 2] = 0xCC;
    spi_single[NBYTE + CBYTE - 1] = XORCheck(spi_single, NBYTE + CBYTE - 1);
    csi_spi_ss_control(handle, SPI_SS_ACTIVE);
    ret = csi_spi_send(spi_t, spi_single, NBYTE + CBYTE);
    csi_spi_ss_control(handle, SPI_SS_INACTIVE);
}
