// Created by thomas on 19.05.24.
//


/**
 * \file
 *         nullnet with sending message to neighbour
 * \author
 *         Thomas Böhme
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

#include <string.h>
#include <stdlib.h> /* For rand() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (10 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-queue.h"
// how to change this to a specific address withouth knowing the addresses of the network
// --> it's ok to set that
static linkaddr_t coordinator_addr =  {{ 0xf4, 0xce, 0x36, 0xda, 0xa6, 0xf8, 0x36, 0x2b }};
#endif /* MAC_CONF_WITH_TSCH */

#include "nbr-table.h" /* For neighbours */
#include "os/dev/leds.h" /* For leds */


/*---------------------------------------------------------------------------*/
PROCESS(send_to_neighbor, "Nullnet with sending messages to neighbor");
AUTOSTART_PROCESSES(&send_to_neighbor);

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
 * Logs the count and the source address and signals the reception via LED2.
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
PROCESS_THREAD(send_to_neighbor, ev, data)
{
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

    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        print_tsch_info();

        /* get number of entries in neighbour table and log them */
        int entries = nbr_table_count_entries();
        LOG_INFO_("\n\r");
        LOG_INFO("Number of entries: %u\n\r", entries);
        LOG_INFO("Neighbours:\n\r");

        /* - gets table_keys, which contain the link-layer address of a neighbour,
         * - checks if link-layer address has tsch_neighbor (via tsch_queue_get_nbr),
         * - if that's true, then the lladdress is logged and if the queue is empty and the packet count of that queue
         * - for each neighbor in the neighbor table
         */
        nbr_table_key_t *key_head = nbr_table_key_head();
        while (key_head != NULL) {
            LOG_INFO(" - ");
            linkaddr_t *addr = &(key_head->lladdr);
            if (addr != NULL) {
                struct tsch_neighbor *tsch_nbr = tsch_queue_get_nbr(addr);
                if (tsch_nbr != NULL) {
                    LOG_INFO_LLADDR(addr);
                    LOG_INFO_("\n\r");

                    int is_tsch = tsch_queue_is_empty(tsch_nbr);
                    LOG_INFO("    Is empty: %d\n\r", is_tsch);
                    int packet_count = tsch_queue_nbr_packet_count(tsch_nbr);
                    LOG_INFO("    Packet count in queue: %d.\n\r", packet_count);
                } else {
                    LOG_INFO_("TSCH Neighbour is NULL --> therefore no queue is available\n\r");
                }
            } else {
                LOG_INFO_("NBR Neighbour is NULL\n\r");
            }
            key_head = nbr_table_key_next(key_head);
        }

        /*
         * select neighbour:
         * - gets random value in range of number of neighbors (without broadcast and eb)
         * - select that neighbor
         */
        entries = entries - 2;
        if (entries > 0) {
            int rand_val = rand() % (entries);
            LOG_INFO("rand_val: %d\n\r", rand_val);
            nbr_table_key_t *key_head_new = nbr_table_key_head();
            // key_head_new = nbr_table_key_next(key_head_new);
            key_head_new = nbr_table_key_next(key_head_new);
            while (rand_val >= 0) {
                key_head_new = nbr_table_key_next(key_head_new);
                --rand_val;
            }
            linkaddr_t *address = &(key_head_new->lladdr);

            /*
             * checks if selected neighbor is available and logs its lladdress and the message to send,
             * truns LED3 on for a second
             */
            if (address != NULL) {
                struct tsch_neighbor *tsch_nbr = tsch_queue_get_nbr(address);
                if (tsch_nbr != NULL) {
                    etimer_set(&periodic_timer, SEND_INTERVAL);
                    LOG_INFO("Selected neighbour is: ");
                    LOG_INFO_LLADDR(address);
                    LOG_INFO_("\n\r");
                    LOG_INFO("Sending count %u to ", count);
                    LOG_INFO_LLADDR(address);
                    LOG_INFO_("\n\r");
                    NETSTACK_NETWORK.output(address);
                    count++;

                    // turn on led3 for 1 sec
                    leds_single_on(LEDS_LED3);
                    etimer_set(&led_timer, CLOCK_SECOND);
                    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&led_timer));
                    leds_single_off(LEDS_LED3);

                } else {
                    LOG_INFO("Selected TSCH neighbour is NULL.\n\r");
                }
            }
        } else {
            LOG_INFO("No neighbour is available to send messages to.\n\r");
        }

        /* get time neighbor*/
        /*
        struct tsch_neighbor *t_nbr = tsch_queue_get_time_source();
        // when there is a time neighbour then send it a message with count as content
        if (t_nbr != NULL) {
            LOG_INFO("Time Neighbour available!");
            LOG_INFO_("\n");
            linkaddr_t *l_nbr = tsch_queue_get_nbr_address(t_nbr);
            etimer_set(&periodic_timer, SEND_INTERVAL);

            LOG_INFO("Sending count %u to ", count);
            LOG_INFO_LLADDR(l_nbr);
            LOG_INFO_("\n");

            print_tsch_info();

            NETSTACK_NETWORK.output(l_nbr);
            count++;

        // if there is no time neighbor, log
        } else {
            LOG_INFO("No time neighbor available!\n");

        }
        */
        // tsch_queue_free_unused_neighbors();

        etimer_reset(&periodic_timer);
    }

    PROCESS_END();
}
