/*
 * This file is part of the coreboot project.
 *
 * File taken from the Linux xgifb driver (v3.18.5)
 * Coreboot-specific includes added at top
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

#include "xgi_coreboot.h"

#include "vgatypes.h"
#include "vb_util.h"

void xgifb_reg_set(unsigned long port, u8 index, u8 data)
{
	outb(index, port);
	outb(data, port + 1);
}

u8 xgifb_reg_get(unsigned long port, u8 index)
{
	u8 data;

	outb(index, port);
	data = inb(port + 1);
	return data;
}

void xgifb_reg_and_or(unsigned long port, u8 index,
		unsigned data_and, unsigned data_or)
{
	u8 temp;

	temp = xgifb_reg_get(port, index); /* XGINew_Part1Port index 02 */
	temp = (temp & data_and) | data_or;
	xgifb_reg_set(port, index, temp);
}

void xgifb_reg_and(unsigned long port, u8 index, unsigned data_and)
{
	u8 temp;

	temp = xgifb_reg_get(port, index); /* XGINew_Part1Port index 02 */
	temp &= data_and;
	xgifb_reg_set(port, index, temp);
}

void xgifb_reg_or(unsigned long port, u8 index, unsigned data_or)
{
	u8 temp;

	temp = xgifb_reg_get(port, index); /* XGINew_Part1Port index 02 */
	temp |= data_or;
	xgifb_reg_set(port, index, temp);
}
