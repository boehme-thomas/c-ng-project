//
// Created by thomas on 14.08.24.
//

/**
 * \file Header file for gathering, sending and receiving statistics.
 * \author Thomas Böhme
 */

#ifndef CONTIKI_NG_PROJEKTMODUL_GATHER_STATISTICS_H
#define CONTIKI_NG_PROJEKTMODUL_GATHER_STATISTICS_H

#include "net/linkaddr.h"
#include "net/link-stats.h"
#include "../modules_types.h"

/**
 * Representation of a statistic of one node-node pair.
 */
struct statistics {
    enum message_type type;
    unsigned int message_count;
    linkaddr_t request_addr;
    linkaddr_t src_addr;
    struct link_stats l_stats;
};

/**
 * Representation of all statistics from one node.
 * Limited to 2 neighbours for one message, due to packet size restrictions.
 */
struct all_statistics {
    enum message_type type;
    unsigned int message_count;
    linkaddr_t src_addr;
    linkaddr_t addr_of_neighbours[2];
    struct link_stats l_stats_of_neighbours[2];
};

/**
 * Representation of a request message.
 */
struct request_message {
    enum message_type type;
    unsigned int message_count;
    linkaddr_t request_addr;
};

/**
 * \brief Handles reception of statistics at edge node / coordinator
 * @param data Data from received messages (should be like struct statistics)
 * @param len Length of message
 * @param src Address of source node
 * @param dest Address of destination node
 *
 * This function is designed to be used as an input callback function and should be used in e.g. nullnet_set_input_callback.
 * It handles the reception of the statistics and sends it to the server.
 */
void receive_statistics(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

/**
 * \brief Handles reception of an all_statistics request
 * @param data Data from received messages (should be like struct all_statistics)
 * @param len Length of message
 * @param src Address of source node
 * @param dest Address of destination node
 *
 * This function is designed to be used as an input callback function and should be used in e.g. nullnet_set_input_callback.
 * It handles the reception of all_statistics and sends it to the server.
 */
void receive_all_statistics(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

/**
 * \brief Handles the request of statistics
 * @param address Address of the node, from which the statistic is requested
 * @param all_statistics Signals whether one node send its statistics, or the network is asked about a specific node
 *
 * A request is send to gather statistics about the node with the respective address.
 * If all_statistics is ture, the node should send the statistics of all its neighbours, if not, every node is asked to
 * send the statistics of the given address.
 */
void request_statistics(linkaddr_t address, uint8_t all_statistics);

/**
 * \brief Handles reception of request of statistics of a node from its neighbours
 * @param data Data from received message (should be like linkaddr_t)
 * @param len Length of message
 * @param src Address of source node
 * @param dest Address of destination node
 *
 * This function is designed to be used as an input callback function and should be used in e.g. nullnet_set_input_callback.
 * It handles the reception of the request of statistics and send the response back to the edge node.
 * A receiving node checks if address is in its neighbour table and then sends the statistics back.
 */
void receive_statistics_request(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

/**
 * \brief Handles reception of request of all statistics of a node
 * @param data Data from received message
 * @param len Length of message
 * @param src Address of source node
 * @param dest Address of destination node
 *
 * This function is designed to be used as an input callback function and should be used in e.g. nullnet_set_input_callback.
 * It handles the reception of the request of statistics and send the response back to the edge node.
 * It collects all of the neighbour statistics of a single node and send them back (in contrast to
 * receive_statistics_request, where the neighbours send the statistics of the searched node)
 */
void receive_all_statistics_request(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

#endif //CONTIKI_NG_PROJEKTMODUL_GATHER_STATISTICS_H
