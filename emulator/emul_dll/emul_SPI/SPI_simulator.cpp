#include <stdint.h>
#include <SPI_simulator.h>


TECH_ACQUIRE_MOCK_SAMPLE_T Current_spi_signal;
bool firstRun = true;
bool new_emulation_process = true;



uint32_t spi_receive_data() {
    uint32_t data_sample = 0;
    data_sample = Current_spi_signal.sample_value;
    return data_sample;
}

