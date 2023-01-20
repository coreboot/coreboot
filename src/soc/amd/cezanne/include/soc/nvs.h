/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the global_nvs structure below must match the layout
 * in soc/soc/amd/cezanne/acpi/globalnvs.asl !!!
 *
 */

#ifndef AMD_CEZANNE_NVS_H
#define AMD_CEZANNE_NVS_H

#include <stdint.h>

struct __packed global_nvs {
	/* Miscellaneous */
	uint32_t	cbmc; /* 0x00 - 0x03 - coreboot Memory Console */
	uint64_t	pm1i; /* 0x04 - 0x0b - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x0c - 0x13 - GPE Wake Source */
};

#endif /* AMD_CEZANNE_NVS_H */
