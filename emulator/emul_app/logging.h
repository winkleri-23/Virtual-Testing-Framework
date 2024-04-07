#ifndef __LOGGING_H_INCLUDED
#define __LOGGING_H_INCLUDED


/*---------------------------------------------------------------------------*/
/** @brief Prepares the base path for logging
 * --------------------------------------------------------------------------
 **/
void log_prepare_base_path(char* buffer, uint32_t buffer_len, char** ptr_last);


/*---------------------------------------------------------------------------*/
/** @brief Initializes the logging
 * --------------------------------------------------------------------------
 **/
void log_init(void);


/*---------------------------------------------------------------------------*/
/** @brief Starts actual logging (resets the outputs file)
 * --------------------------------------------------------------------------
 **/
void log_start(void);


/*---------------------------------------------------------------------------*/
/** @brief De-initializes the logging
 * --------------------------------------------------------------------------
 **/
void log_deinit(void);


/*---------------------------------------------------------------------------*/
/** @brief Logging function for normal characters
 * --------------------------------------------------------------------------
 **/
int Log(const char* fmt, ...);


/*---------------------------------------------------------------------------*/
/** @brief Logging function for wide characters
 * --------------------------------------------------------------------------
 **/
int LogW(const wchar_t* fmt, ...);


/*---------------------------------------------------------------------------*/
/** @brief Dumps a memory HEX dump to the LOG output with prefix string
 * --------------------------------------------------------------------------
 */
void log_dump_memory(void* base, uint32_t len);


// Logging macros
#define LOG(flag,...) if (flag) { Log(__VA_ARGS__); }
#define LOGW(flag,...) if (flag) { LogW(__VA_ARGS__); }


#endif // included