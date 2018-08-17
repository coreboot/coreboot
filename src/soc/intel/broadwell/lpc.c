/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <delay.h>
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
#include <cbmem.h>
#include <reg_script.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/iobp.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/nvs.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/rcba.h>
#include <soc/intel/broadwell/chip.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cpu/cpu.h>

static void pch_enable_ioapic(struct device *dev)
{
	u32 reg32;

	/* Assign unique bus/dev/fn for I/O APIC */
	pci_write_config16(dev, LPC_IBDF,
		PCH_IOAPIC_PCI_BUS << 8 | PCH_IOAPIC_PCI_SLOT << 3);

	set_ioapic_id(VIO_APIC_VADDR, 0x02);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read(VIO_APIC_VADDR, 0x01);

	/* PCH-LP has 39 redirection entries */
	reg32 &= ~0x00ff0000;
	reg32 |= 0x00270000;

	io_apic_write(VIO_APIC_VADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(VIO_APIC_VADDR, 0x03, 0x01);
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
	config_t *config = dev->chip_info;

	pci_write_config8(dev, PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, PIRQD_ROUT, config->pirqd_routing);

	pci_write_config8(dev, PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, PIRQH_ROUT, config->pirqh_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
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

static void pch_power_options(struct device *dev)
{
	u16 reg16;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	int pwr_on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;

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
	pci_write_config16(dev, GEN_PMCON_3, reg16);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* GPE setup based on device tree configuration */
	enable_all_gpe(config->gpe0_en_1, config->gpe0_en_2,
		       config->gpe0_en_3, config->gpe0_en_4);

	/* SMI setup based on device tree configuration */
	enable_alt_smi(config->alt_gp_smi_en);
}

static void pch_rtc_init(struct device *dev)
{
	cmos_init(rtc_failure());
}

static const struct reg_script pch_misc_init_script[] = {
	/* Setup SLP signal assertion, SLP_S4=4s, SLP_S3=50ms */
	REG_PCI_RMW16(GEN_PMCON_3, ~((3 << 4)|(1 << 10)),
		      (1 << 3)|(1 << 11)|(1 << 12)),
	/* Prepare sleep mode */
	REG_IO_RMW32(ACPI_BASE_ADDRESS + PM1_CNT, ~SLP_TYP, SCI_EN),
	/* Setup NMI on errors, disable SERR */
	REG_IO_RMW8(0x61, ~0xf0, (1 << 2)),
	/* Disable NMI sources */
	REG_IO_OR8(0x70, (1 << 7)),
	/* Indicate DRAM init done for MRC */
	REG_PCI_OR8(GEN_PMCON_2, (1 << 7)),
	/* Enable BIOS updates outside of SMM */
	REG_PCI_RMW8(0xdc, ~(1 << 5), 0),
	/* Clear status bits to prevent unexpected wake */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x3310, 0x0000002f),
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x3f02, ~0x0000000f, 0),
	/* Enable PCIe Releaxed Order */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x2314, (1 << 31) | (1 << 7)),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x1114, (1 << 15) | (1 << 14)),
	/* Setup SERIRQ, enable continuous mode */
	REG_PCI_OR8(SERIRQ_CNTL, (1 << 7) | (1 << 6)),
#if !IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)
	REG_PCI_RMW8(SERIRQ_CNTL, ~(1 << 6), 0),
#endif
	REG_SCRIPT_END
};

/* Magic register settings for power management */
static const struct reg_script pch_pm_init_script[] = {
	REG_PCI_WRITE8(0xa9, 0x46),
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x232c, ~1, 0),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x1100, 0x0000c13f),
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x2320, ~0x60, 0x10),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3314, 0x00012fff),
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x3318, ~0x000f0330, 0x0dcf0400),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3324, 0x04000000),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3368, 0x00041400),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3388, 0x3f8ddbff),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33ac, 0x00007001),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33b0, 0x00181900),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33c0, 0x00060A00),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33d0, 0x06200840),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a28, 0x01010101),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a2c, 0x040c0404),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a9c, 0x9000000a),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b1c, 0x03808033),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b34, 0x80000009),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3348, 0x022ddfff),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x334c, 0x00000001),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3358, 0x0001c000),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3380, 0x3f8ddbff),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3384, 0x0001c7e1),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x338c, 0x0001c7e1),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3398, 0x0001c000),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33a8, 0x00181900),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33dc, 0x00080000),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33e0, 0x00000001),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a20, 0x0000040c),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a24, 0x01010101),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a30, 0x01010101),
	REG_PCI_RMW32(0xac, ~0x00200000, 0),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x0410, 0x00000003),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x2618, 0x08000000),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x2300, 0x00000002),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x2600, 0x00000008),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x33b4, 0x00007001),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3350, 0x022ddfff),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3354, 0x00000001),
	/* Power Optimizer */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x33d4, 0x08000000),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x33c8, 0x00000080),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b10, 0x0000883c),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b14, 0x1e0a4616),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b24, 0x40000005),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x2b20, 0x0005db01),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a80, 0x05145005),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x3a84, 0x00001005),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x33d4, 0x2fff2fb1),
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + 0x33c8, 0x00008000),
	REG_SCRIPT_END
};

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
	config_t *config = dev->chip_info;

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

	reg_script_run_on_dev(dev, pch_pm_init_script);

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
	if (pch_is_wpt() || pci_read_config8(SA_DEV_IGD, 0x8) >= 0x0b)
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
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	if (!acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT);
		printk(BIOS_DEBUG, "done.\n");
	}
#endif /* CONFIG_HAVE_SMI_HANDLER */
}

static void lpc_init(struct device *dev)
{
	/* Legacy initialization */
	isa_dma_init();
	pch_rtc_init(dev);
	reg_script_run_on_dev(dev, pch_misc_init_script);

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
	res->size = 0 - default_decode_base;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* RCBA */
	if (default_decode_base > RCBA_BASE_ADDRESS) {
		res = new_resource(dev, RCBA);
		res->base = RCBA_BASE_ADDRESS;
		res->size = 16 * 1024;
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

static inline int pch_io_range_in_default(u16 base, u16 size)
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
	config_t *config = dev->chip_info;

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
	pch_lpc_add_gen_io_resources(dev, config->gen1_dec, LPC_GEN1_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen2_dec, LPC_GEN2_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen3_dec, LPC_GEN3_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen4_dec, LPC_GEN4_DEC);
}

static void pch_lpc_read_resources(struct device *dev)
{
	global_nvs_t *gnvs;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);

	/* Allocate ACPI NVS in CBMEM */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));
	if (!acpi_is_wakeup_s3() && gnvs)
		memset(gnvs, 0, sizeof(global_nvs_t));
}

static void southcluster_inject_dsdt(struct device *device)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));
		if (gnvs)
			memset(gnvs, 0, sizeof(*gnvs));
	}

	if (gnvs) {
		acpi_create_gnvs(gnvs);
		/* And tell SMI about it */
		smm_setup_structures(gnvs, NULL, NULL);

		/* Add it to DSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (u32) gnvs);
		acpigen_pop_len();
	}
}

static unsigned long broadwell_write_acpi_tables(struct device *device,
						 unsigned long current,
						 struct acpi_rsdp *rsdp)
{
	if (IS_ENABLED(CONFIG_INTEL_PCH_UART_CONSOLE))
		current = acpi_write_dbg2_pci_uart(rsdp, current,
			(CONFIG_INTEL_PCH_UART_CONSOLE_NUMBER == 1) ?
				PCH_DEV_UART1 : PCH_DEV_UART0,
			ACPI_ACCESS_SIZE_BYTE_ACCESS);
	return acpi_write_hpet(device, current, rsdp);
}

static struct device_operations device_ops = {
	.read_resources		= &pch_lpc_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.acpi_inject_dsdt_generator = southcluster_inject_dsdt,
	.write_acpi_tables      = broadwell_write_acpi_tables,
	.init			= &lpc_init,
	.scan_bus		= &scan_lpc_bus,
	.ops_pci		= &broadwell_pci_ops,
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
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
