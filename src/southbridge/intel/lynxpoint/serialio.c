/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/nvs.h>
#include <types.h>
#include "chip.h"
#include "iobp.h"
#include "pch.h"

/* Enable clock in PCI mode */
static void serialio_enable_clock(struct resource *bar0)
{
	u32 reg32 = read32(res2mmio(bar0, SIO_REG_PPR_CLOCK, 0));
	reg32 |= SIO_REG_PPR_CLOCK_EN;
	write32(res2mmio(bar0, SIO_REG_PPR_CLOCK, 0), reg32);
}

static bool serialio_uart_is_debug(struct device *dev)
{
	if (CONFIG(SERIALIO_UART_CONSOLE)) {
		switch (dev->path.pci.devfn) {
		case PCH_DEVFN_UART0:
			return CONFIG_UART_FOR_CONSOLE == 0;
		case PCH_DEVFN_UART1:
			return CONFIG_UART_FOR_CONSOLE == 1;
		}
	}
	return 0;
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
	reg = read32(res2mmio(bar0, SIO_REG_PPR_GEN, 0));
	reg &= ~SIO_REG_PPR_GEN_LTR_MODE_MASK;
	write32(res2mmio(bar0, SIO_REG_PPR_GEN, 0), reg);

	/* 2. Program BAR0 + 804h[1:0] = 00b */
	reg = read32(res2mmio(bar0, SIO_REG_PPR_RST, 0));
	reg &= ~SIO_REG_PPR_RST_ASSERT;
	write32(res2mmio(bar0, SIO_REG_PPR_RST, 0), reg);

	/* 3. Program BAR0 + 804h[1:0] = 11b */
	reg = read32(res2mmio(bar0, SIO_REG_PPR_RST, 0));
	reg |= SIO_REG_PPR_RST_ASSERT;
	write32(res2mmio(bar0, SIO_REG_PPR_RST, 0), reg);

	/* 4. Program BAR0 + 814h[31:0] = 00000000h */
	write32(res2mmio(bar0, SIO_REG_AUTO_LTR, 0), 0);
}

/* Enable LTR Auto Mode for D23:F0. */
static void serialio_d23_ltr(struct resource *bar0)
{
	u32 reg;

	/* Program BAR0 + 1008h[2] = 1b */
	reg = read32(res2mmio(bar0, SIO_REG_SDIO_PPR_GEN, 0));
	reg |= SIO_REG_PPR_GEN_LTR_MODE_MASK;
	write32(res2mmio(bar0, SIO_REG_SDIO_PPR_GEN, 0), reg);

	/* Program BAR0 + 1010h = 0x00000000 */
	write32(res2mmio(bar0, SIO_REG_SDIO_PPR_SW_LTR, 0), 0);

	/* Program BAR0 + 3Ch[30] = 1b */
	reg = read32(res2mmio(bar0, SIO_REG_SDIO_PPR_CMD12, 0));
	reg |= SIO_REG_SDIO_PPR_CMD12_B30;
	write32(res2mmio(bar0, SIO_REG_SDIO_PPR_CMD12, 0), reg);
}

/* Select I2C voltage of 1.8V or 3.3V. */
static void serialio_i2c_voltage_sel(struct resource *bar0, u8 voltage)
{
	u32 reg32 = read32(res2mmio(bar0, SIO_REG_PPR_GEN, 0));
	reg32 &= ~SIO_REG_PPR_GEN_VOLTAGE_MASK;
	reg32 |= SIO_REG_PPR_GEN_VOLTAGE(voltage);
	write32(res2mmio(bar0, SIO_REG_PPR_GEN, 0), reg32);
}

/* Init sequence to be run once, done as part of D21:F0 (SDMA) init. */
static void serialio_init_once(int acpi_mode)
{
	if (acpi_mode) {
		/* Enable ACPI IRQ for IRQ13, IRQ7, IRQ6, IRQ5 in RCBA. */
		RCBA32_OR(ACPIIRQEN, (1 << 13) | (1 << 7) | (1 << 6) | (1 << 5));
	}

	/* Program IOBP CB000154h[12,9:8,4:0] = 1001100011111b. */
	pch_iobp_update(SIO_IOBP_GPIODF, ~0x0000131f, 0x0000131f);

	/* Program IOBP CB000180h[5:0] = 111111b (undefined register) */
	pch_iobp_update(0xcb000180, ~0x0000003f, 0x0000003f);
}

static void update_bars(int sio_index, u32 bar0, u32 bar1)
{
	/* Find ACPI NVS to update BARs */
	struct global_nvs *gnvs = acpi_get_gnvs();
	if (!gnvs)
		return;

	gnvs->s0b[sio_index] = bar0;
	gnvs->s1b[sio_index] = bar1;
}

static void serialio_init(struct device *dev)
{
	struct southbridge_intel_lynxpoint_config *config = config_of(dev);
	struct resource *bar0, *bar1;
	int sio_index = -1;

	printk(BIOS_DEBUG, "Initializing Serial IO device\n");

	/* Ensure memory and bus master are enabled */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Find BAR0 and BAR1 */
	bar0 = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!bar0)
		return;
	bar1 = probe_resource(dev, PCI_BASE_ADDRESS_1);
	if (!bar1)
		return;

	if (!config->sio_acpi_mode)
		serialio_enable_clock(bar0);

	switch (dev->path.pci.devfn) {
	case PCH_DEVFN_SDMA: /* SDMA */
		sio_index = SIO_ID_SDMA;
		serialio_init_once(config->sio_acpi_mode);
		serialio_d21_mode(sio_index, SIO_PIN_INTB,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_I2C0: /* I2C0 */
		sio_index = SIO_ID_I2C0;
		serialio_d21_ltr(bar0);
		serialio_i2c_voltage_sel(bar0, config->sio_i2c0_voltage);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_I2C1: /* I2C1 */
		sio_index = SIO_ID_I2C1;
		serialio_d21_ltr(bar0);
		serialio_i2c_voltage_sel(bar0, config->sio_i2c1_voltage);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_SPI0: /* SPI0 */
		sio_index = SIO_ID_SPI0;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_SPI1: /* SPI1 */
		sio_index = SIO_ID_SPI1;
		serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTC,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_UART0: /* UART0 */
		sio_index = SIO_ID_UART0;
		if (!serialio_uart_is_debug(dev))
			serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTD,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_UART1: /* UART1 */
		sio_index = SIO_ID_UART1;
		if (!serialio_uart_is_debug(dev))
			serialio_d21_ltr(bar0);
		serialio_d21_mode(sio_index, SIO_PIN_INTD,
				  config->sio_acpi_mode);
		break;
	case PCH_DEVFN_SDIO: /* SDIO */
		sio_index = SIO_ID_SDIO;
		serialio_d23_ltr(bar0);
		serialio_d23_mode(config->sio_acpi_mode);
		break;
	default:
		return;
	}

	/* Save BAR0 and BAR1 to ACPI NVS */
	if (config->sio_acpi_mode)
		update_bars(sio_index, (u32)bar0->base, (u32)bar1->base);
}

static void serialio_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* Set the configured UART base address for the debug port */
	if (CONFIG(SERIALIO_UART_CONSOLE) && serialio_uart_is_debug(dev)) {
		struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
		res->base = CONFIG_CONSOLE_UART_BASE_ADDRESS;
		res->size = 0x1000;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}
}

static struct device_operations device_ops = {
	.read_resources		= serialio_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= serialio_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_LPT_LP_SDMA,
	PCI_DID_INTEL_LPT_LP_I2C0,
	PCI_DID_INTEL_LPT_LP_I2C1,
	PCI_DID_INTEL_LPT_LP_GSPI0,
	PCI_DID_INTEL_LPT_LP_GSPI1,
	PCI_DID_INTEL_LPT_LP_UART0,
	PCI_DID_INTEL_LPT_LP_UART1,
	PCI_DID_INTEL_LPT_LP_SD,
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
