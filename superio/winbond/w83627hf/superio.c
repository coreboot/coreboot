/*
 * This file is part of the coreboot project.
 *
 * Copyright 2000 AG Electronics Ltd.
 * Copyright 2003-2004 Linux Networx
 * Copyright 2004 Tyan
 * By LYH change from PC87360
 * Copyright 2007 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <io.h>
#include <lib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console.h>
#include <string.h>
#include <uart8250.h>
#include <keyboard.h>
#include <statictree.h>
#include "w83627hf.h"

static void pnp_write_index(u16 port_base, u8 reg, u8 value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static u8 pnp_read_index(u16 port_base, u8 reg)
{
	outb(reg, port_base);
	return inb(port_base + 1);
}

static void enable_hwm_smbus(struct device * dev) {
	/* set the pin 91,92 as I2C bus */
	u8 reg, value;
	reg = 0x2b;
	value = pnp_read_config(dev, reg);
	value &= 0x3f;
	pnp_write_config(dev, reg, value);
}

static void init_acpi(struct device * dev)
{
	u8  value = 0x20;
	int power_on = 1;
#warning Fix CMOS handling
	// get_option(&power_on, "power_on_after_fail");
	pnp_enter_8787(dev);
	pnp_write_index(dev->path.pnp.port,7,0x0a);
	value = pnp_read_config(dev, 0xE4);
	value &= ~(3<<5);
	if(power_on) {
		value |= (1<<5);
	}
	pnp_write_config(dev, 0xE4, value);
	pnp_exit_aa(dev);
}

static void init_hwm(u16 base)
{
	u8  reg, value;
	int i;

	unsigned  hwm_reg_values[] = {
	    /* reg, mask, data */
		0x40, 0xff, 0x81,  /* start HWM */
		0x48, 0xaa, 0x2a,  /* set SMBus base to 0x54>>1	*/
		0x4a, 0x21, 0x21,  /* set T2 SMBus base to 0x92>>1 and T3 SMBus base to 0x94>>1 */
		0x4e, 0x80, 0x00,
		0x43, 0x00, 0xff,
		0x44, 0x00, 0x3f,
		0x4c, 0xbf, 0x18,
		0x4d, 0xff, 0x80   /* turn off beep */

	};

	for (i = 0; i < ARRAY_SIZE(hwm_reg_values); i += 3) {
		reg = hwm_reg_values[i];
	 	value = pnp_read_index(base, reg);
		value &= 0xff & hwm_reg_values[i+1];
		value |= 0xff & hwm_reg_values[i+2];

		printk(BIOS_SPEW, "base = 0x%04x, reg = 0x%02x, value = 0x%02x\n", base, reg,value);

		pnp_write_index(base, reg, value);
	}
}

static void w83627hf_init_func(struct device * dev)
{
	struct resource *res0, *res1;

	printk(BIOS_DEBUG, "%s: %s dummy init XXXX\n",__func__, dev->dtsname);

	switch(dev->path.pnp.device) {
	case W83627HF_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
#warning init_uart8250
		printk(BIOS_DEBUG, "%s: Not calling init_uart8250.\n",__func__);
		//init_uart8250(res0->base, &conf->com1);
		break;
	case W83627HF_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
#warning init_uart8250
		printk(BIOS_DEBUG, "%s: Not calling init_uart8250.\n",__func__);
		//init_uart8250(res0->base, &conf->com2);
		break;
	case W83627HF_KBC:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, NULL);
		break;
	case W83627HF_HWM:
		res0 = find_resource(dev, PNP_IDX_IO0);
#define HWM_INDEX_PORT 5
		init_hwm(res0->base + HWM_INDEX_PORT);
		break;
	case W83627HF_ACPI:
		init_acpi(dev);
		break;
	}
}

void w83627hf_pnp_set_resources(struct device * dev)
{
	pnp_enter_8787(dev);
	pnp_set_resources(dev);
	pnp_exit_aa(dev);
}

void w83627hf_pnp_enable_resources(struct device * dev)
{
	pnp_enter_8787(dev);
	pnp_enable_resources(dev);
	switch(dev->path.pnp.device) {
	case W83627HF_HWM:
		printk(BIOS_DEBUG, "w83627hf hwm smbus enabled\n");
		enable_hwm_smbus(dev);
		break;
	}
	pnp_exit_aa(dev);
}

void w83627hf_enable_resources(struct device * dev)
{
	struct device * child;
	for(child = dev->link[0].children; child; child = child->sibling)
		if (child->path.type == DEVICE_PATH_PNP)
			w83627hf_pnp_enable_resources(child);
}

void w83627hf_pnp_enable(struct device * dev)
{
	if (!dev->enabled) {
		pnp_enter_8787(dev);
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
		pnp_exit_aa(dev);
	}
}

static void w83627hf_init(struct device * dev)
{
	struct device * child;
	for(child = dev->link[0].children; child; child = child->sibling)
		/* All children should be PNP. */
		if (child->path.type == DEVICE_PATH_PNP)
			w83627hf_init_func(child);
}

static void phase3_chip_setup_dev(struct device *dev);

struct device_operations w83627hf_ops = {
	.id = {.type = DEVICE_ID_PNP},
	.phase3_chip_setup_dev   = phase3_chip_setup_dev,
	.phase3_enable           = w83627hf_enable_resources,
	.phase4_read_resources   = pnp_read_resources,
	.phase4_set_resources    = w83627hf_pnp_set_resources,
	.phase5_enable_resources = w83627hf_pnp_enable,
	.phase6_init             = w83627hf_init,
};

static struct pnp_info pnp_dev_info[] = {
		/* Ops, function #, All resources needed by dev,  io_info_structs */
	{ &w83627hf_ops, W83627HF_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &w83627hf_ops, W83627HF_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &w83627hf_ops, W83627HF_SP1, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &w83627hf_ops, W83627HF_SP2, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ 0, }, /* No function 4. */
	{ &w83627hf_ops, W83627HF_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
	{ &w83627hf_ops, W83627HF_CIR, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &w83627hf_ops, W83627HF_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7ff, 0 }, {0x7fe, 0x4}, },
	{ &w83627hf_ops, W83627HF_GPIO2, },
	{ &w83627hf_ops, W83627HF_GPIO3, },
	{ &w83627hf_ops, W83627HF_ACPI, },
	{ &w83627hf_ops, W83627HF_HWM, PNP_IO0 | PNP_IRQ0, { 0xff8, 0 }, },
};

static void phase3_chip_setup_dev(struct device *dev)
{
	/* Get dts values and populate pnp_dev_info. */
	const struct superio_winbond_w83627hf_dts_config * const conf = dev->device_configuration;

	/* Floppy */
	pnp_dev_info[W83627HF_FDC].enable = conf->floppyenable;
	pnp_dev_info[W83627HF_FDC].io0.val = conf->floppyio;
	pnp_dev_info[W83627HF_FDC].irq0 = conf->floppyirq;
	pnp_dev_info[W83627HF_FDC].drq0 = conf->floppydrq;

	/* Parallel port */
	pnp_dev_info[W83627HF_PP].enable = conf->ppenable;
	pnp_dev_info[W83627HF_PP].io0.val = conf->ppio;
	pnp_dev_info[W83627HF_PP].irq0 = conf->ppirq;

	/* COM1 */
	pnp_dev_info[W83627HF_SP1].enable = conf->com1enable;
	pnp_dev_info[W83627HF_SP1].io0.val = conf->com1io;
	pnp_dev_info[W83627HF_SP1].irq0 = conf->com1irq;

	/* COM2 */
	pnp_dev_info[W83627HF_SP2].enable = conf->com2enable;
	pnp_dev_info[W83627HF_SP2].io0.val = conf->com2io;
	pnp_dev_info[W83627HF_SP2].irq0 = conf->com2irq;

	/* Keyboard */
	pnp_dev_info[W83627HF_KBC].enable = conf->kbenable;
	pnp_dev_info[W83627HF_KBC].io0.val = conf->kbio;
	pnp_dev_info[W83627HF_KBC].io1.val = conf->kbio2;
	pnp_dev_info[W83627HF_KBC].irq0 = conf->kbirq;
	pnp_dev_info[W83627HF_KBC].irq1 = conf->kbirq2;

	/* Consumer IR */
	pnp_dev_info[W83627HF_CIR].enable = conf->cirenable;

	/* Game port */
	pnp_dev_info[W83627HF_GAME_MIDI_GPIO1].enable = conf->gameenable;
	pnp_dev_info[W83627HF_GAME_MIDI_GPIO1].io0.val = conf->gameio;
	pnp_dev_info[W83627HF_GAME_MIDI_GPIO1].io1.val = conf->gameio2;
	pnp_dev_info[W83627HF_GAME_MIDI_GPIO1].irq0 = conf->gameirq;

	/* GPIO2 */
	pnp_dev_info[W83627HF_GPIO2].enable = conf->gpio2enable;

	/* GPIO3 */
	pnp_dev_info[W83627HF_GPIO3].enable = conf->gpio3enable;

	/* ACPI */
	pnp_dev_info[W83627HF_ACPI].enable = conf->acpienable;

	/* Hardware Monitor */
	pnp_dev_info[W83627HF_HWM].enable = conf->hwmenable;
	pnp_dev_info[W83627HF_HWM].io0.val = conf->hwmio;
	pnp_dev_info[W83627HF_HWM].irq0 = conf->hwmirq;

	/* Initialize SuperIO for PNP children. */
	if (!dev->links) {
		dev->links = 1;
		dev->link[0].dev = dev;
		dev->link[0].children = NULL;
		dev->link[0].link = 0;
	}

	/* Call init with updated tables to create and enable children. */
	pnp_enable_devices(dev, &w83627hf_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}
