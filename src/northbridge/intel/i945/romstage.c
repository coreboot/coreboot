/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cf9_reset.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <stdint.h>

__weak void mainboard_lpc_decode(void)
{
}

__weak void mainboard_pre_raminit_config(int s3_resume)
{
}

__weak void mainboard_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[1] = 0x51;
	spd_map[2] = 0x52;
	spd_map[3] = 0x53;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0;
	u8 spd_map[4] = {};

	mainboard_lpc_decode();

	if (mchbar_read16(SSKPD) == 0xcafe) {
		system_reset();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i82801gx_early_init();
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	mainboard_pre_raminit_config(s3resume);

	mainboard_get_spd_map(spd_map);

	if (CONFIG(DEBUG_RAM_SETUP))
		dump_spd_registers(spd_map);

	sdram_initialize(s3resume ? BOOT_PATH_RESUME : BOOT_PATH_NORMAL, spd_map);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	mainboard_late_rcba_config();

	/* Chipset Errata! */
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM))
		fixup_i945gm_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
