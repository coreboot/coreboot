/*
 * Copyright (C) 2013 The ChromeOS Authors
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <device/device.h>

#if CONFIG_WRITE_HIGH_TABLES
#include <cbmem.h>
#endif

void hardwaremain(int boot_complete);
void main(void)
{
	console_init();
	printk(BIOS_INFO, "hello from ramstage\n");

#if CONFIG_WRITE_HIGH_TABLES
	/* place at top of physical memory */
	high_tables_size = CONFIG_COREBOOT_TABLES_SIZE;
	high_tables_base = CONFIG_SYS_SDRAM_BASE +
			((CONFIG_DRAM_SIZE_MB << 20UL) * CONFIG_NR_DRAM_BANKS) -
			CONFIG_COREBOOT_TABLES_SIZE;
#endif

	hardwaremain(0);
}

#if 0
static void mainboard_read_resources(device_t dev)
{
	printk(BIOS_DEBUG, "%s: entered\n", __func__);
	/* Report the memory regions */
	ram_resource(dev, 0,
			CONFIG_SYS_SDRAM_BASE + (CONFIG_COREBOOT_ROMSIZE_KB),
			((CONFIG_DRAM_SIZE_MB << 10UL) * CONFIG_NR_DRAM_BANKS) -
			CONFIG_COREBOOT_TABLES_SIZE);
}

static void mainboard_set_resources(device_t dev)
{
	printk(BIOS_DEBUG, "%s: entered\n", __func__);

	assign_resources(dev->link_list);
}

static struct device_operations mainboard_device_ops = {
	.read_resources = mainboard_read_resources,
	.set_resources = mainboard_set_resources,
};
#endif

static void mainboard_enable(device_t dev)
{
	printk(BIOS_DEBUG, "%s: entered\n", __func__);
	//dev->ops = &mainboard_device_ops;
}

struct chip_operations mainboard_ops = {
	.name	= "Samsung/Google ARM ChromeBook",
	.enable_dev = mainboard_enable,
};
