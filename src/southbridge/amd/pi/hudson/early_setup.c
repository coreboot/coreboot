/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _HUDSON_EARLY_SETUP_C_
#define _HUDSON_EARLY_SETUP_C_

#include <assert.h>
#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <device/mmio.h>
#include <device/pci_ops.h>

#include "hudson.h"
#include "pci_devs.h"
#include <Fch/Fch.h>

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

	/* Enable port 80 LPC decode in pci function 3 configuration space. */
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x4a);
	byte |= 1 << 5; /* enable port 80 */
	pci_write_config8(dev, 0x4a, byte);
}

void hudson_lpc_decode(void)
{
	u32 tmp;

	/* Enable LPC controller */
	pm_write8(0xec, pm_read8(0xec) | 0x01);

	const pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	/* Serial port enumeration on Hudson:
	 * PORT0 - 0x3f8
	 * PORT1 - 0x2f8
	 * PORT5 - 0x2e8
	 * PORT7 - 0x3e8
	 */
	tmp =  DECODE_ENABLE_SERIAL_PORT0 | DECODE_ENABLE_SERIAL_PORT1
	     | DECODE_ENABLE_SERIAL_PORT5 | DECODE_ENABLE_SERIAL_PORT7;

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
	const pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
	uint32_t tmp;

	/* Only allow port 0-2 */
	assert(port <= ARRAY_SIZE(wideio_enable));

	if (size == 16) {
		tmp = pci_read_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE);
		tmp |= alt_wideio_enable[port];
		pci_write_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE, tmp);
	} else {	/* 512 */
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
	const pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
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
			BUG();
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

void hudson_clk_output_48Mhz(void)
{
	u32 ctrl;

	/*
	 * Enable the X14M_25M_48M_OSC pin and leaving it at it's default so
	 * 48Mhz will be on ball AP13 (FT3b package)
	 */
	ctrl = misc_read32(FCH_MISC_REG40);

	/* clear the OSCOUT1_ClkOutputEnb to enable the 48 Mhz clock */
	ctrl &= (u32)~(1 << 2);
	misc_write32(FCH_MISC_REG40, ctrl);
}

static uintptr_t hudson_spibase(void)
{
	/* Make sure the base address is predictable */
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 3);

	u32 base = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER)
							& 0xfffffff0;
	if (!base) {
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
	write16p(base + SPI100_SPEED_CONFIG,
			(norm << SPI_NORM_SPEED_NEW_SH) |
			(fast << SPI_FAST_SPEED_NEW_SH) |
			(alt << SPI_ALT_SPEED_NEW_SH) |
			(tpm << SPI_TPM_SPEED_NEW_SH));
	write16p(base + SPI100_ENABLE, SPI_USE_SPI100 |
		read16p(base + SPI100_ENABLE));
}

void hudson_disable_4dw_burst(void)
{
	uintptr_t base = hudson_spibase();
	write16p(base + SPI100_HOST_PREF_CONFIG,
		read16p(base + SPI100_HOST_PREF_CONFIG)
				& ~SPI_RD4DW_EN_HOST);
}

/* Hudson 1-3 only.  For Hudson 1, call with fast=1 */
void hudson_set_readspeed(u16 norm, u16 fast)
{
	uintptr_t base = hudson_spibase();
	write16p(base + SPI_CNTRL1,
		(read16p(base + SPI_CNTRL1)
		& ~SPI_CNTRL1_SPEED_MASK)
		| (norm << SPI_NORM_SPEED_SH)
		| (fast << SPI_FAST_SPEED_SH));
}

void hudson_read_mode(u32 mode)
{
	uintptr_t base = hudson_spibase();
	write32p(base + SPI_CNTRL0,
		(read32p(base + SPI_CNTRL0)
			& ~SPI_READ_MODE_MASK) | mode);
}

void hudson_tpm_decode_spi(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x14, 3);	/* LPC device */

	u32 spibase = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER);
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER, spibase
							| ROUTE_TPM_2_SPI);
}

#endif
