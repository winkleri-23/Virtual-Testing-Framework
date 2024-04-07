#ifndef __DLL_LLC_PAYLOAD_H_INCLUDED
#define __DLL_LLC_PAYLOAD_H_INCLUDED


 /*---------------------------------------------------------------------------*/
/** @brief Helper function: start parsing/composing of parameters
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_start(LLC_REQUEST_FRAME_T* rx_frame, LLC_RESPONSE_FRAME_T* tx_frame);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes processing of the request
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_rx_finish(void);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes composing of the answer
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_tx_finish(void);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: finishes interface function with an error
 * --------------------------------------------------------------------------
 **/
void dll_llc_payload_error(uint16_t error_code);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets UINT32 from input frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_uint32(uint32_t* p_u32);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT32 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint32(uint32_t u32);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets UINT16 from input frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_uint16(uint16_t* p_u16);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT16 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint16(uint16_t u16);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts UINT8 to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_uint8(uint8_t u8);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets IDENTIFIER from input frame (with max len)
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_name(char** p_name, uint32_t name_len);


/*---------------------------------------------------------------------------*/
/** @brief Helper function: gets actual parameters (length)
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_get_data(uint8_t** p_data, uint32_t data_len);



/*---------------------------------------------------------------------------*/
/** @brief Helper function: puts data array to the output frame
 * --------------------------------------------------------------------------
 **/
bool dll_llc_payload_put_data(uint8_t* p_data, uint32_t data_len);



#endif // included