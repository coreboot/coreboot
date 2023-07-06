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
	uint64_t	pm1i; /* 0x00 - 0x07 - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x08 - 0x0f - GPE Wake Source */
	uint8_t		tmps; /* 0x10 - Temperature Sensor ID */
	uint8_t		tcrt; /* 0x11 - Critical Threshold */
	uint8_t		tpsv; /* 0x12 - Passive Threshold */
};

#endif /* AMD_PICASSO_NVS_H */
