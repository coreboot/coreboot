/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2016-2018 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/path.h>
#include <console/console.h>
#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <hwilib.h>
#include <i210.h>
#include <soc/pci_devs.h>
#include <soc/irq.h>
#include <soc/lpc.h>
#include <bootstate.h>
#include <timer.h>
#include <timestamp.h>
#include <pca9538.h>

#define MAX_PATH_DEPTH		12
#define MAX_NUM_MAPPINGS	10

/*
 * SPI Opcode Menu setup for SPIBAR lock down
 * should support most common flash chips.
 */
#define SPI_OPMENU_0 0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0 0x01 /* Write, no address */

#define SPI_OPMENU_1 0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1 0x03 /* Write, address required */

#define SPI_OPMENU_2 0x03 /* READ: Read Data */
#define SPI_OPTYPE_2 0x02 /* Read, address required */

#define SPI_OPMENU_3 0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3 0x00 /* Read, no address */

#define SPI_OPMENU_4 0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4 0x03 /* Write, address required */

#define SPI_OPMENU_5 0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5 0x00 /* Read, no address */

#define SPI_OPMENU_6 0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6 0x03 /* Write, address required */

#define SPI_OPMENU_7 0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7 0x02 /* Read, address required */

#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 << 8) | SPI_OPMENU_4)
#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 << 8) | SPI_OPMENU_0)

#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 << 8) | \
		    (SPI_OPTYPE_3 << 6) | (SPI_OPTYPE_2 << 4) | \
		    (SPI_OPTYPE_1 << 2) | (SPI_OPTYPE_0))

#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */

#define SPIBAR_OFFSET		0x3800
#define SPI_REG_PREOP		0x94
#define SPI_REG_OPTYPE		0x96
#define SPI_REG_OPMENU_L	0x98
#define SPI_REG_OPMENU_H	0x9c

/* Define the slave address for the I/O expander. */
#define PCA9538_SLAVE_ADR	0x71
/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(device_t dev)
{

}

static void mainboard_init(void *chip_info)
{
	uint8_t actl = 0;
	device_t dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));

	/* Route SCI to IRQ 10 to free IRQ 9 slot. */
	actl = pci_read_config8(dev, ACPI_CNTL_OFFSET);
	actl &= ~SCIS_MASK;
	actl |= SCIS_IRQ10;
	pci_write_config8(dev, ACPI_CNTL_OFFSET, actl);

	/* Enable additional I/O decoding ranges on LPC for COM 3 and COM 4 */
	pci_write_config32(dev, LPC_GEN1_DEC, 0x1C02E9);
	pci_write_config32(dev, LPC_GEN2_DEC, 0x1C03E9);
}

static void mainboard_final(void *chip_info)
{
	void *spi_base = NULL;
	uint32_t rcba = 0;
	device_t dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));

	/* Get address of SPI controller. */
	rcba = (pci_read_config32(dev, 0xf0) & 0xffffc000);
	if (!rcba)
		return;
	spi_base = (void *)(rcba + SPIBAR_OFFSET);
	/* Setup OPCODE menu */
	write16((spi_base + SPI_REG_PREOP), SPI_OPPREFIX);
	write16((spi_base + SPI_REG_OPTYPE), SPI_OPTYPE);
	write32((spi_base + SPI_REG_OPMENU_L), SPI_OPMENU_LOWER);
	write32((spi_base + SPI_REG_OPMENU_H), SPI_OPMENU_UPPER);

	/* Set Master Enable for on-board PCI devices. */
	dev = dev_find_device(PCI_VENDOR_ID_SIEMENS, 0x403e, 0);
	if (dev) {
		uint16_t cmd = pci_read_config16(dev, PCI_COMMAND);
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config16(dev, PCI_COMMAND, cmd);
	}
	dev = dev_find_device(PCI_VENDOR_ID_SIEMENS, 0x403f, 0);
	if (dev) {
		uint16_t cmd = pci_read_config16(dev, PCI_COMMAND);
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config16(dev, PCI_COMMAND, cmd);
	}
	/* Show the mainboard version well-visible on console. */
	printk(BIOS_NOTICE, "***************************\n"
			    "* Mainboard version: 0x%02x *\n"
			    "***************************\n",
			    pca9538_read_input());
}

/** \brief This function can decide if a given MAC address is valid or not.
 *         Currently, addresses filled with 0xff or 0x00 are not valid.
 * @param  mac  Buffer to the MAC address to check
 * @return	0 if address is not valid, otherwise 1
 */
static uint8_t is_mac_adr_valid(uint8_t mac[6])
{
	uint8_t buf[6];

	memset(buf, 0, sizeof(buf));
	if (!memcmp(buf, mac, sizeof(buf)))
		return 0;
	memset(buf, 0xff, sizeof(buf));
	if (!memcmp(buf, mac, sizeof(buf)))
		return 0;
	return 1;
}
 /** \brief This function will search for a MAC address which can be assigned
  *         to a MACPHY.
  * @param  dev     pointer to PCI device
  * @param  mac     buffer where to store the MAC address
  * @return cb_err  CB_ERR or CB_SUCCESS
  */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[6])
{
	struct bus *parent = dev->bus;
	uint8_t buf[16], mapping[16], i = 0, chain_len = 0;

	memset(buf, 0, sizeof(buf));
	memset(mapping, 0, sizeof(mapping));

	/* The first entry in the tree is the device itself. */
	buf[0] = dev->path.pci.devfn;
	chain_len = 1;
	for (i = 1; i < MAX_PATH_DEPTH && parent->dev->bus->subordinate; i++) {
		buf[i] = parent->dev->path.pci.devfn;
		chain_len++;
		parent = parent->dev->bus;
	}
	if (i == MAX_PATH_DEPTH) {
		/* The path is deeper than MAX_PATH_DEPTH devices, error. */
		printk(BIOS_ERR, "Too many bridges for %s\n", dev_path(dev));
		return CB_ERR;
	}
	/* Now construct the mapping based on the device chain starting from */
	/* root bridge device to the device itself. */
	mapping[0] = 1;
	mapping[1] = chain_len;
	for (i = 0; i < chain_len; i++)
		mapping[i + 4] = buf[chain_len - i - 1];

	/* Open main hwinfo block */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return CB_ERR;
	/* Now try to find a valid MAC address in hwinfo for this mapping.*/
	for (i = 0; i < MAX_NUM_MAPPINGS; i++) {
		if ((hwilib_get_field(XMac1Mapping + i, buf, 16) == 16) &&
			!(memcmp(buf, mapping, chain_len + 4))) {
		/* There is a matching mapping available, get MAC address. */
			if ((hwilib_get_field(XMac1 + i, mac, 6) == 6) &&
			    (is_mac_adr_valid(mac))) {
				return CB_SUCCESS;
			} else {
				return CB_ERR;
			}
		} else
			continue;
	}
	/* No MAC address found for */
	return CB_ERR;
}

static void wait_for_legacy_dev(void *unused)
{
	uint32_t legacy_delay, us_since_boot;
	struct stopwatch sw;

	/* Open main hwinfo block. */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return;

	/* Get legacy delay parameter from hwinfo. */
	if (hwilib_get_field(LegacyDelay, (uint8_t *) &legacy_delay,
			      sizeof(legacy_delay)) != sizeof(legacy_delay))
		return;

	us_since_boot = get_us_since_boot();
	/* No need to wait if the time since boot is already long enough.*/
	if (us_since_boot > legacy_delay)
		return;
	stopwatch_init_msecs_expire(&sw, (legacy_delay - us_since_boot) / 1000);
	printk(BIOS_NOTICE, "Wait remaining %d of %d us for legacy devices...",
			legacy_delay - us_since_boot, legacy_delay);
	stopwatch_wait_until_expired(&sw);
	printk(BIOS_NOTICE, "done!\n");
}

/*
 * To access the I/O expander PCA9538 we need to know its device structure.
 * This function will provide it as mainboard code has the knowledge of the
 * right I2C slave address for the I/O expander.
 */
struct device *pca9538_get_dev(void)
{
	struct device *dev = NULL;

	while ((dev = dev_find_path(dev, DEVICE_PATH_I2C))) {
		if (dev->path.i2c.device == PCA9538_SLAVE_ADR)
			break;
	}
	return dev;
}


BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, wait_for_legacy_dev, NULL);

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_init,
	.final = mainboard_final
};
