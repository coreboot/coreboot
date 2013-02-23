/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#ifndef SUPERIO_SMSC_FDC37N972_FDC37N972_H
#define SUPERIO_SMSC_FDC37N972_FDC37N972_H

#define FDC37N972_FDC  0x00 /* Floppy */
#define FDC37N972_PP   0x03 /* Parallel port */
#define FDC37N972_SP1  0x04 /* Serial port 1 */
#define FDC37N972_SP2  0x05 /* Serial port 2 */
#define FDC37N972_RTC  0x06 /* Real Time Clock */
#define FDC37N972_KBDC 0x07 /* Keyboard */
#define FDC37N972_EC   0x08 /* Environmental Controller */
#define FDC37N972_MBX  0x09 /* Mailbox register */

#endif
