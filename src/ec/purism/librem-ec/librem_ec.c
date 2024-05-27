/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include "librem_ec.h"
#include "../../system76/ec/system76_ec.h"

#define CMD_PROBE 1

bool librem_ec_has_jack_detect(void)
{
	/* The 'flags' field in the probe command reply was added in an update.
	   Send 4 bytes of zeroes in the "request" to zero out the field if the
	   EC does not set it for its reply. */
	const uint8_t request_data[4] = {0};
	uint8_t reply_data[4] = {0};
	bool ec_cmd_success = system76_ec_cmd(CMD_PROBE, request_data,
		ARRAY_SIZE(request_data), reply_data, ARRAY_SIZE(reply_data));
	if (!ec_cmd_success)
		return false;
	/* Byte 3 is flags, bit 0 is the jack detect flag */
	return reply_data[3] & 0x01;
}
