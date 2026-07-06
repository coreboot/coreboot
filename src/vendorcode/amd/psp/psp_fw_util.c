
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "internal.h"

/*
 * Creates the OSI Fletcher checksum. See 8473-1, Appendix C, section C.3.
 * The checksum field of the passed PDU does not need to be reset to zero.
 *
 * The "Fletcher Checksum" was proposed in a paper by John G. Fletcher of
 * Lawrence Livermore Labs.  The Fletcher Checksum was proposed as an
 * alternative to cyclical redundancy checks because it provides error-
 * detection properties similar to cyclical redundancy checks but at the
 * cost of a simple summation technique.  Its characteristics were first
 * published in IEEE Transactions on Communications in January 1982.  One
 * version has been adopted by ISO for use in the class-4 transport layer
 * of the network protocol.
 *
 */
#define FLETCHER32_BLOCK_LEN 359
uint32_t psp_fw_fletcher32_rdev(const struct region_device *rdev)
{
	uint32_t c0;
	uint32_t c1;
	uint32_t checksum;
	uint32_t index;

	assert (region_device_sz(rdev) %2 == 0);
	uint32_t length = region_device_sz(rdev) / 2;
	uint32_t offset = 0;

	c0 = 0xFFFF;
	c1 = 0xFFFF;

	while (length) {
		index = MIN(length, FLETCHER32_BLOCK_LEN);
		length -= index;
		do {
			uint16_t data;
			if (rdev_readat(rdev, &data, offset, sizeof(uint16_t)) != sizeof(uint16_t))
				return 0;
			offset += 2;
			c0 += data;
			c1 += c0;
		} while (--index);
		c0 = (c0 & 0xFFFF) + (c0 >> 16);
		c1 = (c1 & 0xFFFF) + (c1 >> 16);
	}

	/* Sums[0,1] mod 64K + overflow */
	c0 = (c0 & 0xFFFF) + (c0 >> 16);
	c1 = (c1 & 0xFFFF) + (c1 >> 16);
	checksum = (c1 << 16) | c0;

	return checksum;
}
