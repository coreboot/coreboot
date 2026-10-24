/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/smn.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <types.h>

/* SMN registers accessed indirectly using an index/data pair in D0F00 config space */
#define SMN_INDEX_ADDR		0xb8 /* 32 bit */
#define SMN_DATA_ADDR		0xbc /* 32 bit */

uint32_t smn_read32_at(pci_devfn_t dev, uint32_t reg)
{
	pci_s_write_config32(dev, SMN_INDEX_ADDR, reg);
	return pci_s_read_config32(dev, SMN_DATA_ADDR);
}

uint64_t smn_read64_at(pci_devfn_t dev, uint32_t reg)
{
	const uint32_t low = smn_read32_at(dev, reg);
	const uint32_t high = smn_read32_at(dev, reg + sizeof(uint32_t));

	return low | (uint64_t)high << 32;
}

void smn_write32_at(pci_devfn_t dev, uint32_t reg, uint32_t val)
{
	pci_s_write_config32(dev, SMN_INDEX_ADDR, reg);
	pci_s_write_config32(dev, SMN_DATA_ADDR, val);
}

static pci_devfn_t smn_default_dev(void)
{
#if ENV_PCI_SIMPLE_DEVICE
	return SOC_GNB_DEV;
#else
	return PCI_BDF(SOC_GNB_DEV);
#endif
}

uint32_t smn_read32(uint32_t reg)
{
	return smn_read32_at(smn_default_dev(), reg);
}

uint64_t smn_read64(uint32_t reg)
{
	return smn_read64_at(smn_default_dev(), reg);
}

void smn_write32(uint32_t reg, uint32_t val)
{
	smn_write32_at(smn_default_dev(), reg, val);
}
