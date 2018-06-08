/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include "i82801gx.h"
#include <cpu/x86/smm.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <string.h>
#include <drivers/intel/gma/i915.h>
#include "nvs.h"

#define NMI_OFF	0

#define ENABLE_ACPI_MODE_IN_COREBOOT	0
#define TEST_SMM_FLASH_LOCKDOWN		0

typedef struct southbridge_intel_i82801gx_config config_t;

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void i82801gx_enable_ioapic(struct device *dev)
{
	/* Enable ACPI I/O range decode */
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);

	set_ioapic_id(VIO_APIC_VADDR, 0x02);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(VIO_APIC_VADDR, 0x03, 0x01);
}

static void i82801gx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
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

static void i82801gx_pirq_init(struct device *dev)
{
	struct device *irq_dev;
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

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1:
			/* INTA# */ int_line = config->pirqa_routing; break;
		case 2:
			/* INTB# */ int_line = config->pirqb_routing; break;
		case 3:
			/* INTC# */ int_line = config->pirqc_routing; break;
		case 4:
			/* INTD# */ int_line = config->pirqd_routing; break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void i82801gx_gpi_routing(struct device *dev)
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

	pci_write_config32(dev, GPIO_ROUT, reg32);
}

static void i82801gx_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	int pwr_on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use MAINBOARD_POWER_ON.
	 */
	pwr_on = MAINBOARD_POWER_ON;
	get_option(&pwr_on, "power_on_after_fail");

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

	reg8 |= (3 << 4);	/* avoid #S4 assertions */
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
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
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
	/* Set DEBUG_PERIODIC_SMIS in i82801gx.h to debug using
	 * periodic SMIs.
	 */
	reg16 |= (3 << 0); // Periodic SMI every 8s
#endif
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	// Set the board's GPI routing.
	i82801gx_gpi_routing(dev);

	pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	outl(config->gpe0_en, pmbase + GPE0_EN);
	outw(config->alt_gp_smi_en, pmbase + ALT_GP_SMI_EN);

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT

	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 1);	// enable C3->C0 transition on bus master
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);
}

static void i82801gx_configure_cstates(struct device *dev)
{
	u8 reg8;

	reg8 = pci_read_config8(dev, 0xa9); // Cx state configuration
	reg8 |= (1 << 4) | (1 << 3) | (1 << 2);	// Enable Popup & Popdown
	pci_write_config8(dev, 0xa9, reg8);

	// Set Deeper Sleep configuration to recommended values
	reg8 = pci_read_config8(dev, 0xaa);
	reg8 &= 0xf0;
	reg8 |= (2 << 2);	// Deeper Sleep to Stop CPU: 34-40us
	reg8 |= (2 << 0);	// Deeper Sleep to Sleep: 15us
	pci_write_config8(dev, 0xaa, reg8);
}

static void i82801gx_rtc_init(struct device *dev)
{
	u8 reg8;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);

	cmos_init(rtc_failed);
}

static void enable_hpet(void)
{
	u32 reg32;

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(HPTC);
	reg32 |= (1 << 7); // HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(HPTC) = reg32;
}

static void enable_clock_gating(void)
{
	u32 reg32;

	/* Enable Clock Gating for most devices */
	reg32 = RCBA32(CG);
	reg32 |= (1 << 31);	// LPC clock gating
	reg32 |= (1 << 30);	// PATA clock gating
	// SATA clock gating
	reg32 |= (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24);
	reg32 |= (1 << 23);	// AC97 clock gating
	reg32 |= (1 << 19);	// USB EHCI clock gating
	reg32 |= (1 << 3) | (1 << 1);	// DMI clock gating
	reg32 |= (1 << 2);	// PCIe clock gating;
	reg32 &= ~(1 << 20); // No static clock gating for USB
	reg32 &= ~((1 << 29) | (1 << 28)); // Disable UHCI clock gating
	RCBA32(CG) = reg32;
}

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
static void i82801gx_lock_smm(struct device *dev)
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

#define SPIBASE 0x3020
static void i82801gx_spi_init(void)
{
	u16 spicontrol;

	spicontrol = RCBA16(SPIBASE + 2);
	spicontrol &= ~(1 << 0); // SPI Access Request
	RCBA16(SPIBASE + 2) = spicontrol;
}

static void i82801gx_fixups(struct device *dev)
{
	/* This needs to happen after PCI enumeration */
	RCBA32(0x1d40) |= 1;

	/* USB Transient Disconnect Detect:
	 * Prevent a SE0 condition on the USB ports from being
	 * interpreted by the UHCI controller as a disconnect
	 */
	pci_write_config8(dev, 0xad, 0x3);
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "i82801gx: lpc_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	i82801gx_enable_ioapic(dev);

	i82801gx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801gx_pirq_init(dev);

	/* Setup power options. */
	i82801gx_power_options(dev);

	/* Configure Cx state registers */
	i82801gx_configure_cstates(dev);

	/* Set the state of the GPIO lines. */
	//gpio_init(dev);

	/* Initialize the real time clock. */
	i82801gx_rtc_init(dev);

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
	i82801gx_lock_smm(dev);
#endif

	i82801gx_spi_init();

	i82801gx_fixups(dev);
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	/* LAPIC_NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 0,
				MP_IRQ_POLARITY_HIGH |
				MP_IRQ_TRIGGER_EDGE, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 1, MP_IRQ_POLARITY_HIGH |
				MP_IRQ_TRIGGER_EDGE, 0x01);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 0, 2, MP_IRQ_POLARITY_HIGH | MP_IRQ_TRIGGER_EDGE);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 9, 9, MP_IRQ_POLARITY_HIGH | MP_IRQ_TRIGGER_LEVEL);


	return current;
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	config_t *chip = dev->chip_info;
	u16 pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = pmbase + 0x20;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x28;
	fadt->gpe1_blk = 0;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0;

	fadt->reset_value = 6;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = pmbase;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 0;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 0;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = pmbase + 0x20;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 64;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = pmbase + 0x28;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 0;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x32;

	fadt->model = 1;
	fadt->sci_int = 0x9;
	fadt->smi_cmd = APM_CNT;
	fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
	fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = APM_CNT_PST_CONTROL;

	fadt->cst_cnt = APM_CNT_CST_CONTROL;
	fadt->p_lvl2_lat = 1;
	fadt->p_lvl3_lat = chip->c3_latency;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 1;
	if (chip->p_cnt_throttling_supported)
		fadt->duty_width = 3;
	else
		fadt->duty_width = 0;
	fadt->iapc_boot_arch = 0x03;
	fadt->flags = (ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED
		       | ACPI_FADT_SLEEP_BUTTON | ACPI_FADT_S4_RTC_WAKE
		       | ACPI_FADT_PLATFORM_CLOCK | ACPI_FADT_RESET_REGISTER
		       | ACPI_FADT_C2_MP_SUPPORTED);
	if (chip->docking_supported)
		fadt->flags |= ACPI_FADT_DOCKING_SUPPORTED;
}

static void i82801gx_lpc_read_resources(struct device *dev)
{
	struct resource *res;
	u8 io_index = 0;
	int i;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Set IO decode ranges if required.*/
	for (i = 0; i < 4; i++) {
		u32 gen_dec;
		gen_dec = pci_read_config32(dev, 0x84 + 4 * i);

		if ((gen_dec & 0xFFFC) > 0x1000) {
			res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
			res->base = gen_dec & 0xFFFC;
			res->size = (gen_dec >> 16) & 0xFC;
			res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
		}
	}
}

#define SPIBAR16(x) RCBA16(0x3020 + x)
#define SPIBAR32(x) RCBA32(0x3020 + x)

static void lpc_final(struct device *dev)
{
	u16 tco1_cnt;

	if (!IS_ENABLED(CONFIG_INTEL_CHIPSET_LOCKDOWN))
		return;

	SPIBAR16(PREOP) = SPI_OPPREFIX;
	/* Set SPI opcode menu */
	SPIBAR16(OPTYPE) = SPI_OPTYPE;
	SPIBAR32(OPMENU) = SPI_OPMENU_LOWER;
	SPIBAR32(OPMENU + 4) = SPI_OPMENU_UPPER;

	/* Lock SPIBAR */
	SPIBAR16(0) = SPIBAR16(0) | (1 << 15);

	/* BIOS Interface Lockdown */
	RCBA32(0x3410) |= 1 << 0;

	/* Global SMI Lock */
	pci_or_config16(dev, GEN_PMCON_1, 1 << 4);

	/* TCO_Lock */
	tco1_cnt = inw(DEFAULT_PMBASE + 0x60 + TCO1_CNT);
	tco1_cnt |= (1 << 12); /* TCO lock */
	outw(tco1_cnt, DEFAULT_PMBASE + 0x60 + TCO1_CNT);

	/* Indicate finalize step with post code */
	outb(POST_OS_BOOT, 0x80);
}

static void set_subsystem(struct device *dev, unsigned int vendor,
			  unsigned int device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static void southbridge_inject_dsdt(struct device *dev)
{
	global_nvs_t *gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));

	if (gnvs) {
		const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();

		memset(gnvs, 0, sizeof(*gnvs));

		gnvs->apic = 1;
		gnvs->mpen = 1; /* Enable Multi Processing */

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

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= i82801gx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.write_acpi_tables      = acpi_write_hpet,
	.init			= lpc_init,
	.scan_bus		= scan_lpc_bus,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_ops,
	.final			= lpc_final,
};

/* 27b0: 82801GH (ICH7 DH) */
/* 27b8: 82801GB/GR (ICH7/ICH7R) */
/* 27b9: 82801GBM/GU (ICH7-M/ICH7-U) */
/* 27bc: 82NM10 (NM10) */
/* 27bd: 82801GHM (ICH7-M DH) */

static const unsigned short pci_device_ids[] = {
	0x27b0, 0x27b8, 0x27b9, 0x27bc, 0x27bd, 0
};

static const struct pci_driver ich7_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
