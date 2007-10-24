/*
 * This file is part of the LinuxBIOS project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/*
 * (C) 2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "i82801db.h"

#define ACPI_BAR 0x40
#define GPIO_BAR 0x58

#define NMI_OFF 0
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

static void setup_ioapic(void)
{
	int i;
	unsigned long value_low, value_high;
	unsigned long ioapic_base = 0xfec00000;
	volatile unsigned long *l;
	unsigned interrupts;

	l = (unsigned long *) ioapic_base;

	l[0] = 0x01;
	interrupts = (l[04] >> 16) & 0xff;
	for (i = 0; i < interrupts; i++) {
		l[0] = (i * 2) + 0x10;
		l[4] = DISABLED;
		value_low = l[4];
		l[0] = (i * 2) + 0x11;
		l[4] = NONE; /* Should this be an address? */
		value_high = l[4];
		if (value_low == 0xffffffff) {
			printk_warning("IO APIC not responding.\n");
			return;
		}
	}

	/* Put the ioapic in virtual wire mode */
	l[0] = 0 + 0x10;
	l[4] = ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
}

#define SERIRQ_CNTL 0x64
static void i82801db_enable_serial_irqs(device_t dev)
{
	/* set packet length and toggle silent mode bit */
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(0 << 6)|((21 - 17) << 2)|(0 << 0));
}

#define PCI_DMA_CFG 0x90
static void i82801db_pci_dma_cfg(device_t dev)
{
	/* Set PCI DMA CFG to lpc I/F DMA 
	pci_write_config16(dev, PCI_DMA_CFG, 0xfcff); */
	pci_write_config16(dev, PCI_DMA_CFG, 0x5455);
}

#define LPC_EN 0xe6
static void i82801db_enable_lpc(device_t dev)
{
        /* lpc i/f enable 
        pci_write_config8(dev, LPC_EN, 0x0d); */
        pci_write_config16(dev, LPC_EN, 0x3403);
}

typedef struct southbridge_intel_i82801db_config config_t;

static void set_i82801db_gpio_use_sel(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_use_sel, gpio_use_sel2;
	int i;

	gpio_use_sel  = 0x1A003180;
	gpio_use_sel2 = 0x00000007;
	for(i = 0; i < 64; i++) {
		int val;
		switch(config->gpio[i] & ICH5R_GPIO_USE_MASK) {
		case ICH5R_GPIO_USE_AS_NATIVE: val = 0; break;
		case ICH5R_GPIO_USE_AS_GPIO:   val = 1; break;
		default:
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_use_sel  &= ~( 1 << i);
			gpio_use_sel  |= (val << i);
		} else {
			gpio_use_sel2 &= ~( 1 << (i - 32));
			gpio_use_sel2 |= (val << (i - 32));
		}
	}
	outl(gpio_use_sel,  res->base + 0x00);
	outl(gpio_use_sel2, res->base + 0x30);
}

static void set_i82801db_gpio_direction(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_io_sel, gpio_io_sel2;
	int i;

	gpio_io_sel  = 0x0000ffff;
	gpio_io_sel2 = 0x00000300;
	for(i = 0; i < 64; i++) {
		int val;
		switch(config->gpio[i] & ICH5R_GPIO_SEL_MASK) {
		case ICH5R_GPIO_SEL_OUTPUT: val = 0; break;
		case ICH5R_GPIO_SEL_INPUT:  val = 1; break;
		default: 
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_io_sel  &= ~( 1 << i);
			gpio_io_sel  |= (val << i);
		} else {
			gpio_io_sel2 &= ~( 1 << (i - 32));
			gpio_io_sel2 |= (val << (i - 32));
		}
	}
	outl(gpio_io_sel,  res->base + 0x04);
	outl(gpio_io_sel2, res->base + 0x34);
}

static void set_i82801db_gpio_level(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_lvl, gpio_lvl2;
	uint32_t gpio_blink;
	int i;

	gpio_lvl   = 0x1b3f0000;
	gpio_blink = 0x00040000;
	gpio_lvl2  = 0x00030207;
	for(i = 0; i < 64; i++) {
		int val, blink;
		switch(config->gpio[i] & ICH5R_GPIO_LVL_MASK) {
		case ICH5R_GPIO_LVL_LOW:   val = 0; blink = 0; break;
		case ICH5R_GPIO_LVL_HIGH:  val = 1; blink = 0; break;
		case ICH5R_GPIO_LVL_BLINK: val = 1; blink = 1; break;
		default: 
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_lvl   &= ~(   1 << i);
			gpio_blink &= ~(   1 << i);
			gpio_lvl   |= (  val << i);
			gpio_blink |= (blink << i);
		} else {
			gpio_lvl2  &= ~( 1 << (i - 32));
			gpio_lvl2  |= (val << (i - 32));
		}
	}
	outl(gpio_lvl,   res->base + 0x0c);
	outl(gpio_blink, res->base + 0x18);
	outl(gpio_lvl2,  res->base + 0x38);
}

static void set_i82801db_gpio_inv(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_inv;
	int i;

	gpio_inv   = 0x00000000;
	for(i = 0; i < 32; i++) {
		int val;
		switch(config->gpio[i] & ICH5R_GPIO_INV_MASK) {
		case ICH5R_GPIO_INV_OFF: val = 0; break;
		case ICH5R_GPIO_INV_ON:  val = 1; break;
		default: 
			continue;
		}
		gpio_inv &= ~( 1 << i);
		gpio_inv |= (val << i);
	}
	outl(gpio_inv,   res->base + 0x2c);
}

static void i82801db_pirq_init(device_t dev)
{
	config_t *config;

	/* Get the chip configuration */
	config = dev->chip_info;

	if(config->pirq_a_d) {
		printk_debug("\n\nIRQs A to D = %04x\n\n", config->pirq_a_d);
		pci_write_config32(dev, 0x60, config->pirq_a_d);
	}
	if(config->pirq_e_h) {
		printk_debug("\n\nIRQs e to h = %04x\n\n", config->pirq_e_h);
		pci_write_config32(dev, 0x68, config->pirq_e_h);
	}
}


static void i82801db_gpio_init(device_t dev)
{
	struct resource *res;
	config_t *config;

	/* Skip if I don't have any configuration */
	if (!dev->chip_info) {
		return;
	}
	/* The programmer is responsible for ensuring
	 * a valid gpio configuration.
	 */

	/* Get the chip configuration */
	config = dev->chip_info;
	/* Find the GPIO bar */
	res = find_resource(dev, GPIO_BAR);
	if (!res) {
		return; 
	}

	/* Set the use selects */
	set_i82801db_gpio_use_sel(dev, res, config);

	/* Set the IO direction */
	set_i82801db_gpio_direction(dev, res, config);

	/* Setup the input inverters */
	set_i82801db_gpio_inv(dev, res, config);

	/* Set the value on the GPIO output pins */
	set_i82801db_gpio_level(dev, res, config);

}

static void enable_hpet(struct device *dev)
{
const unsigned long hpet_address = 0xfed0000;

	uint32_t dword;
	uint32_t code = (0 & 0x3);

	dword = pci_read_config32(dev, GEN_CNTL);
	dword |= (1 << 17); /* enable hpet */

	/* Bits [16:15]  Memory Address Range
	 *	    00   FED0_0000h - FED0_03FFh
	 *	    01   FED0_1000h - FED0_13FFh
	 *	    10   FED0_2000h - FED0_23FFh
	 *	    11   FED0_3000h - FED0_33FFh
	 */

	dword &= ~(3 << 15); /* clear it */
	dword |= (code<<15);

	printk_debug("enabling HPET @0x%x\n", hpet_address | (code <<12) );
}

static void lpc_init(struct device *dev)
{
	uint8_t byte;
	uint32_t value;
	int pwr_on=MAINBOARD_POWER_ON_AFTER_POWER_FAIL;

	/* IO APIC initialization */
	value = pci_read_config32(dev, 0xd0);
	value |= (1 << 8)|(1<<7)|(1<<1);
	pci_write_config32(dev, 0xd0, value);
	value = pci_read_config32(dev, 0xd4);
	value |= (1<<1);
	pci_write_config32(dev, 0xd4, value);
	setup_ioapic();

	i82801db_enable_serial_irqs(dev);

	i82801db_pci_dma_cfg(dev);

	i82801db_enable_lpc(dev);

	/* Clear SATA to non raid
	pci_write_config8(dev, 0xae, 0x00);
	*/
	
        get_option(&pwr_on, "power_on_after_fail");
	byte = pci_read_config8(dev, 0xa4);
	byte &= 0xfe;
	if (!pwr_on) {
		byte |= 1;
	}
	pci_write_config8(dev, 0xa4, byte);
	printk_info("set power %s after power fail\n", pwr_on?"on":"off");

	/* Set up the PIRQ */
	i82801db_pirq_init(dev);
	
	/* Set the state of the gpio lines */
	i82801db_gpio_init(dev);

	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	/* Disable IDE (needed when sata is enabled)
	pci_write_config8(dev, 0xf2, 0x60);
	*/
		
	enable_hpet(dev);
}

static void i82801db_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add the ACPI BAR */
	res = pci_get_resource(dev, ACPI_BAR);

	/* Add the GPIO BAR */
	res = pci_get_resource(dev, GPIO_BAR);

	/* Add an extra subtractive resource for both memory and I/O */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void i82801db_lpc_enable_resources(device_t dev)
{
	uint8_t acpi_cntl, gpio_cntl;

	/* Enable the normal pci resources */
	pci_dev_enable_resources(dev);

	/* Enable the ACPI bar */
	acpi_cntl = pci_read_config8(dev, 0x44);
	acpi_cntl |= (1 << 4);
	pci_write_config8(dev, 0x44, acpi_cntl);
	
	/* Enable the GPIO bar */
	gpio_cntl = pci_read_config8(dev, 0x5c);
	gpio_cntl |= (1 << 4);
	pci_write_config8(dev, 0x5c, gpio_cntl);

	enable_childrens_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations lpc_ops  = {
	.read_resources   = i82801db_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = i82801db_lpc_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_static_bus,
	.enable           = i82801db_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_ISA,
};
