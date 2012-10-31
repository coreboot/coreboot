/* FIXME(dhendrix): This is split out from asm/system.h. */
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <asm/system.h>

static inline unsigned int get_cr(void)
{
	unsigned int val;
	asm("mrc p15, 0, %0, c1, c0, 0	@ get CR" : "=r" (val) : : "cc");
	return val;
}

static inline void set_cr(unsigned int val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 0	@ set CR"
	  : : "r" (val) : "cc");
	isb();
}

/* options available for data cache on each page */
enum dcache_option {
	DCACHE_OFF,
	DCACHE_WRITETHROUGH,
	DCACHE_WRITEBACK,
};

/* Size of an MMU section */
enum {
	MMU_SECTION_SHIFT	= 20,
	MMU_SECTION_SIZE	= 1 << MMU_SECTION_SHIFT,
};

/**
 * Change the cache settings for a region.
 *
 * \param start		start address of memory region to change
 * \param size		size of memory region to change
 * \param option	dcache option to select
 */
void mmu_set_region_dcache(unsigned long start, int size,
			enum dcache_option option);

/**
 * Register an update to the page tables, and flush the TLB
 *
 * \param start		start address of update in page table
 * \param stop		stop address of update in page table
 */
void mmu_page_table_flush(unsigned long start, unsigned long stop);

void dram_bank_mmu_setup(unsigned long start, unsigned long size);

void arm_init_before_mmu(void);

 /*
  * FIXME: sdelay, sr32, and wait_on_value originally came from
  * arch/arm/cpu/armv7/exynos5/setup.h in u-boot but do not seem
  * specific to exynos5...
  */
void sdelay(unsigned long loops);
void sr32(void *addr, u32 start_bit, u32 num_bits, u32 value);
u32 wait_on_value(u32 read_bit_mask, u32 match_value, void *read_addr,
		  u32 bound);

#endif	/* SYSTEM_H_ */
