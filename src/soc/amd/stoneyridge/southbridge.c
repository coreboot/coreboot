/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010-2017 Advanced Micro Devices, Inc.
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

#include <arch/io.h>
#include <arch/acpi.h>
#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <amd_pci_util.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/amd_pci_int_defs.h>
#include <fchec.h>
#include <delay.h>
#include <soc/pci_devs.h>

/*
 * Table of APIC register index and associated IRQ name. Using IDX_XXX_NAME
 * provides a visible association with the index, therefor helping
 * maintainability of table. If a new index/name is defined in
 * amd_pci_int_defs.h, just add the pair at the end of this table.
 * Order is not important.
 */
const static struct irq_idx_name irq_association[] = {
	{ PIRQ_A,	"INTA#\t" },
	{ PIRQ_B,	"INTB#\t" },
	{ PIRQ_C,	"INTC#\t" },
	{ PIRQ_D,	"INTD#\t" },
	{ PIRQ_E,	"INTE#\t" },
	{ PIRQ_F,	"INTF#\t" },
	{ PIRQ_G,	"INTG#\t" },
	{ PIRQ_H,	"INTH#\t" },
	{ PIRQ_MISC,	"Misc\t" },
	{ PIRQ_MISC0,	"Misc0\t" },
	{ PIRQ_MISC1,	"Misc1\t" },
	{ PIRQ_MISC2,	"Misc2\t" },
	{ PIRQ_SIRQA,	"Ser IRQ INTA" },
	{ PIRQ_SIRQB,	"Ser IRQ INTB" },
	{ PIRQ_SIRQC,	"Ser IRQ INTC" },
	{ PIRQ_SIRQD,	"Ser IRQ INTD" },
	{ PIRQ_SCI,	"SCI\t" },
	{ PIRQ_SMBUS,	"SMBUS\t" },
	{ PIRQ_ASF,	"ASF\t" },
	{ PIRQ_HDA,	"HDA\t" },
	{ PIRQ_FC,	"FC\t\t" },
	{ PIRQ_PMON,	"PerMon\t" },
	{ PIRQ_SD,	"SD\t\t" },
	{ PIRQ_SDIO,	"SDIO\t" },
	{ PIRQ_IMC0,	"IMC INT0\t" },
	{ PIRQ_IMC1,	"IMC INT1\t" },
	{ PIRQ_IMC2,	"IMC INT2\t" },
	{ PIRQ_IMC3,	"IMC INT3\t" },
	{ PIRQ_IMC4,	"IMC INT4\t" },
	{ PIRQ_IMC5,	"IMC INT5\t" },
	{ PIRQ_EHCI,	"EHCI\t" },
	{ PIRQ_XHCI,	"XHCI\t" },
	{ PIRQ_SATA,	"SATA\t" },
	{ PIRQ_GPIO,	"GPIO\t" },
	{ PIRQ_I2C0,	"I2C0\t" },
	{ PIRQ_I2C1,	"I2C1\t" },
	{ PIRQ_I2C2,	"I2C2\t" },
	{ PIRQ_I2C3,	"I2C3\t" },
	{ PIRQ_UART0,	"UART0\t" },
	{ PIRQ_UART1,	"UART1\t" },
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
}

void configure_stoneyridge_uart(void)
{
	u8 byte, byte2;

	if (CONFIG_UART_FOR_CONSOLE < 0 || CONFIG_UART_FOR_CONSOLE > 1)
		return;

	/* Power on the UART and AMBA devices */
	byte = read8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG56
					+ CONFIG_UART_FOR_CONSOLE * 2);
	byte |= AOAC_PWR_ON_DEV;
	write8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG56
					+ CONFIG_UART_FOR_CONSOLE * 2, byte);

	byte = read8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG62);
	byte |= AOAC_PWR_ON_DEV;
	write8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG62, byte);

	/* Set the GPIO mux to UART */
	write8((void *)FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0);
	write8((void *)FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0);
	write8((void *)FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0);
	write8((void *)FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0);

	/* Wait for the UART and AMBA devices to indicate power and clock OK */
	do {
		udelay(100);
		byte = read8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG57
					+ CONFIG_UART_FOR_CONSOLE * 2);
		byte &= (A0AC_PWR_RST_STATE | AOAC_RST_CLK_OK_STATE);
		byte2 = read8((void *)ACPI_MMIO_BASE + AOAC_BASE
					+ FCH_AOAC_REG63);
		byte2 &= (A0AC_PWR_RST_STATE | AOAC_RST_CLK_OK_STATE);
	} while (!((byte == (A0AC_PWR_RST_STATE | AOAC_RST_CLK_OK_STATE)) &&
		   (byte2 == (A0AC_PWR_RST_STATE | AOAC_RST_CLK_OK_STATE))));

}

void sb_pci_port80(void)
{
	u8 byte;

	byte = pci_read_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DEC_EN_HIGH);
	byte &= ~DECODE_IO_PORT_ENABLE4_H; /* disable lpc port 80 */
	pci_write_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DEC_EN_HIGH, byte);
}

void sb_lpc_port80(void)
{
	u8 byte;

	/* Enable LPC controller */
	outb(PM_LPC_GATING, PM_INDEX);
	byte = inb(PM_DATA);
	byte |= PM_LPC_ENABLE;
	outb(PM_LPC_GATING, PM_INDEX);
	outb(byte, PM_DATA);

	/* Enable port 80 LPC decode in pci function 3 configuration space. */
	byte = pci_read_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DEC_EN_HIGH);
	byte |= DECODE_IO_PORT_ENABLE4_H; /* enable port 80 */
	pci_write_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DEC_EN_HIGH, byte);
}

void sb_lpc_decode(void)
{
	u32 tmp = 0;

	/* Enable I/O decode to LPC bus */
	tmp = DECODE_ENABLE_PARALLEL_PORT0 | DECODE_ENABLE_PARALLEL_PORT2
		| DECODE_ENABLE_PARALLEL_PORT4 | DECODE_ENABLE_SERIAL_PORT0
		| DECODE_ENABLE_SERIAL_PORT1 | DECODE_ENABLE_SERIAL_PORT2
		| DECODE_ENABLE_SERIAL_PORT3 | DECODE_ENABLE_SERIAL_PORT4
		| DECODE_ENABLE_SERIAL_PORT5 | DECODE_ENABLE_SERIAL_PORT6
		| DECODE_ENABLE_SERIAL_PORT7 | DECODE_ENABLE_AUDIO_PORT0
		| DECODE_ENABLE_AUDIO_PORT1 | DECODE_ENABLE_AUDIO_PORT2
		| DECODE_ENABLE_AUDIO_PORT3 | DECODE_ENABLE_MSS_PORT2
		| DECODE_ENABLE_MSS_PORT3 | DECODE_ENABLE_FDC_PORT0
		| DECODE_ENABLE_FDC_PORT1 | DECODE_ENABLE_GAME_PORT
		| DECODE_ENABLE_KBC_PORT | DECODE_ENABLE_ACPIUC_PORT
		| DECODE_ENABLE_ADLIB_PORT;

	pci_write_config32(SOC_LPC_DEV, LPC_IO_PORT_DECODE_ENABLE, tmp);
}

void sb_clk_output_48Mhz(void)
{
	u32 ctrl;

	/*
	 * Enable the X14M_25M_48M_OSC pin and leaving it at it's default so
	 * 48Mhz will be on ball AP13 (FT3b package)
	 */
	ctrl = read32((void *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40));

	/* clear the OSCOUT1_ClkOutputEnb to enable the 48 Mhz clock */
	ctrl &= ~FCH_MISC_REG40_OSCOUT1_EN;
	write32((void *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40), ctrl);
}

static uintptr_t sb_spibase(void)
{
	u32 base, enables;

	/* Make sure the base address is predictable */
	base = pci_read_config32(SOC_LPC_DEV, SPIROM_BASE_ADDRESS_REGISTER);
	enables = base & 0xf;
	base &= ~0x3f;

	if (!base) {
		base = SPI_BASE_ADDRESS;
		pci_write_config32(SOC_LPC_DEV, SPIROM_BASE_ADDRESS_REGISTER,
					base | enables | SPI_ROM_ENABLE);
		/* PCI_COMMAND_MEMORY is read-only and enabled. */
	}
	return (uintptr_t)base;
}

void sb_set_spi100(u16 norm, u16 fast, u16 alt, u16 tpm)
{
	uintptr_t base = sb_spibase();
	write16((void *)base + SPI100_SPEED_CONFIG,
				(norm << SPI_NORM_SPEED_NEW_SH) |
				(fast << SPI_FAST_SPEED_NEW_SH) |
				(alt << SPI_ALT_SPEED_NEW_SH) |
				(tpm << SPI_TPM_SPEED_NEW_SH));
	write16((void *)base + SPI100_ENABLE, SPI_USE_SPI100);
}

void sb_disable_4dw_burst(void)
{
	uintptr_t base = sb_spibase();
	write16((void *)base + SPI100_HOST_PREF_CONFIG,
			read16((void *)base + SPI100_HOST_PREF_CONFIG)
					& ~SPI_RD4DW_EN_HOST);
}

void sb_set_readspeed(u16 norm, u16 fast)
{
	uintptr_t base = sb_spibase();
	write16((void *)base + SPI_CNTRL1, (read16((void *)base + SPI_CNTRL1)
					& ~SPI_CNTRL1_SPEED_MASK)
					| (norm << SPI_NORM_SPEED_SH)
					| (fast << SPI_FAST_SPEED_SH));
}

void sb_read_mode(u32 mode)
{
	uintptr_t base = sb_spibase();
	write32((void *)base + SPI_CNTRL0,
			(read32((void *)base + SPI_CNTRL0)
					& ~SPI_READ_MODE_MASK) | mode);
}

void sb_tpm_decode_spi(void)
{
	u32 spibase = pci_read_config32(SOC_LPC_DEV,
					SPIROM_BASE_ADDRESS_REGISTER);
	pci_write_config32(SOC_LPC_DEV, SPIROM_BASE_ADDRESS_REGISTER, spibase
					| ROUTE_TPM_2_SPI);
}

/*
 * Enable 4MB (LPC) ROM access at 0xFFC00000 - 0xFFFFFFFF.
 *
 * Hardware should enable LPC ROM by pin straps. This function does not
 * handle the theoretically possible PCI ROM, FWH, or SPI ROM configurations.
 *
 * The southbridge power-on default is to map 512K ROM space.
 *
 */
void sb_enable_rom(void)
{
	u8 reg8;

	/*
	 * Decode variable LPC ROM address ranges 1 and 2.
	 * Bits 3-4 are not defined in any publicly available datasheet
	 */
	reg8 = pci_read_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DECODE_ENABLE);
	reg8 |= (1 << 3) | (1 << 4);
	pci_write_config8(SOC_LPC_DEV, LPC_IO_OR_MEM_DECODE_ENABLE, reg8);

	/*
	 * LPC ROM address range 1:
	 * Enable LPC ROM range mirroring start at 0x000e(0000).
	 */
	pci_write_config16(SOC_LPC_DEV, ROM_ADDRESS_RANGE1_START, 0x000e);

	/* Enable LPC ROM range mirroring end at 0x000f(ffff). */
	pci_write_config16(SOC_LPC_DEV, ROM_ADDRESS_RANGE1_END, 0x000f);

	/*
	 * LPC ROM address range 2:
	 *
	 * Enable LPC ROM range start at:
	 * 0xfff8(0000): 512KB
	 * 0xfff0(0000): 1MB
	 * 0xffe0(0000): 2MB
	 * 0xffc0(0000): 4MB
	 */
	pci_write_config16(SOC_LPC_DEV, ROM_ADDRESS_RANGE2_START, 0x10000
					- (CONFIG_COREBOOT_ROMSIZE_KB >> 6));

	/* Enable LPC ROM range end at 0xffff(ffff). */
	pci_write_config16(SOC_LPC_DEV, ROM_ADDRESS_RANGE2_END, 0xffff);
}

void bootblock_fch_early_init(void)
{
	sb_enable_rom();
	sb_lpc_port80();
	sb_lpc_decode();
}

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos)
{
	int i;
	printk(BIOS_DEBUG, "Writing %x of size %d to nvram pos: %d\n",
					dword, size, nvram_pos);

	for (i = 0; i < size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >> (8 * i)) & 0xff, BIOSRAM_DATA);
		nvram_pos++;
	}

	return nvram_pos;
}

int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos)
{
	u32 data = *old_dword;
	int i;
	for (i = 0; i < size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		data &= ~(0xff << (i * 8));
		data |= inb(BIOSRAM_DATA) << (i * 8);
		nvram_pos++;
	}
	*old_dword = data;
	printk(BIOS_DEBUG, "Loading %x of size %d to nvram pos:%d\n",
					*old_dword, size, nvram_pos-size);
	return nvram_pos;
}

int acpi_get_sleep_type(void)
{
	return acpi_sleep_from_pm1(inw(pm_acpi_pm_cnt_blk()));
}

void sb_enable(device_t dev)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
}

static void sb_init_acpi_ports(void)
{
	u32 reg;

	/* We use some of these ports in SMM regardless of whether or not
	 * ACPI tables are generated. Enable these ports indiscriminately.
	 */

	pm_write16(PM_EVT_BLK, ACPI_PM_EVT_BLK);
	pm_write16(PM1_CNT_BLK, ACPI_PM1_CNT_BLK);
	pm_write16(PM_TMR_BLK, ACPI_PM_TMR_BLK);
	pm_write16(PM_GPE0_BLK, ACPI_GPE0_BLK);
	/* CpuControl is in \_PR.CP00, 6 bytes */
	pm_write16(PM_CPU_CTRL, ACPI_CPU_CONTROL);

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		/* APMC - SMI Command Port */
		pm_write16(PM_ACPI_SMI_CMD, APM_CNT);
		configure_smi(SMITYPE_SMI_CMD_PORT, SMI_MODE_SMI);

		/* SMI on SlpTyp requires sending SMI before completion
		 * response of the I/O write.  The BKDG also specifies
		 * clearing ForceStpClkRetry for SMI trapping.
		 */
		reg = pm_read32(PM_PCI_CTRL);
		reg |= FORCE_SLPSTATE_RETRY;
		reg &= ~FORCE_STPCLK_RETRY;
		pm_write32(PM_PCI_CTRL, reg);

		/* Disable SlpTyp feature */
		reg = pm_read8(PM_RST_CTRL1);
		reg &= ~SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, reg);

		configure_smi(SMITYPE_SLP_TYP, SMI_MODE_SMI);
	} else {
		pm_write16(PM_ACPI_SMI_CMD, 0);
	}

	/* Decode ACPI registers and enable standard features */
	pm_write8(PM_ACPI_CONF, PM_ACPI_DECODE_STD |
				PM_ACPI_GLOBAL_EN |
				PM_ACPI_RTC_EN_EN |
				PM_ACPI_TIMER_EN_EN);
}

void southbridge_init(void *chip_info)
{
	sb_init_acpi_ports();
}

void southbridge_final(void *chip_info)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_IMC_FWM)) {
		agesawrapper_fchecfancontrolservice();
		if (!IS_ENABLED(CONFIG_ACPI_ENABLE_THERMAL_ZONE))
			enable_imc_thermal_zone();
	}
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);
