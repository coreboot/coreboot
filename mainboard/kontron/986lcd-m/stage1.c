/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <cpu.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <uart8250.h>
#include <arch/x86/msr.h>
#include <arch/x86/lapic.h>

#include "superio/winbond/w83627thg/w83627thg.h"

#if 0
need these in makefile
#include "ram/ramtest.c"
#include "southbridge/intel/i82801gx/i82801gx_early_smbus.c"
#include "reset.c"
#include "superio/winbond/w83627thg/w83627thg_early_serial.c"

#include "northbridge/intel/i945/udelay.c"
#endif

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

#include "northbridge/intel/i945/ich7.h"
static void setup_ich7_gpios(void)
{
	/* TODO: This is highly board specific and should be moved */
	printk(BIOS_DEBUG, " GPIOS...");
	/* General Registers */
	outl(0x1f1ff7c0, DEFAULT_GPIOBASE + 0x00);	/* GPIO_USE_SEL */
	outl(0xe0e8efc3, DEFAULT_GPIOBASE + 0x04);	/* GP_IO_SEL */
	outl(0xebffeeff, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
	/* Output Control Registers */
	outl(0x00000000, DEFAULT_GPIOBASE + 0x18);	/* GPO_BLINK */
	/* Input Control Registers */
	outl(0x00002180, DEFAULT_GPIOBASE + 0x2c);	/* GPI_INV */
	outl(0x000000ff, DEFAULT_GPIOBASE + 0x30);	/* GPIO_USE_SEL2 */
	outl(0x00000030, DEFAULT_GPIOBASE + 0x34);	/* GP_IO_SEL2 */
	outl(0x00010035, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL */
}

int spd_read_byte(unsigned device, unsigned address)
{
	extern int smbus_read_byte(u16, u16);
	return smbus_read_byte(device, address);
}

/* Usually system firmware turns off system memory clock signals to 
 * unused SO-DIMM slots to reduce EMI and power consumption.
 * However, the Kontron 986LCD-M does not like unused clock signals to
 * be disabled. If other similar mainboard occur, it would make sense
 * to make this an entry in the sysinfo structure, and pre-initialize that
 * structure in the mainboard's auto.c main() function. For now a
 * #define will do.
 */
#define OVERRIDE_CLOCK_DISABLE 1
#if 0
#include "northbridge/intel/i945/raminit.h"
#include "northbridge/intel/i945/raminit.c"
#include "northbridge/intel/i945/reset_test.c"
#include "northbridge/intel/i945/errata.c"
#include "debug.c"
#endif
static void ich7_enable_lpc(void)
{
	// Enable Serial IRQ
	pci_conf1_write_config8(PCI_BDF(0, 0x1f, 0), 0x64, 0xd0);
	// Set COM1/COM2 decode range
	pci_conf1_write_config16(PCI_BDF(0, 0x1f, 0), 0x80, 0x0010);
	// Enable COM1/COM2/KBD/SuperIO1+2
	pci_conf1_write_config16(PCI_BDF(0, 0x1f, 0), 0x82, 0x340b);
	// Enable HWM at 0xa00
	pci_conf1_write_config16(PCI_BDF(0, 0x1f, 0), 0x84, 0x0a01);
	// COM3 decode
	pci_conf1_write_config32(PCI_BDF(0, 0x1f, 0), 0x88, 0x000403e9);
	// COM4 decode
	pci_conf1_write_config32(PCI_BDF(0, 0x1f, 0), 0x8c, 0x000402e9);
	// io 0x300 decode 
	pci_conf1_write_config32(PCI_BDF(0, 0x1f, 0), 0x90, 0x00000301);
}


/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static void early_superio_config_w83627thg(void)
{
	u16 port;
	u8 ldn;
	
	port = 0x2e;
	ldn = W83627THG_SP1;
	rawpnp_enter_ext_func_mode(port);
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x3f8);
	rawpnp_write_config(port, PNP_IDX_IRQ0, 4);
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_SP2;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x2f8);
	rawpnp_write_config(port, PNP_IDX_IRQ0, 3);
	// rawpnp_write_config(dev, 0xf1, 4); // IRMODE0
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_KBC;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x60);
	rawpnp_set_iobase(port, PNP_IDX_IO1, 0x64);
	// rawpnp_write_config(port, 0xf0, 0x82);
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_GAME_MIDI_GPIO1;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_write_config(port, 0xf5, 0xff); // invert all GPIOs
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_GPIO2;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 1); // Just enable it

	ldn = W83627THG_GPIO3;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_write_config(port, 0xf0, 0xfb); // GPIO bit 2 is output
	rawpnp_write_config(port, 0xf1, 0x00); // GPIO bit 2 is 0
	rawpnp_write_config(port, 0x30, 0x03); // Enable GPIO3+4. rawpnp_set_enable is not sufficient

	ldn = W83627THG_FDC;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);

	ldn = W83627THG_PP;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);

	rawpnp_exit_ext_func_mode(port);

	port = 0x2e;
	ldn = W83627THG_SP1;
	rawpnp_enter_ext_func_mode(port);

	rawpnp_set_logical_device(port, ldn); // Set COM3 to sane non-conflicting values
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x3e8);
	rawpnp_write_config(port, PNP_IDX_IRQ0, 11);
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_SP2; 
	rawpnp_set_logical_device(port, ldn); // Set COM4 to sane non-conflicting values
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x2e8);
	rawpnp_write_config(port, PNP_IDX_IRQ0, 10);
	rawpnp_set_enable(port, 1);

	ldn = W83627THG_FDC;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);

	ldn = W83627THG_PP;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);

	ldn = W83627THG_KBC;
	rawpnp_set_logical_device(port, ldn);
	rawpnp_set_enable(port, 0);
	rawpnp_set_iobase(port, PNP_IDX_IO0, 0x00);
	rawpnp_set_iobase(port, PNP_IDX_IO1, 0x00);

	rawpnp_exit_ext_func_mode(port);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;
	//RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	/* Device 1d interrupt pin register */
	RCBA32(0x310c) = 0x00214321;

	/* dev irq route register */
	RCBA16(0x3140) = 0x0132;
	RCBA16(0x3142) = 0x3241;
	RCBA16(0x3144) = 0x0237;
	RCBA16(0x3146) = 0x3210;
	RCBA16(0x3148) = 0x3210;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable unused devices */
	RCBA32(0x3418) = 0x000e0063;

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;
}

static void early_ich7_init(void)
{
	u8 reg8;
	u32 reg32;

	// program secondary mlt XXX byte?
	pci_conf1_write_config8(PCI_BDF(0, 0x1e, 0), 0x1b, 0x20);

	// reset rtc power status
	reg8 = pci_conf1_read_config8(PCI_BDF(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_conf1_write_config8(PCI_BDF(0, 0x1f, 0), 0xa4, reg8);

	// usb transient disconnect
	reg8 = pci_conf1_read_config8(PCI_BDF(0, 0x1f, 0), 0xad);
	reg8 |= (3 << 0);
	pci_conf1_write_config8(PCI_BDF(0, 0x1f, 0), 0xad, reg8);

	reg32 = pci_conf1_read_config32(PCI_BDF(0, 0x1d, 7), 0xfc);
	reg32 |= (1 << 29) | (1 << 17);
	pci_conf1_write_config32(PCI_BDF(0, 0x1d, 7), 0xfc, reg32);

	reg32 = pci_conf1_read_config32(PCI_BDF(0, 0x1d, 7), 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_conf1_write_config32(PCI_BDF(0, 0x1d, 7), 0xdc, reg32);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	reg32 = RCBA32(0x3410);
	reg32 |= (1 << 6);
	RCBA32(0x3410) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(0x3418) |= (1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	// next step only on ich7m b0 and later:
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}
void hardware_stage1(void)
{
	void 	early_superio_config_w83627thg(void);
	void 	ich7_enable_lpc(void);
	int boot_mode = 0;

	enable_lapic();

	ich7_enable_lpc();
	early_superio_config_w83627thg();

	/* Set up the console */
#warning need to know how to call uart8250_init
//	uart8250_init();
	console_init();

}

void mainboard_pre_payload(void)
{
	banner(BIOS_DEBUG, "mainboard_pre_payload: done");
}
