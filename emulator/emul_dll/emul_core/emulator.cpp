#include <chrono>
#include <list>
#include <cmath>
#include <set>
#include <functional>
#include <filesystem>

/*Emulator includes*/
#include <emulator.h>
#include <vector_solution.h>
#include <SPI_simulator.h>
/*Emulator includes*/

#include <stack>
#include "logger.hpp"
#include <module_tech.h>


/*Defines - Events*/
#define TECH_PERFORM_SERVICES 9
#define SYSTICK_FUNCTION 10
/*Defines - Events*/

#define TECH_PERFORM_SERVICE_RELOAD 156250
#define MAX_NUMBER_OF_EVENTS 256

using namespace std;

bool POWER_SUPPLY_ENABLED = false;

Vector_Node* Events_Array[MAX_NUMBER_OF_EVENTS];
static int Event_index = 0;

uint8_t* user_ds128 = nullptr;

uint64_t CurrentTime;
uint64_t timeStamp;
uint64_t BreakPoint;
uint64_t EndOfIteration;


int nodeID;
uint32_t submodule_ID;

uint32_t subm_index;

unsigned char rec_num;
unsigned char len;

uint8_t emul_output_data_buf[EA_EPI_BUFFER_SIZE];



void emul_register_systick_event(uint64_t reload, uint64_t reload_ns, uc_isr_ptr_t isr, int timer_id) {
	systick_event_priv_vector* newEv = new systick_event_priv_vector;
	newEv->setVariables(CurrentTime + reload_ns, SYSTICK_FUNCTION, nodeID);
	newEv->setVariablesSystick(reload, reload_ns, isr, (timer_id));
	Events_Array[Event_index++] = newEv;
}

void emul_reload_systick_event(int min_index) {
	Events_Array[min_index]->setKey(CurrentTime + ((systick_event_priv_vector*)(Events_Array[min_index]))->getReload_ns());
}

void emul_reload_tech_perform_services_event(int min_index) {
	Events_Array[min_index]->setKey(CurrentTime + TECH_PERFORM_SERVICE_RELOAD);
}



void emul_set_output_data(uint8_t* rec_out_data, size_t len)
{
	memcpy(emul_output_data_buf, rec_out_data, std::min(len, sizeof emul_input_data_buf));
}

uint8_t* emul_get_output_data()
{
	return &emul_output_data_buf[0];
}


void executeEvent(int min_index) {

	if (Events_Array[min_index]->getTypeOfEvent() == SYSTICK_FUNCTION) {
		systick_event_priv_vector* newSystick_event = (systick_event_priv_vector*)(Events_Array[min_index]);
		newSystick_event->callSystickFunction();
		emul_reload_systick_event(min_index);
	}
	else if (Events_Array[min_index]->getTypeOfEvent() == TECH_PERFORM_SERVICES) {
		PERFORM_SERVICES();
		emul_reload_tech_perform_services_event(min_index);
	}
}

bool Process_events() {
	Logger::log(LogType::Info, std::format("{} : [Executing Events]", __func__));
	while (true) {
		int min_index = 0;

		for (int i = 0; i < Event_index; i++) {
			if (Events_Array[i]->getKey() < Events_Array[0]->getKey()) {
				min_index = i;
			}
		}

		if (Events_Array[min_index]->getKey() <= EndOfIteration) {
			CurrentTime = Events_Array[min_index]->getKey();
			executeEvent(min_index);
		}
		else {
			return false;
		}
	}
}


uint64_t ConvertorToNs(uint64_t originalTime, string unit) {
	if (unit == "s") {
		originalTime *= 1000000000;
	}
	else if (unit == "ms") {
		originalTime *= 1000000;
	}
	else if (unit == "us") {
		originalTime *= 1000;
	}
	return originalTime;
}


uint64_t ConvertorToMs(uint64_t originalTime, string unit) {
	if (unit == "s") {
		originalTime *= 1000;
	}
	else if (unit == "ns") {
		originalTime /= 1000000;
	}
	else if (unit == "us") {
		originalTime /= 1000;
	}
	return originalTime;
}

// Fuctions for module initialization and parametrization
void emul_execute_tech_init() { MODULE_INITIALIZATION(); }

void emul_execute_tech_open() { OPEN(); }

void emul_execute_tech_start() { START(); }


void emul_execute_set_conf(unsigned char channel, unsigned char mode) { SET_CONF(channel, mode); }


void emul_execute_set_default_mid() {
	SET_DEFAULT_MODULE_ID();
}


void emul_enable_power_suply() { POWER_SUPPLY_ENABLED = true; }


bool emul_execute_tech_check_ds_len(uint16_t rec_num, uint16_t len) {
	return CHECK_LENGTH_PARAMETRIZATION_STRUCTURE(rec_num, len);
}

unsigned char emul_execute_tech_set_para_ds(uint16_t rec_num, uint16_t len, uint8_t* ds) {

	unsigned char retval = SET_PARAMETRIZATION_STRUCTURE(rec_num, len, ds);

	// Save the approved data record for later retrieval by the client.

	if (rec_num == 128) {
		DS128_Data_object tmp(rec_num, ds, len);
		Current_DS128 = tmp;
	}
	else if (rec_num == 129) {
		DS129_data_object curr(ds, len);
		Curr_data_record_obj = curr;
	}

	return retval;
}


void get_current_data_record_ds128(DS128_Data_object& cur) {
	cur = Current_DS128;
	PRINT_DS128(PAR_STRUCTURE_DEBUG ,cur);
	return;
}

void get_current_data_record_ds129(DS129_data_object& cur) {
	cur = Curr_data_record_obj;
	PRINT_DS129(PAR_STRUCTURE_DEBUG, cur);
	return;
}


bool emul_execute_tech_check_para_ds(uint8_t* ds, uint16_t record_number, uint32_t record_len) {
	return CHECK_PARAMETRIZATION_STRUCTURE(record_number, record_len, ds);
}

// Fuctions for module initialization and parametrization

void emul_set_submoduleID(uint32_t ID) {
	SET_SUBMODULE_ID(ID);
	submodule_ID = ID;
}

void emul_execute_tech_perform_services() {
	if (Event_index < 3) {
		Vector_Node* newEv = new Vector_Node;
		newEv->setVariables(109377, TECH_PERFORM_SERVICES, 1);
		Events_Array[Event_index++] = newEv;
	}
}

void emul_execute_service_function() {
	if (Event_index < 3) {
		Vector_Node* newEv = new Vector_Node;
		newEv->setVariables(109377, TECH_PERFORM_SERVICES, 1);
		Events_Array[Event_index++] = newEv;
	}
}

void emul_set_input_data(const uint8_t* input_data, size_t len) {
	memcpy(emul_input_data_buf, input_data, std::min(len, sizeof emul_input_data_buf));
}



uint8_t* emul_get_input_data() {
	return &emul_input_data_buf[0];
}


uint8_t* emul_get_diagnostics(uint8_t channel) {
	return &diagnostics_buf[channel];
}



uint64_t emul_execute_emulation(uint64_t ms) {
	uint64_t ret = ms;
	BreakPoint += ms;
	new_emulation_process = true;
	EndOfIteration = CurrentTime + BreakPoint;
	Process_events();

	for (int i = 0; i < Event_index; i++) {
		if (Events_Array[i]->getKey() >= EndOfIteration) {
			ret = Events_Array[i]->getKey() - EndOfIteration;
			Events_Array[i]->setKey(Events_Array[i]->getKey() - (EndOfIteration));
		}
	}

	CurrentTime = 0;
	BreakPoint = 0;
	module_report();
	return ret;
}


void emul_set_input_signal(uint32_t value) {
	Current_spi_signal.sample_value = value;
	Current_spi_signal.samples_count = 0;
}


void start_and_initialize() {
	Logger::log(LogType::Debug, "[Start and initialize]");
	MODULE_INITIALIZATION();
	OPEN();
	START();
	emul_enable_power_suply();
}


