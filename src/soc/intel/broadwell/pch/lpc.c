/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <option.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/rcba.h>
#include <soc/intel/broadwell/pch/chip.h>
#include <acpi/acpigen.h>
#include <southbridge/intel/common/rtc.h>
#include <southbridge/intel/lynxpoint/iobp.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>

static void pch_enable_ioapic(struct device *dev)
{
	/* Assign unique bus/dev/fn for I/O APIC */
	pci_write_config16(dev, LPC_IBDF,
		PCH_IOAPIC_PCI_BUS << 8 | PCH_IOAPIC_PCI_SLOT << 3);

	/* affirm full set of redirection table entries ("write once") */
	/* PCH-LP has 40 redirection entries */
	ioapic_set_max_vectors(VIO_APIC_VADDR, 40);

	register_new_ioapic_gsi0(VIO_APIC_VADDR);
}

static void enable_hpet(struct device *dev)
{
	size_t i;

	/* Assign unique bus/dev/fn for each HPET */
	for (i = 0; i < 8; ++i)
		pci_write_config16(dev, LPC_HnBDF(i),
			PCH_HPET_PCI_BUS << 8 | PCH_HPET_PCI_SLOT << 3 | i);
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

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!is_enabled_pci(irq_dev))
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

static void pch_power_options(struct device *dev)
{
	u16 reg16;
	const char *state;

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

	if (dev->chip_info) {
		const struct soc_intel_broadwell_pch_config *config = dev->chip_info;

		/* GPE setup based on device tree configuration */
		enable_all_gpe(config->gpe0_en_1, config->gpe0_en_2,
			       config->gpe0_en_3, config->gpe0_en_4);

		/* SMI setup based on device tree configuration */
		enable_alt_smi(config->alt_gp_smi_en);
	}
}

static void pch_misc_init(struct device *dev)
{
	u8 reg8;
	u32 reg32;

	/* Prepare sleep mode */
	reg32 = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	reg32 &= ~SLP_TYP;
	reg32 |= SCI_EN;
	outl(reg32, ACPI_BASE_ADDRESS + PM1_CNT);

	/* Set up NMI on errors */
	reg8 = inb(0x61);
	reg8 &= ~0xf0;		/* Higher nibble must be 0 */
	reg8 |= (1 << 2);	/* PCI SERR# disable for now */
	outb(reg8, 0x61);

	/* Disable NMI sources */
	reg8 = inb(0x70);
	reg8 |= (1 << 7);	/* Can't mask NMI from PCI-E and NMI_NOW */
	outb(reg8, 0x70);

	/* Indicate DRAM init done for MRC */
	pci_or_config8(dev, GEN_PMCON_2, 1 << 7);

	/* Enable BIOS updates outside of SMM */
	pci_and_config8(dev, BIOS_CNTL, ~(1 << 5));

	/* Clear status bits to prevent unexpected wake */
	RCBA32_OR(0x3310, 0x2f);

	RCBA32_AND_OR(0x3f02, ~0xf, 0);

	/* Enable PCIe Releaxed Order */
	RCBA32_OR(0x2314, (1 << 31) | (1 << 7)),
	RCBA32_OR(0x1114, (1 << 15) | (1 << 14)),

	/* Setup SERIRQ, enable continuous mode */
	reg8 = pci_read_config8(dev, SERIRQ_CNTL);
	reg8 |= 1 << 7;

	if (CONFIG(SERIRQ_CONTINUOUS_MODE))
		reg8 |= 1 << 6;

	pci_write_config8(dev, SERIRQ_CNTL, reg8);
}

/* Magic register settings for power management */
static void pch_pm_init_magic(struct device *dev)
{
	pci_write_config8(dev, 0xa9, 0x46);

	RCBA32_AND_OR(0x232c, ~1, 0);

	RCBA32_OR(0x1100, 0x0000c13f);

	RCBA32_AND_OR(0x2320, ~0x60, 0x10);

	RCBA32(0x3314) = 0x00012fff;

	RCBA32_AND_OR(0x3318, ~0x000f0330, 0x0dcf0400);

	RCBA32(0x3324) = 0x04000000;
	RCBA32(0x3368) = 0x00041400;
	RCBA32(0x3388) = 0x3f8ddbff;
	RCBA32(0x33ac) = 0x00007001;
	RCBA32(0x33b0) = 0x00181900;
	RCBA32(0x33c0) = 0x00060A00;
	RCBA32(0x33d0) = 0x06200840;
	RCBA32(0x3a28) = 0x01010101;
	RCBA32(0x3a2c) = 0x040c0404;
	RCBA32(0x3a9c) = 0x9000000a;
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
	RCBA32(0x3a20) = 0x0000040c;
	RCBA32(0x3a24) = 0x01010101;
	RCBA32(0x3a30) = 0x01010101;

	pci_update_config32(dev, 0xac, ~0x00200000, 0);

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
	RCBA32(0x3a84) = 0x00001005;

	RCBA32_OR(0x33d4, 0x2fff2fb1);
	RCBA32_OR(0x33c8, 0x00008000);
}

static void pch_enable_mphy(void)
{
	u32 gpio71_native = gpio_is_native(71);
	u32 data_and = 0xffffffff;
	u32 data_or = (1 << 14) | (1 << 13) | (1 << 12);

	if (gpio71_native) {
		data_or |= (1 << 0);
		if (pch_is_wpt()) {
			data_and &= ~((1 << 7) | (1 << 6) | (1 << 3));
			data_or |= (1 << 5) | (1 << 4);

			if (pch_is_wpt_ulx()) {
				/* Check if SATA and USB3 MPHY are enabled */
				u32 strap19 = pch_read_soft_strap(19);
				strap19 &= ((1 << 31) | (1 << 30));
				strap19 >>= 30;
				if (strap19 == 3) {
					data_or |= (1 << 3);
					printk(BIOS_DEBUG, "Enable ULX MPHY PG "
					       "control in single domain\n");
				} else if (strap19 == 0) {
					printk(BIOS_DEBUG, "Enable ULX MPHY PG "
					       "control in split domains\n");
				} else {
					printk(BIOS_DEBUG, "Invalid PCH Soft "
					       "Strap 19 configuration\n");
				}
			} else {
				data_or |= (1 << 3);
			}
		}
	}

	pch_iobp_update(0xCF000000, data_and, data_or);
}

static void pch_init_deep_sx(struct device *dev)
{
	const struct soc_intel_broadwell_pch_config *config = dev->chip_info;

	if (!config)
		return;

	if (config->deep_sx_enable_ac) {
		RCBA32_OR(DEEP_S3_POL, DEEP_S3_EN_AC);
		RCBA32_OR(DEEP_S5_POL, DEEP_S5_EN_AC);
	}

	if (config->deep_sx_enable_dc) {
		RCBA32_OR(DEEP_S3_POL, DEEP_S3_EN_DC);
		RCBA32_OR(DEEP_S5_POL, DEEP_S5_EN_DC);
	}

	if (config->deep_sx_enable_ac || config->deep_sx_enable_dc)
		RCBA32_OR(DEEP_SX_CONFIG,
			  DEEP_SX_WAKE_PIN_EN | DEEP_SX_GP27_PIN_EN);
}

/* Power Management init */
static void pch_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "PCH PM init\n");

	pch_init_deep_sx(dev);

	pch_enable_mphy();

	pch_pm_init_magic(dev);

	if (pch_is_wpt()) {
		RCBA32_OR(0x33e0, (1 << 4) | (1 << 1));
		RCBA32_OR(0x2b1c, (1 << 22) | (1 << 14) | (1 << 13));
		RCBA32(0x33e4) = 0x16bf0002;
		RCBA32_OR(0x33e4, 0x1);
	}

	pch_iobp_update(0xCA000000, ~0UL, 0x00000009);

	/* Set RCBA 0x2b1c[29]=1 if DSP disabled */
	if (RCBA32(FD) & PCH_DISABLE_ADSPD)
		RCBA32_OR(0x2b1c, (1 << 29));

}

static void pch_cg_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	struct device *igd_dev = pcidev_path_on_root(SA_DEVFN_IGD);

	/* DMI */
	RCBA32_OR(0x2234, 0xf);

	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~(1 << 10); /* Disable BIOS_PCI_EXP_EN for native PME */
	if (pch_is_wpt())
		reg16 &= ~(1 << 11);
	else
		reg16 |= (1 << 11);
	reg16 |= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 12);
	reg16 |= (1 << 2); // PCI CLKRUN# Enable
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	/*
	 * RCBA + 0x2614[27:25,14:13,10,8] = 101,11,1,1
	 * RCBA + 0x2614[23:16] = 0x20
	 * RCBA + 0x2614[30:28] = 0x0
	 * RCBA + 0x2614[26] = 1 (IF 0:2.0@0x08 >= 0x0b)
	 */
	RCBA32_AND_OR(0x2614, ~0x64ff0000, 0x0a206500);

	/* Check for 0:2.0@0x08 >= 0x0b */
	if (pch_is_wpt() || pci_read_config8(igd_dev, 0x8) >= 0x0b)
		RCBA32_OR(0x2614, (1 << 26));

	RCBA32_OR(0x900, 0x0000031f);

	reg32 = RCBA32(CG);
	if (RCBA32(0x3454) & (1 << 4))
		reg32 &= ~(1 << 29); // LPC Dynamic
	else
		reg32 |= (1 << 29); // LPC Dynamic
	reg32 |= (1 << 31); // LP LPC
	reg32 |= (1 << 30); // LP BLA
	if (RCBA32(0x3454) & (1 << 4))
		reg32 &= ~(1 << 29);
	else
		reg32 |= (1 << 29);
	reg32 |= (1 << 28); // GPIO Dynamic
	reg32 |= (1 << 27); // HPET Dynamic
	reg32 |= (1 << 26); // Generic Platform Event Clock
	if (RCBA32(BUC) & PCH_DISABLE_GBE)
		reg32 |= (1 << 23); // GbE Static
	if (RCBA32(FD) & PCH_DISABLE_HD_AUDIO)
		reg32 |= (1 << 21); // HDA Static
	reg32 |= (1 << 22); // HDA Dynamic
	RCBA32(CG) = reg32;

	/* PCH-LP LPC */
	if (pch_is_wpt())
		RCBA32_AND_OR(0x3434, ~0x1f, 0x17);
	else
		RCBA32_OR(0x3434, 0x7);

	/* SPI */
	RCBA32_OR(0x38c0, 0x3c07);

	pch_iobp_update(0xCE00C000, ~1UL, 0x00000000);
}

static void pch_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	}
}

static void lpc_init(struct device *dev)
{
	/* Legacy initialization */
	isa_dma_init();
	sb_rtc_init();
	pch_misc_init(dev);

	/* Interrupt configuration */
	pch_enable_ioapic(dev);
	pch_pirq_init(dev);
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
	enable_hpet(dev);

	/* Initialize power management */
	pch_power_options(dev);
	pch_pm_init(dev);
	pch_cg_init(dev);

	pch_set_acpi_mode();
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
	res->size = 4ull * GiB - default_decode_base;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* RCBA */
	if (default_decode_base > CONFIG_FIXED_RCBA_MMIO_BASE) {
		res = new_resource(dev, RCBA);
		res->base = CONFIG_FIXED_RCBA_MMIO_BASE;
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
	 * Check if the register is enabled. If so and the base exceeds the
	 * device's default claim range add the resource.
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
	pch_lpc_add_io_resource(dev, GPIO_BASE_ADDRESS,
				GPIO_BASE_SIZE, GPIO_BASE);

	/* PMBASE */
	pch_lpc_add_io_resource(dev, ACPI_BASE_ADDRESS, ACPI_BASE_SIZE, PMBASE);

	/* LPC Generic IO Decode range. */
	if (dev->chip_info) {
		const struct soc_intel_broadwell_pch_config *config = dev->chip_info;
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

static unsigned long broadwell_write_acpi_tables(const struct device *device,
						 unsigned long current,
						 struct acpi_rsdp *rsdp)
{
	if (CONFIG(SERIALIO_UART_CONSOLE)) {
		current = acpi_write_dbg2_pci_uart(rsdp, current,
			(CONFIG_UART_FOR_CONSOLE == 1) ?
				PCH_DEV_UART1 : PCH_DEV_UART0,
			ACPI_ACCESS_SIZE_DWORD_ACCESS);
	}
	return acpi_write_hpet(device, current, rsdp);
}

static struct device_operations device_ops = {
	.read_resources		= &pch_lpc_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.write_acpi_tables      = broadwell_write_acpi_tables,
	.init			= &lpc_init,
	.scan_bus		= &scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCH_LPT_LP_SAMPLE,
	PCH_LPT_LP_PREMIUM,
	PCH_LPT_LP_MAINSTREAM,
	PCH_LPT_LP_VALUE,
	PCH_WPT_HSW_U_SAMPLE,
	PCH_WPT_BDW_U_SAMPLE,
	PCH_WPT_BDW_U_PREMIUM,
	PCH_WPT_BDW_U_BASE,
	PCH_WPT_BDW_Y_SAMPLE,
	PCH_WPT_BDW_Y_PREMIUM,
	PCH_WPT_BDW_Y_BASE,
	PCH_WPT_BDW_H,
	0
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
