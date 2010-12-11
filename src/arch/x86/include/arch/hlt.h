#ifndef ARCH_HLT_H
#define ARCH_HLT_H

#if defined(__ROMCC__)
static void hlt(void)
{
	__builtin_hlt();
}
#else
static inline __attribute__((always_inline)) void hlt(void)
{
	asm("hlt");
}
#endif

#endif /* ARCH_HLT_H */
