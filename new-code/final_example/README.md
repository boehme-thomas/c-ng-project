To compile the project the directives `EDGE` and `BOARD` must be given. Look in [here](/README.md) for more information about them.

For the edge node (the node communicating with the server) compilation would be like:

    make TARGET=nrf52840 BOARD=dk EDGE=1

To flash the (edge) dk:
    
    make TARGET=nrf52840 BOARD=dk EDGE=1 test_distribution_schedule.upload

To compile for and flash the dongle:

    make TARGET=nrf52840 BOARD=dongle EDGE=0 PORT=<device_path> test_distribution_schedule_dongle.dfu-upload


`EDGE=1` will add shell functionality.

`BOARD=dongle` will additionally select another source file for the dongle.

