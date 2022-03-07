/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <option.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include "chip.h"
#include "i82801dx.h"

#define NMI_OFF 0

typedef struct southbridge_intel_i82801dx_config config_t;

/**
 * Enable ACPI I/O range.
 *
 * @param dev PCI device with ACPI and PM BAR's
 */
static void i82801dx_enable_acpi(struct device *dev)
{
	/* Set ACPI base address (I/O space). */
	pci_write_config32(dev, PMBASE, (PMBASE_ADDR | 1));

	/* Enable ACPI I/O range decode and ACPI power management. */
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);
}

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void i82801dx_enable_ioapic(struct device *dev)
{
	u32 reg32;

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (1 << 13);	/* Coprocessor error enable (COPR_ERR_EN) */
	reg32 |= (3 << 7);	/* IOAPIC enable (APIC_EN) */
	reg32 |= (1 << 2);	/* DMA collection buffer enable (DCB_EN) */
	reg32 |= (1 << 1);	/* Delayed transaction enable (DTE) */
	pci_write_config32(dev, GEN_CNTL, reg32);
	printk(BIOS_DEBUG, "IOAPIC Southbridge enabled %x\n", reg32);

	setup_ioapic(VIO_APIC_VADDR, 0x02);

	ioapic_set_boot_config(VIO_APIC_VADDR, true);
}

static void i82801dx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
}

static void i82801dx_pirq_init(struct device *dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	pci_write_config8(dev, PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, PIRQD_ROUT, config->pirqd_routing);
	pci_write_config8(dev, PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, PIRQH_ROUT, config->pirqh_routing);
}

static void i82801dx_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use MAINBOARD_POWER_ON.
	 */
	const unsigned int pwr_on = get_uint_option("power_on_after_fail", MAINBOARD_POWER_ON);

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	reg8 &= 0xfe;
	switch (pwr_on) {
		case MAINBOARD_POWER_OFF:
			reg8 |= 1;
			state = "off";
			break;
		case MAINBOARD_POWER_ON:
			reg8 &= ~1;
			state = "on";
			break;
		case MAINBOARD_POWER_KEEP:
			reg8 &= ~1;
			state = "state keep";
			break;
		default:
			state = "undefined";
	}

	reg8 &= ~(1 << 3);	/* minimum assertion is 1 to 2 RTCCLK */

	pci_write_config8(dev, GEN_PMCON_3, reg8);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* Set up NMI on errors. */
	reg8 = inb(0x61);
	reg8 &= 0x0f;		/* Higher Nibble must be 0 */
	reg8 &= ~(1 << 3);	/* IOCHK# NMI Enable */
	// reg8 &= ~(1 << 2);	/* PCI SERR# Enable */
	reg8 |= (1 << 2); /* PCI SERR# Disable for now */
	outb(reg8, 0x61);

	reg8 = inb(0x70);
	const unsigned int nmi_option = get_uint_option("nmi", NMI_OFF);
	if (nmi_option) {
		printk(BIOS_INFO, "NMI sources enabled.\n");
		reg8 &= ~(1 << 7);	/* Set NMI. */
	} else {
		printk(BIOS_INFO, "NMI sources disabled.\n");
		reg8 |= (1 << 7);	/* Disable NMI. */
	}
	outb(reg8, 0x70);

	/* Set SMI# rate down and enable CPU_SLP# */
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~(3 << 0);	// SMI# rate 1 minute
	reg16 |= (1 << 5);	// CPUSLP_EN Desktop only
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT

	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);
}

static void gpio_init(struct device *dev)
{
	/* This should be done in romstage.c already */
	pci_write_config32(dev, GPIO_BASE, (GPIOBASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);
}

static void i82801dx_rtc_init(struct device *dev)
{
	u8 reg8;
	u32 reg32;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~(1 << 1);	/* Preserve the power fail state. */
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	reg32 = pci_read_config32(dev, GEN_STS);
	rtc_failed |= reg32 & (1 << 2);
	cmos_init(rtc_failed);

	/* Enable access to the upper 128 byte bank of CMOS RAM. */
	pci_write_config8(dev, RTC_CONF, 0x04);
}

static void i82801dx_lpc_route_dma(struct device *dev, u8 mask)
{
	u16 reg16;
	int i;

	reg16 = pci_read_config16(dev, PCI_DMA_CFG);
	reg16 &= 0x300;
	for (i = 0; i < 8; i++) {
		if (i == 4)
			continue;
		reg16 |= ((mask & (1 << i)) ? 3 : 1) << (i * 2);
	}
	pci_write_config16(dev, PCI_DMA_CFG, reg16);
}

static void i82801dx_lpc_decode_en(struct device *dev)
{
	/* Decode 0x3F8-0x3FF (COM1) for COMA port, 0x2F8-0x2FF (COM2) for COMB.
	 * LPT decode defaults to 0x378-0x37F and 0x778-0x77F.
	 * Floppy decode defaults to 0x3F0-0x3F5, 0x3F7.
	 * We also need to set the value for LPC I/F Enables Register.
	 */
	pci_write_config8(dev, COM_DEC, 0x10);
	pci_write_config16(dev, LPC_EN, 0x300F);
}

/* ICH4 does not mention HPET in the docs, but
 * all ICH3 and ICH4 do have HPETs built in.
 */
static void enable_hpet(struct device *dev)
{
	u32 reg32, hpet, val;

	/* Set HPET base address and enable it */
	printk(BIOS_DEBUG, "Enabling HPET at 0x%x\n", HPET_BASE_ADDRESS);
	reg32 = pci_read_config32(dev, GEN_CNTL);
	/*
	 * Bit 17 is HPET enable bit.
	 * Bit 16:15 control the HPET base address.
	 */
	reg32 &= ~(3 << 15);	/* Clear it */

	hpet = HPET_BASE_ADDRESS >> 12;
	hpet &= 0x3;

	reg32 |= (hpet << 15);
	reg32 |= (1 << 17);	/* Enable HPET. */
	pci_write_config32(dev, GEN_CNTL, reg32);

	/* Check to see whether it took */
	reg32 = pci_read_config32(dev, GEN_CNTL);
	val = reg32 >> 15;
	val &= 0x7;

	if ((val & 0x4) && (hpet == (val & 0x3))) {
		printk(BIOS_INFO, "HPET enabled at 0x%x\n", HPET_BASE_ADDRESS);
	} else {
		printk(BIOS_WARNING, "HPET was not enabled correctly\n");
		reg32 &= ~(1 << 17);	/* Clear Enable */
		pci_write_config32(dev, GEN_CNTL, reg32);
	}
}

static void lpc_init(struct device *dev)
{
	i82801dx_enable_acpi(dev);
	/* IO APIC initialization. */
	i82801dx_enable_ioapic(dev);

	i82801dx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801dx_pirq_init(dev);

	/* Setup power options. */
	i82801dx_power_options(dev);

	/* Set the state of the GPIO lines. */
	gpio_init(dev);

	/* Initialize the real time clock. */
	i82801dx_rtc_init(dev);

	/* Route DMA. */
	i82801dx_lpc_route_dma(dev, 0xff);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Setup decode ports and LPC I/F enables. */
	i82801dx_lpc_decode_en(dev);

	/* Initialize the High Precision Event Timers */
	enable_hpet(dev);

	setup_i8259();

	/* Don't allow evil boot loaders, kernels, or
	 * userspace applications to deceive us:
	 */
	if (CONFIG(SMM_LEGACY_ASEG))
		aseg_smm_lock();
}

static void i82801dx_lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

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

static struct device_operations lpc_ops = {
	.read_resources		= i82801dx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.enable			= i82801dx_enable,
};

/* 82801DB/DBL */
static const struct pci_driver lpc_driver_db __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_LPC,
};

/* 82801DBM */
static const struct pci_driver lpc_driver_dbm __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DBM_LPC,
};
