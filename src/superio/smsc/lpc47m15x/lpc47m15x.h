/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef SUPERIO_SMSC_LPC47M15X_LPC47M15X_H
#define SUPERIO_SMSC_LPC47M15X_LPC47M15X_H

#define LPC47M15X_FDC              0   /* Floppy */
#define LPC47M15X_PP               3   /* Parallel Port */
#define LPC47M15X_SP1              4   /* Com1 */
#define LPC47M15X_SP2              5   /* Com2 */
#define LPC47M15X_KBC              7   /* Keyboard & Mouse */
#define LPC47M15X_GAME             9   /* GAME */
#define LPC47M15X_PME             10   /* PME  reg*/
#define LPC47M15X_MPU             11   /* MPE -- who knows --   reg*/

#define LPC47M15X2_MAX_CONFIG_REGISTER	0x5F

#endif
