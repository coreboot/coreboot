/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msrtool.h"

static void print_bitdef(FILE *f, const struct msrbits *mb, const char *tail) {
	uint8_t endbit;
	if (!reserved && 0 == strcmp(mb->name, "RSVD"))
		return;
	if (1 == mb->size)
		fprintf(f, "# %5d", mb->start);
	else {
		endbit = mb->start - mb->size + 1;
		fprintf(f, "# %*d:%d", endbit < 10 ? 3 : 2, mb->start, endbit);
	}
	if (0 == strcmp(mb->name, "RSVD"))
		fprintf(f, " [%s]", mb->desc);
	else
		fprintf(f, " %s %s", mb->name, mb->desc);
	fprintf(f, "%s", tail);
}

static void print_bitval(FILE *f, const struct msrbits *mb, const struct msr val) {
	uint8_t i;
	struct msr tmp, mask = MSR1(1);
	const struct msrbitvalues *mbv = mb->bitval;
	while (mbv->text && !msr_eq(mbv->value, val))
		mbv++;
	switch (mb->present) {
	case PRESENT_BIN:
		mask = msr_shl(mask, mb->size - 1);
		for (i = 0; i < mb->size; i++) {
			memcpy(&tmp, &val, sizeof(val));
			msr_and(&tmp, mask);
			fprintf(f, "%d", (tmp.hi || tmp.lo) ? 1 : 0);
			mask = msr_shr(mask, 1);
		}
		break;
	case PRESENT_DEC:
		fprintf(f, "%d", val.lo);
		break;
	case PRESENT_OCT:
		fprintf(f, "0%o", val.lo);
		break;
	case PRESENT_HEX:
		hexprint(f, val, mb->size);
		break;
	case PRESENT_HEXDEC:
		hexprint(f, val, mb->size);
		fprintf(f, " %d", val.lo);
		break;
	case PRESENT_STR:
		strprint(f, val, mb->size);
		break;
	}
	if (mbv->text)
		fprintf(f, ": %s", mbv->text);
	fprintf(f, "\n");
}

void hexprint(FILE *f, const struct msr val, const uint8_t bits) {
	if (bits <= 4)
		fprintf(f, "0x%01x", val.lo & 0xf);
	else if (bits <= 8)
		fprintf(f, "0x%02x", val.lo & 0xff);
	else if (bits <= 12)
		fprintf(f, "0x%03x", val.lo & 0xfff);
	else if (bits <= 16)
		fprintf(f, "0x%04x", val.lo & 0xffff);
	else if (bits <= 20)
		fprintf(f, "0x%05x", val.lo & 0xfffff);
	else if (bits <= 24)
		fprintf(f, "0x%06x", val.lo & 0xffffff);
	else if (bits <= 28)
		fprintf(f, "0x%07x", val.lo & 0xfffffff);
	else if (bits <= 32)
		fprintf(f, "0x%08x", val.lo);
	else if (bits <= 36)
		fprintf(f, "0x%01x%08x", val.hi & 0xf, val.lo);
	else if (bits <= 40)
		fprintf(f, "0x%02x%08x", val.hi & 0xff, val.lo);
	else if (bits <= 44)
		fprintf(f, "0x%03x%08x", val.hi & 0xfff, val.lo);
	else if (bits <= 48)
		fprintf(f, "0x%04x%08x", val.hi & 0xffff, val.lo);
	else if (bits <= 52)
		fprintf(f, "0x%05x%08x", val.hi & 0xfffff, val.lo);
	else if (bits <= 56)
		fprintf(f, "0x%06x%08x", val.hi & 0xffffff, val.lo);
	else if (bits <= 60)
		fprintf(f, "0x%07x%08x", val.hi & 0xfffffff, val.lo);
	else
		fprintf(f, "0x%08x%08x", val.hi, val.lo);
}

void strprint(FILE *f, const struct msr val, const uint8_t bits) {
	if (bits > 56)
		fputc(val.hi, f);
	if (bits > 48)
		fputc(val.hi >> 8, f);
	if (bits > 40)
		fputc(val.hi >> 16, f);
	if (bits > 32)
		fputc(val.hi >> 24, f);
	if (bits > 24)
		fputc(val.lo, f);
	if (bits > 16)
		fputc(val.lo >> 8, f);
	if (bits > 8)
		fputc(val.lo >> 16, f);
	if (bits > 0)
		fputc(val.lo >> 24, f);
}

int msr_eq(const struct msr a, const struct msr b) {
	return a.hi == b.hi && a.lo == b.lo;
}

struct msr msr_shl(const struct msr a, const uint8_t bits) {
	struct msr ret;

	ret.hi = bits < 32 ? a.hi << bits : 0;
	ret.lo = bits < 32 ? a.lo << bits : 0;

	if (bits < 32)
		ret.hi |= bits ? a.lo >> (32 - bits) : 0;
	else
		ret.hi |= a.lo << (bits - 32);

	return ret;
}

struct msr msr_shr(const struct msr a, const uint8_t bits) {
	struct msr ret;

	ret.hi = bits < 32 ? a.hi >> bits : 0;
	ret.lo = bits < 32 ? a.lo >> bits : 0;

	if (bits < 32)
		ret.lo |= bits ? a.hi << (32 - bits) : 0;
	else
		ret.lo |= a.hi >> (bits - 32);

	return ret;
}

void msr_and(struct msr *a, const struct msr b) {
	a->hi &= b.hi;
	a->lo &= b.lo;
}

const struct msrdef *findmsrdef(const uint32_t addr) {
	uint8_t t;
	const struct msrdef *m;
	if (!targets)
		return NULL;
	for (t = 0; t < targets_found; t++)
		for (m = targets[t]->msrs; !MSR_ISEOT(*m); m++)
			if (addr == m->addr)
				return m;
	return NULL;
}

uint32_t msraddrbyname(const char *name) {
	uint8_t t;
	const uint32_t addr = strtoul(name, NULL, 16);
	const struct msrdef *m;
	if (!targets)
		return addr;
	for (t = 0; t < targets_found; t++)
		for (m = targets[t]->msrs; !MSR_ISEOT(*m); m++) {
			if (addr == m->addr)
				return m->addr;
			if (!strcasecmp(name, m->symbol))
				return m->addr;
		}
	return addr;
}

void dumpmsrdefs(const struct targetdef *t) {
	const struct msrdef *m;
	const struct msrbits *mb;
	if (NULL == t)
		return;
	printf("# %s MSRs:\n", t->name);
	for (m = t->msrs; !MSR_ISEOT(*m); m++) {
		if (t->msrs != m)
			printf("\n");
		printf("# %s\n", m->symbol);
		for (mb = m->bits; mb->size; mb++)
			print_bitdef(stdout, mb, "\n");
		printf("0x%08x\n", m->addr);
	}
}

int dumpmsrdefsvals(FILE *f, const struct targetdef *t, const uint8_t cpu) {
	struct msr val = MSR1(0);
	const struct msrdef *m;
	const struct msrbits *mb;
	if (NULL == t)
		return 1;
	fprintf(f, "# %s MSRs:\n", t->name);
	for (m = t->msrs; !MSR_ISEOT(*m); m++) {
		if (t->msrs != m)
			fprintf(f, "\n");
		if (!sys->rdmsr(cpu, m->addr, &val))
			return 1;
		fprintf(f, "# %s\n", m->symbol);
		for (mb = m->bits; mb->size; mb++)
			print_bitdef(f, mb, "\n");
		fprintf(f, "0x%08x 0x%08x%08x\n", m->addr, val.hi, val.lo);
	}
	return 0;
}

/**
 * Parse a hexadecimal string into an MSR value.
 *
 * Leading 0x or 0X is optional, the string is always parsed as hexadecimal.
 * Any non-hexadecimal character except ' ' can separate the high 32 bits and
 * the low 32 bits. If there is such a separator, high and low values do not
 * need to be zero padded. If there is no separator, the last <=8 digits are
 * the low 32 bits and any characters before them are the high 32 bits.
 * When there is no separator and less than eight digits, the high 32 bits
 * are set to 0.
 * Parsing fails when there is a separator and it is followed by another
 * non-hexadecimal character.
 *
 * @param str The string to parse. The string must be writable but will be
 * restored before return.
 * @param msr Pointer to the struct msr where the value will be stored.
 * @param endptr If endptr is not NULL, *endptr will point to after the MSR.
 * @return 1 on success, 0 on parse failure. msr is unchanged on failure.
 */
uint8_t str2msr(char *str, struct msr *msr, char **endptr) {
	char c;
	size_t len, lo;
	if (0 == strncmp(str, "0x", 2) || 0 == strncmp(str, "0X", 2))
		str += 2;
	len = strspn(str, HEXCHARS);
	if (len <= 8 && (0 == str[len] || ' ' == str[len])) {
		msr->hi = 0;
		lo = 0;
	} else if (len <= 8) {
		lo = len + strcspn(str + len, HEXCHARS);
		if (0 == len && 0 == strspn(str + lo, HEXCHARS))
			return 0;
		c = str[len];
		str[len] = 0;
		msr->hi = strtoul(str, NULL, 16);
		str[len] = c;
	} else {
		lo = len - 8;
		c = str[lo];
		str[lo] = 0;
		msr->hi = strtoul(str, NULL, 16);
		str[lo] = c;
	}
	msr->lo = strtoul(str + lo, endptr, 16);
	return 1;
}

void decodemsr(const uint8_t cpu, const uint32_t addr, const struct msr val) {
	struct msr bitval, mask;
	const struct msrdef *m = findmsrdef(addr);
	const struct msrbits *mb;

	if (NULL != m)
		printf("# %s ", m->symbol);
	printf("0x%08x = 0x%08x%08x\n", addr, val.hi, val.lo);
	if (NULL == m) {
		fprintf(stderr, "Sorry - no definition exists for this MSR! Please add it and send a signed-off\n");
		fprintf(stderr, "patch to coreboot@coreboot.org. Thanks for your help!\n");
		return;
	}

	for (mb = m->bits; mb->size; mb++) {
		if (!reserved && 0 == strcmp(mb->name, "RSVD"))
			continue;
		print_bitdef(stdout, mb, " = ");
		mask.hi = mask.lo = 0xffffffff;
		mask = msr_shr(mask, 64 - mb->size);
		bitval = msr_shr(val, mb->start - mb->size + 1);
		msr_and(&bitval, mask);
		print_bitval(stdout, mb, bitval);
	}
}

/**
 * Compare two MSR values and print any differences with field definitions and
 * both old and new values decoded.
 *
 * @param f Output stream.
 * @param addr MSR address.
 * @param a Left value.
 * @param b Right value.
 * @return 1 when a and b differ, 0 when they are equal or only reserved bits
 * differ and processing of reserved bits was not requested (with -r).
 */
uint8_t diff_msr(FILE *f, const uint32_t addr, const struct msr a, const struct msr b) {
	uint8_t ret = 0, first = 1;
	struct msr aval, bval, mask;
	const struct msrdef *m = findmsrdef(addr);
	const struct msrbits *mb;

	if (a.hi == b.hi && a.lo == b.lo)
		return 0;

	if (NULL == m) {
		fprintf(stderr, "MSR 0x%08x has no definition! Please add it and send a Signed-off-by patch\n", addr);
		fprintf(stderr, "to coreboot@coreboot.org. Thank you for your help!\n");
		return 1;
	}

	for (mb = m->bits; mb->size; mb++) {
		if (!reserved && 0 == strcmp(mb->name, "RSVD"))
			continue;
		mask.hi = mask.lo = 0xffffffff;
		mask = msr_shr(mask, 64 - mb->size);
		aval = msr_shr(a, mb->start - mb->size + 1);
		bval = msr_shr(b, mb->start - mb->size + 1);
		msr_and(&aval, mask);
		msr_and(&bval, mask);
		if (msr_eq(aval, bval))
			continue;
		if (first) {
			fprintf(f, "# %s\n", m->symbol);
			fprintf(f, "-0x%08x 0x%08x%08x\n", addr, a.hi, a.lo);
			fprintf(f, "+0x%08x 0x%08x%08x\n", addr, b.hi, b.lo);
			first = 0;
			ret = 1;
		}
		print_bitdef(f, mb, "\n-");
		print_bitval(f, mb, aval);
		fprintf(f, "+");
		print_bitval(f, mb, bval);
	}
	return ret;
}
