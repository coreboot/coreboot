/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/p2sblib.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>

void p2sb_dev_enable_bar(pci_devfn_t dev, uint64_t bar)
{
	/* Enable PCR Base addresses */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (uint32_t)bar);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, (uint32_t)(bar >> 32));

	/* Enable P2SB MSE */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

bool p2sb_dev_is_hidden(pci_devfn_t dev)
{
	const uint16_t pci_vid = pci_read_config16(dev, PCI_VENDOR_ID);

	if (pci_vid == 0xffff)
		return true;
	if (pci_vid == PCI_VID_INTEL)
		return false;
	printk(BIOS_ERR, "P2SB PCI_VENDOR_ID is invalid, unknown if hidden\n");
	return true;
}

static void p2sb_dev_set_hide_bit(pci_devfn_t dev, int hide)
{
	const uint16_t reg = P2SBC + 1;
	const uint8_t mask = P2SBC_HIDE_BIT;
	uint8_t val;

	val = pci_read_config8(dev, reg);
	val &= ~mask;
	if (hide)
		val |= mask;
	pci_write_config8(dev, reg, val);
}

void p2sb_dev_unhide(pci_devfn_t dev)
{
	p2sb_dev_set_hide_bit(dev, 0);

	if (p2sb_dev_is_hidden(dev))
		die_with_post_code(POST_HW_INIT_FAILURE,
				"Unable to unhide the P2SB device!\n");
}

void p2sb_dev_hide(pci_devfn_t dev)
{
	p2sb_dev_set_hide_bit(dev, 1);

	if (!p2sb_dev_is_hidden(dev))
		die_with_post_code(POST_HW_INIT_FAILURE,
				"Unable to hide the P2SB device!\n");
}

static void p2sb_execute_sideband_access(pci_devfn_t dev, uint8_t cmd, uint8_t pid,
						uint16_t reg, uint32_t *data)
{
	struct pcr_sbi_msg msg = {
		.pid = pid,
		.offset = reg,
		.opcode = cmd,
		.is_posted = false,
		.fast_byte_enable = 0xF,
		.bar = 0,
		.fid = 0
	};
	uint8_t response;
	int status;

	/* Unhide the P2SB device */
	p2sb_dev_unhide(dev);

	status = pcr_execute_sideband_msg(dev, &msg, data, &response);
	if (status || response)
		printk(BIOS_ERR, "Fail to execute p2sb sideband access\n");

	/* Hide the P2SB device */
	p2sb_dev_hide(dev);
}

uint32_t p2sb_dev_sbi_read(pci_devfn_t dev, uint8_t pid, uint16_t reg)
{
	uint32_t val = 0;
	p2sb_execute_sideband_access(dev, PCR_READ, pid, reg, &val);
	return val;
}

void p2sb_dev_sbi_write(pci_devfn_t dev, uint8_t pid, uint16_t reg, uint32_t val)
{
	p2sb_execute_sideband_access(dev, PCR_WRITE, pid, reg, &val);
}
