#pragma once

#include "message_queue.hpp"
#include "log_type.hpp"

namespace Logger {
    
extern MessageQueue msgQueue;

template <typename T>
requires emul_concepts::printable<T>
void log(LogType::type logLvl, T message) {
    msgQueue.push(logLvl, message);
}

extern MessageQueue* getMsgQueue();
}


#define MODULE_LOG false
#define TECH_LOG false
#define PAR_STRUCTURE_DEBUG true


#define LOG_EMUL(flag,...)      \
  do {                     \
    if (flag) {            \
      printf("emul: ");     \
      printf(__VA_ARGS__ );  \
    };                     \
} while(0)

#define LOG_SAMPLE_MODULE_INTERFACE(flag,...)      \
  do {                     \
    if (flag) {            \
      printf("SAMPLE_MODULE INTERFACE --> ");     \
      printf(__VA_ARGS__ );  \
    };                     \
} while(0)

#define LOG_NAMUR_INTERFACE_ARRAYS(flag,...)      \
  do {                     \
    if (flag) {            \
      printf("");     \
      printf(__VA_ARGS__ );  \
    };                     \
} while(0)

#define PRINT_BITS(flag, byte)          \
    if(flag){                           \
        std::cout << "bit representation: ";   \
        for (int i = 7; i >= 0; i--) {  \
            std::cout << ((byte >> i) & 1);  \
        }\
        std::cout << std::endl;               \
    };


#define LOG_SAMPLE_MODULE(flag,...)      \
  do {                     \
    if (flag) {            \
      printf("MODULE --> ");     \
      printf(__VA_ARGS__ );  \
    };                     \
} while(0)



#define PRINT_DS129(flag, data_object) \
    do { \
        if (flag) { \
            std::cout << "Hex representation of Data Rec: "; \
            for (size_t i = 0; i < (data_object).len; ++i) { \
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((data_object).DS129[i]) << " "; \
            } \
            std::cout << std::dec << std::endl; \
        } \
    } while (0)


#define PRINT_DS128(flag, data_object) \
    do { \
        if (flag) { \
            std::cout << "Hex representation of ds128: "; \
            for (size_t i = 0; i < (data_object).len; ++i) { \
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((data_object).ds128[i]) << " "; \
            } \
            std::cout << std::dec << std::endl; \
        } \
    } while (0)