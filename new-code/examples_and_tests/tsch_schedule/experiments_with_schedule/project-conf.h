//
// Created by thomas on 24.06.24.
//

/*
 * LOG_LEVELs: NONE, ERR, WARN, INFO, DBG
 */

#ifndef CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H
#define CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H

/* To show Log messages */
#define LOG_CONF_LEVEL_MAC  LOG_LEVEL_INFO
#define LOG_CONF_LEVEL_6TOP LOG_LEVEL_NONE

/* To not initialize tsch with a 6TiSCH minimal schedule,
 * that would delete our custom schedule. */
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL 0

/* To define the use of different hopping sequences. */
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_16_16 // all possilbe
// #define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_1_1 // 1 hopping sequence, will lead to the use of only on chnnel

/* To use CCA */
#define TSCH_CONF_CCA_ENABLED 0

#endif //CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H