# Virtual Testing Framework for Industrial IO Modules

## Description

This thesis aims to create a Virtual Testing Framework that can effectively increase the efficiency of the project delivery process 
of I/O modules in the Company. In order to achieve this goal, the following steps need to be followed.

Firstly, get familiar with the current testing throughout the development process in the Company. This includes understanding 
the testing requirements, current methods, and the overall workflow of the testing process in the Company.

Secondly, analyze the current testing process and identify the areas where the testing can be improved. 
This step of identifying the bottlenecks is crucial for deriving the requirements for the Virtual Testing Framework.

Thirdly, based on the requirements identified in the previous step, propose a solution that addresses the identified inefficiencies 
and therefore lowers the required resources for testing while improving its quality.

Lastly, implement the proposed solution, demonstrate its capabilities, and evaluate its impact from the project management perspective. 
That includes the time saved on testing, the quality of the testing, and the overall improvement in the project delivery process.


## Directory Structure
```
.
├── readme.md                          # Contents description
├── sample_module                      # Sample I/O module project with virtual testing
│   ├── emulator                       # Emulator project
│   │   ├── emul_app                   # Server application for emulator
│   │   ├── emul_client_py             # Shared files for VT clients
│   │   ├── emul_common                # Files shared between VT library components
│   │   │   ├── templates              # Shared templates
│   │   │   └── util                   # Shared tools
│   │   ├── emul_dll                   # Emulator DLL library
│   │   │   ├── dll_llc_layer          # LLC layer
│   │   │   ├── emul_core              # Emulator core - Event calendar
│   │   │   ├── emul_log               # Shared logging
│   │   │   ├── emul_SPI               # SPI emulation
│   │   │   ├── module_data_obj        # Data objects for the sample module
│   │   │   └── module_interfaces      # Module interface
│   │   └── module_client              # Directory for testing clients
│   │       └── sample_module_client   # Client adjusted for the sample module
│   ├── module_core                    # Module layer controlling hardware
│   └── src                            # Source code of the module - Technology code
└── Thesis.pdf                         # PDF version of the thesis
```
## Installation & Usage

Installation and usage instructions can be found in the [Thesis](Thesis.pdf) document in Apendix B.
