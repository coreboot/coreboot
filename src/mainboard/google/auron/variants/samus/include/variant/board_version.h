/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SAMUS_BOARD_VERSION_H
#define SAMUS_BOARD_VERSION_H

#include <boardid.h>

#define SAMUS_EC_BOARD_VERSION_EVT1	3
#define SAMUS_EC_BOARD_VERSION_EVT2	4
#define SAMUS_EC_BOARD_VERSION_EVT3	5
#define SAMUS_EC_BOARD_VERSION_EVT4	0
#define SAMUS_EC_BOARD_VERSION_UNKNOWN	BOARD_ID_UNKNOWN

const char *samus_board_version(void);

#endif
