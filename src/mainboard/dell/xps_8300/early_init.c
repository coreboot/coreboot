/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>

#define GPIO_DEV  PNP_DEV(0x2e, IT8772F_GPIO)
#define MOUSE_DEV PNP_DEV(0x2e, IT8772F_KBCM)
#define EC_DEV    PNP_DEV(0x2e, IT8772F_EC)

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0e);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);

	/* Set up GPIOs on Super I/O. */
	ite_ac_resume_southbridge(EC_DEV);
	ite_reg_write(MOUSE_DEV, 0x30, 0x00); // PS/2 Mouse disable

	ite_reg_write(EC_DEV, 0x30, 0x01); // Environment controller activate
	ite_reg_write(EC_DEV, 0x60, 0x0a); // Environment controller MSB Register Base Address
	ite_reg_write(EC_DEV, 0x61, 0x30); // Environment controller LSB Register Base Address
	ite_reg_write(EC_DEV, 0x62, 0x0a); // PME Direct Access MSB Register Base Address
	ite_reg_write(EC_DEV, 0x63, 0x20); // PME Direct Access LSB Register Base Address

	ite_reg_write(GPIO_DEV, 0x25, 0x00); // GPIO Set 1
	ite_reg_write(GPIO_DEV, 0x26, 0xfc); // GPIO Set 2, Enable pin 7 and 8 to GPIO
	ite_reg_write(GPIO_DEV, 0x27, 0x00); // GPIO Set 3
	ite_reg_write(GPIO_DEV, 0x28, 0x00); // GPIO Set 4
	ite_reg_write(GPIO_DEV, 0x29, 0x00); // GPIO Set 5 and 6
	ite_reg_write(GPIO_DEV, 0x2a, 0x00); // Special function 1
	ite_reg_write(GPIO_DEV, 0x2b, 0x00); // Special function 2
	ite_reg_write(GPIO_DEV, 0x2c, 0x03); // Special function 3
	ite_reg_write(GPIO_DEV, 0x60, 0x0a); // SMI MSB Register Base Address
	ite_reg_write(GPIO_DEV, 0x62, 0x0a); // Simple I/O MSB Register Base Address
	ite_reg_write(GPIO_DEV, 0xb0, 0x00); // Pin set 1 polarity registers
	ite_reg_write(GPIO_DEV, 0xb1, 0x00); // Pin set 2 polarity registers
	ite_reg_write(GPIO_DEV, 0xb2, 0x00); // Pin set 3 polarity registers
	ite_reg_write(GPIO_DEV, 0xb3, 0x00); // Pin set 4 polarity registers
	ite_reg_write(GPIO_DEV, 0xb4, 0x00); // Pin set 5 polarity registers
	ite_reg_write(GPIO_DEV, 0xb8, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xb9, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xba, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xbb, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xbc, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xbd, 0x00); // Pin set 1 int pull-up disable
	ite_reg_write(GPIO_DEV, 0xc0, 0x01); // Set Simple I/O functions on SI/O Set 1
	ite_reg_write(GPIO_DEV, 0xc1, 0x0c); // Set Simple I/O functions on SI/O Set 2
	ite_reg_write(GPIO_DEV, 0xc2, 0x00); // Set Simple I/O functions on SI/O Set 3
	ite_reg_write(GPIO_DEV, 0xc3, 0x40); // Set Simple I/O functions on SI/O Set 4
	ite_reg_write(GPIO_DEV, 0xc4, 0x00); // Set Simple I/O functions on SI/O Set 5
	ite_reg_write(GPIO_DEV, 0xc8, 0x01); // Set Simple I/O Output on SI/O Set 1
	ite_reg_write(GPIO_DEV, 0xc9, 0x0c); // Set Simple I/O Output on SI/O Set 2
	ite_reg_write(GPIO_DEV, 0xca, 0x00); // Set Simple I/O Output on SI/O Set 3
	ite_reg_write(GPIO_DEV, 0xcb, 0x40); // Set Simple I/O Output on SI/O Set 4
	ite_reg_write(GPIO_DEV, 0xcc, 0x00); // Set Simple I/O Output on SI/O Set 5
	ite_reg_write(GPIO_DEV, 0xcd, 0x00); // Set Simple I/O Output on SI/O Set 6
	ite_reg_write(GPIO_DEV, 0xe9, 0x07); // GPIO Bus Select Control Register
	ite_reg_write(GPIO_DEV, 0xf6, 0x00); // Hardware Monitor Alert Beep Pin Mapping Register
}
