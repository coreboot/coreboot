#ifndef CPU_X86_CACHE
#define CPU_X86_CACHE

static inline unsigned long read_cr0(void)
{
	unsigned long cr0;
	asm volatile ("movl %%cr0, %0" : "=r" (cr0));
	return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
	asm volatile ("movl %0, %%cr0" : : "r" (cr0));
}

static inline void invd(void)
{
	asm volatile("invd" ::: "memory");
}
static inline void wbinvd(void)
{
	asm volatile ("wbinvd");
}

static inline void enable_cache(void)
{
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 &= 0x9fffffff;
	write_cr0(cr0);
}

static inline void disable_cache(void)
{
	/* Disable and write back the cache */
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 |= 0x40000000;
	wbinvd();
	write_cr0(cr0);
	wbinvd();
}

#if !defined(__PRE_RAM__)
void x86_enable_cache(void);
#endif

#endif /* CPU_X86_CACHE */
