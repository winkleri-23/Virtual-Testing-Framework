#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <psapi.h>
#include <string>
#include <set>
// logging
#include "logger.hpp"
#include "logging.h"
// exception handler
#include "exception_handler.h"

// str to crc converter
#include "str2crc.hpp"

// Definitions for LLC protocol
#include "emul_llc.h"
// Definition for emulator DLL 
#include "emul_dll.h"
#include "app_view.hpp"
#include "view_handle.hpp"
#include "message_queue.hpp"
#include "dll_data.hpp"
//========================
// Logging flags
//========================
// Logging of the client sessions
#define LOG_SESSION             true
// Logging of processing of parameters
#define LOG_PARAMS              false
// Logging of DLL operations
#define LOG_DLL                 false
// Log discovering of DLL LLC service functions
#define LOG_DLL_FUNCTIONS       false
// Logging of client actions
#define LOG_CLIENT              false
// Logging of pipe operations
#define LOG_PIPE                false
// Logging of LLC errors
#define LOG_LLC_ERRORS          false
// Log execution of all the LLC codes
#define LOG_EXECUTE_LLC         false
// All frames received from pipe will be logged
#define LOG_RX_FRAMES           false
// All frames transmitted to the pipe will be logged
#define LOG_TX_FRAMES           false


// Enumeration for application return codes
typedef enum _MAIN_RETURN_CODE_E {
    // Everything is OK
    MAIN_RETVAL_OK = 0,
    // Starting failed = cannot open LLC pipe
    MAIN_RETVAL_FAILED_OPEN_PIPE = 1,
    // Pipe operation failed during the run
    MAIN_RETVAL_FAILED_RUN_PIPE = 2,
    // Error processing program parameters
    MAIN_RETVAL_BAD_PARAMETERS = 3,

} MAIN_RETURN_CODE_T;

// Enumeration for pipe FSM
typedef enum _PIPE_STATE_E {
    // No state
    PIPE_STATE_NONE,
    // start new session
    PIPE_STATE_START,
    // if we are trying to open a pipe
    PIPE_STATE_TRY_OPEN,
    // waiting after open error, before a retry
    PIPE_STATE_WAIT_OPEN,
    // Waiting for a client - trying to connect
    PIPE_STATE_WAIT_CLIENT,
    // Client is connected, performing communication, waiting for a disconnection
    PIPE_STATE_RUNNING,
} PIPE_STATE_T;

// Timeout [ms] when passive client gets disconnected from server
#define EMULATOR_CLIENT_TIMEOUT     3000
// Timeout [ms] after which server tries to open its pipe
#define EMULATOR_OPEN_RETRY         1000

// Flag for global cycle sleeping (false means that fast loop is required)
static bool main_sleep_flag;

static bool main_run_indefinitely = false;

static std::string emulator_pipe_name = EMUL_LLC_PIPE_NAME;
// Handle to opened pipe for receiving LLC communication
static HANDLE emulator_pipe_handle = INVALID_HANDLE_VALUE;
// Timeout value for server timeout (automatically disconnects the client
static uint32_t emulator_timeout;
// Timer for last received transaction 
static uint32_t emulator_last_transaction;
// Timer for last attempt of pipe opening
static uint32_t emulator_last_open;

// Current state of pipe FSM
static PIPE_STATE_T emulator_pipe_state = PIPE_STATE_NONE;
// Name of the computer connected to our pipe
static WCHAR client_computer[200];
// Process ID of the client
static ULONG client_process_id;
// Name of the module in client's process which connected to our pipe
static WCHAR client_module[MAX_PATH + 10];

// Receive message
static LLC_REQUEST_FRAME_T  rx_frame;
// Actually received bytes
static DWORD rx_bytes;

// Transmit message
static LLC_RESPONSE_FRAME_T  tx_frame;
// Actually sent bytes
static DWORD tx_bytes;

// Path to EMUL-DLL (default)
#define MAX_DLL_PATH_LEN  4096
static WCHAR emul_dll_path[MAX_DLL_PATH_LEN];

// Handle of loaded DLL with emulator
static HMODULE emul_dll_module = NULL;

// Structure for one discovered function in DLL
typedef struct _EMUL_DLL_FUNCTION_S EMUL_DLL_FUNCTION_T;
struct _EMUL_DLL_FUNCTION_S {
    // Descriptor passed from DLL by discovery function (code+name)
    EMUL_DLL_INTERFACE_FUNCTION_T  descriptor;
    // Discovered real address in DLL
    EMUL_DLL_LLC_FUNCTION function_address;
    // pointer to next
    EMUL_DLL_FUNCTION_T* next;
};

// Table of LLC service function pointers 
static EMUL_DLL_FUNCTION_T* emul_dll_functions = NULL;


/*---------------------------------------------------------------------------*/
/** @brief Helper function for 32-bit cyclic timestamp
 * --------------------------------------------------------------------------
 **/
static uint32_t _main_time(void)
{
    return static_cast<uint32_t>(GetTickCount64());
}


// Macro for logging at MAIN level
#define MAIN_LOG(flag,...)  do { if (flag) { _main_log(__VA_ARGS__); }; } while (0)


/*---------------------------------------------------------------------------*/
/** @brief Helper logging function
 * --------------------------------------------------------------------------
 **/
static void _main_log(const char* format, ...)
{
    static char linebuf[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(linebuf, sizeof(linebuf) - 3, format, arg);
    va_end(arg);
    Logger::log(LogType::Info, linebuf);
}


/*---------------------------------------------------------------------------*/
/** @brief Helper error function
 * --------------------------------------------------------------------------
 **/
static void _main_error(const char* format, ...)
{
    static char linebuf[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(linebuf, sizeof(linebuf) - 3, format, arg);
    va_end(arg);
    Logger::log(LogType::Error, linebuf);
}


/*---------------------------------------------------------------------------*/
/** @brief Backcall function from EXCEPTION handler for processsing log strings
 * --------------------------------------------------------------------------
 **/
void main_exception_log(char* str)
{
    FILE* f;
    static char exception_file_path[500] = { 0 };
    printf("%s", str);
    // prepare the path if necessary
    if (exception_file_path[0] == 0) {
        // prepare the path
        char* last;
        log_prepare_base_path(exception_file_path, sizeof(exception_file_path) - 2, &last);
        sprintf_s(last, sizeof(exception_file_path) - (last - exception_file_path) - 3, "%s", "exception.err");
    };
    f = NULL;
    fopen_s(&f, exception_file_path, "at");
    if (f != NULL) {
        fprintf(f, "%s", str);
        fclose(f);
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Helper function for brief dumping of a hex array
 * --------------------------------------------------------------------------
 **/
static char* _main_dump_hex_brief(uint8_t* buf, uint32_t buflen, uint32_t maxlen)
{
    static char outbuf[3 * 100 + 10];
    char* p;
    bool add_dots = false;
    if (buflen > maxlen) {
        buflen = maxlen;
        add_dots = true;
    };
    if (buflen > (sizeof(outbuf) - 10) / 3) {
        buflen = (sizeof(outbuf) - 10) / 3;
        add_dots = true;
    };
    p = outbuf;
    while (buflen--) {
        p += sprintf_s(p, outbuf + sizeof(outbuf) - p - 3, "%02X%s",
            *buf, ((buflen) ? "," : ""));
        buf++;
    };
    if (add_dots) {
        p += sprintf_s(p, outbuf + sizeof(outbuf) - p - 3, "...");
    };
    return outbuf;
}

/*---------------------------------------------------------------------------*/
/** @brief Opens LLC communication pipe (interprocess communication)
 * --------------------------------------------------------------------------
 * Creates a named pipe "EMULATOR_PIPE_NAME"
 *
 * @retval Retuns flag of success
 *
 **/
static bool _main_open_pipe(const std::string& name)
{
    bool retval = false;
    const WCHAR* pipename_prefix = L"\\\\.\\pipe\\";
    std::wstringstream cls;
    cls << pipename_prefix << name.c_str();
    std::wstring pipename = cls.str();

    MAIN_LOG(LOG_PIPE, "opening IPC pipe \"%s\"", name.c_str());
    // create the pipe
    emulator_pipe_handle =
        CreateNamedPipe(reinterpret_cast<LPCWSTR>(pipename.data()),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT | PIPE_REJECT_REMOTE_CLIENTS,
            1,   // only one instance allowed, e.g. this one
            1024, 1024,
            1,   // 1ms waiting time
            NULL);
    if (emulator_pipe_handle != INVALID_HANDLE_VALUE) {
        MAIN_LOG(LOG_PIPE, "opened successfully");
        retval = true;
    }
    else {
        _main_error("open failed, err=%d", GetLastError());
    };
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief Closes LLC communication pipe (interprocess communication)
 * --------------------------------------------------------------------------
 **/
static void _main_close_pipe(void)
{
    if (emulator_pipe_handle != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(emulator_pipe_handle);
        CloseHandle(emulator_pipe_handle);
        emulator_pipe_handle = INVALID_HANDLE_VALUE;
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Retrieves information about the connected client
 * --------------------------------------------------------------------------
 *
 * @retval returns flag of a fatal (unrecoverable) error
 **/
static void _main_discover_connected_client(void)
{
    DWORD error_code = 0;
    HANDLE client_process;

    // set default data
    lstrcpyW(client_computer, L"<no-data>");
    client_process_id = static_cast<ULONG>(UINT32_MAX);
    lstrcpyW(client_module, L"<no-data>");

    // get computer name
    if (GetNamedPipeClientComputerName(emulator_pipe_handle, client_computer, sizeof(client_computer) - 2)) {
        // discovery OK
    }
    else {
        // in case that was a local connection
        error_code = GetLastError();
        if (error_code == ERROR_SUCCESS || error_code == ERROR_PIPE_LOCAL) {
            lstrcpyW(client_computer, L"<local-computer>");
            error_code = 0;
        }
        else {
            wsprintf(client_computer, L"failed, error=%d\n", error_code);
        };
    };

    // name of the process
    GetNamedPipeClientProcessId(emulator_pipe_handle, &client_process_id);

    // discovery the client's modules
    client_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, client_process_id);
    if (client_process == NULL) {
        // no success
    }
    else {
        if (GetModuleFileNameEx(client_process, 0, client_module, sizeof(client_module) / sizeof(client_module[0]) - 2)) {
            // we have valid name
        }
        else {
            error_code = GetLastError();
            wsprintf(client_module, L"failed, error=%d\n", error_code);
        };
        CloseHandle(client_process);
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Unloads the DLL
 * --------------------------------------------------------------------------
 **/
static void _main_unload_DLL(void)
{
    // unregister DLL msg source and destroy msg queue
    viewhandle::unregister_msg_source(viewhandle::DLL_TAG);
    dll_data::deinit_queue();

    // unload Win32 module (disconnect from app)
    if (emul_dll_module != NULL) {
        MAIN_LOG(LOG_DLL, "unloading EMUL-DLL");
        FreeLibrary(emul_dll_module);
        emul_dll_module = NULL;
    };
    // free discovered functions
    while (emul_dll_functions != NULL) {
        EMUL_DLL_FUNCTION_T* next = emul_dll_functions->next;
        delete emul_dll_functions;
        emul_dll_functions = next;
    };

}


/*---------------------------------------------------------------------------*/
/** @brief Reloads the DLL (frees current, loads again)
 * --------------------------------------------------------------------------
 **/
static void _main_reload_DLL(void)
{
    EMUL_DLL_IDENT_FUNCTION ident_func;
    bool error_flag = true;
    EMUL_DLL_INTERFACE_FUNCTION_T* list_func;
    EMUL_DLL_LLC_FUNCTION iface_func;
    EMUL_DLL_FUNCTION_T* iface_record;

    EMUL_DLL_GET_MSG_FUNC getMsgFunc;
    EMUL_DLL_GET_MSG_LEN_FUNC getLenFunc;

    // unload if the is a dll loaded
    _main_unload_DLL();

    // Load the library by using the path
    MAIN_LOG(LOG_DLL, "loading EMUL-DLL");
    emul_dll_module = LoadLibrary(emul_dll_path);
    if (emul_dll_module == NULL) {
        _main_error("emulator DLL file not found");
        goto FINISH;
    };

    // Find the interface-list function
    MAIN_LOG(LOG_DLL, "loading EMUL-DLL services ...");
    ident_func = reinterpret_cast<EMUL_DLL_IDENT_FUNCTION>(GetProcAddress(emul_dll_module, "EMULDLL_get_list_of_functions"));
    if (ident_func == NULL) {
        // emulator DLL does not implement identification function
        _main_error("emulator DLL does not provide identification");
        goto FINISH;
    };

    // Call the identification func and get the list of provided interface functions
    list_func = (*ident_func)();

    // Process the list and create identified functions
    while (list_func->function_name != NULL) {
        MAIN_LOG(LOG_DLL_FUNCTIONS, "   service: %s, code %d", list_func->function_name, list_func->llc_code);
        // try to retrieve this interface function
        iface_func = reinterpret_cast<EMUL_DLL_LLC_FUNCTION>(GetProcAddress(emul_dll_module, list_func->function_name));
        if (iface_func == NULL) {
            // EMUL-DLL specifies interface function but does not implement it
            _main_error("emulator DLL does not implement this interface function!");
            goto FINISH;
        };
        // create the record
        iface_record = new EMUL_DLL_FUNCTION_T;
        if (iface_record == NULL) {
            _main_error("memory allocation error");
            goto FINISH;
        };
        // fill the record and insert to list (order does not matter)
        iface_record->descriptor = *list_func;
        iface_record->function_address = iface_func;
        iface_record->next = emul_dll_functions;
        emul_dll_functions = iface_record;
        // process next function
        list_func++;
    };

    getMsgFunc = reinterpret_cast<EMUL_DLL_GET_MSG_FUNC>(GetProcAddress(emul_dll_module, "EMULDLL_get_log_message"));
    getLenFunc = reinterpret_cast<EMUL_DLL_GET_MSG_LEN_FUNC>(GetProcAddress(emul_dll_module, "EMULDLL_get_log_msg_len"));
    dll_data::init_queue(getLenFunc, getMsgFunc);
    viewhandle::register_msg_source(viewhandle::DLL_TAG, dll_data::dllMsgQueue);

    // all functions loaded ok
    error_flag = false;

FINISH:;
    if (error_flag) {
        // there was some error, keep DLL unloaded
        _main_unload_DLL();
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Processes one request from the client
 * --------------------------------------------------------------------------
 **/
static void _main_process_LLC_request(void)
{
    LLC_MESSAGE_REQUEST_HEADER_T* rx_hdr;
    LLC_MESSAGE_RESPONSE_HEADER_T* tx_hdr;
    DWORD tx_bytes;

    //Log("data read: (%d bytes)\n", rx_bytes);
    //for (uint32_t i = 0; i < rx_bytes; i++) Log("\\x%02x", rx_buffer[i]);
    // Log("\n");

    MAIN_LOG(LOG_RX_FRAMES, "rx frame: %d bytes: %s", rx_bytes, _main_dump_hex_brief(rx_frame.raw, rx_bytes, 10));

    // Initial tests
    if (rx_bytes < sizeof(LLC_MESSAGE_REQUEST_HEADER_T)) {
        MAIN_LOG(LOG_LLC_ERRORS, "LLC: bad message, length=%d is less than minimum %d\n", rx_bytes, sizeof(LLC_MESSAGE_REQUEST_HEADER_T));
        // bad message
        return;
    };
    // we can analyze at least the header
    rx_hdr = &rx_frame.header;
    // check consistency
    if (rx_hdr->start_byte != LLC_MESSAGE_START_HEADER_BYTE) {
        MAIN_LOG(LOG_LLC_ERRORS, "LLC: bad message, invalid format\n");
        return;
    };

    // header length must not be less than header itself 
    if (rx_hdr->message_len < sizeof(LLC_MESSAGE_REQUEST_HEADER_T)) {
        MAIN_LOG(LOG_LLC_ERRORS, "LLC: bad message, invalid header length(% d)\n", rx_hdr->message_len);
    };

    // message length includes header as well as the payload
    if (rx_hdr->message_len != rx_bytes) {
        MAIN_LOG(LOG_LLC_ERRORS, "LLC: bad message, invalid length, expected %d, received %d\n", rx_hdr->message_len, rx_bytes);
        return;
    };
    // consistency check OK

    // prepare raw answer (by default no data, error not supported)
    tx_hdr = &tx_frame.header;
    *tx_hdr = {};
    tx_hdr->start_byte = LLC_MESSAGE_START_HEADER_BYTE;
    tx_hdr->message_code = rx_hdr->message_code | 0x80;
    tx_hdr->message_len = sizeof(*tx_hdr);
    tx_hdr->error_code = LLC_ERR_NOT_SUPPORTED;

    bool skipExecution = false;

    // special case: for the code "POWER-ON", reload the DLL and its functions
    if (rx_hdr->message_code == EMUL_LLC_POWER_ON) {

        // Load DLL if not loaded
        if (emul_dll_module == NULL)
            _main_reload_DLL();
        else
            skipExecution = true;

        tx_hdr->error_code = LLC_ERR_OK;
    };


    // now if DLL is not loaded (started), do nothing
    if (emul_dll_module != NULL && !skipExecution) {
        // we have the DLL loaded
        // try to find the message code in the supported functions by DLL
        EMUL_DLL_FUNCTION_T* f = emul_dll_functions;
        while (f != NULL) {
            if (f->descriptor.llc_code == rx_hdr->message_code) {
                // supported DLL call found
                break;
            };
            f = f->next;
        };
        if (f != NULL) {
            // function supported
            MAIN_LOG(LOG_EXECUTE_LLC, "executing: %s", f->descriptor.function_name);
            // call the DLL
            (*f->function_address)(&rx_frame, &tx_frame);
            // if the functions returned an error, display it
            if (tx_frame.header.error_code != LLC_ERR_OK) {
                _main_error("EMUL-DLL returns error code %d ", tx_frame.header.error_code);
            };
        }
        else {
            // this code is not supported
            _main_error("LLC code %d is not supported by EMUL-DLL", rx_hdr->message_code);
        };
    }


    // always send the answer back to pipe
    MAIN_LOG(LOG_TX_FRAMES, "tx frame: %d bytes: %s", tx_hdr->message_len, _main_dump_hex_brief(tx_frame.raw, tx_hdr->message_len, 10));
    MAIN_LOG(LOG_TX_FRAMES, "-------------------");

    // it can be default ("NOT SUPPORTED") answer or real answer prepared by DLL
    WriteFile(emulator_pipe_handle, tx_frame.raw, tx_hdr->message_len, &tx_bytes, NULL);
}


/*---------------------------------------------------------------------------*/
/** @brief Processes named pipe logic
 * --------------------------------------------------------------------------
 *
 * @retval PIPE_STATE_T last pipe state
 **/
static PIPE_STATE_T _main_process_pipe(void)
{
    bool connect_flag;
    bool data_read;
    bool check_timeout;
    DWORD error_code;
    uint32_t akt_time = _main_time();

    switch (emulator_pipe_state) {

        // start a new connection
    case PIPE_STATE_START:
        MAIN_LOG(LOG_SESSION, "==========================================");
        MAIN_LOG(LOG_SESSION, "new emulator session");
        emulator_pipe_state = PIPE_STATE_TRY_OPEN;
        break;

        // if we are trying to open a pipe
    case PIPE_STATE_TRY_OPEN:
        // Try to open the pipe
        if (!_main_open_pipe(emulator_pipe_name)) {
            // error opening pipe
            emulator_last_open = akt_time;
            emulator_pipe_state = PIPE_STATE_WAIT_OPEN;
        }
        else {
            // Pipe opened successfully
            MAIN_LOG(LOG_SESSION, "prepared to accept a client");
            emulator_pipe_state = PIPE_STATE_WAIT_CLIENT;
        };
        break;

        // waiting after open error, before a retry
    case PIPE_STATE_WAIT_OPEN:
        if (akt_time - emulator_last_open > EMULATOR_OPEN_RETRY) {
            emulator_pipe_state = PIPE_STATE_TRY_OPEN;
        };
        break;

        // if we are waiting for a client
    case PIPE_STATE_WAIT_CLIENT:
        connect_flag = false;
        if (ConnectNamedPipe(emulator_pipe_handle, NULL)) {
            // client connected
            connect_flag = true;
            // no connection or client already connected
        }
        else {
            // client could have connected in between
            DWORD last_error = GetLastError();
            if (last_error == ERROR_PIPE_CONNECTED) {
                // client connected
                connect_flag = true;
            }
            else {
                // other error, wait further
            };
        };
        if (connect_flag) {
            // we have a connection
            LOGW(LOG_CLIENT, L"Client connected.\n");
            // Client discovery
            _main_discover_connected_client();
            LOGW(LOG_CLIENT, L"   computer name: %s\n", client_computer);
            LOGW(LOG_CLIENT, L"   client PID:    %u\n", client_process_id);
            LOGW(LOG_CLIENT, L"   client module: %s\n", client_module);
            // goto the running state
            emulator_last_transaction = akt_time;
            emulator_pipe_state = PIPE_STATE_RUNNING;
        }
        else {
            //Log(".");
        };
        break;

        // if we have connected client
    case PIPE_STATE_RUNNING:
        // try to read message
        data_read = false;
        // if not read any data, by default check timeout
        check_timeout = true;
        rx_bytes = 0;
        if (!ReadFile(emulator_pipe_handle, rx_frame.raw, sizeof(rx_frame.raw) - 2, &rx_bytes, NULL)) {
            // error reading data
            error_code = GetLastError();
            // ignore some errors completely
            switch (error_code) {
                // these errors should be ignored
            case ERROR_SUCCESS:
            case ERROR_PIPE_LOCAL:
            case ERROR_NO_DATA:
                // some specially-handled error code. Ignored if there are actual data.
                if (rx_bytes != 0) {
                    // ignored error code
                    data_read = true;
                }
                else {
                    // silently ignore this error
                };
                break;

                // If pipe has been closed from the client's side
            case ERROR_BROKEN_PIPE:
                MAIN_LOG(LOG_SESSION, "IPC disconnected from the client");
                _main_close_pipe();
                // restart FSM
                emulator_pipe_state = PIPE_STATE_START;
                check_timeout = false;
                break;

                // uknown or real error
            default:
                _main_error("readfile error=%d, bytes=%d\n", GetLastError(), rx_bytes);
            };
        }
        else {
            // data read ok, process if nonzero read
            if (rx_bytes != 0) {
                data_read = true;
            };
        };

        // Process message if really read
        if (data_read) {
            _main_process_LLC_request();
            // at least something read, reset the timer
            emulator_last_transaction = akt_time;
        }
        else
            if (check_timeout) {
                // no data in this pass - check time
                // do not check in case of zero timeout value (== timeout disabled)
                if (emulator_timeout > 0) {
                    uint32_t seconds_elapsed = akt_time - emulator_last_transaction;
                    if (seconds_elapsed >= emulator_timeout) {
                        MAIN_LOG(LOG_SESSION, "client timeout - IPC disconnect");
                        _main_close_pipe();
                        // restart FSM
                        emulator_pipe_state = PIPE_STATE_START;
                    };
                }
                else {
                    // timeout disabled
                };
            }
            else {
                // no processing needed 
            };
        break;

    default:
        // ignore and set initial state
        emulator_pipe_state = PIPE_STATE_WAIT_CLIENT;
        break;
    };

    return emulator_pipe_state;
}


/*---------------------------------------------------------------------------*/
/** @brief Shows small help about the command line parameters
 * --------------------------------------------------------------------------
 **/
static void _main_show_help_parameters(void)
{
    _main_log("Available parameters:");
    _main_log("\t-L / --location <path_to_dll_file>     e.g.  -L ..\\example.dll");
    _main_log("\t-T / --timeout  <timeout>              IPC timeout in seconds, e.g. -T 10 or -T 0 for infinity");
    _main_log("\t-I / --infinite                        enable infinite run of the emulator");
    _main_log("\t-P / --pipe-name                       enable infinite run of the emulator");
    _main_log("\t--log-level <lvl>                      set log level");
    _main_log("\t--colored                              enables colored terminal output");
}


/*---------------------------------------------------------------------------*/
/** @brief Processes the command line parameters
 * --------------------------------------------------------------------------
 **/
static bool _main_process_parameters(int argc, char* argv[])
{
    uint8_t i;
    char* par;
    int num;

    std::set<uint32_t> usedCmds;

    for (i = 1; i < argc; i++) {

        auto cmdHash = str2crc(argv[i], std::strlen(argv[i]));

        if (usedCmds.contains(cmdHash))
        {
            _main_error("Double setting of one argument: \"%s\"", argv[i]);
            return false;
        }

        switch (cmdHash) {
        case "--infinite"_hash:
        case "-I"_hash:

            main_run_indefinitely = true;
            MAIN_LOG(LOG_PARAMS, "Enabled infinite emulator runs");
            break;

        case "--location"_hash:
        case "-L"_hash:

            usedCmds.insert("--location"_hash);
            usedCmds.insert("-L"_hash);

            i++;
            par = argv[i];
            MultiByteToWideChar(CP_ACP, 0, par, -1, emul_dll_path, MAX_DLL_PATH_LEN - 3);
            MAIN_LOG(LOG_PARAMS, "DLL location set to: \"%s\"", par);
            break;

        case "--pipe-name"_hash:
        case "-P"_hash:

            usedCmds.insert("--pipe-name"_hash);
            usedCmds.insert("-P"_hash);

            i++;
            emulator_pipe_name = std::string(argv[i]);
            break;

        case "--timeout"_hash:
        case "-T"_hash:

            usedCmds.insert("--timeout"_hash);
            usedCmds.insert("-T"_hash);

            i++;
            try {
                num = std::stoi(argv[i]);
            }
            catch (std::exception e) {
                _main_error("Parameter error: bad number \"%s\"", argv[i]);
                return false;
            }

            if (num < 0 || num>100000) {
                _main_error("Parameter error: IPC timeout out of range \"%s\"", num);
                return false;
            };

            if (num == 0) {
                MAIN_LOG(LOG_PARAMS, "IPC timeout was disabled (set to zero).");
            }
            else {
                MAIN_LOG(LOG_PARAMS, "IPC timeout set to %d seconds", num);
            };
            // set timeout in milliseconds
            emulator_timeout = num * 1000;
            break;

        case "--colored"_hash:
            viewhandle::enable_console_color_view();
            break;

        case "--log-level"_hash:
            i++;
            viewhandle::set_msg_lvl(argv[i]);
            break;

        default:
            _main_error("Parameter error: unknown switch \"%s\"", argv[i]);
            return false;
        }
    }

    return true;
}


/*---------------------------------------------------------------------------*/
/** @brief Main program function
 * --------------------------------------------------------------------------
 **/
int main(int argc, char* argv[])
{

    viewhandle::init_default_view();

    int retval = MAIN_RETVAL_OK;
    bool exit_app;

    // register our own handler for exceptions
    exception_handler_init();

    // initialize main server timeout (default 3 secs)
    emulator_timeout = EMULATOR_CLIENT_TIMEOUT;

    // emulator not loaded
    emul_dll_module = NULL;
    emul_dll_functions = NULL;

    // initialize main FSM
    emulator_pipe_state = PIPE_STATE_START;

    // load potential parameters
    if (!_main_process_parameters(argc, argv)) {
        viewhandle::run();
        _main_show_help_parameters();
        viewhandle::stop();
        return MAIN_RETVAL_BAD_PARAMETERS;
    };

    viewhandle::run();

    // initialize logging
    _main_log("application start");

    // Main program cycle
    PIPE_STATE_T last_state = emulator_pipe_state;
    exit_app = false;
    while (!exit_app) {
        // flag that if there is nothing to do, perform a short delay
        main_sleep_flag = true;

        // Task 1 - service our pipe (process clients, receive and answer commands)
        if (_main_process_pipe() != last_state) {
            if (last_state > emulator_pipe_state && !main_run_indefinitely)
                exit_app = true;

            last_state = emulator_pipe_state;
        };

        // Task 2 - service keyboard
        if (_kbhit()) {
            uint16_t key = _getch();
            switch (key) {
            case 27:
                retval = MAIN_RETVAL_OK;
                main_sleep_flag = false;
                exit_app = true;
                break;
            default:
                // ignore this key
                break;
            };
        };

        // If we have the flag of delay, sleep some time
        if (main_sleep_flag) {
            SleepEx(5, TRUE);
        }
        else {
            // immediately execute next action
        };
    };

    // unload DLL as it is not needed anymore
    _main_unload_DLL();
    // return value from the application
    viewhandle::stop();
    return retval;
}

