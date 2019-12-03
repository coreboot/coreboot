/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <northbridge/amd/agesa/state_machine.h>
#include <superio/smsc/smscsuperio/smscsuperio.h>
#include <sb_cimx.h>

#define SERIAL_DEV PNP_DEV(0x4e, SMSCSUPERIO_SP1)

void board_BeforeAgesa(struct sysinfo *cb)
{
	sb_Poweron_Init();
	smscsuperio_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
