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
#include "chip.h"
#include "i82801jx.h"
#include <southbridge/intel/common/pciehp.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>

#define NMI_OFF	0

typedef struct southbridge_intel_i82801jx_config config_t;

static void i82801jx_enable_apic(struct device *dev)
{
	/* Enable IOAPIC. Keep APIC Range Select at zero. */
	RCBA8(0x31ff) = 0x03;
	/* We have to read 0x31ff back if bit0 changed. */
	RCBA8(0x31ff);

	/* Lock maximum redirection entries (MRE), R/WO register. */
	ioapic_lock_max_vectors(VIO_APIC_VADDR);

	register_new_ioapic_gsi0(VIO_APIC_VADDR);
}

static void i82801jx_enable_serial_irqs(struct device *dev)
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

static void i82801jx_pirq_init(struct device *dev)
{
	struct device *irq_dev;

	/* Interrupt 11 is not used by legacy devices and so can always be used
	 * for PCI interrupts. Full legacy IRQ routing is complicated and hard
	 * to get right. Fortunately all modern OS use MSI and so it's not that
	 * big of an issue anyway. Still we have to provide a reasonable
	 * default. Using interrupt 11 for it everywhere is a working default.
	 * ACPI-aware OS can move it to any interrupt and others will just leave
	 * them at default.
	 */
	const u8 pirq_routing = 11;

	pci_write_config8(dev, D31F0_PIRQA_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQB_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQC_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQD_ROUT, pirq_routing);

	pci_write_config8(dev, D31F0_PIRQE_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQF_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQG_ROUT, pirq_routing);
	pci_write_config8(dev, D31F0_PIRQH_ROUT, pirq_routing);

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0;

		if (!is_enabled_pci(irq_dev))
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		if (int_pin == 0)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, pirq_routing);
	}
}

static void i82801jx_gpi_routing(struct device *dev)
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

static void i82801jx_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	/* BIOS must program... */
	pci_or_config32(dev, 0xac, (1 << 30) | (3 << 8));

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use MAINBOARD_POWER_ON.
	 */
	const unsigned int pwr_on = get_uint_option("power_on_after_fail", MAINBOARD_POWER_ON);

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
	reg8 &= ~(1 << 3);	/* minimum assertion is 1 to 2 RTCCLK */

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
	if (CONFIG(DEBUG_PERIODIC_SMI))
		reg16 |= (3 << 0); // Periodic SMI every 8s
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
	i82801jx_gpi_routing(dev);

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

static void i82801jx_rtc_init(struct device *dev)
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

static void i82801jx_set_acpi_mode(struct device *dev)
{
	if (!acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	} else {
		apm_control(APM_CNT_ACPI_ENABLE);
	}
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "i82801jx: %s\n", __func__);

	/* IO APIC initialization. */
	i82801jx_enable_apic(dev);

	i82801jx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801jx_pirq_init(dev);

	/* Setup power options. */
	i82801jx_power_options(dev);

	/* Initialize the real time clock. */
	i82801jx_rtc_init(dev);

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

	i82801jx_set_acpi_mode(dev);
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

static void i82801jx_lpc_read_resources(struct device *dev)
{
	int i, io_index = 0;
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
	 * (0x0400-0x041f)....SMBus (CONFIG_FIXED_SMBUS_IO_BASE, during raminit)
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
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0xff000000;
	res->size = 0x01000000; /* 16 MB for flash */
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

static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}

static void southbridge_fill_ssdt(const struct device *device)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	config_t *chip = dev->chip_info;

	intel_acpi_pcie_hotplug_generator(chip->pcie_hotplug_map, 8);
	intel_acpi_gen_def_acpi_pirq(device);
}

static struct device_operations device_ops = {
	.read_resources		= i82801jx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x3a10, /* ICH10R Eng. Sample */
	0x3a14, /* ICH10DO */
	0x3a16, /* ICH10R */
	0x3a18, /* ICH10 */
	0x3a1a, /* ICH10D */
	0x3a1e, /* ICH10 Eng. Sample */
	0
};

static const struct pci_driver ich10_lpc __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
