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

	\dirtree{%
		.1 readme.md\DTcomment{Contents description}.
		.1 sample\_module\DTcomment{Sample I/O module project with virtual testing}.
		.2 emulator\DTcomment{Emulator project}.
        .3 emul\_app\DTcomment{Server application for emulator}.
        .3 emul\_client\_py\DTcomment{Shared files for VT clients}.
        .3 emul\_common\DTcomment{Files shared between VT library components}.
        .4 templates\DTcomment{Shared templates}.
        .4 util \DTcomment{Shared tools}.
        .3 emul\_dll\DTcomment{Emulator DLL library}.
        .4 dll\_llc\_layer\DTcomment{LLC layer}.
        .4 emul\_core\DTcomment{Emulator core - Event calendar}.
        .4 emul\_log\DTcomment{Shared logging}.
        .4 emul\_SPI\DTcomment{SPI emulation}.
        .4 module\_data\_obj\DTcomment{Data objects for the sample module}.
        .4 module\_interfaces\DTcomment{Module interface}.
        .3 module\_client\DTcomment{Directory for testing clients}.
        .4 sample\_module\_client\DTcomment{Client adjusted for the sample module}.
        .2 module\_core\DTcomment{Module layer controlling hardware}.
        .2 src\DTcomment{Source code of the module - Technology code}.
		.1 Thesis.pdf\DTcomment{PDF version of the thesis}.
	}

- [List the key features of your project.]

## Installation

[Provide instructions on how to install and set up your project.]

## Usage

[Explain how to use your project, including any relevant code examples.]


Link to publication - dspace.cvut.cz