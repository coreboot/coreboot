/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#if 0
#include <asm/io.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/dmc.h>
#endif
#include <arch/io.h>

#include <cpu/samsung/exynos5-common/clk.h>
#include <cpu/samsung/exynos5-common/clock.h>
#include <cpu/samsung/exynos5-common/cpu.h>

#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5-common/cpu.h>	/* for EXYNOS_PRO_ID */

/* FIXME(dhendrix): consolidate samsung ID code/#defines to a common location */
#include <cpu/samsung/exynos5250/setup.h>	/* cpu_info_init() prototype */

/*
 * The following CPU infos are initialized in lowlevel_init(). They should be
 * put in the .data section. Otherwise, a compile will put them in the .bss
 * section since they don't have initial values. The relocation code which
 * runs after lowlevel_init() will reset them to zero.
 */
unsigned int s5p_cpu_id __attribute__((section(".data")));
unsigned int s5p_cpu_rev __attribute__((section(".data")));

void cpu_info_init(void)
{
	s5p_set_cpu_id();
}

int s5p_get_cpu_id(void)
{
	return s5p_cpu_id;
}

int s5p_get_cpu_rev(void)
{
	return s5p_cpu_rev;
}

void s5p_set_cpu_id(void)
{
	s5p_cpu_id = readl((void *)EXYNOS_PRO_ID);
	s5p_cpu_id = (0xC000 | ((s5p_cpu_id & 0x00FFF000) >> 12));

	/*
	 * 0xC200: EXYNOS4210 EVT0
	 * 0xC210: EXYNOS4210 EVT1
	 */
	if (s5p_cpu_id == 0xC200) {
		s5p_cpu_id |= 0x10;
		s5p_cpu_rev = 0;
	} else if (s5p_cpu_id == 0xC210) {
		s5p_cpu_rev = 1;
	}
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	char buf[32];

	printf("CPU:   S5P%X @ %sMHz\n",
			s5p_cpu_id, strmhz(buf, get_arm_clk()));

	return 0;
}
#endif

#if 0
void board_show_dram(ulong size)
{
	enum ddr_mode mem_type;
	unsigned frequency_mhz;
	unsigned arm_freq;
	enum mem_manuf mem_manuf;
	char buf[32];
	int ret;

	/* Get settings from the fdt */
	ret = clock_get_mem_selection(&mem_type, &frequency_mhz,
				       &arm_freq, &mem_manuf);
	if (ret)
		panic("Invalid DRAM information");

	puts("DRAM:  ");
	print_size(size, " ");
	printf("%s %s @ %sMHz",
	       clock_get_mem_manuf_name(mem_manuf),
	       clock_get_mem_type_name(mem_type),
	       strmhz(buf, frequency_mhz));
	putc('\n');
}
#endif

#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	cpu_info_init();

	return 0;
}
#endif
