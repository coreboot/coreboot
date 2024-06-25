/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_DIMM_SLOT_H_
#define _SOC_DIMM_SLOT_H_

#include <stdint.h>

struct dimm_slot_config {
	uint8_t socket;
	uint8_t channel;
	uint8_t dimm;
	/*
	 * Refer to DSP0134_3.6.0.pdf
	 * page 103 'Device Locator', 'Bank Locator' and 'Asset Tag'.
	 */
	const char *dev_locator;
	const char *bank_locator;
	const char *asset_tag;
};

#define DIMM_SLOT_EQUAL(dimm_slot, s, c, d) (\
	(dimm_slot.socket == s) &&\
	(dimm_slot.channel == c) &&\
	(dimm_slot.dimm == d))

const struct dimm_slot_config *get_dimm_slot_config_table(int *size);

#endif /* _SOC_DIMM_SLOT_H_ */
