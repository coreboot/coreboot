/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Denis 'GNUtoo' Carikli <GNUtoo@no-log.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef M4A785T_M_HDA_H
#define M4A785T_M_HDA_H
static CODECENTRY m4a785t_m_codec_vt1708s[] = /* VIA VT1708S */
{
	{0x19, 0x01011012},
	{0x1a, 0x01a19026},
	{0x1b, 0x0181302e},
	{0x1c, 0x01014010},
	{0x1d, 0x0221401f},
	{0x1e, 0x02a19027},
	{0x1f, 0x593311f8},
	{0x20, 0x074411f0},
	{0x21, 0x985601f0},
	{0x22, 0x01016011},
	{0x23, 0x01012014},
	{0xff, 0xffffffff} /* end of table */
};
#endif
