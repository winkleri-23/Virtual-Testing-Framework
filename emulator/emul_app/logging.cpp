#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

// Default log file name
#define LOG_FILE_NAME  "emul_app.log"

// Buffer for log-filename
#define MAX_LOG_PATH_SIZE  500
static char log_filename[MAX_LOG_PATH_SIZE];

/*---------------------------------------------------------------------------*/
/** @brief Prepares the base path for logging
 * --------------------------------------------------------------------------
 **/
void log_prepare_base_path(char * buffer, uint32_t buffer_len, char** ptr_last)
{
    // Buffer for module name
    #define MAX_MODULE_NAME_SIZE  500
    static TCHAR module_path[MAX_MODULE_NAME_SIZE];
    // prepare the whole path for logfile
    GetModuleFileName(NULL, module_path, MAX_MODULE_NAME_SIZE - 3);
    WideCharToMultiByte(CP_ACP, 0, module_path, -1, buffer, buffer_len-1, NULL, NULL);
    // find the last backslash or slash
    char* p = buffer;
    char* last = NULL;
    while (*p) {
        if (*p == '\\' || *p == '/') {
            last = p;
        };
        p++;
    };
    if (last == NULL) {
        // no slash/backslash in the path, replace the path with empty one
        buffer[0] = 0;
        last = buffer;
    } else {
        // we found at least one slash, trim the string after it
        last++;
        *last = 0;
    };
    // not we have the path in log_filename without last slash
    if (ptr_last != NULL) {
        *ptr_last = last;
    };
}


/*---------------------------------------------------------------------------*/
/** @brief Initializes the logging 
 * --------------------------------------------------------------------------
 **/
void log_init(void)
{
    char* last;
    log_prepare_base_path(log_filename, MAX_LOG_PATH_SIZE, &last);
    // add the filename
    char * p = const_cast<char*>(LOG_FILE_NAME);
    while (*p) {
        if (last >= log_filename + MAX_LOG_PATH_SIZE - 3) {
            // does not fit, use log_path without any path
            sprintf_s(log_filename, MAX_LOG_PATH_SIZE - 3, "%s", LOG_FILE_NAME);
            break;
        };
        *last++ = *p++;
    };
    // finish the string
    *last++ = 0;
    // now we have prepared the log path
}


/*---------------------------------------------------------------------------*/
/** @brief Starts actual logging (resets the outputs file)
 * --------------------------------------------------------------------------
 **/
void log_start(void)
{
    FILE* f;
    f = NULL;
    fopen_s(&f, log_filename, "wt");
    if (f != NULL) {
        fclose(f);
    };
}


/*---------------------------------------------------------------------------*/
/** @brief De-initializes the logging
 * --------------------------------------------------------------------------
 **/
void log_deinit(void)
{
    // nothing to do really
}


/*---------------------------------------------------------------------------*/
/** @brief Logging function for normal characters
 * --------------------------------------------------------------------------
 **/
int Log(const char* fmt, ...)
{
    static char linebuf[1100];
    va_list arg;
    int size;
    FILE* f;

    va_start(arg, fmt);
    size = vsnprintf(linebuf, sizeof(linebuf) - 3, fmt, arg);
    va_end(arg);
    printf("%s", linebuf);
    f = NULL;
    fopen_s(&f, log_filename, "at");
    if (f != NULL) {
        fprintf(f, "%s", linebuf);
        fclose(f);
    };
    return size;
}


/*---------------------------------------------------------------------------*/
/** @brief Logging function for wide characters
 * --------------------------------------------------------------------------
 **/
int LogW(const wchar_t* fmt, ...)
{
    static wchar_t linebuf[1100];
    static char line[1100];
    va_list arg;
    int size;
    FILE* f;

    va_start(arg, fmt);
    size = wvsprintf(linebuf, fmt, arg);
    va_end(arg);
    size = wprintf(L"%s", linebuf);
    WideCharToMultiByte(CP_ACP, 0, linebuf, -1, line, sizeof(line), NULL, NULL);
    f = NULL;
    fopen_s(&f, log_filename, "at");
    if (f != NULL) {
        fprintf(f, "%s", line);
        fclose(f);
    };
    return size;
}


/*---------------------------------------------------------------------------*/
/** @brief Dumps a memory HEX dump to the LOG output with prefix string
 * --------------------------------------------------------------------------
 */
void log_dump_memory(void* base, uint32_t len)
{
    uint32_t MemLine;
    uint8_t MemByte;
    uint8_t b;
    uint8_t* linebase;
    // Dumps memory
    linebase = reinterpret_cast<uint8_t*>(base);
    for (MemLine = 0; MemLine < len / 16 + 1; MemLine++) {
        // Address
        Log("%04x:%08p: ", linebase- reinterpret_cast<uint8_t*>(base), linebase);
        // Bytes 0-8 from address
        for (MemByte = 0; MemByte < 8; MemByte++) {
            if (static_cast<uint32_t>(MemLine * 16 + MemByte) >= len) {
                Log("   ");
            } else {
                Log("%02X ", linebase[MemByte]);
            };
        };
        Log(const_cast<char*>("| "));
        for (MemByte = 8; MemByte < 16; MemByte++) {
            if (static_cast<uint32_t>(MemLine * 16 + MemByte) >= len) {
                Log("   ");
            } else {
                Log("%02X ", linebase[MemByte]);
            };
        };
        Log(const_cast<char*>("  "));
        for (MemByte = 0; MemByte < 16; MemByte++) {
            if (static_cast<uint32_t>(MemLine * 16 + MemByte) >= len) {
                Log(" ");
            } else {
                b = linebase[MemByte];
                if (!(b >= 32 && b < 127)) {
                    b = '.';
                };
                Log("%c ", b);
            };
        };
        Log("\n");
        // Add one line to base
        linebase += 16;
        if (static_cast<uint32_t>(linebase - reinterpret_cast<uint8_t*>(base)) >= len) {
            break;
        };
    };
}
