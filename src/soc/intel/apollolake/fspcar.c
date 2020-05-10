/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <FsptUpd.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
		.Signature = 0x545F4450554C5041ULL,	/* 'APLUPD_T' */
		.Revision = 1,
		.Reserved = {0},
	},
	.FsptCommonUpd = {
		.Revision = 0,
		.Reserved = {0},
		/*
		 * It is a requirement for firmware to have Firmware Interface Table
		 * (FIT), which contains pointers to each microcode update.
		 * The microcode update is loaded for all logical processors before
		 * cpu reset vector.
		 *
		 * All SoC since Gen-4 has above mechanism in place to load microcode
		 * even before hitting CPU reset vector. Hence skipping FSP-T loading
		 * microcode after CPU reset by passing '0' value to
		 * FSPT_UPD.MicrocodeRegionBase and FSPT_UPD.MicrocodeRegionLength.
		 */
		.MicrocodeRegionBase = 0,
		.MicrocodeRegionLength = 0,
		.CodeRegionBase =
			(uint32_t)(0x100000000ULL - CONFIG_ROM_SIZE),
		.CodeRegionLength = (uint32_t)CONFIG_ROM_SIZE,
		.Reserved1 = {0},
	},
};
