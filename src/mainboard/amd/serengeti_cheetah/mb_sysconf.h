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

#ifndef MB_SYSCONF_H

#define MB_SYSCONF_H

struct mb_sysconf_t {
	unsigned char bus_8132_0;
	unsigned char bus_8132_1;
	unsigned char bus_8132_2;
	unsigned char bus_8111_0;
	unsigned char bus_8111_1;

	unsigned char bus_8132a[7][3];

	unsigned char bus_8151[7][2];

	unsigned apicid_8111;
	unsigned apicid_8132_1;
	unsigned apicid_8132_2;
	unsigned apicid_8132a[7][2];

	unsigned sbdn3;
	unsigned sbdn3a[7];
	unsigned sbdn5[7];

};

#endif
