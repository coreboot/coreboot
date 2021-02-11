/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _HUDSON_EARLY_SETUP_C_
#define _HUDSON_EARLY_SETUP_C_

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <device/pci_ops.h>

#include "hudson.h"

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

#endif /* _HUDSON_EARLY_SETUP_C_ */
