/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <fsp/util.h>

static bool use_fsp_v1(void)
{
	/*
	 * Per the Comet Lake FSP documentation, differentiate Comet Lake v1/v2
	 * by CPUID.  CML v1 has eax 0x000A0660 or 0x000806EC, CML v2 has
	 * 0x000A0661.
	 */
	uint32_t cpuid = cpu_get_cpuid();
	switch (cpuid) {
	case 0x000A0660:
	case 0x000806EC:
		printk(BIOS_INFO, "CPUID %08X is Comet Lake v1\n", cpuid);
		return true;
	case 0x000A0661:
		printk(BIOS_INFO, "CPUID %08X is Comet Lake v2\n", cpuid);
		return false;
	default:
		/*
		 * It's unlikely any new Comet Lake SKUs would be added
		 * at this point, but guess CML v2 rather than failing
		 * to boot entirely.
		 */
		printk(BIOS_ERR, "CPUID %08X is unknown, guessing Comet Lake v2\n",
			cpuid);
		return false;
	}
}

const char *soc_select_fsp_m_cbfs(void)
{
	return use_fsp_v1() ? CONFIG_FSP_M_CBFS : CONFIG_FSP_M_CBFS_2;
}

const char *soc_select_fsp_s_cbfs(void)
{
	return use_fsp_v1() ? CONFIG_FSP_S_CBFS : CONFIG_FSP_S_CBFS_2;
}
