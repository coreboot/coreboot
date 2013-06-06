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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
#include <elog.h>
#include "pch.h"

#define NMI_OFF	0

#define ENABLE_ACPI_MODE_IN_COREBOOT	0
#define TEST_SMM_FLASH_LOCKDOWN		0

typedef struct southbridge_intel_ibexpeak_config config_t;

/**
 * Set miscellanous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	u32 reg32;

	/* Enable ACPI I/O range decode */
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);

	set_ioapic_id(IO_APIC_ADDR, 0x01); // FIXME shouldn't this be 2?

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read(IO_APIC_ADDR, 0x01);
	io_apic_write(IO_APIC_ADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(IO_APIC_ADDR, 0x03, 0x01);
}

static void pch_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
#if !CONFIG_SERIRQ_CONTINUOUS_MODE
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
#endif
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

static void pch_pirq_init(device_t dev)
{
	device_t irq_dev;
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

	for(irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
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

static void pch_gpi_routing(device_t dev)
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

	pci_write_config32(dev, 0xb8, reg32);
}

static void pch_power_options(device_t dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	int pwr_on=CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use Kconfig setting.
	 */
	get_option(&pwr_on, "power_on_after_fail");

	reg16 = pci_read_config16(dev, GEN_PMCON_3);
	reg16 &= 0xfffe;
	switch (pwr_on) {
	case MAINBOARD_POWER_OFF:
		reg16 |= 1;
		state = "off";
		break;
	case MAINBOARD_POWER_ON:
		reg16 &= ~1;
		state = "on";
		break;
	case MAINBOARD_POWER_KEEP:
		reg16 &= ~1;
		state = "state keep";
		break;
	default:
		state = "undefined";
	}

	reg16 &= ~(3 << 4);	/* SLP_S4# Assertion Stretch 4s */
	reg16 |= (1 << 3);	/* SLP_S4# Assertion Stretch Enable */

	reg16 &= ~(1 << 10);
	reg16 |= (1 << 11);	/* SLP_S3# Min Assertion Width 50ms */

	reg16 |= (1 << 12);	/* Disable SLP stretch after SUS well */

	pci_write_config16(dev, GEN_PMCON_3, reg16);
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
		reg8 |= ( 1 << 7);	/* Can't mask NMI from PCI-E and NMI_NOW */
	}
	outb(reg8, 0x70);

	/* Enable CPU_SLP# and Intel Speedstep, set SMI# rate down */
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~(3 << 0);	// SMI# rate 1 minute
	reg16 &= ~(1 << 10);	// Disable BIOS_PCI_EXP_EN for native PME
#if DEBUG_PERIODIC_SMIS
	/* Set DEBUG_PERIODIC_SMIS in pch.h to debug using
	 * periodic SMIs.
	 */
	reg16 |= (3 << 0); // Periodic SMI every 8s
#endif
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	// Set the board's GPI routing.
	pch_gpi_routing(dev);

	pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	outl(config->gpe0_en, pmbase + GPE0_EN);
	outw(config->alt_gp_smi_en, pmbase + ALT_GP_SMI_EN);

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT
	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);

	/* Clear magic status bits to prevent unexpected wake */
	reg32 = RCBA32(0x3310);
	reg32 |= (1 << 4)|(1 << 5)|(1 << 0);
	RCBA32(0x3310) = reg32;

	reg32 = RCBA32(0x3f02);
	reg32 &= ~0xf;
	RCBA32(0x3f02) = reg32;
}

static void pch_rtc_init(struct device *dev)
{
	u8 reg8;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(dev, GEN_PMCON_3, reg8);
#if CONFIG_ELOG
		elog_add_event(ELOG_TYPE_RTC_RESET);
#endif
	}
	printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);

	rtc_init(rtc_failed);
}

/* Ibex Peak PCH Power Management init */
static void mobile5_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Mobile 5 PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);
	RCBA32 (0x1d44) = 0x00000000;
	(void) RCBA32 (0x1d44);
	RCBA32 (0x1d48) = 0x00030000;
	(void) RCBA32 (0x1d48);
	RCBA32 (0x1e80) = 0x000c0801;
	(void) RCBA32 (0x1e80);
	RCBA32 (0x1e84) = 0x000200f0;
	(void) RCBA32 (0x1e84);
	RCBA32 (0x2010) = 0x00188200;
	(void) RCBA32 (0x2010);
	RCBA32 (0x2014) = 0x14000016;
	(void) RCBA32 (0x2014);
	RCBA32 (0x2018) = 0xbc4abcb5;
	(void) RCBA32 (0x2018);
	RCBA32 (0x201c) = 0x00000000;
	(void) RCBA32 (0x201c);
	RCBA32 (0x2020) = 0xf0c9605b;
	(void) RCBA32 (0x2020);
	RCBA32 (0x2024) = 0x13683040;
	(void) RCBA32 (0x2024);
	RCBA32 (0x2028) = 0x04c8f16e;
	(void) RCBA32 (0x2028);
	RCBA32 (0x202c) = 0x09e90170;
	(void) RCBA32 (0x202c);
	RCBA32 (0x2100) = 0x00000000;
	(void) RCBA32 (0x2100);
	RCBA32 (0x2104) = 0x00000757;
	(void) RCBA32 (0x2104);
	RCBA32 (0x2108) = 0x00170001;
	(void) RCBA32 (0x2108);
	RCBA32 (0x211c) = 0x00000000;
	(void) RCBA32 (0x211c);
	RCBA32 (0x2120) = 0x00010000;
	(void) RCBA32 (0x2120);
	RCBA32 (0x21fc) = 0x00000000;
	(void) RCBA32 (0x21fc);
	RCBA32 (0x2200) = 0x20000044;
	(void) RCBA32 (0x2200);
	RCBA32 (0x2204) = 0x00000001;
	(void) RCBA32 (0x2204);
	RCBA32 (0x2208) = 0x00003457;
	(void) RCBA32 (0x2208);
	RCBA32 (0x2210) = 0x00000000;
	(void) RCBA32 (0x2210);
	RCBA32 (0x2214) = 0x00000001;
	(void) RCBA32 (0x2214);
	RCBA32 (0x2218) = 0xa0fff210;
	(void) RCBA32 (0x2218);
	RCBA32 (0x221c) = 0x0000df00;
	(void) RCBA32 (0x221c);
	RCBA32 (0x2220) = 0x00e30880;
	(void) RCBA32 (0x2220);
	RCBA32 (0x2224) = 0x00000070;
	(void) RCBA32 (0x2224);
	RCBA32 (0x2228) = 0x00004000;
	(void) RCBA32 (0x2228);
	RCBA32 (0x222c) = 0x00000000;
	(void) RCBA32 (0x222c);
	RCBA32 (0x2230) = 0x00e30880;
	(void) RCBA32 (0x2230);
	RCBA32 (0x2234) = 0x00000070;
	(void) RCBA32 (0x2234);
	RCBA32 (0x2238) = 0x00004000;
	(void) RCBA32 (0x2238);
	RCBA32 (0x223c) = 0x00000000;
	(void) RCBA32 (0x223c);
	RCBA32 (0x2240) = 0x00002301;
	(void) RCBA32 (0x2240);
	RCBA32 (0x2244) = 0x36000000;
	(void) RCBA32 (0x2244);
	RCBA32 (0x2248) = 0x00010107;
	(void) RCBA32 (0x2248);
	RCBA32 (0x224c) = 0x00160000;
	(void) RCBA32 (0x224c);
	RCBA32 (0x2250) = 0x00001b01;
	(void) RCBA32 (0x2250);
	RCBA32 (0x2254) = 0x36000000;
	(void) RCBA32 (0x2254);
	RCBA32 (0x2258) = 0x00010107;
	(void) RCBA32 (0x2258);
	RCBA32 (0x225c) = 0x00160000;
	(void) RCBA32 (0x225c);
	RCBA32 (0x2260) = 0x00000601;
	(void) RCBA32 (0x2260);
	RCBA32 (0x2264) = 0x16000000;
	(void) RCBA32 (0x2264);
	RCBA32 (0x2268) = 0x00010107;
	(void) RCBA32 (0x2268);
	RCBA32 (0x226c) = 0x00160000;
	(void) RCBA32 (0x226c);
	RCBA32 (0x2270) = 0x00001c01;
	(void) RCBA32 (0x2270);
	RCBA32 (0x2274) = 0x16000000;
	(void) RCBA32 (0x2274);
	RCBA32 (0x2278) = 0x00010107;
	(void) RCBA32 (0x2278);
	RCBA32 (0x227c) = 0x00160000;
	(void) RCBA32 (0x227c);
	RCBA32 (0x2300) = 0x00000000;
	(void) RCBA32 (0x2300);
	RCBA32 (0x2304) = 0x40000000;
	(void) RCBA32 (0x2304);
	RCBA32 (0x2308) = 0x4646827b;
	(void) RCBA32 (0x2308);
	RCBA32 (0x230c) = 0x6e803131;
	(void) RCBA32 (0x230c);
	RCBA32 (0x2310) = 0x32c77887;
	(void) RCBA32 (0x2310);
	RCBA32 (0x2314) = 0x00077733;
	(void) RCBA32 (0x2314);
	RCBA32 (0x2318) = 0x00007447;
	(void) RCBA32 (0x2318);
	RCBA32 (0x231c) = 0x00000040;
	(void) RCBA32 (0x231c);
	RCBA32 (0x2320) = 0xcccc0cfc;
	(void) RCBA32 (0x2320);
	RCBA32 (0x2324) = 0x0fbb0fff;
	(void) RCBA32 (0x2324);
	RCBA32 (0x30fc) = 0x00000000;
	(void) RCBA32 (0x30fc);
	RCBA32 (0x3100) = 0x04341200;
	(void) RCBA32 (0x3100);
	RCBA32 (0x3104) = 0x00000000;
	(void) RCBA32 (0x3104);
	RCBA32 (0x3108) = 0x40043214;
	(void) RCBA32 (0x3108);
	RCBA32 (0x310c) = 0x00014321;
	(void) RCBA32 (0x310c);
	RCBA32 (0x3110) = 0x00000002;
	(void) RCBA32 (0x3110);
	RCBA32 (0x3114) = 0x30003214;
	(void) RCBA32 (0x3114);
	RCBA32 (0x311c) = 0x00000002;
	(void) RCBA32 (0x311c);
	RCBA32 (0x3120) = 0x00000000;
	(void) RCBA32 (0x3120);
	RCBA32 (0x3124) = 0x00002321;
	(void) RCBA32 (0x3124);
	RCBA32 (0x313c) = 0x00000000;
	(void) RCBA32 (0x313c);
	RCBA32 (0x3140) = 0x00003107;
	(void) RCBA32 (0x3140);
	RCBA32 (0x3144) = 0x76543210;
	(void) RCBA32 (0x3144);
	RCBA32 (0x3148) = 0x00000010;
	(void) RCBA32 (0x3148);
	RCBA32 (0x314c) = 0x00007654;
	(void) RCBA32 (0x314c);
	RCBA32 (0x3150) = 0x00000004;
	(void) RCBA32 (0x3150);
	RCBA32 (0x3158) = 0x00000000;
	(void) RCBA32 (0x3158);
	RCBA32 (0x315c) = 0x00003210;
	(void) RCBA32 (0x315c);
	RCBA32 (0x31fc) = 0x03000000;
	(void) RCBA32 (0x31fc);
	RCBA32 (0x330c) = 0x00000000;
	(void) RCBA32 (0x330c);
	RCBA32 (0x3310) = 0x02060100;
	(void) RCBA32 (0x3310);
	RCBA32 (0x3314) = 0x0000000f;
	(void) RCBA32 (0x3314);
	RCBA32 (0x3318) = 0x01020000;
	(void) RCBA32 (0x3318);
	RCBA32 (0x331c) = 0x80000000;
	(void) RCBA32 (0x331c);
	RCBA32 (0x3324) = 0x04000000;
	(void) RCBA32 (0x3324);
	RCBA32 (0x3340) = 0x000fffff;
	(void) RCBA32 (0x3340);
	RCBA32 (0x3378) = 0x7f8fdfff;
	(void) RCBA32 (0x3378);
	RCBA32 (0x33a0) = 0x00003900;
	(void) RCBA32 (0x33a0);
	RCBA32 (0x33c0) = 0x00010000;
	(void) RCBA32 (0x33c0);
	RCBA32 (0x33cc) = 0x0001004b;
	(void) RCBA32 (0x33cc);
	RCBA32 (0x33d0) = 0x06000008;
	(void) RCBA32 (0x33d0);
	RCBA32 (0x33d4) = 0x00010000;
	(void) RCBA32 (0x33d4);
	RCBA32 (0x33fc) = 0x00000000;
	(void) RCBA32 (0x33fc);
	RCBA32 (0x3400) = 0x0000001c;
	(void) RCBA32 (0x3400);
	RCBA32 (0x3404) = 0x00000080;
	(void) RCBA32 (0x3404);
	RCBA32 (0x340c) = 0x00000000;
	(void) RCBA32 (0x340c);
	RCBA32 (0x3410) = 0x00000c61;
	(void) RCBA32 (0x3410);
	RCBA32 (0x3414) = 0x00000000;
	(void) RCBA32 (0x3414);
	RCBA32 (0x3418) = 0x16e61fe1;
	(void) RCBA32 (0x3418);
	RCBA32 (0x341c) = 0xbf4f001f;
	(void) RCBA32 (0x341c);
	RCBA32 (0x3420) = 0x00000000;
	(void) RCBA32 (0x3420);
	RCBA32 (0x3424) = 0x00060010;
	(void) RCBA32 (0x3424);
	RCBA32 (0x3428) = 0x0000001d;
	(void) RCBA32 (0x3428);
	RCBA32 (0x343c) = 0x00000000;
	(void) RCBA32 (0x343c);
	RCBA32 (0x3440) = 0xdeaddeed;
	(void) RCBA32 (0x3440);
	RCBA32 (0x34fc) = 0x00000000;
	(void) RCBA32 (0x34fc);
	RCBA32 (0x3500) = 0x20000557;
	(void) RCBA32 (0x3500);
	RCBA32 (0x3504) = 0x2000055f;
	(void) RCBA32 (0x3504);
	RCBA32 (0x3508) = 0x2000074b;
	(void) RCBA32 (0x3508);
	RCBA32 (0x350c) = 0x2000074b;
	(void) RCBA32 (0x350c);
	RCBA32 (0x3510) = 0x20000557;
	(void) RCBA32 (0x3510);
	RCBA32 (0x3514) = 0x2000014b;
	(void) RCBA32 (0x3514);
	RCBA32 (0x3518) = 0x2000074b;
	(void) RCBA32 (0x3518);
	RCBA32 (0x351c) = 0x2000074b;
	(void) RCBA32 (0x351c);
	RCBA32 (0x3520) = 0x2000074b;
	(void) RCBA32 (0x3520);
	RCBA32 (0x3524) = 0x2000074b;
	(void) RCBA32 (0x3524);
	RCBA32 (0x3528) = 0x2000055f;
	(void) RCBA32 (0x3528);
	RCBA32 (0x352c) = 0x2000055f;
	(void) RCBA32 (0x352c);
	RCBA32 (0x3530) = 0x20000557;
	(void) RCBA32 (0x3530);
	RCBA32 (0x3534) = 0x2000055f;
	(void) RCBA32 (0x3534);
	RCBA32 (0x355c) = 0x00000000;
	(void) RCBA32 (0x355c);
	RCBA32 (0x3560) = 0x00000001;
	(void) RCBA32 (0x3560);
	RCBA32 (0x3564) = 0x000026a3;
	(void) RCBA32 (0x3564);
	RCBA32 (0x3568) = 0x00040002;
	(void) RCBA32 (0x3568);
	RCBA32 (0x356c) = 0x01000052;
	(void) RCBA32 (0x356c);
	RCBA32 (0x3570) = 0x02000772;
	(void) RCBA32 (0x3570);
	RCBA32 (0x3574) = 0x16000f8f;
	(void) RCBA32 (0x3574);
	RCBA32 (0x3578) = 0x1800ff4f;
	(void) RCBA32 (0x3578);
	RCBA32 (0x357c) = 0x0001d630;
	(void) RCBA32 (0x357c);
	RCBA32 (0x359c) = 0x00000000;
	(void) RCBA32 (0x359c);
	RCBA32 (0x35a0) = 0xfc000201;
	(void) RCBA32 (0x35a0);
	RCBA32 (0x35a4) = 0x3c000201;
	(void) RCBA32 (0x35a4);
	RCBA32 (0x35fc) = 0x00000000;
	(void) RCBA32 (0x35fc);
	RCBA32 (0x3600) = 0x0a001f00;
	(void) RCBA32 (0x3600);
	RCBA32 (0x3608) = 0x00000000;
	(void) RCBA32 (0x3608);
	RCBA32 (0x360c) = 0x00000001;
	(void) RCBA32 (0x360c);
	RCBA32 (0x3610) = 0x00010000;
	(void) RCBA32 (0x3610);
	RCBA32 (0x36d0) = 0x00000000;
	(void) RCBA32 (0x36d0);
	RCBA32 (0x36d4) = 0x089c0018;
	(void) RCBA32 (0x36d4);
	RCBA32 (0x36dc) = 0x00000000;
	(void) RCBA32 (0x36dc);
	RCBA32 (0x36e0) = 0x11111111;
	(void) RCBA32 (0x36e0);
	RCBA32 (0x3720) = 0x00000000;
	(void) RCBA32 (0x3720);
	RCBA32 (0x3724) = 0x4e564d49;
	(void) RCBA32 (0x3724);
	RCBA32 (0x37fc) = 0x00000000;
	(void) RCBA32 (0x37fc);
	RCBA32 (0x3800) = 0x07ff0500;
	(void) RCBA32 (0x3800);
	RCBA32 (0x3804) = 0x3f04e008;
	(void) RCBA32 (0x3804);
	RCBA32 (0x3808) = 0x0058efc0;
	(void) RCBA32 (0x3808);
	RCBA32 (0x380c) = 0x00000000;
	(void) RCBA32 (0x380c);
	RCBA32 (0x384c) = 0x92000000;
	(void) RCBA32 (0x384c);
	RCBA32 (0x3850) = 0x00000a0b;
	(void) RCBA32 (0x3850);
	RCBA32 (0x3854) = 0x00000000;
	(void) RCBA32 (0x3854);
	RCBA32 (0x3858) = 0x07ff0500;
	(void) RCBA32 (0x3858);
	RCBA32 (0x385c) = 0x04ff0003;
	(void) RCBA32 (0x385c);
	RCBA32 (0x3860) = 0x00020001;
	(void) RCBA32 (0x3860);
	RCBA32 (0x3864) = 0x00000fff;
	(void) RCBA32 (0x3864);
	RCBA32 (0x3870) = 0x00000000;
	(void) RCBA32 (0x3870);
	RCBA32 (0x3874) = 0x9fff07d0;
	(void) RCBA32 (0x3874);
	RCBA32 (0x388c) = 0x00000000;
	(void) RCBA32 (0x388c);
	RCBA32 (0x3890) = 0xf8400000;
	(void) RCBA32 (0x3890);
	RCBA32 (0x3894) = 0x143b5006;
	(void) RCBA32 (0x3894);
	RCBA32 (0x3898) = 0x05200302;
	(void) RCBA32 (0x3898);
	RCBA32 (0x389c) = 0x0601209f;
	(void) RCBA32 (0x389c);
	RCBA32 (0x38ac) = 0x00000000;
	(void) RCBA32 (0x38ac);
	RCBA32 (0x38b0) = 0x00000004;
	(void) RCBA32 (0x38b0);
	RCBA32 (0x38b4) = 0x03040002;
	(void) RCBA32 (0x38b4);
	RCBA32 (0x38c0) = 0x00000007;
	(void) RCBA32 (0x38c0);
	RCBA32 (0x38c4) = 0x00802005;
	(void) RCBA32 (0x38c4);
	RCBA32 (0x38c8) = 0x00002005;
	(void) RCBA32 (0x38c8);
	RCBA32 (0x3dfc) = 0x00000000;
	(void) RCBA32 (0x3dfc);
	RCBA32 (0x3e7c) = 0xffffffff;
	(void) RCBA32 (0x3e7c);
	RCBA32 (0x3efc) = 0x00000000;
	(void) RCBA32 (0x3efc);
	RCBA32 (0x3f00) = 0x0000010b;
	(void) RCBA32 (0x3f00);
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

static void enable_clock_gating(device_t dev)
{
	u32 reg32;
	u16 reg16;

	RCBA32_AND_OR(0x2234, ~0UL, 0xf);

	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 |= (1 << 2) | (1 << 11);
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	pch_iobp_update(0xEB007F07, ~0UL, (1 << 31));
	pch_iobp_update(0xEB004000, ~0UL, (1 << 7));
	pch_iobp_update(0xEC007F07, ~0UL, (1 << 31));
	pch_iobp_update(0xEC004000, ~0UL, (1 << 7));

	reg32 = RCBA32(CG);
	reg32 |= (1 << 31);
	reg32 |= (1 << 29) | (1 << 28);
	reg32 |= (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24);
	reg32 |= (1 << 16);
	reg32 |= (1 << 17);
	reg32 |= (1 << 18);
	reg32 |= (1 << 22);
	reg32 |= (1 << 23);
	reg32 &= ~(1 << 20);
	reg32 |= (1 << 19);
	reg32 |= (1 << 0);
	reg32 |= (0xf << 1);
	RCBA32(CG) = reg32;

	RCBA32_OR(0x38c0, 0x7);
	RCBA32_OR(0x36d4, 0x6680c004);
	RCBA32_OR(0x3564, 0x3);
}

#if CONFIG_HAVE_SMI_HANDLER
static void pch_lock_smm(struct device *dev)
{
#if TEST_SMM_FLASH_LOCKDOWN
	u8 reg8;
#endif

	if (acpi_slp_type != 3) {
#if ENABLE_ACPI_MODE_IN_COREBOOT
		printk(BIOS_DEBUG, "Enabling ACPI via APMC:\n");
		outb(0xe1, 0xb2); // Enable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#else
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(0x1e, 0xb2); // Disable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#endif
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

static void pch_disable_smm_only_flashing(struct device *dev)
{
	u8 reg8;

	printk(BIOS_SPEW, "Enabling BIOS updates outside of SMM... ");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	reg8 &= ~(1 << 5);
	pci_write_config8(dev, 0xdc, reg8);
}

static void pch_fixups(struct device *dev)
{
	/*
	 * Enable DMI ASPM in the PCH
	 */
	RCBA32_AND_OR(0x2304, ~(1 << 10), 0);
	RCBA32_OR(0x21a4, (1 << 11)|(1 << 10));
	RCBA32_OR(0x21a8, 0x3);
}

static void pch_decode_init(struct device *dev)
{
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "pch_decode_init\n");

	pci_write_config32(dev, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(dev, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(dev, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(dev, LPC_GEN4_DEC, config->gen4_dec);
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: lpc_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	pch_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);

	/* Setup power options. */
	pch_power_options(dev);

	/* Initialize power management */
	switch (pch_silicon_type()) {
	case PCH_TYPE_MOBILE5:
		mobile5_pm_init (dev);
		break;
	default:
		printk(BIOS_ERR, "Unknown Chipset: 0x%04x\n", dev->device);
	}

	/* Set the state of the GPIO lines. */
	//gpio_init(dev);

	/* Initialize the real time clock. */
	pch_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet();

	/* Initialize Clock Gating */
	enable_clock_gating(dev);

	setup_i8259();

	/* The OS should do this? */
	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	pch_disable_smm_only_flashing(dev);

#if CONFIG_HAVE_SMI_HANDLER
	pch_lock_smm(dev);
#endif

	pch_fixups(dev);
}

static void pch_lpc_read_resources(device_t dev)
{
	struct resource *res;
	config_t *config = dev->chip_info;
	u8 io_index = 0;

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

	/* Set PCH IO decode ranges if required.*/
	if ((config->gen1_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen1_dec & 0xFFFC;
		res->size = (config->gen1_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen2_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen2_dec & 0xFFFC;
		res->size = (config->gen2_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen3_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen3_dec & 0xFFFC;
		res->size = (config->gen3_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen4_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen4_dec & 0xFFFC;
		res->size = (config->gen4_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO| IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}
}

static void pch_lpc_enable_resources(device_t dev)
{
	pch_decode_init(dev);
	return pci_dev_enable_resources(dev);
}

static void pch_lpc_enable(device_t dev)
{
	/* Enable PCH Display Port */
	RCBA16(DISPBDF) = 0x0010;
	RCBA32_OR(FD2, PCH_ENABLE_DBDF);

	pch_enable(dev);
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

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pch_lpc_enable_resources,
	.init			= lpc_init,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_ops,
};


/* IDs for LPC device of Intel 5 Series Chipset */

static const unsigned short pci_device_ids[] = { 0x3b07, 0 };

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
