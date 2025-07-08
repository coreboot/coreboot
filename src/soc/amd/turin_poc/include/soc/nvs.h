/* SPDX-License-Identifier: GPL-2.0-or-later */

/* TODO: Check if this is still correct */

/*
 * NOTE: The layout of the global_nvs structure below must match the layout
 * in soc/soc/amd/turin_poc/acpi/globalnvs.asl !!!
 *
 */

#ifndef AMD_TURIN_POC_NVS_H
#define AMD_TURIN_POC_NVS_H

#include <stdint.h>

struct __packed global_nvs {
	/* Miscellaneous */
	uint64_t	pm1i; /* 0x00 - 0x07 - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x08 - 0x0f - GPE Wake Source */
};

#endif /* AMD_TURIN_POC_NVS_H */
