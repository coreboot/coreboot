/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/* This code is based on src/southbridge/intel/esb6300/esb6300_lpc.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include "i3100.h"

#define ACPI_BAR 0x40
#define GPIO_BAR 0x48
#define RCBA 0xf0

#define SERIRQ_CNTL 0x64

#define GEN_PMCON_1 0xA0
#define GEN_PMCON_2 0xA2
#define GEN_PMCON_3 0xA4

#define NMI_OFF 0
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void i3100_enable_serial_irqs(device_t dev)
{
	/* set packet length and toggle silent mode bit */
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(0 << 6)|((21 - 17) << 2)|(0 << 0));
}

typedef struct southbridge_intel_i3100_config config_t;

static void set_i3100_gpio_use_sel(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_use_sel, gpio_use_sel2;
	int i;

	gpio_use_sel = inl(res->base + 0x00) | 0x0000c603;
	gpio_use_sel2 = inl(res->base + 0x30) | 0x00000100;
	for (i = 0; i < 64; i++) {
		int val;
		switch (config->gpio[i] & I3100_GPIO_USE_MASK) {
		case I3100_GPIO_USE_AS_NATIVE:
			val = 0;
			break;
		case I3100_GPIO_USE_AS_GPIO:
			val = 1;
			break;
		default:
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_use_sel &= ~(1 << i);
			gpio_use_sel |= (val << i);
		} else {
			gpio_use_sel2 &= ~(1 << (i - 32));
			gpio_use_sel2 |= (val << (i - 32));
		}
	}
	outl(gpio_use_sel, res->base + 0x00);
	outl(gpio_use_sel2, res->base + 0x30);
}

static void set_i3100_gpio_direction(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_io_sel, gpio_io_sel2;
	int i;

	gpio_io_sel = inl(res->base + 0x04);
	gpio_io_sel2 = inl(res->base + 0x34);
	for (i = 0; i < 64; i++) {
		int val;
		switch (config->gpio[i] & I3100_GPIO_SEL_MASK) {
		case I3100_GPIO_SEL_OUTPUT:
			val = 0;
			break;
		case I3100_GPIO_SEL_INPUT:
			val = 1;
			break;
		default:
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_io_sel &= ~(1 << i);
			gpio_io_sel |= (val << i);
		} else {
			gpio_io_sel2 &= ~(1 << (i - 32));
			gpio_io_sel2 |= (val << (i - 32));
		}
	}
	outl(gpio_io_sel, res->base + 0x04);
	outl(gpio_io_sel2, res->base + 0x34);
}

static void set_i3100_gpio_level(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_lvl, gpio_lvl2;
	u32 gpio_blink;
	int i;

	gpio_lvl = inl(res->base + 0x0c);
	gpio_blink = inl(res->base + 0x18);
	gpio_lvl2 = inl(res->base + 0x38);
	for (i = 0; i < 64; i++) {
		int val, blink;
		switch (config->gpio[i] & I3100_GPIO_LVL_MASK) {
		case I3100_GPIO_LVL_LOW:
			val = 0;
			blink = 0;
			break;
		case I3100_GPIO_LVL_HIGH:
			val = 1;
			blink = 0;
			break;
		case I3100_GPIO_LVL_BLINK:
			val = 1;
			blink = 1;
			break;
		default:
			continue;
		}
		/* The caller is responsible for not playing with unimplemented bits */
		if (i < 32) {
			gpio_lvl &= ~(1 << i);
			gpio_blink &= ~(1 << i);
			gpio_lvl |= (val << i);
			gpio_blink |= (blink << i);
		} else {
			gpio_lvl2 &= ~(1 << (i - 32));
			gpio_lvl2 |= (val << (i - 32));
		}
	}
	outl(gpio_lvl, res->base + 0x0c);
	outl(gpio_blink, res->base + 0x18);
	outl(gpio_lvl2, res->base + 0x38);
}

static void set_i3100_gpio_inv(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_inv;
	int i;

	gpio_inv = inl(res->base + 0x2c);
	for (i = 0; i < 32; i++) {
		int val;
		switch (config->gpio[i] & I3100_GPIO_INV_MASK) {
		case I3100_GPIO_INV_OFF:
			val = 0;
			break;
		case I3100_GPIO_INV_ON:
			val = 1;
			break;
		default:
			continue;
		}
		gpio_inv &= ~(1 << i);
		gpio_inv |= (val << i);
	}
	outl(gpio_inv, res->base + 0x2c);
}

static void i3100_pirq_init(device_t dev)
{
	device_t irq_dev;
	config_t *config;

	/* Get the chip configuration */
	config = dev->chip_info;

	if(config->pirq_a_d)
		pci_write_config32(dev, 0x60, config->pirq_a_d);

	if(config->pirq_e_h)
		pci_write_config32(dev, 0x68, config->pirq_e_h);

        for(irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
                u8 int_pin=0, int_line=0;

                if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
                        continue;

                int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);
                switch (int_pin) {
                case 1: /* INTA# */
			int_line = config->pirq_a_d & 0xff;
			break;

                case 2: /* INTB# */
			int_line = (config->pirq_a_d >> 8) & 0xff;
			break;

		case 3: /* INTC# */
			int_line = (config->pirq_a_d >> 16) & 0xff;
			break;

                case 4: /* INTD# */
			int_line = (config->pirq_a_d >> 24) & 0xff;
			break;
                }

                if (!int_line)
                        continue;

		printk(BIOS_DEBUG, "%s: irq pin %d, irq line %d\n", dev_path(irq_dev), int_pin, int_line);
                pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
        }


}

static void i3100_power_options(device_t dev) {
  u8 reg8;
  u16 reg16;
  int pwr_on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
  int nmi_option;

  /* Which state do we want to goto after g3 (power restored)?
   * 0 == S0 Full On
   * 1 == S5 Soft Off
   */
  get_option(&pwr_on, "power_on_after_fail");
  reg8 = pci_read_config8(dev, GEN_PMCON_3);
  reg8 &= 0xfe;
  if (pwr_on) {
    reg8 &= ~1;
  } else {
    reg8 |= 1;
  }
  /* avoid #S4 assertions */
  reg8 |= (3 << 4);
  /* minimum asssertion is 1 to 2 RTCCLK */
  reg8 &= ~(1 << 3);
  pci_write_config8(dev, GEN_PMCON_3, reg8);
  printk(BIOS_INFO, "set power %s after power fail\n", pwr_on ? "on" : "off");

  /* Set up NMI on errors. */
  reg8 = inb(0x61);
  /* Higher Nibble must be 0 */
  reg8 &= 0x0f;
  /* IOCHK# NMI Enable */
  reg8 &= ~(1 << 3);
  /* PCI SERR# Enable */
  // reg8 &= ~(1 << 2);
  /* PCI SERR# Disable for now */
  reg8 |= (1 << 2);
  outb(reg8, 0x61);

  reg8 = inb(0x70);
  nmi_option = NMI_OFF;
  get_option(&nmi_option, "nmi");
  if (nmi_option) {
    /* Set NMI. */
    printk(BIOS_INFO, "NMI sources enabled.\n");
    reg8 &= ~(1 << 7);
  } else {
    /* Can't mask NMI from PCI-E and NMI_NOW */
    printk(BIOS_INFO, "NMI sources disabled.\n");
    reg8 |= ( 1 << 7);
  }
  outb(reg8, 0x70);

  // Enable CPU_SLP# and Intel Speedstep, set SMI# rate down
  reg16 = pci_read_config16(dev, GEN_PMCON_1);
  reg16 &= ~((3 << 0) | (1 << 10));
  reg16 |= (1 << 3) | (1 << 5);
  /* CLKRUN_EN */
  // reg16 |= (1 << 2);
  pci_write_config16(dev, GEN_PMCON_1, reg16);

  // Set the board's GPI routing.
  // i82801gx_gpi_routing(dev);
}

static void i3100_gpio_init(device_t dev)
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
	set_i3100_gpio_use_sel(dev, res, config);

	/* Set the IO direction */
	set_i3100_gpio_direction(dev, res, config);

	/* Setup the input inverters */
	set_i3100_gpio_inv(dev, res, config);

	/* Set the value on the GPIO output pins */
	set_i3100_gpio_level(dev, res, config);

}


static void lpc_init(struct device *dev)
{
	struct resource *res;

	/* Enable IO APIC */
	res = find_resource(dev, RCBA);
	if (!res) {
		return;
	}
	*((u8 *)((u32)res->base + 0x31ff)) |= (1 << 0);

	// TODO this code sets int 0 of the IOAPIC in Virtual Wire Mode
	// (register 0x10/0x11) while the old code used int 1 (register 0x12)
	// ... Why?
	setup_ioapic(IO_APIC_ADDR, 0); // Don't rename IOAPIC ID

	/* Decode 0xffc00000 - 0xffffffff to fwh idsel 0 */
	pci_write_config32(dev, 0xd0, 0x00000000);

	i3100_enable_serial_irqs(dev);

	/* Set up the PIRQ */
	i3100_pirq_init(dev);

	/* Setup power options */
	i3100_power_options(dev);

	/* Set the state of the gpio lines */
	i3100_gpio_init(dev);

	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
}

static void i3100_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add the ACPI BAR */
	res = pci_get_resource(dev, ACPI_BAR);

	/* Add the GPIO BAR */
	res = pci_get_resource(dev, GPIO_BAR);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Add resource for RCBA */
	res = new_resource(dev, RCBA);
	res->size = 0x4000;
	res->limit = 0xffffc000;
	res->align = 14;
	res->gran = 14;
	res->flags = IORESOURCE_MEM;
}

static void i3100_lpc_enable_resources(device_t dev)
{
	u8 acpi_cntl, gpio_cntl;

	/* Enable the normal pci resources */
	pci_dev_enable_resources(dev);

	/* Enable the ACPI bar */
	acpi_cntl = pci_read_config8(dev, 0x44);
	acpi_cntl |= (1 << 7);
	pci_write_config8(dev, 0x44, acpi_cntl);

	/* Enable the GPIO bar */
	gpio_cntl = pci_read_config8(dev, 0x4c);
	gpio_cntl |= (1 << 4);
	pci_write_config8(dev, 0x4c, gpio_cntl);

	/* Enable the RCBA */
	pci_write_config32(dev, RCBA, pci_read_config32(dev, RCBA) | (1 << 0));
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations lpc_ops  = {
	.read_resources   = i3100_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = i3100_lpc_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_static_bus,
	.enable           = i3100_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_LPC,
};

static const struct pci_driver lpc_driver_ep80579 __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_EP80579_LPC,
};
