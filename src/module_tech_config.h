#ifndef __MODULE_TECH__CONFIG_H_INCLUDED
#define __MODULE_TECH__CONFIG_H_INCLUDED


#define TECHNOLOGY_MODULE_ID_SAMPLE_MODULE						  1



/* CONFIGURATION MACROS */
#define MODULE_DS128_LEN										  50
#define MODULE_DS129_LEN										  154
#define DS128_NUM												  128
#define DS129_NUM												  129
/* CONFIGURATION MACROS */

#define SYSTICK_RELOAD										      1250

/* CORE CONFIGURATION MACROS */
#define INPUT_OUTPUT_BUFFER_SIZE 288

#define IN_OFFSET				 0
#define IN_LEN					 1
#define QI_OFFSET				 1
#define QI_LEN					 1
/* CORE CONFIGURATION MACROS */

/* DIAGNOSTICS MACROS */
#define DIAG_NO_ERROR 0x00
#define WB 0x40
#define SC 0x02
#define NO_SUPPLY 0x20


#define WB_ACTIVATED false
/* DIAGNOSTICS MACROS */

/* SAMPLE MODULE CFG */
#define STRUCTURE_OK		true
#define PARAMETRIZATION_OK	true
/* SAMPLE MODULE CFG */
#endif
