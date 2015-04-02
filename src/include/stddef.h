#ifndef STDDEF_H
#define STDDEF_H

typedef long ptrdiff_t;
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

typedef int wchar_t;
typedef unsigned int wint_t;

#define NULL ((void *)0)

/* Standard units. */
#define KiB (1<<10)
#define MiB (1<<20)
#define GiB (1<<30)
/* Could we ever run into this one? I hope we get this much memory! */
#define TiB (1<<40)

#define KHz (1000)
#define MHz (1000 * KHz)
#define GHz (1000 * MHz)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define check_member(structure, member, offset) _Static_assert( \
	offsetof(struct structure, member) == offset, \
	"`struct " #structure "` offset for `" #member "` is not " #offset )

/**
 * container_of - cast a member of a structure out to the containing structure
 * @param ptr:    the pointer to the member.
 * @param type:   the type of the container struct this is embedded in.
 * @param member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#ifdef __PRE_RAM__
#define ROMSTAGE_CONST const
#else
#define ROMSTAGE_CONST
#endif

/* Work around non-writable data segment in execute-in-place romstage on x86. */
#if defined(__PRE_RAM__) && CONFIG_ARCH_X86
#define MAYBE_STATIC
#else
#define MAYBE_STATIC static
#endif

#endif /* STDDEF_H */
