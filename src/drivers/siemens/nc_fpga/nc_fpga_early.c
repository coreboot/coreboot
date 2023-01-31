/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>

#include "nc_fpga.h"

static DEVTREE_CONST uint32_t fpga_bar = CONFIG_EARLY_PCI_MMIO_BASE;
static bool nc_fpga_present = false;

int pci_early_device_probe(u8 bus, u8 dev, u32 mmio_base)
{
	pci_devfn_t pci_dev = PCI_DEV(bus, dev, 0);
	uint32_t id = pci_s_read_config32(pci_dev, PCI_VENDOR_ID);

	if (id != (0x4091 << 16 | PCI_VID_SIEMENS))
		return -1;

	/* Setup base address for BAR0. */
	pci_s_write_config32(pci_dev, PCI_BASE_ADDRESS_0, mmio_base);
	/* Enable memory access for pci_dev. */
	u16 reg16 = pci_s_read_config16(pci_dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_s_write_config16(pci_dev, PCI_COMMAND, reg16);
	nc_fpga_present = true;

	return 0;
}

void nc_fpga_remap(uint32_t new_mmio)
{
#if ENV_RAMSTAGE
	fpga_bar = new_mmio;
#endif
}

void nc_fpga_post(uint8_t value)
{
	/* The function pci_early_device_probe is called in bootblock and romstage. Make sure
	   that in these stages the initialization code was successful before the POST code
	   value is written to the register. */
	if ((ENV_BOOTBLOCK || ENV_ROMSTAGE) && nc_fpga_present == false)
		return;
	write32p(fpga_bar + NC_FPGA_POST_OFFSET, value);
}
