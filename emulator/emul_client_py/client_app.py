import emul_llc as llc
import time

print("\n############")
print("# POWERON")
print("############")
ret = llc.emul_llc_poweron()
if ret!=llc.LLC_ERR_OK:
    print(f"POWERON failed with result={ret}")
else:
    print(f"POWERON ok")


print("\n##############################")
print("# SET \"PAR1\" (U16) to 56789")
print("##############################")
value = llc.emul_llc_U16_to_bytes(56789)
ret = llc.emul_llc_set_parameter("PAR1",value)
if ret!=llc.LLC_ERR_OK:
    print(f"SETPARAM failed with result={ret}")
else:
    print(f"SETPARAM ok (REC_NUM should be 128)")


print("\n##############")
print("# GET \"PAR1\"")
print("##############")
ret,data = llc.emul_llc_get_parameter("PAR1")
if ret!=llc.LLC_ERR_OK:
    print(f"SETPARAM failed with result={ret}")
else:
    print(f"SETPARAM ok (LEN_PAR should be 50)")

# value = llc.emul_llc_U32_to_bytes(9000)
# ret = llc.emul_llc_execute_emulation(value)
#byte_array = bytearray([0x11, 0x06, 0x14, 0x0f, 0x00, 0x01, 0x00, 0x00,
#                        0x14, 0x0f, 0x00, 0x01, 0x00, 0x00, 0x10, 0x0f,
#                        0x00, 0x01, 0x00, 0x00, 0x10, 0x0f, 0x00, 0x01,
#                        0x00, 0x00, 0x10, 0x0f, 0x00, 0x01, 0x00, 0x00,
#                        0x10, 0x0f, 0x00, 0x01, 0x00, 0x00, 0x10, 0x0f,
#                        0x00, 0x01, 0x00, 0x00, 0x10, 0x0f, 0x00, 0x01,
#                        0x00, 0x00])


print("\n####################################")
print("# WRITE RECORD \"128\" with ok data")
print("####################################")
record_data = bytearray([222]*50)   # 50 even numbers are ok (first byte even)
ret,status = llc.emul_llc_write_data_record(128,record_data)
if ret!=llc.LLC_ERR_OK:
    print(f"WRITE-RECORD failed with result={ret}")
else:
    print(f"WRITE_RECORD ok (STATUS={llc.emul_llc_STATUS_to_hex_string(status)})")
    if llc.euml_llc_STATUS_is_ok(status):
        print(f"    STATUS is OK (expected)")
    else:
        print(f"    unexpected non-ok STATUS!")


print("\n####################################")
print("# WRITE RECORD \"128\" with bad data")
print("####################################")
record_data = bytearray([221])+bytearray([222]*49)   # first byte odd causes not accepted data
ret,status = llc.emul_llc_write_data_record(128,record_data)
if ret!=llc.LLC_ERR_OK:
    print(f"WRITE-RECORD failed with result={ret}")
else:
    print(f"WRITE_RECORD ok (STATUS={llc.emul_llc_STATUS_to_hex_string(status)})")
    if llc.euml_llc_STATUS_is_ok(status):
        print(f"    unexpected ok STATUS!")
    else:
        print(f"    STATUS is ERROR (expected)")


print("\n####################################")
print("# READ RECORD \"128\", whole 50 bytes")
print("####################################")
ret,status,data = llc.emul_llc_read_data_record(128,50)
if ret!=llc.LLC_ERR_OK:
    print(f"READ-RECORD failed with result={ret}")
else:
    print(f"READ_RECORD ok (STATUS={llc.emul_llc_STATUS_to_hex_string(status)})")
    if llc.euml_llc_STATUS_is_ok(status):
        print(f"    STATUS is OK (expected)")
    else:
        print(f"    unexpected non-ok STATUS!")
    print(f"    record_data:{data}")


print("\n####################################")
print("# READ RECORD \"128\", only 5 bytes")
print("####################################")
ret,status,data = llc.emul_llc_read_data_record(128,5)
if ret!=llc.LLC_ERR_OK:
    print(f"READ-RECORD failed with result={ret}")
else:
    print(f"READ_RECORD ok (STATUS={llc.emul_llc_STATUS_to_hex_string(status)})")
    if llc.euml_llc_STATUS_is_ok(status):
        print(f"    STATUS is OK (expected)")
    else:
        print(f"    unexpected non-ok STATUS!")
    print(f"    record_data:{data}")


print("\n######################################################################################")
print("# QUERY handle for object \"NAMUR_diag_channels\", should have 8*4 bytes (no items)")
print("######################################################################################")
dataobj_len = 8*4
ret,handle = llc.emul_llc_query_dataobj_handle("NAMUR_diag_channels",dataobj_len,0)
if ret!=llc.LLC_ERR_OK:
    print(f"QUERY_HANDLE failed with result={ret}")
else:
    print(f"QUERY_HANDLE ok (NAMUR_diag_channels handle == {handle} == {hex(handle)}")
do_handle_diag = handle
assert do_handle_diag!=None


print("\n##################################################################################")
print("# QUERY handle for object \"NAMUR_spi_input\", should have 4 bytes (no items)")
print("##################################################################################")
dataobj_len = 3
ret,handle = llc.emul_llc_query_dataobj_handle("NAMUR_spi_input",dataobj_len,0)
if ret!=llc.LLC_ERR_OK:
    print(f"QUERY_HANDLE failed with result={ret}")
else:
    print(f"QUERY_HANDLE ok (NAMUR_spi_input handle == {handle} == {hex(handle)}")
do_handle_spi_input = handle
assert do_handle_spi_input!=None


print("\n################################################################")
print("# Write to a data object NAMUR_diag_channels (not successful)")
print("################################################################")
data = bytearray(1)   # write a single byte
ret,written_bytes,extra_data = llc.emul_llc_write_dataobj_data(do_handle_diag,0,data)
if ret!=llc.LLC_ERR_OK:
    print(f"WRITE_DATAOBJ failed with result={ret} (expected, object is not writable)")
else:
    print(f"WRITE_DATAOBJ ok (written bytes = {written_bytes}, extra data = {extra_data})")


print("\n################################################################")
print("# Write to a data object NAMUR_spi_input (must succeed)")
print("################################################################")
data = bytearray([0xee])   # write a single byte (value 0xEE)
ret,written_bytes,extra_data = llc.emul_llc_write_dataobj_data(do_handle_spi_input,0,data)
if ret!=llc.LLC_ERR_OK:
    print(f"WRITE_DATAOBJ failed with result={ret} (expected, object is not writable)")
else:
    print(f"WRITE_DATAOBJ ok (written bytes = {written_bytes}, extra data = {extra_data})")


print("\n################################################################")
print("# Read from a data object NAMUR_spi_input (out of range will fail)")
print("################################################################")
read_ofs = 3
read_len = 1
ret,read_bytes,data = llc.emul_ll_read_dataobj_data(do_handle_spi_input,read_ofs,read_len)
if ret!=llc.LLC_ERR_OK:
    print(f"READ_DATAOBJ failed with result={ret} (expected, object has only 3 bytes, can't read from offset {read_ofs})")
else:
    print(f"READ_DATAOBJ ok NOT EXPECTED (read bytes = {read_bytes}, data = {data})")


print("\n################################################################")
print("# Read from a data object NAMUR_spi_input (OK)")
print("################################################################")
read_ofs = 0
read_len = 3
ret,read_bytes,data = llc.emul_ll_read_dataobj_data(do_handle_spi_input,read_ofs,read_len)
if ret!=llc.LLC_ERR_OK:
    print(f"READ_DATAOBJ failed with result={ret} (NOT EXPECTED )")
else:
    print(f"READ_DATAOBJ ok (read bytes = {read_bytes}, data = {data})")


print("\n################################################################")
print("# Execute emulation for 10ms = 10*1000*1000 ns")
print("################################################################")
exec_ms = 10
exec_ns = exec_ms * 1000*1000
ret,polling = llc.emul_llc_execute_emulation(exec_ns)
if ret!=llc.LLC_ERR_OK:
    print(f"EXEC_SIMULATION failed with result={ret}")
else:
    start_exec = time.time()
    print(f"EXEC_SIMULATION ok (recommended polling = {polling} real ms")


print("\n################################################################")
print("# Wait for finished emulation 10ms")
print("################################################################")
count = 0
while True:
    ret,remain = llc.emul_llc_query_execution_finish()
    if ret!=llc.LLC_ERR_OK:
        print(f"QUERY_SIMULATION failed with result={ret}")
        break
    # return ok
    print(f"remaining {remain} model ns")
    if remain==0:
        start_exec = time.time() - start_exec
        print(f"SIMULATION ok, finished after {count} queries = after ~{start_exec:.3f} seconds")
        break
    # next attempt
    count=count+1
    if count>100:
        print(f"SIMULATION DID NOT STOP after 100 queries!")
        break
    time.sleep(0.001)


print("\n################################################################")
print("# Execute emulation for 1000ms = 1000*1000*1000 ns")
print("################################################################")
exec_ms = 1000
exec_ns = exec_ms * 1000*1000
ret,polling = llc.emul_llc_execute_emulation(exec_ns)
if ret!=llc.LLC_ERR_OK:
    print(f"EXEC_SIMULATION failed with result={ret}")
else:
    start_exec = time.time()
    print(f"EXEC_SIMULATION ok (recommended polling = {polling} real ms")


print("\n################################################################")
print("# Wait for finished emulation 1000ms")
print("################################################################")
count = 0
while True:
    ret,remain = llc.emul_llc_query_execution_finish()
    if ret!=llc.LLC_ERR_OK:
        print(f"QUERY_SIMULATION failed with result={ret}")
        break
    # return ok
    print(f"remaining {remain} model ns")
    if remain==0:
        start_exec = time.time() - start_exec
        print(f"SIMULATION ok, finished after {count} queries = after ~{start_exec:.3f} seconds")
        break
    # next attempt
    count=count+1
    if count>100:
        print(f"SIMULATION DID NOT STOP after 100 queries!")
        break
    time.sleep(0.001)


print("\n################################################################")
print("# Set improper module ID (not accepted)")
print("################################################################")
mid = 0xDEADBEEF  # not accepted, emulator is veggie
ret,flag = llc.emul_llc_set_module(mid)
if ret!=llc.LLC_ERR_OK:
    print(f"SET_MODULE failed with result={ret} (NOT EXPECTED )")
else:
    if flag:
        print(f"SET_MODULE accepted mid={hex(mid)} (NOT EXPECTED)")
    else:
        print(f"SET_MODULE refused mid={hex(mid)} (OK)")


print("\n################################################################")
print("# Set proper module ID (must be accepted)")
print("################################################################")
mid = 0xEEEE0001
ret,flag = llc.emul_llc_set_module(mid)
if ret!=llc.LLC_ERR_OK:
    print(f"SET_MODULE failed with result={ret} (NOT EXPECTED )")
else:
    if flag:
        print(f"SET_MODULE accepted mid={hex(mid)} (OK)")
    else:
        print(f"SET_MODULE refused mid={hex(mid)} (NOT EXPECTED)")


print("\n############################################################################")
print("# Insert wrong/correct submodule to wrong/correct subslot (module 0xEEEE001)")
print("##############################################################################")
print("for module 0xEEEE0001, in subslot #0 are allowed 0xDDDD0101 and 0xDDDD0103")
print("                       in subslot #1 is  allowed 0xDDDD0202")
#
print(">>> 1. wrong submodule to wrong subslot")
smid = 0xbadbad
subslot = 2
ret = llc.emul_llc_insert_submodule(subslot,smid)
if ret!=llc.LLC_ERR_OK:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} failed with result={ret} (OK)")
else:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} succeeded (NOT EXPECTED)")
#
print(">>> 2. wrong submodule to correct subslot")
smid = 0xbadbad
subslot = 1
ret = llc.emul_llc_insert_submodule(subslot,smid)
if ret!=llc.LLC_ERR_OK:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} failed with result={ret} (OK)")
else:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} succeeded (NOT EXPECTED)")
#
print(">>> 3. correct submodule to wrong subslot")
smid = 0xDDDD0202
subslot = 0  # allowed only to subslot_idx==1
ret = llc.emul_llc_insert_submodule(subslot,smid)
if ret!=llc.LLC_ERR_OK:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} failed with result={ret} (OK)")
else:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} succeeded (NOT EXPECTED)")
#
print(">>> 4. correct submodule to correct subslot")
smid = 0xDDDD0202
subslot = 1
ret = llc.emul_llc_insert_submodule(subslot,smid)
if ret!=llc.LLC_ERR_OK:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} failed with result={ret} (NOT EXPECTED)")
else:
    print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} succeeded (OK)")


print("\n################################################################")
print("# Remove submodule from a correct/invalid subslot")
print("################################################################")
subslot = 3
ret = llc.emul_llc_remove_submodule(subslot)
if ret!=llc.LLC_ERR_OK:
    print(f"REMOVE_SUBMODULE from subslot {subslot} failed with result={ret} (OK)")
else:
    print(f"REMOVE_SUBMODULE from subslot {subslot} succeeded (NOT EXPECTED)")
subslot = 1
ret = llc.emul_llc_remove_submodule(subslot)
if ret!=llc.LLC_ERR_OK:
    print(f"REMOVE_SUBMODULE from subslot {subslot} failed with result={ret} (NOT EXPECTED)")
else:
    print(f"REMOVE_SUBMODULE from subslot {subslot} succeeded (OK)")


print("\n################################################################")
print("# Deparametrize submodule in a subslot")
print("################################################################")
# configure submodule in subslot
smid = 0xDDDD0202
subslot = 1
ret = llc.emul_llc_insert_submodule(subslot,smid)
assert ret==llc.LLC_ERR_OK
print(f"INSERT-SUBMODULE {hex(smid)} to subslot {subslot} succeeded (OK)")
# depara wrong subslot
subslot = 3
ret = llc.emul_llc_depara_submodule(subslot)
if ret!=llc.LLC_ERR_OK:
    print(f"DEPARA_SUBMODULE from subslot {subslot} failed with result={ret} (OK)")
else:
    print(f"DEPARA_SUBMODULE from subslot {subslot} succeeded (NOT EXPECTED)")
# depara correct subslot
subslot = 1
ret = llc.emul_llc_depara_submodule(subslot)
if ret!=llc.LLC_ERR_OK:
    print(f"DEPARA_SUBMODULE from subslot {subslot} failed with result={ret} (NOT EXPECTED)")
else:
    print(f"DEPARA_SUBMODULE from subslot {subslot} succeeded (OK)")
