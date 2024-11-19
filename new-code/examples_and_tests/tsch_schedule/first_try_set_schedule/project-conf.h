//
// Created by thomas on 19.05.24.
//

/*
 * LOG_LEVELs: NONE, ERR, WARN, INFO, DBG
 */

#ifndef CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H
#define CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H

/* To show Log messages */
#define LOG_CONF_LEVEL_MAC  LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_6TOP LOG_LEVEL_NONE

/* To not initialize tsch with a 6TiSCH minimal schedule,
 * that would delete our custom schedule. */
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL 0

#endif //CONTIKI_NG_PROJEKTMODUL_PROJECT_CONF_H


