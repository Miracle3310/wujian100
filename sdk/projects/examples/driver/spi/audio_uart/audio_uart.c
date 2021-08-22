#include "audio_uart.h"
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
    usart_send_async(uart_t, audio_command[cls], sizeof(audio_command[cls]));
}
