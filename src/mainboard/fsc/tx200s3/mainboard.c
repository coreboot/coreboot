/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <boot/tables.h>
#include <delay.h>
#include <arch/coreboot_tables.h>
#include "chip.h"
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <smbios.h>
#include <string.h>
#include <../../../drivers/ipmi/ipmi_kcs.h>

static const unsigned char ipmi_acpi_poweron[] = { 0xa0, 0x7f };
static const unsigned char ipmi_boot_wdt_off[] = { 0x80, 0x28, 0x00, 0x0a, 0xc0 };
static const unsigned char ipmi_clear_errors[] = { 0x80, 0x28, 0x00, 0x08, 0x01 };

static void mainboard_enable(device_t dev)
{
	unsigned char ipmi_response[32];
	int i, ret;

	ret = ipmi_kcs_message(0xca2, IPMI_NETFN_APPLICATION, 0, IPMI_CMD_ACPI_POWERON,
			       ipmi_acpi_poweron, sizeof(ipmi_acpi_poweron),
			       ipmi_response, sizeof(ipmi_response));

	if (ret <= 0) {
		printk(BIOS_WARNING, "IPMI status: %d\n", ret);
		return;
	}

	printk(BIOS_INFO, "IPMI response: ");
	for(i = 0; i < ret; i++) {
		printk(BIOS_INFO, "%02X ", ipmi_response[i]);
	}
	printk(BIOS_INFO, "\n");

	ret = ipmi_kcs_message(0xca2, 0x2e, 0, 0xf1,
			       ipmi_boot_wdt_off, sizeof(ipmi_boot_wdt_off),
			       ipmi_response, sizeof(ipmi_response));

	if (ret <= 0) {
		printk(BIOS_WARNING, "IPMI status: %d\n", ret);
		return;
	}

	printk(BIOS_INFO, "IPMI response: ");
	for(i = 0; i < ret; i++) {
		printk(BIOS_INFO, "%02X ", ipmi_response[i]);
	}
	printk(BIOS_INFO, "\n");

	ret = ipmi_kcs_message(0xca2, 0x2e, 0, 0xf1,
			       ipmi_clear_errors, sizeof(ipmi_clear_errors),
			       ipmi_response, sizeof(ipmi_response));

	if (ret <= 0) {
		printk(BIOS_WARNING, "IPMI status: %d\n", ret);
		return;
	}

	printk(BIOS_INFO, "IPMI response: ");
	for(i = 0; i < ret; i++) {
		printk(BIOS_INFO, "%02X ", ipmi_response[i]);
	}
	printk(BIOS_INFO, "\n");

}

static int smbios_data(device_t dev, int *handle, unsigned long *current)
{
	struct smbios_type38 *t = (struct smbios_type38 *)*current;
	int len = sizeof(struct smbios_type38);

	memset(t, 0, sizeof(struct smbios_type38));
	t->type = 38;
	t->handle = *handle++;
	t->length = len;
	t->interface_type = 1; /* KCS */
	t->ipmi_rev = 0x15;
	t->i2c_slave_addr = 0x24;
	t->nv_storage_addr = 0xff;
	t->base_address = 0xca3;
	t->base_address_modifier = 0;
	*current += len;
	return len - 1; /* FIXME: why? */

}

struct chip_operations mainboard_ops = {
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER)
	.enable_dev = mainboard_enable,
	.get_smbios_data = smbios_data,
};

