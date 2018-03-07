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
#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <amdblocks/amd_pci_util.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/amd_pci_int_defs.h>
#include <fchec.h>
#include <delay.h>
#include <soc/pci_devs.h>
#include <agesa_headers.h>

static int is_sata_config(void)
{
	return !((CONFIG_STONEYRIDGE_SATA_MODE == SataNativeIde)
			|| (CONFIG_STONEYRIDGE_SATA_MODE == SataLegacyIde));
}

static inline int sb_sata_enable(void)
{
	/* True if IDE or AHCI. */
	return (CONFIG_STONEYRIDGE_SATA_MODE == SataNativeIde) ||
		(CONFIG_STONEYRIDGE_SATA_MODE == SataAhci);
}

static inline int sb_ide_enable(void)
{
	/* True if IDE or LEGACY IDE. */
	return (CONFIG_STONEYRIDGE_SATA_MODE == SataNativeIde) ||
		(CONFIG_STONEYRIDGE_SATA_MODE == SataLegacyIde);
}

void SetFchResetParams(FCH_RESET_INTERFACE *params)
{
	params->Xhci0Enable = IS_ENABLED(CONFIG_STONEYRIDGE_XHCI_ENABLE);
	params->SataEnable = sb_sata_enable();
	params->IdeEnable = sb_ide_enable();
}

void SetFchEnvParams(FCH_INTERFACE *params)
{
	params->AzaliaController = AzEnable;
	params->SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
	params->SataEnable = is_sata_config();
	params->IdeEnable = !params->SataEnable;
	params->SataIdeMode = (CONFIG_STONEYRIDGE_SATA_MODE == SataLegacyIde);
}

void SetFchMidParams(FCH_INTERFACE *params)
{
	SetFchEnvParams(params);
}

/*
 * Table of APIC register index and associated IRQ name. Using IDX_XXX_NAME
 * provides a visible association with the index, therefore helping
 * maintainability of table. If a new index/name is defined in
 * amd_pci_int_defs.h, just add the pair at the end of this table.
 * Order is not important.
 */
const static struct irq_idx_name irq_association[] = {
	{ PIRQ_A,	"INTA#" },
	{ PIRQ_B,	"INTB#" },
	{ PIRQ_C,	"INTC#" },
	{ PIRQ_D,	"INTD#" },
	{ PIRQ_E,	"INTE#" },
	{ PIRQ_F,	"INTF#" },
	{ PIRQ_G,	"INTG#" },
	{ PIRQ_H,	"INTH#" },
	{ PIRQ_MISC,	"Misc" },
	{ PIRQ_MISC0,	"Misc0" },
	{ PIRQ_MISC1,	"Misc1" },
	{ PIRQ_MISC2,	"Misc2" },
	{ PIRQ_SIRQA,	"Ser IRQ INTA" },
	{ PIRQ_SIRQB,	"Ser IRQ INTB" },
	{ PIRQ_SIRQC,	"Ser IRQ INTC" },
	{ PIRQ_SIRQD,	"Ser IRQ INTD" },
	{ PIRQ_SCI,	"SCI" },
	{ PIRQ_SMBUS,	"SMBUS" },
	{ PIRQ_ASF,	"ASF" },
	{ PIRQ_HDA,	"HDA" },
	{ PIRQ_FC,	"FC" },
	{ PIRQ_PMON,	"PerMon" },
	{ PIRQ_SD,	"SD" },
	{ PIRQ_SDIO,	"SDIOt" },
	{ PIRQ_IMC0,	"IMC INT0" },
	{ PIRQ_IMC1,	"IMC INT1" },
	{ PIRQ_IMC2,	"IMC INT2" },
	{ PIRQ_IMC3,	"IMC INT3" },
	{ PIRQ_IMC4,	"IMC INT4" },
	{ PIRQ_IMC5,	"IMC INT5" },
	{ PIRQ_EHCI,	"EHCI" },
	{ PIRQ_XHCI,	"XHCI" },
	{ PIRQ_SATA,	"SATA" },
	{ PIRQ_GPIO,	"GPIO" },
	{ PIRQ_I2C0,	"I2C0" },
	{ PIRQ_I2C1,	"I2C1" },
	{ PIRQ_I2C2,	"I2C2" },
	{ PIRQ_I2C3,	"I2C3" },
	{ PIRQ_UART0,	"UART0" },
	{ PIRQ_UART1,	"UART1" },
};

/*
 * Structure to simplify code obtaining the total of used wide IO
 * registers and the size assigned to each.
 */
static struct wide_io_ioport_and_bits {
	uint32_t enable;
	uint16_t port;
	uint8_t alt;
} wio_io_en[TOTAL_WIDEIO_PORTS] = {
	{
		LPC_WIDEIO0_ENABLE,
		LPC_WIDEIO_GENERIC_PORT,
		LPC_ALT_WIDEIO0_ENABLE
	},
	{
		LPC_WIDEIO1_ENABLE,
		LPC_WIDEIO1_GENERIC_PORT,
		LPC_ALT_WIDEIO1_ENABLE
	},
	{
		LPC_WIDEIO2_ENABLE,
		LPC_WIDEIO2_GENERIC_PORT,
		LPC_ALT_WIDEIO2_ENABLE
	}
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
}

void sb_program_gpios(const struct soc_amd_stoneyridge_gpio *gpio_ptr,
		      size_t size)
{
	void *tmp_ptr;
	uint8_t control, mux, index;

	for (index = 0; index < size; index++) {
		mux = gpio_ptr[index].function;
		control = gpio_ptr[index].control;
		tmp_ptr = (void *)(gpio_ptr[index].gpio + AMD_GPIO_MUX);
		write8(tmp_ptr, mux & AMD_GPIO_MUX_MASK);

		/*
		 * Get the address of AMD_GPIO_CONTROL (dword) relative
		 * to the desired pin and program bits 16-23.
		 */
		tmp_ptr = (void *)(gpio_ptr[index].gpio * sizeof(uint32_t) +
					AMD_GPIO_CONTROL + 2);
		write8(tmp_ptr, control);
	}
}

/**
 * @brief Find the size of a particular wide IO
 *
 * @param index = index of desired wide IO
 *
 * @return size of desired wide IO
 */
uint16_t sb_wideio_size(int index)
{
	uint32_t enable_register;
	uint16_t size = 0;
	uint8_t alternate_register;

	if (index >= TOTAL_WIDEIO_PORTS)
		return size;
	enable_register = pci_read_config32(SOC_LPC_DEV,
				LPC_IO_OR_MEM_DECODE_ENABLE);
	alternate_register = pci_read_config8(SOC_LPC_DEV,
				LPC_ALT_WIDEIO_RANGE_ENABLE);
	if (enable_register & wio_io_en[index].enable)
		size = (alternate_register & wio_io_en[index].alt) ?
				16 : 512;
	return size;
}

/**
 * @brief Identify if any LPC wide IO is covering the IO range
 *
 * @param start = start of IO range
 * @param size = size of IO range
 *
 * @return Index of wide IO covering the range or error
 */
int sb_find_wideio_range(uint16_t start, uint16_t size)
{
	uint32_t enable_register;
	int i, index = WIDEIO_RANGE_ERROR;
	uint16_t end, current_size, start_wideio, end_wideio;

	end = start + size;
	enable_register = pci_read_config32(SOC_LPC_DEV,
					   LPC_IO_OR_MEM_DECODE_ENABLE);
	for (i = 0; i < TOTAL_WIDEIO_PORTS; i++) {
		current_size = sb_wideio_size(i);
		if (current_size == 0)
			continue;
		start_wideio = pci_read_config16(SOC_LPC_DEV,
						 wio_io_en[i].port);
		end_wideio = start_wideio + current_size;
		if ((start >= start_wideio) && (end <= end_wideio)) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * @brief Program a LPC wide IO to support an IO range
 *
 * @param start = start of range to be routed through wide IO
 * @param size = size of range to be routed through wide IO
 *
 * @return Index of wide IO register used or error
 */
int sb_set_wideio_range(uint16_t start, uint16_t size)
{
	int i, index = WIDEIO_RANGE_ERROR;
	uint32_t enable_register;
	uint8_t alternate_register;

	enable_register = pci_read_config32(SOC_LPC_DEV,
					   LPC_IO_OR_MEM_DECODE_ENABLE);
	alternate_register = pci_read_config8(SOC_LPC_DEV,
					      LPC_ALT_WIDEIO_RANGE_ENABLE);
	for (i = 0; i < TOTAL_WIDEIO_PORTS; i++) {
		if (enable_register & wio_io_en[i].enable)
			continue;
		index = i;
		pci_write_config16(SOC_LPC_DEV, wio_io_en[i].port, start);
		enable_register |= wio_io_en[i].enable;
		pci_write_config32(SOC_LPC_DEV, LPC_IO_OR_MEM_DECODE_ENABLE,
				   enable_register);
		if (size <= 16)
			alternate_register |= wio_io_en[i].alt;
		else
			alternate_register &= ~wio_io_en[i].alt;
		pci_write_config8(SOC_LPC_DEV,
				  LPC_ALT_WIDEIO_RANGE_ENABLE,
				  alternate_register);
		break;
	}
	return index;
}

static void power_on_aoac_device(int aoac_device_control_register)
{
	uint8_t byte;
	uint8_t *register_pointer = (uint8_t *)(uintptr_t)AOAC_MMIO_BASE
			+ aoac_device_control_register;

	/* Power on the UART and AMBA devices */
	byte = read8(register_pointer);
	byte |= FCH_AOAC_PWR_ON_DEV;
	write8(register_pointer, byte);
}

static bool is_aoac_device_enabled(int aoac_device_status_register)
{
	uint8_t byte;
	byte = read8((uint8_t *)(uintptr_t)AOAC_MMIO_BASE
			+ aoac_device_status_register);
	byte &= (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_RST_CLK_OK_STATE);
	if (byte == (FCH_AOAC_PWR_RST_STATE | FCH_AOAC_RST_CLK_OK_STATE))
		return true;
	else
		return false;
}

void configure_stoneyridge_uart(void)
{
	bool status;

	/* Power on the UART and AMBA devices */
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_UART0
			+ CONFIG_UART_FOR_CONSOLE * 2);
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_AMBA);

	/* Set the GPIO mux to UART */
	write8((void *)FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0);
	write8((void *)FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0);
	write8((void *)FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0);
	write8((void *)FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0);

	/* Wait for the UART and AMBA devices to indicate power and clock OK */
	do {
		udelay(100);
		status = is_aoac_device_enabled(FCH_AOAC_D3_STATE_UART0
					+ CONFIG_UART_FOR_CONSOLE * 2);
		status &= is_aoac_device_enabled(FCH_AOAC_D3_STATE_AMBA);
	} while (!status);
}

void configure_stoneyridge_i2c(void)
{
	bool status;

	/* Power on the I2C devices */
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_I2C0);
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_I2C1);
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_I2C2);
	power_on_aoac_device(FCH_AOAC_D3_CONTROL_I2C3);

	/* Wait for the I2C devices to indicate power and clock OK */
	do {
		udelay(100);
		status = is_aoac_device_enabled(FCH_AOAC_D3_STATE_I2C0);
		status &= is_aoac_device_enabled(FCH_AOAC_D3_STATE_I2C1);
		status &= is_aoac_device_enabled(FCH_AOAC_D3_STATE_I2C2);
		status &= is_aoac_device_enabled(FCH_AOAC_D3_STATE_I2C3);
	} while (!status);
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

void sb_acpi_mmio_decode(void)
{
	uint8_t byte;

	/* Enable ACPI MMIO range 0xfed80000 - 0xfed81fff */
	outb(PM_ISA_CONTROL, PM_INDEX);
	byte = inb(PM_DATA);
	byte |= MMIO_EN;
	outb(PM_ISA_CONTROL, PM_INDEX);
	outb(byte, PM_DATA);
}

void sb_clk_output_48Mhz(void)
{
	u32 ctrl;
	u32 *misc_clk_cntl_1_ptr = (u32 *)(uintptr_t)(MISC_MMIO_BASE
				+ MISC_MISC_CLK_CNTL_1);

	/*
	 * Enable the X14M_25M_48M_OSC pin and leaving it at it's default so
	 * 48Mhz will be on ball AP13 (FT3b package)
	 */
	ctrl = read32(misc_clk_cntl_1_ptr);

	/* clear the OSCOUT1_ClkOutputEnb to enable the 48 Mhz clock */
	ctrl &= ~OSCOUT1_CLK_OUTPUT_ENB;
	write32(misc_clk_cntl_1_ptr, ctrl);
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

/*
 * Enable FCH to decode TPM associated Memory and IO regions
 *
 * Enable decoding of TPM cycles defined in TPM 1.2 spec
 * Enable decoding of legacy TPM addresses: IO addresses 0x7f-
 * 0x7e and 0xef-0xee.
 * This function should be called if TPM is connected in any way to the FCH and
 * conforms to the regions decoded.
 * Absent any other routing configuration the TPM cycles will be claimed by the
 * LPC bus
 */
void sb_tpm_decode(void)
{
	u32 value;

	value = pci_read_config32(SOC_LPC_DEV, LPC_TRUSTED_PLATFORM_MODULE);
	value |= TPM_12_EN | TPM_LEGACY_EN;
	pci_write_config32(SOC_LPC_DEV, LPC_TRUSTED_PLATFORM_MODULE, value);
}

/*
 * Enable FCH to decode TPM associated Memory and IO regions to SPI
 *
 * This should be used if TPM is connected to SPI bus.
 * Assumes SPI address space is already configured via a call to sb_spibase().
 */
void sb_tpm_decode_spi(void)
{
	/* Enable TPM decoding to FCH */
	sb_tpm_decode();

	/* Route TPM accesses to SPI */
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
	uint8_t restored_power = PM_S5_AT_POWER_RECOVERY;

	if (IS_ENABLED(CONFIG_STONEYRIDGE_IMC_FWM)) {
		agesawrapper_fchecfancontrolservice();
		if (!IS_ENABLED(CONFIG_ACPI_ENABLE_THERMAL_ZONE))
			enable_imc_thermal_zone();
	}
	if (IS_ENABLED(CONFIG_MAINBOARD_POWER_RESTORE))
		restored_power = PM_RESTORE_S0_IF_PREV_S0;
	pm_write8(PM_RTC_SHADOW, restored_power);
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
