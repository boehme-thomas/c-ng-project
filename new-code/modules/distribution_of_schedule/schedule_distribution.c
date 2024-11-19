//
// Created by thomas on 31.07.24.
//

/**
 * \file Implements functionality to distribute and receive schedule(s) in the network.
 * \author Thomas Böhme
 */

#include "schedule_distribution.h"

#include <stdlib.h>

#include "contiki.h"
#include "net/linkaddr.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

PROCESS(schedule_distribute, "Distribute Schedule");
PROCESS_THREAD(schedule_distribute, ev, data) {

    struct schedule_message *d = (struct schedule_message *)data;

    nullnet_buf = (uint8_t *)d;
    nullnet_len = sizeof(*d);

    PROCESS_BEGIN();

    // is tsch_droadcast_address right?
    NETSTACK_NETWORK.output(&tsch_broadcast_address);
    LOG_INFO("Schedule Sent!\n");

    PROCESS_END();
}

/*
 * Possible changes and extensions:
 *  - implement functionality to set schedule and distribute it further to the network
 */

void set_schedule(struct schedule_data data) {
    struct tsch_slotframe *sf = tsch_schedule_get_slotframe_by_handle(data.slot_frame);

    linkaddr_t partner_addr = linkaddr_cmp(&data.addr_1, &linkaddr_node_addr) ? data.addr_2 : data.addr_1;

    // add new slotframe with respective parameters if no slotframe with the given handle is found
    if (sf == NULL) {
        LOG_INFO("Slotframe handle NULL -> adding new slotframe!\n");
        struct tsch_slotframe *new_sf = tsch_schedule_add_slotframe(data.slot_frame, 7);
        if (new_sf != NULL) {
            tsch_schedule_add_link(new_sf, LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
                                  LINK_TYPE_ADVERTISING, &partner_addr,
                                  data.timeslot, data.channel_offset, 1);
        }

        // if slotframe not NULL
    } else {
        LOG_INFO("Slotframe handle not NULL!\n");
        // if slotframe is found and timeslot is NULL and channel offset is NULL, delete the slotframe
        if (data.timeslot > TSCH_SCHEDULE_MAX_LINKS && data.channel_offset >= 1024) {
            LOG_INFO("timeslot and channle offset NULL -> delete slotframe!\n");
            tsch_schedule_remove_slotframe(sf);
            return;
        }
        struct tsch_link * tl = tsch_schedule_get_link_by_timeslot(sf, data.timeslot);

        // when link exists, assume a new channel offset is given (because the message wouldn't be necessary then)
        // therefore the old was is deleted and a new timeslot is created
        if (tl != NULL) {
            LOG_INFO("Delete old link!\n");
            tsch_schedule_remove_link(sf, tl);

            // if the timeslot is found and the channel offset is NULL delete the existing link in that timeslot
            if (data.channel_offset >= 1024) {
                return;
            }
        }
        LOG_INFO("Add new link with given options!\n");
        tsch_schedule_add_link(sf, LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
                               LINK_TYPE_ADVERTISING, &partner_addr,
                               data.timeslot, data.channel_offset, 1);
    }
}

void distribute_schedule(unsigned int message_count, uint16_t slotframe, uint16_t timeslot, uint16_t channel_offset, linkaddr_t addr_1, linkaddr_t addr_2) {

    struct schedule_data d;
    d.slot_frame = slotframe;
    d.timeslot = timeslot;
    d.channel_offset = channel_offset;
    d.addr_1 = addr_1;
    d.addr_2 = addr_2;
    //linkaddr_copy(d.addr_1, addr_1);
    //linkaddr_copy(d.addr_2, addr_2);
    LOG_INFO_LLADDR(&addr_1);
    LOG_INFO_LLADDR(&d.addr_2);
    if (linkaddr_cmp(&addr_1, &linkaddr_node_addr) || linkaddr_cmp(&addr_2, &linkaddr_node_addr)) {
        set_schedule(d);
    }

    LOG_INFO("Message count %d: \n", message_count);
    struct schedule_message mes = {schedule_message_type, message_count, d};
    void *data = &mes;

    process_start(&schedule_distribute, data);
}


void receive_schedule(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {

    static unsigned int last_seen_message_count = 0;

    if (len == sizeof(struct schedule_message)) {

        struct schedule_message d;
        memcpy(&d, data, sizeof(struct schedule_message));
        LOG_INFO("Message id: %d\n", d.message_id);
        LOG_INFO_LLADDR(&d.data.addr_1);
        LOG_INFO_LLADDR(&d.data.addr_2);
        if (d.message_id > last_seen_message_count) {
            last_seen_message_count = d.message_id;
            distribute_schedule(d.message_id, d.data.slot_frame, d.data.timeslot, d.data.channel_offset, d.data.addr_1, d.data.addr_2);
        }
    }
}