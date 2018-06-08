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
 */

#ifndef __STONEYRIDGE_SMBUS_H__
#define __STONEYRIDGE_SMBUS_H__

#include <stdint.h>
#include <soc/iomap.h>

#define SMBHSTSTAT			0x0
#define   SMBHST_STAT_FAILED		0x10
#define   SMBHST_STAT_COLLISION		0x08
#define   SMBHST_STAT_ERROR		0x04
#define   SMBHST_STAT_INTERRUPT		0x02
#define   SMBHST_STAT_BUSY		0x01
#define   SMBHST_STAT_CLEAR		0xff
#define   SMBHST_STAT_NOERROR		0x02
#define   SMBHST_STAT_VAL_BITS		0x1f
#define   SMBHST_STAT_ERROR_BITS	0x1c

#define SMBSLVSTAT			0x1
#define   SMBSLV_STAT_ALERT		0x20
#define   SMBSLV_STAT_SHADOW2		0x10
#define   SMBSLV_STAT_SHADOW1		0x08
#define   SMBSLV_STAT_SLV_STS		0x04
#define   SMBSLV_STAT_SLV_INIT		0x02
#define   SMBSLV_STAT_SLV_BUSY		0x01
#define   SMBSLV_STAT_CLEAR		0x1f

#define SMBHSTCTRL			0x2
#define   SMBHST_CTRL_RST		0x80
#define   SMBHST_CTRL_STRT		0x40
#define   SMBHST_CTRL_QCK_RW		0x00
#define   SMBHST_CTRL_BTE_RW		0x04
#define   SMBHST_CTRL_BDT_RW		0x08
#define   SMBHST_CTRL_WDT_RW		0x0c
#define   SMBHST_CTRL_BLK_RW		0x14
#define   SMBHST_CTRL_MODE_BITS		0x1c
#define   SMBHST_CTRL_KILL		0x02
#define   SMBHST_CTRL_IEN		0x01

#define SMBHSTCMD			0x3
#define SMBHSTADDR			0x4
#define SMBHSTDAT0			0x5
#define SMBHSTDAT1			0x6
#define SMBHSTBLKDAT			0x7
#define SMBSLVCTRL			0x8
#define SMBSLVCMD_SHADOW		0x9
#define SMBSLVEVT			0xa
#define SMBSLVDAT			0xc
#define SMBTIMING			0xe

#define SMB_ASF_IO_BASE			0x01
#define SMB_SPEED_400KHZ		(66000000 / (400000 * 4))

#define AX_INDXC			0
#define AX_INDXP			2
#define AXCFG				4
#define ABCFG				6
#define RC_INDXC			1
#define RC_INDXP			3

/*
 * Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100 * 1000 * 10)

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

int do_smbus_read_byte(u16 smbus_io_base, u8 device, u8 address);
int do_smbus_write_byte(u16 smbus_io_base, u8 device, u8 address, u8 val);
int do_smbus_recv_byte(u16 smbus_io_base, u8 device);
int do_smbus_send_byte(u16 smbus_io_base, u8 device, u8 val);
void alink_rc_indx(u32 reg_space, u32 reg_addr, u32 port, u32 mask, u32 val);
void alink_ab_indx(u32 reg_space, u32 reg_addr, u32 mask, u32 val);
void alink_ax_indx(u32 space /*c or p? */, u32 axindc, u32 mask, u32 val);

#endif /* __STONEYRIDGE_SMBUS_H__ */
