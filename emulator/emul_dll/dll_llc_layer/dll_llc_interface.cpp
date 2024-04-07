#include <windows.h>
#include "emul_dll.h"
#include "emul_llc.h"
#include "logger.hpp"


// Select implementation of interface (mock or real functions)
#define EMUL_DLL_IFACE_DP
//#define EMUL_DLL_IFACE_REAL

// Declarations of service functions
#if defined(EMUL_DLL_IFACE_DP)
    #include "emul_interface.h"
#define EMUL_DLL_FUNC_ACTUAL(name)           name
    // all interface functions will have the name xxx_MOCK
#elif defined(EMUL_DLL_IFACE_REAL)
    #include "emul_interface_real.h"
#define EMUL_DLL_FUNC_ACTUAL(name)           name
#else
    #error "Select interface implementation"
#endif 
 

// Array of implemented interface functions
static const EMUL_DLL_INTERFACE_FUNCTION_T __dll_function_array[] =
{
    // POWERON
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(start_and_initialize)),
        .llc_code = EMUL_LLC_POWER_ON,
    },
    // WRITE_DATA_RECORD
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(write_data_record)),
        .llc_code = EMUL_LLC_WRITE_DATA_RECORD,
    },
    // READ_DATA_RECORD
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(read_data_record)),
        .llc_code = EMUL_LLC_READ_DATA_RECORD,
    },
    // QUERY DATAOBJECT HANDLE
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(query_do_handle)),
        .llc_code = EMUL_LLC_QUERY_DO_HANDLE,
    },
    // WRITE DATAOBJECT DATA
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(write_do_data)),
        .llc_code = EMUL_LLC_WRITE_DO_DATA,
    },
    // READ DATAOBJECT DATA
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(read_do_data)),
        .llc_code = EMUL_LLC_READ_DO_DATA,
    },
    // EMUL_LLC_EXECUTE_NS
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(execute_emulation_ns)),
        .llc_code = EMUL_LLC_EXECUTE_NS,
    },
    // EMUL_LLC_SET_MODULE
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(set_module)),
        .llc_code = EMUL_LLC_SET_MODULE,
    },
    // EMUL_LLC_INSERT_SUBMODULE
    {
        .function_name = EMUL_DLL_IFACE_FUNC_NAME_STRING(EMUL_DLL_FUNC_ACTUAL(insert_submodule)),
        .llc_code = EMUL_LLC_INSERT_SUBMODULE,
    },
    // Last item (endstop)
    {
        .function_name = NULL,
        .llc_code = 0,
    },
};


/*---------------------------------------------------------------------------*/
/** @brief Mandatory function - retrieves the list of supported API functions
 * --------------------------------------------------------------------------
 *
 * @retval  pointer to the array of structures (the last structure is filled with zeroes)
 *
 **/
EMULDLL_IFACE_FUNCTION EMUL_DLL_INTERFACE_FUNCTION_T* EMULDLL_get_list_of_functions(void)
{
    return const_cast<EMUL_DLL_INTERFACE_FUNCTION_T *>(&__dll_function_array[0]);
}


EMULDLL_IFACE_FUNCTION uint32_t EMULDLL_get_log_msg_len()
{
    if (Logger::msgQueue.isEmpty())
        return 0;

    return Logger::msgQueue.front().getSerializedLength();
}


/*---------------------------------------------------------------------------*/
/** @brief Mandatory function - gets message from DLL message buffer
 * --------------------------------------------------------------------------
 *
 * @param   outBuffer   pointer to output buffer array
 * @param   bufferLen   buffer length
 * @retval  DLL_GET_MSG_RESP_CODE status code
 *
 **/
EMULDLL_IFACE_FUNCTION DLL_GET_MSG_RESP_CODE EMULDLL_get_log_message(uint8_t* outBuffer, size_t bufferLen)
{
    if (Logger::msgQueue.isEmpty())
        return DLL_GET_MSG_RESP_CODE::NO_MSG;

    if (Logger::msgQueue.front().getSerializedLength() > bufferLen)
        return DLL_GET_MSG_RESP_CODE::BUFFER_OVERFLOW;

    auto ret = Logger::msgQueue.front().serialize(outBuffer, bufferLen);

    if (!ret.has_value())
        return DLL_GET_MSG_RESP_CODE::UNKNOWN_ERROR;

    Logger::msgQueue.pop();
    return DLL_GET_MSG_RESP_CODE::MSG_OK;
}