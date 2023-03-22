/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <types.h>

#include "ec.h"
#include "mainboard.h"
#include "vpd.h"

const union emi_eeprom_vpd *get_emi_eeprom_vpd(void)
{
	static union emi_eeprom_vpd vpd = {0};

	/* Check if cached VPD is valid */
	if (vpd.header.revision == VPD_LATEST_REVISION)
		return &vpd;

	ec_emi_read(vpd.raw, EMI_0_IO_BASE_ADDR, 0, 0, sizeof(vpd.raw));

	/* If the magic value doesn't match, consider EEPROM VPD unreliable */
	if (vpd.header.magic != VPD_MAGIC) {
		printk(BIOS_WARNING, "Atlas VPD: Bad magic value, using fallback defaults\n");
		vpd.header.revision = 0;
	} else {
		printk(BIOS_DEBUG, "Atlas VPD: Got revision %u from EC\n", vpd.header.revision);
	}

	/*
	 * For backwards compatibility, if the VPD from the EC is an older
	 * version, uprev it to the latest version coreboot knows about by
	 * filling in the remaining fields with default values. Should the
	 * EC provide a newer VPD revision, coreboot would downgrade it to
	 * the latest version it knows about as the VPD layout is designed
	 * to be backwards compatible.
	 *
	 * Whenever the value of `VPD_LATEST_REVISION` is incremented, add
	 * a new `case` label just before the `default` label that matches
	 * the second latest revision to initialise the newly-added fields
	 * of the VPD structure with a reasonable fallback value. Note the
	 * intentional falling through.
	 */
	switch (vpd.header.revision) {
	case 0:
		memset(vpd.raw, 0, sizeof(vpd.raw));
		vpd.header.magic = VPD_MAGIC;
		vpd.serial_number[0] = '\0';
		vpd.part_number[0] = '\0';
		vpd.profile = ATLAS_PROF_UNPROGRAMMED;
		__fallthrough;
	default:
		/* Ensure serial numbers are NULL-terminated, update revision last */
		vpd.serial_number[ATLAS_SN_PN_LENGTH - 1] = '\0';
		vpd.part_number[ATLAS_SN_PN_LENGTH - 1] = '\0';
		vpd.header.revision = VPD_LATEST_REVISION;
		break;
	}

	return &vpd;
}
