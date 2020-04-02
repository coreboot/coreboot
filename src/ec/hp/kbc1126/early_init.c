/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include "ec.h"

void kbc1126_enter_conf(void)
{
	outb(0x55, 0x2e);
	outb(0x22, 0x2e);
	outb(0x00, 0x2f);
	outb(0x23, 0x2e);
	outb(0x00, 0x2f);
	outb(0x24, 0x2e);
	outb(0x84, 0x2f);
}

void kbc1126_exit_conf(void)
{
	outb(0xaa, 0x2e);
	/* one more time in PlatformStage1 of vendor firmware */
	outb(0xaa, 0x2e);

	outb(0x83, 0x200);
	outb(0x00, 0x201);
	inb(0x201);
}

void kbc1126_mailbox_init(void)
{
	pnp_devfn_t dev = PNP_DEV(0x2e, KBC1100_MAILBOX);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x200);
	pnp_set_enable(dev, 1);
}

void kbc1126_kbc_init(void)
{
	pnp_devfn_t dev = PNP_DEV(0x2e, KBC1100_KBC);
	pnp_set_logical_device(dev);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 0x1);
	pnp_set_irq(dev, PNP_IDX_IRQ1, 0xc);
	pnp_set_enable(dev, 1);
}

void kbc1126_ec_init(void)
{
	pnp_devfn_t dev = PNP_DEV(0x2e, KBC1100_EC0);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x62);
	pnp_set_enable(dev, 1);
}

void kbc1126_com1_init(void)
{
	pnp_devfn_t dev = PNP_DEV(0x2e, SMSCSUPERIO_SP1);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x280);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 0x6);
	pnp_set_enable(dev, 1);
}

void kbc1126_pm1_init(void)
{
	pnp_devfn_t dev = PNP_DEV(0x2e, KBC1100_PM1);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x220);
	pnp_set_enable(dev, 1);
}

/*
 * This code is found in PEI module F65354B9-1FF0-46D7-A5F7-0926CB238048
 * of the OEM firmware.
 *
 * For mainboards without a Super I/O at 0x4e, without this code, superiotool
 * will detect an Infineon Super I/O at 0x4e.
 */

void kbc1126_disable4e(void)
{
	outb(0x55, 0x4e);

	outb(0x26, 0x4e);
	outb(0x00, 0x4f);
	outb(0x27, 0x4e);
	outb(0xfe, 0x4f);
	outb(0x60, 0x4e);
	outb(0xfe, 0x4f);
	outb(0x61, 0x4e);
	outb(0x80, 0x4f);
	outb(0x30, 0x4e);
	outb(0x01, 0x4f);

	outb(0xaa, 0x4e);
}
