/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/smn.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <types.h>

/* SMN registers accessed indirectly using an index/data pair in D0F00 config space */
#define SMN_INDEX_ADDR		0xb8 /* 32 bit */
#define SMN_DATA_ADDR		0xbc /* 32 bit */

uint32_t smn_read32(uint32_t reg)
{
	pci_write_config32(SOC_GNB_DEV, SMN_INDEX_ADDR, reg);
	return pci_read_config32(SOC_GNB_DEV, SMN_DATA_ADDR);
}

void smn_write32(uint32_t reg, uint32_t val)
{
	pci_write_config32(SOC_GNB_DEV, SMN_INDEX_ADDR, reg);
	pci_write_config32(SOC_GNB_DEV, SMN_DATA_ADDR, val);
}
