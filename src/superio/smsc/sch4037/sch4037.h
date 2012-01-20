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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SUPERIO_SCH_4037_H
#define SUPERIO_SCH_4037_H

/* BITS Define */
#ifndef BIT0
#define BIT0        0x0000000000000001ull
#endif
#ifndef BIT1
#define BIT1        0x0000000000000002ull
#endif
#ifndef BIT2
#define BIT2        0x0000000000000004ull
#endif
#ifndef BIT3
#define BIT3        0x0000000000000008ull
#endif
#ifndef BIT4
#define BIT4        0x0000000000000010ull
#endif
#ifndef BIT5
#define BIT5        0x0000000000000020ull
#endif
#ifndef BIT6
#define BIT6        0x0000000000000040ull
#endif
#ifndef BIT7
#define BIT7        0x0000000000000080ull
#endif
#ifndef BIT8
#define BIT8        0x0000000000000100ull
#endif
#ifndef BIT9
#define BIT9        0x0000000000000200ull
#endif
#ifndef BIT10
#define BIT10       0x0000000000000400ull
#endif
#ifndef BIT11
#define BIT11       0x0000000000000800ull
#endif
#ifndef BIT12
#define BIT12       0x0000000000001000ull
#endif
#ifndef BIT13
#define BIT13       0x0000000000002000ull
#endif
#ifndef BIT14
#define BIT14       0x0000000000004000ull
#endif
#ifndef BIT15
#define BIT15       0x0000000000008000ull
#endif
#ifndef BIT16
#define BIT16       0x0000000000010000ull
#endif
#ifndef BIT17
#define BIT17       0x0000000000020000ull
#endif
#ifndef BIT18
#define BIT18       0x0000000000040000ull
#endif
#ifndef BIT19
#define BIT19       0x0000000000080000ull
#endif
#ifndef BIT20
#define BIT20       0x0000000000100000ull
#endif
#ifndef BIT21
#define BIT21       0x0000000000200000ull
#endif
#ifndef BIT22
#define BIT22       0x0000000000400000ull
#endif
#ifndef BIT23
#define BIT23       0x0000000000800000ull
#endif
#ifndef BIT24
#define BIT24       0x0000000001000000ull
#endif
#ifndef BIT25
#define BIT25       0x0000000002000000ull
#endif
#ifndef BIT26
#define BIT26       0x0000000004000000ull
#endif
#ifndef BIT27
#define BIT27       0x0000000008000000ull
#endif
#ifndef BIT28
#define BIT28       0x0000000010000000ull
#endif
#ifndef BIT29
#define BIT29       0x0000000020000000ull
#endif
#ifndef BIT30
#define BIT30       0x0000000040000000ull
#endif
#ifndef BIT31
#define BIT31       0x0000000080000000ull
#endif
#ifndef BIT32
#define BIT32       0x0000000100000000ull
#endif
#ifndef BIT33
#define BIT33       0x0000000200000000ull
#endif
#ifndef BIT34
#define BIT34       0x0000000400000000ull
#endif
#ifndef BIT35
#define BIT35       0x0000000800000000ull
#endif
#ifndef BIT36
#define BIT36       0x0000001000000000ull
#endif
#ifndef BIT37
#define BIT37       0x0000002000000000ull
#endif
#ifndef BIT38
#define BIT38       0x0000004000000000ull
#endif
#ifndef BIT39
#define BIT39       0x0000008000000000ull
#endif
#ifndef BIT40
#define BIT40       0x0000010000000000ull
#endif
#ifndef BIT41
#define BIT41       0x0000020000000000ull
#endif
#ifndef BIT42
#define BIT42       0x0000040000000000ull
#endif
#ifndef BIT43
#define BIT43       0x0000080000000000ull
#endif
#ifndef BIT44
#define BIT44       0x0000100000000000ull
#endif
#ifndef BIT45
#define BIT45       0x0000200000000000ull
#endif
#ifndef BIT46
#define BIT46       0x0000400000000000ull
#endif
#ifndef BIT47
#define BIT47       0x0000800000000000ull
#endif
#ifndef BIT48
#define BIT48       0x0001000000000000ull
#endif
#ifndef BIT49
#define BIT49       0x0002000000000000ull
#endif
#ifndef BIT50
#define BIT50       0x0004000000000000ull
#endif
#ifndef BIT51
#define BIT51       0x0008000000000000ull
#endif
#ifndef BIT52
#define BIT52       0x0010000000000000ull
#endif
#ifndef BIT53
#define BIT53       0x0020000000000000ull
#endif
#ifndef BIT54
#define BIT54       0x0040000000000000ull
#endif
#ifndef BIT55
#define BIT55       0x0080000000000000ull
#endif
#ifndef BIT56
#define BIT56       0x0100000000000000ull
#endif
#ifndef BIT57
#define BIT57       0x0200000000000000ull
#endif
#ifndef BIT58
#define BIT58       0x0400000000000000ull
#endif
#ifndef BIT59
#define BIT59       0x0800000000000000ull
#endif
#ifndef BIT60
#define BIT60       0x1000000000000000ull
#endif
#ifndef BIT61
#define BIT61       0x2000000000000000ull
#endif
#ifndef BIT62
#define BIT62       0x4000000000000000ull
#endif
#ifndef BIT63
#define BIT63       0x8000000000000000ull
#endif

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
