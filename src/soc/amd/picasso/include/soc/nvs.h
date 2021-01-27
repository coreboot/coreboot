/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the global_nvs structure below must match the layout
 * in soc/soc/amd/picasso/acpi/globalnvs.asl !!!
 *
 */

#ifndef AMD_PICASSO_NVS_H
#define AMD_PICASSO_NVS_H

#include <stdint.h>
#include <soc/southbridge.h>

struct __packed global_nvs {
	/* Miscellaneous */
	uint8_t		unused_was_pcnt; /* 0x00 - Processor Count */
	uint8_t		lids; /* 0x01 - LID State */
	uint8_t		pwrs; /* 0x02 - AC Power State */
	uint32_t	cbmc; /* 0x03 - 0x06 - coreboot Memory Console */
	uint64_t	pm1i; /* 0x07 - 0x0e - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x0f - 0x16 - GPE Wake Source */
	uint8_t		tmps; /* 0x17 - Temperature Sensor ID */
	uint8_t		tcrt; /* 0x18 - Critical Threshold */
	uint8_t		tpsv; /* 0x19 - Passive Threshold */
};

#endif /* AMD_PICASSO_NVS_H */
