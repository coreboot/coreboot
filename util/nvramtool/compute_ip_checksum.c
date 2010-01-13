/*****************************************************************************\
 * compute_ip_checksum.c
\*****************************************************************************/

#include <stdint.h>
#include "ip_checksum.h"

/* Note: The contents of this file were borrowed from the coreboot source
 *       code which may be obtained from http://www.coreboot.org.
 *       Specifically, this code was obtained from coreboot (LinuxBIOS)
 *       version 1.0.0.8.
 */

unsigned long compute_ip_checksum(void *addr, unsigned long length)
{
	uint8_t *ptr;
	volatile union {
		uint8_t byte[2];
		uint16_t word;
	} value;
	unsigned long sum;
	unsigned long i;
	/* In the most straight forward way possible,
	 * compute an ip style checksum.
	 */
	sum = 0;
	ptr = addr;
	for (i = 0; i < length; i++) {
		unsigned long value;
		value = ptr[i];
		if (i & 1) {
			value <<= 8;
		}
		/* Add the new value */
		sum += value;
		/* Wrap around the carry */
		if (sum > 0xFFFF) {
			sum = (sum + (sum >> 16)) & 0xFFFF;
		}
	}
	value.byte[0] = sum & 0xff;
	value.byte[1] = (sum >> 8) & 0xff;
	return (~value.word) & 0xFFFF;
}
