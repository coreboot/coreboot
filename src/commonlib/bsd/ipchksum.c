/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#include <commonlib/bsd/helpers.h>
#include <commonlib/bsd/ipchksum.h>

/* See RFC 1071 for mathematical explanations of why we can first sum in a larger register and
   then narrow down, why we don't need to worry about endianness, etc. */
uint16_t ipchksum(const void *data, size_t size)
{
	const uint8_t *p1 = data;
	unsigned long wide_sum = 0;
	uint32_t sum = 0;
	size_t i = 0;

#if defined(__aarch64__)
	size_t size16 = size / 16;
	const uint64_t *p8 = data;
	if (size16) {
		unsigned long tmp1, tmp2;
		i = size16 * 16;
		asm (
			"adds	xzr, xzr, xzr\n\t"	/* init carry flag for addition */
			"1:\n\t"
			"ldp	%[v1], %[v2], [%[p8]], #16\n\t"
			"adcs	%[wsum], %[wsum], %[v1]\n\t"
			"adcs	%[wsum], %[wsum], %[v2]\n\t"
			"sub	%[size16], %[size16], #1\n\t"
			"cbnz	%[size16], 1b\n\t"
			"adcs	%[wsum], %[wsum], xzr\n\t"	/* use up last carry */
		: [v1] "=r" (tmp1),
		  [v2] "=r" (tmp2),
		  [wsum] "+r" (wide_sum),
		  [p8] "+r" (p8),
		  [size16] "+r" (size16)
		:: "cc"
		);
	}
#elif defined(__i386__) || defined(__x86_64__)	/* __aarch64__ */
	size_t size8 = size / 8;
	const uint64_t *p8 = data;
	i = size8 * 8;
	asm (
		"clc\n\t"
		"1:\n\t"
		"jecxz	2f\n\t"		/* technically RCX on 64, but not gonna be that big */
		"adc	(%[p8]), %[wsum]\n\t"
#if defined(__i386__)
		"adc	4(%[p8]), %[wsum]\n\t"
#endif	/* __i386__ */
		"lea	-1(%[size8]), %[size8]\n\t"	/* Use LEA as a makeshift ADD that */
		"lea	8(%[p8]), %[p8]\n\t"		/* doesn't modify the carry flag. */
		"jmp	1b\n\t"
		"2:\n\t"
		"setc	%b[size8]\n\t"	/* reuse size register to save last carry */
		"add	%[size8], %[wsum]\n\t"
	: [wsum] "+r" (wide_sum),
	  [p8] "+r" (p8),
	  [size8] "+c" (size8)		/* put size in ECX so we can JECXZ */
	:: "cc"
	);
#else	/* __i386__ || __x86_64__ */
	size_t aligned_size = ALIGN_DOWN(size, sizeof(unsigned long));
	const unsigned long *p_long = data;
	for (; i < aligned_size; i += sizeof(unsigned long)) {
		unsigned long new_sum = wide_sum + *p_long++;
		/* Overflow check to emulate a manual "add with carry" in C. The compiler seems
		   to be clever enough to find ways to elide the branch on most archs. */
		if (new_sum < wide_sum)
			new_sum++;
		wide_sum = new_sum;
	}
#endif

	while (wide_sum) {
		sum += wide_sum & 0xFFFF;
		wide_sum >>= 16;
	}
	sum = (sum & 0xFFFF) + (sum >> 16);

	for (; i < size; i++) {
		uint32_t v = p1[i];
		if (i % 2)
			v <<= 8;
		sum += v;

		/* Doing this unconditionally seems to be faster. */
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return (uint16_t)~sum;
}

uint16_t ipchksum_add(size_t offset, uint16_t first, uint16_t second)
{
	first = ~first;
	second = ~second;

	/*
	 * Since the checksum is calculated in 16-bit chunks, if the offset at which
	 * the data covered by the second checksum would start (if both data streams
	 * came one after the other) is odd, that means the second stream starts in
	 * the middle of a 16-bit chunk. This means the second checksum is byte
	 * swapped compared to what we need it to be, and we must swap it back.
	 */
	if (offset % 2)
		second = (second >> 8) | (second << 8);

	uint32_t sum = first + second;
	sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)~sum;
}
