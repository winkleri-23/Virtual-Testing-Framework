#include <windows.h>
#include <iostream>
#include <inttypes.h>

#include "emul_dll.h"
#include "emul_llc.h"
#include "dll_llc_payload.h"
#include <emulator.h>
#include <bitset>
#include <cstdio>
#include <map>
#include "logger.hpp"
#include <data_obj.hpp>



/*---------------------------------------------------------------------------*/
/** @brief Helper function which sets the data record status
 * --------------------------------------------------------------------------
 **/
static bool _set_4B_status(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    if (!dll_llc_payload_put_uint8(b1)) {
        return false;
    };
    if (!dll_llc_payload_put_uint8(b2)) {
        return false;
    };
    if (!dll_llc_payload_put_uint8(b3)) {
        return false;
    };
    if (!dll_llc_payload_put_uint8(b4)) {
        return false;
    };
    return true;
}


/*---------------------------------------------------------------------------*/
/** @brief Performs initialization of the module after power on
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(start_and_initialize)
{
    // start parameters 
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        Logger::log(LogType::Error, std::format("{} : Error - payload start", __func__));
        return;
    };
    // as there are no parameters in frame, finish them
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Error - rx finish", __func__));
        return;
    };

    if (!dll_llc_payload_tx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Error - tx finish", __func__));
        return;
    };
    Logger::log(LogType::Info, std::format("{} : DLL: POWERON + init params", __func__));
    start_and_initialize();
}



/*---------------------------------------------------------------------------*/
/** @brief Queries a handle to a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(query_do_handle)
{
    char* data_obj_name;
    uint32_t expected_len = 0;
    uint32_t item_len = 0;
    uint32_t handle = 0;

    // start parameters
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        return;
    };
    // get expected length of the data object
    if (!dll_llc_payload_get_uint32(&expected_len)) {
        Logger::log(LogType::Error, std::format("{} : Get data object expected len - ERR", __func__));
        return;
    };
    // get expected length of one item
    if (!dll_llc_payload_get_uint32(&item_len)) {
        Logger::log(LogType::Error, std::format("{} : Get item object expected len - ERR", __func__));
        return;
    };
    // get name
    if (!dll_llc_payload_get_name(&data_obj_name, sizeof(rx_frame->query_do.do_name))) {
        Logger::log(LogType::Error, std::format("{} : Get name- ERR", __func__));
        return;
    };
    // finish the request
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Finish payload - ERR", __func__));
        return;
    };


    auto query = DataObj::convert(std::string_view(data_obj_name));

    if (!query.has_value()) {
        Logger::log(LogType::Error, std::format("query_do_handle: Unknown handle \"{}\" received", data_obj_name));
        dll_llc_payload_error(LLC_ERR_DATAOBJ_INVALID_HANDLE);
        return;
    }

    handle = query.value();

    // object found
    if (!dll_llc_payload_put_uint32(handle)) {
        return;
    };

    // payload ok
    if (!dll_llc_payload_tx_finish()) {
        return;
    };
}


EMUL_DLL_DECLARE_IFACE_FUNC(write_data_record)
{

    uint16_t record_number;
    uint32_t record_len;
    uint8_t* record_data;
    bool ret_set_para;


    Logger::log(LogType::Info, std::format("{}", __func__));
    // start parameters
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        Logger::log(LogType::Error, std::format("{} : Write data record Start_parameters - ERR", __func__));
        return;
    };
    // get record number
    if (!dll_llc_payload_get_uint16(&record_number)) {
        Logger::log(LogType::Error, std::format("{} : Write data record record_number - ERR", __func__));
        return;
    };
    // get data len
    if (!dll_llc_payload_get_uint32(&record_len)) {
        Logger::log(LogType::Error, std::format("{} : Write data record record_len - ERR", __func__));
        return;
    };
    // get data
    if (!dll_llc_payload_get_data(&record_data, record_len)) {
        Logger::log(LogType::Error, std::format("{} : Record len: {}", __func__, record_len));
        return;
    };
    // finish the request
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Write data record  rx_finish - ERR", __func__));
        return;
    };


    // length is ok, copy data and perform necessary BASY operations
    // attempt write to data record which is known and has correct length
    emul_execute_set_default_mid();
    Logger::log(LogType::Debug, std::format("{} : record number: {}, record len {} ", __func__, record_number, record_len));
    emul_execute_set_conf(0, 1);

    if (!emul_execute_tech_check_ds_len(record_number, record_len)) {
        Logger::log(LogType::Error, std::format("{} : Check para ds_len- WRONG LENGTH", __func__));
        dll_llc_payload_error(LLC_ERR_DATAOBJ_INVALID_VALUES);
        dll_llc_payload_tx_finish();
        return;
    }

    if (!emul_execute_tech_check_para_ds(record_data, record_number, record_len)) {
        dll_llc_payload_error(LLC_ERR_DATAOBJ_INVALID_VALUES);
        dll_llc_payload_tx_finish();
        Logger::log(LogType::Error, std::format("{} :Check para ds - Did not succeded", __func__));
        return;
    };
    
    Logger::log(LogType::Debug, std::format("{} :Check para ds - success", __func__));

    ret_set_para = emul_execute_tech_set_para_ds(record_number, record_len, record_data);


    if (ret_set_para != true) {
        dll_llc_payload_tx_finish();
        return;
    };

    
    // Schedule the mandotary events for execution
    emul_execute_tech_perform_services();
    

    if (!_set_4B_status(0x00, 0x00, 0x00, 0x00)) {
        return;
    };

    if (!dll_llc_payload_tx_finish()) {
        return;
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Reads a data-record
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(read_data_record)
{

    uint16_t record_number;
    uint32_t record_len;


    DS128_Data_object curr_ds128;
    DS129_data_object curr_data_record;

    uint8_t* data_ptr;
    size_t data_object_size = 0;

    // start parameters
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        return;
    };
    // get record number
    if (!dll_llc_payload_get_uint16(&record_number)) {
        Logger::log(LogType::Error, std::format("Record number - error"));
        return;
    };

    // get data len
    if (!dll_llc_payload_get_uint32(&record_len)) {
        Logger::log(LogType::Error, std::format("Record len - error"));
        return;
    };

    // finish the request
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("Payload rx finish - error"));
        return;
    };

    Logger::log(LogType::Debug, std::format("Reading of rx_frame - done; Record number: {}", record_number));
    if (record_number == 128) {
        get_current_data_record_ds128(curr_ds128);
        data_ptr = curr_ds128.ds128;
        data_object_size = curr_ds128.len;
    }
    else if (record_number == 129) {
        get_current_data_record_ds129(curr_data_record);
        data_ptr = curr_data_record.DS129;
        data_object_size = curr_data_record.len;
    }
    else {
        Logger::log(LogType::Error, std::format("Unknown data object"));
        dll_llc_payload_error(LLC_ERR_DATAOBJ_UNKNOWN);
        return;
    }

    Logger::log(LogType::Debug, std::format("Curr data record len: {}; Data object size : {}", curr_data_record.len, data_object_size));
    // found, check the length
    if (record_len > data_object_size) {
        Logger::log(LogType::Error, std::format("Error - trying to read more bytes than there is in the current data record"));
        if (!_set_4B_status(0xDF, 0x80, 0xB1, 0x01)) {
            return;
        };
        dll_llc_payload_tx_finish();
        return;
    };


    /*LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "Read data record - ptr_last_val: \n");
    for (size_t i = 0; i < record_len; i++) {
        uint8_t byte_value = data_ptr[i];
        LOG_NAMUR_INTERFACE_ARRAYS(INTERFACE_INFO, "%02x ", byte_value);
    }
    LOG_NAMUR_INTERFACE_ARRAYS(INTERFACE_INFO, "\n");
    LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "End of data record\n");*/


    // reply with success
    if (!_set_4B_status(0x00, 0x00, 0x00, 0x00)) {
        return;
    };

    // add record length
    if (!dll_llc_payload_put_uint32(record_len)) {
        Logger::log(LogType::Error, std::format("Error while putting record_len into the tx_frame"));
        return;
    };

    // add the data (requested length)
    if (!dll_llc_payload_put_data(data_ptr, record_len)) {
        Logger::log(LogType::Error, std::format("Error while inserting data into the tx_frame"));
        return;
    };

    if (!dll_llc_payload_tx_finish()) {
        Logger::log(LogType::Error, std::format("Error - tx_finish"));
        return;
    };

}

/*---------------------------------------------------------------------------*/
/** @brief Reads data from a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(read_do_data)
{
    uint32_t handle;
    uint32_t read_offset;
    uint32_t read_len;
    uint8_t* data_ptr;

    //LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "%s\n", __func__);
    // start parameters
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        Logger::log(LogType::Error, std::format("payload_start failed"));
        return;
    };
    // get handle
    if (!dll_llc_payload_get_uint32(&handle)) {
        Logger::log(LogType::Error, std::format("Reading handle from the received frame failed"));
        return;
    };
    // get read offset
    if (!dll_llc_payload_get_uint32(&read_offset)) {
        Logger::log(LogType::Error, std::format("Reading offset from the received frame failed"));
        return;
    };
    // get write len
    if (!dll_llc_payload_get_uint32(&read_len)) {
        Logger::log(LogType::Error, std::format("Reading read_len from the received frame failed"));
        return;
    };
    // payload ok
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("Payload_rx_finish failed"));
        return;
    };

    // zero length is not allowed
    if (read_len == 0) {
        Logger::log(LogType::Error, std::format("read_len is equal to 0 - BAD_LENGTH"));
        dll_llc_payload_error(LLC_ERR_DATAOBJ_BAD_LENGTH);
        return;
    };

    data_ptr = nullptr;

    switch (handle) {
    case DataObj::to_integral(DataObj::SPI_IN):
        Logger::log(LogType::Debug, std::format("{} : Input data - {}", __func__, handle));
        data_ptr = emul_get_input_data();
        break;
    case DataObj::to_integral(DataObj::DIAG_BUFFER):
        Logger::log(LogType::Debug, std::format("{} : Diag buffer - {};", __func__, handle));
        Logger::log(LogType::Debug, std::format("offset: {}, readlen: {}",read_offset, read_len));
        data_ptr = emul_get_diagnostics(read_offset);
        break;
    case DataObj::to_integral(DataObj::OUT_DATA):
        Logger::log(LogType::Debug, std::format("{} : Diag buffer - {};", __func__, handle));
        data_ptr = emul_get_output_data();
        break;
    case DataObj::to_integral(DataObj::IN_DATA):
        Logger::log(LogType::Debug, std::format("{} : Diag buffer - {};", __func__, handle));
        data_ptr = emul_get_input_data();
        break;
    default:
        Logger::log(LogType::Error, std::format("read_do_data: Unknown handle - {}", handle));
        return;
    }
   


    // compose the answer
    if (!dll_llc_payload_put_uint32(handle)) {
        Logger::log(LogType::Error, std::format("Error while composing an answer - handle"));
        return;
    };
    // add read offset
    if (!dll_llc_payload_put_uint32(read_offset)) {
        Logger::log(LogType::Error, std::format("Error while composing an answer - read_offset"));
        return;
    };
    // add read length
    if (!dll_llc_payload_put_uint32(read_len)) {
        Logger::log(LogType::Error, std::format("Error while composing an answer - read_len"));
        return;
    };

    // put data from the object to output frame
    if (!dll_llc_payload_put_data(data_ptr, read_len)) {
        Logger::log(LogType::Error, std::format("Error while composing an answer - data"));
        return;
    };
    // no additional data
    if (!dll_llc_payload_tx_finish()) {
        Logger::log(LogType::Error, std::format("Error while composing an answer - finish"));
        return;
    };
}

/*---------------------------------------------------------------------------*/
/** @brief Writes data to a data object
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(write_do_data)
{
    uint32_t write_len;
    uint32_t write_offset;
    uint32_t handle;
    uint8_t* write_data;
    uint32_t value;
    std::bitset<32> bits;


    // start parameters
    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        Logger::log(LogType::Debug, "Error - Payload start");
        return;
    };
    // get handle
    if (!dll_llc_payload_get_uint32(&handle)) {
        Logger::log(LogType::Debug, "Error - reading handle");
        return;
    };
    // get write offset+
    if (!dll_llc_payload_get_uint32(&write_offset)) {
        Logger::log(LogType::Debug, "Error - reading write offset");
        return;
    };
    // get write len
    if (!dll_llc_payload_get_uint32(&write_len)) {
        Logger::log(LogType::Debug, "Error - reading write_len");
        return;
    };
    // get write data
    if (!dll_llc_payload_get_data(&write_data, write_len)) {
        Logger::log(LogType::Debug, "Error - reading data");
        return;
    };
    // payload ok
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Debug, "Error - rx_finish");
        return;
    };

    switch (handle) {
        case DataObj::to_integral(DataObj::SPI_OUT):
            {
                Logger::log(LogType::Debug,
                std::format("write_do_data: DataObj - {}", DataObj::convert(handle).value()));
                //LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "Input data handle found\n");
                memcpy(&value, write_data, write_len);
                emul_set_input_signal(value);
                break;
            }
        case DataObj::to_integral(DataObj::OUT_DATA):
            {
                //Logger::log(LogType::Debug,
            Logger::log(LogType::Debug, std::format("read_do_data: Input data - {}", handle));
                //LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "Output data handle found\n");
                emul_set_output_data(write_data, write_len);     
                break;
            }
        default:
            // LOG_SAMPLE_MODULE_INTERFACE(INTERFACE_INFO, "No matching handle\n");
            Logger::log(LogType::Error, std::format("read_do_data: Unknown handle - {}", handle));
            return;
    }

    if (!dll_llc_payload_tx_finish()) {
        return;
    };
}

/*---------------------------------------------------------------------------*/
/** @Erich Winkler - temporary - test purposes
 * --------------------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(execute_emulation_ns)
{

    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        Logger::log(LogType::Error, std::format("{} : Error - payload start", __func__));
        return;
    };

    uint32_t value_u32;
    if (!dll_llc_payload_get_uint32(&value_u32)) {
        Logger::log(LogType::Error, std::format("{} : Error - read value", __func__));
        return;
    };
    // finish the parameters
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Error - finish", __func__));
        return;
    };


    if (!_set_4B_status(0x00, 0x00, 0x00, 0x00)) {
        return;
    };

    if (!dll_llc_payload_tx_finish()) {
        return;
    };

    emul_execute_emulation((uint64_t)value_u32);
}


/*----------------------------------------------------------------*/
/** @brief Configures a module number (inserts module to slot)
 * ---------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(set_module)
{
    dll_llc_payload_error(LLC_ERR_NOT_IMPLEMENTED);
}


/*------------------------------------------------------------------*/
/** @brief Configures a submodule (inserts submodule to a subslot)
 * -----------------------------------------------------------------
 **/
EMUL_DLL_DECLARE_IFACE_FUNC(insert_submodule)
{
    uint32_t slot_index;
    uint32_t submoduleID;

    if (!dll_llc_payload_start(rx_frame, tx_frame)) {
        return;
    };

    if (!dll_llc_payload_get_uint32(&slot_index)) {
        Logger::log(LogType::Error, std::format("{} : Error - reading slot index", __func__));
        return;
    };

    if (!dll_llc_payload_get_uint32(&submoduleID)) {
        Logger::log(LogType::Error, std::format("{} : Error while reading submoduleID", __func__));
        return;
    };
    // finish the parameters
    if (!dll_llc_payload_rx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Error rx_finish", __func__));
        return;
    };
    if (!_set_4B_status(0x00, 0x00, 0x00, 0x00)) {
        return;
    };
    if (!dll_llc_payload_tx_finish()) {
        Logger::log(LogType::Error, std::format("{} : Error - tx_finish", __func__));
        return;
    };
    emul_set_submoduleID(submoduleID);
}
