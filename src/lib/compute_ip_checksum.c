#include <stdint.h>
#include <ip_checksum.h>

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
