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
#include <elog.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/agesawrapper.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/amd_pci_int_defs.h>
#include <delay.h>
#include <soc/pci_devs.h>
#include <agesa_headers.h>
#include <soc/nvs.h>
#include <reset.h>

/*
 * Table of devices that need their AOAC registers enabled and waited
 * upon (usually about .55 milliseconds). Instead of individual delays
 * waiting for each device to become available, a single delay will be
 * executed.
 */
const static struct stoneyridge_aoac aoac_devs[] = {
	{ (FCH_AOAC_D3_CONTROL_UART0 + CONFIG_UART_FOR_CONSOLE * 2),
		(FCH_AOAC_D3_STATE_UART0 + CONFIG_UART_FOR_CONSOLE * 2) },
	{ FCH_AOAC_D3_CONTROL_AMBA, FCH_AOAC_D3_STATE_AMBA },
	{ FCH_AOAC_D3_CONTROL_I2C0, FCH_AOAC_D3_STATE_I2C0 },
	{ FCH_AOAC_D3_CONTROL_I2C1, FCH_AOAC_D3_STATE_I2C1 },
	{ FCH_AOAC_D3_CONTROL_I2C2, FCH_AOAC_D3_STATE_I2C2 },
	{ FCH_AOAC_D3_CONTROL_I2C3, FCH_AOAC_D3_STATE_I2C3 }
};

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
	const struct device *dev = dev_find_slot(0, SATA_DEVFN);
	params->Xhci0Enable = IS_ENABLED(CONFIG_STONEYRIDGE_XHCI_ENABLE);
	if (dev && dev->enabled) {
		params->SataEnable = sb_sata_enable();
		params->IdeEnable = sb_ide_enable();
	} else {
		params->SataEnable = FALSE;
		params->IdeEnable = FALSE;
	}
}

void SetFchEnvParams(FCH_INTERFACE *params)
{
	const struct device *dev = dev_find_slot(0, SATA_DEVFN);
	params->AzaliaController = AzEnable;
	params->SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
	if (dev && dev->enabled) {
		params->SataEnable = is_sata_config();
		params->IdeEnable = !params->SataEnable;
		params->SataIdeMode = (CONFIG_STONEYRIDGE_SATA_MODE ==
					SataLegacyIde);
	} else {
		params->SataEnable = FALSE;
		params->IdeEnable = FALSE;
		params->SataIdeMode = FALSE;
	}
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

void enable_aoac_devices(void)
{
	bool status;
	int i;

	for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
		power_on_aoac_device(aoac_devs[i].enable);

	/* Wait for AOAC devices to indicate power and clock OK */
	do {
		udelay(100);
		status = true;
		for (i = 0; i < ARRAY_SIZE(aoac_devs); i++)
			status &= is_aoac_device_enabled(aoac_devs[i].status);
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

static void sb_enable_cf9_io(void)
{
	uint32_t reg = pm_read32(PM_DECODE_EN);

	pm_write32(PM_DECODE_EN, reg | CF9_IO_EN);
}

static void sb_enable_legacy_io(void)
{
	uint32_t reg = pm_read32(PM_DECODE_EN);

	pm_write32(PM_DECODE_EN, reg | LEGACY_IO_EN);
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

static void sb_lpc_early_setup(void)
{
	uint32_t dword;

	/* Enable SPI prefetch */
	dword = pci_read_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL);
	dword |= SPI_FROM_HOST_PREFETCH_EN | SPI_FROM_USB_PREFETCH_EN;
	pci_write_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL, dword);

	if (IS_ENABLED(CONFIG_STONEYRIDGE_LEGACY_FREE)) {
		/* Decode SIOs at 2E/2F and 4E/4F */
		dword = pci_read_config32(SOC_LPC_DEV,
						LPC_IO_OR_MEM_DECODE_ENABLE);
		dword |= DECODE_ALTERNATE_SIO_ENABLE | DECODE_SIO_ENABLE;
		pci_write_config32(SOC_LPC_DEV,
					LPC_IO_OR_MEM_DECODE_ENABLE, dword);
	}
}

static void setup_spread_spectrum(int *reboot)
{
	uint16_t rstcfg = pm_read16(PWR_RESET_CFG);

	rstcfg &= ~TOGGLE_ALL_PWR_GOOD;
	pm_write16(PWR_RESET_CFG, rstcfg);

	uint32_t cntl1 = misc_read32(MISC_CLK_CNTL1);

	if (cntl1 & CG1PLL_FBDIV_TEST) {
		printk(BIOS_DEBUG, "Spread spectrum is ready\n");
		misc_write32(MISC_CGPLL_CONFIG1,
			     misc_read32(MISC_CGPLL_CONFIG1) |
				     CG1PLL_SPREAD_SPECTRUM_ENABLE);

		return;
	}

	printk(BIOS_DEBUG, "Setting up spread spectrum\n");

	uint32_t cfg6 = misc_read32(MISC_CGPLL_CONFIG6);
	cfg6 &= ~CG1PLL_LF_MODE_MASK;
	cfg6 |= (0x0F8 << CG1PLL_LF_MODE_SHIFT) & CG1PLL_LF_MODE_MASK;
	misc_write32(MISC_CGPLL_CONFIG6, cfg6);

	uint32_t cfg3 = misc_read32(MISC_CGPLL_CONFIG3);
	cfg3 &= ~CG1PLL_REFDIV_MASK;
	cfg3 |= (0x003 << CG1PLL_REFDIV_SHIFT) & CG1PLL_REFDIV_MASK;
	cfg3 &= ~CG1PLL_FBDIV_MASK;
	cfg3 |= (0x04B << CG1PLL_FBDIV_SHIFT) & CG1PLL_FBDIV_MASK;
	misc_write32(MISC_CGPLL_CONFIG3, cfg3);

	uint32_t cfg5 = misc_read32(MISC_CGPLL_CONFIG5);
	cfg5 &= ~CG1PLL_SS_AMOUNT_NFRAC_SLIP_MASK;
	cfg5 |= (0x2 << CG1PLL_SS_AMOUNT_NFRAC_SLIP_SHIFT) & CG1PLL_SS_AMOUNT_NFRAC_SLIP_MASK;
	misc_write32(MISC_CGPLL_CONFIG5, cfg5);

	uint32_t cfg4 = misc_read32(MISC_CGPLL_CONFIG4);
	cfg4 &= ~CG1PLL_SS_AMOUNT_DSFRAC_MASK;
	cfg4 |= (0xD000 << CG1PLL_SS_AMOUNT_DSFRAC_SHIFT) & CG1PLL_SS_AMOUNT_DSFRAC_MASK;
	cfg4 &= ~CG1PLL_SS_STEP_SIZE_DSFRAC_MASK;
	cfg4 |= (0x02D5 << CG1PLL_SS_STEP_SIZE_DSFRAC_SHIFT) & CG1PLL_SS_STEP_SIZE_DSFRAC_MASK;
	misc_write32(MISC_CGPLL_CONFIG4, cfg4);

	rstcfg |= TOGGLE_ALL_PWR_GOOD;
	pm_write16(PWR_RESET_CFG, rstcfg);

	cntl1 |= CG1PLL_FBDIV_TEST;
	misc_write32(MISC_CLK_CNTL1, cntl1);

	*reboot = 1;
}

static void setup_misc(int *reboot)
{
	/* Undocumented register */
	uint32_t reg = misc_read32(0x50);
	if (!(reg & BIT(16))) {
		reg |= BIT(16);

		misc_write32(0x50, reg);
		*reboot = 1;
	}
}

void bootblock_fch_early_init(void)
{
	int reboot = 0;

	sb_enable_rom();
	sb_lpc_port80();
	sb_lpc_decode();
	sb_lpc_early_setup();
	sb_spibase();
	sb_disable_4dw_burst(); /* Must be disabled on CZ(ST) */
	sb_acpi_mmio_decode();
	sb_enable_cf9_io();
	setup_spread_spectrum(&reboot);
	setup_misc(&reboot);

	if (reboot)
		soft_reset();

	sb_enable_legacy_io();
	enable_aoac_devices();
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

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pm1_sts = inw(ACPI_PM1_STS);
	outw(pm1_sts, ACPI_PM1_STS);
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *const pm1_sts_bits[] = {
		[0] = "TMROF",
		[4] = "BMSTATUS",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

static void sb_log_pm1_status(uint16_t pm1_sts)
{
	if (!IS_ENABLED(CONFIG_ELOG))
		return;

	if (pm1_sts & WAK_STS)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
				    acpi_is_wakeup_s3() ? ACPI_S3 : ACPI_S5);

	if (pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	if (pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	if (pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);
}

static void sb_save_sws(uint16_t pm1_status)
{
	struct soc_power_reg *sws;
	uint32_t reg32;
	uint16_t reg16;

	sws = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(struct soc_power_reg));
	if (sws == NULL)
		return;
	sws->pm1_sts = pm1_status;
	sws->pm1_en = inw(ACPI_PM1_EN);
	reg32 = inl(ACPI_GPE0_STS);
	outl(ACPI_GPE0_STS, reg32);
	sws->gpe0_sts = reg32;
	sws->gpe0_en = inl(ACPI_GPE0_EN);
	reg16 = inw(ACPI_PM1_CNT_BLK);
	reg16 &= SLP_TYP;
	sws->wake_from = reg16 >> SLP_TYP_SHIFT;
}

static void sb_clear_pm1_status(void)
{
	uint16_t pm1_sts = reset_pm1_status();

	sb_save_sws(pm1_sts);
	sb_log_pm1_status(pm1_sts);
	print_pm1_status(pm1_sts);
}

static int get_index_bit(uint32_t value, uint16_t limit)
{
	uint16_t i;
	uint32_t t;

	if (limit >= TOTAL_BITS(uint32_t))
		return -1;

	/* get a mask of valid bits. Ex limit = 3, set bits 0-2 */
	t = (1 << limit) - 1;
	if ((value & t) == 0)
		return -1;
	t = 1;
	for (i = 0; i < limit; i++) {
		if (value & t)
			break;
		t <<= 1;
	}
	return i;
}

static void set_nvs_sws(void *unused)
{
	struct soc_power_reg *sws;
	struct global_nvs_t *gnvs;
	int index;

	sws = cbmem_find(CBMEM_ID_POWER_STATE);
	if (sws == NULL)
		return;
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs == NULL)
		return;

	index = get_index_bit(sws->pm1_sts & sws->pm1_en, PM1_LIMIT);
	if (index < 0)
		gnvs->pm1i = ~0ULL;
	else
		gnvs->pm1i = index;

	index = get_index_bit(sws->gpe0_sts & sws->gpe0_en, GPE0_LIMIT);
	if (index < 0)
		gnvs->gpei = ~0ULL;
	else
		gnvs->gpei = index;
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, set_nvs_sws, NULL);

void southbridge_init(void *chip_info)
{
	sb_init_acpi_ports();
	sb_clear_pm1_status();
}

void southbridge_final(void *chip_info)
{
	uint8_t restored_power = PM_S5_AT_POWER_RECOVERY;

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
