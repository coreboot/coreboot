/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_SMBUS_H
#define AMD_BLOCK_SMBUS_H

/* SMBUS MMIO offsets 0xfed80a00 */
#define SMBHSTSTAT			0x0
#define   SMBHST_STAT_FAILED		(1 << 4)
#define   SMBHST_STAT_COLLISION		(1 << 3)
#define   SMBHST_STAT_ERROR		(1 << 2)
#define   SMBHST_STAT_INTERRUPT		(1 << 1)
#define   SMBHST_STAT_BUSY		(1 << 0)
#define   SMBHST_STAT_CLEAR		0xff
#define   SMBHST_STAT_NOERROR		(1 << 1) /* TODO: this one looks odd */
#define   SMBHST_STAT_VAL_BITS		0x1f
#define   SMBHST_STAT_ERROR_BITS	0x1c

#define SMBSLVSTAT			0x1
#define   SMBSLV_STAT_ALERT		(1 << 5)
#define   SMBSLV_STAT_SHADOW2		(1 << 4)
#define   SMBSLV_STAT_SHADOW1		(1 << 3)
#define   SMBSLV_STAT_SLV_STS		(1 << 2)
#define   SMBSLV_STAT_SLV_INIT		(1 << 1)
#define   SMBSLV_STAT_SLV_BUSY		(1 << 0)
#define   SMBSLV_STAT_CLEAR		0x1f

#define SMBHSTCTRL			0x2
#define   SMBHST_CTRL_RST		(1 << 7)
#define   SMBHST_CTRL_STRT		(1 << 6)
#define   SMBHST_CTRL_QCK_RW		(0x0 << 2)
#define   SMBHST_CTRL_BTE_RW		(0x1 << 2)
#define   SMBHST_CTRL_BDT_RW		(0x2 << 2)
#define   SMBHST_CTRL_WDT_RW		(0x3 << 2)
#define   SMBHST_CTRL_BLK_RW		(0x5 << 2)
#define   SMBHST_CTRL_MODE_BITS		(0x7 << 2)
#define   SMBHST_CTRL_KILL		(1 << 1)
#define   SMBHST_CTRL_IEN		(1 << 0)

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

void fch_smbus_init(void);

#endif /* AMD_BLOCK_SMBUS_H */
