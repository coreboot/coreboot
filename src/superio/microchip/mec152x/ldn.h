/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_MICROCHIP_MEC152X_LDN_H
#define SUPERIO_MICROCHIP_MEC152X_LDN_H

/*
 * The MEC152x family deviates from ISA PnP such that io base addresses
 * (`io 0x60 = ...`) and irq assignments (`irq 0x70 = ...`) are not written
 * to the affected LDN's register space, but to the MEC152X_LDN_ESPI_IOC LDN
 * instead.
 * We present this chip to coreboot as if it used the "normal" behavior and
 * internally remap these registers, based on the LDN, to the "true registers".
 */

enum mec152x_ldn {
	MEC152X_LDN_MAILBOX = 0x0,
	MEC152X_LDN_KBC = 0x1,
	MEC152X_LDN_ACPI_EC0 = 0x2,
	MEC152X_LDN_ACPI_EC1 = 0x3,
	MEC152X_LDN_ACPI_EC2 = 0x4,
	MEC152X_LDN_ACPI_EC3 = 0x5,
	MEC152X_LDN_ACPI_PM1 = 0x7,
	MEC152X_LDN_LEGACY_FAST_KB = 0x8,
	MEC152X_LDN_UART0 = 0x9,
	MEC152X_LDN_UART1 = 0xa,
	MEC152X_LDN_UART2 = 0xb,
	MEC152X_LDN_ESPI_IOC = 0xd,
	MEC152X_LDN_ESPI_MC = 0xe,
	MEC152X_LDN_EMI0 = 0x10,
	MEC152X_LDN_EMI1 = 0x11,
	MEC152X_LDN_RTC = 0x14,
	MEC152X_LDN_PORT80_0 = 0x20,
	MEC152X_LDN_PORT80_1 = 0x21,
	MEC152X_LDN_TEST = 0x2f
};

#endif
