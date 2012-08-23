#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned long d0, d1, d2;

	asm volatile(
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
		"rep ; movsb\n\t"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n >> 2), "g" (n & 3), "1" (dest), "2" (src)
		: "memory"
	);

	return dest;
}
