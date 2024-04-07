#ifndef __EMUL_DLL_H_INCLUDED
#define __EMUL_DLL_H_INCLUDED

// C-class for all of the DLL-interface functions
#define EMULDLL_EXTERNC  extern "C"

// Based on the project and usage type, select linkage type of interface functions
#if defined(EMULDLL_EXPORTS)
    // This is DLL project, interface functions are exported
    #define  EMULDLL_IFACE_FUNCTION  EMULDLL_EXTERNC __declspec(dllexport)
#elif defined(EMULDLL_IMPORTS)
    // This is application code (uses DLL as library)
    #define  EMULDLL_IFACE_FUNCTION  EMULDLL_EXTERNC __declspec(dllimport)
#elif defined(EMULDLL_EMBEDDING)
    // This is application which uses DLL code as normal code (no DLL)
    #define  EMULDLL_IFACE_FUNCTION  EMULDLL_EXTERNC 
#else
    // Unknown translation
    #error Unknown usage of DLL code
#endif


// Standard includes used
#include <stdint.h>

// Include for LLC definitions
#include "emul_llc.h"

// Structure describing one provided interface function
#pragma pack(push,4)
typedef struct _EMUL_DLL_INTERFACE_FUNCTION_S {
    // Name of the function (this string must be resolvable by GetProcAddress call)
    const char* function_name;
    // Supported code
    uint8_t llc_code;
} EMUL_DLL_INTERFACE_FUNCTION_T;
#pragma pack(pop)

enum DLL_GET_MSG_RESP_CODE : uint8_t {
    NO_MSG = 0,
    MSG_OK = 1,
    BUFFER_OVERFLOW = 2,
    UNKNOWN_ERROR = 3
};


/*---------------------------------------------------------------------------*/
/** @brief Mandatory function - retrieves the list of supported API functions
 * --------------------------------------------------------------------------
 *
 * @retval  pointer to the array of structures (the last structure is filled with zeroes)
 *
 **/
EMULDLL_IFACE_FUNCTION EMUL_DLL_INTERFACE_FUNCTION_T* EMULDLL_get_list_of_functions(void);

/*---------------------------------------------------------------------------*/
/** @brief Mandatory function - Gets next log message length
 * --------------------------------------------------------------------------
 *
 * @retval Length of the message, 0 if no message available
 *
 **/
EMULDLL_IFACE_FUNCTION uint32_t EMULDLL_get_log_msg_len();


/*---------------------------------------------------------------------------*/
/** @brief Mandatory function - gets message from DLL message buffer
 * --------------------------------------------------------------------------
 *
 * @param   outBuffer   pointer to output buffer array
 * @param   bufferLen   buffer length
 * @retval  DLL_GET_MSG_RESP_CODE status code
 *
 **/
EMULDLL_IFACE_FUNCTION DLL_GET_MSG_RESP_CODE EMULDLL_get_log_message(uint8_t* outBuffer, size_t bufferLen);

// Function type for DLL-identification function
typedef EMUL_DLL_INTERFACE_FUNCTION_T* (*EMUL_DLL_IDENT_FUNCTION)(void);

// Function type for DLL get message lenght
typedef uint32_t(*EMUL_DLL_GET_MSG_LEN_FUNC)(void);

// Function type for DLL get message function
typedef DLL_GET_MSG_RESP_CODE(*EMUL_DLL_GET_MSG_FUNC)(uint8_t* array, size_t len);

// Unified macro for interface function name
#define EMUL_DLL_IFACE_FUNC_NAME(name) EMULDLL_iface_##name
// Unified macro for interface function name string
#define EMUL_DLL_IFACE_FUNC_NAME_STRING_HELPER2(x) #x
#define EMUL_DLL_IFACE_FUNC_NAME_STRING_HELPER(x) EMUL_DLL_IFACE_FUNC_NAME_STRING_HELPER2(x)
#define EMUL_DLL_IFACE_FUNC_NAME_STRING(name) EMUL_DLL_IFACE_FUNC_NAME_STRING_HELPER(EMUL_DLL_IFACE_FUNC_NAME(name))

// Unified macro for declaring EMUL-DLL interface function
#define EMUL_DLL_DECLARE_IFACE_FUNC(name)                       \
    EMULDLL_IFACE_FUNCTION void EMUL_DLL_IFACE_FUNC_NAME(name)( \
        LLC_REQUEST_FRAME_T * rx_frame,                         \
        LLC_RESPONSE_FRAME_T * tx_frame)

// Function type for all EMUL-DLL interface LLC functions
typedef void (*EMUL_DLL_LLC_FUNCTION)(LLC_REQUEST_FRAME_T* rx_frame, LLC_RESPONSE_FRAME_T* tx_frame);

#endif // included