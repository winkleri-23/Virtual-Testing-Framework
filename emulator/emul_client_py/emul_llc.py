import time, pywintypes, win32api, win32file, win32pipe
from enum import IntEnum

#==================
# Global constants 
#==================
EMULATOR_PIPE_NAME = r'\\.\pipe\ET200SP_emulator_LLC_pipe'

#====================
# LLC command codes
#====================
class LLC_CODE(IntEnum):
    EMUL_LLC_POWER_ON               = 1
    EMUL_LLC_SET_PARAM              = 2
    EMUL_LLC_GET_PARAM              = 3
    EMUL_LLC_WRITE_DATA_RECORD      = 4
    EMUL_LLC_READ_DATA_RECORD       = 5
    EMUL_LLC_QUERY_DO_HANDLE        = 6
    EMUL_LLC_WRITE_DO_DATA          = 7
    EMUL_LLC_READ_DO_DATA           = 8
    EMUL_LLC_EXECUTE_NS             = 9
    EMUL_LLC_QUERY_EXECUTION        = 10
    EMUL_LLC_SET_MODULE             = 11,
    EMUL_LLC_INSERT_SUBMODULE       = 12,
    EMUL_LLC_REMOVE_SUBMODULE       = 13,
    EMUL_LLC_DEPARA_SUBMODULE       = 14,
    EMUL_LLC_READ_PRAL              = 15,

#====================
# LLC error codes
#====================
##### everything is OK
LLC_ERR_OK                      = 0
##### interface errors (not actual LLC responses)
LLC_ERR_NO_CONNECTION           = 101
LLC_ERR_CONNECTION_FAILED       = 102
LLC_ERR_WRITE_FAILED            = 103
LLC_ERR_READ_FAILED             = 104
# Function not supported 
LLC_ERR_NOT_SUPPORTED           = 105
# Function supported, but not implemented
LLC_ERR_NOT_IMPLEMENTED         = 106
# wrong parameters supplied
LLC_ERR_WRONG_PARAMETERS        = 107
# Wrong answer from server (general)
LLC_ERR_WRONG_RESPONSE          = 108
# Wrong response from server (payload)
LLC_ERR_WRONG_PAYLOAD           = 109
##### codes 1000+ reserved for EMUL-errors


#================
# Logging flags
#================   
# Connection to the emulator
LOG_CONNECT_EMUL              = False     
# All sent frames
LOG_SENT_FRAMES               = False
# All received frames
LOG_RECV_FRAMES               = False
# Data from received frame
LOG_RECV_RESULT               = False

# Helper logging function
def Log(flag,*args):
    if flag==True:
        print(*args)

#===========================
# Global private variables
#===========================

# PyHANDLE instance for the pipe connection
__emulator_pipe_handle         = None


#========================================================
# Helper function, converts number to LE bytes (uint32)
#========================================================
def emul_llc_U32_to_bytes(val):
    return bytes([ val&0xFF, (val>>8)&0xFF, (val>>16)&0xFF, (val>>24)&0xFF ])


#========================================================
# Helper function, converts LE bytes (uint32) to number
#========================================================
def emul_llc_bytes_to_U32(b):
    return b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24)


#========================================================
# Helper function, converts number to LE bytes (uint16)
#========================================================
def emul_llc_U16_to_bytes(val):
    return bytes([ val&0xFF, (val>>8)&0xFF ])


#========================================================
# Helper function, converts LE bytes (uint16) to number
#========================================================
def emul_llc_bytes_to_U16(b):
    return b[0] + (b[1]<<8)


#========================================================
# Helper function, converts text to brief variant
#========================================================
def emul_llc_to_brief(txt):
    if len(txt)>16:
        txt = str(txt[:16])+"..."
    else:
        txt = str(txt)
    return txt


#==============================================
# One attempt of a connection to the emulator 
#==============================================
def __emul_try_connect_emulator():
    global __emulator_pipe_handle

    retval = False
    # connects to a named pipe, catch exceptions
    try:
        Log(LOG_CONNECT_EMUL,f"Trying to connect to: {EMULATOR_PIPE_NAME}")

        __emulator_pipe_handle = win32file.CreateFile(
                    EMULATOR_PIPE_NAME,
                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                    0,
                    None,
                    win32file.OPEN_EXISTING,
                    0,
                    None
                )
        # open succeeded
        Log(LOG_CONNECT_EMUL,"Connection succeeded")

        # configure the channel to message mode, non-blocking
        res = win32pipe.SetNamedPipeHandleState(__emulator_pipe_handle, 
                                                win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_NOWAIT, None, None)
        if res == 0:
            Log(LOG_CONNECT_EMUL,f"SetNamedPipeHandleState return code: {res}")
            raise Exception("SetNamedPipeHandleState failed")

        # ok
        Log(LOG_CONNECT_EMUL,f"all done {__emulator_pipe_handle}")
        retval = True

    except pywintypes.error as e:
        Log(LOG_CONNECT_EMUL,f"Connection exception error: {e}")
        exception_code = e.args[0]
        if exception_code == 2:
            # Try again
            pass
        elif exception_code == 109:
            # Pipe has been closed (broken)
            pass

    # return True only in case of success
    Log(LOG_CONNECT_EMUL,f"returning {retval}")
    return retval


#===========================================
# Closes the connection to emulator's pipe
#===========================================
def __emul_close_connection():
    global __emulator_pipe_handle
    if __emulator_pipe_handle!=None:
        win32file.CloseHandle(__emulator_pipe_handle)
        __emulator_pipe_handle = None
    return


#===================================================
# Establishes a connection to the emulator process
#===================================================
"""! Connects to the emulator (several attempts)
@return  True if connected successfully, False otherwise
"""
def emul_llc_connect_emulator():
    attempt = 0;
    while attempt<5:
        if __emul_try_connect_emulator():
            break
        attempt=attempt+1
    return attempt<5


#===================================================
# Finishes the connection to the emulator process
#===================================================
"""! Disconnects from the emulator
"""
def emul_llc_disconnect_emulator():
    __emul_close_connection()
    return


#==============================================================================
# Establishes a connection to the emulator process, if there is no connection
#==============================================================================
"""! Connects to the emulator if needed
@return  True if connected successfully, False otherwise
"""
def emul_llc_ensure_connect_emulator():
    global __emulator_pipe_handle
    if __emulator_pipe_handle==None:
        return emul_llc_connect_emulator()
    # already connected
    return True


#==================================================================
# Sends a LLC command and return the answer along with error code
#==================================================================
"""! Executes one LLC transaction on opened connection
@param cmd      command code
@param param    parameter bytearray
@return         list [comm_code,header_error_code,flags,payload,header]
"""
def emul_llc_do_llc_transaction(cmd,*params):
    global __emulator_pipe_handle

    # return list
    retval = [0,0,0,None,None]

    # if there is no connection, fail
    if __emulator_pipe_handle==None:
        Log(LOG_SENT_FRAMES, "No connection to emulator")
        retval[0] = LLC_ERR_NO_CONNECTION
        return retval

    # build a header of the frame (8 bytes)
    frame = bytearray([0xA2, cmd, 0, 0, 0, 0, 0, 0]) 
    # sum up all parameters + count length
    plen = 8
    for p in params:
        plen = plen + len(p)
        frame.extend(p)
    # fill the length
    frame[2] = int(plen & 0xff)
    frame[3] = int(plen/256)

    # write the command
    Log(LOG_SENT_FRAMES, "Send:", emul_llc_to_brief(frame))
    try:
        errcode,byteswritten = win32file.WriteFile(__emulator_pipe_handle,frame)
    except pywintypes.error as e:
        Log(LOG_SENT_FRAMES,f"Write command failed: {e}")
        retval[0] = LLC_ERR_WRITE_FAILED
        return retval

    # small delay for server to react
    time.sleep(0.02)

    # receive answer
    attempt = 0
    while attempt<100:
        if attempt>0:
            Log(True,f"receiving attempt {attempt+1} ...")
        try:
            errcode,answer = win32file.ReadFile(__emulator_pipe_handle,64*1024)
            break
        except pywintypes.error as e:
            exception_code = e.args[0]
            if exception_code == 232:
                # No data from server, can try again
                pass
            elif exception_code == 109:
                # pipe has been closed (server ended)
                Log(LOG_RECV_FRAMES,f"Connection closed by server.")
                __emul_close_connection()    
                retval[0] = LLC_ERR_READ_FAILED
                return retval
            else:
                Log(LOG_RECV_FRAMES,f"Read command failed: {e}")
                __emul_close_connection()    
                retval[0] = LLC_ERR_READ_FAILED
                return retval

        # Next attempt
        attempt = attempt+1
        time.sleep(0.5)

    # if there was no answer
    if attempt>=100:
        Log(LOG_RECV_FRAMES,"Read command failed - server is not responding")
        __emul_close_connection()    
        retval[0] = LLC_ERR_READ_FAILED
        return retval

    Log(LOG_RECV_FRAMES, "Recv:", emul_llc_to_brief(answer))

    # check the answer header
    if len(answer)<8:
        Log(LOG_RECV_FRAMES,"Read command failed - too short answer from server")
        __emul_close_connection()    
        retval[0] = LLC_ERR_WRONG_RESPONSE
        return retval

    if answer[0]!=0xA2:
        Log(LOG_RECV_FRAMES,"Read command failed - too short answer from server")
        __emul_close_connection()    
        retval[0] = LLC_ERR_WRONG_RESPONSE
        return retval

    answer_len = answer[2] + 256*answer[3]
    if answer_len!=len(answer):
        Log(LOG_RECV_FRAMES,"Read command failed - bad length")
        __emul_close_connection()    
        retval[0] = LLC_ERR_WRONG_RESPONSE
        return retval

    if answer[1]!=frame[1]+0x80:
        Log(LOG_RECV_FRAMES,"Read command failed - bad message code")
        __emul_close_connection()    
        retval[0] = LLC_ERR_WRONG_RESPONSE
        return retval

    # response is correct, parse it
    retval[0] = LLC_ERR_OK
    retval[1] = answer[4] + 256*answer[5]
    retval[2] = answer[7]
    retval[3] = answer[8:]
    retval[4] = answer[0:8]

    # log answer if needed
    if LOG_RECV_RESULT:
        print(f"Received answer:")
        print(f"   error_code = {retval[1]}")
        print(f"   flags      = {retval[2]}")
        print(f"   payload    = {emul_llc_to_brief(retval[3])}")
        print(f"   header     = {retval[4]}")

    return retval


#================================================================
# Resets the emulated module to its initial state (power cycle)
#================================================================
"""! requests POWER cycle of the emulator
@return         error_code (int)
"""
def emul_llc_poweron():
    if not emul_llc_ensure_connect_emulator():
        return LLC_ERR_CONNECTION_FAILED
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_POWER_ON, bytearray([]) )
    if retval[0]!=0:
        # error with transaction
        return retval[0]
    if retval[1]!=0:
        # error in command
        return retval[1]
    # check answer
    if len(retval[3])!=0:
        return LLC_ERR_WRONG_PAYLOAD
    # OK, done
    return LLC_ERR_OK


#=====================================
# Sets emulator parameter to a value
#=====================================
"""! sets internal emulator parameter
@param     name   parameter name (string)
@param     data   parameter binary data (must conform with parameter type)
@return    error_code (int)
"""
def emul_llc_set_parameter(name,data):
    # name must not be too long and data must be same
    if len(name)>32 or len(data)==0:
        return LLC_ERR_WRONG_PARAMETERS
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return LLC_ERR_CONNECTION_FAILED
    # prepare data
    par=bytes(name,'ascii').ljust(32,b'\x00')+data
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_SET_PARAM, par )
    if retval[0]!=0:
        # error with transaction
        return retval[0]
    if retval[1]!=0:
        # error in command
        return retval[1]
    # check answer (expected empty payload)
    if len(retval[3])!=0:
        return LLC_ERR_WRONG_PAYLOAD
    # OK, done
    return LLC_ERR_OK


#===========================
# Gets emulator parameter 
#===========================
"""! sets internal emulator parameter
@param     name   parameter name (string)
@return    (error_code,parameter_data) (tuple)
"""
def emul_llc_get_parameter(name):
    # name must not be too long and data must be same
    if len(name)>32:
        return (LLC_ERR_WRONG_PARAMETERS,None)
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return (LLC_ERR_CONNECTION_FAILED,None)
    # prepare data
    par=bytes(name,'ascii').ljust(32,b'\x00')
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_GET_PARAM, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected not-empty payload)
    if len(retval[3])==0:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # OK, done
    return (LLC_ERR_OK,retval[3])


#=================================================
# Helper function, check 4 bytes of status value
#=================================================
def euml_llc_STATUS_is_ok(status):
    if not type(status) is list:
        return False
    if len(status)!=4:
        return False
    if status[0]!=0 or status[1]!=0 or status[2]!=0 or status[3]!=0:
        return False
    # status is ok
    return True


#==============================================================
# Helper function, converts 4 bytes of STATUS to a hex string
#==============================================================
def emul_llc_STATUS_to_hex_string(status):
    if not type(status) is list:
        return False
    if len(status)!=4:
        return False
    return '['+hex(status[0])+','+hex(status[1])+','+hex(status[2])+','+hex(status[3])+']'


#===========================
# Writes a data record
#===========================
"""! performs write-data-record operation
@param     recnum   record number (e.g., 128)
@param     recdata  record data
@return    (error_code,status) (tuple), status is list[4] of error codes
"""
def emul_llc_write_data_record(recnum,recdata):
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return (LLC_ERR_CONNECTION_FAILED,None)
    # prepare data
    par=emul_llc_U16_to_bytes(recnum)
    par=par+emul_llc_U32_to_bytes(len(recdata))
    par=par+recdata
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_WRITE_DATA_RECORD, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected exactly 4 bytes)
    if len(retval[3])!=4:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # OK, done
    return (LLC_ERR_OK,list(retval[3]))


#======================
# Reads a data record
#======================
"""! performs read-data-record operation
@param     recnum   record number (e.g., 128)
@param     datalen  required read length (from the beginning of data record)
@return    (error_code,status,data) (tuple), status is list[4] of error codes, data is bytearray of read data
"""
def emul_llc_read_data_record(recnum,datalen):
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return (LLC_ERR_CONNECTION_FAILED,None,None)
    # prepare data
    par=emul_llc_U16_to_bytes(recnum)
    par=par+emul_llc_U32_to_bytes(datalen)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_READ_DATA_RECORD, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None,None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None,None)
    # check answer (expected exactly 4 bytes or more)
    if len(retval[3])<4:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    if len(retval[3])==4:
        # error reading only status given
        return (LLC_ERR_OK,list(retval[3]),None)
    # more that 4 bytes, so it must be at least 4 more
    if len(retval[3])<8:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # extract the length
    readlen = emul_llc_bytes_to_U32(retval[3][4:8])
    if readlen!=datalen:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # some bytes read
    return (LLC_ERR_OK,list(retval[3][:4]),retval[3][8:])


#====================================================
# Queries a handle to existing emulator data object
#====================================================
"""! queries emulator data object
@param     object_name   name of the data object (string)
@param     expected_len  expected len of the data object (0 for no checking or for stream data objects)
@param     expected_item_len  only for stream-like data objects (expected len must be 0, this parameters checks item size)
@return    (error_code,data) (int,bytes)
"""
def emul_llc_query_dataobj_handle(object_name,expected_len,expected_item_len):
    # name must not be too long and data must be same
    if len(object_name)>128:
        return (LLC_ERR_WRONG_PARAMETERS,None)
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return (LLC_ERR_CONNECTION_FAILED,None)
    # prepare data
    par=emul_llc_U32_to_bytes(expected_len)
    par=par+emul_llc_U32_to_bytes(expected_item_len)
    par=par+bytes(object_name,'ascii').ljust(128,b'\x00')
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_QUERY_DO_HANDLE, par )
    hex_string_3 = retval[3]
    hex_string_0 = retval[0]
    hex_string_1 = retval[1]
    hex_string_2 = retval[2]
    # print("Retval[0] " + str(hex_string_0))
    # print("Retval[1] " + str(hex_string_1))
    # print("Retval[2] " + str(hex_string_2))
    # print("Retval[3] " + str(hex_string_3))
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected 4 bytes payload)
    if len(retval[3])!=4:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # OK, done
    return (LLC_ERR_OK,emul_llc_bytes_to_U32(retval[3]))


#============================================
# Writes a new data to emulator data object
#============================================
"""! writes data to a data object
@param     handle  handle of the data object (discovered by quesy_handle call)
@param     offset  offset in data object to write to (if not zero, object must by partially writable, chunk must fit to the object)
@param     data    data array to write to the data object
@return    (error_code,written_bytes,extra_data) (int,int,bytes)
"""
def emul_llc_write_dataobj_data(handle,offset,data):
    # connect if needed
    if not emul_llc_ensure_connect_emulator():
        return (LLC_ERR_CONNECTION_FAILED,None)
    # prepare data
    par=emul_llc_U32_to_bytes(handle)
    par=par+emul_llc_U32_to_bytes(offset)
    par=par+emul_llc_U32_to_bytes(len(data))
    par=par+data
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_WRITE_DO_DATA, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None,None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None,None)
    # check answer (expected at least 8 bytes of payload)
    if len(retval[3])<8:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # extract and check the handle
    wr_handle = emul_llc_bytes_to_U32(retval[3][0:4])
    if wr_handle!=handle:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # written length must be same
    wr_len = emul_llc_bytes_to_U32(retval[3][4:8])
    if wr_len!=len(data):
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # everything is ok
    return (LLC_ERR_OK,wr_len,retval[3][8:])


#===============================================
# Reads actual data from emulator data object
#===============================================
"""! reads data from a data object
@param     handle  handle of the data object (discovered by quesy_handle call)
@param     offset  offset in data object to read from (if not zero, object must by partially readable, chunk must fit to the object)
@param     length  length of data object to be read 
@return    (error_code,read_num_bytes,read_data) (int,int,bytes)
"""
def emul_ll_read_dataobj_data(handle,offset,length):
    # prepare data
    par=emul_llc_U32_to_bytes(handle)
    par=par+emul_llc_U32_to_bytes(offset)
    par=par+emul_llc_U32_to_bytes(length)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_READ_DO_DATA, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None,None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None,None)
    # check answer (expected at least 3*4=12 bytes of payload)
    if len(retval[3])<12:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # extract and check the handle
    wr_handle = emul_llc_bytes_to_U32(retval[3][0:4])
    if wr_handle!=handle:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # read offset must be same
    read_ofs = emul_llc_bytes_to_U32(retval[3][4:8])
    if read_ofs!=offset:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # read len must be same
    read_len = emul_llc_bytes_to_U32(retval[3][8:12])
    if read_len!=length:
        return (LLC_ERR_WRONG_PAYLOAD,None,None)
    # everything is ok
    return (LLC_ERR_OK,read_len,retval[3][12:])


#====================================================
# Executes emulator for given number of nanoseconds
#====================================================
"""! executes emulation for a given time
@param     ns   requested simulation time (nanoseconds of model time)
@return    (error_code,recommended_polling) (int,int)
"""
def emul_llc_execute_emulation(ns):
    # prepare data
    par=emul_llc_U32_to_bytes(ns)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_EXECUTE_NS, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected 4 bytes of payload)
    if len(retval[3])!=4:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # get polling
    polling = emul_llc_bytes_to_U32(retval[3][0:4])
    # everything is ok
    return (LLC_ERR_OK,polling)


#==================================
# Queries if emulation still runs 
#==================================
"""! returns number of remaining nanoseconds of last simulation step (0=complete)
@return    (error_code,remaining_ns) (int,int)
"""
def emul_llc_query_execution_finish():
    # prepare data (none)
    par=bytes()
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_QUERY_EXECUTION, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected 4 bytes of payload)
    if len(retval[3])!=4:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # get remaining ns
    remain = emul_llc_bytes_to_U32(retval[3][0:4])
    # everything is ok
    return (LLC_ERR_OK,remain)


#==============================================
# Sets a module number (configures module ID)
#==============================================
"""! configures the module number in emulated device
@param     module_id    ID code of desired module
@return    (error_code,flag_accepted) (int,int)
"""
def emul_llc_set_module(module_id):
    # prepare data 
    par=emul_llc_U32_to_bytes(module_id)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_SET_MODULE, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None)
    if retval[1]!=0:
        # error in command
        return (retval[1],None)
    # check answer (expected 4 bytes of payload)
    if len(retval[3])!=4:
        return (LLC_ERR_WRONG_PAYLOAD,None)
    # get flag of result
    flag_accept = emul_llc_bytes_to_U32(retval[3][0:4])
    # everything is ok
    return (LLC_ERR_OK,flag_accept)


#==========================================================
# Configures a submodule (inserts submodule to a subslot)
#==========================================================
"""! emulates configuring a submodule to a subslot
@param     subslot_idx  zero based index of subslot
@param     submodule_id  number of the configured submodule
@return    error_code   int
"""
def emul_llc_insert_submodule(subslot_idx,submodule_id):
    # prepare data 
    par=emul_llc_U32_to_bytes(subslot_idx)
    par=par+emul_llc_U32_to_bytes(submodule_id)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_INSERT_SUBMODULE, par )
    if retval[0]!=0:
        # error with transaction
        return retval[0]
    if retval[1]!=0:
        # error in command
        return retval[1]
    # check answer (expected none bytes of payload)
    if len(retval[3])!=0:
        return LLC_ERR_WRONG_PAYLOAD
    # everything is ok
    return LLC_ERR_OK


#=======================================================
# Removes configured a submodule from a subslot index
#=======================================================
"""! emulates removing (de-configuring) of a submodule from a subslot
@param     subslot_idx  zero based index of subslot
@return    error_code   int
"""
def emul_llc_remove_submodule(subslot_idx):
    # prepare data 
    par=emul_llc_U32_to_bytes(subslot_idx)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_REMOVE_SUBMODULE, par )
    if retval[0]!=0:
        # error with transaction
        return retval[0]
    if retval[1]!=0:
        # error in command
        return retval[1]
    # check answer (expected none bytes of payload)
    if len(retval[3])!=0:
        return LLC_ERR_WRONG_PAYLOAD
    # everything is ok
    return LLC_ERR_OK


#=================================================================
# Deparametrizes a submodule (removes parameters from a subslot)
#=================================================================
"""! emulates removing parameters (de-parametrizing) of a submodule 
     in a subslot (submodule remains parametrized)
@param     subslot_idx  zero based index of subslot
@return    error_code   int
"""
def emul_llc_depara_submodule(subslot_idx):
    # prepare data 
    par=emul_llc_U32_to_bytes(subslot_idx)
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_DEPARA_SUBMODULE, par )
    if retval[0]!=0:
        # error with transaction
        return retval[0]
    if retval[1]!=0:
        # error in command
        return retval[1]
    # check answer (expected none bytes of payload)
    if len(retval[3])!=0:
        return LLC_ERR_WRONG_PAYLOAD
    # everything is ok
    return LLC_ERR_OK


def emul_llc_read_pral():
    # prepare data (none)
    par=bytes()
    # perform transaction
    retval = emul_llc_do_llc_transaction( LLC_CODE.EMUL_LLC_READ_PRAL, par )
    if retval[0]!=0:
        # error with transaction
        return (retval[0],None,None)
    # read len must be same
    read_len = emul_llc_bytes_to_U32(retval[3][0:4])
    # everything is ok
    return (LLC_ERR_OK,read_len,retval[3][4:])

#=================
# Initialization
#=================
# no emulator handle
__emulator_pipe_handle         = None

