/*
 * This file is part of the coreboot project.
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

/* Dummy CHROMEEC file to provide stub functions for vboot compilation */

#include <stdint.h>

int google_chromeec_vbnv_context(int is_read, uint8_t *data, int len);
int google_chromeec_vbnv_context(int is_read, uint8_t *data, int len)
{
	return 0;
}
