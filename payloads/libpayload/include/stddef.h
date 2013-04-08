
#include <arch/types.h>

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;
typedef long ssize_t;

#define offsetof(TYPE, MEMBER)	((size_t) &((TYPE *) 0)->MEMBER)

