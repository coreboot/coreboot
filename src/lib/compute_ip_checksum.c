#include <stdint.h>
#include <ip_checksum.h>

#include <console/console.h>

unsigned long compute_ip_checksum(void *addr, unsigned long length)
{
	uint8_t *ptr;
	volatile union {
		uint8_t  byte[2];
		uint16_t word;
	} value;
	unsigned long sum;
	unsigned long i;
	/* In the most straight forward way possible,
	 * compute an ip style checksum.
	 */
	sum = 0;
	ptr = addr;
	printk(BIOS_DEBUG, "%s: ptr: 0x%p, length: %ld\n", __func__, ptr, length);
	for(i = 0; i < length; i++) {
		unsigned long v;
		v = ptr[i];
		if (i & 1) {
			v <<= 8;
		}
		/* Add the new value */
		sum += v;
		/* Wrap around the carry */
		if (sum > 0xFFFF) {
			sum = (sum + (sum >> 16)) & 0xFFFF;
		}
	}
	printk(BIOS_DEBUG, "%s: done\n", __func__);
	value.byte[0] = sum & 0xff;
	value.byte[1] = (sum >> 8) & 0xff;
	printk(BIOS_DEBUG, "%s: exiting...\n", __func__);
	return (~value.word) & 0xFFFF;
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
