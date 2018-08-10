/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#define DPTF_CPU_DEVICE		TCPU

#ifndef DPTF_CPU_PASSIVE
#define DPTF_CPU_PASSIVE	80
#endif

#ifndef DPTF_CPU_CRITICAL
#define DPTF_CPU_CRITICAL	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC0
#define DPTF_CPU_ACTIVE_AC0	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC1
#define DPTF_CPU_ACTIVE_AC1	80
#endif

#ifndef DPTF_CPU_ACTIVE_AC2
#define DPTF_CPU_ACTIVE_AC2	70
#endif

#ifndef DPTF_CPU_ACTIVE_AC3
#define DPTF_CPU_ACTIVE_AC3	60
#endif

#ifndef DPTF_CPU_ACTIVE_AC4
#define DPTF_CPU_ACTIVE_AC4	50
#endif
