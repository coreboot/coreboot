/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __DEVICE_PNP_OPS_H__
#define __DEVICE_PNP_OPS_H__

#include <stdint.h>
#include <arch/io.h>
#include <device/pnp_type.h>

#if ENV_PNP_SIMPLE_DEVICE

static __always_inline void pnp_write_config(
	pnp_devfn_t dev, uint8_t reg, uint8_t value)
{
	unsigned int port = dev >> 8;
	outb(reg, port);
	outb(value, port + 1);
}

static __always_inline uint8_t pnp_read_config(
	pnp_devfn_t dev, uint8_t reg)
{
	unsigned int port = dev >> 8;
	outb(reg, port);
	return inb(port + 1);
}

static __always_inline
void pnp_set_logical_device(pnp_devfn_t dev)
{
	unsigned int device = dev & 0xff;
	pnp_write_config(dev, 0x07, device);
}

static __always_inline
void pnp_set_enable(pnp_devfn_t dev, int enable)
{
	pnp_write_config(dev, 0x30, enable?0x1:0x0);
}

static __always_inline
int pnp_read_enable(pnp_devfn_t dev)
{
	return !!pnp_read_config(dev, 0x30);
}

static __always_inline
void pnp_set_iobase(pnp_devfn_t dev, unsigned int index, unsigned int iobase)
{
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

static __always_inline
uint16_t pnp_read_iobase(pnp_devfn_t dev, unsigned int index)
{
	return ((uint16_t)(pnp_read_config(dev, index)) << 8)
		| pnp_read_config(dev, index + 1);
}

static __always_inline
void pnp_set_irq(pnp_devfn_t dev, unsigned int index, unsigned int irq)
{
	pnp_write_config(dev, index, irq);
}

static __always_inline
void pnp_set_drq(pnp_devfn_t dev, unsigned int index, unsigned int drq)
{
	pnp_write_config(dev, index, drq & 0xff);
}

#endif

#endif
