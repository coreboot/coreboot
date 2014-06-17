#include <console/console.h>
#include <lib.h>

/* Assume 8 bits per byte */
#define CHAR_BIT 8

unsigned long log2(unsigned long x)
{
	/* assume 8 bits per byte. */
	unsigned long pow = sizeof(x) * CHAR_BIT - 1ULL;
	unsigned long i = 1ULL << pow;

	if (!x) {
		printk(BIOS_WARNING, "%s called with invalid parameter of 0\n",
			__func__);
		return -1;
	}

	for (; i > x; i >>= 1, pow--);

	return pow;
}

unsigned long log2_ceil(unsigned long x)
{
	unsigned long pow;

	if (!x)
		return -1;

	pow = log2(x);

	if (x > (1ULL << pow))
		pow++;

	return pow;
}
