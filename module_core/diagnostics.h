#ifndef DIAG_COM_H
#define DIAG_COM_H
#include <cstdint>
#include <vector>


#define DIAG_MODULE_CHANNEL           255
#if !defined (GLOB_DIAG_CHANNEL_TYPE)
#define GLOB_DIAG_CHANNEL_TYPE        0
#endif

extern uint8_t   SET_DIAGNOSTICS(uint8_t ch, uint8_t in);
// extern std::vector<std::pair<std::pair<uint32_t, uint64_t>, uint8_t>> diag_debug_vector;

#endif
