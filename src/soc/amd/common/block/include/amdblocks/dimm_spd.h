/*
 * This file is part of the coreboot project.
 *
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

#ifndef __DIMMSPD_H__
#define __DIMMSPD_H__

#include <amdblocks/agesawrapper.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Fill the buf and returns 0 on success.
 * Return -1 on failure and the caller tries sb_read_spd()
 * to get the SPD from I2C.
 */
int mainboard_read_spd(uint8_t spdAddress, char *buf, size_t len);
int sb_read_spd(uint8_t spdAddress, char *buf, size_t len);

#endif
