//
// Created by thomas on 31.07.24.
//

/**
 * \file Header file for distributing and receiving schedule in the network.
 * \author Thomas Böhme
 */

#ifndef CONTIKI_NG_PROJEKTMODUL_SCHEDULE_DISTRIBUTION_H
#define CONTIKI_NG_PROJEKTMODUL_SCHEDULE_DISTRIBUTION_H

#include "net/linkaddr.h"
#include "../modules_types.h"

/**
 * Struct to define the data of a schedule, that should be sent
 */
struct schedule_data {
    uint16_t slot_frame;
    uint16_t timeslot;
    uint16_t channel_offset;
    linkaddr_t addr_1;
    linkaddr_t addr_2;
};

/**
 * Struct to define a schedule message
 */
struct schedule_message {
    enum message_type type;
    unsigned int message_id;
    struct schedule_data data;
};

/**
 * \brief Handles sending of schedule.
 * \param slotframe The slot frame handle
 * \param timeslot The timeslot, also cell
 * \param channel_offset The channel offset
 * \param addr_1 One of the receiver addresses
 * \param addr_2 The other receiver address
 *
 * Handles the sending of a schedule into the network.
 */
void distribute_schedule(unsigned int message_count, uint16_t slotframe, uint16_t timeslot, uint16_t channel_offset, linkaddr_t addr_1, linkaddr_t addr_2);

/**
 * \brief Handles reception of schedule.
 * \param data Data from received message (should be of type schedule_data)
 * \param len Length of message
 * \param src Address of source node
 * \param dest Address of destination node
 *
 * This function is designed to be used as an input callback function and should be used in e.g. nullnet_set_input_callback.
 * It either handles the new schedule or sends it to the next node.
 * When it is not send to the next node, following rules are applied (so data must be send in the correct form to prevent
 * misconfigurations):\n
 *  - if no slotframe is found a new slotframe is added with the respective following parameters\n
 *  - if slotframe is found and timeslot == NULL and channel_offset == NULL, then the existing slotframe is deleted\n
 *  - if slotframe is found and timeslot is found and channel_offset is not NULL, the timeslot is deleted and a new link is created with the parameters\n
 *  - if slotframe is found and timeslot is found and channel_offset == NULL, then the existing timeslot is deleted\n
 */
void receive_schedule(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);


#endif //CONTIKI_NG_PROJEKTMODUL_SCHEDULE_DISTRIBUTION_H
