/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014  Edward O'Callaghan <eocallaghan@alterapraxis.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef DRIVERS_SUPERIO_HWM_H
#define DRIVERS_SUPERIO_HWM_H

#include <device/device.h>

/* Initialization parameters?? */
typedef struct drivers_superio_hwm_config {
	u32 base;
} hwm_config_t;

#endif /* DRIVERS_SUPERIO_HWM_H */
