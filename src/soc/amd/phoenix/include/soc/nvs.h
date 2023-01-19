/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the global_nvs structure below must match the layout
 * in soc/soc/amd/phoenix/acpi/globalnvs.asl !!!
 *
 */

#ifndef AMD_PHOENIX_NVS_H
#define AMD_PHOENIX_NVS_H

#include <stdint.h>

struct __packed global_nvs {
	/* Miscellaneous */
	uint8_t		lids; /* 0x00 - LID State */
	uint32_t	cbmc; /* 0x01 - 0x04 - coreboot Memory Console */
	uint64_t	pm1i; /* 0x05 - 0x0c - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x0d - 0x14 - GPE Wake Source */
};

#endif /* AMD_PHOENIX_NVS_H */
