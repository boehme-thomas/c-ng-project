//
// Created by thomas on 01.06.24.
//

/**
 * \file
 *         nullnet with reading linkaddr and print it plus send hello message
 * \author
 *         Thomas Böhme
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "os/dev/leds.h"

#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (10 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"

static linkaddr_t coordinator_addr =  {{ 0xf4, 0xce, 0x36, 0xda, 0xa6, 0xf8, 0x36, 0x2b }};
#endif /* MAC_CONF_WITH_TSCH */


/*---------------------------------------------------------------------------*/
PROCESS(print_link_addr_send_hello, "Print tsch info and send hello message");
AUTOSTART_PROCESSES(&print_link_addr_send_hello);

/* Logs mac address, if coordinator, if associated, broadcast and eb addresses, if mac configured for TSCH. */
void print_tsch_info() {
#if MAC_CONF_WITH_TSCH
    LOG_INFO_("\n\r");
    LOG_INFO("TSCH Infos:\n\r");
    LOG_INFO("my address: ");
    LOG_INFO_LLADDR(&linkaddr_node_addr);
    LOG_INFO_("\n\r");
    LOG_INFO("coordinator: %i, associated: %i, broadcast: ", tsch_is_coordinator, tsch_is_associated);
    LOG_INFO_LLADDR(&tsch_broadcast_address);
    LOG_INFO_(", eb: ");
    LOG_INFO_LLADDR(&tsch_eb_address);
    LOG_INFO_("\n\r");
#endif
}

/* Callback function, when receiving a packet.
 * Log the count and the source address and signals the reception via LED2.
 */
void input_callback(const void *data, uint16_t len,
                    const linkaddr_t *src, const linkaddr_t *dest)
{
    if(len == sizeof(unsigned)) {
        unsigned count;
        memcpy(&count, data, sizeof(count));
        leds_single_on(LEDS_LED2);
        LOG_INFO_("\n\r");
        LOG_INFO("Received count %u from ", count);
        LOG_INFO_LLADDR(src);
        LOG_INFO_("\n\r");
        print_tsch_info();
        leds_single_off(LEDS_LED2);
    }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(print_link_addr_send_hello, ev, data) {

    static struct etimer periodic_timer;
    static struct etimer led_timer;
    static unsigned count = 0;

    PROCESS_BEGIN();

    #if MAC_CONF_WITH_TSCH
        tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
    #endif /* MAC_CONF_WITH_TSCH */

    /* Initialize NullNet */
    nullnet_buf = (uint8_t *)&count;
    nullnet_len = sizeof(count);
    nullnet_set_input_callback(input_callback);


    /* Logs tsch info and messagse which is sent, sends the count to the coordinator. Signals the sending via LED3. */
    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        print_tsch_info();
        tsch_schedule_print();
        etimer_set(&periodic_timer, SEND_INTERVAL);

        LOG_INFO("Sending count %u to ", count);
        LOG_INFO_LLADDR(&coordinator_addr);
        LOG_INFO_("\n\r");

        NETSTACK_NETWORK.output(&coordinator_addr);
        count++;

        leds_single_on(LEDS_LED3);
        etimer_set(&led_timer, CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&led_timer));
        leds_single_off(LEDS_LED3);
    }
    etimer_reset(&periodic_timer);

    PROCESS_END();
}