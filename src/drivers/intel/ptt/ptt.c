/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <security/intel/txt/txt_register.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#include "ptt.h"

#define PCI_ME_HFSTS4 0x64
#define PTT_ENABLE (1 << 19)

/* Dump Intel ME register */
static uint32_t read_register(int reg_addr)
{
	if (!PCH_DEV_CSE)
		return 0xFFFFFFFF;

	return pci_read_config32(PCH_DEV_CSE, reg_addr);
}

/*
 * ptt_active()
 *
 * Check if PTT Flag is set - so that PTT is active.
 *
 * Return true if active, false otherwise.
 */
bool ptt_active(void)
{
	uint32_t sts_ftif;
	uint32_t fwsts4 = read_register(PCI_ME_HFSTS4);

	if (fwsts4 == 0xFFFFFFFF)
		return false;

	if ((fwsts4 & PTT_ENABLE) == 0) {
		printk(BIOS_DEBUG, "Intel ME Establishment bit not valid.\n");
		sts_ftif = read32p(TXT_STS_FTIF);

		if (sts_ftif != 0 && sts_ftif != UINT32_MAX) {
			if ((sts_ftif & TXT_PTT_PRESENT) == TXT_PTT_PRESENT) {
				printk(BIOS_DEBUG, "TXT_STS_FTIF: PTT present and active\n");
				return true;
			}
		}
		return false;
	}

	return true;
}
