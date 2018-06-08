/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef _HUDSON_EARLY_SETUP_C_
#define _HUDSON_EARLY_SETUP_C_

#include <assert.h>
#include <stdint.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <reset.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include "hudson.h"
#include "pci_devs.h"
#include <Fch/Fch.h>

#if IS_ENABLED(CONFIG_HUDSON_UART)

#include <cpu/x86/msr.h>
#include <delay.h>

void configure_hudson_uart(void)
{
	u8 byte;

	byte = read8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG56 + CONFIG_UART_FOR_CONSOLE * 2);
	byte |= 1 << 3;
	write8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG56 + CONFIG_UART_FOR_CONSOLE * 2, byte);
	byte = read8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG62);
	byte |= 1 << 3;
	write8((void *)ACPI_MMIO_BASE + AOAC_BASE + FCH_AOAC_REG62, byte);
	write8((void *)FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0);
	write8((void *)FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0);
	write8((void *)FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0);
	write8((void *)FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0);

	udelay(2000);
	write8((void *)0xFEDC6000 + 0x2000 * CONFIG_UART_FOR_CONSOLE + 0x88, 0x01); /* reset UART */
}

#endif

void hudson_pci_port80(void)
{
	u8 byte;
	pci_devfn_t dev;

	/* P2P Bridge */
	dev = PCI_DEV(0, 0x14, 4);

	/* Chip Control: Enable subtractive decoding */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 5;
	pci_write_config8(dev, 0x40, byte);

	/* Misc Control: Enable subtractive decoding if 0x40 bit 5 is set */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 7;
	pci_write_config8(dev, 0x4B, byte);

	/* The same IO Base and IO Limit here is meaningful because we set the
	 * bridge to be subtractive. During early setup stage, we have to make
	 * sure that data can go through port 0x80.
	 */
	/* IO Base: 0xf000 */
	byte = pci_read_config8(dev, 0x1C);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1C, byte);

	/* IO Limit: 0xf000 */
	byte = pci_read_config8(dev, 0x1D);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1D, byte);

	/* PCI Command: Enable IO response */
	byte = pci_read_config8(dev, 0x04);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x04, byte);

	/* LPC controller */
	dev = PCI_DEV(0, 0x14, 3);

	byte = pci_read_config8(dev, 0x4A);
	byte &= ~(1 << 5); /* disable lpc port 80 */
	pci_write_config8(dev, 0x4A, byte);
}

void hudson_lpc_port80(void)
{
	u8 byte;
	pci_devfn_t dev;

	/* Enable LPC controller */
	outb(0xEC, 0xCD6);
	byte = inb(0xCD7);
	byte |= 1;
	outb(0xEC, 0xCD6);
	outb(byte, 0xCD7);

	/* Enable port 80 LPC decode in pci function 3 configuration space. */
	dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x4a);
	byte |= 1 << 5; /* enable port 80 */
	pci_write_config8(dev, 0x4a, byte);
}

void hudson_lpc_decode(void)
{
	pci_devfn_t dev;
	u32 tmp = 0;

	/* Enable I/O decode to LPC bus */
	dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
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

	pci_write_config32(dev, LPC_IO_PORT_DECODE_ENABLE, tmp);
}

static void enable_wideio(uint8_t port, uint16_t size)
{
	uint32_t wideio_enable[] = {
		LPC_WIDEIO0_ENABLE,
		LPC_WIDEIO1_ENABLE,
		LPC_WIDEIO2_ENABLE
	};
	uint32_t alt_wideio_enable[] = {
		LPC_ALT_WIDEIO0_ENABLE,
		LPC_ALT_WIDEIO1_ENABLE,
		LPC_ALT_WIDEIO2_ENABLE
	};
	pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
	uint32_t tmp;

	/* Only allow port 0-2 */
	assert(port <= ARRAY_SIZE(wideio_enable));

	if (size == 16) {
		tmp = pci_read_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE);
		tmp |= alt_wideio_enable[port];
		pci_write_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE, tmp);
	} else { 	/* 512 */
		tmp = pci_read_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE);
		tmp &= ~alt_wideio_enable[port];
		pci_write_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE, tmp);
	}

	/* Enable the range */
	tmp = pci_read_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE);
	tmp |= wideio_enable[port];
	pci_write_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE, tmp);
}

/*
 * lpc_wideio_window() may be called any point in romstage, but take
 * care that AGESA doesn't overwrite the range this function used.
 * The function checks if there is an empty range and if all ranges are
 * used the function throws an assert. The function doesn't check for a
 * duplicate range, for ranges that can  be merged into a single
 * range, or ranges that overlap.
 *
 * The developer is expected to ensure that there are no conflicts.
 */
static void lpc_wideio_window(uint16_t base, uint16_t size)
{
	pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
	u32 tmp;

	/* Support 512 or 16 bytes per range */
	assert(size == 512 || size == 16);

	/* Find and open Base Register and program it */
	tmp = pci_read_config32(dev, LPC_WIDEIO_GENERIC_PORT);

	if ((tmp & 0xFFFF) == 0) {	/* WIDEIO0 */
		tmp |= base;
		pci_write_config32(dev, LPC_WIDEIO_GENERIC_PORT, tmp);
		enable_wideio(0, size);
	} else if ((tmp & 0xFFFF0000) == 0) {	/* WIDEIO1 */
		tmp |= (base << 16);
		pci_write_config32(dev, LPC_WIDEIO_GENERIC_PORT, tmp);
		enable_wideio(1, size);
	} else { /* Check WIDEIO2 register */
		tmp = pci_read_config32(dev, LPC_WIDEIO2_GENERIC_PORT);
		if ((tmp & 0xFFFF) == 0) {	/* WIDEIO2 */
			tmp |= base;
			pci_write_config32(dev, LPC_WIDEIO2_GENERIC_PORT, tmp);
			enable_wideio(2, size);
		} else {	/* All WIDEIO locations used*/
			assert(0);
		}
	}
}

void lpc_wideio_512_window(uint16_t base)
{
	assert(IS_ALIGNED(base, 512));
	lpc_wideio_window(base, 512);
}

void lpc_wideio_16_window(uint16_t base)
{
	assert(IS_ALIGNED(base, 16));
	lpc_wideio_window(base, 16);
}

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos)
{
	int i;
	printk(BIOS_DEBUG, "Writing %x of size %d to nvram pos: %d\n", dword, size, nvram_pos);

	for (i = 0; i < size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
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
		data |= inb(BIOSRAM_DATA) << (i *8);
		nvram_pos++;
	}
	*old_dword = data;
	printk(BIOS_DEBUG, "Loading %x of size %d to nvram pos:%d\n", *old_dword, size,
		nvram_pos-size);
	return nvram_pos;
}

void hudson_clk_output_48Mhz(void)
{
	u32 ctrl;

	/*
	 * Enable the X14M_25M_48M_OSC pin and leaving it at it's default so
	 * 48Mhz will be on ball AP13 (FT3b package)
	 */
	ctrl = read32((void *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40));

	/* clear the OSCOUT1_ClkOutputEnb to enable the 48 Mhz clock */
	ctrl &= (u32)~(1<<2);
	write32((void *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40), ctrl);
}

static uintptr_t hudson_spibase(void)
{
	/* Make sure the base address is predictable */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);

	u32 base = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER)
							& 0xfffffff0;
	if (!base){
		base = SPI_BASE_ADDRESS;
		pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER, base
							| SPI_ROM_ENABLE);
		/* PCI_COMMAND_MEMORY is read-only and enabled. */
	}
	return (uintptr_t)base;
}

void hudson_set_spi100(u16 norm, u16 fast, u16 alt, u16 tpm)
{
	uintptr_t base = hudson_spibase();
	write16((void *)base + SPI100_SPEED_CONFIG,
				(norm << SPI_NORM_SPEED_NEW_SH) |
				(fast << SPI_FAST_SPEED_NEW_SH) |
				(alt << SPI_ALT_SPEED_NEW_SH) |
				(tpm << SPI_TPM_SPEED_NEW_SH));
	write16((void *)base + SPI100_ENABLE, SPI_USE_SPI100);
}

void hudson_disable_4dw_burst(void)
{
	uintptr_t base = hudson_spibase();
	write16((void *)base + SPI100_HOST_PREF_CONFIG,
			read16((void *)base + SPI100_HOST_PREF_CONFIG)
					& ~SPI_RD4DW_EN_HOST);
}

/* Hudson 1-3 only.  For Hudson 1, call with fast=1 */
void hudson_set_readspeed(u16 norm, u16 fast)
{
	uintptr_t base = hudson_spibase();
	write16((void *)base + SPI_CNTRL1, (read16((void *)base + SPI_CNTRL1)
					& ~SPI_CNTRL1_SPEED_MASK)
					| (norm << SPI_NORM_SPEED_SH)
					| (fast << SPI_FAST_SPEED_SH));
}

void hudson_read_mode(u32 mode)
{
	uintptr_t base = hudson_spibase();
	write32((void *)base + SPI_CNTRL0,
			(read32((void *)base + SPI_CNTRL0)
					& ~SPI_READ_MODE_MASK) | mode);
}

void hudson_tpm_decode_spi(void)
{
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);	/* LPC device */

	u32 spibase = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER);
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER, spibase
							| ROUTE_TPM_2_SPI);
}

#endif
