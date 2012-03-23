#include <string.h>

void *memcpy(void *__restrict __dest,
	     __const void *__restrict __src, size_t __n)
{
	asm("cld\n"
	    "rep\n"
	    "movsb"
	    :	/* no input (?) */
	    :"S"(__src), "D"(__dest), "c"(__n)
	);
	return __dest;
}
