#include <vector_solution.h>


void Vector_Node::setVariables(uint64_t key, int TypeOfEvent, int nodeID)
{
	this->key = key;
	this->TypeOfEvent = TypeOfEvent;
	this->NodeID = nodeID;
}


void systick_event_priv_vector::setVariablesSystick(uint64_t reload, uint64_t reload_ns, uc_isr_ptr_t isr, int timer_id) {
	this->Reload = reload;
	this->Reload_ns = reload_ns;
	this->systick = isr;
	this->timer_id = timer_id;
}
