/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * cache.h: Cache maintenance API for ARM
 */

#ifndef ARM_CACHE_H
#define ARM_CACHE_H

#include <stddef.h>
#include <stdint.h>

/* SCTLR bits */
#define SCTLR_M		(1 << 0)	/* MMU enable			*/
#define SCTLR_A		(1 << 1)	/* Alignment check enable	*/
#define SCTLR_C		(1 << 2)	/* Data/unified cache enable	*/
/* Bits 4:3 are reserved */
#define SCTLR_CP15BEN	(1 << 5)	/* CP15 barrier enable		*/
/* Bit 6 is reserved */
#define SCTLR_B		(1 << 7)	/* Endianness			*/
/* Bits 9:8 */
#define SCTLR_SW	(1 << 10)	/* SWP and SWPB enable		*/
#define SCTLR_Z		(1 << 11)	/* Branch prediction enable	*/
#define SCTLR_I		(1 << 12)	/* Instruction cache enable	*/
#define SCTLR_V		(1 << 13)	/* Low/high exception vectors	*/
#define SCTLR_RR	(1 << 14)	/* Round Robin select		*/
/* Bits 16:15 are reserved */
#define SCTLR_HA	(1 << 17)	/* Hardware Access flag enable	*/
/* Bit 18 is reserved */
/* Bits 20:19 reserved virtualization not supported */
#define SCTLR_WXN	(1 << 19)	/* Write permission implies XN	*/
#define SCTLR_UWXN	(1 << 20)	/* Unprivileged write permission
					   implies PL1 XN		*/
#define SCTLR_FI	(1 << 21)	/* Fast interrupt config enable	*/
#define SCTLR_U		(1 << 22)	/* Unaligned access behavior	*/
#define SCTLR_VE	(1 << 24)	/* Interrupt vectors enable	*/
#define SCTLR_EE	(1 << 25)	/* Exception endianness		*/
/* Bit 26 is reserved */
#define SCTLR_NMFI	(1 << 27)	/* Non-maskable FIQ support	*/
#define SCTLR_TRE	(1 << 28)	/* TEX remap enable		*/
#define SCTLR_AFE	(1 << 29)	/* Access flag enable		*/
#define SCTLR_TE	(1 << 30)	/* Thumb exception enable	*/
/* Bit 31 is reserved */

/*
 * Sync primitives
 */

/* data memory barrier */
static inline void dmb(void)
{
	asm volatile ("dmb" : : : "memory");
}

/* data sync barrier */
static inline void dsb(void)
{
	asm volatile ("dsb" : : : "memory");
}

/* instruction sync barrier */
static inline void isb(void)
{
	asm volatile ("isb" : : : "memory");
}

/*
 * Low-level TLB maintenance operations
 */

/* invalidate entire unified TLB */
static inline void tlbiall(void)
{
	asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0) : "memory");
}

/* invalidate unified TLB by MVA and ASID */
static inline void tlbimva(unsigned long mva)
{
	asm volatile ("mcr p15, 0, %0, c8, c7, 1" : : "r" (mva) : "memory");
}

/* write data access control register (DACR) */
static inline void write_dacr(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c3, c0, 0" : : "r" (val));
}

/* read memory model feature register 0 (MMFR0) */
static inline uint32_t read_mmfr0(void)
{
	uint32_t mmfr;
	asm volatile ("mrc p15, 0, %0, c0, c1, 4" : "=r" (mmfr));
	return mmfr;
}
/* read MAIR0 (memory address indirection register 0) */
static inline uint32_t read_mair0(void)
{
	uint32_t mair;
	asm volatile ("mrc p15, 0, %0, c10, c2, 0" : "=r" (mair));
	return mair;
}
/* write MAIR0 (memory address indirection register 0) */
static inline void write_mair0(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c10, c2, 0" : : "r" (val));
}
/* write translation table base register 0 (TTBR0) */
static inline void write_ttbr0(uint32_t val)
{
	if (CONFIG(ARM_LPAE))
		asm volatile ("mcrr p15, 0, %[val], %[zero], c2" : :
			[val] "r" (val), [zero] "r" (0));
	else
		asm volatile ("mcr p15, 0, %0, c2, c0, 0" : : "r" (val) : "memory");
}

/* read translation table base control register (TTBCR) */
static inline uint32_t read_ttbcr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 0, %0, c2, c0, 2" : "=r" (val));
	return val;
}

/* write translation table base control register (TTBCR) */
static inline void write_ttbcr(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c2, c0, 2" : : "r" (val) : "memory");
}

/*
 * Low-level cache maintenance operations
 */

/* branch predictor invalidate all */
static inline void bpiall(void)
{
	asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));
}

/* data cache clean and invalidate by MVA to PoC */
static inline void dccimvac(unsigned long mva)
{
	asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (mva) : "memory");
}

/* data cache invalidate by set/way */
static inline void dccisw(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c7, c14, 2" : : "r" (val) : "memory");
}

/* data cache clean by MVA to PoC */
static inline void dccmvac(unsigned long mva)
{
	asm volatile ("mcr p15, 0, %0, c7, c10, 1" : : "r" (mva) : "memory");
}

/* data cache clean by set/way */
static inline void dccsw(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c7, c10, 2" : : "r" (val) : "memory");
}

/* data cache invalidate by MVA to PoC */
static inline void dcimvac(unsigned long mva)
{
	asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (mva) : "memory");
}

/* data cache invalidate by set/way */
static inline void dcisw(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c7, c6, 2" : : "r" (val) : "memory");
}

/* instruction cache invalidate all by PoU */
static inline void iciallu(void)
{
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));
}

/*
 * Cache co-processor (CP15) access functions
 */

/* read cache level ID register (CLIDR) */
static inline uint32_t read_clidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c0, c0, 1" : "=r" (val));
	return val;
}

/* read cache size ID register (CCSIDR) */
static inline uint32_t read_ccsidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (val));
	return val;
}

/* read cache size selection register (CSSELR) */
static inline uint32_t read_csselr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 2, %0, c0, c0, 0" : "=r" (val));
	return val;
}

/* write to cache size selection register (CSSELR) */
static inline void write_csselr(uint32_t val)
{
	/*
	 * Bits [3:1] - Cache level + 1 (0b000 = L1, 0b110 = L7, 0b111 is rsvd)
	 * Bit 0 - 0 = data or unified cache, 1 = instruction cache
	 */
	asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (val));
	isb();	/* ISB to sync the change to CCSIDR */
}

/* read L2 control register (L2CTLR) */
static inline uint32_t read_l2ctlr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c9, c0, 2" : "=r" (val));
	return val;
}

/* write L2 control register (L2CTLR) */
static inline void write_l2ctlr(uint32_t val)
{
	/*
	 * Note: L2CTLR can only be written when the L2 memory system
	 * is idle, ie before the MMU is enabled.
	 */
	asm volatile("mcr p15, 1, %0, c9, c0, 2" : : "r" (val) : "memory" );
	isb();
}

/* read L2 Auxiliary Control Register (L2ACTLR) */
static inline uint32_t read_l2actlr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 1, %0, c15, c0, 0" : "=r" (val));
	return val;
}

/* write L2 Auxiliary Control Register (L2ACTLR) */
static inline void write_l2actlr(uint32_t val)
{
	asm volatile ("mcr p15, 1, %0, c15, c0, 0" : : "r" (val) : "memory" );
	isb();
}

/* read system control register (SCTLR) */
static inline uint32_t read_sctlr(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}

/* write system control register (SCTLR) */
static inline void write_sctlr(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (val) : "cc");
	isb();
}

/* read data fault address register (DFAR) */
static inline uint32_t read_dfar(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (val));
	return val;
}

/* read data fault status register (DFSR) */
static inline uint32_t read_dfsr(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (val));
	return val;
}

/* read instruction fault address register (IFAR) */
static inline uint32_t read_ifar(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c6, c0, 2" : "=r" (val));
	return val;
}

/* read instruction fault status register (IFSR) */
static inline uint32_t read_ifsr(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c5, c0, 1" : "=r" (val));
	return val;
}

/* read auxiliary data fault status register (ADFSR) */
static inline uint32_t read_adfsr(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c5, c1, 0" : "=r" (val));
	return val;
}

/* read auxiliary instruction fault status register (AIFSR) */
static inline uint32_t read_aifsr(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c5, c1, 1" : "=r" (val));
	return val;
}

/*
 * Cache maintenance API
 */

/* dcache clean and invalidate all (on current level given by CCSELR) */
void dcache_clean_invalidate_all(void);

/* dcache clean by modified virtual address to PoC */
void dcache_clean_by_mva(void const *addr, size_t len);

/* dcache clean and invalidate by modified virtual address to PoC */
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);

/* dcache invalidate by modified virtual address to PoC */
void dcache_invalidate_by_mva(void const *addr, size_t len);

void dcache_clean_all(void);

/* dcache invalidate all (on current level given by CCSELR) */
void dcache_invalidate_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* dcache and MMU disable */
void dcache_mmu_disable(void);

/* dcache and MMU enable */
void dcache_mmu_enable(void);

/* perform all icache/dcache maintenance needed after loading new code */
void cache_sync_instructions(void);

/* tlb invalidate all */
void tlb_invalidate_all(void);

/*
 * Generalized setup/init functions
 */

/* MMU initialization (set page table base, permissions, initialize subtable
 * buffer, etc.). Must only be called ONCE PER BOOT, before any mappings. */
void mmu_init(void);

enum dcache_policy {
	DCACHE_OFF,
	DCACHE_WRITEBACK,
	DCACHE_WRITETHROUGH,
};

/* disable the mmu for a range. Primarily useful to lock out address 0. */
void mmu_disable_range(u32 start_mb, u32 size_mb);
/* mmu range configuration (set dcache policy) */
void mmu_config_range(u32 start_mb, u32 size_mb, enum dcache_policy policy);

/* Reconfigure memory mappings at the fine-grained (4K) page level. Must be
 * called on a range contained within a single, already mapped block/superpage.
 * Careful: Do NOT map over this address range with mmu_config_range() again
 * later, or you will leak resources and may desync your TLB! */
void mmu_config_range_kb(u32 start_kb, u32 size_kb, enum dcache_policy policy);
void mmu_disable_range_kb(u32 start_kb, u32 size_kb);

#endif /* ARM_CACHE_H */
