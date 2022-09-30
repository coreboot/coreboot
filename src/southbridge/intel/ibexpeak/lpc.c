/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <option.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <elog.h>
#include <acpi/acpigen.h>
#include <cpu/x86/smm.h>
#include "chip.h"
#include "pch.h"
#include <southbridge/intel/common/pciehp.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/spi.h>
#include <southbridge/intel/common/rcba_pirq.h>

#define NMI_OFF	0

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	/* affirm full set of redirection table entries ("write once") */
	ioapic_lock_max_vectors(VIO_APIC_VADDR);

	setup_ioapic(VIO_APIC_VADDR, 0x01);
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
	/*
	 * Interrupt 11 is not used by legacy devices and so can always be used for
	 * PCI interrupts. Full legacy IRQ routing is complicated and hard to
	 * get right. Fortunately all modern OS use MSI and so it's not that big of
	 * an issue anyway. Still we have to provide a reasonable default. Using
	 * interrupt 11 for it everywhere is a working default. ACPI-aware OS can
	 * move it to any interrupt and others will just leave them at default.
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

		if (!is_enabled_pci(irq_dev))
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
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;
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
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;

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
	reg32 |= (1 << 5) | (1 << 4) | (1 << 0);
	RCBA32(PRSTS) = reg32;

	/* FIXME: Does this even exist? */
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
		elog_add_event(ELOG_TYPE_RTC_RESET);
	}
	printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);

	cmos_init(rtc_failed);
}

static void mobile5_pm_init(struct device *dev)
{
	int i;

	printk(BIOS_DEBUG, "Mobile 5 PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);

	RCBA32(0x1d44) = 0x00000000;
	(void)RCBA32(0x1d44);
	RCBA32(0x1d48) = 0x00030000;
	(void)RCBA32(0x1d48);
	RCBA32(0x1e80) = 0x000c0801;
	(void)RCBA32(0x1e80);
	RCBA32(0x1e84) = 0x000200f0;
	(void)RCBA32(0x1e84);

	const u32 rcba2010[] = {
		/* 2010: */ 0x00188200, 0x14000016, 0xbc4abcb5, 0x00000000,
		/* 2020: */ 0xf0c9605b, 0x13683040, 0x04c8f16e, 0x09e90170
	};
	for (i = 0; i < ARRAY_SIZE(rcba2010); i++) {
		RCBA32(0x2010 + 4 * i) = rcba2010[i];
		RCBA32(0x2010 + 4 * i);
	}

	RCBA32(0x2100) = 0x00000000;
	(void)RCBA32(0x2100);
	RCBA32(0x2104) = 0x00000757;
	(void)RCBA32(0x2104);
	RCBA32(0x2108) = 0x00170001;
	(void)RCBA32(0x2108);

	RCBA32(0x211c) = 0x00000000;
	(void)RCBA32(0x211c);
	RCBA32(0x2120) = 0x00010000;
	(void)RCBA32(0x2120);

	RCBA32(0x21fc) = 0x00000000;
	(void)RCBA32(0x21fc);
	RCBA32(0x2200) = 0x20000044;
	(void)RCBA32(0x2200);
	RCBA32(0x2204) = 0x00000001;
	(void)RCBA32(0x2204);
	RCBA32(0x2208) = 0x00003457;
	(void)RCBA32(0x2208);

	const u32 rcba2210[] = {
		/* 2210 */ 0x00000000, 0x00000001, 0xa0fff210, 0x0000df00,
		/* 2220 */ 0x00e30880, 0x00000070, 0x00004000, 0x00000000,
		/* 2230 */ 0x00e30880, 0x00000070, 0x00004000, 0x00000000,
		/* 2240 */ 0x00002301, 0x36000000, 0x00010107, 0x00160000,
		/* 2250 */ 0x00001b01, 0x36000000, 0x00010107, 0x00160000,
		/* 2260 */ 0x00000601, 0x16000000, 0x00010107, 0x00160000,
		/* 2270 */ 0x00001c01, 0x16000000, 0x00010107, 0x00160000
	};

	for (i = 0; i < ARRAY_SIZE(rcba2210); i++) {
		RCBA32(0x2210 + 4 * i) = rcba2210[i];
		RCBA32(0x2210 + 4 * i);
	}

	const u32 rcba2300[] = {
		/* 2300: */ 0x00000000, 0x40000000, 0x4646827b, 0x6e803131,
		/* 2310: */ 0x32c77887, 0x00077733, 0x00007447, 0x00000040,
		/* 2320: */ 0xcccc0cfc, 0x0fbb0fff
	};

	for (i = 0; i < ARRAY_SIZE(rcba2300); i++) {
		RCBA32(0x2300 + 4 * i) = rcba2300[i];
		RCBA32(0x2300 + 4 * i);
	}

	RCBA32(0x37fc) = 0x00000000;
	(void)RCBA32(0x37fc);
	RCBA32(0x3dfc) = 0x00000000;
	(void)RCBA32(0x3dfc);
	RCBA32(0x3e7c) = 0xffffffff;
	(void)RCBA32(0x3e7c);
	RCBA32(0x3efc) = 0x00000000;
	(void)RCBA32(0x3efc);
	RCBA32(0x3f00) = 0x0000010b;
	(void)RCBA32(0x3f00);
}

static void enable_hpet(void)
{
	u32 reg32;

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(HPTC);
	reg32 |= (1 << 7); // HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(HPTC) = reg32;
	RCBA32(HPTC); /* Read back for it to work */

	write32((u32 *)0xfed00010, read32((u32 *)0xfed00010) | 1);
}

static void enable_clock_gating(struct device *dev)
{
	u32 reg32;
	u16 reg16;

	RCBA32_AND_OR(0x2234, ~0UL, 0xf);

	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 |= (1 << 2) | (1 << 11);
	pci_write_config16(dev, GEN_PMCON_1, reg16);

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
	/*
	 * Enable DMI ASPM in the PCH
	 */
	RCBA32_AND_OR(0x2304, ~(1 << 10), 0);
	RCBA32_OR(0x21a4, (1 << 11)|(1 << 10));
	RCBA32_OR(0x21a8, 0x3);
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
	mobile5_pm_init(dev);

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

	pch_set_acpi_mode();

	pch_fixups(dev);
}

static void pch_lpc_read_resources(struct device *dev)
{
	struct resource *res;
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;
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
	struct southbridge_intel_ibexpeak_config *chip = dev->chip_info;

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

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.write_acpi_tables	= acpi_write_hpet,
	.init			= lpc_init,
	.final			= lpc_final,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_IBEXPEAK_LPC_P55,
	PCI_DID_INTEL_IBEXPEAK_LPC_PM55,
	PCI_DID_INTEL_IBEXPEAK_LPC_H55,
	PCI_DID_INTEL_IBEXPEAK_LPC_QM57,
	PCI_DID_INTEL_IBEXPEAK_LPC_H57,
	PCI_DID_INTEL_IBEXPEAK_LPC_HM55,
	PCI_DID_INTEL_IBEXPEAK_LPC_Q57,
	PCI_DID_INTEL_IBEXPEAK_LPC_HM57,
	PCI_DID_INTEL_IBEXPEAK_LPC_QS57,
	PCI_DID_INTEL_IBEXPEAK_LPC_3400,
	PCI_DID_INTEL_IBEXPEAK_LPC_3420,
	PCI_DID_INTEL_IBEXPEAK_LPC_3450,
	0
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
