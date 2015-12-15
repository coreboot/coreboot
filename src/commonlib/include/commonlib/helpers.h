#ifndef COMMONLIB_HELPERS_H
#define COMMONLIB_HELPERS_H
/* This file is for helpers for both coreboot firmware and its utilities. */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define ALIGN(x,a)              __ALIGN_MASK(x,(__typeof__(x))(a)-1UL)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN_UP(x,a)           ALIGN((x),(a))
#define ALIGN_DOWN(x,a)         ((x) & ~((__typeof__(x))(a)-1UL))
#define IS_ALIGNED(x,a)         (((x) & ((__typeof__(x))(a)-1UL)) == 0)

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#define CEIL_DIV(a, b)  (((a) + (b) - 1) / (b))
#define IS_POWER_OF_2(x)  (((x) & ((x) - 1)) == 0)

/* Standard units. */
#define KiB (1<<10)
#define MiB (1<<20)
#define GiB (1<<30)
/* Could we ever run into this one? I hope we get this much memory! */
#define TiB (1<<40)

#define KHz (1000)
#define MHz (1000 * KHz)
#define GHz (1000 * MHz)

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#if !defined(__clang__)
#define check_member(structure, member, offset) _Static_assert( \
	offsetof(struct structure, member) == offset, \
	"`struct " #structure "` offset for `" #member "` is not " #offset )
#else
#define check_member(structure, member, offset)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @param ptr:    the pointer to the member.
 * @param type:   the type of the container struct this is embedded in.
 * @param member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef __unused
#define __unused __attribute__((unused))
#endif

#endif /* COMMONLIB_HELPERS_H */
