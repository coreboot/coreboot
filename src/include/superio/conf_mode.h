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

#ifndef DEVICE_PNP_CONF_MODE_H
#define DEVICE_PNP_CONF_MODE_H

#include <device/device.h>
#include <device/pnp.h>

/* Common enter/exit implementations */
void pnp_enter_conf_mode_55(struct device *dev);
void pnp_enter_conf_mode_6767(struct device *dev);
void pnp_enter_conf_mode_7777(struct device *dev);
void pnp_enter_conf_mode_8787(struct device *dev);
void pnp_enter_conf_mode_a0a0(struct device *dev);
void pnp_exit_conf_mode_aa(struct device *dev);
void pnp_enter_conf_mode_870155aa(struct device *dev);
void pnp_exit_conf_mode_0202(struct device *dev);

extern const struct pnp_mode_ops pnp_conf_mode_55_aa;
extern const struct pnp_mode_ops pnp_conf_mode_6767_aa;
extern const struct pnp_mode_ops pnp_conf_mode_7777_aa;
extern const struct pnp_mode_ops pnp_conf_mode_8787_aa;
extern const struct pnp_mode_ops pnp_conf_mode_a0a0_aa;
extern const struct pnp_mode_ops pnp_conf_mode_870155_aa;

#endif /* DEVICE_PNP_CONF_MODE_H */
