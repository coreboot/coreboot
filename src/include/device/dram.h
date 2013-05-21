/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEVICE_DRAM_H
#define DEVICE_DRAM_H

/**
 * @file dram.h
 *
 * \brief Common definitions for DRAM SPDs
 *
 * Include this file as:
 * @code{.c}
 *	#include <device/dram.h>
 * @endcode
 *
 * @{
 */

#include <stdint.h>


/* DRAM type, byte 2 of spd */
enum dram_type {
	DRAM_TYPE_UNDEFINED		= 0x00,
	DRAM_TYPE_FPM_DRAM		= 0x01,
	DRAM_TYPE_EDO			= 0x02,
	DRAM_TYPE_PIPELINED_NIBBLE	= 0x03,
	DRAM_TYPE_SDRAM			= 0x04,
	DRAM_TYPE_ROM			= 0x05,
	DRAM_TYPE_DDR_SGRAM		= 0x06,
	DRAM_TYPE_DDR			= 0x07,
	DRAM_TYPE_DDR2			= 0x08,
	DRAM_TYPE_DDR2_FBDIMM		= 0x09,
	DRAM_TYPE_DDR2_FB_PROBE		= 0x0a,
	DRAM_TYPE_DDR3			= 0x0b,
};

/* Module type (byte 3, bits 3:0) of SPD */
enum dimm_type {
	DIMM_TYPE_UNDEFINED		= 0,
	DIMM_TYPE_RDIMM			= 1,
	DIMM_TYPE_UDIMM			= 2,
	DIMM_TYPE_SO_DIMM		= 3,
	DIMM_TYPE_MICRO_DIMM		= 4,
	DIMM_TYPE_MINI_RDIMM		= 5,
	DIMM_TYPE_MINI_UDIMM		= 6,
	DIMM_TYPE_MINI_CDIMM		= 7,
	DIMM_TYPE_72B_SO_UDIMM		= 8,
	DIMM_TYPE_72B_SO_RDIMM		= 9,
	DIMM_TYPE_72B_SO_CDIMM		= 10,
	/* Masks to bits 3:0 to give the dimm type */
	DIMM_TYPE_MASK			= 0x0f,
};

/**
 * \brief Convenience macro for enabling printk with CONFIG_DEBUG_RAM_SETUP
 *
 * Use this macro instead of printk(); for verbose RAM initialization messages.
 * When CONFIG_DEBUG_RAM_SETUP is not selected, these messages are automatically
 * disabled.
 * @{
 */
#if defined(CONFIG_DEBUG_RAM_SETUP) && (CONFIG_DEBUG_RAM_SETUP)
#define printram(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printram(x, ...)
#endif
/** @} */


/** @} */
#endif /* DEVICE_DRAM_H */
