from sample_module_client import SampleModuleEmul, ParametrizationStructure_versionX, SensorType, \
                      ModuleID, SubmoduleID, Input, DiagOutput

# Instantiate emulation library (el)
el = SampleModuleEmul()

ParStructure = ParametrizationStructure_versionX()
ParStructure.ch[0].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[0].diag_wb = True
ParStructure.ch[0].hw_int_rising = 0x1
ParStructure.ch[1].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[1].diag_wb = True
ParStructure.ch[1].hw_int_rising = 0x1
el.power_on()


el.set_submodule(ModuleID.SAMPLE_MODULE_V2, SubmoduleID.DEFAULT_SUBMODULE)
el.query_handles()
el.write_data_rec(128, bytearray(ParStructure))

# test if the module has accepted the data record with no changes

ret, _ , data_record = el.read_data_rec(128, len(bytearray(ParStructure)))
assert data_record == bytearray(ParStructure)


# Test that the actual level is HIGH


print("Test done")