/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <option.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <cpu/x86/smm.h>
#include <string.h>
#include "chip.h"
#include "pch.h"
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/common/pciehp.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/rcba_pirq.h>
#include <southbridge/intel/common/rtc.h>
#include <southbridge/intel/common/spi.h>
#include <types.h>

#define NMI_OFF	0

typedef struct southbridge_intel_bd82x6x_config config_t;

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
	/* Interrupt 11 is not used by legacy devices and so can always be used for
	   PCI interrupts. Full legacy IRQ routing is complicated and hard to
	   get right. Fortunately all modern OS use MSI and so it's not that big of
	   an issue anyway. Still we have to provide a reasonable default. Using
	   interrupt 11 for it everywhere is a working default. ACPI-aware OS can
	   move it to any interrupt and others will just leave them at default.
	 */
	const u8 pirq_routing = 11;

	pci_write_config8(dev, PIRQA_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQB_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQC_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQD_ROUT, pirq_routing);

	pci_write_config8(dev, PIRQE_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQF_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQG_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQH_ROUT, pirq_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin=0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		if (int_pin == 0)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, pirq_routing);
	}
}

static void pch_gpi_routing(struct device *dev)
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

static void pch_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

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
	if (CONFIG(DEBUG_PERIODIC_SMI))
		reg16 |= (3 << 0); // Periodic SMI every 8s
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
	reg32 = RCBA32(PRSTS);
	reg32 |= (1 << 4)|(1 << 5)|(1 << 0);
	RCBA32(PRSTS) = reg32;

	reg32 = RCBA32(0x3f02);
	reg32 &= ~0xf;
	RCBA32(0x3f02) = reg32;
}

/* CougarPoint PCH Power Management init */
static void cpt_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "CougarPoint PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);
	RCBA32_AND_OR(CIR30, ~0U, (1 << 6)|(1 << 0));
	RCBA32_AND_OR(CIR5, ~0U, (1 << 0));
	RCBA16_AND_OR(CIR3, ~0U, (1 << 13)|(1 << 14));
	RCBA16_AND_OR(CIR2, ~0U, (1 << 14));
	RCBA32(DMC) = 0xc0388400;
	RCBA32_AND_OR(CIR6, ~0U, (1 << 5)|(1 << 18));
	RCBA32_AND_OR(CIR9, ~0U, (1 << 15)|(1 << 1));
	RCBA32_AND_OR(CIR7, ~0x1f, 0xf);
	RCBA32(PM_CFG) = 0x050f0000;
	RCBA32(CIR8) = 0x04000000;
	RCBA32_AND_OR(CIR10, ~0U, 0xfffff);
	RCBA32_AND_OR(CIR11, ~0U, (1 << 1));
	RCBA32(CIR12) = 0x0001c000;
	RCBA32(CIR14) = 0x00061100;
	RCBA32(CIR15) = 0x7f8fdfff;
	RCBA32(CIR13) = 0x000003fc;
	RCBA32(CIR16) = 0x00001000;
	RCBA32(CIR18) = 0x0001c000;
	RCBA32(CIR17) = 0x00000800;
	RCBA32(CIR23) = 0x00001000;
	RCBA32(CIR19) = 0x00093900;
	RCBA32(CIR20) = 0x24653002;
	RCBA32(CIR21) = 0x062108fe;
	RCBA32_AND_OR(CIR22, 0xf000f000, 0x00670060);
	RCBA32(CIR24) = 0x01010000;
	RCBA32(CIR25) = 0x01010404;
	RCBA32(CIR27) = 0x01041041;
	RCBA32_AND_OR(CIR28, ~0x0000ffff, 0x00001001);
	RCBA32_AND_OR(CIR28, ~0UL, (1 << 24)); /* SATA 2/3 disabled */
	RCBA32_AND_OR(CIR29, ~0UL, (1 << 0));  /* SATA 4/5 disabled */
	RCBA32(CIR26) = 0x00000001;
	RCBA32_AND_OR(0x2344, 0x00ffff00, 0xff00000c);
	RCBA32_AND_OR(0x80c, ~(0xff << 20), 0x11 << 20);
	RCBA32(PMSYNC_CFG) = 0;
	RCBA32_AND_OR(0x21b0, ~0UL, 0xf);
}

/* PantherPoint PCH Power Management init */
static void ppt_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "PantherPoint PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);
	RCBA32_AND_OR(CIR30, ~0UL, (1 << 0));
	RCBA32_AND_OR(CIR5, ~0UL, (1 << 0));
	RCBA16_AND_OR(CIR3, ~0UL, (1 << 13)|(1 << 14));
	RCBA16_AND_OR(CIR2, ~0UL, (1 << 14));
	RCBA32(DMC) = 0xc03b8400;
	RCBA32_AND_OR(CIR6, ~0UL, (1 << 5)|(1 << 18));
	RCBA32_AND_OR(CIR9, ~0UL, (1 << 15)|(1 << 1));
	RCBA32_AND_OR(CIR7, ~0x1f, 0xf);
	RCBA32(PM_CFG) = 0x054f0000;
	RCBA32(CIR8) = 0x04000000;
	RCBA32_AND_OR(CIR10, ~0UL, 0xfffff);
	RCBA32_AND_OR(CIR11, ~0UL, (1 << 1)|(1 << 0));
	RCBA32(CIR12) = 0x0001c000;
	RCBA32(CIR14) = 0x00061100;
	RCBA32(CIR15) = 0x7f8fdfff;
	RCBA32(CIR13) = 0x000003fd;
	RCBA32(CIR16) = 0x00001000;
	RCBA32(CIR18) = 0x0001c000;
	RCBA32(CIR17) = 0x00000800;
	RCBA32(CIR23) = 0x00001000;
	RCBA32(CIR19) = 0x00093900;
	RCBA32(CIR20) = 0x24653002;
	RCBA32(CIR21) = 0x067388fe;
	RCBA32_AND_OR(CIR22, 0xf000f000, 0x00670060);
	RCBA32(CIR24) = 0x01010000;
	RCBA32(CIR25) = 0x01010404;
	RCBA32(CIR27) = 0x01040000;
	RCBA32_AND_OR(CIR28, ~0x0000ffff, 0x00001001);
	RCBA32_AND_OR(CIR28, ~0UL, (1 << 24)); /* SATA 2/3 disabled */
	RCBA32_AND_OR(CIR29, ~0UL, (1 << 0));  /* SATA 4/5 disabled */
	RCBA32(CIR26) = 0x00000001;
	RCBA32_AND_OR(0x2344, 0x00ffff00, 0xff00000c);
	RCBA32_AND_OR(0x80c, ~(0xff << 20), 0x11 << 20);
	RCBA32_AND_OR(0x33a4, ~0UL, (1 << 0));
	RCBA32(PMSYNC_CFG) = 0;
	RCBA32_AND_OR(0x21b0, ~0UL, 0xf);
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
}

static void enable_clock_gating(struct device *dev)
{
	u32 reg32;
	u16 reg16;

	RCBA32_AND_OR(DMIC, ~0UL, 0xf);

	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~(3 << 2); /* Clear CLKRUN bits for mobile and desktop */
	if (get_platform_type() == PLATFORM_MOBILE)
		reg16 |= (1 << 2); /* CLKRUN_EN for mobile */
	else if (get_platform_type() == PLATFORM_DESKTOP_SERVER)
		reg16 |= (1 << 3); /* PSEUDO_CLKRUN_EN for desktop */
	reg16 |= (1 << 11);
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	pch_iobp_update(0xEB007F07, ~0U, (1 << 31));
	pch_iobp_update(0xEB004000, ~0U, (1 << 7));
	pch_iobp_update(0xEC007F07, ~0U, (1 << 31));
	pch_iobp_update(0xEC004000, ~0U, (1 << 7));

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

static void pch_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	}
}

static void pch_fixups(struct device *dev)
{
	/* Indicate DRAM init done for MRC S3 to know it can resume */
	pci_or_config8(dev, GEN_PMCON_2, 1 << 7);

	/*
	 * Enable DMI ASPM in the PCH
	 */
	RCBA32_AND_OR(DMC, ~(1 << 10), 0);
	RCBA32_OR(LCAP, (1 << 11)|(1 << 10));
	RCBA32_OR(LCTL, 0x3);
}

static void pch_spi_init(const struct device *const dev)
{
	const config_t *const config = dev->chip_info;

	printk(BIOS_DEBUG, "%s\n", __func__);

	if (config->spi_uvscc)
		RCBA32(0x3800 + 0xc8) = config->spi_uvscc;
	if (config->spi_lvscc)
		RCBA32(0x3800 + 0xc4) = config->spi_lvscc;

	if (config->spi_uvscc || config->spi_lvscc)
		RCBA32_OR(0x3800 + 0xc4, 1 << 23); /* lock both UVSCC + LVSCC */
}

static const struct {
	u16 dev_id;
	const char *dev_name;
} pch_table[] = {
	/* 6-series PCI ids from
	 * Intel® 6 Series Chipset and
	 * Intel® C200 Series Chipset
	 * Specification Update - NDA
	 * October 2013
	 * CDI / IBP#: 440377
	 */
	{0x1C41, "SFF Sample"},
	{0x1C42, "Desktop Sample"},
	{0x1C43, "Mobile Sample"},
	{0x1C44, "Z68"},
	{0x1C46, "P67"},
	{0x1C47, "UM67"},
	{0x1C49, "HM65"},
	{0x1C4A, "H67"},
	{0x1C4B, "HM67"},
	{0x1C4C, "Q65"},
	{0x1C4D, "QS67"},
	{0x1C4E, "Q67"},
	{0x1C4F, "QM67"},
	{0x1C50, "B65"},
	{0x1C52, "C202"},
	{0x1C54, "C204"},
	{0x1C56, "C206"},
	{0x1C5C, "H61"},
	/* 7-series PCI ids from Intel document 472178 */
	{0x1E41, "Desktop Sample"},
	{0x1E42, "Mobile Sample"},
	{0x1E43, "SFF Sample"},
	{0x1E44, "Z77"},
	{0x1E45, "H71"},
	{0x1E46, "Z75"},
	{0x1E47, "Q77"},
	{0x1E48, "Q75"},
	{0x1E49, "B75"},
	{0x1E4A, "H77"},
	{0x1E53, "C216"},
	{0x1E55, "QM77"},
	{0x1E56, "QS77"},
	{0x1E58, "UM77"},
	{0x1E57, "HM77"},
	{0x1E59, "HM76"},
	{0x1E5D, "HM75"},
	{0x1E5E, "HM70"},
	{0x1E5F, "NM70"},
};

static void report_pch_info(struct device *dev)
{
	const u16 dev_id = pci_read_config16(dev, PCI_DEVICE_ID);
	int i;

	const char *pch_type = "Unknown";
	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].dev_id == dev_id) {
			pch_type = pch_table[i].dev_name;
			break;
		}
	}
	printk(BIOS_INFO, "PCH: detected %s, device id: 0x%x, rev id 0x%x\n",
		pch_type, dev_id, pci_read_config8(dev, PCI_CLASS_REVISION));
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: %s\n", __func__);

	/* Print detected platform */
	report_pch_info(dev);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	pch_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);

	/* Setup power options. */
	pch_power_options(dev);

	/* Initialize power management */
	switch (pch_silicon_type()) {
	case PCH_TYPE_CPT: /* CougarPoint */
		cpt_pm_init(dev);
		break;
	case PCH_TYPE_PPT: /* PantherPoint */
		ppt_pm_init(dev);
		break;
	default:
		printk(BIOS_ERR, "Unknown Chipset: 0x%04x\n", dev->device);
	}

	/* Initialize the real time clock. */
	sb_rtc_init();

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);

	/* Initialize Clock Gating */
	enable_clock_gating(dev);

	setup_i8259();

	/* The OS should do this? */
	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	pch_set_acpi_mode();

	pch_fixups(dev);

	pch_spi_init(dev);
}

static void pch_lpc_read_resources(struct device *dev)
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
	res->base = 0xff000000;
	/* Some systems (e.g. X230) have 12 MiB flash.
	   SPI controller supports up to 2 x 16 MiB of flash but
	   address map limits this to 16MiB.  */
	res->size = 0x01000000; /* 16 MB for flash */
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

static void southbridge_fill_ssdt(const struct device *device)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	config_t *chip = dev->chip_info;

	intel_acpi_pcie_hotplug_generator(chip->pcie_hotplug_map, 8);
	intel_acpi_gen_def_acpi_pirq(dev);
}

static void lpc_final(struct device *dev)
{
	spi_finalize_ops();

	/* Call SMM finalize() handlers before resume */
	if (CONFIG(INTEL_CHIPSET_LOCKDOWN) ||
	    acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_FINALIZE);
	}
}

void intel_southbridge_override_spi(
		struct intel_swseq_spi_config *spi_config)
{
	struct device *dev = pcidev_on_root(0x1f, 0);

	if (!dev)
		return;
	/* Devicetree may override defaults. */
	const config_t *const config = dev->chip_info;

	if (!config)
		return;

	if (config->spi.ops[0].op != 0)
		memcpy(spi_config, &config->spi, sizeof(*spi_config));
}

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.init			= lpc_init,
	.final			= lpc_final,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

/* IDs for LPC device of Intel 6 Series Chipset, Intel 7 Series Chipset, and
 * Intel C200 Series Chipset
 */

static const unsigned short pci_device_ids[] = {
	0x1c40, 0x1c41, 0x1c42, 0x1c43, 0x1c44, 0x1c45, 0x1c46, 0x1c47, 0x1c48,
	0x1c49, 0x1c4a, 0x1c4b, 0x1c4c, 0x1c4d, 0x1c4e, 0x1c4f, 0x1c50, 0x1c51,
	0x1c52, 0x1c53, 0x1c54, 0x1c55, 0x1c56, 0x1c57, 0x1c58, 0x1c59, 0x1c5a,
	0x1c5b, 0x1c5c, 0x1c5d, 0x1c5e, 0x1c5f,

	0x1e41, 0x1e42, 0x1e43, 0x1e44, 0x1e45, 0x1e46, 0x1e47, 0x1e48, 0x1e49,
	0x1e4a, 0x1e4b, 0x1e4c, 0x1e4d, 0x1e4e, 0x1e4f, 0x1e50, 0x1e51, 0x1e52,
	0x1e53, 0x1e54, 0x1e55, 0x1e56, 0x1e57, 0x1e58, 0x1e59, 0x1e5a, 0x1e5b,
	0x1e5c, 0x1e5d, 0x1e5e, 0x1e5f,

	0 };

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
