#include <stdio.h>
#include <iostream>
#include <diagnostics.h>
#include <emulator.h>
#include <module_data.h>

uint8_t diagnostics_buf[MAX_NUMBER_OF_CHANNEL];


uint8_t SET_DIAGNOSTICS(uint8_t ch, uint8_t in)
{
    switch (in) {
        case (1 || 3):
            diagnostics_buf[ch * 4] = DIAG_NO_ERROR;
            break;
        case 0:
            if (tested_module.get_WB_Enabled(ch)) {
                diagnostics_buf[ch * 4] = WB;
            }
            else {
                diagnostics_buf[ch * 4] = DIAG_NO_ERROR;
            }
            break;
        case 7:
            if (tested_module.get_SC_Enabled(ch)) {
                diagnostics_buf[ch * 4] = SC;
            }
            else {
                diagnostics_buf[ch * 4] = DIAG_NO_ERROR;
            }
            break;
        default:
            return 1;
    }
    diagnostics_buf[ch * 4 + 1] = 0x00;
    diagnostics_buf[ch * 4 + 2] = 0x00;
    diagnostics_buf[ch * 4 + 3] = 0x00;
    return 0;
}


// End emulator when the signal is done not the number of events

// line of the event when error occured
