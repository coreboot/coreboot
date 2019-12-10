/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
 * Copyright (C) 2017-2018 Eltan B.V.
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

#include <arch/io.h>
#include <device/device.h>
#include <superio/conf_mode.h>
#include <arch/acpigen.h>

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

void pnp_enter_conf_mode_a5a5(struct device *dev)
{
	outb(0xa5, dev->path.pnp.port);
	outb(0xa5, dev->path.pnp.port);
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
	pnp_write_config(dev, 0x02, (1 << 1));
}

/* Functions for ACPI */
#if CONFIG(HAVE_ACPI_TABLES)
static void pnp_ssdt_enter_conf_mode_55(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0x55);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_enter_conf_mode_6767(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0x67);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x67);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_enter_conf_mode_7777(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0x77);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x77);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_enter_conf_mode_8787(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0x87);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x87);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_enter_conf_mode_a0a0(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0xa0);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0xa0);
	acpigen_emit_namestring(idx);

}

static void pnp_ssdt_enter_conf_mode_a5a5(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0xa5);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0xa5);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_enter_conf_mode_870155aa(struct device *dev,
					      const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0x87);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x01);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x55);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	if (dev->path.pnp.port == 0x4e)
		acpigen_write_byte(0xaa);
	else
		acpigen_write_byte(0x55);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_exit_conf_mode_aa(struct device *dev, const char *idx, const char *data)
{
	acpigen_write_store();
	acpigen_write_byte(0xaa);
	acpigen_emit_namestring(idx);
}

static void pnp_ssdt_exit_conf_mode_0202(struct device *dev, const char *idx, const char *data)
{

	acpigen_write_store();
	acpigen_write_byte(0x02);
	acpigen_emit_namestring(idx);

	acpigen_write_store();
	acpigen_write_byte(0x02);
	acpigen_emit_namestring(data);
}
#endif

const struct pnp_mode_ops pnp_conf_mode_55_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_55,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_55,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_6767_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_6767,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_6767,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_7777_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_7777,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_7777,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_8787_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_8787,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_8787,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_a0a0_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_a0a0,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_a0a0,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_a5a5_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_a5a5,
	.exit_conf_mode  = pnp_exit_conf_mode_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_a5a5,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_aa,
#endif
};

const struct pnp_mode_ops pnp_conf_mode_870155_aa = {
	.enter_conf_mode = pnp_enter_conf_mode_870155aa,
	.exit_conf_mode  = pnp_exit_conf_mode_0202,
#if CONFIG(HAVE_ACPI_TABLES)
	.ssdt_enter_conf_mode = pnp_ssdt_enter_conf_mode_870155aa,
	.ssdt_exit_conf_mode = pnp_ssdt_exit_conf_mode_0202,
#endif
};
