/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <arch/acpigen.h>
#include <cbmem.h>
#include <string.h>
#include "i82801ix.h"
#include "nvs.h"
#include <southbridge/intel/common/pciehp.h>
#include <drivers/intel/gma/i915.h>

#define NMI_OFF	0

#define ENABLE_ACPI_MODE_IN_COREBOOT	0
#define TEST_SMM_FLASH_LOCKDOWN		0

typedef struct southbridge_intel_i82801ix_config config_t;

static void i82801ix_enable_apic(struct device *dev)
{
	u32 reg32;
	volatile u32 *ioapic_index = (volatile u32 *)(IO_APIC_ADDR);
	volatile u32 *ioapic_data = (volatile u32 *)(IO_APIC_ADDR + 0x10);

	/* Enable IOAPIC. Keep APIC Range Select at zero. */
	RCBA8(0x31ff) = 0x03;
	/* We have to read 0x31ff back if bit0 changed. */
	RCBA8(0x31ff);

	/* Lock maximum redirection entries (MRE), R/WO register. */
	*ioapic_index	= 0x01;
	reg32		= *ioapic_data;
	*ioapic_index	= 0x01;
	*ioapic_data	= reg32;

	setup_ioapic(VIO_APIC_VADDR, 2); /* ICH7 code uses id 2. */
}

static void i82801ix_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, D31F0_SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
}

/* PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

static void i82801ix_pirq_init(device_t dev)
{
	device_t irq_dev;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	pci_write_config8(dev, D31F0_PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, D31F0_PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, D31F0_PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, D31F0_PIRQD_ROUT, config->pirqd_routing);

	pci_write_config8(dev, D31F0_PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, D31F0_PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, D31F0_PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, D31F0_PIRQH_ROUT, config->pirqh_routing);

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin=0, int_line=0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */ int_line = config->pirqa_routing; break;
		case 2: /* INTB# */ int_line = config->pirqb_routing; break;
		case 3: /* INTC# */ int_line = config->pirqc_routing; break;
		case 4: /* INTD# */ int_line = config->pirqd_routing; break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void i82801ix_gpi_routing(device_t dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	u32 reg32 = 0;

	/* An array would be much nicer here, or some
	 * other method of doing this.
	 */
	reg32 |= (config->gpi0_routing & 0x03) << 0;
	reg32 |= (config->gpi1_routing & 0x03) << 2;
	reg32 |= (config->gpi2_routing & 0x03) << 4;
	reg32 |= (config->gpi3_routing & 0x03) << 6;
	reg32 |= (config->gpi4_routing & 0x03) << 8;
	reg32 |= (config->gpi5_routing & 0x03) << 10;
	reg32 |= (config->gpi6_routing & 0x03) << 12;
	reg32 |= (config->gpi7_routing & 0x03) << 14;
	reg32 |= (config->gpi8_routing & 0x03) << 16;
	reg32 |= (config->gpi9_routing & 0x03) << 18;
	reg32 |= (config->gpi10_routing & 0x03) << 20;
	reg32 |= (config->gpi11_routing & 0x03) << 22;
	reg32 |= (config->gpi12_routing & 0x03) << 24;
	reg32 |= (config->gpi13_routing & 0x03) << 26;
	reg32 |= (config->gpi14_routing & 0x03) << 28;
	reg32 |= (config->gpi15_routing & 0x03) << 30;

	pci_write_config32(dev, D31F0_GPIO_ROUT, reg32);
}

static void i82801ix_power_options(device_t dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	int pwr_on=CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* BIOS must program... */
	reg32 = pci_read_config32(dev, 0xac);
	pci_write_config32(dev, 0xac, reg32 | (1 << 30) | (3 << 8));

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use MAINBOARD_POWER_ON.
	 */
	pwr_on = MAINBOARD_POWER_ON;
	get_option(&pwr_on, "power_on_after_fail");

	reg8 = pci_read_config8(dev, D31F0_GEN_PMCON_3);
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

	reg8 |= (3 << 4);	/* avoid #S4 assertions */
	reg8 &= ~(1 << 3);	/* minimum asssertion is 1 to 2 RTCCLK */

	pci_write_config8(dev, D31F0_GEN_PMCON_3, reg8);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* Set up NMI on errors. */
	reg8 = inb(0x61);
	reg8 &= 0x0f;		/* Higher Nibble must be 0 */
	reg8 &= ~(1 << 3);	/* IOCHK# NMI Enable */
	// reg8 &= ~(1 << 2);	/* PCI SERR# Enable */
	reg8 |= (1 << 2); /* PCI SERR# Disable for now */
	outb(reg8, 0x61);

	reg8 = inb(0x74); /* Read from 0x74 as 0x70 is write only. */
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		printk(BIOS_INFO, "NMI sources enabled.\n");
		reg8 &= ~(1 << 7);	/* Set NMI. */
	} else {
		printk(BIOS_INFO, "NMI sources disabled.\n");
		reg8 |= (1 << 7);	/* Can't mask NMI from PCI-E and NMI_NOW */
	}
	outb(reg8, 0x70);

	/* Enable CPU_SLP# and Intel Speedstep, set SMI# rate down */
	reg16 = pci_read_config16(dev, D31F0_GEN_PMCON_1);
	reg16 &= ~(3 << 0);	// SMI# rate 1 minute
	reg16 |= (1 << 2);	// CLKRUN_EN - Mobile/Ultra only
	reg16 |= (1 << 3);	// Speedstep Enable - Mobile/Ultra only
	reg16 |= (1 << 5);	// CPUSLP_EN Desktop only

	if (config->c4onc3_enable)
		reg16 |= (1 << 7);

	// another laptop wants this?
	// reg16 &= ~(1 << 10);	// BIOS_PCI_EXP_EN - Desktop/Mobile only
	reg16 |= (1 << 10);	// BIOS_PCI_EXP_EN - Desktop/Mobile only
#if DEBUG_PERIODIC_SMIS
	/* Set DEBUG_PERIODIC_SMIS in i82801ix.h to debug using
	 * periodic SMIs.
	 */
	reg16 |= (3 << 0); // Periodic SMI every 8s
#endif
	if (config->c5_enable)
		reg16 |= (1 << 11); /* Enable C5, C6 and PMSYNC# */
	pci_write_config16(dev, D31F0_GEN_PMCON_1, reg16);

	/* Set exit timings for C5/C6. */
	if (config->c5_enable) {
		reg8 = pci_read_config8(dev, D31F0_C5_EXIT_TIMING);
		reg8 &= ~((7 << 3) | (7 << 0));
		if (config->c6_enable)
			reg8 |= (5 << 3) | (3 << 0); /* 38-44us PMSYNC# to STPCLK#,
							95-102us DPRSTP# to STP_CPU# */
		else
			reg8 |= (0 << 3) | (1 << 0); /* 16-17us PMSYNC# to STPCLK#,
							34-40us DPRSTP# to STP_CPU# */
		pci_write_config8(dev, D31F0_C5_EXIT_TIMING, reg8);
	}

	// Set the board's GPI routing.
	i82801ix_gpi_routing(dev);

	pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	outl(config->gpe0_en, pmbase + 0x28);
	outw(config->alt_gp_smi_en, pmbase + 0x38);

	/* Set up power management block and determine sleep mode */
	reg16 = inw(pmbase + 0x00); /* PM1_STS */
	outw(reg16, pmbase + 0x00); /* Clear status bits. At least bit11 (power
				       button override) must be cleared or SCI
				       will be constantly fired and OSPM must
				       not know about it (ACPI spec says to
				       ignore the bit). */
	reg32 = inl(pmbase + 0x04); // PM1_CNT
	reg32 &= ~(7 << 10);	// SLP_TYP
	outl(reg32, pmbase + 0x04);

	/* Set duty cycle for hardware throttling (defaults to 0x0: 50%). */
	reg32 = inl(pmbase + 0x10);
	reg32 &= ~(7 << 5);
	reg32 |= (config->throttle_duty & 7) << 5;
	outl(reg32, pmbase + 0x10);
}

static void i82801ix_configure_cstates(device_t dev)
{
	u8 reg8;

	reg8 = pci_read_config8(dev, D31F0_CxSTATE_CNF);
	reg8 |= (1 << 4) | (1 << 3) | (1 << 2);	// Enable Popup & Popdown
	pci_write_config8(dev, D31F0_CxSTATE_CNF, reg8);

	// Set Deeper Sleep configuration to recommended values
	reg8 = pci_read_config8(dev, D31F0_C4TIMING_CNT);
	reg8 &= 0xf0;
	reg8 |= (2 << 2);	// Deeper Sleep to Stop CPU: 34-40us
	reg8 |= (2 << 0);	// Deeper Sleep to Sleep: 15us
	pci_write_config8(dev, D31F0_C4TIMING_CNT, reg8);

	/* We could enable slow-C4 exit here, if someone needs it? */
}

static void i82801ix_rtc_init(struct device *dev)
{
	u8 reg8;
	int rtc_failed;

	reg8 = pci_read_config8(dev, D31F0_GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(dev, D31F0_GEN_PMCON_3, reg8);
	}
	printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);

	cmos_init(rtc_failed);
}

static void enable_hpet(void)
{
	u32 reg32;

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(RCBA_HPTC);
	reg32 |= (1 << 7); // HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(RCBA_HPTC) = reg32;
}

static void enable_clock_gating(void)
{
	u32 reg32;

	/* Enable DMI dynamic clock gating. */
	RCBA32(RCBA_DMIC) |= 3;

	/* Enable Clock Gating for most devices. */
	reg32 = RCBA32(RCBA_CG);
	reg32 |= (1 << 31);	/* LPC dynamic clock gating */
	/* USB UHCI dynamic clock gating: */
	reg32 |= (1 << 29) | (1 << 28);
	/* SATA dynamic clock gating [0-3]: */
	reg32 |= (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24);
	reg32 |= (1 << 23);	/* LAN static clock gating (if LAN disabled) */
	reg32 |= (1 << 22);	/* HD audio dynamic clock gating */
	reg32 &= ~(1 << 21);	/* No HD audio static clock gating */
	reg32 &= ~(1 << 20);	/* No USB EHCI static clock gating */
	reg32 |= (1 << 19);	/* USB EHCI dynamic clock gating */
	/* More SATA dynamic clock gating [4-5]: */
	reg32 |= (1 << 18) | (1 << 17);
	reg32 |= (1 << 16);	/* PCI dynamic clock gating */
	/* PCIe, DMI dynamic clock gating: */
	reg32 |= (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1);
	reg32 |= (1 << 0);	/* PCIe root port static clock gating */
	RCBA32(RCBA_CG) = reg32;

	/* Enable SPI dynamic clock gating. */
	RCBA32(0x38c0) |= 7;
}

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
static void i82801ix_lock_smm(struct device *dev)
{
#if TEST_SMM_FLASH_LOCKDOWN
	u8 reg8;
#endif

	if (!acpi_is_wakeup_s3()) {
#if ENABLE_ACPI_MODE_IN_COREBOOT
		printk(BIOS_DEBUG, "Enabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_ENABLE, APM_CNT); // Enable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#else
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT); // Disable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#endif
	} else {
		printk(BIOS_DEBUG, "S3 wakeup, enabling ACPI via APMC\n");
		outb(APM_CNT_ACPI_ENABLE, APM_CNT);
	}
	/* Don't allow evil boot loaders, kernels, or
	 * userspace applications to deceive us:
	 */
	smm_lock();

#if TEST_SMM_FLASH_LOCKDOWN
	/* Now try this: */
	printk(BIOS_DEBUG, "Locking BIOS to RO... ");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk(BIOS_DEBUG, " BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");
	reg8 &= ~(1 << 0);			/* clear BIOSWE */
	pci_write_config8(dev, 0xdc, reg8);
	reg8 |= (1 << 1);			/* set BLE */
	pci_write_config8(dev, 0xdc, reg8);
	printk(BIOS_DEBUG, "ok.\n");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk(BIOS_DEBUG, " BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");

	printk(BIOS_DEBUG, "Writing:\n");
	*(volatile u8 *)0xfff00000 = 0x00;
	printk(BIOS_DEBUG, "Testing:\n");
	reg8 |= (1 << 0);			/* set BIOSWE */
	pci_write_config8(dev, 0xdc, reg8);

	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk(BIOS_DEBUG, " BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");
	printk(BIOS_DEBUG, "Done.\n");
#endif
}
#endif

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "i82801ix: lpc_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	i82801ix_enable_apic(dev);

	i82801ix_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801ix_pirq_init(dev);

	/* Setup power options. */
	i82801ix_power_options(dev);

	/* Configure Cx state registers */
	if (LPC_IS_MOBILE(dev))
		i82801ix_configure_cstates(dev);

	/* Initialize the real time clock. */
	i82801ix_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet();

	/* Initialize Clock Gating */
	enable_clock_gating();

	setup_i8259();

	/* The OS should do this? */
	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	i82801ix_lock_smm(dev);
#endif
}

static void i82801ix_lpc_read_resources(device_t dev)
{
	/*
	 *             I/O Resources
	 *
	 * 0x0000 - 0x000f....ISA DMA
	 * 0x0010 - 0x001f....ISA DMA aliases
	 * 0x0020 ~ 0x003d....PIC
	 * 0x002e - 0x002f....Maybe Super I/O
	 * 0x0040 - 0x0043....Timer
	 * 0x004e - 0x004f....Maybe Super I/O
	 * 0x0050 - 0x0053....Timer aliases
	 * 0x0061.............NMI_SC
	 * 0x0070.............NMI_EN (readable in alternative access mode)
	 * 0x0070 - 0x0077....RTC
	 * 0x0080 - 0x008f....ISA DMA
	 * 0x0090 ~ 0x009f....ISA DMA aliases
	 * 0x0092.............Fast A20 and Init
	 * 0x00a0 ~ 0x00bd....PIC
	 * 0x00b2 - 0x00b3....APM
	 * 0x00c0 ~ 0x00de....ISA DMA
	 * 0x00c1 ~ 0x00df....ISA DMA aliases
	 * 0x00f0.............Coprocessor Error
	 * (0x0400-0x041f)....SMBus (SMBUS_IO_BASE, during raminit)
	 * 0x04d0 - 0x04d1....PIC
	 * 0x0500 - 0x057f....PM (DEFAULT_PMBASE)
	 * 0x0580 - 0x05bf....SB GPIO (DEFAULT_GPIOBASE)
	 * 0x05c0 - 0x05ff....SB GPIO cont. (mobile only)
	 * 0x0cf8 - 0x0cff....PCI
	 * 0x0cf9.............Reset Control
	 */

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

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static void southbridge_inject_dsdt(device_t dev)
{
	global_nvs_t *gnvs = cbmem_add (CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));

	if (gnvs) {
		const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
		memset(gnvs, 0, sizeof(*gnvs));
		acpi_create_gnvs(gnvs);

		gnvs->ndid = gfx->ndid;
		memcpy(gnvs->did, gfx->did, sizeof(gnvs->did));

		/* And tell SMI about it */
		smm_setup_structures(gnvs, NULL, NULL);

		/* Add it to SSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (u32) gnvs);
		acpigen_pop_len();
	}
}

static void southbridge_fill_ssdt(device_t device)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	config_t *chip = dev->chip_info;

	intel_acpi_pcie_hotplug_generator(chip->pcie_hotplug_map, 8);
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= i82801ix_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt_generator = southbridge_fill_ssdt,
	.init			= lpc_init,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x2912, /* ICH9DH  */
	0x2914, /* ICH9DO  */
	0x2916, /* ICH9R   */
	0x2918, /* ICH9    */
	0x2917, /* ICH9M-E */
	0x2919, /* ICH9M   */
	0
};

static const struct pci_driver ich9_lpc __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
