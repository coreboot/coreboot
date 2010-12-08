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
#include <arch/ioapic.h>
#include "esb6300.h"

#define ACPI_BAR 0x40
#define GPIO_BAR 0x58

#define NMI_OFF 0
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define SERIRQ_CNTL 0x64
static void esb6300_enable_serial_irqs(device_t dev)
{
	/* set packet length and toggle silent mode bit */
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(0 << 6)|((21 - 17) << 2)|(0 << 0));
}

#define PCI_DMA_CFG 0x90
static void esb6300_pci_dma_cfg(device_t dev)
{
	/* Set PCI DMA CFG to lpc I/F DMA */
	pci_write_config16(dev, PCI_DMA_CFG, 0xfcff);
}

#define LPC_EN 0xe6
static void esb6300_enable_lpc(device_t dev)
{
        /* lpc i/f enable */
        pci_write_config8(dev, LPC_EN, 0x0d);
}

typedef struct southbridge_intel_esb6300_config config_t;

static void set_esb6300_gpio_use_sel(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_use_sel, gpio_use_sel2;

//	gpio_use_sel  = 0x1B003100;
//	gpio_use_sel2 = 0x03000000;
	gpio_use_sel  = 0x1BBC31C0;
	gpio_use_sel2 = 0x03000FE1;
#if 0
	int i;
	for(i = 0; i < 64; i++) {
		int val;
		switch(config->gpio[i] & ESB6300_GPIO_USE_MASK) {
		case ESB6300_GPIO_USE_AS_NATIVE: val = 0; break;
		case ESB6300_GPIO_USE_AS_GPIO:   val = 1; break;
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
#endif
	outl(gpio_use_sel,  res->base + 0x00);
	outl(gpio_use_sel2, res->base + 0x30);
}

static void set_esb6300_gpio_direction(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_io_sel, gpio_io_sel2;

//	gpio_io_sel  = 0x0000ffff;
//	gpio_io_sel2 = 0x00000000;
	gpio_io_sel  = 0x1900ffff;
	gpio_io_sel2 = 0x00000fe1;
#if 0
	int i;
	for(i = 0; i < 64; i++) {
		int val;
		switch(config->gpio[i] & ESB6300_GPIO_SEL_MASK) {
		case ESB6300_GPIO_SEL_OUTPUT: val = 0; break;
		case ESB6300_GPIO_SEL_INPUT:  val = 1; break;
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
#endif
	outl(gpio_io_sel,  res->base + 0x04);
	outl(gpio_io_sel2, res->base + 0x34);
}

static void set_esb6300_gpio_level(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_lvl, gpio_lvl2;
	uint32_t gpio_blink;

//	gpio_lvl   = 0x1b3f0000;
//	gpio_blink = 0x00040000;
//	gpio_lvl2  = 0x00000fff;
	gpio_lvl   = 0x19370000;
	gpio_blink = 0x00000000;
	gpio_lvl2  = 0x00000fff;
#if 0
	int i;
	for(i = 0; i < 64; i++) {
		int val, blink;
		switch(config->gpio[i] & ESB6300_GPIO_LVL_MASK) {
		case ESB6300_GPIO_LVL_LOW:   val = 0; blink = 0; break;
		case ESB6300_GPIO_LVL_HIGH:  val = 1; blink = 0; break;
		case ESB6300_GPIO_LVL_BLINK: val = 1; blink = 1; break;
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
#endif
	outl(gpio_lvl,   res->base + 0x0c);
	outl(gpio_blink, res->base + 0x18);
	outl(gpio_lvl2,  res->base + 0x38);
}

static void set_esb6300_gpio_inv(
	device_t dev, struct resource *res, config_t *config)
{
	uint32_t gpio_inv;

	gpio_inv   = 0x00003100;
#if 0
	int i;
	for(i = 0; i < 32; i++) {
		int val;
		switch(config->gpio[i] & ESB6300_GPIO_INV_MASK) {
		case ESB6300_GPIO_INV_OFF: val = 0; break;
		case ESB6300_GPIO_INV_ON:  val = 1; break;
		default:
			continue;
		}
		gpio_inv &= ~( 1 << i);
		gpio_inv |= (val << i);
	}
#endif
	outl(gpio_inv,   res->base + 0x2c);
}

static void esb6300_pirq_init(device_t dev)
{
	config_t *config;

	/* Get the chip configuration */
	config = dev->chip_info;

	if(config->pirq_a_d) {
		pci_write_config32(dev, 0x60, config->pirq_a_d);
	}
	if(config->pirq_e_h) {
		pci_write_config32(dev, 0x68, config->pirq_e_h);
	}
}


static void esb6300_gpio_init(device_t dev)
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
	set_esb6300_gpio_use_sel(dev, res, config);

	/* Set the IO direction */
	set_esb6300_gpio_direction(dev, res, config);

	/* Setup the input inverters */
	set_esb6300_gpio_inv(dev, res, config);

	/* Set the value on the GPIO output pins */
	set_esb6300_gpio_level(dev, res, config);

}


static void lpc_init(struct device *dev)
{
	uint8_t byte;
	uint32_t value;
	int pwr_on=CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;

	/* sata settings */
	pci_write_config32(dev, 0x58, 0x00001181);

	/* IO APIC initialization */
	value = pci_read_config32(dev, 0xd0);
	value |= (1 << 8)|(1<<7);
	value |= (6 << 0)|(1<<13)|(1<<11);
	pci_write_config32(dev, 0xd0, value);
	setup_ioapic(IO_APIC_ADDR, 0); // don't rename IO APIC ID

	/* disable reset timer */
	pci_write_config8(dev, 0xd4, 0x02);

	/* cmos ram 2nd 128 */
	pci_write_config8(dev, 0xd8, 0x04);

	/* comm 2 */
	pci_write_config8(dev, 0xe0, 0x10);

	/* fwh sellect */
	pci_write_config32(dev, 0xe8, 0x00112233);

	/* fwh decode */
	pci_write_config8(dev, 0xf0, 0x0f);

	/* av disable, sata controller */
	pci_write_config8(dev, 0xf2, 0xc0);

	/* undocumented */
	pci_write_config8(dev, 0xa0, 0x20);
	pci_write_config8(dev, 0xad, 0x03);
	pci_write_config8(dev, 0xbb, 0x09);

	/* apic1 rout */
	pci_write_config8(dev, 0xf4, 0x40);

	/* undocumented */
	pci_write_config8(dev, 0xa0, 0x20);
	pci_write_config8(dev, 0xad, 0x03);
	pci_write_config8(dev, 0xbb, 0x09);

	esb6300_enable_serial_irqs(dev);

	esb6300_pci_dma_cfg(dev);

	esb6300_enable_lpc(dev);

        get_option(&pwr_on, "power_on_after_fail");
	byte = pci_read_config8(dev, 0xa4);
	byte &= 0xfe;
	if (!pwr_on) {
		byte |= 1;
	}
	pci_write_config8(dev, 0xa4, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", pwr_on?"on":"off");

	/* Set up the PIRQ */
	esb6300_pirq_init(dev);

	/* Set the state of the gpio lines */
	esb6300_gpio_init(dev);

	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();
}

static void esb6300_lpc_read_resources(device_t dev)
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
}

static void esb6300_lpc_enable_resources(device_t dev)
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
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations lpc_ops  = {
	.read_resources   = esb6300_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = esb6300_lpc_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_static_bus,
	.enable           = esb6300_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_LPC,
};
