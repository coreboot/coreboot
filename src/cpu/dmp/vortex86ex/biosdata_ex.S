/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

/*
PLL Freq = 25 * NS /(MS * 2^RS)
CPU Freq = PLL/(CPU_DIV+2)
DRAM Freq = PLL/2(DRAM_DIV+1)

DDR3
CPU/DRAM/PCI            B6 B7 BB BC BD BF
200/200/33              30 03 0F 02 8F 07
300/300/33              48 03 0F 02 1F 07
300/300/33              48 03 0F 3A DF 07          ; write leveling disable, CPU bypass disable
300/300/33              48 03 0F 22 3F 07          ; CPU bypass disable
300/300/100             48 03 23 02 7F 07
400/200/33              60 43 0F 02 3F 07          ; without 200MHz timing, so set 300MHz timing
400/200/100             60 43 23 02 4F 07
400/400/33              60 03 0F 02 BF 09
500/250/33              50 42 0F 02 DF 07
500/500/33              78 03 0F 02 4F 09
400/300/33              90 53 0F 02 3F 07
400/300/33              90 53 0F 1A DF 07          ; write leveling/gate training disable
400/300/100             90 53 23 02 9F 07
444/333/33              A0 53 0F 02 5F 08
466/350/33              A8 53 0F 02 DF 09
500/375/33              B4 53 0F 02 AF 09
*/

#if CONFIG_PLL_200_200_33
	// 200/200/33              30 03 0F 02 8F 07
	byte_fffb6 = 0x30
	byte_fffb7 = 0x03
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_300_300_33
	// 300/300/33              48 03 0F 02 1F 07
	byte_fffb6 = 0x48
	byte_fffb7 = 0x03
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_300_300_100
	// 300/300/100             48 03 23 02 7F 07
	byte_fffb6 = 0x48
	byte_fffb7 = 0x03
	byte_fffbb = 0x23
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_400_200_33
	// 400/200/33              60 43 0F 02 3F 07          ; without 200MHz timing, so set 300MHz timing
	byte_fffb6 = 0x60
	byte_fffb7 = 0x43
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_400_200_100
	// 400/200/100             60 43 23 02 4F 07
	byte_fffb6 = 0x60
	byte_fffb7 = 0x43
	byte_fffbb = 0x23
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_400_400_33
	// 400/400/33              60 03 0F 02 BF 09
	byte_fffb6 = 0x60
	byte_fffb7 = 0x03
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x09
#elif CONFIG_PLL_500_250_33
	// 500/250/33              50 42 0F 02 DF 07
	byte_fffb6 = 0x50
	byte_fffb7 = 0x42
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_500_500_33
	// 500/500/33              78 03 0F 02 4F 09
	byte_fffb6 = 0x78
	byte_fffb7 = 0x03
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x09
#elif CONFIG_PLL_400_300_33
	// 400/300/33              90 53 0F 02 3F 07
	byte_fffb6 = 0x90
	byte_fffb7 = 0x53
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_400_300_100
	// 400/300/100             90 53 23 02 9F 07
	byte_fffb6 = 0x90
	byte_fffb7 = 0x53
	byte_fffbb = 0x23
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x07
#elif CONFIG_PLL_444_333_33
	// 444/333/33              A0 53 0F 02 5F 08
	byte_fffb6 = 0xa0
	byte_fffb7 = 0x53
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x08
#elif CONFIG_PLL_466_350_33
	// 466/350/33              A8 53 0F 02 DF 09
	byte_fffb6 = 0xa8
	byte_fffb7 = 0x53
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x09
#elif CONFIG_PLL_500_375_33
	// 500/375/33              B4 53 0F 02 AF 09
	byte_fffb6 = 0xb4
	byte_fffb7 = 0x53
	byte_fffbb = 0x0f
	byte_fffbc = 0x02
	byte_fffbe = 0xff
	byte_fffbf = 0x09
#else
	#error Error Strap PLL config.
#endif

tmp_sum = byte_fffb6 + byte_fffb7 + byte_fffbb + byte_fffbc
pll_checksum = ((tmp_sum >> 8) & 0x3) + ((tmp_sum >> 4) & 0x0f) + (tmp_sum & 0x0f)

byte_fffbd = ((pll_checksum & 0x0f) << 4) | 0x0f

	.section ".a9123_crossbar_config", "a", @progbits

	.skip 0x3fdf0 - 0x3fd00, 0xff

.previous

	.section ".a9123_strap_1", "a", @progbits

	.byte byte_fffb6, byte_fffb7

.previous

	.section ".a9123_strap_2", "a", @progbits

	.byte byte_fffbb, byte_fffbc, byte_fffbd, byte_fffbe, byte_fffbf

.previous
