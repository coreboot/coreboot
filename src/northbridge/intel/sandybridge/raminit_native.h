/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_NATIVE_H
#define RAMINIT_NATIVE_H

#include "sandybridge.h"
#include <device/dram/ddr3.h>

/* The order is: ch0dimmA, ch0dimmB, ch1dimmA, ch1dimmB */
void read_spd(spd_raw_data *spd, u8 addr, bool id_only);
void mainboard_get_spd(spd_raw_data *spd, bool id_only);

#endif /* RAMINIT_NATIVE_H */
