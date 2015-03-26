/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <superio/ite/common/ite.h>
#include "it8718f.h"

/*
 * GIGABYTE uses a special Super I/O register to protect its Dual BIOS
 * mechanism. It lives in the GPIO LDN. However, register 0xEF is not
 * mentioned in the IT8718F datasheet so just hardcode it to 0x7E for now.
 */
void it8718f_disable_reboot(pnp_devfn_t dev)
{
	ite_reg_write(dev, 0xEF, 0x7E);
}
