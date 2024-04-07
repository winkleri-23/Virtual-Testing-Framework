import pywin32

#============
# Constants
#============

# Maximum number of nanoseconds that can be executed in one command
EMUL_TECH_MAX_EXEC_ns   = 2000000000        # 2G
# Maximum number of microseconds that can be executed in one command
EMUL_TECH_MAX_EXEC_us   = 2000000           # 2M
# Maximum number of milliseconds that can be executed in one command
EMUL_TECH_MAX_EXEC_ms   = 2000              # 2k


#===========================================================
# Executes the emulated module for the specified time [ns]
#===========================================================
def emul_ll_execute_ns(ns):
    """! Executes emulation for specified interval 

    @param ns       number of nanoseconds of model time
    @return  none

    during the execution will be executed:

    1. tech_perform_services according to planned interval
    2. interrupts registered and executed by BASY
    """
    while ns>EMUL_TECH_MAX_EXEC_ns:
        emul_ll_execute_ns(EMUL_TECH_MAX_EXEC_ns)
        ns = ns-EMUL_TECH_MAX_EXEC_ns
    # really execute
    return

#===========================================================
# Executes the emulated module for the specified time [us]
#===========================================================
def emul_ll_execute_us(us):
    """! Executes emulation for specified interval
    @param ms       number of microseconds of model time
    @return  none
    """
    while us>EMUL_TECH_MAX_EXEC_us:
        emul_ll_execute_us(EMUL_TECH_MAX_EXEC_us)
        us = us-EMUL_TECH_MAX_EXEC_us
    # really execute
    emul_ll_execute_ns(us*1000)
    return


#===========================================================
# Executes the emulated module for the specified time [ms]
#===========================================================
def emul_ll_execute_ms(ms):
    """! Executes emulation for specified interval
    @param ms       number of milliseconds of model time
    @return  none
    """
    while ms>EMUL_TECH_MAX_EXEC_ms:
        emul_ll_execute_ms(EMUL_TECH_MAX_EXEC_ms)
        ms = ms-EMUL_TECH_MAX_EXEC_ms
    # really execute
    emul_ll_execute_us(ms*1000)
    return


#============================================
# Executes (tech_init) function in emulator
#============================================
def emul_ll_execute_tech_init():
    pass

# Opens the emulated module's technology layer (tech_open)
def emul_ll_execute_tech_open():
    pass

# Starts the emulated module's technology layer (tech_start)
def emul_ll_execute_tech_start():
    pass

# Sets up parameters for periodic executing (tech_perform_services)
def emul_ll_setup_perform_services():
    pass

