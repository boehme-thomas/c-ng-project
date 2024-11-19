# Makefile for test_distribute_schedule project.
# With BOARD = { dk | dongle } the correct file is selected
# With EDGE = { 0 | 1 } the functionalities for the edge node are turned on

ifeq ($(BOARD), dongle)
CONTIKI_PROJECT = final_example_dongle project-conf
else
CONTIKI_PROJECT = final_example project-conf
endif

all: $(CONTIKI_PROJECT)
CONTIKI = ../../../contiki-ng

ifeq ($(EDGE), 1)
MODULES += os/services/shell
MODULES_REL += ../modules/my_shell ../modules/distribution_of_schedule ../modules/publication_of_statistics
NRF52840_NATIVE_USB = 1
endif

ifeq ($(EDGE), 0)
MODULES_REL += ../modules/distribution_of_schedule ../modules/publication_of_statistics
NRF52840_NATIVE_USB = 0
endif

MAKE_MAC = MAKE_MAC_TSCH
MAKE_NET = MAKE_NET_NULLNET

include $(CONTIKI)/Makefile.include