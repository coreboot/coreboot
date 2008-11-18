/*
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 coresystems GmbH
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
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef DEVICE_PNP_H
#define DEVICE_PNP_H

#include <types.h>
#include <device/device.h>

/* Very low-level PNP functions, mostly intended for stage 0 or 1 */
void rawpnp_enter_ext_func_mode(u16 port);
void rawpnp_exit_ext_func_mode(u16 port);
void rawpnp_write_config(u16 port, u8 reg, u8 value);
u8 rawpnp_read_config(u16 port, u8 reg);
void rawpnp_set_logical_device(u16 port, u8 ldn);
void rawpnp_set_enable(u16 port, int enable);
void rawpnp_set_iobase(u16 port, u8 index, u16 iobase);

/* Primitive pnp resource manipulation */
void    pnp_write_config(struct device * dev, u8 reg, u8 value);
u8 pnp_read_config(struct device * dev, u8 reg);
void    pnp_set_logical_device(struct device * dev);
void    pnp_set_enable(struct device * dev, int enable);
int     pnp_read_enable(struct device * dev);
void    pnp_set_iobase(struct device * dev, unsigned index, unsigned iobase);
void    pnp_set_irq(struct device * dev, unsigned index, unsigned irq);
void    pnp_set_drq(struct device * dev, unsigned index, unsigned drq);

/* PNP device operations */
void pnp_read_resources(struct device * dev);
void pnp_set_resources(struct device * dev);
void pnp_enable_resources(struct device * dev);
void pnp_enable(struct device * dev);

extern struct device_operations pnp_ops;

/* PNP helper operations */

struct io_info {
	unsigned mask, set, val;
};

struct pnp_info {
	struct device_operations *ops;
	unsigned function;
	unsigned enable;
	unsigned flags;
#define PNP_IO0  0x01
#define PNP_IO1  0x02
#define PNP_IO2  0x04
#define PNP_IO3  0x08
#define PNP_IRQ0 0x10
#define PNP_IRQ1 0x20
#define PNP_DRQ0 0x40
#define PNP_DRQ1 0x80
	struct io_info io0, io1, io2, io3, irq0, irq1, drq0, drq1;
};
struct resource *pnp_get_resource(struct device * dev, unsigned index);
void pnp_enable_devices(struct device *dev, struct device_operations *ops,
	unsigned functions, struct pnp_info *info);

#define PNP_IDX_IO0  0x60
#define PNP_IDX_IO1  0x62
#define PNP_IDX_IO2  0x64
#define PNP_IDX_IO3  0x66
#define PNP_IDX_IRQ0 0x70
#define PNP_IDX_IRQ1 0x72
#define PNP_IDX_DRQ0 0x74
#define PNP_IDX_DRQ1 0x75

/* PNP functions used to enter/exit PNP config mode */
void pnp_enter_ite_it8661f(struct device *dev);
void pnp_enter_ite_it8671f(struct device *dev);
void pnp_enter_ite(struct device *dev);
void pnp_enter_ite_it8761e(struct device *dev);
void pnp_enter_ite_it8228e(struct device *dev);
void pnp_enter_8787(struct device *dev);
void pnp_enter_ali(struct device *dev);
void pnp_enter_smsc(struct device *dev);
void pnp_enter_88(struct device *dev);
void pnp_enter_89(struct device *dev);
void pnp_enter_8686(struct device *dev);
void pnp_exit_aa(struct device *dev);
void pnp_exit_ite(struct device *dev);
void pnp_exit_ali(struct device *dev);

#endif /* DEVICE_PNP_H */
