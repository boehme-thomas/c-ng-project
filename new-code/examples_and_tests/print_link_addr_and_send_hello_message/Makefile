CONTIKI_PROJECT = print_link_addr_send_hello project-conf
all: $(CONTIKI_PROJECT)

#change this to your local contiki-ng path
CONTIKI = ../../../../contiki-ng

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC = MAKE_MAC_TSCH
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
