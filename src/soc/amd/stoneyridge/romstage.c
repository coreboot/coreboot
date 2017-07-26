/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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

#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <agesawrapper.h>
#include <agesawrapper_call.h>
#include <soc/northbridge.h>
#include <soc/hudson.h>
#include <amdblocks/psp.h>

asmlinkage void car_stage_entry(void)
{
	console_init();

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);

	post_code(0x41);
	psp_notify_dram();

	post_code(0x42);
	cbmem_initialize_empty();

	/*
	 * This writes contents to DRAM backing before teardown.
	 * todo: move CAR teardown to postcar implementation and
	 *       relocate amdinitenv to ramstage.
	 */
	chipset_teardown_car();

	post_code(0x43);
	AGESAWRAPPER(amdinitenv);

	post_code(0x50);
	run_ramstage();

	post_code(0x54);  /* Should never see this post code. */
}
