/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef SUPERIO_SMSC_FDC37M60X_FDC37M60X_H
#define SUPERIO_SMSC_FDC37M60X_FDC37M60X_H

/*
 * Datasheet:
 *   - Name: FDC37M60x Enhanced Super I/O Controller with Infrared Support.
 *   - URL: http://www.smsc.com/main/discfdc.html
 *   - PDF: http://www.smsc.com/main/tools/discontinued/37m602.pdf
 *   - Revision: 6/6/97
 */

/* Status: Serial port 1 is tested and works (tested on FDC37M602). */

/* Note: Logical devices 1, 2, 6, and 9 are reserved. */

#define FDC37M60X_FDC  0x00 /* Floppy */
#define FDC37M60X_PP   0x03 /* Parallel port */
#define FDC37M60X_SP1  0x04 /* Com1 */
#define FDC37M60X_SP2  0x05 /* Com2 */
#define FDC37M60X_KBCK 0x07 /* Keyboard */
#define FDC37M60X_AUX  0x08 /* Auxiliary I/O */

#endif
