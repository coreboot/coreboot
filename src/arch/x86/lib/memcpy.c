#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned long d0, d1, d2;

	asm volatile(
		"rep movsb"
		: "=S"(d0), "=D"(d1), "=c"(d2)
		: "0"(src), "1"(dest), "2"(n)
		: "memory"
		);

	return dest;
}
