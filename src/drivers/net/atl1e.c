/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This driver sets the macaddress of a Atheros AR8121/AR8113/AR8114
 */

#include <device/mmio.h>
#include <device/device.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <types.h>

#define REG_SPI_FLASH_CTRL		0x200
#define SPI_FLASH_CTRL_EN_VPD		0x2000

#define REG_PCIE_CAP_LIST		0x58

#define REG_MAC_STA_ADDR	0x1488

static u8 get_hex_digit(const u8 c)
{
	u8 ret = 0;

	ret = c - '0';
	if (ret > 0x09) {
		ret = c - 'A' + 0x0a;
		if (ret > 0x0f)
			ret = c - 'a' + 0x0a;
	}
	if (ret > 0x0f) {
		printk(BIOS_ERR, "Invalid hex digit found: "
				 "%c - 0x%02x\n", (char)c, c);
		ret = 0;
	}
	return ret;
}

#define MACLEN 17

static enum cb_err fetch_mac_string_cbfs(u8 *macstrbuf)
{
	if (!cbfs_load("atl1e-macaddress", macstrbuf, MACLEN)) {
		printk(BIOS_ERR, "atl1e: Error reading MAC from CBFS\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static void get_mac_address(u8 *macaddr, const u8 *strbuf)
{
	size_t offset = 0;
	int i;

	if ((strbuf[2] != ':') || (strbuf[5] != ':') ||
	    (strbuf[8] != ':') || (strbuf[11] != ':') ||
	    (strbuf[14] != ':')) {
		printk(BIOS_ERR, "atl1e: ignore invalid MAC address in cbfs\n");
		return;
	}

	for (i = 0; i < 6; i++) {
		macaddr[i] = 0;
		macaddr[i] |= get_hex_digit(strbuf[offset]) << 4;
		macaddr[i] |= get_hex_digit(strbuf[offset + 1]);
		offset += 3;
	}
}

static void program_mac_address(u32 mem_base)
{
	u8 macstrbuf[MACLEN] = { 0 };
	/* Default MAC Address of 90:e6:ba:24:f9:d2 */
	u8 mac[6] = { 0x90, 0xe6, 0xba, 0x24, 0xf9, 0xd2 };
	u32 value;

	if (fetch_mac_string_cbfs(macstrbuf) != CB_SUCCESS) {
		printk(BIOS_ERR, "atl1e: Error reading MAC from CBFS,"
		       " using default 90:e6:ba:24:f9:d2\n");
	} else {
		get_mac_address(mac, macstrbuf);
	}

	printk(BIOS_DEBUG, "atl1e: Programming MAC Address...");

	value = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | (mac[5] << 0);
	write32p(mem_base + REG_MAC_STA_ADDR, value);
	value = (mac[0] << 8) | (mac[1] << 0);
	write32p(mem_base + REG_MAC_STA_ADDR + 4, value);

	printk(BIOS_DEBUG, "done\n");
}

static int atl1e_eeprom_exist(u32 mem_base)
{
	u32 value = read32p(mem_base + REG_SPI_FLASH_CTRL);
	if (value & SPI_FLASH_CTRL_EN_VPD) {
		value &= ~SPI_FLASH_CTRL_EN_VPD;
		write32p(mem_base + REG_SPI_FLASH_CTRL, value);
	}
	value = read32p(mem_base + REG_PCIE_CAP_LIST);
	return ((value & 0xff00) == 0x6c00) ? 1 : 0;
}

static void atl1e_init(struct device *dev)
{
	/* Get the resource of the NIC mmio */
	struct resource *nic_res = probe_resource(dev, PCI_BASE_ADDRESS_0);

	if (nic_res == NULL) {
		printk(BIOS_ERR, "atl1e: resource not found\n");
		return;
	}

	u32 mem_base = nic_res->base;

	if (!mem_base) {
		printk(BIOS_ERR, "atl1e: resource not assigned\n");
		return;
	}

	if (atl1e_eeprom_exist(mem_base)) {
		printk(BIOS_INFO, "atl1e NIC has SPI eeprom, not setting MAC\n");
		return;
	}

	/* Check if the base is invalid */
	if (!mem_base) {
		printk(BIOS_ERR, "atl1e: Error can't find MEM resource\n");
		return;
	}
	/* Enable but do not set bus master */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Program MAC address based on CBFS "macaddress" containing
	 * a string AA:BB:CC:DD:EE:FF */
	program_mac_address(mem_base);
}

static struct device_operations atl1e_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = atl1e_init,
};

static const struct pci_driver atl1e_driver __pci_driver = {
	.ops    = &atl1e_ops,
	.vendor = 0x1969,
	.device = 0x1026,
};

struct chip_operations drivers_net_ops = {
	CHIP_NAME("Atheros AR8121/AR8113/AR8114")
};
