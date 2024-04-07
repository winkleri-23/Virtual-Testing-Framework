#ifndef CHIP_CONTROL_H_INCLUDED
#define CHIP_CONTROL_H_INCLUDED

#include <cstdint>
#include <stdio.h>
#include <iostream>

#include <emulator.h>
#include <module_tech.h>

typedef void (*systick_func)(void);

template<uint32_t QUARTZ_HZ = 40000000ul>
struct uc_timeconf_c
{
    static uint_fast32_t  get_quartz_hz(void) { return QUARTZ_HZ; }
};


static int timer_id = 0;

struct timer_struct {
    int id;
    uint64_t systick_reload_;
    uint64_t reload_us;
    uint8_t prio;
    systick_func isr;
};

static const uint8_t CASCADE_ISR_COUNT = 5;

static timer_struct timer_struct_array[CASCADE_ISR_COUNT];

void systickfunc() {
    printf("%s\n", __func__);
}

void timer_init(uint32_t systick_reload_, uint8_t prio)
{
    uint32_t tmp = uc_timeconf_c<>::get_quartz_hz() / systick_reload_;
    uint64_t reload_ns = (1000000000) / (tmp);;
    timer_struct_array[timer_id].id = timer_id;
    timer_struct_array[timer_id].systick_reload_ = systick_reload_;
    timer_struct_array[timer_id].reload_us = reload_ns;
    timer_struct_array[timer_id].prio = prio;

    switch (timer_id) {
    case 0:
        timer_struct_array[timer_id].isr = systick_HW;
        break;
    case 1:
        timer_struct_array[timer_id].isr = systick_MODULE_EXECUTION;
        break;
    case 2:
        timer_struct_array[timer_id].isr = systick_HW;
        break;
    case 3:
        timer_struct_array[timer_id].isr = systick_MODULE_EXECUTION;
        break;
    }
    emul_register_systick_event(systick_reload_, reload_ns, timer_struct_array[timer_id].isr, (timer_id));
    timer_id++;
}



#endif CHIP_CONTROL_H_INCLUDED

