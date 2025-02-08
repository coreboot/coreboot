/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_HASWELL_RAMINIT_H
#define NORTHBRIDGE_INTEL_HASWELL_RAMINIT_H

#include <types.h>
#include "chip.h"

#define SPD_MEMORY_DOWN	0xff

struct spd_info {
	uint8_t addresses[4];
	unsigned int spd_index;
};

/* Mainboard callback to fill in the SPD addresses */
void mb_get_spd_map(struct spd_info *spdi);

void get_spd_info(struct spd_info *spdi, const struct northbridge_intel_haswell_config *cfg);
void perform_raminit(const bool s3resume);

#endif	/* NORTHBRIDGE_INTEL_HASWELL_RAMINIT_H */
