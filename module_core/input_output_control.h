#ifndef __INPUT_OUTPUT_H_INCLUDED
#define __INPUT_OUTPUT_H_INCLUDED
#include <cstdint>
#include <module_tech_config.h>





unsigned char DP_set_all_inputs(uint8_t* in_pt, uint8_t* qi_pt);

unsigned char DP_get_all_outputs(uint8_t* out_pt, uint8_t* qi_pt, uint8_t* od_pt);

#endif