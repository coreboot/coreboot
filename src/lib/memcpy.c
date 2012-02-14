#include <string.h>
void *memcpy(void *vdest, const void *vsrc, size_t bytes)
{
	const char *src = vsrc;
	char *dest = vdest;
	int i;

	for (i = 0; i < (int)bytes; i++)
		dest[i] = src[i];

	return vdest;
}

/* from linux kernel 2.6.32 asm/string_32.h */
void inline __attribute__((always_inline))  memcopy_fast(void *dest, const void *src, unsigned long bytes)
{
	int d0, d1, d2;
	asm volatile("cld ; rep ; movsl\n\t"
			"movl %4,%%ecx\n\t"
			"andl $3,%%ecx\n\t"
			"jz 1f\n\t"
			"rep ; movsb\n\t"
			"1:"
			: "=&c" (d0), "=&D" (d1), "=&S" (d2)
			: "0" (bytes / 4), "g" (bytes), "1" ((long)dest), "2" ((long)src)
			: "memory", "cc");
}
