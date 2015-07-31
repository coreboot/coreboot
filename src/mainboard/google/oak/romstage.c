/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/io.h>

#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <symbols.h>
#include <timestamp.h>

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	/* init uart baudrate when pll on */
	console_init();
	exception_init();

	run_ramstage();
}
