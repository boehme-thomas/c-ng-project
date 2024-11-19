//
// Created by thomas on 04.06.24.
//

/**
 * \file
 *         The final project, for usage with the statistics module and the server. Used for the dk boards.
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

#include "../modules/distribution_of_schedule/schedule_distribution.h"
#include "../modules/publication_of_statistics/gather_statistics.h"
#include "../modules/modules_types.h"

/* Configuration */
#define SEND_INTERVAL (10 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"
#endif /* MAC_CONF_WITH_TSCH */

static linkaddr_t coordinator_addr =  {{ 0xf4, 0xce, 0x36, 0xda, 0xa6, 0xf8, 0x36, 0x2b }};

/*---------------------------------------------------------------------------*/
PROCESS(final_example, "Final running example");
AUTOSTART_PROCESSES(&final_example);

void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    enum message_type *type = (enum message_type *)data;
    switch (*type) {
        case statistics_type:
            receive_statistics(data, len, src, dest);
            break;
        case statistics_all_type:
            receive_all_statistics(data, len, src, dest);
            break;
        case request_message_type:
            receive_statistics_request(data, len, src, dest);
            break;
        case request_all_message_type:
            receive_all_statistics_request(data, len, src, dest);
            break;
        case schedule_message_type:
            receive_schedule(data, len, src, dest);
            break;
        default:
            return;
    }
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(final_example, ev, data) {

    static struct etimer periodic_timer;
    static struct etimer led_timer;
    static struct statistics stats;
    //static struct schedule_message sched_data;
    static struct request_message request;

    PROCESS_BEGIN();

    struct tsch_slotframe *new_sf = tsch_schedule_add_slotframe(0, 7);
    tsch_schedule_add_link(new_sf, LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
                            LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
                            0, 0, 1);

    if (linkaddr_cmp(&linkaddr_node_addr, &coordinator_addr)) {
        tsch_set_coordinator(1);
        nullnet_buf = (uint8_t *)&stats;
        nullnet_len = sizeof(stats);

    } else {

        nullnet_buf = (uint8_t *)&request;
        nullnet_len = sizeof(request);
    }

    nullnet_set_input_callback(input_callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        etimer_set(&periodic_timer, SEND_INTERVAL);

        if (!tsch_is_associated) {
            leds_single_on(LEDS_LED2);
        } else {
            leds_single_on(LEDS_LED3);
        }

        etimer_set(&led_timer, CLOCK_SECOND);

        PROCESS_WAIT_EVENT_UNTIL (etimer_expired(&led_timer));

        leds_single_off(LEDS_LED2);
        leds_single_off(LEDS_LED3);

    }

    etimer_reset(&periodic_timer);

    PROCESS_END();
}