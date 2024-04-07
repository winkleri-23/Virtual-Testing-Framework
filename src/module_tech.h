#ifndef __MODULE_TECH_H_INCLUDED
#define __MODULE_TECH_H_INCLUDED
#include <cstdint>

extern uint8_t di;
extern uint8_t qi;


/* --------------------------------------------------------------------------*/
/** @brief TECHNOLOGY INITIALIZATION FUNCTION
 * --------------------------------------------------------------------------
 *
 * - Called just once after device powerup.
 *
 */
void MODULE_INITIALIZATION(void);



/*---------------------------------------------------------------------------*/
/** @brief OPEN function - 2nd phase of the TECHNOLOGY layer initialization
 * --------------------------------------------------------------------------
 *
 * - Call of resources. If the function cannot finished synchronously,
 *   TECH_SET_EVENT_IND() must be called for further work.
 * - Called just once after powerup, after tech_init
 *
 * @return TECH_RESULT_ASYNC, TECH_RESULT_SYNC.
 */
unsigned char OPEN(void);


/*---------------------------------------------------------------------------*/
/** @brief START function - 3rd phase of the TECHNOLOGY layer initialization
 * --------------------------------------------------------------------------
 */
unsigned char START(void);



/*---------------------------------------------------------------------------*/
/** @brief TECH set default module ID
 * --------------------------------------------------------------------------
 */
void SET_DEFAULT_MODULE_ID(void);


/*---------------------------------------------------------------------------*/
/** @brief Set submodule ID. 
 * --------------------------------------------------------------------------
 */
void SET_SUBMODULE_ID(uint32_t submoduleID);


/*---------------------------------------------------------------------------*/
/** @brief TECH checks if parameter structure length is OK
 * --------------------------------------------------------------------------
 */
bool CHECK_LENGTH_PARAMETRIZATION_STRUCTURE(uint16_t num, uint16_t len);


/*---------------------------------------------------------------------------*/
/** @brief TECH sets new configuration according to submodule ID 
 * --------------------------------------------------------------------------
 */
void SET_CONF(unsigned char channel, unsigned char mode);


/*---------------------------------------------------------------------------*/
/** @brief TECH check function for a parameter DS - tech_check_para_ds
 * --------------------------------------------------------------------------
*/
unsigned char CHECK_PARAMETRIZATION_STRUCTURE(uint16_t rec_num, uint16_t len, uint8_t* ds_ptr);


/*---------------------------------------------------------------------------*/
/** @brief This function performs the parametrization using the provided
 *	parametrization structure
 * --------------------------------------------------------------------------
 */
bool SET_PARAMETRIZATION_STRUCTURE(uint16_t rec_num, uint16_t len, uint8_t* tech_dspara_ptr); 


/*---------------------------------------------------------------------------*/
/** @brief Technology performs services function
 * --------------------------------------------------------------------------
 * This technology function has numerous background acitivites based on 
 * the tested module. Those activities are a blackbox from the VT Framework.
 */
void PERFORM_SERVICES(void);

/*---------------------------------------------------------------------------*/
/** @brief Timer backcall function - reads SPI data, triggers next transfer
 * --------------------------------------------------------------------------
 *
 * called only from interrupt context!
 *
 **/
void SPI_TRANSANCTION(void);



void systick_HW();



void systick_MODULE_EXECUTION(void);



void MODULE_EXECUTION(void);

/*---------------------------------------------------------------------------*/
/** @brief This function simulates the logic of an unspecified module
 * --------------------------------------------------------------------------
 *
 * The logic varies between different modules. The testing of this function is conducted using the VT Framework.
 *
 **/
void MODULE_LOGIC();

/*---------------------------------------------------------------------------*/
/** @brief This function set the DI and QI bit according to the module logic.
 *  HELPER FUNCTION.
 * --------------------------------------------------------------------------
 **/
void SET_DI_QI(uint8_t ch, uint8_t val);

void module_report();
#endif
