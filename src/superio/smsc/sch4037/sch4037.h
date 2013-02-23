/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef SUPERIO_SCH_4037_H
#define SUPERIO_SCH_4037_H


#define SCH4037_FDD              0            /* FDD */
#define SCH4037_LPT              3            /* LPT */
#define SMSCSUPERIO_SP1          4            /* Com1 */
#define SMSCSUPERIO_SP2          5            /* Com2 */
#define SCH4037_RTC              6            /* RTC */
#define SCH4037_KBC              7            /* KBC */
#define SCH4037_HWM              8            /* HWM */
#define SCH4037_RUNTIME          0x0A         /* Runtime */
#define SCH4037_XBUS             0x0B         /* X-BUS */

#endif //SUPERIO_SCH_4037_H
