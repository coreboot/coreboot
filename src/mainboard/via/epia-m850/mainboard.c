/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>

#if IS_ENABLED(CONFIG_VGA_ROM_RUN)

#include <arch/interrupt.h>
#include <x86emu/x86emu.h>

#include <northbridge/via/vx900/vx900.h>

static int vx900_int15_handler(void)
{
	int res;

	printk(BIOS_DEBUG, "%s %0x\n", __func__, X86_AX & 0xffff);
	/* Set AX return value here so we don't set it every time. Just set it
	 * to something else if the callback is unsupported */
	res = -1;
	switch (X86_AX & 0xffff) {
#if 0
	case 0x5f01:
		/* VGA POST - panel type */
		/* FIXME: Don't hardcode panel type */
		/* Panel Type Number */
		X86_CX = 0;
		res = 0;
		break;
	case 0x5f02:
		{
			/* Boot device selection */
			X86_BL = INT15_5F02_BL_HWOPT_CRTCONN;
			/* FIXME: or 0 ? */
			X86_BH = 0;	// INT15_5F02_BH_TV_CONN_DEFAULT;
			X86_EBX = 0;	//  INT15_5F02_EBX_HDTV_RGB;
			X86_ECX = INT15_5F02_ECX_DISPLAY_CRT;
			//X86_ECX |= INT15_5F02_ECX_TV_MODE_RGB;
			//X86_ECX |= INT15_5F02_ECX_HDTV_1080P;
			X86_DL = INT15_5F02_DL_TV_LAYOUT_DEFAULT;
			res = 0;
			break;
		}
#endif
	case 0x5f18:
		X86_BL = vx900_int15_get_5f18_bl();
		res = 0;
		break;
#if 0
	case 0x5f2a:
		/* Get SSC Control Settings */
		/* FIXME: No idea what this does. Just disable this feature
		 * for now */
		X86_CX = INT15_5F2A_CX_SSC_ENABLE;
		res = 0;
		break;
	case 0x5f2b:
		/* Engine clock setting */
		/* FIXME: ECLK fixed 250MHz ? */
		X86_EBX = INT15_5F2B_EBX_ECLK_250MHZ;
		break;
#endif
	default:
		printk(BIOS_DEBUG, "Unsupported INT15 call %04x!\n",
		       X86_AX & 0xffff);
		X86_AX = 0;
		res = -1;
		break;
	}

	if (res == 0)
		X86_AX = 0x5f;
	else
		X86_AX = 0;
	return X86_AX;
}
#endif

static void mainboard_enable(struct device *dev)
{
	(void)dev;

#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
	printk(BIOS_DEBUG, "Installing INT15 handler...\n");
	mainboard_interrupt_handlers(0x15, &vx900_int15_handler);
#endif
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("VIA EPIA-M850 Mainboard")
	    .enable_dev = mainboard_enable,
};
