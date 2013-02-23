/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
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

/* Datasheets:
 * Name: VT82C686A "Super South" South Bridge
 * Link: http://www.datasheets.org.uk/datasheet.php?article=3510260
 * Name: VT82C686B "Super South" South Bridge
 * Link: http://www.datasheet4u.com/html/V/T/8/VT82C686B_VIA.pdf.html
 */

/* Super I/0 Configuration Registers. */
/* Any registers not listed here are reserved. */
#define	VT82C686_SIO_DEV_ID	0xe0 /* Super-I/O Device ID */
#define	VT82C686_SIO_REV	0xe1 /* Super-I/O Device Revision */
#define	VT82C686_FS		0xe2 /* Function Select */
#define	VT82C686_FDC		0xe3 /* Floppy */
#define	VT82C686_PP		0xe6 /* Parallel Port */
#define	VT82C686_SP1		0xe7 /* Serial Port 1 */
#define	VT82C686_SP2		0xe8 /* Serial Port 2 */
#define	VT82C686_SP_CFG		0xee /* Serial Port Configuration */
#define	VT82C686_POWER		0xef /* Power Down Control */
#define	VT82C686_PP_CTRL	0xf0 /* Parallel Port Control */
#define	VT82C686_SP_CTRL	0xf1 /* Serial Port Control */
#define	VT82C686_FDC_CFG	0xf6 /* Floppy Controller Configuration */
#define	VT82C686_FDC_DC		0xf8 /* Floppy Drive Control */
#define	VT82C686_GPIO		0xfc /* General Purpose I/O */

/* For reference, used PCI IDs and their names in pci_ids.h: */
/*
PCI_VENDOR_ID_VIA		0x1106
PCI_DEVICE_ID_VIA_82C686	0x0686 // Function 0, PCI Config
PCI_DEVICE_ID_VIA_82C586_1	0x0571 // Function 1, IDE Controller
PCI_DEVICE_ID_VIA_82C586_2	0x3038 // Functions 2 & 3, USB Ports 0-1 & 2-3
PCI_DEVICE_ID_VIA_82C586_3	0x3040 // Possible 2nd USB Controller?
PCI_DEVICE_ID_VIA_82C686_4	0x3057 // Function 4, Power Management
PCI_DEVICE_ID_VIA_82C686_5	0x3058 // Function 5, AC'97 Codec
PCI_DEVICE_ID_VIA_82C686_6	0x3068 // Function 6, MC'97 Codec
*/

