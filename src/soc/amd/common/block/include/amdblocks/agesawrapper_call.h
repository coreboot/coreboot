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

#ifndef __AGESAWRAPPER_CALL_H__
#define __AGESAWRAPPER_CALL_H__

#include <amdblocks/agesawrapper.h>
#include <stdint.h>
#include <console/console.h>

/*
 * Possible AGESA_STATUS values:
 *
 * 0x0 = AGESA_SUCCESS
 * 0x1 = AGESA_UNSUPPORTED
 * 0x2 = AGESA_BOUNDS_CHK
 * 0x3 = AGESA_ALERT
 * 0x4 = AGESA_WARNING
 * 0x5 = AGESA_ERROR
 * 0x6 = AGESA_CRITICAL
 * 0x7 = AGESA_FATAL
 */
static const char *decodeAGESA_STATUS(AGESA_STATUS sret)
{
	const char *statusStrings[] = { "AGESA_SUCCESS", "AGESA_UNSUPPORTED",
					"AGESA_BOUNDS_CHK", "AGESA_ALERT",
					"AGESA_WARNING", "AGESA_ERROR",
					"AGESA_CRITICAL", "AGESA_FATAL"
					};
	if (sret >= ARRAY_SIZE(statusStrings))
		return "unknown"; /* Non-AGESA error code */
	return statusStrings[sret];
}

static inline u32 do_agesawrapper(AGESA_STRUCT_NAME func, const char *name)
{
	AGESA_STATUS ret;
	printk(BIOS_DEBUG, "agesawrapper_%s() entry\n", name);
	ret = agesa_execute_state(func);
	printk(BIOS_DEBUG, "agesawrapper_%s() returned %s\n",
			name, decodeAGESA_STATUS(ret));
	return (u32)ret;
}

#endif /* __AGESAWRAPPER_CALL_H__ */
