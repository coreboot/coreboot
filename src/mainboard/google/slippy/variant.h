/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <types.h>

unsigned int variant_get_spd_index(void);
bool variant_is_dual_channel(const unsigned int spd_index);

#endif
