/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#ifndef SUPERIO_SMSC_LPC47N227_LPC47N227_H
#define SUPERIO_SMSC_LPC47N227_LPC47N227_H

/*
 * Since the LPC47N227 does not have logical devices but a flat configuration
 * space, these are arbitrary, but must match declarations in the mainboard
 * devicetree.cb.
 */
#define LPC47N227_PP               1	/* Parallel Port */
#define LPC47N227_SP1              2	/* COM1 */
#define LPC47N227_SP2              3	/* COM2 */
#define LPC47N227_KBDC             5	/* Keyboard */

#define LPC47N227_MAX_CONFIG_REGISTER	0x39

#endif
