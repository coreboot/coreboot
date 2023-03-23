/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <soc/msr.h>
#include <types.h>

/* Value defined in Serial VID Interface 2.0 spec (#48022, NDA only) */
#define  SERIAL_VID_2_DECODE_MICROVOLTS	6250
#define  SERIAL_VID_2_MAX_MICROVOLTS	1550000L

uint32_t get_uvolts_from_vid(uint16_t core_vid)
{
	if (core_vid >= 0xF8) {
		/* Voltage off for VID codes >= 0xF8 */
		return 0;
	} else {
		return SERIAL_VID_2_MAX_MICROVOLTS -
			(SERIAL_VID_2_DECODE_MICROVOLTS * core_vid);
	}
}

uint32_t get_pstate_core_uvolts(union pstate_msr pstate_reg)
{
	return get_uvolts_from_vid(pstate_reg.cpu_vid_0_7);
}
