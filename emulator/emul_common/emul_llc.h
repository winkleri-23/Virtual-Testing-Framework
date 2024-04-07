#ifndef __EMUL_LLC_H_INCLUDED
#define __EMUL_LLC_H_INCLUDED

// Name of the emulator PIPE
#define EMUL_LLC_PIPE_NAME   "ET200SP_emulator_LLC_pipe"

// Enumeration type for LLC commands
typedef enum _LLC_MESSAGE_CODE_E {

    //===============================================================
    // Requests power cycle of the emulator, initializes the module
    //===============================================================
    //
    //   input_data     8 bytes     - request header
    // 
    //  output_data     8 bytes     - response header
    //
    EMUL_LLC_POWER_ON               = 1,


    //================================
    // Sets a parameter in emulator
    //================================
    // 
    //   input data:    8 bytes     - request header
    //                  32 bytes    - parameter name padded with zeroes
    //                   n bytes    - payload specific to parameter
    // 
    //   output data:   8 bytes     - response header
    //
    EMUL_LLC_SET_PARAM              = 2,


    //========================================================
    // Gets (reads) a parameter or data value from emulator
    //========================================================
    //
    //   input data     8 bytes     - request header
    //                  32 bytes    - parameter name padded with zeroes
    //
    //   output data:   8 bytes     - response header
    //                 32 bytes     - parameter name padded with zeroes
    //                  n bytes     - payload specific to parameter or no data in case of an error
    //
    EMUL_LLC_GET_PARAM              = 3,


    //==================================================================================
    // Writes a data-record
    //==================================================================================
    //
    //   input_data     8 bytes     - request header
    //                  4 bytes     - data record number:
    //                                      bits 0-15:  record number (aka record index)
    //                                      bits 16-31: unused
    //                  4 bytes     - length of written data (follows)
    //                  n bytes     - record data
    // 
    //  output_data     8 bytes     - response header
    //                  4 bytes     - data record write status (same as from FB)
    //                                      1 byte error-code B0, e.g. 0xDF
    //                                      1 byte error-code B1, e.g. 0x80
    //                                      1 byte error-code B2, e.g. 0xB0
    //                                      1 byte error-code B3, e.g. 0x00
    //
    EMUL_LLC_WRITE_DATA_RECORD      = 4,


    //==================================================================================
    // Reads a data-record
    //==================================================================================
    //
    //   input_data     8 bytes     - request header
    //                  4 bytes     - data record number:
    //                                      bits 0-15:  record number (aka record index)
    //                                      bits 16-31: unused
    //                  4 bytes     - read length
    // 
    //  output_data     8 bytes     - response header
    //                  4 bytes     - data record read status (same as from FB)
    //                                      1 byte error-code B0, e.g. 0xDF
    //                                      1 byte error-code B1, e.g. 0x80
    //                                      1 byte error-code B2, e.g. 0xB0
    //                                      1 byte error-code B3, e.g. 0x00
    //              only if STATUS is OK:
    //                  4 bytes     - length of data read (copy from request, data array follows)
    //                  n bytes     - record data (according to the length)
    //
    EMUL_LLC_READ_DATA_RECORD       = 5,


    //==================================================================================
    // Checks availability of a data objects, checks its length, returns handle to it
    //==================================================================================
    //
    //   input_data     8 bytes     - request header
    //                  4 bytes     - expected length of data object (or 0 if there should be no checking)
    //                  4 bytes     - expected length of one item (mandatory for stream data objects, 0 otherwise)
    //                  128 bytes   - data object name, padded with zeroes
    // 
    //  output_data     8 bytes     - response header
    //                  4 bytes     - handle to this data object
    //
    EMUL_LLC_QUERY_DO_HANDLE        = 6,


    //================================
    // Writes content of data object
    //================================
    //
    //   input_data       8 bytes   - request header
    //                    4 bytes   - handle of data object to write
    //                    4 bytes   - offset in data object to start write 
    //                    4 bytes   - length of data to write (must be conforming with DO)
    //                    n bytes   - data to write (actual size must correspond to length of data)
    // 
    //  output_data       8 bytes   - response header
    //                    4 bytes   - handle to the data object
    //                    4 bytes   - length of data written
    //                    n bytes   - additional data (specific to write operation to particular object type)
    //
    EMUL_LLC_WRITE_DO_DATA          = 7,

    //======================================
    // Reads content from data object
    //======================================
    //
    //   input_data       8 bytes   - request header
    //                    4 bytes   - handle of data object to read
    //                    4 bytes   - offset in data object to start reading 
    //                    4 bytes   - length of data to read (conforming with DO, if case of stream DO size of the item)
    // 
    //  output_data       8 bytes   - response header
    //                    4 bytes   - handle to the data object
    //                    4 bytes   - offset in data object to start reading 
    //                    4 bytes   - length of data read (0 in case of an error)
    //                    n bytes   - read data (actual size must correspond to length of data)
    //
    EMUL_LLC_READ_DO_DATA           = 8,

    //======================================================================
    // Executes simulation for a given number of nanoseconds of model time
    //======================================================================
    //
    //   input_data       8 bytes           - request header
    //                    4 bytes           - number of nanoseconds to execute
    // 
    //  output_data       8 bytes           - response header
    //                    4 bytes           - recommended polling interval in REAL microseconds
    //
    EMUL_LLC_EXECUTE_NS             = 9,

    //=============================================
    // Queries whether the simulation still runs
    //=============================================
    //
    //   input_data       8 bytes           - request header
    // 
    //  output_data       8 bytes           - response header
    //                    4 bytes           - remaining nanoseconds to execute (0 means all done = step finished)
    //
    EMUL_LLC_QUERY_EXECUTION        = 10,

    //======================================================
    // Configures a module number (inserts module to slot)
    //======================================================
    //
    //   input_data       8 bytes           - request header
    //                    4 bytes           - module number (MID)
    // 
    //  output_data       8 bytes           - response header
    //                    4 bytes           - return code (false => module number can't be used)
    //
    EMUL_LLC_SET_MODULE             = 11,

    //==========================================================
    // Configures a submodule (inserts submodule to a subslot)
    //==========================================================
    //
    //   input_data       8 bytes           - request header
    //                    4 bytes           - subslot index (indexed from zero, i.e.  0 .. SUBSLOT_COUNT-1)
    //                    4 bytes           - submodule number (SMID)
    // 
    //  output_data       8 bytes           - response header
    //
    EMUL_LLC_INSERT_SUBMODULE       = 12,

    //==============================================================
    // Deconfigures a submodule (removes submodule from a subslot)
    //==============================================================
    //
    //   input_data       8 bytes           - request header
    //                    4 bytes           - subslot index (indexed from zero, i.e.  0 .. SUBSLOT_COUNT-1)
    // 
    //  output_data       8 bytes           - response header
    //
    EMUL_LLC_REMOVE_SUBMODULE       = 13,

    //=================================================================
    // Deparametrizes a submodule (removes parameters from a subslot)
    //=================================================================
    //
    //   input_data       8 bytes           - request header
    //                    4 bytes           - subslot index (indexed from zero, i.e.  0 .. SUBSLOT_COUNT-1)
    // 
    //  output_data       8 bytes           - response header
    //
    EMUL_LLC_DEPARA_SUBMODULE       = 14,

    EMUL_LLC_READ_QUEUE       = 15,

    // Maximum service code provided by EMULATOR (range 1..MAX-1) must be supported
    EMUL_LLC_MAXIMUM_CODE,

} LLC_MESSAGE_CODE_T;

// Fixed byte which starts every LLC message
#define LLC_MESSAGE_START_HEADER_BYTE       0xA2


// Structure for request/response header
#pragma pack(push,1)
typedef struct _LLC_MESSAGE_REQUEST_HEADER_S {
    // 1 byte fixed code 0xA2
    uint8_t start_byte;
    // 1 byte request code
    uint8_t message_code;
    // 2 bytes message length (LE)
    uint16_t message_len;
    // 4 bytes reserved (zeroes)
    uint8_t reserved[4];
} LLC_MESSAGE_REQUEST_HEADER_T;
#pragma pack(pop)
static_assert(sizeof(LLC_MESSAGE_REQUEST_HEADER_T) == 8, "bad packing");

// Request buffer (receive frame)
#pragma pack(push,1)
typedef union _LLC_REQUEST_FRAME_U {
    uint8_t raw[64 * 1024];
    struct {
        LLC_MESSAGE_REQUEST_HEADER_T header;
        union {
            // raw data of payload (up to 64k)
            uint8_t message_data[64 * 1024];
            // EMUL_LLC_POWER_ON
            struct {
                // nothing
            } power_on;

            // EMUL_LLC_SET_PARAM
            struct {
                char parameter_name[32];
                uint8_t parameter_data[1];
            } set_param;

            // EMUL_LLC_GET_PARAM
            struct {
                char parameter_name[32];
            } get_param;

            // EMUL_LLC_WRITE_DATA_RECORD
            struct {
                uint32_t data_record_number;
                uint32_t write_length;
                uint8_t record_data[1];
            } write_data_record;

            // EMUL_LLC_READ_DATA_RECORD
            struct {
                uint32_t data_record_number;
            } read_data_record;

            // EMUL_LLC_QUERY_DO_HANDLE
            struct {
                uint32_t exp_len_do;
                uint32_t exp_len_item;
                char do_name[128];
            } query_do;

            // EMUL_LLC_WRITE_DO_DATA
            struct {
                uint32_t handle;
                uint32_t offset;
                uint32_t length;
                uint8_t data[1];
            } write_do;

            // EMUL_LLC_READ_DO_DATA
            struct {
                uint32_t handle;
                uint32_t length;
            } read_do;

            // EMUL_LLC_EXECUTE_NS
            struct {
                uint32_t ns;
            } execute_ns;

            // EMUL_LLC_QUERY_EXECUTION
            struct {
                // nothing
            } query_exec;

            // EMUL_LLC_SET_MODULE
            struct {
                uint32_t module_id;
            } set_module;

            // EMUL_LLC_INSERT_SUBMODULE
            struct {
                uint32_t subslot_index;
                uint32_t submodule_id;
            } insert_submodule;

            // EMUL_LLC_REMOVE_SUBMODULE
            struct {
                uint32_t subslot_index;
            } remove_submodule;

            // EMUL_LLC_DEPARA_SUBMODULE
            struct {
                uint32_t subslot_index;
            } depara_submodule;

        };
    };
} LLC_REQUEST_FRAME_T;
#pragma pack(pop)


// Enumeration for LLC response frame error codes
typedef enum _LLC_ERROR_CODE_E {
    // Everything is OK
    LLC_ERR_OK                          = 0,

    //========== General errors =========
    // Function not supported (for codes not supported by EMUL-DLL)
    LLC_ERR_NOT_SUPPORTED               = 1001,
    // Function supported, but not implemented
    LLC_ERR_NOT_IMPLEMENTED             = 1002,
    // Wrong length of command payload
    LLC_ERR_WRONG_PAYLOAD_LENGTH        = 1003,

    //========== GET/SET parameter ==========
    // This parameter name was not recognized
    LLC_ERR_PARAMETER_UNKNOWN           = 2001,
    // Parameter value is out of range
    LLC_ERR_PARAMETER_OUT_OF_RANGE      = 2002,

    //========= DATA objects ================
    // Unknown dataobject name
    LLC_ERR_DATAOBJ_UNKNOWN             = 3001,
    // Wrong dataobject length
    LLC_ERR_DATAOBJ_BAD_LENGTH          = 3002,
    // Wrong dataobject item length
    LLC_ERR_DATAOBJ_BAD_ITEM_LENGTH     = 3003,
    // Requested data object handle is invalid
    LLC_ERR_DATAOBJ_INVALID_HANDLE      = 3004,
    // Data object is not writable
    LLC_ERR_DATAOBJ_IS_NOT_WRITABLE     = 3005,
    // Write operation is out of range of the dataobject
    LLC_ERR_DATAOBJ_WRITE_OUT_OF_RANGE  = 3006,
    // Data object is not readable
    LLC_ERR_DATAOBJ_IS_NOT_READABLE     = 3007,
    // Read oepration is out of range of the dataobject
    LLC_ERR_DATAOBJ_READ_OUT_OF_RANGE   = 3008,

    // Data object consists of invalid values
    LLC_ERR_DATAOBJ_INVALID_VALUES = 3009,

    //========= SIMULATION execution ========
    // Cannot start simulation again, previous step is still running
    LLC_ERR_EXEC_STILL_RUNNING          = 4001,

    //========= CONFIGURATION ===============
    // Cannot configure submodules, module was not set
    LLC_ERR_CONF_MODULE_NOT_CONFIGURED  = 5001,
    // Subslot index is wrong (not available for this module ID)
    LLC_ERR_CONF_BAD_SUBSLOT_INDEX      = 5002,
    // This submodule ID cannot be used for this subslot index
    LLC_ERR_CONF_BAD_SUBMODULE          = 5003,

    // Unknown error (default value)
    LLC_ERR_UNKNOWN                     = 0xFFFF
} LLC_ERROR_CODE_T;


// Structure for response header
#pragma pack(push,1)
typedef struct _LLC_MESSAGE_RESPONSE_HEADER_S {
    // 1 byte fixed code 0xA2
    uint8_t start_byte;
    // 1 byte request code (bit 7 set)
    uint8_t message_code;
    // 2 bytes message length (LE)
    uint16_t message_len;
    // 2 bytes error code (reaction to the request, see LLC_ERROR_CODE_T)
    uint16_t error_code;
    // 1 byte flags 
    uint8_t flags;
    // 1 byte reserved
    uint8_t reserved;
} LLC_MESSAGE_RESPONSE_HEADER_T;
#pragma pack(pop)
static_assert(sizeof(LLC_MESSAGE_RESPONSE_HEADER_T) == 8, "bad len");


// Response buffer (transmit frame)
#pragma pack(push,1)
typedef union _LLC_RESPONSE_FRAME_U {
    uint8_t raw[64 * 1024];
    struct {
        LLC_MESSAGE_RESPONSE_HEADER_T header;
        union {
            // raw data (up to 64k)
            uint8_t message_data[64 * 1024];

            // EMUL_LLC_POWER_ON
            struct {
                // nothing
            } power_on;

            // EMUL_LLC_SET_PARAM
            struct {
                // nothing
            } set_param;

            // EMUL_LLC_GET_PARAM
            struct {
                char parameter_name[32];
                uint8_t data[1];
            } get_param;

            // EMUL_LLC_WRITE_DATA_RECORD
            struct {
                uint8_t status_bytes[4];
            } write_data_record;

            // EMUL_LLC_READ_DATA_RECORD
            struct {
                uint8_t status_bytes[4];
                uint32_t read_length;
                uint8_t record_data[1];
            } read_data_record;

            // EMUL_LLC_QUERY_DO_HANDLE
            struct {
                uint32_t  handle;
            } query_do;

            // EMUL_LLC_WRITE_DO_DATA
            struct {
                uint32_t handle;
                uint32_t length;
            } write_do;

            // EMUL_LLC_READ_DO_DATA
            struct {
                uint32_t handle;
                uint32_t length;
                uint8_t data[1];
            } read_do;

            // EMUL_LLC_EXECUTE_NS
            struct {
                uint32_t poll_us;
            } execute_ns;

            // EMUL_LLC_QUERY_EXECUTION
            struct {
                uint32_t remaining_ns;
            } query_exec;

            // EMUL_LLC_SET_MODULE
            struct {
                uint32_t return_code;
            } set_module;

            // EMUL_LLC_INSERT_SUBMODULE
            struct {
                // none
            } insert_submodule;

            // EMUL_LLC_REMOVE_SUBMODULE
            struct {
                // none
            } remove_submodule;

            // EMUL_LLC_DEPARA_SUBMODULE
            struct {
                // none
            } depara_submodule;

        };
    };
} LLC_RESPONSE_FRAME_T;
#pragma pack(pop)


// Structure for PRAL records
#pragma pack(push,1)
typedef union _LLC_PRAL_RECORD_U {
    // structured data
    struct {
        // channel index
        uint8_t channel_index;
        // PRAL parameter (parameter of the hardware interrupt)
        uint8_t pral_parameter;
        // count of events (multiple PRALs can be queued as single record)
        uint8_t event_count;
    } pral_data;
    // raw data (structure size aligned for faster access)
    uint32_t raw_data;
} LLC_PRAL_RECORD_T;
#pragma pack(pop)
static_assert(sizeof(LLC_PRAL_RECORD_T) == 4, "wrong size");


#endif // included

