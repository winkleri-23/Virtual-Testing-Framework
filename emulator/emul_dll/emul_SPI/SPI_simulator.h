#include <cstdint>

#include <vector>

// positions of channel 3-bits in SPI input word
// every channel occupies just three bits
#define TECH_HW_SPI_BITPOS_CH0        (7*3)     // bits 21-23
#define TECH_HW_SPI_BITPOS_CH1        (6*3)     // bits 18-20
#define TECH_HW_SPI_BITPOS_CH2        (5*3)     // bits 15-17
#define TECH_HW_SPI_BITPOS_CH3        (4*3)     // bits 12-14
#define TECH_HW_SPI_BITPOS_CH4        (3*3)     // bits  9-11
#define TECH_HW_SPI_BITPOS_CH5        (2*3)     // bits  6- 8
#define TECH_HW_SPI_BITPOS_CH6        (1*3)     // bits  3- 5
#define TECH_HW_SPI_BITPOS_CH7        (0*3)     // bits  0- 2

#ifndef __SPI_SIMULATOR_H_INCLUDED
#define __SPI_SIMULATOR_H_INCLUDED

#define ACQ_MOCK_SAMPLE_VALUE(a,b,c,d,e,f,g,h) \
    ((h)<<TECH_HW_SPI_BITPOS_CH7) | ((g)<<TECH_HW_SPI_BITPOS_CH6) | \
    ((f)<<TECH_HW_SPI_BITPOS_CH5) | ((e)<<TECH_HW_SPI_BITPOS_CH4) | \
    ((d)<<TECH_HW_SPI_BITPOS_CH3) | ((c)<<TECH_HW_SPI_BITPOS_CH2) | \
    ((b)<<TECH_HW_SPI_BITPOS_CH1) | ((a)<<TECH_HW_SPI_BITPOS_CH0)

#define ACQ_MOCK_SAMPLE(a,b,c,d,e,f,g,h,num_samples)   \
    { .samples_count=num_samples, .sample_value=ACQ_MOCK_SAMPLE_VALUE(a,b,c,d,e,f,g,h) }

#define ACQ_CH0_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(a,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH1_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, a,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH2_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0, a,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH3_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0,TECH_CHS_0, a,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH4_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,a,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH5_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,a,TECH_CHS_0,TECH_CHS_0,num_samples)

#define ACQ_CH6_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,a,TECH_CHS_0,num_samples)

#define ACQ_CH7_MOCK1(a,num_samples) \
    ACQ_MOCK_SAMPLE(TECH_CHS_0, TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,TECH_CHS_0,a,num_samples)


#define TECH_CHS_0_SPI_MOCK 1

#define ACQ_MOCK_MS(ms)     ((uint32_t)((uint32_t)(ms)*TECH_NAMUR_SAMPLING_FREQUENCY/1000L))
#define ACQ_MOCK_NS(ns)     ((uint64_t)((uint64_t)(ns)*TECH_NAMUR_SAMPLING_FREQUENCY/1000000000L))
#define ACQUIRE_MOCK_DECIMATION_FACTOR      2


class TECH_ACQUIRE_MOCK_SAMPLE_T {
public:
    
    TECH_ACQUIRE_MOCK_SAMPLE_T() {
        sample_value = 0xffff;
        samples_count = 0;
    }
    // number of samples to consume
    uint32_t samples_count;
    // sample value
    uint32_t sample_value;
};

uint32_t spi_receive_data();

extern TECH_ACQUIRE_MOCK_SAMPLE_T Current_spi_signal;
extern std::vector<TECH_ACQUIRE_MOCK_SAMPLE_T>::iterator it;
extern bool firstRun;
extern bool new_emulation_process;
extern uint32_t tech_acquire_mock_sample_count;
extern uint32_t CurrentSampleValue;
extern uint64_t cnt;
extern int RepeatSignal;
#endif
