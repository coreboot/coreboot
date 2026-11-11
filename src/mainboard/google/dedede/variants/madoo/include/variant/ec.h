/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_EC_H
#define MAINBOARD_EC_H

#include <baseboard/ec.h>

/* Enable Tablet switch for Windows drivers */
#if !CONFIG(CHROMEOS)
#define EC_ENABLE_TBMC_DEVICE
#endif

#endif
