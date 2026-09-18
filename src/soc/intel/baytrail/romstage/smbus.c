/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

uintptr_t smbus_base(void)
{
	return SMBUS_BASE_ADDRESS;
}

int smbus_enable_iobar(uintptr_t base)
{
	uint32_t reg;
	const uint32_t smbus_dev = PCI_DEV(0, SMBUS_DEV, SMBUS_FUNC);

	/* SMBus I/O BAR */
	reg = base | 2;
	pci_write_config32(smbus_dev, PCI_BASE_ADDRESS_4, reg);
	/* Enable decode of I/O space. */
	reg = pci_read_config16(smbus_dev, PCI_COMMAND);
	reg |= PCI_COMMAND_IO;
	pci_write_config16(smbus_dev, PCI_COMMAND, reg);
	/* Enable Host Controller */
	reg = pci_read_config8(smbus_dev, 0x40);
	reg |= 1;
	pci_write_config8(smbus_dev, 0x40, reg);

	/* Configure pads to be used for SMBus */
	score_select_func(PCU_SMB_CLK_PAD, 1);
	score_select_func(PCU_SMB_DATA_PAD, 1);

	return 0;
}
