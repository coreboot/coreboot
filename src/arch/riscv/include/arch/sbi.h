/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Jonathan Neuschäfer <j.neuschaefer@gmx.net>
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


#ifndef _ARCH_SBI_H
#define _ARCH_SBI_H

#define SBI_ECALL_HART_ID 0
#define SBI_ECALL_CONSOLE_PUT 1
#define SBI_ECALL_SEND_DEVICE_REQUEST 2
#define SBI_ECALL_RECEIVE_DEVICE_RESPONSE 3
#define SBI_ECALL_SEND_IPI 4
#define SBI_ECALL_CLEAR_IPI 5
#define SBI_ECALL_SHUTDOWN 6
#define SBI_ECALL_SET_TIMER 7
#define SBI_ECALL_QUERY_MEMORY 8
#define SBI_ECALL_NUM_HARTS 9

#ifndef __ASSEMBLY__
struct opaque;
extern struct opaque sbi_page;
#endif

#endif
