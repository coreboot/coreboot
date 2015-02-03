/*
 * This file is part of the coreboot project.
 *
 * File taken verbatim from the Linux xgifb driver (v3.18.5)
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

#ifndef _VBUTIL_
#define _VBUTIL_
extern void xgifb_reg_set(unsigned long, u8, u8);
extern u8 xgifb_reg_get(unsigned long, u8);
extern void xgifb_reg_or(unsigned long, u8, unsigned);
extern void xgifb_reg_and(unsigned long, u8, unsigned);
extern void xgifb_reg_and_or(unsigned long, u8, unsigned, unsigned);
#endif

