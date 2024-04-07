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

- [readme.md](readme.md) <!-- Contents description -->
- [sample_module](sample_module) <!-- Sample I/O module project with virtual testing -->
  - [emulator](sample_module/emulator) <!-- Emulator project -->
    - [emul_app](sample_module/emulator/emul_app) <!-- Server application for emulator -->
    - [emul_client_py](sample_module/emulator/emul_client_py) <!-- Shared files for VT clients -->
    - [emul_common](sample_module/emulator/emul_common) <!-- Files shared between VT library components -->
      - [templates](sample_module/emulator/emul_common/templates) <!-- Shared templates -->
      - [util](sample_module/emulator/emul_common/util) <!-- Shared tools -->
    - [emul_dll](sample_module/emulator/emul_dll) <!-- Emulator DLL library -->
      - [dll_llc_layer](sample_module/emulator/emul_dll/dll_llc_layer) <!-- LLC layer -->
      - [emul_core](sample_module/emulator/emul_dll/emul_core) <!-- Emulator core - Event calendar -->
      - [emul_log](sample_module/emulator/emul_dll/emul_log) <!-- Shared logging -->
      - [emul_SPI](sample_module/emulator/emul_dll/emul_SPI) <!-- SPI emulation -->
      - [module_data_obj](sample_module/emulator/emul_dll/module_data_obj) <!-- Data objects for the sample module -->
      - [module_interfaces](sample_module/emulator/emul_dll/module_interfaces) <!-- Module interface -->
    - [module_client](sample_module/emulator/module_client) <!-- Directory for testing clients -->
      - [sample_module_client](sample_module/emulator/module_client/sample_module_client) <!-- Client adjusted for the sample module -->
  - [module_core](sample_module/module_core) <!-- Module layer controlling hardware -->
  - [src](sample_module/src) <!-- Source code of the module - Technology code -->
- [Thesis.pdf](Thesis.pdf) <!-- PDF version of the thesis -->


- [List the key features of your project.]

## Installation

[Provide instructions on how to install and set up your project.]

## Usage

[Explain how to use your project, including any relevant code examples.]


Link to publication - dspace.cvut.cz