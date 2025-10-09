/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "smn.h"
#include "amdtool.h"

#define SMN_INDIRECT_INDEX		0xB8
#define SMN_INDIRECT_DATA		0xBC

static struct pci_dev *nb_dev = NULL;

uint32_t smn_read32(uint32_t addr)
{
	if (!nb_dev)
		return UINT32_MAX;

	pci_write_long(nb_dev, SMN_INDIRECT_INDEX, addr & 0xfffffffc);
	return pci_read_long(nb_dev, SMN_INDIRECT_DATA);
}

uint16_t smn_read16(uint32_t addr)
{
	if (!nb_dev)
		return UINT8_MAX;

	pci_write_long(nb_dev, SMN_INDIRECT_INDEX, addr & 0xfffffffc);
	return pci_read_word(nb_dev, SMN_INDIRECT_DATA + (addr & 2));
}

uint8_t smn_read8(uint32_t addr)
{
	if (!nb_dev)
		return UINT8_MAX;

	pci_write_long(nb_dev, SMN_INDIRECT_INDEX, addr & 0xfffffffc);
	return pci_read_byte(nb_dev, SMN_INDIRECT_DATA + (addr & 3));
}

void init_smn(struct pci_dev *nb)
{
	nb_dev = nb;
}
