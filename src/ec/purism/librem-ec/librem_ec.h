/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LIBREM_EC_H
#define LIBREM_EC_H

#include <stdbool.h>

/*
 * Check whether librem-ec has working jack detect.  This was fixed in librem-ec
 * 1.13, so we only use the verbs with jack detect if the EC has been updated.
 */
bool librem_ec_has_jack_detect(void);

#endif
