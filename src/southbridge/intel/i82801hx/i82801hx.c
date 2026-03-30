/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Intel ICH8-M (82801HBM) Southbridge - Main Ramstage Driver
 *
 * ICH8-M differences from ICH9:
 *   - 5 UHCI controllers (not 6)
 *   - 3 SATA ports (not 6)
 *   - LAN disable is FDSW_LAND (RCBA 0x3420 bit 0)
 *   - ICH8-M is always mobile
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <pc80/i8259.h>
#include <pc80/isa-dma.h>
#include <pc80/mc146818rtc.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/speedstep.h>
#include <option.h>
#include <southbridge/intel/common/hpet.h>
#include <southbridge/intel/common/pciehp.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba_pirq.h>
#include <static.h>
#include "chip.h"
#include "i82801hx.h"

/* ================================================================== */
/* chip_operations: enable_dev callback                                */
/* ================================================================== */

/*
 * i82801hx_hide_devfn() - Disable a PCI function via the FD register.
 *
 * Sets the corresponding bit in RCBA FD to hide the device,
 * based on its devfn. Modeled on bd82x6x pch_hide_devfn().
 */
static void i82801hx_hide_devfn(unsigned int devfn)
{
	switch (devfn) {
	case PCI_DEVFN(25, 0): /* GbE LAN */
		RCBA32_OR(RCBA_FDSW, FDSW_LAND);
		break;
	case PCI_DEVFN(31, 2): /* SATA #1 */
		RCBA32_OR(RCBA_FD, FD_SAD1);
		break;
	case PCI_DEVFN(31, 5): /* SATA #2 */
		RCBA32_OR(RCBA_FD, FD_SAD2);
		break;
	case PCI_DEVFN(31, 6): /* Thermal */
		RCBA32_OR(RCBA_FD, FD_TTD);
		break;
	case PCI_DEVFN(29, 0): /* UHCI #1 */
		RCBA32_OR(RCBA_FD, FD_U1D);
		break;
	case PCI_DEVFN(29, 1): /* UHCI #2 */
		RCBA32_OR(RCBA_FD, FD_U2D);
		break;
	case PCI_DEVFN(29, 2): /* UHCI #3 */
		RCBA32_OR(RCBA_FD, FD_U3D);
		break;
	case PCI_DEVFN(29, 7): /* EHCI #1 */
		RCBA32_OR(RCBA_FD, FD_EHCI1D);
		break;
	case PCI_DEVFN(26, 0): /* UHCI #4 */
		RCBA32_OR(RCBA_FD, FD_U4D);
		break;
	case PCI_DEVFN(26, 1): /* UHCI #5 */
		RCBA32_OR(RCBA_FD, FD_U5D);
		break;
	case PCI_DEVFN(26, 7): /* EHCI #2 */
		RCBA32_OR(RCBA_FD, FD_EHCI2D);
		break;
	case PCI_DEVFN(27, 0): /* HD Audio */
		RCBA32_OR(RCBA_FD, FD_HDAD);
		break;
	case PCI_DEVFN(28, 0): /* PCIe Root Port 1 */
		RCBA32_OR(RCBA_FD, FD_PE1D);
		break;
	case PCI_DEVFN(28, 1): /* PCIe Root Port 2 */
		RCBA32_OR(RCBA_FD, FD_PE2D);
		break;
	case PCI_DEVFN(28, 2): /* PCIe Root Port 3 */
		RCBA32_OR(RCBA_FD, FD_PE3D);
		break;
	case PCI_DEVFN(28, 3): /* PCIe Root Port 4 */
		RCBA32_OR(RCBA_FD, FD_PE4D);
		break;
	case PCI_DEVFN(28, 4): /* PCIe Root Port 5 */
		RCBA32_OR(RCBA_FD, FD_PE5D);
		break;
	case PCI_DEVFN(28, 5): /* PCIe Root Port 6 */
		RCBA32_OR(RCBA_FD, FD_PE6D);
		break;
	}
}

static void i82801hx_enable_device(struct device *dev)
{
	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));
		pci_and_config16(dev, PCI_COMMAND,
				 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));
		i82801hx_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

/* ================================================================== */
/* Chipset Init Registers (CIR)                                        */
/* ================================================================== */

static void i82801hx_early_settings(void)
{
	/*
	 * Vendor BIOS 2B_0.rom @ 0xd204 programs these RCBA/CIR bits as a
	 * table of 16-bit masked writes.  ICH9 coreboot used CIR8=2 and set
	 * both CIR10 bits 16/17; the X61 ICH8-M vendor code uses CIR8=1 and
	 * explicitly clears CIR10 bit 17.  Match the board firmware here.
	 */
	RCBA32_OR(RCBA_GCS, (1 << 6) | (1 << 5));
	RCBA32_AND_OR(RCBA_CIR8, ~(0x3 << 0), 0x1 << 0);
	RCBA32_AND_OR(RCBA_CIR9, ~(0x3 << 26), 0x2 << 26);
	RCBA32_AND_OR(RCBA_CIR7, ~(0xf << 16), 0x5 << 16);
	RCBA32_AND_OR(RCBA_CIR13, ~(0xf << 16), 0x5 << 16);
	RCBA32_OR(RCBA_CIR5, (1 << 0));
	RCBA32_AND_OR(RCBA_CIR10, ~(1 << 17), 0);
}

/* ================================================================== */
/* chip_operations: init callback                                      */
/* ================================================================== */

static void i82801hx_init(void *chip_info)
{
	printk(BIOS_DEBUG, "Initializing i82801hx southbridge...\n");

	/* Intel-required chipset init registers. */
	i82801hx_early_settings();

	/* PCIe root port setup. */
	i82801hx_pcie_init();

	/* Reset watchdog timer. */
#if !CONFIG(HAVE_SMI_HANDLER)
	outw(0x0008, DEFAULT_TCOBASE + 0x12); /* Set higher timer value. */
#endif
	outw(0x0000, DEFAULT_TCOBASE + 0x00); /* Update timer. */
}

/* ================================================================== */
/* LPC device_operations: .init callback                               */
/* ================================================================== */

/*
 * Wrappers for functions from lpc.c and lpc_late.c that need a
 * struct device * context (for chip_info access, etc.)
 */

static void lpc_enable_apic(struct device *dev)
{
	/* Enable IOAPIC via RCBA OIC register. */
	i82801hx_enable_ioapic();

	/* Lock maximum redirection entries (MRE), R/WO register. */
	ioapic_lock_max_vectors(IO_APIC_ADDR);

	/* Register the IOAPIC with the coreboot device model. */
	register_new_ioapic_gsi0(IO_APIC_ADDR);
}

static void lpc_enable_serial_irqs(struct device *dev)
{
	/*
	 * SERIRQ: bit 7 = enable, bit 6 = continuous mode, 4 clocks as start frame pulse width
	 */
	pci_write_config8(dev, D31F0_SERIRQ_CNTL, (1 << 7) | (1 << 6) | (0 << 0));
}

static void lpc_pirq_init(struct device *dev)
{
	struct device *irq_dev;
	struct southbridge_intel_i82801hx_config *config = dev->chip_info;

	pci_write_config8(dev, D31F0_PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, D31F0_PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, D31F0_PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, D31F0_PIRQD_ROUT, config->pirqd_routing);
	pci_write_config8(dev, D31F0_PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, D31F0_PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, D31F0_PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, D31F0_PIRQH_ROUT, config->pirqh_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!is_enabled_pci(irq_dev))
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
			int_line = config->pirqa_routing;
			break;
		case 2: /* INTB# */
			int_line = config->pirqb_routing;
			break;
		case 3: /* INTC# */
			int_line = config->pirqc_routing;
			break;
		case 4: /* INTD# */
			int_line = config->pirqd_routing;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void lpc_gpi_routing(struct device *dev)
{
	struct southbridge_intel_i82801hx_config *config = dev->chip_info;
	u32 reg32 = 0;

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

bool southbridge_support_c5(void)
{
	struct device *lpc_dev = __pci_0_1f_0;
	struct southbridge_intel_i82801hx_config *config = lpc_dev->chip_info;
	return config->c5_enable;
}

bool southbridge_support_c6(void)
{
	struct device *lpc_dev = __pci_0_1f_0;
	struct southbridge_intel_i82801hx_config *config = lpc_dev->chip_info;
	return config->c6_enable;
}

static void lpc_power_options(struct device *dev)
{
	u8 reg8;
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	struct southbridge_intel_i82801hx_config *config = dev->chip_info;

	/*
	 * Enable USB transient disconnect detect and global reset on writes
	 * to the CF9 reset control port.  The vendor BIOS sets D31:F0
	 * offset 0xad bits [1:0] to 3.
	 */
	pci_or_config32(dev, D31F0_PMIR,
			PMIR_USB_TRANSIENT_DISCONNECT | PMIR_CF9GR);

	/*
	 * Which state do we want to goto after g3 (power restored)?
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

	reg8 |= GEN_PMCON_3_SLP_S4_MAW; /* SLP_S4# minimum assertion width. */
	reg8 &= ~GEN_PMCON_3_SLP_S4_STRETCH;

	pci_write_config8(dev, D31F0_GEN_PMCON_3, reg8);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* Set up NMI on errors. */
	reg8 = inb(0x61);
	reg8 &= 0x0f;      /* Higher Nibble must be 0 */
	reg8 &= ~(1 << 3); /* IOCHK# NMI Enable */
	reg8 |= (1 << 2);  /* PCI SERR# Disable for now */
	outb(reg8, 0x61);

	reg8 = inb(0x74); /* Read from 0x74 as 0x70 is write only. */
	reg8 |= (1 << 7); /* Can't mask NMI from PCI-E and NMI_NOW */
	outb(reg8, 0x70);

	/* Enable CPU_SLP# and Intel Speedstep, set SMI# rate down */
	reg16 = pci_read_config16(dev, D31F0_GEN_PMCON_1);
	reg16 &= ~(3 << 0); /* SMI# rate 1 minute */
	reg16 |= (1 << 2);  /* CLKRUN_EN - Mobile/Ultra only */
	reg16 |= (1 << 3);  /* Speedstep Enable - Mobile/Ultra only */
	reg16 |= (1 << 5);  /* CPUSLP_EN Desktop only */

	if (config->c4onc3_enable)
		reg16 |= (1 << 7);

	reg16 |= (1 << 10); /* BIOS_PCI_EXP_EN - Desktop/Mobile only */
	if (CONFIG(DEBUG_PERIODIC_SMI))
		reg16 |= (3 << 0); /* Periodic SMI every 8s */
	if (southbridge_support_c5())
		reg16 |= (1 << 11); /* Enable C5, C6 and PMSYNC# */
	pci_write_config16(dev, D31F0_GEN_PMCON_1, reg16);

	/* Set exit timings for C5/C6. */
	if (southbridge_support_c5()) {
		reg8 = pci_read_config8(dev, D31F0_C5_EXIT_TIMING);
		reg8 &= ~((7 << 3) | (7 << 0));
		if (southbridge_support_c6())
			reg8 |= (5 << 3) | (3 << 0); /* 38-44us PMSYNC# to STPCLK#,
							95-102us DPRSTP# to STP_CPU# */
		else
			reg8 |= (0 << 3) | (1 << 0); /* 16-17us PMSYNC# to STPCLK#,
							34-40us DPRSTP# to STP_CPU# */
		pci_write_config8(dev, D31F0_C5_EXIT_TIMING, reg8);
	}

	/* Set the board's GPI routing. */
	lpc_gpi_routing(dev);

	pmbase = pci_read_config16(dev, D31F0_PMBASE) & 0xfffe;

	outl(config->gpe0_en, pmbase + GPE0_EN);
	outw(config->alt_gp_smi_en, pmbase + ALT_GP_SMI_EN);

	/* Set up power management block and determine sleep mode */
	reg16 = inw(pmbase + PM1_STS);
	outw(reg16, pmbase + PM1_STS); /* Clear status bits. */

	/* Set duty cycle for hardware throttling (defaults to 0x0: 50%). */
	reg32 = inl(pmbase + 0x10);
	reg32 &= ~(7 << 5);
	reg32 |= (config->throttle_duty & 7) << 5;
	outl(reg32, pmbase + 0x10);
}

static void lpc_configure_cstates(struct device *dev)
{
	/* Enable Popup & Popdown */
	pci_or_config8(dev, D31F0_C5_ENABLE, (1 << 4) | (1 << 3) | (1 << 2));

	/* Set Deeper Sleep configuration to recommended values */
	pci_update_config8(dev, D31F0_C4TIMING_CNT, ~0x0f, (2 << 2) | (2 << 0));
}

static void lpc_rtc_init(struct device *dev)
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

static void enable_clock_gating(void)
{
	u32 reg32;

	/* Enable DMI dynamic clock gating. */
	RCBA32(RCBA_DMIC) |= 3;

	/* Enable Clock Gating for most devices. */
	reg32 = RCBA32(RCBA_CG);
	reg32 |= (1 << 31); /* LPC dynamic clock gating */
	/* USB UHCI dynamic clock gating: */
	reg32 |= (1 << 29) | (1 << 28);
	/* SATA dynamic clock gating [0-3]: */
	reg32 |= (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24);
	reg32 |= (1 << 23);  /* LAN static clock gating (if LAN disabled) */
	reg32 |= (1 << 22);  /* HD audio dynamic clock gating */
	reg32 &= ~(1 << 21); /* No HD audio static clock gating */
	reg32 &= ~(1 << 20); /* No USB EHCI static clock gating */
	reg32 |= (1 << 19);  /* USB EHCI dynamic clock gating */
	/* More SATA dynamic clock gating [4-5]: */
	reg32 |= (1 << 18) | (1 << 17);
	reg32 |= (1 << 16); /* PCI dynamic clock gating */
	/* PCIe, DMI dynamic clock gating: */
	reg32 |= (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1);
	reg32 |= (1 << 0); /* PCIe root port static clock gating */
	RCBA32(RCBA_CG) = reg32;

	/* Enable SPI dynamic clock gating. */
	RCBA32(0x38c0) |= 7;
}

static void i82801hx_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3())
		apm_control(APM_CNT_ACPI_DISABLE);
	else
		apm_control(APM_CNT_ACPI_ENABLE);
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "i82801hx: %s\n", __func__);

	/* IO APIC initialization. */
	lpc_enable_apic(dev);

	lpc_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	lpc_pirq_init(dev);

	/* Setup power options. */
	lpc_power_options(dev);

	/* Configure Cx state registers (ICH8-M is always mobile). */
	lpc_configure_cstates(dev);

	/* Initialize the real time clock. */
	lpc_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Enable HPET (common: RCBA_HPTC enable + start counter).
	 * The vendor BIOS also sets bit 2 of RCBA_RC (0x3400) at this point. */
	enable_hpet();
	RCBA8(RCBA_RC) |= (1 << 2);

	/* Initialize Clock Gating */
	enable_clock_gating();

	setup_i8259();

	/* The OS should do this? */
	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	i82801hx_set_acpi_mode();
}

/* ================================================================== */
/* LPC device_operations: .final callback                              */
/* ================================================================== */

static void lpc_final(struct device *dev)
{
	/* Lock the Function Disable register. */
	RCBA32_OR(RCBA_FDSW, (1 << 7));

	/* Lock UHCI remap register (write current value back = R/WO lock). */
	RCBA32(RCBA_MAP) = RCBA32(RCBA_MAP);

	if (CONFIG(INTEL_CHIPSET_LOCKDOWN) || acpi_is_wakeup_s3())
		apm_control(APM_CNT_FINALIZE);
}

/* ================================================================== */
/* LPC device_operations: .read_resources callback                     */
/* ================================================================== */

static void i82801hx_lpc_read_resources(struct device *dev)
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
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff000000;
	res->size = 0x01000000; /* 16 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

/* ================================================================== */
/* ACPI support                                                        */
/* ================================================================== */

static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}

static void southbridge_fill_ssdt(const struct device *device)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	struct southbridge_intel_i82801hx_config *chip = dev->chip_info;

	intel_acpi_pcie_hotplug_generator(chip->pcie_hotplug_map, 6);
	intel_acpi_gen_def_acpi_pirq(device);
}

/* ================================================================== */
/* LPC PCI device driver registration                                  */
/* ================================================================== */

static struct device_operations device_ops = {
	.read_resources		= i82801hx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.write_acpi_tables	= acpi_write_hpet,
	.acpi_fill_ssdt		= southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.init			= lpc_init,
	.final			= lpc_final,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_82801HBM_LPC,
	PCI_DID_INTEL_82801HEM_LPC,
	0
};

static const struct pci_driver ich8_lpc __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};

/* ================================================================== */
/* chip_operations                                                     */
/* ================================================================== */

struct chip_operations southbridge_intel_i82801hx_ops = {
	.name		= "Intel ICH8-M (82801HBM) Series Southbridge",
	.enable_dev	= i82801hx_enable_device,
	.init		= i82801hx_init,
};
