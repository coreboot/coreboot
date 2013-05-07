/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef _NE2K_H__
#define _NE2K_H__
void ne2k_append_data(unsigned char *d, int len, unsigned int base);
int ne2k_init(unsigned int eth_nic_base);
void ne2k_transmit(unsigned int eth_nic_base);
#endif /* _NE2K_H */
