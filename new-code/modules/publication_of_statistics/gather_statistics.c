//
// Created by thomas on 14.08.24.
//

/**
 * \file Implements functionality to gather, sending receiving statistics.
 * \author Thomas Böhme
 */

#include "../arch/cpu/nrf52840/usb/usb-serial.h"
#include "gather_statistics.h"
#include "net/linkaddr.h"
#include "net/link-stats.h"
#include <stdlib.h>
#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/mac/tsch/tsch.h"

#include "nbr-table.h"
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/*
 * Possible changes and extensions:
 * - sending statistics requests and answers not on the broadcast address, but to the node directly (when e.g. a schedule is already set)
 * - fixing: when nodes rebroadcast statistics / request it comes to collision (atm commented out)
 *      - e.g. for not edge nodes: same timeslot for listening to broadcast, different timeslot for sending broadcast
 * - functionality for the edge node to log a statistics_request
 * - sending data via usb and not via LOG_ (is currently not supported by the server module)
 */

void log_all_statistics_edge_node();


PROCESS(sending_statistics, "Send Statistics");
PROCESS_THREAD(sending_statistics, ev, data) {

    enum message_type *type = (enum message_type *)data;
    if (*type == statistics_all_type) {
        struct all_statistics *stats = (struct all_statistics *)data;
        nullnet_buf = (uint8_t *)stats;
        nullnet_len = sizeof(*stats);
    } else if (*type == statistics_type) {
        struct statistics *stats = (struct statistics *)data;
        nullnet_buf = (uint8_t *)stats;
        nullnet_len = sizeof(*stats);
    }
    PROCESS_BEGIN();

    NETSTACK_NETWORK.output(&tsch_broadcast_address);

    //LOG_INFO("Statistics Sent!\n");

    PROCESS_END();
}

PROCESS(process_request_statistics, "Request Statistics");
PROCESS_THREAD(process_request_statistics, ev, data) {

    struct request_message *req = (struct request_message *)data;

    nullnet_buf = (uint8_t *)req;
    nullnet_len = sizeof(*req);

    PROCESS_BEGIN();

    NETSTACK_NETWORK.output(&tsch_broadcast_address);

    //LOG_INFO("Request Sent!\n");

    PROCESS_END();
}

void request_statistics(linkaddr_t address, uint8_t all_statistics) {
    static unsigned int message_count = 0;
    ++message_count;
    struct request_message req;

    if (all_statistics == 1) {
        if (tsch_is_coordinator & linkaddr_cmp(&address, &linkaddr_node_addr)) {
            // log statistics directly if requested node is edge node
            log_all_statistics_edge_node();
        }
        req.type = request_all_message_type;
    } else {
        req.type = request_message_type;
    }

    req.message_count = message_count;
    req.request_addr = address;
    process_start(&process_request_statistics, (void *)&req);

}

void receive_statistics_request(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    static unsigned int message_count = 0;
    if (len == sizeof(struct request_message)) {
        struct request_message msg;
        memcpy(&msg, data, sizeof(struct request_message));

        if (msg.message_count > message_count) {
            LOG_INFO("Statistics request received!\n");
            message_count = msg.message_count;
            // for rebroadcasting statistics_request
            // process_start(&process_request_statistics, (void *) &msg);

            // stop if the statistics of us is requested
            LOG_INFO("My address: ");
            LOG_INFO_LLADDR(&linkaddr_node_addr);
            LOG_INFO_("\n");
            LOG_INFO("Requested address: ");
            LOG_INFO_LLADDR(&msg.request_addr);
            LOG_INFO_("\n");
            if (linkaddr_cmp(&msg.request_addr, &linkaddr_node_addr)) {
                return;
            }

            LOG_INFO("Searching for neighbour and gathering statistics...\n");
            const struct link_stats *link_statistics = link_stats_from_lladdr(&msg.request_addr);

            if (link_statistics == NULL) {
                LOG_INFO("No statistics found...\n");
                return;
            } else {
                LOG_INFO("Statistics found. Trying to make struct\n");
                struct statistics stats;
                stats.type = statistics_type;
                stats.message_count = msg.message_count;
                stats.request_addr = msg.request_addr;
                stats.src_addr = linkaddr_node_addr;
                stats.l_stats = *link_statistics;

                LOG_INFO("message: count: %d, type: %d, request addr: ", stats.message_count, stats.type);
                LOG_INFO_LLADDR(&stats.request_addr);
                LOG_INFO_(", src addr: ");
                LOG_INFO_LLADDR(&stats.src_addr);
                LOG_INFO_(", lstats: last_tx_time: %ld, etx: %d, rssi: %d, freshness: %d, tx_count: %d, ack_count: %d\n", stats.l_stats.last_tx_time, stats.l_stats.etx, stats.l_stats.rssi, stats.l_stats.freshness, stats.l_stats.tx_count, stats.l_stats.ack_count);

                // LOG_INFO("Address after stats:");
                // LOG_INFO_LLADDR(&stats.req->request_addr);
                // LOG_INFO_("\n");
                process_start(&sending_statistics, (void *) &stats);
            }
        }
    }
}

/**
* \brief Does the same as receive_all_statistics_request, but it logs the data from the edge node directly
*/
void log_all_statistics_edge_node() {
    static linkaddr_t no_addr_addr =  {{ 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 }};

    nbr_table_key_t *key_head = nbr_table_key_head();
    int entries = nbr_table_count_entries();

    int max_entries = 2;
    int needed_entries = entries + (entries % max_entries);

    for (int i = 0; i < needed_entries; ++i) {
        int mod = i % max_entries;
        if (mod == 0) {
            LOG_INFO_("\n");
            LOG_INFO_("%d %d\n", statistics_all_type, 0);
            LOG_INFO_LLADDR(&linkaddr_node_addr);
            LOG_INFO_("\n");
        }

        if (i < entries) {
            linkaddr_t *link_addr = &(key_head->lladdr);
            const struct link_stats *link_statistics = link_stats_from_lladdr(link_addr);
            if (link_statistics != NULL) {

                LOG_INFO_LLADDR(link_addr);

                LOG_INFO_(" %ld %d %d %d %d %d\n", link_statistics->last_tx_time, link_statistics->etx,
                          link_statistics->rssi, link_statistics->freshness,
                          link_statistics->tx_count, link_statistics->ack_count);
            } else {
                LOG_INFO_LLADDR(link_addr);
                LOG_INFO_(" %d %d %d %d %d %d\n", 0, 0, 0, 0, 0, 0);
            }
            key_head = nbr_table_key_next(key_head);
        } else {
            LOG_INFO_LLADDR(&no_addr_addr);
            LOG_INFO_(" %d %d %d %d %d %d\n", 0, 0, 0, 0, 0, 0);
        }
    }
}

void receive_all_statistics_request(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    static unsigned int message_count = 0;
    if (len == sizeof(struct request_message)) {
        struct request_message msg;
        memcpy(&msg, data, sizeof(struct request_message));
        static linkaddr_t no_addr_addr =  {{ 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 }};

        if (msg.message_count > message_count) {
            LOG_INFO("Statistics request_all received!\n");
            message_count = msg.message_count;
            if (linkaddr_cmp(&msg.request_addr, &linkaddr_node_addr)) {
                nbr_table_key_t *key_head = nbr_table_key_head();
                int entries = nbr_table_count_entries();

                struct all_statistics stats;
                stats.type = statistics_all_type;
                stats.message_count = msg.message_count;
                stats.src_addr = msg.request_addr;

                int max_entries = 2;
                int needed_entries = entries + (entries % max_entries);

                for (int i = 0; i < needed_entries; ++i) {
                    int mod = i % max_entries;
                    if (i < entries) {
                        linkaddr_t *link_addr = &(key_head->lladdr);
                        const struct link_stats *link_statistics = link_stats_from_lladdr(link_addr);
                        if (link_statistics != NULL) {
                            stats.l_stats_of_neighbours[mod] = *link_statistics;
                            stats.addr_of_neighbours[mod] = *link_addr;
                        } else {
                            stats.l_stats_of_neighbours[mod].last_tx_time = 0;
                            stats.l_stats_of_neighbours[mod].etx = 0;
                            stats.l_stats_of_neighbours[mod].rssi = 0;
                            stats.l_stats_of_neighbours[mod].freshness = 0;
                            stats.l_stats_of_neighbours[mod].tx_count = 0;
                            stats.l_stats_of_neighbours[mod].ack_count = 0;
                            stats.addr_of_neighbours[mod] = *link_addr;
                        }
                        key_head = nbr_table_key_next(key_head);
                    } else {
                        stats.l_stats_of_neighbours[mod].last_tx_time = 0;
                        stats.l_stats_of_neighbours[mod].etx = 0;
                        stats.l_stats_of_neighbours[mod].rssi = 0;
                        stats.l_stats_of_neighbours[mod].freshness = 0;
                        stats.l_stats_of_neighbours[mod].tx_count = 0;
                        stats.l_stats_of_neighbours[mod].ack_count = 0;
                        stats.addr_of_neighbours[mod] = no_addr_addr;
                    }

                    LOG_INFO("message: count: %d, type: %d, src addr: ", stats.message_count,
                             stats.type);
                    LOG_INFO_LLADDR(&stats.src_addr);
                    LOG_INFO_(", addr fo neighbour: ");
                    LOG_INFO_LLADDR(&stats.addr_of_neighbours[mod]);
                    LOG_INFO_(
                            ", lstats: last_tx_time: %ld, etx: %d, rssi: %d, freshness: %d, tx_count: %d, ack_count: %d\n",
                            stats.l_stats_of_neighbours[mod].last_tx_time, stats.l_stats_of_neighbours[mod].etx,
                            stats.l_stats_of_neighbours[mod].rssi, stats.l_stats_of_neighbours[mod].freshness,
                            stats.l_stats_of_neighbours[mod].tx_count, stats.l_stats_of_neighbours[mod].ack_count);
                    LOG_INFO("size of stats: %d\n", sizeof(stats));

                    if (mod == 1) {
                        struct all_statistics stats_copy = stats;
                        process_start(&sending_statistics, (void *) &stats_copy);
                    }
                }
            } else {
                LOG_INFO("Not my address!\n");
                // for rebroadcasting the all_statistics_request if receiving node is not requested node
                //process_start(&process_request_statistics, (void *) &msg);
            }
        }
    }
}

void receive_all_statistics(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    // static unsigned int last_seen_message_count = 0;
    enum message_type *type = (enum message_type *)data;
    if (len == sizeof(struct all_statistics) && *type == statistics_all_type) {
        struct all_statistics stats;
        memcpy(&stats, data, sizeof(struct all_statistics));

        if (!tsch_is_coordinator) {
            // make sure to send it, but how to not flood the network / how to check if packet has already been seen
            // send to next hop
            // last_seen_message_count = stats.req_message.message_count;
            // process_start(&sending_statistics, (void *)&stats);
            return;
        }

        //LOG_INFO("All_statistics Received from ");
        LOG_INFO_("\n");
        LOG_INFO_("%d %d\n", stats.type, stats.message_count);
        LOG_INFO_LLADDR(&stats.src_addr);
        LOG_INFO_("\n");
        for (int i = 0; i < 2; ++i) {
            //LOG_INFO("Addr of neighbour:");
            LOG_INFO_LLADDR(&stats.addr_of_neighbours[i]);

            //uint32_t buffer_size = len;
            LOG_INFO_(" %ld %d %d %d %d %d\n",
                      stats.l_stats_of_neighbours[i].last_tx_time, stats.l_stats_of_neighbours[i].etx,
                      stats.l_stats_of_neighbours[i].rssi, stats.l_stats_of_neighbours[i].freshness,
                      stats.l_stats_of_neighbours[i].tx_count, stats.l_stats_of_neighbours[i].ack_count);
        }
        /*
        // for using serial usb and not LOG_ (does not work together with the current version of the server module)
        usb_serial_init();

        uint8_t buffer[sizeof(struct all_statistics)];
        LOG_INFO("sizeof buffer %d\n", sizeof(buffer));
        memcpy(buffer, &stats, sizeof(struct all_statistics));

        usb_serial_flush();
        for (int i = 0; i < len; i++) {
            usb_serial_writeb(*(buffer + i));
        }
        usb_serial_flush();
        */
    }
}

void receive_statistics(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    enum message_type *type = (enum message_type *)data;
    if (len == sizeof(struct statistics) && *type == statistics_type) {
        struct statistics stats;
        memcpy(&stats, data, sizeof(struct statistics));

        if (!tsch_is_coordinator) {
            // make sure to send it, but how to not flood the network / how to check if packet has already been seen
            // send to next hop
            // last_seen_message_count = stats.req_message.message_count;
            // process_start(&sending_statistics, (void *)&stats);
            return;
        }

        LOG_INFO("Statistics Received from ");
        LOG_INFO_LLADDR(&stats.src_addr);
        LOG_INFO_(" of ");
        LOG_INFO_LLADDR(&stats.request_addr);
        LOG_INFO_(".\n Stats:  message: count: %d, type: %d,", stats.message_count, stats.type);
        LOG_INFO_(" lstats: last_tx_time: %ld, etx: %d, rssi: %d, freshness: %d, tx_count: %d, ack_count: %d.\n", stats.l_stats.last_tx_time, stats.l_stats.etx, stats.l_stats.rssi, stats.l_stats.freshness, stats.l_stats.tx_count, stats.l_stats.ack_count);

        LOG_INFO_LLADDR(&stats.src_addr);
        LOG_INFO_(" ");
        LOG_INFO_LLADDR(&stats.request_addr);
        LOG_INFO_(" %ld %d %d %d %d %d\n", stats.l_stats.last_tx_time, stats.l_stats.etx, stats.l_stats.rssi, stats.l_stats.freshness, stats.l_stats.tx_count, stats.l_stats.ack_count);

        /*
        // for using serial usb and not LOG_ (does not work together with the current version of the server module)
         usb_serial_init();

        uint8_t *usb_buffer = (unsigned char *)data;
        for (int i = 0; i < len; ++i) {
            usb_serial_writeb(*usb_buffer);
            usb_buffer += i;
            usb_serial_flush();
        }
        LOG_INFO_("\n");
        // usb_serial_writeb(*usb_buffer);
        //usb_serial_writeb(*stats);
         */
    }
}
