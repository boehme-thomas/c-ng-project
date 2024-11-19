# Contiki-NG Projektmodul
***
This repo contains helpful source code for the development, usage and understanding of the Time Slotted Channel Hopping
(TSCH) protocol. Examples are created using Contiki-NG and are tested with Cooja or using Nordic Semiconductor nRF52840-DK or -dongle boards.

## General
### Directories
This directory is subdivided between changed code from the original repo by Contiki-NG (in [/changed-code](/changed-code)) and 
own written code (in [/new-code](/new-code)).

The [/new-code](/new-code) directory further contains:
 - [/examples_and_tests](/new-code/examples_and_tests) contains different examples for the usage of tsch
   - [/tsch_schedule](/new-code/examples_and_tests/tsch_schedule) contains different examples corresponding to tsch schedules
 - [/modules](/new-code/modules) contains source code for different functionalities (schedule and statistics; more info [here](/new-code/modules/MODULES.md)) 
 - [/server](/new-code/server) contains source code for a server (requesting statistics and creating a plot)
 - [/fina_example](/new-code/final_example) contains the source code, which is used with /modules and /server

### Deployability
The different projects might be configured to just run with the devices at hand, e.g. [/new-code/example_and_tests/tsch_schedule](/new-code/examples_and_tests/tsch_schedule), without changes,
should only be deployed for Nordic Semiconductor boards nRF52840-DK and is not guaranteed to work when used with e.g. cooja.
Additionally, without changes some examples might not run (correctly or with full functionality) on every nRF52840-DK device. 
The reason for that is the use of static Mac-addresses and device specific hardware specifications.

#### Battery
When using nRF52840-dongles with batteries, they might don't send when not connected to a PC. To change that, the compiler
directive `NRF52840_NATIVE_USB` should be set to `0`.

### Device path and serial number
For some commands it can be handy to set the device port or the serial number of a specific node. The device path (in linux) may look something like
/dev/ttyACM0. The serial number for nRF52840-DK / -dongle devices can be found on the node itself; the last, eleven digits long number on the white patch. \
Contiki-NG gives a possibility to list all connected nodes with their port(s) and serial number. 

    make TARGET=nrf52840 motelist-all

The resulting list may contain more than one entry per device; you should use the port with the lower number.

## Compilation
Makefiles and project-conf.h files are for configuring the specific source file.

Generally it is good practise to read the `Makefile` of a project to see which compiler directive might be needed.

An example for a minimal Makefile (from [/new-code/example_and_tests/nullnet_message_to_neighbor](/new-code/examples_and_tests/nullnet_message_to_neighbor)):

    CONTIKI_PROJECT = send_to_neighbor project-conf
    all: $(CONTIKI_PROJECT)

    CONTIKI = ../../../../contiki-ng
    
    #use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
    MAKE_MAC = MAKE_MAC_TSCH
    MAKE_NET = MAKE_NET_NULLNET
    include $(CONTIKI)/Makefile.include

For local compilation, Makefiles might need to be changed, so that `CONTIKI` points to the Contiki-NG folder on the local machine.

To compile a project, a target should be selected with like:
    
    make TARGET=nrf52840

Additionally, when compiling, one might set the `ARCH_PATH` argument to point to /contiki-ng/arch/ (this might not be necessary), e.g.:

    make ARCH_PATH=<Path_to_contiki-ng>/arch/ TARGET=cooja

**Compilation and flashing nrf52840 boards depends on the boards in use. With the compiler directive `BOARD={dk|dongle}` the correct
platform can be selected.**

### DK

For deployment on nRF52840-DK boards, the target and board needs to be set correctly:

    make TARGET=nrf52840 BOARD=dk

To flash the boards, the preconditions from Nordic Semiconductor must be fulfilled and the board(s) must be connected via a 
micro-usb data cable:

    make TARGET=nrf52840 BOARD=dk <name_of_file>.upload

It is also possible to execute the compilation and flashing in the same step using the previous command.

### Dongle

When a dongle is used, flashing has to be done like the following, where a port (on which the device is connected, on linux devices e.g. /dev/ttyACM0)
has to be given:

    make TARGET=nrf52840 BOARD=dongle PORT=<device_path> <name_of_file>.dfu-upload

### Recovery

However, flashing may lead to an error where the board(s) can't be updated. To solve the issue use 

    nrfjprog --recover

and execute the previous command again. 

To flash or recover only certain devices you can set a port or the serial number (found on the device itself) respectively 
(for more information see [here](#Device-path-and-serial-number)).

    make TARGET=nrf52840 PORT=<device_path> <name_of_file>.upload

    nrfjprog --recover -s XXXXXXXXXXX

For more information see the [nrf52840 SDK site in the contiki documentation](https://docs.contiki-ng.org/en/develop/doc/platforms/nrf52840.html).

### EDGE directive

For some projects (like [final_example](/new-code/final_example)) it is important to set an edge node. That is the node, which is
connected to the server and functions as a network edge device; sending into and receiving messages from the network, and directing
it to the server.\
To select whether a node is an edge node the following compilation directive can be set to `0` or `1`:

    make TARGET=nrf52840 EDGE=0

That might lead to and can be used for adding [extended functionality](#extended-functionality) like the extended shell functionality
in the [Makefile of final_example](/new-code/final_example/Makefile).

## Extended functionality

To extend the functionality of Contiki-NG the source files have to be compiled and linked correctly.

To use functionality included in Contiki-NG but not set, the directive `MODULES += <path>` in the Makefile can be used (e.g. `MODULES += os/services/shell`).
\
For selecting own code or replacing files of Contiki-NG the directive `MODULES_REL += <path_to_module>` has to be used. 
\
Both of them before "`include $(CONTIKI)Makefile.inlcude`". Look [here](/new-code/final_example/Makefile) for an example.

## Connection to a device
It is possible to connect to a device via:

    make TARGET=nrf52840 PORT=<device_path> login

When the "shell" service is additionally activated in the Makefile (before "Makefile.include")

    ...
    MODULES += os/services/shell
    ...

it is also possible to send commands to the device.

When the compiler directive `NRF52840_NATIVE_USB` is set to `1` (like e.g. [here](/new-code/final_example/Makefile)),
the DK board needs to be connected to the "nrf52840 USB" port, after flashing.