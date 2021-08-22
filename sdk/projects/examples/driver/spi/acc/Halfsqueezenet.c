#include "Halfsqueezenet.h"

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
    // while (((SDMA->state) & 0x00000001) == 0)
    //     ;
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
    // while (((SDMA->state) & 0x00000001) == 0)
    //     ;
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
