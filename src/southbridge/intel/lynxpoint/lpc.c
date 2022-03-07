/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <option.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include "chip.h"
#include "iobp.h"
#include "pch.h"
#include <acpi/acpigen.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>
#include <southbridge/intel/common/rtc.h>
#include <southbridge/intel/common/spi.h>
#include <types.h>

#define NMI_OFF	0

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	/* Assign unique bus/dev/fn for I/O APIC */
	pci_write_config16(dev, LPC_IBDF,
		PCH_IOAPIC_PCI_BUS << 8 | PCH_IOAPIC_PCI_SLOT << 3);

	/* affirm full set of redirection table entries ("write once") */
	/* PCH-LP has 40 redirection entries */
	if (pch_is_lp())
		ioapic_set_max_vectors(VIO_APIC_VADDR, 40);
	else
		ioapic_lock_max_vectors(VIO_APIC_VADDR);

	setup_ioapic(VIO_APIC_VADDR, 0x02);

}

static void pch_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
#if !CONFIG(SERIRQ_CONTINUOUS_MODE)
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
#endif
}

static void enable_hpet(struct device *const dev)
{
	u32 reg32;
	size_t i;

	/* Assign unique bus/dev/fn for each HPET */
	for (i = 0; i < 8; ++i)
		pci_write_config16(dev, LPC_HnBDF(i),
			PCH_HPET_PCI_BUS << 8 | PCH_HPET_PCI_SLOT << 3 | i);

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(HPTC);
	reg32 |= (1 << 7); // HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(HPTC) = reg32;
	/* Read it back to stick. It's affected by posted write syndrome. */
	RCBA32(HPTC);
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

static void pch_pirq_init(struct device *dev)
{
	struct device *irq_dev;

	const uint8_t pirq = 0x80;

	pci_write_config8(dev, PIRQA_ROUT, pirq);
	pci_write_config8(dev, PIRQB_ROUT, pirq);
	pci_write_config8(dev, PIRQC_ROUT, pirq);
	pci_write_config8(dev, PIRQD_ROUT, pirq);

	pci_write_config8(dev, PIRQE_ROUT, pirq);
	pci_write_config8(dev, PIRQF_ROUT, pirq);
	pci_write_config8(dev, PIRQG_ROUT, pirq);
	pci_write_config8(dev, PIRQH_ROUT, pirq);

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
		case 2: /* INTB# */
		case 3: /* INTC# */
		case 4: /* INTD# */
			int_line = pirq;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void pch_gpi_routing(struct device *dev,
			    struct southbridge_intel_lynxpoint_config *config)
{
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

static void pch_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;
	const char *state;
	u16 pmbase = get_pmbase();

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use Kconfig setting.
	 */
	const unsigned int pwr_on = get_uint_option("power_on_after_fail",
					  CONFIG_MAINBOARD_POWER_FAILURE_STATE);

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
	const unsigned int nmi_option = get_uint_option("nmi", NMI_OFF);
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
	reg16 &= ~(1 << 10);	// Disable BIOS_PCI_EXP_EN for native PME
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	if (dev->chip_info) {
		struct southbridge_intel_lynxpoint_config *config = dev->chip_info;

		/*
		 * Set the board's GPI routing on LynxPoint-H.
		 * This is done as part of GPIO configuration on LynxPoint-LP.
		 */
		if (!pch_is_lp())
			pch_gpi_routing(dev, config);

		/* GPE setup based on device tree configuration */
		enable_all_gpe(config->gpe0_en_1, config->gpe0_en_2,
			       config->gpe0_en_3, config->gpe0_en_4);

		/* SMI setup based on device tree configuration */
		enable_alt_smi(config->alt_gp_smi_en);
	}

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT
	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);

	/* Clear magic status bits to prevent unexpected wake */
	reg32 = RCBA32(0x3310);
	reg32 |= (1 << 4) | (1 << 5) | (1 << 0);
	RCBA32(0x3310) = reg32;

	reg16 = RCBA16(0x3f02);
	reg16 &= ~0xf;
	RCBA16(0x3f02) = reg16;
}

static void configure_dmi_pm(struct device *dev)
{
	struct device *const pcie_dev = pcidev_on_root(0x1c, 0);

	/* Additional PCH DMI programming steps */

	/* EL0 */
	u32 reg32 = 3 << 12;

	/* EL1 */
	if (pcie_dev && !(pci_read_config8(pcie_dev, 0xf5) & 1 << 0))
		reg32 |= 2 << 15;
	else
		reg32 |= 4 << 15;

	RCBA32_AND_OR(0x21a4, ~(7 << 15 | 7 << 12), reg32);

	RCBA32_AND_OR(0x2348, ~0xf, 0);

	/* Clear prior to enabling DMI ASPM */
	RCBA32_AND_OR(0x2304, ~(1 << 10), 0);

	RCBA32_OR(0x21a4, 3 << 10);

	RCBA16(0x21a8) |= 3 << 0;

	/* Set again after enabling DMI ASPM */
	RCBA32_OR(0x2304, 1 << 10);
}

/* LynxPoint PCH Power Management init */
static void lpt_pm_init(struct device *dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;

	struct device *const pcie_dev = pcidev_on_root(0x1c, 0);

	printk(BIOS_DEBUG, "LynxPoint H PM init\n");

	/* Configure additional PM */
	pci_write_config8(dev, 0xa9, 0x46);

	pci_or_config32(dev, PMIR, PMIR_CF9LOCK);

	/* Step 3 is skipped */

	/* Program DMI Hardware Width Control (thermal throttling) */
	u32 reg32 = 0;
	reg32 |= 1 <<  0;	/* DMI Thermal Sensor Autonomous Width Enable */
	reg32 |= 0 <<  4;	/* Thermal Sensor 0 Target Width */
	reg32 |= 1 <<  6;	/* Thermal Sensor 1 Target Width */
	reg32 |= 1 <<  8;	/* Thermal Sensor 2 Target Width */
	reg32 |= 2 << 10;	/* Thermal Sensor 3 Target Width */
	RCBA32(0x2238) = reg32;

	RCBA32_OR(0x232c, 1 << 0);
	RCBA32_OR(0x1100, 3 << 13);	/* Assume trunk clock gating is to be enabled */

	RCBA32(0x2304) = 0xc07b8400;	/* DMI misc control */

	RCBA32_OR(0x2314, 1 << 23 | 1 << 5);

	if (pcie_dev)
		pci_update_config8(pcie_dev, 0xf5, ~0xf, 0x5);

	RCBA32_OR(0x2320, 1 << 1);

	RCBA32(0x3314) = 0x000007bf;

	/* NOTE: Preserve bit 5 */
	RCBA32_OR(0x3318, 0x0dcf0000);

	RCBA32(0x3324) = 0x04000000;
	RCBA32(0x3340) = 0x020ddbff;

	RCBA32_OR(0x3344, 1 << 0);

	RCBA32(0x3368) = 0x00041000;
	RCBA32(0x3378) = 0x3f8ddbff;
	RCBA32(0x337c) = 0x000001e1;
	RCBA32(0x3388) = 0x00001000;
	RCBA32(0x33a0) = 0x00000800;
	RCBA32(0x33ac) = 0x00001000;
	RCBA32(0x33b0) = 0x00001000;
	RCBA32(0x33c0) = 0x00011900;
	RCBA32(0x33d0) = 0x06000802;
	RCBA32(0x3a28) = 0x01010000;
	RCBA32(0x3a2c) = 0x01010404;

	RCBA32_OR(0x33a4, 1 << 0);

	/* DMI power optimizer */
	RCBA32_OR(0x33d4, 1 << 27);
	RCBA32_OR(0x33c8, 1 << 27);
	RCBA32(0x2b14) = 0x1e0a0317;
	RCBA32(0x2b24) = 0x4000000b;
	RCBA32(0x2b28) = 0x00000002;
	RCBA32(0x2b2c) = 0x00008813;

	RCBA32(0x3a80) = 0x01040000;
	reg32 = 0x01041001;
	/* Port 1 and 0 disabled */
	if (config && (config->sata_port_map & ((1 << 1) | (1 << 0))) == 0)
		reg32 |= (1 << 20) | (1 << 18);
	/* Port 3 and 2 disabled */
	if (config && (config->sata_port_map & ((1 << 3) | (1 << 2))) == 0)
		reg32 |= (1 << 24) | (1 << 26);
	RCBA32(0x3a84) = reg32;
	RCBA32(0x3a88) = 0x00000001;
	RCBA32(0x33d4) = 0xc80bc000;

	configure_dmi_pm(dev);
}

/* LynxPoint LP PCH Power Management init */
static void lpt_lp_pm_init(struct device *dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
	u32 data;

	printk(BIOS_DEBUG, "LynxPoint LP PM init\n");

	pci_write_config8(dev, 0xa9, 0x46);

	RCBA32_AND_OR(0x232c, ~1, 0);

	RCBA32_AND_OR(0x1100, ~0xc000, 0xc000);
	RCBA32_OR(0x1100, 0x00000100);
	RCBA32_OR(0x1100, 0x0000003f);

	RCBA32_AND_OR(0x2320, ~0x60, 0x10);

	RCBA32(0x3314) = 0x00012fff;
	RCBA32(0x3318) = 0x0dcf0400;
	RCBA32(0x3324) = 0x04000000;
	RCBA32(0x3368) = 0x00041400;
	RCBA32(0x3388) = 0x3f8ddbff;
	RCBA32(0x33ac) = 0x00007001;
	RCBA32(0x33b0) = 0x00181900;
	RCBA32(0x33c0) = 0x00060A00;
	RCBA32(0x33d0) = 0x06200840;
	RCBA32(0x3a28) = 0x01010101;
	RCBA32(0x3a2c) = 0x04040404;
	RCBA32(0x2b1c) = 0x03808033;
	RCBA32(0x2b34) = 0x80000009;
	RCBA32(0x3348) = 0x022ddfff;
	RCBA32(0x334c) = 0x00000001;
	RCBA32(0x3358) = 0x0001c000;
	RCBA32(0x3380) = 0x3f8ddbff;
	RCBA32(0x3384) = 0x0001c7e1;
	RCBA32(0x338c) = 0x0001c7e1;
	RCBA32(0x3398) = 0x0001c000;
	RCBA32(0x33a8) = 0x00181900;
	RCBA32(0x33dc) = 0x00080000;
	RCBA32(0x33e0) = 0x00000001;
	RCBA32(0x3a20) = 0x00000404;
	RCBA32(0x3a24) = 0x01010101;
	RCBA32(0x3a30) = 0x01010101;

	RCBA32_OR(0x0410, 0x00000003);
	RCBA32_OR(0x2618, 0x08000000);
	RCBA32_OR(0x2300, 0x00000002);
	RCBA32_OR(0x2600, 0x00000008);

	RCBA32(0x33b4) = 0x00007001;
	RCBA32(0x3350) = 0x022ddfff;
	RCBA32(0x3354) = 0x00000001;

	/* Power Optimizer */
	RCBA32_OR(0x33d4, 0x08000000);
	RCBA32_OR(0x33c8, 0x00000080);

	RCBA32(0x2b10) = 0x0000883c;
	RCBA32(0x2b14) = 0x1e0a4616;
	RCBA32(0x2b24) = 0x40000005;
	RCBA32(0x2b20) = 0x0005db01;
	RCBA32(0x3a80) = 0x05145005;

	pci_or_config32(dev, 0xac, 1 << 21);

	pch_iobp_update(0xED00015C, ~(1 << 11), 0x00003700);
	pch_iobp_update(0xED000118, ~0, 0x00c00000);
	pch_iobp_update(0xED000120, ~0, 0x00240000);
	pch_iobp_update(0xCA000000, ~0, 0x00000009);

	/* Set RCBA CIR28 0x3A84 based on SATA port enables */
	data = 0x00001005;
	/* Port 3 and 2 disabled */
	if (config && (config->sata_port_map & ((1 << 3) | (1 << 2))) == 0)
		data |= (1 << 24) | (1 << 26);
	/* Port 1 and 0 disabled */
	if (config && (config->sata_port_map & ((1 << 1) | (1 << 0))) == 0)
		data |= (1 << 20) | (1 << 18);
	RCBA32(0x3a84) = data;

	/* Set RCBA 0x2b1c[29]=1 if DSP disabled */
	if (RCBA32(FD) & PCH_DISABLE_ADSPD)
		RCBA32_OR(0x2b1c, (1 << 29));

	/* Set RCBA 0x33D4 after other setup */
	RCBA32_OR(0x33d4, 0x2fff2fb1);

	/* Set RCBA 0x33C8[15]=1 as last step */
	RCBA32_OR(0x33c8, (1 << 15));
}

static void enable_clock_gating(struct device *dev)
{
	/* LynxPoint Mobile */
	u32 reg32;
	u16 reg16;

	/* DMI */
	RCBA32_AND_OR(0x2234, ~0, 0xf);
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 |= (1 << 11) | (1 << 12) | (1 << 14);
	reg16 |= (1 << 2); // PCI CLKRUN# Enable
	pci_write_config16(dev, GEN_PMCON_1, reg16);
	RCBA32_OR(0x900, (1 << 14));

	reg32 = RCBA32(CG);
	reg32 |= (1 << 22); // HDA Dynamic
	reg32 |= (1 << 31); // LPC Dynamic
	reg32 |= (1 << 16); // PCIe Dynamic
	reg32 |= (1 << 27); // HPET Dynamic
	reg32 |= (1 << 28); // GPIO Dynamic
	RCBA32(CG) = reg32;

	RCBA32_OR(0x38c0, 0x7); // SPI Dynamic
}

static void enable_lp_clock_gating(struct device *dev)
{
	/* LynxPoint LP */
	u32 reg32;
	u16 reg16;

	/* DMI */
	RCBA32_AND_OR(0x2234, ~0, 0xf);
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~((1 << 11) | (1 << 14));
	reg16 |= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 12) | (1 << 13);
	reg16 |= (1 << 2); // PCI CLKRUN# Enable
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	pci_or_config32(dev, 0x64, 1 << 6);

	/*
	 * RCBA + 0x2614[27:25,14:13,10,8] = 101,11,1,1
	 * RCBA + 0x2614[23:16] = 0x20
	 * RCBA + 0x2614[30:28] = 0x0
	 * RCBA + 0x2614[26] = 1 (IF 0:2.0@0x08 >= 0x0b)
	 */
	RCBA32_AND_OR(0x2614, ~0x74000000, 0x0a206500);

	/* Check for LPT-LP B2 stepping and 0:31.0@0xFA > 4 */
	struct device *const gma = pcidev_on_root(2, 0);
	if (gma && pci_read_config8(gma, 0x8) >= 0x0b)
		RCBA32_OR(0x2614, (1 << 26));

	RCBA32_OR(0x900, 0x0000031f);

	reg32 = RCBA32(CG);
	if (RCBA32(0x3454) & (1 << 4))
		reg32 &= ~(1 << 29); // LPC Dynamic
	else
		reg32 |= (1 << 29); // LPC Dynamic
	reg32 |= (1 << 31); // LP LPC
	reg32 |= (1 << 30); // LP BLA
	reg32 |= (1 << 28); // GPIO Dynamic
	reg32 |= (1 << 27); // HPET Dynamic
	reg32 |= (1 << 26); // Generic Platform Event Clock
	if (RCBA32(BUC) & PCH_DISABLE_GBE)
		reg32 |= (1 << 23); // GbE Static
	reg32 |= (1 << 22); // HDA Dynamic
	reg32 |= (1 << 16); // PCI Dynamic
	RCBA32(CG) = reg32;

	RCBA32_OR(0x3434, 0x7); // LP LPC

	RCBA32_OR(0x38c0, 0x3c07); // SPI Dynamic

	pch_iobp_update(0xCF000000, ~0, 0x00007001);
	pch_iobp_update(0xCE00C000, ~1, 0x00000000); // bit0=0 in BWG 1.4.0
}

static void pch_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3())
		apm_control(APM_CNT_ACPI_DISABLE);
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: %s\n", __func__);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	pch_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);

	/* Setup power options. */
	pch_power_options(dev);

	/* Initialize power management */
	if (pch_is_lp()) {
		lpt_lp_pm_init(dev);
		enable_lp_clock_gating(dev);
	} else {
		lpt_pm_init(dev);
		enable_clock_gating(dev);
	}

	/* Initialize the real time clock. */
	sb_rtc_init();

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);

	setup_i8259();

	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	pch_set_acpi_mode();

	/* Indicate DRAM init done for MRC S3 to know it can resume */
	pci_or_config8(dev, GEN_PMCON_2, 1 << 7);
}

static void pch_lpc_add_mmio_resources(struct device *dev)
{
	u32 reg;
	struct resource *res;
	const u32 default_decode_base = IO_APIC_ADDR;

	/*
	 * Just report all resources from IO-APIC base to 4GiB. Don't mark
	 * them reserved as that may upset the OS if this range is marked
	 * as reserved in the e820.
	 */
	res = new_resource(dev, OIC);
	res->base = default_decode_base;
	res->size = 0 - default_decode_base;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* RCBA */
	if (CONFIG_FIXED_RCBA_MMIO_BASE < default_decode_base) {
		res = new_resource(dev, RCBA);
		res->base = (resource_t)CONFIG_FIXED_RCBA_MMIO_BASE;
		res->size = CONFIG_RCBA_LENGTH;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			     IORESOURCE_FIXED | IORESOURCE_RESERVE;
	}

	/* Check LPC Memory Decode register. */
	reg = pci_read_config32(dev, LGMR);
	if (reg & 1) {
		reg &= ~0xffff;
		if (reg < default_decode_base) {
			res = new_resource(dev, LGMR);
			res->base = reg;
			res->size = 16 * 1024;
			res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
				     IORESOURCE_FIXED | IORESOURCE_RESERVE;
		}
	}
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static inline int pch_io_range_in_default(int base, int size)
{
	/* Does it start above the range? */
	if (base >= LPC_DEFAULT_IO_RANGE_UPPER)
		return 0;

	/* Is it entirely contained? */
	if (base >= LPC_DEFAULT_IO_RANGE_LOWER &&
	    (base + size) < LPC_DEFAULT_IO_RANGE_UPPER)
		return 1;

	/* This will return not in range for partial overlaps. */
	return 0;
}

/*
 * Note: this function assumes there is no overlap with the default LPC device's
 * claimed range: LPC_DEFAULT_IO_RANGE_LOWER -> LPC_DEFAULT_IO_RANGE_UPPER.
 */
static void pch_lpc_add_io_resource(struct device *dev, u16 base, u16 size,
				    int index)
{
	struct resource *res;

	if (pch_io_range_in_default(base, size))
		return;

	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void pch_lpc_add_gen_io_resources(struct device *dev, int reg_value,
					 int index)
{
	/*
	 * Check if the register is enabled. If so, and the base exceeds the
	 * device's default claim range, add the resource.
	 */
	if (reg_value & 1) {
		u16 base = reg_value & 0xfffc;
		u16 size = (0x3 | ((reg_value >> 16) & 0xfc)) + 1;
		pch_lpc_add_io_resource(dev, base, size, index);
	}
}

static void pch_lpc_add_io_resources(struct device *dev)
{
	struct resource *res;

	/* Add the default claimed IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* GPIOBASE */
	pch_lpc_add_io_resource(dev, get_gpiobase(), DEFAULT_GPIOSIZE, GPIO_BASE);

	/* PMBASE */
	pch_lpc_add_io_resource(dev, get_pmbase(), 256, PMBASE);

	/* LPC Generic IO Decode range. */
	if (dev->chip_info) {
		struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
		pch_lpc_add_gen_io_resources(dev, config->gen1_dec, LPC_GEN1_DEC);
		pch_lpc_add_gen_io_resources(dev, config->gen2_dec, LPC_GEN2_DEC);
		pch_lpc_add_gen_io_resources(dev, config->gen3_dec, LPC_GEN3_DEC);
		pch_lpc_add_gen_io_resources(dev, config->gen4_dec, LPC_GEN4_DEC);
	}
}

static void pch_lpc_read_resources(struct device *dev)
{
	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);
}

static void pch_lpc_enable(struct device *dev)
{
	/* Enable PCH Display Port */
	RCBA16(DISPBDF) = 0x0010;
	RCBA32_OR(FD2, PCH_ENABLE_DBDF);

	pch_enable(dev);
}

static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}

static void southbridge_fill_ssdt(const struct device *dev)
{
	intel_acpi_gen_def_acpi_pirq(dev);
}

static unsigned long southbridge_write_acpi_tables(const struct device *device,
						   unsigned long start,
						   struct acpi_rsdp *rsdp)
{
	unsigned long current;

	current = start;

	/* Align ACPI tables to 16byte */
	current = acpi_align_current(current);

	/*
	 * We explicitly add these tables later on:
	 */
	current = acpi_write_hpet(device, current, rsdp);

	current = acpi_align_current(current);

	if (pch_is_lp()) {
		printk(BIOS_DEBUG, "ACPI:     * SSDT2\n");
		acpi_header_t *ssdt = (acpi_header_t *)current;
		acpi_create_serialio_ssdt(ssdt);
		current += ssdt->length;
		acpi_add_table(rsdp, ssdt);
		current = acpi_align_current(current);
	}

	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}

static void lpc_final(struct device *dev)
{
	spi_finalize_ops();

	/* Lock */
	RCBA32_OR(0x3a6c, 0x00000001);

	if (acpi_is_wakeup_s3() || CONFIG(INTEL_CHIPSET_LOCKDOWN))
		apm_control(APM_CNT_FINALIZE);
}

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.write_acpi_tables      = southbridge_write_acpi_tables,
	.init			= lpc_init,
	.final			= lpc_final,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

/* IDs for LPC device of Intel 8 Series Chipset (Lynx Point) */
static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_LPT_MOBILE_SAMPLE,
	PCI_DID_INTEL_LPT_DESKTOP_SAMPLE,
	PCI_DID_INTEL_LPT_Z87,
	PCI_DID_INTEL_LPT_Z85,
	PCI_DID_INTEL_LPT_HM86,
	PCI_DID_INTEL_LPT_H87,
	PCI_DID_INTEL_LPT_HM87,
	PCI_DID_INTEL_LPT_Q85,
	PCI_DID_INTEL_LPT_Q87,
	PCI_DID_INTEL_LPT_QM87,
	PCI_DID_INTEL_LPT_B85,
	PCI_DID_INTEL_LPT_C222,
	PCI_DID_INTEL_LPT_C224,
	PCI_DID_INTEL_LPT_C226,
	PCI_DID_INTEL_LPT_H81,
	PCI_DID_INTEL_LPT_LP_SAMPLE,
	PCI_DID_INTEL_LPT_LP_PREMIUM,
	PCI_DID_INTEL_LPT_LP_MAINSTREAM,
	PCI_DID_INTEL_LPT_LP_VALUE,
	0
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
