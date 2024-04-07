#ifndef __EMUL_INTERFACE_DP_H_INCLUDED
#define __EMUL_INTERFACE_DP_H_INCLUDED

#include "emul_dll.h"

/*---------------------------------------------------------------------------*/
/** @brief Performs initialization of the module after power on
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(start_and_initialize);


/*---------------------------------------------------------------------------*/
/** @brief Queries a handle to a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(query_do_handle);


EMUL_DLL_DECLARE_IFACE_FUNC(write_data_record);


/*---------------------------------------------------------------------------*/
/** @brief Reads a data-record
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(read_data_record);

/*---------------------------------------------------------------------------*/
/** @brief Reads data from a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(read_do_data);

/*---------------------------------------------------------------------------*/
/** @brief Writes data to a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(write_do_data);

/*---------------------------------------------------------------------------*/
/** @Execute emulation for a given number of ns
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(execute_emulation_ns);

/*----------------------------------------------------------------*/
/** @brief Configures a module number (inserts module to slot)
 * ---------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(set_module);

/*------------------------------------------------------------------*/
/** @brief Configures a submodule (inserts submodule to a subslot)
 * -----------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(insert_submodule);


#endif // included