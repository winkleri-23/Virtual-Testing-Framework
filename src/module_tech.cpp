#include "module_tech.h"
#include <iostream>
#include <module_data.h>
#include <logger.hpp>
#include "chip_control.h"
#include <SPI_simulator.h>
#include <diagnostics.h>
#include <input_output_control.h>


module_data tested_module;
typedef enum {
	SET,
	CLEAR,
	TOGGLE
} BitOperation;

/* --------------------------------------------------------------------------*/
/** @brief Helper function that allows me to modify specific bits in the output
 * signal
 * --------------------------------------------------------------------------
 */

void modifyBit(uint8_t* variable, int bitPosition, BitOperation operation) {
	switch (operation) {
	case SET:
		*variable |= (1 << bitPosition);
		break;
	case CLEAR:
		*variable &= ~(1 << bitPosition);
		break;
	case TOGGLE:
		*variable ^= (1 << bitPosition);
		break;
	default:
		// Handle invalid operation
		break;
	}
}


uint8_t di;
uint8_t qi;


void MODULE_INITIALIZATION(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	timer_init(SYSTICK_RELOAD, 1);
	timer_init(SYSTICK_RELOAD, 1);
	tested_module.module_init();
	tested_module.report_module_data();
	return;
}

unsigned char OPEN(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	Logger::log(LogType::Debug, "[MODULE_INITIALIZATION]");
	tested_module.module_open();
	tested_module.report_module_data();
	return 0;
}

unsigned char START(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.module_start();
	tested_module.report_module_data();
	return 0;
}

void SET_DEFAULT_MODULE_ID(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.set_default_module_ID();
	tested_module.report_module_data();
}

void SET_SUBMODULE_ID(uint32_t submoduleID) {
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.set_submodule_ID(submoduleID);
	tested_module.report_module_data();
}

bool CHECK_LENGTH_PARAMETRIZATION_STRUCTURE(uint16_t num, uint16_t len)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	std::cout << "LEN: " << len << std::endl;
	switch (num) {
		case DS128_NUM:
			return (len == MODULE_DS128_LEN);
		case DS129_NUM:
			return (len == MODULE_DS129_LEN);
		default:
			return false;
	}
}


unsigned char CHECK_PARAMETRIZATION_STRUCTURE(uint16_t rec_num, uint16_t len, uint8_t* ds_ptr) {
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.report_module_data();
	return STRUCTURE_OK;
}

void PERFORM_SERVICES(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.perform_services();
	
}

bool SET_PARAMETRIZATION_STRUCTURE(uint16_t rec_num, uint16_t len, uint8_t* structure_ptr)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	bool ret = tested_module.parametrize_module(rec_num, len, structure_ptr);
	tested_module.report_module_data();
	return ret;
}

void SET_CONF(unsigned char channel, unsigned char mode)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	tested_module.configure_module(channel, mode);
	tested_module.report_module_data();
}




void module_report() {
	tested_module.report_module_data();
}



void SPI_TRANSANCTION(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	spi_receive_data();
}

void systick_HW(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	SPI_TRANSANCTION();
}


void systick_MODULE_EXECUTION(void)
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	MODULE_EXECUTION();
}

void SET_DI_QI(uint8_t ch, uint8_t val) {
	if (val == 1) {
		modifyBit(&di, ch, CLEAR);
		modifyBit(&qi, ch, SET);
	}else if(val == 3){
		modifyBit(&di, ch, SET);
		modifyBit(&qi, ch, SET);
	}
	else {
		modifyBit(&di, ch, CLEAR);
		modifyBit(&qi, ch, CLEAR);
	}
}

void MODULE_LOGIC()
{
	LOG_SAMPLE_MODULE(TECH_LOG, "%s\n", __func__);
	uint32_t SPI_input = spi_receive_data(); 
	for (int i = 0; i < 8; i++) {
		uint8_t val = (SPI_input >> (3 * (7 - i))) & 0x7;
		SET_DI_QI(i, val);
		SET_DIAGNOSTICS(i, val);
	}
}

void MODULE_EXECUTION(void) {
	
	// module logic IMPLEMENT SAMPLE LOGIC
	MODULE_LOGIC();
	uint8_t* in_pt = &di;
	uint8_t* qi_pt = &qi;

	DP_set_all_inputs(in_pt, qi_pt);

}
