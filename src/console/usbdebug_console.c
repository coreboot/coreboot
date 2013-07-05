/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <string.h>
#include <console/console.h>
#include <usbdebug.h>
#include <device/pci.h>
#include <pc80/mc146818rtc.h>

static struct ehci_debug_info dbg_info;
static struct device_operations *ehci_drv_ops;
static struct device_operations ehci_dbg_ops;

static void usbdebug_re_enable(unsigned ehci_base)
{
	unsigned diff;

	if (!dbg_info.ehci_debug)
		return;

	diff = (unsigned)dbg_info.ehci_caps - ehci_base;
	dbg_info.ehci_regs -= diff;
	dbg_info.ehci_debug -= diff;
	dbg_info.ehci_caps = (void*)ehci_base;
	dbg_info.status |= DBGP_EP_ENABLED;
}

static void usbdebug_disable(void)
{
	dbg_info.status &= ~DBGP_EP_ENABLED;
}

static void pci_ehci_set_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "%s EHCI Debug Port hook triggered\n", dev_path(dev));
	usbdebug_disable();

	if (ehci_drv_ops->set_resources)
		ehci_drv_ops->set_resources(dev);
	res = find_resource(dev, EHCI_BAR_INDEX);
	if (!res)
		return;

	usbdebug_re_enable((u32)res->base);
	report_resource_stored(dev, res, "");
	printk(BIOS_DEBUG, "%s EHCI Debug Port relocated\n", dev_path(dev));
}

void pci_ehci_read_resources(struct device *dev)
{
	if (!ehci_drv_ops) {
		memcpy(&ehci_dbg_ops, dev->ops, sizeof(ehci_dbg_ops));
		ehci_drv_ops = dev->ops;
		ehci_dbg_ops.set_resources = pci_ehci_set_resources;
		dev->ops = &ehci_dbg_ops;
		printk(BIOS_DEBUG, "%s EHCI BAR hook registered\n", dev_path(dev));
	} else {
		printk(BIOS_DEBUG, "More than one caller of %s from %s\n", __func__, dev_path(dev));
	}

	pci_dev_read_resources(dev);
}

static void dbgp_init(void)
{
#if !CONFIG_EARLY_CONSOLE
	enable_usbdebug(CONFIG_USBDEBUG_DEFAULT_PORT);
#endif
	usbdebug_init(CONFIG_EHCI_BAR, CONFIG_EHCI_DEBUG_OFFSET, &dbg_info);
}

static void dbgp_tx_byte(unsigned char data)
{
	usbdebug_tx_byte(&dbg_info, data);
}

static unsigned char dbgp_rx_byte(void)
{
	unsigned char data = 0xff;

	if (dbgp_ep_is_active(&dbg_info))
		dbgp_bulk_read_x(&dbg_info, &data, 1);

	return data;
}

static void dbgp_tx_flush(void)
{
	usbdebug_tx_flush(&dbg_info);
}

static int dbgp_tst_byte(void)
{
	return dbgp_ep_is_active(&dbgp_info);
}

static const struct console_driver usbdebug_direct_console __console = {
	.init     = dbgp_init,
	.tx_byte  = dbgp_tx_byte,
	.tx_flush = dbgp_tx_flush,
	.rx_byte  = dbgp_rx_byte,
	.tst_byte = dbgp_tst_byte,
};
