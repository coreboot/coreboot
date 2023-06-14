/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * cache.c: Cache maintenance routines for ARMv8 (aarch64)
 *
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 */

#include <stdint.h>

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <program_loading.h>

enum cache_type cpu_get_cache_type(enum cache_level level)
{
	uint32_t ctype_bitshift = (level - 1) * 3;

	if (level < CACHE_L1 || level > CACHE_L7)
		return NO_CACHE;

	/* 3-bits per cache-level */
	return (raw_read_clidr_el1() >> ctype_bitshift) & 0x7;
}

static uint64_t get_ccsidr_el1_assoc(uint64_t ccsidr_el1)
{
	/* [23:20] - CCIDX support enables 64-bit CCSIDR_EL1 */
	if ((raw_read_id_aa64mmfr2_el1() & 0xF00000) == 0x100000) {
		/* [23:3] */
		return (ccsidr_el1 & 0xFFFFF8) >> 3;
	} else {
		/* [12:3] */
		return (ccsidr_el1 & 0x1FF8) >> 3;
	}
}

static uint64_t get_ccsidr_el1_numsets(uint64_t ccsidr_el1)
{
	/* [23:20] - CCIDX support enables 64-bit CCSIDR_EL1 */
	if ((raw_read_id_aa64mmfr2_el1() & 0xF00000) == 0x100000) {
		/* [55:32] */
		return (ccsidr_el1 & 0xFFFFFF00000000) >> 32;
	} else {
		/* [27:13] */
		return (ccsidr_el1 & 0xFFFE000) >> 13;
	}
}

void cpu_get_cache_info(enum cache_level level, enum cache_type type, size_t *cache_size, size_t *assoc)
{
	uint64_t ccsidr_el1;

	if (cache_size == NULL || assoc == NULL)
		return;

	if (level < CACHE_L1 || level > CACHE_L7)
		return;

	/* [0] - Indicates instruction cache; [3:1] - Indicates cache level */
	raw_write_csselr_el1(((level - 1) << 1) | (type == CACHE_INSTRUCTION));
	ccsidr_el1 = raw_read_ccsidr_el1();

	/* [2:0] - Indicates (Log2(Number of bytes in cache line) - 4) */
	uint8_t line_length = 1 << ((ccsidr_el1 & 0x7) + 4);
	/* (Number of sets in cache) - 1 */
	uint64_t num_sets = get_ccsidr_el1_numsets(ccsidr_el1) + 1;
	/* (Associativity of cache) - 1 */
	*assoc = get_ccsidr_el1_assoc(ccsidr_el1) + 1;
	*cache_size = line_length * *assoc * num_sets;
}

unsigned int dcache_line_bytes(void)
{
	uint32_t ctr_el0;
	static unsigned int line_bytes = 0;

	if (line_bytes)
		return line_bytes;

	ctr_el0 = raw_read_ctr_el0();
	/* [19:16] - Indicates (Log2(number of words in cache line) */
	line_bytes = 1 << ((ctr_el0 >> 16) & 0xf);
	/* Bytes in a word (32-bit) */
	line_bytes *= sizeof(uint32_t);

	return line_bytes;
}

enum dcache_op {
	OP_DCCSW,
	OP_DCCISW,
	OP_DCISW,
	OP_DCCIVAC,
	OP_DCCVAC,
	OP_DCIVAC,
};

/*
 * Do a dcache operation by virtual address. This is useful for maintaining
 * coherency in drivers which do DMA transfers and only need to perform
 * cache maintenance on a particular memory range rather than the entire cache.
 */
static void dcache_op_va(void const *addr, size_t len, enum dcache_op op)
{
	uint64_t line, linesize;

	linesize = dcache_line_bytes();
	line = (uint64_t)addr & ~(linesize - 1);

	dsb();
	while ((void *)line < addr + len) {
		switch (op) {
		case OP_DCCIVAC:
			dccivac(line);
			break;
		case OP_DCCVAC:
			dccvac(line);
			break;
		case OP_DCIVAC:
			dcivac(line);
			break;
		default:
			break;
		}
		line += linesize;
	}
	isb();
}

void dcache_clean_by_mva(void const *addr, size_t len)
{
	dcache_op_va(addr, len, OP_DCCVAC);
}

void dcache_clean_invalidate_by_mva(void const *addr, size_t len)
{
	dcache_op_va(addr, len, OP_DCCIVAC);
}

void dcache_invalidate_by_mva(void const *addr, size_t len)
{
	dcache_op_va(addr, len, OP_DCIVAC);
}

/*
 * For each segment of a program loaded this function is called
 * to invalidate caches for the addresses of the loaded segment
 */
void arch_segment_loaded(uintptr_t start, size_t size, int flags)
{
	uint32_t sctlr = raw_read_sctlr_el3();
	if (sctlr & SCTLR_C)
		dcache_clean_by_mva((void *)start, size);
	else if (sctlr & SCTLR_I)
		dcache_clean_invalidate_by_mva((void *)start, size);
	icache_invalidate_all();
}
