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

# Trigger LOW level on channel 0


for i in range (0, 10):
    el.set_channel(0, Input.SAMPLE_MODULE_LOW)

    el.wait_ms(70)
    assert el.get_channel_di(0) == 0
    assert el.get_channel_qi(0) == 1
    el.set_channel(0, Input.SAMPLE_MODULE_HIGH)
    el.wait_ms(70)
    assert el.get_channel_di(0) == 1
    assert el.get_channel_qi(0) == 1
    # el.read_pral_queue(4)

    el.set_channel(1, Input.SAMPLE_MODULE_LOW)
    el.wait_ms(70)
    assert el.get_channel_di(1) == 0
    el.set_channel(1, Input.SAMPLE_MODULE_HIGH)
    el.wait_ms(70)
    assert el.get_channel_di(1) == 1
    assert el.get_channel_qi(1) == 1




print("Test done")