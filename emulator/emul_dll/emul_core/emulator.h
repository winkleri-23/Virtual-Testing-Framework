#include <vector>
#include <fstream>
#include <filesystem>
#include <queue>
#include <module_tech_config.h>
#include <logger.hpp>


#ifndef __EMULATOR_H_INCLUDED
#define __EMULATOR_H_INCLUDED

#define MAX_NUMBER_OF_CHANNEL 32
#define EA_EPI_BUFFER_SIZE 288
#define BASY_PRAL_QUEUE_CNT   8

typedef uint64_t process_data_t;
extern uint32_t submodule_ID;
extern uint64_t EndOfIteration;
extern uint32_t subm_index;
extern uint8_t emul_output_data_buf[EA_EPI_BUFFER_SIZE]; // EA_EPI_BUFFER_SIZE
extern uint8_t diagnostics_buf[MAX_NUMBER_OF_CHANNEL];


extern uint8_t emul_input_data_buf[INPUT_OUTPUT_BUFFER_SIZE];

#ifndef UC_ISR_DECL_HEADER
#define UC_ISR_DECL_HEADER
#define UC_ISR_DECL_TRAILER
#define UC_ISR_DEFN_HEADER
#define UC_ISR_DEFN_TRAILER
#endif

#define LOG_INFO false
#define LOG_SIGNAL false
#define DLL_INFO false
#define EMUL_INFO false
#define INTERFACE_INFO true
#define LOG_SUBM_DESCS false
#define DP_LOG true


#define PATH ""
#define USE_VECTOR
//#define USE_STATIC_ARRAY







extern std::vector<uint32_t> Input;
extern uint64_t CurrentTime;
extern uint64_t BreakPoint;
extern unsigned char rec_num;
extern unsigned char len;

static size_t filesize;

extern int TimerDebugID;

static uint64_t SignalCnt = 0;
/* DIPLOMA THESIS DATA OBJECTS */
// DS128/129 legal

/* DIPLOMA THESIS DATA OBJECTS */
class DS128_Data_object {
public:
    DS128_Data_object() {
        len = 50;
        rec_num = 0;
        for (size_t i = 0; i < len; i++) {
            ds128[i] = 0;
        }
    }

    DS128_Data_object(uint16_t rec_num, uint8_t * received_ds128, size_t len) {
        this->rec_num = rec_num;
        this->len = len;
        memcpy(ds128, received_ds128, len);
    }

    uint8_t ds128[50] = {};
    uint16_t rec_num;
    size_t len;
};

class DS129_data_object {
public:
    DS129_data_object() {
        len = 0;
        DS129 = NULL;
    }

    DS129_data_object(uint8_t* ds ,size_t len) {
        this->len = len;
        DS129 = new uint8_t[len];
        memcpy(DS129, ds, len);
    }

    DS129_data_object& operator=(const DS129_data_object& other) {
        if (this != &other) {
            if (DS129 != NULL) {
                delete[] DS129;
            }
            len = other.len;
            DS129 = new uint8_t[len];
            memcpy(DS129, other.DS129, len);
        }
        return *this;
    }


    ~DS129_data_object() {
        delete[] DS129;
    }

    uint8_t* DS129;
    size_t len;
};

static DS128_Data_object Current_DS128;
static DS129_data_object Curr_data_record_obj;


typedef void (UC_ISR_DECL_HEADER* uc_isr_ptr_t)(void) UC_ISR_DECL_TRAILER;

void emul_register_systick_event(uint64_t reload, uint64_t reload_us, uc_isr_ptr_t isr, int timer_id);

int main_emul(int argc, char** argv);

void get_current_data_record_ds128(DS128_Data_object& cur);

void get_current_data_record_ds129(DS129_data_object& cur);


/*---------------------------------------------------------------------------*/
/** @brief Execute tech_init function
 * --------------------------------------------------------------------------
 **/
void emul_execute_tech_init();


/*---------------------------------------------------------------------------*/
/** @brief Execute tech_open function
 * --------------------------------------------------------------------------
 **/

void emul_execute_tech_open();


/*---------------------------------------------------------------------------*/
/** @brief Execute tech_start function
 * --------------------------------------------------------------------------
 **/
void emul_execute_tech_start();




/*---------------------------------------------------------------------------*/
/** @brief Periodicaly schedule tech_perform_services event
 * --------------------------------------------------------------------------
 **/
void emul_execute_tech_perform_services();



/*---------------------------------------------------------------------------*/
/** @brief Returns diag output for a given channel
 * --------------------------------------------------------------------------
 **/
uint8_t* emul_get_diagnostics(uint8_t channel);


void emul_get_para();

/*---------------------------------------------------------------------------*/
/** @brief Start emulator execution for given number of ms
 * --------------------------------------------------------------------------
 **/
uint64_t emul_execute_emulation(uint64_t ms);

/*---------------------------------------------------------------------------*/
/** @brief Set the given value as an input signal
 * --------------------------------------------------------------------------
 **/
void emul_set_input_signal(uint32_t value);

/*---------------------------------------------------------------------------*/
/** @brief Returns a pointer to output data buffer
 * --------------------------------------------------------------------------
 **/
uint8_t * emul_get_output_data();

void emul_set_output_data(uint8_t * rec_out_data, size_t len);

/*---------------------------------------------------------------------------*/
/** @brief Function is used to notify emualtor once EA updated input data
 * --------------------------------------------------------------------------
 **/
void emul_set_input_data(const uint8_t* input_data, size_t len);


/*---------------------------------------------------------------------------*/
/** @brief Return current input data set in emulator
 * --------------------------------------------------------------------------
 **/
uint8_t* emul_get_input_data();


/*DIPLOMA THESIS FUNCTIONS */
/*---------------------------------------------------------------------------*/
/** @brief Performs mandatory events for module initialization
    - INIT
    - OPEN
    - START
 * --------------------------------------------------------------------------
 **/
void start_and_initialize();


/*---------------------------------------------------------------------------*/
/** @brief Enable power supply 8V (bit 6)
 * --------------------------------------------------------------------------
 **/
void emul_enable_power_suply();

/*---------------------------------------------------------------------------*/
/** @brief Execute tech_set_default_mid function
 * --------------------------------------------------------------------------
 **/
void emul_execute_set_default_mid();


/*---------------------------------------------------------------------------*/
/** @brief Execute tech_set_conf function
 * --------------------------------------------------------------------------
 **/
void emul_execute_set_conf(unsigned char channel, unsigned char mode);

/*---------------------------------------------------------------------------*/
/** @brief Execute technology functions that parametrize the module according
 * to the parametrization structure
 * --------------------------------------------------------------------------
 **/
unsigned char emul_execute_tech_set_para_ds(uint16_t rec_num, uint16_t len, uint8_t* ds);

/*---------------------------------------------------------------------------*/
/** @brief Execute technology functions that checks the lenght 
 *   of the parametrization structure
 * --------------------------------------------------------------------------
 **/
bool emul_execute_tech_check_ds_len(uint16_t rec_num, uint16_t len);

/*---------------------------------------------------------------------------*/
/** @brief Execute technology function that checks the requirements for 
 * a correct parametrization structure
 * --------------------------------------------------------------------------
 **/
bool emul_execute_tech_check_para_ds(uint8_t* ds, uint16_t record_number, uint32_t record_len);


/*---------------------------------------------------------------------------*/
/** @brief Periodically schedule the perform service event
 * --------------------------------------------------------------------------
 **/
void emul_execute_service_function();


/*---------------------------------------------------------------------------*/
/** @brief Helper - only set value for Emulator - tech set conf must be called
 * for passing the value into user_code
 * --------------------------------------------------------------------------
 **/
void emul_set_submoduleID(uint32_t ID);



/*DIPLOMA THESIS FUNCTIONS */



#endif // __EMULATOR_H_INCLUDED
