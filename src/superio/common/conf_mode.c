/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <device/device.h>
#include <superio/conf_mode.h>

/* Common enter/exit implementations */

void pnp_enter_conf_mode_55(struct device *dev)
{
	outb(0x55, dev->path.pnp.port);
}

void pnp_enter_conf_mode_6767(struct device *dev)
{
	outb(0x67, dev->path.pnp.port);
	outb(0x67, dev->path.pnp.port);
}

void pnp_enter_conf_mode_7777(struct device *dev)
{
	outb(0x77, dev->path.pnp.port);
	outb(0x77, dev->path.pnp.port);
}

void pnp_enter_conf_mode_8787(struct device *dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

void pnp_enter_conf_mode_a0a0(struct device *dev)
{
	outb(0xa0, dev->path.pnp.port);
	outb(0xa0, dev->path.pnp.port);
}

void pnp_exit_conf_mode_aa(struct device *dev)
{
	outb(0xaa, dev->path.pnp.port);
}

void pnp_enter_conf_mode_870155aa(struct device *dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x01, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);

	if (dev->path.pnp.port == 0x4e)
		outb(0xaa, dev->path.pnp.port);
	else
		outb(0x55, dev->path.pnp.port);
}

void pnp_exit_conf_mode_0202(struct device *dev)
{
	outb(0x02, dev->path.pnp.port);
	outb(0x02, dev->path.pnp.port + 1);
}


const struct pnp_mode_ops pnp_conf_mode_55_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_55,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
};

const struct pnp_mode_ops pnp_conf_mode_6767_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_6767,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
};

const struct pnp_mode_ops pnp_conf_mode_7777_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_7777,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
};

const struct pnp_mode_ops pnp_conf_mode_8787_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_8787,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
};

const struct pnp_mode_ops pnp_conf_mode_a0a0_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_a0a0,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
};

const struct pnp_mode_ops pnp_conf_mode_870155_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_870155aa,
	.exit_conf_mode  = pnp_exit_conf_mode_0202,
};
