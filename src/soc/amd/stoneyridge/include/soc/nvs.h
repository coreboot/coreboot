/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the global_nvs structure below must match the layout
 * in soc/soc/amd/stoneyridge/acpi/globalnvs.asl !!!
 *
 */

#ifndef __SOC_STONEYRIDGE_NVS_H__
#define __SOC_STONEYRIDGE_NVS_H__

#include <stdint.h>
#include <soc/southbridge.h>

struct __packed global_nvs {
	/* Miscellaneous */
	uint32_t	cbmc; /* 0x00 - 0x03 - coreboot Memory Console */
	uint64_t	pm1i; /* 0x04 - 0x0b - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x0c - 0x13 - GPE Wake Source */
	uint8_t		tmps; /* 0x14 - Temperature Sensor ID */
	uint8_t		tcrt; /* 0x15 - Critical Threshold */
	uint8_t		tpsv; /* 0x16 - Passive Threshold */
	uint8_t		pad1[9];
	aoac_devs_t	aoac; /* 0x20 - AOAC device enables */
	uint16_t	fw00; /* 0x24 - XhciFwRomAddr_Rom, Boot RAM */
	uint16_t	fw02; /* 0x26 - XhciFwRomAddr_Ram, Instr RAM */
	uint32_t	fw01; /* 0x28 - XhciFwRamAddr_Rom, Boot RAM sz/base */
	uint32_t	fw03; /* 0x2c - XhciFwRomAddr_Ram, Instr RAM sz/base */
	uint32_t	eh10; /* 0x30 - EHCI BAR */
};

#endif /* __SOC_STONEYRIDGE_NVS_H__ */
