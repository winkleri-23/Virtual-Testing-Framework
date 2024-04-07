#include <windows.h>
#include <iostream>
#include "emul_dll.h"
#include "emul_llc.h"

// include for this file
#include "dll_llc_payload.h"

using namespace std;

// Variables for LLC RX-payload parsing
static LLC_REQUEST_FRAME_T* _rx_frame;
static uint8_t* _rx_payload_current;
static uint32_t _rx_payload_remain;

// Variables for LLC TX-payload composing
static LLC_RESPONSE_FRAME_T* _tx_frame;
static uint8_t* _tx_payload_current;
static uint32_t _tx_payload_remain;


/*---------------------------------------------------------------------------*/
/** @brief Helper function: start parsing/composing of parameters
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_start(LLC_REQUEST_FRAME_T * rx_frame, LLC_RESPONSE_FRAME_T * tx_frame)
{
    bool retval = false;
    // first of all, do not accept too short lengths
    if (rx_frame->header.message_len < sizeof(rx_frame->header)) {
        goto FINISH;
    };
    // start parsing
    _rx_frame = rx_frame;
    _rx_payload_current = rx_frame->message_data;
    // remaining = all sent data
    _rx_payload_remain = rx_frame->header.message_len - sizeof(rx_frame->header);
    // start composing of answer
    _tx_frame = tx_frame;
    _tx_payload_current = tx_frame->message_data;
    _tx_payload_remain  = sizeof(tx_frame->message_data);
    // set default error code to TX header
    _tx_frame->header.error_code = LLC_ERR_UNKNOWN;
    // ok
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes processing of the request
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_rx_finish(void)
{
    bool retval = false;
    // if the error code is not unset, respect it
    if (_tx_frame->header.error_code != LLC_ERR_UNKNOWN) {
        _tx_frame->header.message_len = sizeof(_tx_frame->header);
        _tx_frame->header.flags = 0;
        goto FINISH;
    };
    // no error from parsing, but input parameters must be consumed
    //cout << "payload finish, rem=" << _rx_payload_remain << endl;
    if (_rx_payload_remain != 0) {
        // error, ignore actual payload
        _tx_frame->header.message_len = sizeof(_tx_frame->header);
        _tx_frame->header.flags = 0;
        _tx_frame->header.error_code = LLC_ERR_WRONG_PAYLOAD_LENGTH;
        goto FINISH;
    };
    // ok
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes composing of the answer
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_tx_finish(void)
{
    bool retval = false;
    // if the error code is not unset, respect it
    if (_tx_frame->header.error_code != LLC_ERR_UNKNOWN) {
        _tx_frame->header.message_len = sizeof(_tx_frame->header);
        _tx_frame->header.flags = 0;
        goto FINISH;
    };
    // ok
    _tx_frame->header.message_len = (_tx_payload_current - _tx_frame->message_data) + sizeof(_tx_frame->header);
    _tx_frame->header.error_code = LLC_ERR_OK;
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes interface function with an error
 * --------------------------------------------------------------------------
 **/
void dll_llc_payload_error(uint16_t error_code)
{
    _tx_frame->header.error_code = error_code;
    _tx_frame->header.message_len = sizeof(_tx_frame->header);
    _tx_frame->header.flags = 0;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets UINT32 from input frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_uint32(uint32_t* p_u32)
{
    bool retval = false;
    static_assert(sizeof(uint32_t) == 4, "wrong size");
    if (_rx_payload_remain < sizeof(uint32_t)) {
        goto FINISH;
    };
    *p_u32 = *reinterpret_cast<uint32_t*>(_rx_payload_current);
    _rx_payload_current += sizeof(uint32_t);
    _rx_payload_remain  -= sizeof(uint32_t);
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT32 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint32(uint32_t u32)
{
    bool retval = false;
    static_assert(sizeof(uint32_t) == 4, "wrong size");
    if (_tx_payload_remain < sizeof(uint32_t)) {
        goto FINISH;
    };
    *reinterpret_cast<uint32_t*>(_tx_payload_current) = u32;
    _tx_payload_current += sizeof(uint32_t);
    _tx_payload_remain -= sizeof(uint32_t);
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets UINT16 from input frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_uint16(uint16_t* p_u16)
{
    bool retval = false;
    static_assert(sizeof(uint16_t) == 2, "wrong size");
    if (_rx_payload_remain < sizeof(uint16_t)) {
        goto FINISH;
    };
    *p_u16 = *reinterpret_cast<uint16_t*>(_rx_payload_current);
    _rx_payload_current += sizeof(uint16_t);
    _rx_payload_remain -= sizeof(uint16_t);
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT16 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint16(uint16_t u16)
{
    bool retval = false;
    static_assert(sizeof(uint16_t) == 2, "wrong size");
    if (_tx_payload_remain < sizeof(uint16_t)) {
        goto FINISH;
    };
    *reinterpret_cast<uint16_t*>(_tx_payload_current) = u16;
    _tx_payload_current += sizeof(uint16_t);
    _tx_payload_remain -= sizeof(uint16_t);
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT8 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint8(uint8_t u8)
{
    bool retval = false;
    static_assert(sizeof(uint8_t) == 1, "wrong size");
    if (_tx_payload_remain < sizeof(uint8_t)) {
        goto FINISH;
    };
    *reinterpret_cast<uint8_t*>(_tx_payload_current) = u8;
    _tx_payload_current += sizeof(uint8_t);
    _tx_payload_remain -= sizeof(uint8_t);
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets IDENTIFIER from input frame (with max len)
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_name(char** p_name, uint32_t name_len)
{
    bool retval = false;
    if (_rx_payload_remain < name_len) {
        goto FINISH;
    };
    // finish the string (the must be at least one zero)
    _rx_payload_current[name_len - 1] = 0;
    *p_name = reinterpret_cast<char*>(_rx_payload_current);
    _rx_payload_current += name_len;
    _rx_payload_remain -= name_len;
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets actual parameters (length)
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_data(uint8_t** p_data, uint32_t data_len)
{
    bool retval = false;
    if (_rx_payload_remain < data_len) {
        goto FINISH;
    };
    *p_data = reinterpret_cast<uint8_t*>(_rx_payload_current);
    _rx_payload_current += data_len;
    _rx_payload_remain -= data_len;
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts data array to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_data(uint8_t * p_data, uint32_t data_len)
{
    bool retval = false;
    if (_tx_payload_remain < data_len) {
        goto FINISH;
    };
    memcpy( _tx_payload_current, p_data, data_len );
    _tx_payload_current += data_len;
    _tx_payload_remain -= data_len;
    retval = true;
FINISH:;
    return retval;
}
