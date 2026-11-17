/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __VARIANT_EC_H__
#define __VARIANT_EC_H__

#include <baseboard/ec.h>

/* Enable Tablet switch for Windows drivers */
#if !CONFIG(CHROMEOS)
#define EC_ENABLE_TBMC_DEVICE
#endif

#endif
