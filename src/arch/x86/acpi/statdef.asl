/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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


/* Status and notification definitions */

#define STA_MISSING			    0x00
#define STA_PRESENT			    0x01
#define	STA_ENABLED			    0x03
#define STA_DISABLED		    0x09
#define	STA_INVISIBLE		    0x0B
#define	STA_UNAVAILABLE		    0x0D
#define	STA_VISIBLE			    0x0F

/* SMBus status codes */
#define SMB_OK                  0x00
#define SMB_UnknownFail         0x07
#define SMB_DevAddrNAK          0x10
#define SMB_DeviceError         0x11
#define SMB_DevCmdDenied        0x12
#define SMB_UnknownErr          0x13
#define SMB_DevAccDenied        0x17
#define SMB_Timeout             0x18
#define SMB_HstUnsuppProtocol   0x19
#define SMB_Busy                0x1A
#define SMB_PktChkError         0x1F

/* Device Object Notification Values */
#define	NOTIFY_BUS_CHECK		0x00
#define	NOTIFY_DEVICE_CHECK		0x01
#define	NOTIFY_DEVICE_WAKE		0x02
#define	NOTIFY_EJECT_REQUEST	0x03
#define	NOTIFY_DEVICE_CHECK_JR	0x04
#define	NOTIFY_FREQUENCY_ERROR	0x05
#define	NOTIFY_BUS_MODE			0x06
#define	NOTIFY_POWER_FAULT		0x07
#define	NOTIFY_CAPABILITIES		0x08
#define	NOTIFY_PLD_CHECK		0x09
#define	NOTIFY_SLIT_UPDATE		0x0B

/* Battery Device Notification Values */
#define	NOTIFY_BAT_STATUSCHG	0x80
#define	NOTIFY_BAT_INFOCHG  	0x81
#define	NOTIFY_BAT_MAINTDATA    0x82

/* Power Source Object Notification Values */
#define	NOTIFY_PWR_STATUSCHG	0x80

/* Thermal Zone Object Notification Values */
#define	NOTIFY_TZ_STATUSCHG	    0x80
#define	NOTIFY_TZ_TRIPPTCHG	    0x81
#define	NOTIFY_TZ_DEVLISTCHG	0x82
#define	NOTIFY_TZ_RELTBLCHG 	0x83

/* Power Button Notification Values */
#define	NOTIFY_POWER_BUTTON		0x80

/* Sleep Button Notification Values */
#define	NOTIFY_SLEEP_BUTTON		0x80

/* Lid Notification Values */
#define	NOTIFY_LID_STATUSCHG	0x80

/* Processor Device Notification Values */
#define	NOTIFY_CPU_PPCCHG   	0x80
#define	NOTIFY_CPU_CSTATECHG  	0x81
#define	NOTIFY_CPU_THROTLCHG    0x82

/* User Presence Device Notification Values */
#define	NOTIFY_USR_PRESNCECHG	0x80

/* Battery Device Notification Values */
#define	NOTIFY_ALS_ILLUMCHG 	0x80
#define	NOTIFY_ALS_COLORTMPCHG 	0x81
#define	NOTIFY_ALS_RESPCHG      0x82


