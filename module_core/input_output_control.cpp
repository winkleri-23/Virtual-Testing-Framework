#include "input_output_control.h"
#include <cstring> 
#include <iostream>


uint8_t emul_input_data_buf[INPUT_OUTPUT_BUFFER_SIZE];

unsigned char DP_set_all_inputs(uint8_t* in_pt, uint8_t* qi_pt)
{    
    std::memcpy((void*)&emul_input_data_buf[IN_OFFSET],   // Destination
        in_pt + IN_OFFSET,                                   // Source
        IN_LEN);                                             // Length



    if (NULL != qi_pt)
    {
        memcpy((void*)&emul_input_data_buf[QI_OFFSET],
            qi_pt,
            QI_LEN);
    }
    unsigned char  cc;
    cc = 0;
    return cc;
}


unsigned char DP_get_all_outputs(uint8_t* out_pt, uint8_t* qi_pt, uint8_t* od_pt) {
    return 0;
}