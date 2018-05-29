/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifdef __PRE_RAM__

#include <arch/cpu.h>
#include "southbridge/amd/sb600/sb600.h"

#else

#include <device/pci.h>
#include <device/pci_ids.h>

#endif

#include "tn_post_code.h"


#ifdef __SIMPLE_DEVICE__

// TechNexion's Post Code Initially.
void technexion_post_code_init(void)
{
	uint8_t reg8_data;
	pci_devfn_t dev = 0;

	// SMBus Module and ACPI Block (Device 20, Function 0)
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB600_SM), 0);

	// LED[bit0]:GPIO0
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pmio_read(0x60);
	reg8_data |= (1 << 7);  // 1: GPIO if not used by SATA
	pmio_write(0x60, reg8_data);

	reg8_data = pci_read_config8(dev, 0x80);
	reg8_data = ((reg8_data | (1 << 0)) & ~(1 << 4));
	pci_write_config8(dev, 0x80, reg8_data);

	// LED[bit1]:GPIO1
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pci_read_config8(dev, 0x80);
	reg8_data = ((reg8_data | (1 << 1)) & ~(1 << 5));
	pci_write_config8(dev, 0x80, reg8_data);

	// LED[bit2]:GPIO4
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pmio_read(0x5e);
	reg8_data &= ~(1 << 7); // 0: GPIO if not used by SATA
	pmio_write(0x5e, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa8);
	reg8_data |= (1 << 0);
	pci_write_config8(dev, 0xa8, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa9);
	reg8_data &= ~(1 << 0);
	pci_write_config8(dev, 0xa9, reg8_data);

	// LED[bit3]:GPIO6
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pmio_read(0x60);
	reg8_data |= (1 << 7); // 1: GPIO if not used by SATA
	pmio_write(0x60, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa8);
	reg8_data |= (1 << 2);
	pci_write_config8(dev, 0xa8, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa9);
	reg8_data &= ~(1 << 2);
	pci_write_config8(dev, 0xa9, reg8_data);
	// LED[bit4]:GPIO7
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pci_read_config8(dev, 0xa8);
	reg8_data |= (1 << 3);
	pci_write_config8(dev, 0xa8, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa9);
	reg8_data &= ~(1 << 3);
	pci_write_config8(dev, 0xa9, reg8_data);

	// LED[bit5]:GPIO8
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pci_read_config8(dev, 0xa8);
	reg8_data |= (1 << 4);
	pci_write_config8(dev, 0xa8, reg8_data);

	reg8_data = pci_read_config8(dev, 0xa9);
	reg8_data &= ~(1 << 4);
	pci_write_config8(dev, 0xa9, reg8_data);

	// LED[bit6]:GPIO10
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pci_read_config8(dev, 0xab);
	reg8_data = ((reg8_data | (1 << 0)) & ~(1 << 1));
	pci_write_config8(dev, 0xab, reg8_data);

	// LED[bit7]:GPIO66
	// This is reference SB600 RRG 4.1.1 GPIO
	reg8_data = pmio_read(0x68);
	reg8_data &= ~(1 << 5); // 0: GPIO
	pmio_write(0x68, reg8_data);

	reg8_data = pci_read_config8(dev, 0x7e);
	reg8_data = ((reg8_data | (1 << 1)) & ~(1 << 5));
	pci_write_config8(dev, 0x7e, reg8_data);

}

#endif

/* TechNexion's Post Code.
 */
void technexion_post_code(uint8_t udata8)
{
	uint8_t u8_data;

	// SMBus Module and ACPI Block (Device 20, Function 0)
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev = 0;
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB600_SM), 0);
#else
	struct device *dev = NULL;
	dev = dev_find_device(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB600_SM, 0);
#endif

	udata8 = ~(udata8);

	// LED[bit0]:GPIO0
	u8_data = pci_read_config8(dev, 0x80);
	if (udata8 & 0x1) {
		u8_data |= (1 << 0);
	}
	else {
		u8_data &= ~(1 << 0);
	}
	pci_write_config8(dev, 0x80, u8_data);

	// LED[bit1]:GPIO1
	u8_data = pci_read_config8(dev, 0x80);
	if (udata8 & 0x2) {
		u8_data |= (1 << 1);
	}
	else {
		u8_data &= ~(1 << 1);
	}
	pci_write_config8(dev, 0x80, u8_data);

	// LED[bit2]:GPIO4
	u8_data = pci_read_config8(dev, 0xa8);
	if (udata8 & 0x4) {
		u8_data |= (1 << 0);
	}
	else {
		u8_data &= ~(1 << 0);
	}
	pci_write_config8(dev, 0xa8, u8_data);

	// LED[bit3]:GPIO6
	u8_data = pci_read_config8(dev, 0xa8);
	if (udata8 & 0x8) {
		u8_data |= (1 << 2);
	}
	else {
		u8_data &= ~(1 << 2);
	}
	pci_write_config8(dev, 0xa8, u8_data);

	// LED[bit4]:GPIO7
	u8_data = pci_read_config8(dev, 0xa8);
	if (udata8 & 0x10) {
		u8_data |= (1 << 3);
	}
	else {
		u8_data &= ~(1 << 3);
	}
	pci_write_config8(dev, 0xa8, u8_data);

	// LED[bit5]:GPIO8
	u8_data = pci_read_config8(dev, 0xa8);
	if (udata8 & 0x20) {
		u8_data |= (1 << 4);
	}
	else {
		u8_data &= ~(1 << 4);
	}
	pci_write_config8(dev, 0xa8, u8_data);

	// LED[bit6]:GPIO10
	u8_data = pci_read_config8(dev, 0xab);
	if (udata8 & 0x40) {
		u8_data |= (1 << 0);
	}
	else {
		u8_data &= ~(1 << 0);
	}
	pci_write_config8(dev, 0xab, u8_data);

	// LED[bit7]:GPIO66
	u8_data = pci_read_config8(dev, 0x7e);
	if (udata8 & 0x80) {
		u8_data |= (1 << 1);
	}
	else {
		u8_data &= ~(1 << 1);
	}
	pci_write_config8(dev, 0x7e, u8_data);

}
