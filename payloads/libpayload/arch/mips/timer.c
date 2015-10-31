/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <libpayload.h>
#include <arch/cpu.h>
#include <arch/io.h>

#define PISTACHIO_CLOCK_SWITCH		0xB8144200
#define MIPS_EXTERN_PLL_BYPASS_MASK	0x00000002

/**
 * @ingroup arch
 * Global variable containing the speed of the processor in KHz.
 */
u32 cpu_khz;

/**
 * Calculate the speed of the processor for use in delays.
 *
 * @return The CPU speed in kHz.
 */
unsigned int get_cpu_speed(void)
{
	if (IMG_PLATFORM_ID() != IMG_PLATFORM_ID_SILICON)
		cpu_khz = 50000; /* FPGA board */
	else {
		/* If MIPS PLL external bypass bit is set, it means
		 * that the MIPS PLL is already set up to work at a
		 * frequency of 550 MHz; otherwise, the crystal is
		 * used with a frequency of 52 MHz
		 */
		if (read32(PISTACHIO_CLOCK_SWITCH) &
				MIPS_EXTERN_PLL_BYPASS_MASK)
			cpu_khz = 550000;
		else
			cpu_khz = 52000;
	}

	return cpu_khz;
}
