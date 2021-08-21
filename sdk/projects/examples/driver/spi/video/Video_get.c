#include "Video_get.h"

void Spidata_get(ElementType *pass_data)
{
    static uint8_t frame_num = 0;
    frame_num++;
    ElementType color = (frame_num & 0b111) << 5;
    memset(pass_data, color, TOTALBYTE);
    
    /*if more complicated test patterns are needed*/
    // uint16_t i, j, k;
    // color = (ElementType *)malloc(NCHANNEL);
    // for (i = 0; i < NCHANNEL; i++){
    //     color[i] = frame << 8;
    // }
    // for (i = 0; i < LENGTH; i++)
    // {
    //     for (j = 0; j < LENGTH; j++)
    //     {
    //         for (k = 0; k < NCHANNEL; k++)
    //             pass_data[i * LENGTH * NCHANNEL + j * NCHANNEL + k] = *color;
    //     }
    // }
}

void Videopass_get(ElementType *pass_data)
{
    uint16_t i, j, k;
    uint32_t temp_data;
    VIDEO->SR = 0x01;

    switch (NCHANNEL)
    {
    case (1):
        /*for specific (BRAM 1 channel) .bit file*/
        // for (i = 0; i < LENGTH; i++) 
        // {
        //     for (j = 0; j < 56; j++)
        //     {
        //         VIDEO->IR = i * (224 / LENGTH) * 56 + j;
        //         while (VIDEO->SR != 0x03)
        //             ;
        //         temp_data = VIDEO->OR;
        //         for (k = 0; k < (LENGTH / 56); k++)
        //         {
        //             // pass_data[i * LENGTH + j * (LENGTH / 56) + k] = (temp_data >> ((32 * 56 / LENGTH) * k)) & 0XFF;
        //             pass_data[i * LENGTH * NCHANNEL + j * (LENGTH / 56) + k] = (temp_data >> 8 * (3 - k));
        //         }
        //     }
        // }
        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 112; j++) // BRAM horizontal address range: 112
            {
                VIDEO->IR = i * (224 / LENGTH) * 112 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                ElementType r, g, b;
                int y;
                r = ((temp_data >> 16) & (0b1111100000000000)) >> 8;
                g = ((temp_data >> 16) & (0b0000011111100000)) >> 2;
                b = ((temp_data >> 16) & (0b0000000000011111)) << 3;
                y = FLOAT2FIX(0.2990f) * r + FLOAT2FIX(0.5870f) * g + FLOAT2FIX(0.1140f) * b - (128 << FIXQ);
                y >>= FIXQ - 2;
                pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 0] = (ElementType)y;
                if (LENGTH == 224)
                {
                    r = ((temp_data) & (0b1111100000000000)) >> 8;
                    g = ((temp_data) & (0b0000011111100000)) >> 2;
                    b = ((temp_data) & (0b0000000000011111)) << 3;
                    y = FLOAT2FIX(0.2990f) * r + FLOAT2FIX(0.5870f) * g + FLOAT2FIX(0.1140f) * b - (128 << FIXQ);
                    y >>= FIXQ - 2;
                    pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 1] = (ElementType)y;
                }
            }
        }
        break;
    case (2):
        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 112; j++)
            {
                VIDEO->IR = i * (224 / LENGTH) * 112 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                for (k = 0; k < (LENGTH / 56); k++)
                {
                    pass_data[i * LENGTH * NCHANNEL + j * (LENGTH / 56) + k] = (temp_data >> 8 * (3 - k));
                }
            }
        }
        break;
    case(3):
        /* not optimized for 224 (half effciency and not completed)*/
        // for (i = 0; i < LENGTH; i++){
        //     for (j = 0; j < LENGTH; j++){
        //         VIDEO->IR = i * (224 / LENGTH) * 112 + int(j / 2) * (224 / LENGTH);
        //         while (VIDEO->SR != 0x03)
        //             ;
        //         temp_data = VIDEO->OR;
        //         pass_data[i * LENGTH * NCHANNEL + j * NCHANNEL + 0] = (temp_data) & (0XFF000000 >> 8 * k);
        //         pass_data[i * LENGTH * NCHANNEL + j * NCHANNEL + 1] = (temp_data) & (0XFF000000 >> 8 * k);
        //         pass_data[i * LENGTH * NCHANNEL + j * NCHANNEL + 2] = (temp_data) & (0XFF000000 >> 8 * k);
        //         }
        //     }
        // }

        for (i = 0; i < LENGTH; i++)
        {
            for (j = 0; j < 112; j++) // BRAM horizontal address range: 112
            {
                VIDEO->IR = i * (224 / LENGTH) * 112 + j;
                while (VIDEO->SR != 0x03)
                    ;
                temp_data = VIDEO->OR;
                pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 0] = ((temp_data>>16) & (0b1111100000000000)) >> 8;
                pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 1] = ((temp_data>>16) & (0b0000011111100000)) >> 2;
                pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 2] = ((temp_data>>16) & (0b0000000000011111)) << 3;
                if (LENGTH == 224)                                                                                          
                {                                                                                                           
                    pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 3] = ((temp_data) & (0b1111100000000000)) >> 8;
                    pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 4] = ((temp_data) & (0b0000011111100000)) >> 2;
                    pass_data[i * LENGTH * NCHANNEL + j * (LENGTH * NCHANNEL / 112) + 5] = ((temp_data) & (0b0000000000011111)) << 3;
                }
            }
        }
        break;
        default:
            // printf("Not supported\n");
            break;
    }

    VIDEO->SR = 0x00;
}
