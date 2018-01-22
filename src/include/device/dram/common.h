/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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

#ifndef DEVICE_DRAM_COMMON_H
#define DEVICE_DRAM_COMMON_H

/**
 * \brief Convenience definitions for TCK values
 *
 * Different values for tCK, representing standard DDR3 frequencies.
 * These values are in 1/256 ns units.
 * @{
 */
#define NS2MHZ_DIV256	(1000 << 8)

#define TCK_1333MHZ     192
#define TCK_1200MHZ     212
#define TCK_1100MHZ     232
#define TCK_1066MHZ     240
#define TCK_1000MHZ     256
#define TCK_933MHZ      274
#define TCK_900MHZ      284
#define TCK_800MHZ      320
#define TCK_700MHZ      365
#define TCK_666MHZ      384
#define TCK_533MHZ      480
#define TCK_400MHZ      640
#define TCK_333MHZ      768
#define TCK_266MHZ      960
#define TCK_200MHZ      1280
/** @} */

/**
 * \brief Convenience macro for enabling printk with CONFIG_DEBUG_RAM_SETUP
 *
 * Use this macro instead of printk(); for verbose RAM initialization messages.
 * When CONFIG_DEBUG_RAM_SETUP is not selected, these messages are automatically
 * disabled.
 * @{
 */
#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
#define printram(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printram(x, ...)
#endif
/** @} */

/** Result of the SPD decoding process */
enum spd_status {
	SPD_STATUS_OK = 0,
	SPD_STATUS_INVALID,
	SPD_STATUS_CRC_ERROR,
	SPD_STATUS_INVALID_FIELD,
};

u16 ddr3_crc16(const u8 *ptr, int n_crc);

#endif /* DEVICE_DRAM_COMMON_H */
