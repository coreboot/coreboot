/*
 * This file is part of the LinuxBIOS project.
 *
 * It was taken from kexec-tools 1.101.
 *
 * Copyright (C) 2003,2004 Eric Biederman <ebiederm@xmission.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <ip_checksum.h>
#include <string.h>

unsigned long compute_ip_checksum(void *addr, unsigned long length)
{
	uint16_t *ptr;
	unsigned long sum;
	unsigned long len;
	unsigned long laddr;
	/* compute an ip style checksum */
	laddr = (unsigned long )addr;
	sum = 0;
	if (laddr & 1) {
		uint16_t buffer;
		unsigned char *ptr;
		/* copy the first byte into a 2 byte buffer.
		 * This way automatically handles the endian question
		 * of which byte (low or high) the last byte goes in.
		 */
		buffer = 0;
		ptr = addr;
		memcpy(&buffer, ptr, 1);
		sum += buffer;
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
		length -= 1;
		addr = ptr +1;
		
	}
	len = length >> 1;
	ptr = addr;
	while (len--) {
		sum += *(ptr++);
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	addr = ptr;
	if (length & 1) {
		uint16_t buffer;
		unsigned char *ptr;
		/* copy the last byte into a 2 byte buffer.
		 * This way automatically handles the endian question
		 * of which byte (low or high) the last byte goes in.
		 */
		buffer = 0;
		ptr = addr;
		memcpy(&buffer, ptr, 1);
		sum += buffer;
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	return (~sum) & 0xFFFF;
	
}

unsigned long add_ip_checksums(unsigned long offset, unsigned long sum, unsigned long new)
{
	unsigned long checksum;
	sum = ~sum & 0xFFFF;
	new = ~new & 0xFFFF;
	if (offset & 1) {
		/* byte swap the sum if it came from an odd offset 
		 * since the computation is endian independant this
		 * works.
		 */
		new = ((new >> 8) & 0xff) | ((new << 8) & 0xff00);
	}
	checksum = sum + new;
	if (checksum > 0xFFFF) {
		checksum -= 0xFFFF;
	}
	return (~checksum) & 0xFFFF;
}

unsigned long negate_ip_checksum(unsigned long sum)
{
	sum = ~sum & 0xFFFF;

	sum = 0xFFFF - sum;

	return ~sum & 0xFFFF;
}
