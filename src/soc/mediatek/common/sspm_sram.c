/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/spm.h>
#include <soc/sspm.h>

#define SPM_PRJ_CODE		0xB160001
#define SSPM_SRAM_SLEEP_B	0x10
#define SSPM_SRAM_ISOINT_B	0x2

void sspm_enable_sram(void)
{
	write32p(SPM_BASE, SPM_PRJ_CODE);
	write32(&mtk_spm->sspm_sram_con, SSPM_SRAM_SLEEP_B | SSPM_SRAM_ISOINT_B);
}
