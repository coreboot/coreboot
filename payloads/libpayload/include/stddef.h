
#include <arch/types.h>

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;
/* There is a GCC macro for a size_t type, but not
 * for a ssize_t type. Below construct tricks GCC
 * into making __SIZE_TYPE__ signed.
 */
#define unsigned signed
typedef __SIZE_TYPE__ ssize_t;
#undef unsigned

#define offsetof(TYPE, MEMBER)	((size_t) &((TYPE *) 0)->MEMBER)
#define member_size(TYPE, MEMBER)	(sizeof(((TYPE *) 0)->MEMBER))

#define check_member(structure, member, offset) _Static_assert( \
	offsetof(struct structure, member) == offset, \
	"`struct " #structure "` offset for `" #member "` is not " #offset)

/* Standard units. */
#define KiB (1 << 10)
#define MiB (1 << 20)
#define GiB (1 << 30)

#define KHz (1000)
#define MHz (1000*KHz)
#define GHz (1000*MHz)

#define NSECS_PER_SEC 1000000000
#define USECS_PER_SEC 1000000
#define MSECS_PER_SEC 1000
#define NSECS_PER_MSEC (NSECS_PER_SEC / MSECS_PER_SEC)
#define NSECS_PER_USEC (NSECS_PER_SEC / USECS_PER_SEC)
#define USECS_PER_MSEC (USECS_PER_SEC / MSECS_PER_SEC)
