#include <cstdint>
#include <iostream>
#ifndef __VECTOR_H_INCLUDED
#define __VECTOR_H_INCLUDED

#ifndef UC_ISR_DECL_HEADER
#define UC_ISR_DECL_HEADER
#define UC_ISR_DECL_TRAILER
#define UC_ISR_DEFN_HEADER
#define UC_ISR_DEFN_TRAILER
#endif

typedef void (UC_ISR_DECL_HEADER* uc_isr_ptr_t)(void) UC_ISR_DECL_TRAILER;

class Vector_Node
{
private:
	uint64_t key;
	int TypeOfEvent;
	int NodeID;

public:
	void setKey(uint64_t timeStamp) {this->key = timeStamp;}
	void setTypeOfEvent(int eventType) { this->TypeOfEvent = eventType; }
	void setNodeID(int ID) { this->NodeID = ID;}


	const uint64_t getKey(){ return this->key;}
	const int getTypeOfEvent() { return TypeOfEvent;}
	const int getNodeID() { return NodeID; }
	void setVariables(uint64_t key, int TypeOfEvent, int nodeID);
};

class systick_event_priv_vector : public Vector_Node {
private:
	uint64_t Reload;
	uint64_t Reload_ns;
	void(*systick)(void);
	int timer_id;

public:
	const uint64_t getReload() { return Reload; }
	const uint64_t getReload_ns() { return Reload_ns; }
	const uc_isr_ptr_t getSystickFunction() { return systick; }
	const void callSystickFunction() { this->systick();}
	const int get_timer_id() { return timer_id; };
	void setVariablesSystick(uint64_t reload, uint64_t reload_ns, uc_isr_ptr_t isr, int timer_id);
};



#endif
