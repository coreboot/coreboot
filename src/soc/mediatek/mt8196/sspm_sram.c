/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/sspm.h>

#define SSPM_SRAM_CON		(SPM_BASE + 0xEE4)
#define SPM_PRJ_CODE		0xB160001
#define SSPM_SRAM_SLEEP_B	0x10
#define SSPM_SRAM_ISOINT_B	0x2

void sspm_enable_sram(void)
{
	write32p(SPM_BASE, SPM_PRJ_CODE);
	write32p(SSPM_SRAM_CON, SSPM_SRAM_SLEEP_B | SSPM_SRAM_ISOINT_B);
}
