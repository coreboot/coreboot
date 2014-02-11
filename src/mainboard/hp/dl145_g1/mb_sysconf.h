/*
 * Copyright (c) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */
#ifndef MB_SYSCONF_H

#define MB_SYSCONF_H

struct mb_sysconf_t {
	unsigned char bus_8131_0;
	unsigned char bus_8131_1;
	unsigned char bus_8131_2;
	unsigned char bus_8111_0;
	unsigned char bus_8111_1;

	unsigned apicid_8111;
	unsigned apicid_8131_1;
	unsigned apicid_8131_2;

	unsigned sbdn3;
};

#endif

