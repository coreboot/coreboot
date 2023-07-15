/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "pei_data.h"

#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_RESET		1
#define BOOT_PATH_RESUME	2

void mainboard_fill_pei_data(struct pei_data *pei_data);

/*
 * SPD information API adopted from nb/intel/haswell.
 *
 * This applies to both MRC and native raminit, only for boards with
 * CONFIG(HAVE_SPD_IN_CBFS).
 *
 * spd_info.addresses is an array of 4 bytes representing the SMBus addresses
 * of the SPD EEPROM of (respectively) Channel 0 Slot 0, C0S1, C1S0, C1S1.
 * Boards with onboard memory for the slot without actual SPD EEPROM enter
 * SPD_MEMORY_DOWN in that position and enter in spd_info.spd_index a 0-based index into
 * spd.bin file in CBFS, which is a concatenation of 256-byte SPD data blobs.
 *
 * Only one set of SPD data is supported.
 */

#define SPD_MEMORY_DOWN 0xFF

struct spd_info {
	uint8_t addresses[4];
	unsigned int spd_index;
};

/*
 * Mainboard callback to fill in the SPD addresses.
 *
 * @param spdi Pointer to spd_info struct to be populated by mainboard.
 */
void mb_get_spd_map(struct spd_info *spdi);

#endif /* RAMINIT_H */
