#include <ip_checksum.h>

unsigned long compute_ip_checksum(void *addr, unsigned long length)
{
	unsigned short *ptr;
	unsigned long sum;
	unsigned long len;
	/* Assumes len is a multiple of two, and addr is 2 byte aligned. */
	/* compute an ip style checksum */
	sum = 0;
	len = length >> 1;
	ptr = addr;
	while (len--) {
		sum += *(ptr++);
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	return (~sum) & 0xFFFF;
	
}
