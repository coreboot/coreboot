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
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>
#include <southbridge/intel/common/hpet.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/spi.h>

#include "chip.h"
#include "i82801gx.h"

#define NMI_OFF	0

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void i82801gx_enable_ioapic(struct device *dev)
{
	register_new_ioapic_gsi0(VIO_APIC_VADDR);
}

static void i82801gx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
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
	const struct southbridge_intel_i82801gx_config *config = dev->chip_info;

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

		if (!is_enabled_pci(irq_dev))
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
	const struct southbridge_intel_i82801gx_config *config = dev->chip_info;
	u32 reg32 = 0;

	/* An array would be much nicer here, or some other method of doing this. */
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
	u16 reg16;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	const struct southbridge_intel_i82801gx_config *config = dev->chip_info;

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
	reg16 |= (1 << 2);	// CLKRUN_EN - Mobile/Ultra only
	reg16 |= (1 << 3);	// Speedstep Enable - Mobile/Ultra only
	reg16 |= (1 << 5);	// CPUSLP_EN Desktop only

	if (config->c4onc3_enable)
		reg16 |= (1 << 7);

	// another laptop wants this?
	// reg16 &= ~(1 << 10);	// BIOS_PCI_EXP_EN - Desktop/Mobile only
	reg16 |= (1 << 10);	// BIOS_PCI_EXP_EN - Desktop/Mobile only
	if (CONFIG(DEBUG_PERIODIC_SMI))
		reg16 |= (3 << 0); // Periodic SMI every 8s
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	// Set the board's GPI routing.
	i82801gx_gpi_routing(dev);

	write_pmbase32(GPE0_EN, config->gpe0_en);
	write_pmbase16(ALT_GP_SMI_EN, config->alt_gp_smi_en);

	/* Set up power management block and determine sleep mode */
	reg32 = read_pmbase32(PM1_CNT);

	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 1);	// enable C3->C0 transition on bus master
	reg32 |= (1 << 0);	// SCI_EN
	write_pmbase32(PM1_CNT, reg32);
}

static void i82801gx_configure_cstates(struct device *dev)
{
	// Enable Popup & Popdown
	pci_or_config8(dev, 0xa9, (1 << 4) | (1 << 3) | (1 << 2));

	// Set Deeper Sleep configuration to recommended values
	// Deeper Sleep to Stop CPU: 34-40us
	// Deeper Sleep to Sleep: 15us
	pci_update_config8(dev, 0xaa, 0xf0, (2 << 2) | (2 << 0));
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

static void i82801gx_set_acpi_mode(struct device *dev)
{
	if (!acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	} else {
		apm_control(APM_CNT_ACPI_ENABLE);
	}
}

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
	printk(BIOS_DEBUG, "i82801gx: %s\n", __func__);

	/* IO APIC initialization. */
	i82801gx_enable_ioapic(dev);

	i82801gx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801gx_pirq_init(dev);

	/* Setup power options. */
	i82801gx_power_options(dev);

	/* Configure Cx state registers */
	i82801gx_configure_cstates(dev);

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

	i82801gx_set_acpi_mode(dev);

	i82801gx_spi_init();

	i82801gx_fixups(dev);
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC_ADDR);

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

	if (!CONFIG(INTEL_CHIPSET_LOCKDOWN))
		return;

	if (CONFIG(BOOT_DEVICE_SPI_FLASH))
		spi_finalize_ops();

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
	post_code(POST_OS_BOOT);
}

static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}

static void southbridge_fill_ssdt(const struct device *device)
{
	intel_acpi_gen_def_acpi_pirq(device);
}

static struct device_operations device_ops = {
	.read_resources		= i82801gx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_dev_ops_pci,
	.final			= lpc_final,
};

static const unsigned short pci_device_ids[] = {
	0x27b0, /* 82801GH (ICH7 DH) */
	0x27b8, /* 82801GB/GR (ICH7/ICH7R) */
	0x27b9, /* 82801GBM/GU (ICH7-M/ICH7-U) */
	0x27bc, /* 82NM10 (NM10) */
	0x27bd, /* 82801GHM (ICH7-M DH) */
	0
};

static const struct pci_driver ich7_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.devices = pci_device_ids,
};
