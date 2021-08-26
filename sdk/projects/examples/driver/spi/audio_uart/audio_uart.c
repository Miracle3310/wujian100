#include "audio_uart.h"
uint8_t audio_command[12][9] = {
    {0X7E, 0X05, 0X41, 0X00, 0X01, 0X45, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X02, 0X46, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X03, 0X47, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X04, 0X40, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X05, 0X41, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X06, 0X42, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X07, 0X43, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X08, 0X4C, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X09, 0X4D, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X0A, 0X4E, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X0B, 0X4F, 0XEF, '\r','\n'},
    {0X7E, 0X05, 0X41, 0X00, 0X0C, 0X48, 0XEF, '\r','\n'}};
int32_t usart_send_async(usart_handle_t usart, const void *data, uint32_t num)
{
    int time_out = 0x7ffff;
    tx_async_flag = 0;

    csi_usart_send(usart, data, num);

    while (time_out) {
        time_out--;

        if (tx_async_flag == 1) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    tx_async_flag = 0;
    return 0;
}

void usart_event_cb(int32_t idx, uint32_t event)
{
    uint8_t g_data[15];

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            tx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVE_COMPLETE:
            rx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVED:
            csi_usart_receive_query(uart_t, g_data, 15);

        default:
            break;
    }
}

int wujian100_uart_init(int32_t idx)
{
    int32_t ret;
    uart_t = csi_usart_initialize(idx,(usart_event_cb_t)usart_event_cb);
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

void wujian100_uart_send(uint8_t cls)
{
    static uint8_t last_class = 255;
    // if (last_class != cls)
    {
        last_class = cls;
        usart_send_async(uart_t, audio_command[cls], sizeof(audio_command[cls]));
    }
}
