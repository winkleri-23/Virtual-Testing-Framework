from sample_module_client import SampleModuleEmul, ParametrizationStructure_versionX, SensorType, \
                      ModuleID, SubmoduleID, Input, DiagOutput

# Instantiate emulation library (el)
el = SampleModuleEmul()

ParStructure = ParametrizationStructure_versionX()
ParStructure.ch[0].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[1].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[2].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[3].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[4].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[5].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[6].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[7].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[0].diag_wb = True
ParStructure.ch[1].diag_wb = False
ParStructure.ch[2].diag_wb = True
ParStructure.ch[3].diag_wb = True
ParStructure.ch[4].diag_wb = True
ParStructure.ch[5].diag_wb = True
ParStructure.ch[6].diag_wb = True
ParStructure.ch[7].diag_wb = True
el.power_on()

el.set_submodule(ModuleID.SAMPLE_MODULE_V2, SubmoduleID.DEFAULT_SUBMODULE)
el.write_data_rec(128, bytearray(ParStructure))
ret, _ , data_record = el.read_data_rec(128, len(bytearray(ParStructure)))
assert data_record == bytearray(ParStructure)

el.query_handles()
el.set_channel(1, Input.SAMPLE_MODULE_HIGH)
el.set_channel(2, Input.SAMPLE_MODULE_HIGH)
el.set_channel(3, Input.SAMPLE_MODULE_HIGH)
el.set_channel(4, Input.SAMPLE_MODULE_HIGH)
el.set_channel(5, Input.SAMPLE_MODULE_HIGH)
el.set_channel(6, Input.SAMPLE_MODULE_HIGH)
el.set_channel(7, Input.SAMPLE_MODULE_HIGH)

el.wait_ms(100)
assert el.get_channel_di(1) == 1
assert el.get_channel_qi(1) == 1
assert el.get_diagnostics(1) == DiagOutput.DIAG_NO_ERROR
  

for i in range (0, 10):
    el.set_channel(0, Input.SAMPLE_MODULE_LOW)
    el.wait_ms(100)
    assert el.get_channel_di(0) == 0
    assert el.get_channel_qi(0) == 1
    assert el.get_diagnostics(0) == DiagOutput.DIAG_NO_ERROR
    el.set_channel(0, Input.SAMPLE_MODULE_HIGH)
    el.wait_ms(100)
    assert el.get_channel_di(0) == 1
    assert el.get_channel_qi(0) == 1
    assert el.get_diagnostics(0) == DiagOutput.DIAG_NO_ERROR
    el.set_channel(1, Input.SAMPLE_MODULE_WB)
    el.wait_ms(100)
    assert el.get_channel_di(1) == 0
    assert el.get_channel_qi(1) == 0
    assert el.get_diagnostics(1) == DiagOutput.DIAG_NO_ERROR
    el.set_channel(2, Input.SAMPLE_MODULE_LOW)
    el.wait_ms(100)
    assert el.get_channel_di(2) == 0
    assert el.get_channel_qi(2) == 1
    assert el.get_diagnostics(2) == DiagOutput.DIAG_NO_ERROR
  


print("Test done")