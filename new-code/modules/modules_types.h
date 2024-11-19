//
// Created by thomas on 23.09.24.
//

/**
 * \file Header file for the declaration of variables, used in different module files.
 * \author Thomas Böhme
 */

#ifndef CONTIKI_NG_PROJEKTMODUL_MODULES_TYPES_H
#define CONTIKI_NG_PROJEKTMODUL_MODULES_TYPES_H

/**
 * Enum for different message types.
 */
enum message_type {
    statistics_type,
    statistics_all_type,
    request_message_type,
    request_all_message_type,
    schedule_message_type
};

#endif //CONTIKI_NG_PROJEKTMODUL_MODULES_TYPES_H
