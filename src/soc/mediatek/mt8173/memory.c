/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <soc/emi.h>
#include <soc/pll.h>

enum {
	/* test patterns */
	PATTERN0 = 0x00000000,
	PATTERN1 = 0x5A5A5A5A,
	PATTERN2 = 0xA5A5A5A5,
	PATTERN3 = 0xA5A5A500,
	PATTERN4 = 0xA500A500,
	PATTERN5 = 0xA5000000,
	PATTERN6 = 0xFFFF0000,
	PATTERN7 = 0x0000FFFF,
	PATTERN8 = 0x00000012,
	PATTERN9 = 0x00000034,
	PATTERNA = 0x00000056,
	PATTERNB = 0x00000078,
	PATTERNC = 0x00001234,
	PATTERND = 0x00005678,
	PATTERNE = 0x12345678,
	PATTERNF = 0xFFFFFFFF
};

static int complex_mem_test(unsigned int start, unsigned int len)
{
	unsigned char *mem8_base = (unsigned char *)(uintptr_t)start;
	unsigned short *mem16_base = (unsigned short *)(uintptr_t)start;
	unsigned int *mem32_base = (unsigned int *)(uintptr_t)start;
	unsigned int *mem_base = (unsigned int *)(uintptr_t)start;
	unsigned char pattern8;
	unsigned short pattern16;
	unsigned int i, j, size, pattern32;
	unsigned int value;

	size = len >> 2;

	/*  verify the tied bits (tied high)  */
	for (i = 0; i < size; i++) {
		mem32_base[i] = PATTERN0;
	}

	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN0) {
			return -1;
		} else {
			mem32_base[i] = PATTERNF;
		}
	}

	/*  verify the tied bits (tied low)  */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERNF) {
			return -2;
		} else
			mem32_base[i] = PATTERN0;
	}

	/*  verify pattern 1 (0x00~0xff)  */
	pattern8 = PATTERN0;
	for (i = 0; i < len; i++)
		mem8_base[i] = pattern8++;
	pattern8 = PATTERN0;
	for (i = 0; i < len; i++) {
		if (mem8_base[i] != pattern8++) {
			return -3;
		}
	}

	/*  verify pattern 2 (0x00~0xff)  */
	pattern8 = PATTERN0;
	for (i = j = 0; i < len; i += 2, j++) {
		if (mem8_base[i] == pattern8)
			mem16_base[j] = pattern8;
		if (mem16_base[j] != pattern8) {
			return -4;
		}
		pattern8 += 2;
	}

	/*  verify pattern 3 (0x00~0xffff)  */
	pattern16 = PATTERN0;
	for (i = 0; i < (len >> 1); i++)
		mem16_base[i] = pattern16++;
	pattern16 = PATTERN0;
	for (i = 0; i < (len >> 1); i++) {
		if (mem16_base[i] != pattern16++) {
			return -5;
		}
	}

	/*  verify pattern 4 (0x00~0xffffffff)  */
	pattern32 = PATTERN0;
	for (i = 0; i < (len >> 2); i++)
		mem32_base[i] = pattern32++;
	pattern32 = PATTERN0;
	for (i = 0; i < (len >> 2); i++) {
		if (mem32_base[i] != pattern32++) {
			return -6;
		}
	}

	/*  pattern 5: filling memory range with 0x12345678  */
	for (i = 0; i < size; i++)
		mem32_base[i] = PATTERNE;

	/*  read check then fill memory with a5a5a5a5 pattern  */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERNE) {
			return -7;
		} else {
			mem32_base[i] = PATTERN2;
		}
	}

	/* read check then fill memory with 00 byte pattern at offset 0h */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN2) {
			return -8;
		} else {
			mem8_base[i * 4] = PATTERN0;
		}
	}

	/* read check then fill memory with 00 byte pattern at offset 2h */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN3) {
			return -9;
		} else {
			mem8_base[i * 4 + 2] = PATTERN0;
		}
	}

	/*  read check then fill memory with 00 byte pattern at offset 1h  */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN4) {
			return -10;
		} else {
			mem8_base[i * 4 + 1] = PATTERN0;
		}
	}

	/*  read check then fill memory with 00 byte pattern at offset 3h  */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN5) {
			return -11;
		} else {
			mem8_base[i * 4 + 3] = PATTERN0;
		}
	}

	/*  read check then fill memory with ffff word pattern at offset 1h */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN0) {
			return -12;
		} else {
			mem16_base[i * 2 + 1] = PATTERN7;
		}
	}

	/*  read check then fill memory with ffff word pattern at offset 0h */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERN6) {
			return -13;
		} else {
			mem16_base[i * 2] = PATTERN7;
		}
	}

	/*  read check  */
	for (i = 0; i < size; i++) {
		if (mem32_base[i] != PATTERNF) {
			return -14;
		}
	}

	/*  stage 1 => write 0  */
	for (i = 0; i < size; i++) {
		mem_base[i] = PATTERN1;
	}

	/*  stage 2 => read 0, write 0xf  */
	for (i = 0; i < size; i++) {
		value = mem_base[i];

		if (value != PATTERN1) {
			return -15;
		}
		mem_base[i] = PATTERN2;
	}

	/*  stage 3 => read 0xf, write 0  */
	for (i = 0; i < size; i++) {
		value = mem_base[i];
		if (value != PATTERN2) {
			return -16;
		}
		mem_base[i] = PATTERN1;
	}

	/*  stage 4 => read 0, write 0xf  */
	for (i = 0; i < size; i++) {
		value = mem_base[i];
		if (value != PATTERN1) {
			return -17;
		}
		mem_base[i] = PATTERN2;
	}

	/*  stage 5 => read 0xf, write 0  */
	for (i = 0; i < size; i++) {
		value = mem_base[i];
		if (value != PATTERN2) {
			return -18;
		}
		mem_base[i] = PATTERN1;
	}

	/*  stage 6 => read 0  */
	for (i = 0; i < size; i++) {
		value = mem_base[i];
		if (value != PATTERN1) {
			return -19;
		}
	}

	/*  1/2/4-byte combination test  */
	i = (unsigned int)(uintptr_t)mem_base;

	while (i < (unsigned int)(uintptr_t)mem_base + (size << 2)) {
		*((unsigned char *)(uintptr_t)i) = PATTERNB;
		i += 1;
		*((unsigned char *)(uintptr_t)i) = PATTERNA;
		i += 1;
		*((unsigned short *)(uintptr_t)i) = PATTERNC;
		i += 2;
		*((unsigned int *)(uintptr_t)i) = PATTERNE;
		i += 4;
		*((unsigned short *)(uintptr_t)i) = PATTERND;
		i += 2;
		*((unsigned char *)(uintptr_t)i) = PATTERN9;
		i += 1;
		*((unsigned char *)(uintptr_t)i) = PATTERN8;
		i += 1;
		*((unsigned int *)(uintptr_t)i) = PATTERNE;
		i += 4;
		*((unsigned char *)(uintptr_t)i) = PATTERNB;
		i += 1;
		*((unsigned char *)(uintptr_t)i) = PATTERNA;
		i += 1;
		*((unsigned short *)(uintptr_t)i) = PATTERNC;
		i += 2;
		*((unsigned int *)(uintptr_t)i) = PATTERNE;
		i += 4;
		*((unsigned short *)(uintptr_t)i) = PATTERND;
		i += 2;
		*((unsigned char *)(uintptr_t)i) = PATTERN9;
		i += 1;
		*((unsigned char *)(uintptr_t)i) = PATTERN8;
		i += 1;
		*((unsigned int *)(uintptr_t)i) = PATTERNE;
		i += 4;
	}

	for (i = 0; i < size; i++) {
		value = mem_base[i];
		if (value != PATTERNE) {
			return -20;
		}
	}

	/*  verify pattern 1 (0x00~0xff)  */
	pattern8 = PATTERN0;
	mem8_base[0] = pattern8;
	for (i = 0; i < size * 4; i++) {
		unsigned char waddr8, raddr8;

		waddr8 = i + 1;
		raddr8 = i;
		if (i < size * 4 - 1)
			mem8_base[waddr8] = pattern8 + 1;
		if (mem8_base[raddr8] != pattern8) {
			return -21;
		}
		pattern8++;
	}

	/*  verify pattern 2 (0x00~0xffff)  */
	pattern16 = PATTERN0;
	mem16_base[0] = pattern16;
	for (i = 0; i < size * 2; i++) {
		if (i < size * 2 - 1)
			mem16_base[i + 1] = pattern16 + 1;
		if (mem16_base[i] != pattern16) {
			return -22;
		}
		pattern16++;
	}

	/*  verify pattern 3 (0x00~0xffffffff)  */
	pattern32 = PATTERN0;
	mem32_base[0] = pattern32;
	for (i = 0; i < size; i++) {
		if (i < size - 1)
			mem32_base[i + 1] = pattern32 + 1;
		if (mem32_base[i] != pattern32) {
			return -23;
		}
		pattern32++;
	}

	return 0;
}

void mt_mem_init(const struct mt8173_sdram_params *sdram_params)
{
	int i = 0;

	/* init mempll */
	mem_pll_init(sdram_params);

	/* memory calibration */
	mt_set_emi(sdram_params);

	if (IS_ENABLED(CONFIG_MEMORY_TEST)) {
		/* do memory test:
		 * set memory scan range 0x2000
		 * larger test length, longer system boot up time
		 */
		i = complex_mem_test(DDR_BASE, 0x2000);

		printk(BIOS_DEBUG, "[MEM] complex R/W mem test %s : %d\n",
		       (i == 0) ? "pass" : "fail", i);

		ASSERT(i == 0);
	}
}
