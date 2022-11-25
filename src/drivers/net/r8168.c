/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This driver resets the 10ec:8168 NIC then tries to read
 * "macaddress" string XX:XX:XX:XX:XX:XX from CBFS.
 * If no MAC is found, it programs a default MAC address in the device
 * so that if the EEPROM/efuse is unconfigured it still has a default MAC.
 */

#include <cbfs.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <delay.h>
#include <fmap.h>
#include <types.h>

#include "chip.h"

#define NIC_TIMEOUT		1000

#define CMD_REG			0x37
#define  CMD_REG_RESET		0x10
#define CMD_LED0_LED1		0x18
#define CMD_LED_FEATURE		0x94
#define CMD_LEDSEL0		0x18
#define CMD_LEDSEL2		0x84

#define CFG_9346		0x50
#define  CFG_9346_LOCK		0x00
#define  CFG_9346_UNLOCK	0xc0
#define CMD_REG_ASPM		0xb0
#define ASPM_L1_2_MASK		0xe059000f

#define DEVICE_INDEX_BYTE	12
#define MAX_DEVICE_SUPPORT	10
/**
 * search: Find first instance of string in a given region
 * @param p       string to find
 * @param a       start address of region to search
 * @param lengthp length of string to search for
 * @param lengtha length of region to search in
 * @return offset offset from start address a where string was found.
 *                If string not found, return lengtha.
 */
static size_t search(const char *p, const u8 *a, size_t lengthp,
		     size_t lengtha)
{
	size_t i, j;

	if (lengtha < lengthp)
		return lengtha;
	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++)
			;
		if (i >= lengthp && a[j - 1] == lengthp)
			return j;
	}
	return lengtha;
}

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

/* Returns MAC address based on the key that is passed in. */
static enum cb_err fetch_mac_vpd_key(u8 *macstrbuf, const char *vpd_key)
{
	struct region_device rdev;
	void *search_address;
	size_t search_length;
	size_t offset;

	if (fmap_locate_area_as_rdev("RO_VPD", &rdev)) {
		printk(BIOS_ERR, "Couldn't find RO_VPD region.");
		return CB_ERR;
	}
	search_address = rdev_mmap_full(&rdev);
	if (search_address == NULL) {
		printk(BIOS_ERR, "LAN: VPD not found.\n");
		return CB_ERR;
	}

	search_length = region_device_sz(&rdev);
	offset = search(vpd_key, search_address, strlen(vpd_key),
			search_length);

	if (offset == search_length) {
		printk(BIOS_ERR, "Could not locate '%s' in VPD\n", vpd_key);
		rdev_munmap(&rdev, search_address);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", vpd_key);

	offset += strlen(vpd_key) + 1;	/* move to next character */

	if (offset + MACLEN > search_length) {
		rdev_munmap(&rdev, search_address);
		printk(BIOS_ERR, "Search result too small!\n");
		return CB_ERR;
	}
	memcpy(macstrbuf, search_address + offset, MACLEN);
	rdev_munmap(&rdev, search_address);

	return CB_SUCCESS;
}

/* Prepares vpd_key by concatenating ethernet_mac with device_index */
static enum cb_err fetch_mac_vpd_dev_idx(u8 *macstrbuf, u8 device_index)
{
	char key[] = "ethernet_mac "; /* Leave a space at tail to stuff an index */

	/*
	 * Map each NIC on the DUT to "ethernet_macN", where N is [0-9].
	 * Translate index number from integer to ascii by adding '0' char.
	 */
	key[DEVICE_INDEX_BYTE] = device_index + '0';

	return fetch_mac_vpd_key(macstrbuf, key);
}

static void fetch_mac_string_vpd(struct drivers_net_config *config, u8 *macstrbuf)
{
	if (!config)
		return;

	/* Current implementation is up to 10 NIC cards */
	if (config->device_index > MAX_DEVICE_SUPPORT) {
		printk(BIOS_ERR, "r8168: the maximum device_index should be less then %d\n."
					" Using default 00:e0:4c:00:c0:b0\n", MAX_DEVICE_SUPPORT);
		return;
	}

	if (fetch_mac_vpd_dev_idx(macstrbuf, config->device_index) == CB_SUCCESS)
		return;

	if (!CONFIG(RT8168_SUPPORT_LEGACY_VPD_MAC)) {
		printk(BIOS_ERR, "r8168: mac address not found in VPD,"
						 " using default 00:e0:4c:00:c0:b0\n");
		return;
	}

	if (fetch_mac_vpd_key(macstrbuf, "ethernet_mac") != CB_SUCCESS)
		printk(BIOS_ERR, "r8168: mac address not found in VPD,"
					 " using default 00:e0:4c:00:c0:b0\n");
}

static enum cb_err fetch_mac_string_cbfs(u8 *macstrbuf)
{
	if (!cbfs_load("rt8168-macaddress", macstrbuf, MACLEN)) {
		printk(BIOS_ERR, "r8168: Error reading MAC from CBFS\n");
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
		printk(BIOS_ERR, "r8168: ignore invalid MAC address in cbfs\n");
		return;
	}

	for (i = 0; i < 6; i++) {
		macaddr[i] = 0;
		macaddr[i] |= get_hex_digit(strbuf[offset]) << 4;
		macaddr[i] |= get_hex_digit(strbuf[offset + 1]);
		offset += 3;
	}
}

static void program_mac_address(struct device *dev, u16 io_base)
{
	u8 macstrbuf[MACLEN] = { 0 };
	int i = 0;
	/* Default MAC Address of 00:E0:4C:00:C0:B0 */
	u8 mac[6] = { 0x00, 0xe0, 0x4c, 0x00, 0xc0, 0xb0 };
	struct drivers_net_config *config = dev->chip_info;

	/* check the VPD for the mac address */
	if (CONFIG(RT8168_GET_MAC_FROM_VPD)) {
		fetch_mac_string_vpd(config, macstrbuf);
	} else {
		if (fetch_mac_string_cbfs(macstrbuf) != CB_SUCCESS)
			printk(BIOS_ERR, "r8168: Error reading MAC from CBFS,"
							" using default 00:e0:4c:00:c0:b0\n");
	}
	get_mac_address(mac, macstrbuf);

	/* Reset NIC */
	printk(BIOS_DEBUG, "r8168: Resetting NIC...");
	outb(CMD_REG_RESET, io_base + CMD_REG);

	/* Poll for reset, with 1sec timeout */
	while (i < NIC_TIMEOUT && (inb(io_base + CMD_REG) & CMD_REG_RESET)) {
		udelay(1000);
		if (++i >= NIC_TIMEOUT)
			printk(BIOS_ERR, "timeout waiting for nic to reset\n");
	}
	if (i < NIC_TIMEOUT)
		printk(BIOS_DEBUG, "done\n");

	printk(BIOS_DEBUG, "r8168: Programming MAC Address...");

	/* Disable register protection */
	outb(CFG_9346_UNLOCK, io_base + CFG_9346);

	/* Set MAC address: only 4-byte write accesses allowed */
	outl(mac[4] | mac[5] << 8, io_base + 4);
	inl(io_base + 4);
	outl(mac[0] | mac[1] << 8 | mac[2] << 16 | mac[3] << 24,
		io_base);
	inl(io_base);
	/* Lock config regs */
	outb(CFG_9346_LOCK, io_base + CFG_9346);

	printk(BIOS_DEBUG, "done\n");
}

static void enable_aspm_l1_2(u16 io_base)
{
	printk(BIOS_INFO, "rtl: Enable ASPM L1.2\n");

	/* Disable register protection */
	outb(CFG_9346_UNLOCK, io_base + CFG_9346);

	/* Enable ASPM_L1.2 */
	outl(ASPM_L1_2_MASK, io_base + CMD_REG_ASPM);

	/* Lock config regs */
	outb(CFG_9346_LOCK, io_base + CFG_9346);
}

static void r8168_set_customized_led(struct device *dev, u16 io_base)
{
	struct drivers_net_config *config = dev->chip_info;

	if (!config)
		return;

	if (dev->device == PCI_DID_REALTEK_8125) {
		/* Set LED global Feature register */
		outb(config->led_feature, io_base + CMD_LED_FEATURE);
		printk(BIOS_DEBUG, "r8125: read back LED global feature setting as 0x%x\n",
		inb(io_base + CMD_LED_FEATURE));

		/*
		 * Refer to RTL8125 datasheet 5.Customizable LED Configuration
		 * Register Name	IO Address
		 * LEDSEL0		0x18
		 * LEDSEL2		0x84
		 * LEDFEATURE		0x94
		 *
		 * LEDSEL Bit[]		Description
		 * Bit0			Link10M
		 * Bit1			Link100M
		 * Bit3			Link1000M
		 * Bit5			Link2.5G
		 * Bit9			ACT
		 * Bit10		preboot enable
		 * Bit11		lp enable
		 * Bit12		active low/high
		 *
		 * LEDFEATURE		Description
		 * Bit0			LED Table V1/V2
		 * Bit1~3		Reserved
		 * Bit4~5		LED Blinking Duty Cycle	12.5%/ 25%/ 50%/ 75%
		 * Bit6~7		LED Blinking Freq. 240ms/160ms/80ms/Link-Speed-Dependent
		 */

		/* Set customized LED0 register */
		outw(config->customized_led0, io_base + CMD_LEDSEL0);
		printk(BIOS_DEBUG, "r8125: read back LED0 setting as 0x%x\n",
			inw(io_base + CMD_LEDSEL0));

		/* Set customized LED2 register */
		outw(config->customized_led2, io_base + CMD_LEDSEL2);
		printk(BIOS_DEBUG, "r8125: read back LED2 setting as 0x%x\n",
			inw(io_base + CMD_LEDSEL2));
	} else {
		/* Read the customized LED setting from devicetree */
		printk(BIOS_DEBUG, "r8168: Customized LED 0x%x\n", config->customized_leds);

		/*
		 * Refer to RTL8111H datasheet 7.2 Customizable LED Configuration
		 * Starting from offset 0x18
		 * Bit[15:12]	LED Feature Control(FC)
		 * Bit[11:08]	LED Select for PINLED2
		 * Bit[07:04]	LED Select for PINLED1
		 * Bit[03:00]	LED Select for PINLED0
		 *
		 * Speed	Link10M		Link100M	Link1000M	ACT/Full
		 * LED0		Bit0		Bit1		Bit2		Bit3
		 * LED1		Bit4		Bit5		Bit6		Bit7
		 * LED2		Bit8		Bit9		Bit10		Bit11
		 * FC		Bit12		Bit13		Bit14		Bit15
		 */

		/* Set customized LED registers */
		outw(config->customized_leds, io_base + CMD_LED0_LED1);
		printk(BIOS_DEBUG, "r8168: read back LED setting as 0x%x\n",
			inw(io_base + CMD_LED0_LED1));
	}
}

static void r8168_init(struct device *dev)
{
	/* Get the resource of the NIC mmio */
	struct resource *nic_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 io_base = (u16)nic_res->base;

	/* Check if the base is invalid */
	if (!io_base) {
		printk(BIOS_ERR, "r8168: Error can't find IO resource\n");
		return;
	}
	/* Enable but do not set bus master */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Program MAC address based on CBFS "macaddress" containing
	 * a string AA:BB:CC:DD:EE:FF */
	program_mac_address(dev, io_base);

	/* Program customized LED mode */
	if (CONFIG(RT8168_SET_LED_MODE))
		r8168_set_customized_led(dev, io_base);

	struct drivers_net_config *config = dev->chip_info;
	if (CONFIG(PCIEXP_ASPM) && config->enable_aspm_l1_2)
		enable_aspm_l1_2(io_base);
}

#if CONFIG(HAVE_ACPI_TABLES)
#define R8168_ACPI_HID "R8168"
static void r8168_net_fill_ssdt(const struct device *dev)
{
	struct drivers_net_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev->bus->dev);
	u32 address;

	if (!path || !config)
		return;

	/* Device */
	acpigen_write_scope(path);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", R8168_ACPI_HID);
	acpi_device_write_uid(dev);

	if (dev->chip_ops)
		acpigen_write_name_string("_DDN", dev->chip_ops->name);

	/* Power Resource */
	if (CONFIG(RT8168_GEN_ACPI_POWER_RESOURCE) && config->has_power_resource) {
		const struct acpi_power_res_params power_res_params = {
			.stop_gpio = &config->stop_gpio,
			.stop_delay_ms = config->stop_delay_ms,
			.stop_off_delay_ms = config->stop_off_delay_ms
		};
		acpi_device_add_power_res(&power_res_params);
	}

	/* Address */
	address = PCI_SLOT(dev->path.pci.devfn) & 0xffff;
	address <<= 16;
	address |= PCI_FUNC(dev->path.pci.devfn) & 0xffff;
	acpigen_write_name_dword("_ADR", address);

	/* Wake capabilities */
	if (config->wake)
		acpigen_write_PRW(config->wake, 3);

	if (config->add_acpi_dma_property)
		acpi_device_add_dma_property(NULL);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s.%s: %s %s\n", path, acpi_device_name(dev),
		dev->chip_ops ? dev->chip_ops->name : "", dev_path(dev));
}

static const char *r8168_net_acpi_name(const struct device *dev)
{
	return "RLTK";
}
#endif

static struct device_operations r8168_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = r8168_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = r8168_net_acpi_name,
	.acpi_fill_ssdt   = r8168_net_fill_ssdt,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_REALTEK_8168,
	PCI_DID_REALTEK_8125,
	PCI_DID_REALTEK_8111,
	0
};

static const struct pci_driver r8168_driver __pci_driver = {
	.ops    = &r8168_ops,
	.vendor = PCI_VID_REALTEK,
	.devices = pci_device_ids,
};

struct chip_operations drivers_net_ops = {
	CHIP_NAME("Realtek r8168")
};
