/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef FSP_VALUES_H
#define FSP_VALUES_H

#ifndef FSP_DEBUG_LEVEL
# define FSP_DEBUG_LEVEL	BIOS_SPEW
#endif

#ifndef FSP_INFO_LEVEL
# define FSP_INFO_LEVEL	BIOS_DEBUG
#endif

#define INCREMENT_FOR_DEFAULT(x) (x+1)

#define UPD_DEFAULT		0x00
#define UPD_DISABLE		INCREMENT_FOR_DEFAULT(0)
#define UPD_ENABLE		INCREMENT_FOR_DEFAULT(1)
#define UPD_USE_DEVICETREE	0xff

#define UPD_SPD_ADDR_DEFAULT	UPD_DEFAULT
#define UPD_SPD_ADDR_DISABLED	0xFF

#endif
