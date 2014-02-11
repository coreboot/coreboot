/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include "pch.h"
#include "nvs.h"

/* Enable clock in PCI mode */
static void serialio_enable_clock(struct resource *bar0)
{
	u32 reg32 = read32(bar0->base + SIO_REG_PPR_CLOCK);
	reg32 |= SIO_REG_PPR_CLOCK_EN;
	write32(bar0->base + SIO_REG_PPR_CLOCK, reg32);
}

/* Put Serial IO D21:F0-F6 device into desired mode. */
static void serialio_d21_mode(int sio_index, int int_pin, int acpi_mode)
{
	u32 portctrl = SIO_IOBP_PORTCTRL_PM_CAP_PRSNT;

	/* Snoop select 1. */
	portctrl |= SIO_IOBP_PORTCTRL_SNOOP_SELECT(1);

	/* Set interrupt pin. */
	portctrl |= SIO_IOBP_PORTCTRL_INT_PIN(int_pin);

	if (acpi_mode) {
		/* Enable ACPI interrupt mode. */
		portctrl |= SIO_IOBP_PORTCTRL_ACPI_IRQ_EN;

		/* Disable PCI config space. */
		portctrl |= SIO_IOBP_PORTCTRL_PCI_CONF_DIS;
	}

	pch_iobp_update(SIO_IOBP_PORTCTRLX(sio_index), 0, portctrl);
}

/* Put Serial IO D23:F0 device into desired mode. */
static void serialio_d23_mode(int acpi_mode)
{
	u32 portctrl = 0;

	/* Snoop select 1. */
	pch_iobp_update(SIO_IOBP_PORTCTRL1, 0,
			SIO_IOBP_PORTCTRL1_SNOOP_SELECT(1));

	if (acpi_mode) {
		/* Enable ACPI interrupt mode. */
		portctrl |= SIO_IOBP_PORTCTRL0_ACPI_IRQ_EN;

		/* Disable PCI config space. */
		portctrl |= SIO_IOBP_PORTCTRL0_PCI_CONF_DIS;
	}

	pch_iobp_update(SIO_IOBP_PORTCTRL0, 0, portctrl);
}

/* Enable LTR Auto Mode for D21:F1-F6. */
static void serialio_d21_ltr(struct resource *bar0)
{
	u32 reg;

	/* 1. Program BAR0 + 808h[2] = 0b */
	reg = read32(bar0->base + SIO_REG_PPR_GEN);
	reg &= ~SIO_REG_PPR_GEN_LTR_MODE_MASK;
	write32(bar0->base + SIO_REG_PPR_GEN, reg);

	/* 2. Program BAR0 + 804h[1:0] = 00b */
	reg = read32(bar0->base + SIO_REG_PPR_RST);
	reg &= ~SIO_REG_PPR_RST_ASSERT;
	write32(bar0->base + SIO_REG_PPR_RST, reg);

	/* 3. Program BAR0 + 804h[1:0] = 11b */
	reg = read32(bar0->base + SIO_REG_PPR_RST);
	reg |= SIO_REG_PPR_RST_ASSERT;
	write32(bar0->base + SIO_REG_PPR_RST, reg);

	/* 4. Program BAR0 + 814h[31:0] = 00000000h */
	write32(bar0->base + SIO_REG_AUTO_LTR, 0);
}

/* Enable LTR Auto Mode for D23:F0. */
static void serialio_d23_ltr(struct resource *bar0)
{
	u32 reg;

	/* Program BAR0 + 1008h[2] = 1b */
	reg = read32(bar0->base + SIO_REG_SDIO_PPR_GEN);
	reg |= SIO_REG_PPR_GEN_LTR_MODE_MASK;
	write32(bar0->base + SIO_REG_SDIO_PPR_GEN, reg);

	/* Program BAR0 + 1010h = 0x00000000 */
	write32(bar0->base + SIO_REG_SDIO_PPR_SW_LTR, 0);

	/* Program BAR0 + 3Ch[30] = 1b */
	reg = read32(bar0->base + SIO_REG_SDIO_PPR_CMD12);
	reg |= SIO_REG_SDIO_PPR_CMD12_B30;
	write32(bar0->base + SIO_REG_SDIO_PPR_CMD12, reg);
}

/* Select I2C voltage of 1.8V or 3.3V. */
static void serialio_i2c_voltage_sel(struct resource *bar0, u8 voltage)
{
	u32 reg32 = read32(bar0->base + SIO_REG_PPR_GEN);
	reg32 &= ~SIO_REG_PPR_GEN_VOLTAGE_MASK;
	reg32 |= SIO_REG_PPR_GEN_VOLTAGE(voltage);
	write32(bar0->base + SIO_REG_PPR_GEN, reg32);
}

/* Init sequence to be run once, done as part of D21:F0 (SDMA) init. */
static void serialio_init_once(int acpi_mode)
{
	if (acpi_mode) {
		/* Enable ACPI IRQ for IRQ13, IRQ7, IRQ6, IRQ5 in RCBA. */
		RCBA32_OR(ACPIIRQEN, (1 << 13)|(1 << 7)|(1 << 6)|(1 << 5));
	}

	/* Program IOBP CB000154h[12,9:8,4:0] = 1001100011111b. */
	pch_iobp_update(SIO_IOBP_GPIODF, ~0x0000131f, 0x0000131f);

	/* Program IOBP CB000180h[5:0] = 111111b (undefined register) */
	pch_iobp_update(0xcb000180, ~0x0000003f, 0x0000003f);
}

static void serialio_init(struct device *dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
	struct resource *bar0, *bar1;
	int sio_index = -1;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing Serial IO device\n");

	/* Ensure memory and bus master are enabled */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Find BAR0 and BAR1 */
	bar0 = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!bar0)
		return;
	bar1 = find_resource(dev, PCI_BASE_ADDRESS_1);
	if (!bar1)
		return;

	if (!config->sio_acpi_mode)
		serialio_enable_clock(bar0);

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(21, 0): /* SDMA */
		sio_index = SIO_ID_SDMA;
		serialio_init_once(config->sio_acpi_mode);
		serialio_d21_mode(sio_index, SIO_PIN_INTB,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 1): /* I2C0 */
		sio_index = SIO_ID_I2C0;
		serialio_d21_ltr(bar0);
		serialio_i2c_voltage_sel(bar0, config->sio_i2c0_voltage);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 2): /* I2C1 */
		sio_index = SIO_ID_I2C1;
		serialio_d21_ltr(bar0);
		serialio_i2c_voltage_sel(bar0, config->sio_i2c1_voltage);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 3): /* SPI0 */
		sio_index = SIO_ID_SPI0;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 4): /* SPI1 */
		sio_index = SIO_ID_SPI1;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 5): /* UART0 */
		sio_index = SIO_ID_UART0;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTD,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(21, 6): /* UART1 */
		sio_index = SIO_ID_UART1;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTD,
				  config->sio_acpi_mode);
		break;
	case PCI_DEVFN(23, 0): /* SDIO */
		sio_index = SIO_ID_SDIO;
		serialio_d23_ltr(bar0);
		serialio_d23_mode(config->sio_acpi_mode);
		break;
	default:
		return;
	}

	if (config->sio_acpi_mode) {
		global_nvs_t *gnvs;

		/* Find ACPI NVS to update BARs */
		gnvs = (global_nvs_t *)cbmem_find(CBMEM_ID_ACPI_GNVS);
		if (!gnvs) {
			printk(BIOS_ERR, "Unable to locate Global NVS\n");
			return;
		}

		/* Save BAR0 and BAR1 to ACPI NVS */
		gnvs->s0b[sio_index] = (u32)bar0->base;
		gnvs->s1b[sio_index] = (u32)bar1->base;
	}
}

static void serialio_set_subsystem(device_t dev, unsigned vendor,
				   unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem		= serialio_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= serialio_init,
	.ops_pci		= &pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9c60, /* 0:15.0 - SDMA */
	0x9c61, /* 0:15.1 - I2C0 */
	0x9c62, /* 0:15.2 - I2C1 */
	0x9c65, /* 0:15.3 - SPI0 */
	0x9c66, /* 0:15.4 - SPI1 */
	0x9c63, /* 0:15.5 - UART0 */
	0x9c64, /* 0:15.6 - UART1 */
	0x9c35, /* 0:17.0 - SDIO */
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
