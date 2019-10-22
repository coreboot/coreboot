/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/stages.h>
#include <soc/usb.h>
#include <soc/qclib_common.h>

static void prepare_usb(void)
{
	/*
	 * Do DWC3 core and phy reset. Kick these resets
	 * off early so they get at least 1ms to settle.
	 */
	reset_usb0();
}

void platform_romstage_main(void)
{
	prepare_usb();

	/* QCLib: DDR init & train */
	qclib_load_and_run();
}
