#ifndef ARCH_HLT_H
#define ARCH_HLT_H

#ifdef __ROMCC__
static void hlt(void)
{
	__builtin_hlt();
}

#endif

#ifdef __GNUC__
static inline void hlt(void)
{
	asm("hlt");
	return;
}
#endif

#endif /* ARCH_HLT_H */
