import emul_llc as llc

class Emul:
    def power_on(self):
        return llc.emul_llc_poweron()

    def set_submodule(self, module_id, submodule_id):
        llc.emul_llc_insert_submodule(1, submodule_id)

    def read_data_rec(self, num: int, datalen: int):
        return llc.emul_llc_read_data_record(num, datalen)

    def write_data_rec(self, num: int, data: bytearray):
        return llc.emul_llc_write_data_record(num, data)

    def get_input_data(self):
        pass

    def set_output_data(self, data: bytearray):
        pass

    def wait_ns(self, ns: int):
        return llc.emul_llc_execute_emulation(ns)

    def wait_us(self, us: int):
        return self.wait_ns(us * 1000)

    def wait_ms(self, ms: int):
        return self.wait_ns(ms * 1000000)

    def get_diag_queue(self):
        pass