/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ATLAS_VPD_H
#define ATLAS_VPD_H

/*
 * The Atlas COMe module stores some non-volatile vital product data in
 * an EC-attached I2C EEPROM. The EC firmware reads the EEPROM contents
 * and provides them to the host via EMI (Embedded Memory Interface) 0.
 */

#include <types.h>

#define VPD_MAGIC	0x56504453  /* 'VPDS' */

/*
 * Increment this value whenever new fields are added to the structures.
 * Furthermore, adapt the `get_emi_eeprom_vpd()` function accordingly to
 * provide fallback values for newly-added fields.
 */
#define VPD_LATEST_REVISION	1

struct __packed emi_eeprom_vpd_header {
	uint32_t magic;
	uint8_t revision;
	uint8_t _rfu[15];	/* Reserved for Future Use */
};

/* For backwards compatibility reasons, do NOT reuse enum values! */
enum atlas_profile {
	ATLAS_PROF_UNPROGRAMMED		= 0,	/* EEPROM not initialised */
	ATLAS_PROF_DEFAULT		= 1,
	ATLAS_PROF_REALTIME_PERFORMANCE	= 2,
	ATLAS_PROF_THEMIS_LED_CONFIG	= 3,
};

#define ATLAS_SN_PN_LENGTH	20

struct __packed emi_eeprom_vpd {
	struct emi_eeprom_vpd_header header;
	char serial_number[ATLAS_SN_PN_LENGTH];	/* xx-xx-xxx-xxx */
	char part_number[ATLAS_SN_PN_LENGTH];	/* xxx-xxxx-xxxx.Rxx */
	uint16_t profile;
};

/* Always returns a non-NULL pointer to valid data */
const struct emi_eeprom_vpd *get_emi_eeprom_vpd(void);

#endif /* ATLAS_VPD_H */
