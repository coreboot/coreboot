#ifndef ARCH_HLT_H
#define ARCH_HLT_H

static inline __attribute__((always_inline)) void hlt(void)
{
	for (;;) ;
	//asm("hlt");
}

#endif /* ARCH_HLT_H */
