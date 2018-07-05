/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Damien Zammit <damien@zamaudio.com>
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

#define SUPERIO_BASE 0x2e
#define SUPERIO_DEV PNP_DEV(SUPERIO_BASE, 0)
#define SUPERIO_GPIO PNP_DEV(SUPERIO_BASE, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(SUPERIO_BASE, 0x01)

#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <superio/ite/it8728f/it8728f.h>
#include <superio/ite/common/ite.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>

static void it8728f_b75md3h_disable_reboot(pnp_devfn_t dev)
{
	/* GPIO SIO settings */
	ite_reg_write(dev, 0xEF, 0x7E); // magic

	ite_reg_write(dev, 0x25, 0x40); // gpio pin function -> gp16
	ite_reg_write(dev, 0x27, 0x10); // gpio pin function -> gp34
	ite_reg_write(dev, 0x2c, 0x80); // smbus isolation on parallel port
	ite_reg_write(dev, 0x62, 0x0a); // simple iobase 0xa00
	ite_reg_write(dev, 0x72, 0x20); // watchdog timeout clear!
	ite_reg_write(dev, 0x73, 0x00); // watchdog timeout clear!
	ite_reg_write(dev, 0xcb, 0x00); // simple io set4 direction -> in
	ite_reg_write(dev, 0xe9, 0x27); // bus select disable
	ite_reg_write(dev, 0xf0, 0x10); // ?
	ite_reg_write(dev, 0xf1, 0x42); // ?
	ite_reg_write(dev, 0xf6, 0x1c); // hardware monitor alert beep -> gp36(pin12)

	/* EC SIO settings */
	ite_reg_write(IT8728F_EC, 0xf1, 0xc0);
	ite_reg_write(IT8728F_EC, 0xf6, 0xf0);
	ite_reg_write(IT8728F_EC, 0xf9, 0x48);
	ite_reg_write(IT8728F_EC, 0x60, 0x0a);
	ite_reg_write(IT8728F_EC, 0x61, 0x30);
	ite_reg_write(IT8728F_EC, 0x62, 0x0a);
	ite_reg_write(IT8728F_EC, 0x63, 0x20);
	ite_reg_write(IT8728F_EC, 0x30, 0x01);
}

void mainboard_rcba_config(void)
{
/*
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE + 4, 0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44, 0x80);

	outw (inw (DEFAULT_PMBASE | 0x003c) | 2, DEFAULT_PMBASE | 0x003c);

	RCBA32(0x3500) = 0x2000035f;
	RCBA32(0x3504) = 0x2000035f;
	RCBA32(0x3508) = 0x2000035f;
	RCBA32(0x350c) = 0x2000035f;
	RCBA32(0x3510) = 0x2000035f;
	RCBA32(0x3514) = 0x2000035f;
	RCBA32(0x3518) = 0x2000035f;
	RCBA32(0x351c) = 0x2000035f;
	RCBA32(0x3520) = 0x2000035f;
	RCBA32(0x3524) = 0x2000035f;
	RCBA32(0x3528) = 0x2000035f;
	RCBA32(0x352c) = 0x2000035f;
	RCBA32(0x3530) = 0x2000035f;
	RCBA32(0x3534) = 0x2000035f;
	RCBA32(0x3560) = 0x024c8001;
	RCBA32(0x3564) = 0x000024a3;
	RCBA32(0x3568) = 0x00040002;
	RCBA32(0x356c) = 0x01000050;
	RCBA32(0x3570) = 0x02000662;
	RCBA32(0x3574) = 0x18000f9f;
	RCBA32(0x3578) = 0x1800ff4f;
	RCBA32(0x357c) = 0x0001d530;
	RCBA32(0x35a0) = 0xc0300c03;
	RCBA32(0x35a4) = 0x00241803;

	pci_write_config32 (PCI_DEV (0, 0x14, 0), 0xe4, 0x00000000);

	outw (0x0000, DEFAULT_PMBASE | 0x003c);

	RCBA32(0x2240) = 0x00330e71;
	RCBA32(0x2244) = 0x003f0eb1;
	RCBA32(0x2248) = 0x002102cd;
	RCBA32(0x224c) = 0x00f60000;
	RCBA32(0x2250) = 0x00020000;
	RCBA32(0x2254) = 0x00e3004c;
	RCBA32(0x2258) = 0x00e20bef;
	RCBA32(0x2260) = 0x003304ed;
	RCBA32(0x2278) = 0x001107c1;
	RCBA32(0x227c) = 0x001d07e9;
	RCBA32(0x2280) = 0x00e20000;
	RCBA32(0x2284) = 0x00ee0000;
	RCBA32(0x2288) = 0x005b05d3;
	RCBA32(0x2318) = 0x04b8ff2e;
	RCBA32(0x231c) = 0x03930f2e;
//	RCBA32(0x3418) = 0x1fee1fe1;
	RCBA32(0x3808) = 0x005044a3;
	RCBA32(0x3810) = 0x52410000;
	RCBA32(0x3814) = 0x0000008a;
	RCBA32(0x3818) = 0x00000006;
	RCBA32(0x381c) = 0x0000072e;
	RCBA32(0x3820) = 0x0000000a;
	RCBA32(0x3824) = 0x00000123;
	RCBA32(0x3828) = 0x00000009;
	RCBA32(0x382c) = 0x00000001;
	RCBA32(0x3834) = 0x0000061a;
	RCBA32(0x3838) = 0x00000003;
	RCBA32(0x383c) = 0x00000a76;
	RCBA32(0x3840) = 0x00000004;
	RCBA32(0x3844) = 0x0000e5e4;
	RCBA32(0x3848) = 0x0000000e;
*/
	/* Enable HECI */
	RCBA32(FD2) &= ~0x2;
}

void pch_enable_lpc(void)
{
	/*
	 * Enable:
	 *  EC Decode Range PortA30/A20
	 *  SuperIO Port2E/2F
	 *  PS/2 Keyboard/Mouse Port60/64
	 *  FDD Port3F0h-3F5h and Port3F7h
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN | MC_LPC_EN |
			CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x3c0a01);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, ETR3, 0x10000);

	/* Initialize SuperIO */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	it8728f_b75md3h_disable_reboot(SUPERIO_GPIO);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 5, 0 },
	{ 1, 5, 0 },
	{ 1, 5, 1 },
	{ 1, 5, 1 },
	{ 1, 5, 2 },
	{ 1, 5, 2 },
	{ 1, 5, 3 },
	{ 1, 5, 3 },
	{ 1, 5, 4 },
	{ 1, 5, 4 },
	{ 1, 5, 6 },
	{ 1, 5, 5 },
	{ 1, 5, 5 },
	{ 1, 5, 6 },
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	read_spd (&spd[0], 0x50, id_only);
	read_spd (&spd[1], 0x51, id_only);
	read_spd (&spd[2], 0x52, id_only);
	read_spd (&spd[3], 0x53, id_only);
}

#if 0
static void dmi_config(void)
{
	DMIBAR32(0x0218) = 0x06aa0b0c;
	DMIBAR32(0x021c) = 0x0b0d0b0d;
	DMIBAR32(0x0300) = 0x0011028d;
	DMIBAR32(0x0304) = 0x002102cd;
	DMIBAR32(0x030c) = 0x007d004b;
	DMIBAR32(0x0310) = 0x007e004c;
	DMIBAR32(0x0318) = 0x002304ad;
	DMIBAR32(0x031c) = 0x003304ed;
	DMIBAR32(0x03b8) = 0x005c05a4;
	DMIBAR32(0x03bc) = 0x006c05e4;
	DMIBAR32(0x0530) = 0x41d3b000;
	DMIBAR32(0x0534) = 0x00019f80;
	DMIBAR32(0x0ba4) = 0x0000000d;
	DMIBAR32(0x0d80) = 0x1c9cfc0b;
	DMIBAR32(0x0e1c) = 0x20000000;
	DMIBAR32(0x0e2c) = 0x20000000;
}
#endif

void mainboard_early_init(int s3resume) {
}

void mainboard_config_superio(void)
{
}
