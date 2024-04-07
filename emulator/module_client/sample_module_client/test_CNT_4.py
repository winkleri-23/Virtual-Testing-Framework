from sample_module_client import SampleModuleEmul, ParametrizationStructure_versionX, SensorType, \
                      ModuleID, SubmoduleID, Input, DiagOutput

# Instantiate emulation library (el)
el = SampleModuleEmul()

ParStructure = ParametrizationStructure_versionX()
ParStructure.ch[0].sensor_type = SensorType.SAMPLE_MODULE_DEFAULT
ParStructure.ch[0].diag_wb = True


el.power_on()

el.set_submodule(ModuleID.SAMPLE_MODULE_V2, SubmoduleID.CNT_SUBMODULE)
ds128_1 = bytearray(b'\x11\x06\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00\x10\x01\x00\x01\x00\x00')

el.write_data_rec(128, bytearray(ds128_1))
ret, _ , data_record =  el.read_data_rec(128, len(bytearray(ds128_1)))
assert data_record == bytearray(ds128_1)
el.query_handles()


ds129 = bytearray([0x10, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x27, 0x10, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x10, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,  0x00,  0x00,  0x27,  0x10,
                     0x00,  0x01,  0x04,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x0a,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
                     0x00,  0x00,  0x7f,  0xff,  0xff,  0xff,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x27,  0x10,  0x00,  0x01,  0x04,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00])




el.write_data_rec(129, ds129)
ret, _ , data_record = el.read_data_rec(129, len(ds129))
assert data_record == ds129

outputdata = bytearray([0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 ])
el.write_output_data(outputdata, len(outputdata))

el.wait_ms(100)

outputdata = bytearray([0x01, 0x01, 0x01, 0x01, 0x09, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 ])
el.write_output_data(outputdata, len(outputdata))
retout = el.read_output_data(20)
assert retout == outputdata
el.wait_ms(100)

el.set_channel(0, Input.SAMPLE_MODULE_LOW)
el.wait_ms(100)

el.set_channel(0, Input.SAMPLE_MODULE_HIGH)
el.wait_ms(100)


el.set_channel(0, Input.SAMPLE_MODULE_LOW)
el.wait_ms(100)


el.set_channel(0, Input.SAMPLE_MODULE_HIGH)
el.wait_ms(100)


el.set_channel(0, Input.SAMPLE_MODULE_LOW)
el.wait_ms(100)


el.set_channel(0, Input.SAMPLE_MODULE_HIGH)
el.wait_ms(100)



el.get_input_data(42)
print("================================================================")



