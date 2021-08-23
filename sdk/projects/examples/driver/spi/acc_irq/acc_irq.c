#include "acc_irq.h"
void acc_irq_handle(void *args)
{
    drv_irq_disable(IRQ_ACC);
    static uint8_t count = 1;
    acc_done_flag = 0;
    // printf("acc intr! %d\n", count);
    if (count == 0)
    {
        while (((SDMA->state) & 0x00000002) == 0)
            ;
        weightsfactors_transfer(0);
    }
    else
    {
        SDMA->state = ((SDMA->state) & 0xfffffffe);
        switch (count)
        {
        case (1):
            fire(56, 32, 32, 56, 28, 32, 96, 1, 0);
            break;
        case (2):
            weightsfactors_transfer(1);
            break;
        case (3):
            fire(28, 96, 32, 28, 14, 32, 96, 2, 1);
            break;
        case (4):
            weightsfactors_transfer(2);
            break;
        case (5):
            fire(14, 96, 32, 14, 14, 32, 96, 3, 2);
            break;
        case (6):
            weightsfactors_transfer(3);
            break;
        case (7):
            fire(14, 96, 32, 14, 14, 32, 96, 4, 3);
            break;
        case (8):
            weightsfactors_transfer(4);
            break;
        case (9):
            fire(14, 96, 32, 14, 14, 32, 96, 5, 4);
            break;
        case (10):
            weightsfactors_transfer(5);
            break;
        case (11):
            fire(14, 96, 32, 14, 14, 32, 96, 6, 5);
            break;
        case (12):
            weightsfactors_transfer(6);
            break;
        case (13):
            fire(14, 96, 32, 14, 14, 32, 96, 7, 6);
            break;
        case (14):
            // printf("acc done\n");
            // printf("%d\n", count_print++);
            SDMA->state = ((SDMA->state) & 0xfffffffd);
            while (((SDMA->state) & 0x00000002) == 0)
                ;
            weightsfactors_transfer(0);
            acc_done_flag = 1;
        }
    }
    if (14 == count++)
        count = 1;
    if (!acc_done_flag)
        drv_irq_enable(IRQ_ACC);
}

void wujian100_irq_init(uint32_t idx)
{
    drv_irq_register(idx, acc_irq_handle);
    drv_irq_enable(idx);
    while (((SDMA->state) & 0x00000002) == 0)
        ;
    weightsfactors_transfer(0);
}

ElementType TransType(int32_t pos)
{
    if (pos < 0)
        return 0;
    else if (pos > 224)
        return 224;
    return (ElementType)pos;
}

void wujian100_get_acc_result()
{
    int32_t results[35];
    int last_class = -1;
    int32_t x_min_224, y_min_224, x_max_224, y_max_224 = 0;
    // while (((SDMA->state) & 0x00000002) == 0)
    //     ;
    // Detect();
    // SDMA->state = ((SDMA->state) & 0xfffffffd);
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

    acc_done_flag = 0;
    drv_irq_enable(IRQ_ACC);
}
