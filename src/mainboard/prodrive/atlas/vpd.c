/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include "ec.h"
#include "mainboard.h"
#include "vpd.h"

static void write_invalid_str(char *dest, size_t length)
{
	snprintf(dest, length, "%s", "INVALID");
}

const struct emi_eeprom_vpd *get_emi_eeprom_vpd(void)
{
	static union {
		struct emi_eeprom_vpd layout;
		uint8_t raw[sizeof(struct emi_eeprom_vpd)];
	} vpd = {0};

	/* Check if cached VPD is valid */
	if (vpd.layout.header.revision == VPD_LATEST_REVISION)
		return &vpd.layout;

	ec_emi_read(vpd.raw, EMI_0_IO_BASE_ADDR, 0, 0, sizeof(vpd.raw));

	/* If the magic value doesn't match, consider EEPROM VPD unreliable */
	if (vpd.layout.header.magic != VPD_MAGIC) {
		printk(BIOS_WARNING, "Atlas VPD: Bad magic value, using fallback defaults\n");
		vpd.layout.header.revision = 0;
	} else {
		printk(BIOS_DEBUG, "Atlas VPD: Got revision %u from EC\n",
				vpd.layout.header.revision);
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
	switch (vpd.layout.header.revision) {
	case 0:
		memset(vpd.raw, 0, sizeof(vpd.raw));
		vpd.layout.header.magic = VPD_MAGIC;
		write_invalid_str(vpd.layout.serial_number, sizeof(vpd.layout.serial_number));
		write_invalid_str(vpd.layout.part_number, sizeof(vpd.layout.part_number));
		vpd.layout.profile = ATLAS_PROF_UNPROGRAMMED;
		__fallthrough;
	default:
		/* Ensure serial numbers are NULL-terminated, update revision last */
		vpd.layout.serial_number[ATLAS_SN_PN_LENGTH - 1] = '\0';
		vpd.layout.part_number[ATLAS_SN_PN_LENGTH - 1] = '\0';
		vpd.layout.header.revision = VPD_LATEST_REVISION;
		break;
	}

	return &vpd.layout;
}
