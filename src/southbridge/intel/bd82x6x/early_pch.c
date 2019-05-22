/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
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

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <arch/cbfs.h>
#include <ip_checksum.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/rcba.h>
#include <console/console.h>

/* For DMI bar.  */
#include <northbridge/intel/sandybridge/sandybridge.h>

#include "pch.h"
#include "chip.h"

#define SOUTHBRIDGE PCI_DEV(0, 0x1f, 0)

static void
wait_iobp(void)
{
	while (RCBA8(IOBPS) & 1)
		; // implement timeout?
}

static u32
read_iobp(u32 address)
{
	u32 ret;

	RCBA32(IOBPIRI) = address;
	RCBA16(IOBPS) = (RCBA16(IOBPS) & 0x1ff) | 0x600;
	wait_iobp();
	ret = RCBA32(IOBPD);
	wait_iobp();
	RCBA8(IOBPS); // call wait_iobp() instead here?
	return ret;
}

static void
write_iobp(u32 address, u32 val)
{
	/* this function was probably pch_iobp_update with the andvalue
	 * being 0. So either the IOBP read can be removed or this function
	 * and the pch_iobp_update function in ramstage could be merged */
	read_iobp(address);
	RCBA16(IOBPS) = (RCBA16(IOBPS) & 0x1ff) | 0x600;
	wait_iobp();

	RCBA32(IOBPD) = val;
	wait_iobp();
	RCBA16(IOBPS) = (RCBA16(IOBPS) & 0x1ff) | 0x600;

	RCBA8(IOBPS); // call wait_iobp() instead here?
}

void early_pch_init_native_dmi_pre(void)
{
	/* Link Capabilities Register */
	RCBA32(0x21a4) = (RCBA32(0x21a4) & ~0x3fc00) |
			 (3 << 10) | // L0s and L1 entry supported
			 (2 << 12) | // L0s 128 ns to less than 256 ns
			 (2 << 15);  // L1 2 us to less than 4 us

	RCBA32(0x2340) = (RCBA32(0x2340) & ~0xff0000) | (0x3a << 16);
	RCBA8(0x21b0) = (RCBA8(0x21b0) & ~0xf) | 2;
}

void early_pch_init_native_dmi_post(void)
{
	RCBA32(0x0050);	// !!! = 0x01200654
	RCBA32(0x0050) = 0x01200654;
	RCBA32(0x0050);	// !!! = 0x01200654
	RCBA32(0x0050) = 0x012a0654;
	RCBA32(0x0050);	// !!! = 0x012a0654
	RCBA8(0x1114);	// !!! = 0x00
	RCBA8(0x1114) = 0x05;

	/*
	 * Virtual Channel resources must match settings in DMIBAR!
	 *
	 * Some of the following settings are taken from
	 * "Intel Core i5-600, i3-500 Desktop Processor Series and Intel
	 * Pentium Desktop Processor 6000 Series Vol. 2" datasheet and
	 * serialice traces.
	 */

	/* Virtual Channel 0 Resource Control Register.
	 * Enable channel.
	 * Set Virtual Channel Identifier.
	 * Map TC0 and TC3 and TC4 to VC0.
	 */

	RCBA32(0x2014) = (1 << 31) | (0 << 24) | (0x0c << 1) | 1;

	/* Virtual Channel 1 Resource Control Register.
	 * Enable channel.
	 * Set Virtual Channel Identifier.
	 * Map TC1 and TC5 to VC1.
	 */
	RCBA32(0x2020) = (1 << 31) | (1 << 24) | (0x11 << 1);
	/* Read back register */
	RCBA32(0x2020);

	/* Virtual Channel private Resource Control Register.
	 * Enable channel.
	 * Set Virtual Channel Identifier.
	 * Map TC2 and TC6 to VCp.
	 */
	RCBA32(0x2030) = (1 << 31) | (2 << 24) | (0x22 << 1);
	/* Read back register */
	RCBA32(0x2030);

	/* Virtual Channel ME Resource Control Register.
	 * Enable channel.
	 * Set Virtual Channel Identifier.
	 * Map TC7 to VCm.
	 */
	RCBA32(0x2040) = (1 << 31) | (7 << 24) | (0x40 << 1);

	/* Lock Virtual Channel Resource control register. */
	RCBA32(0x0050) |= 0x80000000;
	/* Read back register */
	RCBA32(0x0050);

	/* Wait for virtual channels negotiation pending */
	while (RCBA16(0x201a) & VCNEGPND)
		;
	while (RCBA16(0x2026) & VCNEGPND)
		;
	while (RCBA16(0x2036) & VCNEGPND)
		;
	while (RCBA16(0x2046) & VCNEGPND)
		;
}

void
early_pch_init_native (void)
{
	pci_write_config8 (SOUTHBRIDGE, 0xa6,
			    pci_read_config8 (SOUTHBRIDGE, 0xa6) | 2);

	RCBA32(0x2088) = 0x00109000;
	RCBA32(0x20ac); // !!! = 0x00000000
	RCBA32(0x20ac) = 0x40000000;
	RCBA32(0x100c) = 0x01110000;
	RCBA8(0x2340) = 0x1b;
	RCBA32(0x2314); // !!! = 0x0a080000
	RCBA32(0x2314) = 0x0a280000;
	RCBA32(0x2310); // !!! = 0xc809605b
	RCBA32(0x2310) = 0xa809605b;
	RCBA32(0x2324) = 0x00854c74;
	RCBA8(0x0400);  // !!! = 0x00
	RCBA32(0x2310); // !!! = 0xa809605b
	RCBA32(0x2310) = 0xa809605b;
	RCBA32(0x2310); // !!! = 0xa809605b
	RCBA32(0x2310) = 0xa809605b;

	write_iobp(0xea007f62, 0x00590133);
	write_iobp(0xec007f62, 0x00590133);
	write_iobp(0xec007f64, 0x59555588);
	write_iobp(0xea0040b9, 0x0001051c);
	write_iobp(0xeb0040a1, 0x800084ff);
	write_iobp(0xec0040a1, 0x800084ff);
	write_iobp(0xea004001, 0x00008400);
	write_iobp(0xeb004002, 0x40201758);
	write_iobp(0xec004002, 0x40201758);
	write_iobp(0xea004002, 0x00601758);
	write_iobp(0xea0040a1, 0x810084ff);
	write_iobp(0xeb0040b1, 0x0001c598);
	write_iobp(0xec0040b1, 0x0001c598);
	write_iobp(0xeb0040b6, 0x0001c598);
	write_iobp(0xea0000a9, 0x80ff969f);
	write_iobp(0xea0001a9, 0x80ff969f);
	write_iobp(0xeb0040b2, 0x0001c396);
	write_iobp(0xeb0040b3, 0x0001c396);
	write_iobp(0xec0040b2, 0x0001c396);
	write_iobp(0xea0001a9, 0x80ff94ff);
	write_iobp(SATA_IOBP_SP0G3IR, 0x0088037f);
	write_iobp(0xea0000a9, 0x80ff94ff);
	write_iobp(SATA_IOBP_SP1G3IR, 0x0088037f);

	write_iobp(0xea007f05, 0x00010642);
	write_iobp(0xea0040b7, 0x0001c91c);
	write_iobp(0xea0040b8, 0x0001c91c);
	write_iobp(0xeb0040a1, 0x820084ff);
	write_iobp(0xec0040a1, 0x820084ff);
	write_iobp(0xea007f0a, 0xc2480000);

	write_iobp(0xec00404d, 0x1ff177f);
	write_iobp(0xec000084, 0x5a600000);
	write_iobp(0xec000184, 0x5a600000);
	write_iobp(0xec000284, 0x5a600000);
	write_iobp(0xec000384, 0x5a600000);
	write_iobp(0xec000094, 0x000f0501);
	write_iobp(0xec000194, 0x000f0501);
	write_iobp(0xec000294, 0x000f0501);
	write_iobp(0xec000394, 0x000f0501);
	write_iobp(0xec000096, 0x00000001);
	write_iobp(0xec000196, 0x00000001);
	write_iobp(0xec000296, 0x00000001);
	write_iobp(0xec000396, 0x00000001);
	write_iobp(0xec000001, 0x00008c08);
	write_iobp(0xec000101, 0x00008c08);
	write_iobp(0xec000201, 0x00008c08);
	write_iobp(0xec000301, 0x00008c08);
	write_iobp(0xec0040b5, 0x0001c518);
	write_iobp(0xec000087, 0x06077597);
	write_iobp(0xec000187, 0x06077597);
	write_iobp(0xec000287, 0x06077597);
	write_iobp(0xec000387, 0x06077597);
	write_iobp(0xea000050, 0x00bb0157);
	write_iobp(0xea000150, 0x00bb0157);
	write_iobp(0xec007f60, 0x77777d77);
	write_iobp(0xea00008d, 0x01320000);
	write_iobp(0xea00018d, 0x01320000);
	write_iobp(0xec0007b2, 0x04514b5e);
	write_iobp(0xec00078c, 0x40000200);
	write_iobp(0xec000780, 0x02000020);
}

static void pch_enable_bars(void)
{
	pci_write_config32(PCH_LPC_DEV, RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCH_LPC_DEV, PMBASE, DEFAULT_PMBASE | 1);

	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, 0x80);

	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);

	/* Enable GPIO functionality. */
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
}

static void pch_generic_setup(void)
{
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	write_pmbase16(TCO1_CNT, 1 << 11);	/* halt timer */
}

static void pch_enable_lpc_gen_decode(void)
{
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_bd82x6x_config *config = NULL;

	/* Set up generic decode ranges */
	if (!dev)
		return;
	if (dev->chip_info)
		config = dev->chip_info;
	if (!config)
		return;

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

void early_pch_init(void)
{
	pch_enable_lpc();

	pch_enable_lpc_gen_decode();

	pch_enable_bars();

	pch_generic_setup();

	setup_pch_gpios(&mainboard_gpio_map);
}
