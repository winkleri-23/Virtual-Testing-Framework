#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "exception_handler.h"

// Configuration - this external function will be called for output "logging" strings
//  - expected prototype:  void FUNC( char * string_to_log );
//  - if defined, it will be declared as external function with the above prototype
#define EXCEPTION_HANDLER_LOG_FUNC               main_exception_log
// Configuration - this action will be done with one string to log
//  - if not defined, default action is to call user function
//  - if neither function nor action is defined, there will be no logging output at all
//#define EXCEPTION_HANDLER_LOG_ACTION(str)      fputs(str, stdout)


// Declare user external function
#ifdef EXCEPTION_HANDLER_LOG_FUNC
// Declare extern user function
extern void EXCEPTION_HANDLER_LOG_FUNC(char* string_to_log);
#endif

// If logging action is not defined (but function is defined), define default logging action
#if !defined(EXCEPTION_HANDLER_LOG_ACTION) && defined(EXCEPTION_HANDLER_LOG_FUNC)
#define EXCEPTION_HANDLER_LOG_ACTION(str)   do { EXCEPTION_HANDLER_LOG_FUNC(str); } while(0)
#endif

#ifdef EXCEPTION_HANDLER_LOG_ACTION
/*---------------------------------------------------------------------------*/
/** @brief Converts exception code to readable text
 * --------------------------------------------------------------------------
 **/
static void exception_handler_log (const char * fmt, ...)
{
    static char out_string[300];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(out_string, sizeof(out_string)-2, fmt, arg);
    va_end(arg);
    // Pass the resulting string to 
    EXCEPTION_HANDLER_LOG_ACTION(out_string);
}
#else
static inline exception_handler_log(const char* fmt, ...)
{
    (void)fmt;
}
#endif


// Table of KNOWN exception names
static const struct _EXCEPTION_RECORD_S 
{
    DWORD exc_code;
    const char * exc_text;
} __exception_names[] = 
{
    {
        EXCEPTION_ACCESS_VIOLATION,             "exc_ACCESS_VIOLATION"
    },
    {
        EXCEPTION_BREAKPOINT,                   "exc_BREAKPOINT"
    },
    {
        EXCEPTION_DATATYPE_MISALIGNMENT,        "exc_DATATYPE_MISALIGNMENT"
    },
    {
        EXCEPTION_SINGLE_STEP,                  "exc_SINGLE_STEP"
    },
    {
        EXCEPTION_ARRAY_BOUNDS_EXCEEDED,        "exc_ARRAY_BOUNDS_EXCEEDED"
    },
    {
        EXCEPTION_FLT_DENORMAL_OPERAND,         "exc_FLT_DENORMAL_OPERAND"
    },
    {
        EXCEPTION_FLT_DIVIDE_BY_ZERO,           "exc_FLT_DIVIDE_BY_ZERO"
    },
    {
        EXCEPTION_FLT_INEXACT_RESULT,           "exc_FLT_INEXACT_RESULT"
    },
    {
        EXCEPTION_FLT_INVALID_OPERATION,        "exc__FLT_INVALID_OPERATION"
    },
    {
        EXCEPTION_FLT_OVERFLOW,                 "exc_FLT_OVERFLOW"
    },
    {
        EXCEPTION_FLT_STACK_CHECK,              "exc_FLT_STACK_CHECK"
    },
    {
        EXCEPTION_FLT_UNDERFLOW,                "exc_FLT_UNDERFLOW"
    },
    {
        EXCEPTION_INT_DIVIDE_BY_ZERO,           "exc_INT_DIVIDE_BY_ZERO"
    },
    {
        EXCEPTION_INT_OVERFLOW,                 "exc_INT_OVERFLOW"
    },
    {
        EXCEPTION_PRIV_INSTRUCTION,             "exc_PRIV_INSTRUCTION"
    },
    {
        EXCEPTION_NONCONTINUABLE_EXCEPTION,     "exc_NONCONTINUABLE_EXCEPTION"
    },
};


/*---------------------------------------------------------------------------*/
/** @brief Converts exception code to readable text
 * --------------------------------------------------------------------------
 **/
static char * exception_handler_get_string(DWORD ex_code)
{
    uint8_t i;
    for (i = 0; i < sizeof(__exception_names) / sizeof(__exception_names[0]); i++) {
        if (__exception_names[i].exc_code == ex_code) {
            return const_cast<char *>(__exception_names[i].exc_text);
        };
    };
    // unknown code
    static char exc_long_buffer[50];
    snprintf(exc_long_buffer, sizeof(exc_long_buffer)-2, "EXC: %u", ex_code);
    return exc_long_buffer;
}


/*---------------------------------------------------------------------------*/
/** @brief Discovers the location of the fault
 * --------------------------------------------------------------------------
 **/
static bool exception_handler_discover_location(
                        PVOID fault_addr, 
                        char * buffer_module_name, DWORD buffer_module_name_len, 
                        DWORD * ptr_section_fault, DWORD * ptr_offset_fault, char ** ptr_section_name )
{
    bool retval = false;
    MEMORY_BASIC_INFORMATION mbi;
    HMODULE hmodule;
    static wchar_t long_module_buffer[500];
    IMAGE_DOS_HEADER * pDosHdr;
    IMAGE_NT_HEADERS * nt_header;
    IMAGE_SECTION_HEADER * ptr_section;
    DWORD rva;
    int i;

    // Query the virtual address and find a module which lies on this virtual address in process space
    if (!VirtualQuery(fault_addr, &mbi, sizeof(mbi))) {
        // error querying -> detection is not possible
        goto FINISH;
    };

    // Get the module handle
    hmodule = (HMODULE)mbi.AllocationBase;

    // Retrieve the module name for this handle
    if (!GetModuleFileName(hmodule, long_module_buffer, sizeof(long_module_buffer)/sizeof(long_module_buffer[0])) ) {
        // not possible do detect the module name
        return false;
    };
    // Convert to ASCII
    WideCharToMultiByte(CP_ACP, 0, long_module_buffer, -1, buffer_module_name, buffer_module_name_len-1, NULL, NULL);

    // Point to the DOS header in memory
    pDosHdr = (IMAGE_DOS_HEADER *)hmodule;

    // From the DOS header, find the NT (PE) header
    nt_header = (IMAGE_NT_HEADERS *)( (DWORD)pDosHdr + pDosHdr->e_lfanew );

    ptr_section = IMAGE_FIRST_SECTION(nt_header);
    // RVA is offset from module load address
    rva = (DWORD)fault_addr - (DWORD)hmodule; 

    // Iterate through the section table, looking for the one that encompasses the linear address.
    for (i = 0; i < nt_header->FileHeader.NumberOfSections; i++)  {

        DWORD section_beg = ptr_section->VirtualAddress;
        DWORD section_end = section_beg + max(ptr_section->SizeOfRawData, ptr_section->Misc.VirtualSize);

        if (rva >= section_beg && rva <= section_end) {
            *ptr_section_fault = i+1;
            *ptr_offset_fault = rva - section_beg;
            *ptr_section_name = reinterpret_cast<char *>(ptr_section->Name);
            break;
        };
        ptr_section++;
    };
    // if the section was not found
    if (i >= nt_header->FileHeader.NumberOfSections) {
        goto FINISH;
    };

    // everything is ok, user pointers are filled
    retval = true;
FINISH:;
    return retval;
}


/*---------------------------------------------------------------------------*/
/** @brief This function is registered as unhandler exception handler
 * --------------------------------------------------------------------------
 **/
static LONG WINAPI __our_unhandled_exception_hander(EXCEPTION_POINTERS * exc_info)
{
    EXCEPTION_RECORD * exc_record = exc_info->ExceptionRecord;
    SYSTEMTIME st;
    // path to module which caused the exception
    char path_to_module_which_faulted[MAX_PATH + 3];
    // section:offset location of the fault
    DWORD section_fault, offset_fault;
    char* section_name;

    // log the exception timestamp and type
    GetLocalTime(&st);
    exception_handler_log("##### UNHANDLED EXCEPTION at %02d.%02d.%04d, %02d:%02d:%02d.%03d #####\n",
        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    exception_handler_log("Exception code: %08X (%s)\n", exc_record->ExceptionCode, exception_handler_get_string(exc_record->ExceptionCode));

    // discover the fault location
    exception_handler_log("Fault address:  %08X -> ", exc_record->ExceptionAddress);
    if (!exception_handler_discover_location(exc_record->ExceptionAddress,
                                            path_to_module_which_faulted,
                                            sizeof(path_to_module_which_faulted) - 1,
                                            &section_fault, &offset_fault, &section_name)) 
    {
        // some error in detection of fault location
        exception_handler_log("location could not be discovered\n");
    } else {
        // fault location detected successfully
        exception_handler_log("section:offset %04X:%08X (%s)\n", section_fault, offset_fault, section_name);
        exception_handler_log("Fault module:   %s\n", path_to_module_which_faulted);
    };

    // Display the context and try to make the stack walk
    CONTEXT * ptr_context = exc_info->ContextRecord;
    exception_handler_log("Fault context:  EAX:%08X EBX:%08X ECX:%08X EDX:%08X ESI:%08X EDI:%08X\n", 
            ptr_context->Eax, ptr_context->Ebx, ptr_context->Ecx, ptr_context->Edx, ptr_context->Esi, ptr_context->Edi);
    exception_handler_log("                ESP:%08X EBP:%08X EIP:%08X flg:%08X CS:%04X DS:%04X SS:%04X ES:%04X FS:%04X GS:%04X\n",
            ptr_context->Esp, ptr_context->Ebp, ptr_context->Eip, ptr_context->EFlags, 
            ptr_context->SegCs, ptr_context->SegDs, ptr_context->SegSs, ptr_context->SegEs, ptr_context->SegFs, ptr_context->SegGs);

    // Display the stack trace information
    exception_handler_log("Call stack dump:\n");

    DWORD pc_address = ptr_context->Eip;
    DWORD* this_frame;
    DWORD* previous_frame;
    uint32_t local_data_len;
    uint8_t * local_data_begin;
    uint8_t* local_data_end;
    uint8_t stack_index;

    // initialize stack walk
    pc_address = ptr_context->Eip;
    this_frame = (DWORD *)ptr_context->Ebp;
    previous_frame = (DWORD*)ptr_context->Esp;
    local_data_begin = (LPBYTE)previous_frame;
    local_data_end = (LPBYTE)(this_frame);
    local_data_len = local_data_end - local_data_begin;

    // walk up to the bottom of the stack
    stack_index = 1;
    for (;;) {

        exception_handler_log("#%d\n", stack_index);

        exception_handler_log("Fault frame:    %08X\n", this_frame);
        exception_handler_log("Fault address:  %08X -> ", pc_address);
        if (!exception_handler_discover_location( (PVOID)pc_address,
            path_to_module_which_faulted,
            sizeof(path_to_module_which_faulted) - 1,
            &section_fault, &offset_fault, &section_name))
        {
            // some error in detection of fault location
            exception_handler_log("location could not be discovered\n");
        } else {
            // fault location detected successfully
            exception_handler_log("section:offset %04X:%08X (%s)\n", section_fault, offset_fault, section_name);
            exception_handler_log("Fault module:   %s\n", path_to_module_which_faulted);
        };

        // Dump local data, if there are any
        exception_handler_log("Local data:     ");
        if (local_data_len == 0) {
            exception_handler_log("NONE\n");
        } else {
            exception_handler_log("%d bytes at %08X", local_data_len, local_data_begin);
            uint32_t display_data_count = local_data_len;
            if (local_data_len > 128) {
                display_data_count = 128;
                exception_handler_log(" (first %d bytes shown)", display_data_count);
            };
            exception_handler_log("\n");
            exception_handler_log("                ");
            // display all bytes
            uint8_t bytes_in_row = 0;
            uint8_t* ptr_data = local_data_begin;
            while (display_data_count) {
                if (IsBadReadPtr(ptr_data, 1)) {
                    exception_handler_log("?? ");
                } else {
                    exception_handler_log("%02X ", *ptr_data);
                };
                if (++bytes_in_row >= 32) {
                    exception_handler_log("\n");
                    // if there will be at least one more byte
                    if (display_data_count > 1) {
                        exception_handler_log("                ");
                    };
                    bytes_in_row = 0;
                };
                ptr_data++;
                display_data_count--;
            };
            // complete the line if necessary
            if (bytes_in_row > 0) {
                exception_handler_log("\n");
            };
        };

        // walk up the stack
        pc_address = this_frame[1];
        previous_frame = this_frame;
        this_frame = (DWORD *)this_frame[0]; 
        if ((DWORD)this_frame & 0x03) {
            // Not aligned frame pointer, break
            break;
        };
        if (this_frame <= previous_frame) {
            // invalid addresses, stop
            break;
        };
        if (IsBadWritePtr(this_frame, sizeof(PVOID) * 2)) {
            // two words must be accessible
            break;
        };

        // Next frame seems to be OK
        local_data_begin = (LPBYTE)previous_frame;
        local_data_begin += 2*sizeof(DWORD);
        local_data_end   = (LPBYTE)this_frame;
        if (local_data_begin < local_data_end) {
            local_data_len = (DWORD)(local_data_end - local_data_begin);
        } else {
            local_data_len = 0;
        };
        // Next stack level
        stack_index++;
    };

    return EXCEPTION_EXECUTE_HANDLER;
}


/*---------------------------------------------------------------------------*/
/** @brief Initializes exception handler layer
 * --------------------------------------------------------------------------
 **/
void exception_handler_init (void)
{
    SetUnhandledExceptionFilter(__our_unhandled_exception_hander);
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
}

