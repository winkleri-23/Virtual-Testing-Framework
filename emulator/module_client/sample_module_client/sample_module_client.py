from ast import Str
import ctypes as ct
from enum import IntEnum
from emul import Emul
import emul_llc as llc

NUMBER_OF_CHANNELS = 8

class Input(IntEnum):
    SAMPLE_MODULE_LOW = 1
    SAMPLE_MODULE_HIGH = 3
    SAMPLE_MODULE_WB = 0
    SAMPLE_MODULE_SC = 7

class DiagOutput(IntEnum):
    # Diagnostics value
    DIAG_NO_ERROR = 0x00
    DIAG_SC = 0x02
    DIAG_WB = 0x40
    CHANGEOVER_ERROR = 0x4000000
    CHATTER_ERROR = 0x20000000
    NO_SUPPLY_VOLTAGE = 0x20000



class ModuleID(IntEnum):
    SAMPLE_MODULE_V1 = 0x0001
    SAMPLE_MODULE_V2 = 0x0002

class SubmoduleID(IntEnum):
    DEFAULT_SUBMODULE = 0x108
    CNT_SUBMODULE = 0x908

class SensorType(IntEnum):
    DEACTIVATED = 0
    SAMPLE_MODULE_DEFAULT = 1
    SINGLE_CONTACT_UNWIRED = 2
    SINGLE_CONTACT_10K = 3

class InputDelay(IntEnum):
    NO_DELAY = 15
    _0_1_DELAY = 1
    _0_2_DELAY = 2
    _0_4_DELAY = 3
    _0_8_DELAY = 4
    _1_6_DELAY = 5
    _3_2_DELAY = 6
    _6_4_DELAY = 7
    _10_DELAY = 8
    _12_8_DELAY = 9
    _20_DELAY = 10
    

class DS128Channel(ct.Structure):
    _pack_ = 1
    _fields_ = [
        # Diagnostics
        ("diag_voltage", ct.c_ubyte, 1),
        ("_reserved_0", ct.c_ubyte, 1),
        ("diag_sc", ct.c_ubyte, 1),
        ("_reserved_1", ct.c_ubyte, 1),
        ("diag_wb", ct.c_ubyte, 1),
        ("_reserved_2", ct.c_ubyte, 1),
        ("diag_chatter", ct.c_ubyte, 1),
        ("diag_changeover", ct.c_ubyte, 1),
        # Input delay + HW interrupts
        ("input_delay", ct.c_ubyte, 4),
        ("hw_int_rising", ct.c_ubyte, 1),
        ("hw_int_falling", ct.c_ubyte, 1),
        ("_reserved_4", ct.c_ubyte, 2),
        # Pulse stretching
        ("pulse_stretching", ct.c_ubyte, 4),
        ("_reserved_3", ct.c_ubyte, 4),
        # Sensor type and digitl input inverting
        ("sensor_type", ct.c_ubyte, 4),
        ("_reserved_5", ct.c_ubyte, 3),
        ("digital_input", ct.c_ubyte, 1),
        # Chatter monitoring
        ("chatter_monitoring", ct.c_ubyte, 5),
        ("_reserved_6", ct.c_ubyte, 3),
        # Monitoring window
        ("monitoring_window", ct.c_ubyte, 7),
        ("_reserved_7", ct.c_ubyte, 1),
    ]

class ParametrizationStructure(ct.Structure):
    _pack_ = 1
    _fields_ = [
        # Version
        ("version_low", ct.c_ubyte, 4),
        ("version_high", ct.c_ubyte, 2),
        ("_reserved_0", ct.c_ubyte, 2),
        # Length
        ("length_of_block", ct.c_ubyte),
        ("ch", DS128Channel * NUMBER_OF_CHANNELS),
    ]

class ParametrizationStructure_versionX(ParametrizationStructure):
    def __init__(self):
        self.version_low = 1
        self.version_high = 1
        self.length_of_block = 6

# This class extends base class Emul with Sample module specific functions
class SampleModuleEmul(Emul):
    h_spi_input = None
    h_spi_output = None
    h_input_data = None
    h_diag_data = None
    h_output_data = None
    spi_input_data = 0

    def set_submodule(self, module_id: int, submodule_id: int):
        # Set submodule
        super(SampleModuleEmul, self).set_submodule(module_id, submodule_id) # module_id is not used


    def query_handles(self):
        # Query all needed handles
        ret, self.h_spi_input = llc.emul_llc_query_dataobj_handle("spi_input_data", 0, 0)
        ret, self.h_spi_output = llc.emul_llc_query_dataobj_handle("spi_output_data", 0, 0)
        ret, self.h_input_data = llc.emul_llc_query_dataobj_handle("input_data", 0, 0)
        ret, self.h_diag_data = llc.emul_llc_query_dataobj_handle("diag_buffer", 0, 0)
        ret, self.h_output_data = llc.emul_llc_query_dataobj_handle("output_data", 0, 0)

        # print("h_spi_input: " + str(self.h_spi_input))
        # print("h_spi_output: " + str(self.h_spi_output))
        # print("h_spi_data: " + str(self.h_input_data) )
        # print("h_diag_data: " + str(self.h_diag_data) )


    def set_channel(self, ch: int, val: int):
        # Update current SPI data
        #   0        1       2          7
        # |3 bits| 3 bits| 3 bits|...|3 bits|
        #
        # Clear the bits of the specified channel
        # self.spi_input_data &= ~(0b111 << (ch * 3))
        # Set the new value for the specified channel
        # self.spi_input_data |= (val & 0b111) << (ch * 3)
        vari = self.spi_input_data
        vari &= ~(0x7 << (3 * (7 - ch)))
        vari |= val << (3 * (7 - ch))
        self.spi_input_data &= ~(0x7 << (3 * (7 - ch))) # take a look at this
        self.spi_input_data |= val << (3 * (7 - ch))


        # print("WriteDoData - value")
        # print(hex(self.spi_input_data))
        # Send the data
        tx_data = bytearray()
        tx_data.extend(self.spi_input_data.to_bytes(4, byteorder='little'))
        return llc.emul_llc_write_dataobj_data(self.h_spi_output,0, tx_data)

    def get_input_data(self, length):
        ret, read_len, data = llc.emul_ll_read_dataobj_data(self.h_input_data, 0, length) # check lenght !
        if ret != 0:
            return
        # print(data)
        return data

    def get_channel_di(self, ch: int):
        ret, read_len, data = llc.emul_ll_read_dataobj_data(self.h_spi_input, 0, 2) # check lenght !
        if ret != 0:
            return
        di = (data[0] >> ch) & 0x1
        # print(str(di) + "  <-----")
        return di

    def get_channel_qi(self, ch: int): # here
        ret,read_len, data = llc.emul_ll_read_dataobj_data(self.h_spi_input, 0, 2) # check lenght !
        if ret != 0:
            return
        # print("QI DATA")
        # print(data)
        qi = (data[1] >> ch) & 0x1
        # print(str(qi) + "<------")
        return qi

    def get_diagnostics(self, ch: int):
        # print("GET DIAG")
        ret, read_len, data = llc.emul_ll_read_dataobj_data(self.h_diag_data, ch*4, 4) # here 
        if ret != 0:
            # print("ERROR - ret != 0, it is {}", ret)
            return
        # print("Diag data")
        # print(data)
        # print(int.from_bytes(data, byteorder='little'))
        return int.from_bytes(data, byteorder='little')
        

    def set_para(self, par, val):
        # print("\n##############################")
        # print(f"# SET \"{par}\" (U16) to {val}")
        # print("##############################")
        value = llc.emul_llc_U16_to_bytes(val)
        ret = llc.emul_llc_set_parameter(par,value)
        if ret!=llc.LLC_ERR_OK:
            print(f"SETPARAM failed with result={ret}")
        else:
            print(f"SETPARAM ok ({par} should be {val})")
        return ret


    def read_data_record(self, recnum, datalen):
        ret, _, data_record = super(SampleModuleEmul, self).read_data_rec(recnum, datalen)
        # print("DATA RECORD: ")
        # print(data_record)

        # print("DATARECORD ----------------")

    def read_output_data(self, datalen):
        # print("GET OUTPUT DATA")
        ret, read_len, data = llc.emul_ll_read_dataobj_data(self.h_output_data, 0, 20)
        if ret != 0:
            # print("ERROR - ret != 0, it is {}", ret)
            return
        # print("OUTPUT DATA: ")
        # print(data)
        # print(int.from_bytes(data, byteorder='little'))
        return data

    def write_output_data(self, tx_data, datalen):
        # print("WRITE OUTPUT DATA")
        llc.emul_llc_write_dataobj_data(self.h_output_data, 0, tx_data)


