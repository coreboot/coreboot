/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include "SBPLATFORM.h"
#include "sb_cimx.h"
#include "cfg.h"		/*sb800_cimx_config*/

/**
 * @brief South Bridge CIMx romstage entry,
 *        wrapper of sbPowerOnInit entry point.
 */
void sb_Poweron_Init(void)
{
	AMDSBCFG sb_early_cfg;

	sb800_cimx_config(&sb_early_cfg);
	//sb_early_cfg.StdHeader.Func = SB_POWERON_INIT;
	//AmdSbDispatcher(&sb_early_cfg);
	//TODO
	//AMD_IMAGE_HEADER was missing, when using AmdSbDispatcher,
	// VerifyImage() will fail, LocateImage() takes minutes to find the image.
	sbPowerOnInit(&sb_early_cfg);
}

/**
 * CIMX not set the clock to 48Mhz until sbBeforePciInit,
 * coreboot may need to set this even more earlier
 */
void sb800_clk_output_48Mhz(void)
{

	misc_write32(0x40, misc_read32(0x40) & (~5));
	misc_write32(0x40, misc_read32(0x40) | 2);
}
