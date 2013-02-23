/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef SB800_SMBUS_H
#define SB800_SMBUS_H

//#include <stdint.h>

#define SMBHSTSTAT 0x0
#define SMBSLVSTAT 0x1
#define SMBHSTCTRL 0x2
#define SMBHSTCMD  0x3
#define SMBHSTADDR 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBHSTBLKDAT 0x7

#define SMBSLVCTRL 0x8
#define SMBSLVCMD_SHADOW 0x9
#define SMBSLVEVT 0xa
#define SMBSLVDAT 0xc

#define AX_INDXC  0
#define AX_INDXP  2
#define AXCFG     4
#define ABCFG     6
#define RC_INDXC  1
#define RC_INDXP  3

#define AB_INDX   0xCD8
#define AB_DATA   (AB_INDX+4)

/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100*1000*10)

#define abcfg_reg(reg, mask, val)	\
	alink_ab_indx((ABCFG), (reg), (mask), (val))
#define axcfg_reg(reg, mask, val)	\
	alink_ab_indx((AXCFG), (reg), (mask), (val))
#define axindxc_reg(reg, mask, val)	\
	alink_ax_indx((AX_INDXC), (reg), (mask), (val))
#define axindxp_reg(reg, mask, val)		\
	alink_ax_indx((AX_INDXP), (reg), (mask), (val))
#define rcindxc_reg(reg, port, mask, val)	\
	alink_rc_indx((RC_INDXC), (reg), (port), (mask), (val))
#define rcindxp_reg(reg, port, mask, val)	\
	alink_rc_indx((RC_INDXP), (reg), (port), (mask), (val))

int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address, u8 val);
int do_smbus_recv_byte(u32 smbus_io_base, u32 device);
int do_smbus_send_byte(u32 smbus_io_base, u32 device, u8 val);
void alink_rc_indx(u32 reg_space, u32 reg_addr, u32 port, u32 mask, u32 val);
void alink_ab_indx(u32 reg_space, u32 reg_addr, u32 mask, u32 val);
void alink_ax_indx(u32 space /*c or p? */ , u32 axindc, u32 mask, u32 val);


#endif
