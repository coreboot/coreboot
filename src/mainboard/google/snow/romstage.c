/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <system.h>
#include <cache.h>

#if 0
#include <arch/io.h>

/* FIXME: make i2c.h use standard types */
#define uchar unsigned char
#define uint  unsigned int
#include <device/i2c.h>

#include <cpu/samsung/s5p-common/s3c24x0_i2c.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5-common/uart.h>
#endif
#include <cbfs.h>
#include <common.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <console/console.h>
#include <arch/bootblock_exit.h>
#include <arch/stages.h>

void main(void)
{
	struct cbfs_media cbfs;
//	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
//	power_init();
//	clock_init();
//	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	console_init();
	printk(BIOS_INFO, "hello from romstage\n");
	struct mem_timings *mem;
	int ret;

	mem = clock_get_mem_timings();
	printk(BIOS_SPEW, "clock_get_mem_timings returns 0x%p\n", mem);
	printk(BIOS_SPEW, "man: 0x%x type: 0x%x, div: 0x%x, mhz: 0x%x\n",
		mem->mem_manuf,
		mem->mem_type,
		mem->mpll_mdiv,
		mem->frequency_mhz);

       ret = ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE);
       if (ret) {
               printk(BIOS_ERR, "Memory controller init failed, err: %x\n",
                      ret);
               while(1);
       }

       printk(BIOS_INFO, "ddr3_init done\n");
       /* wow, did it work? */
	int i;
	u32 *c = (void *)CONFIG_RAMBASE;

//	mmu_setup(CONFIG_SYS_SDRAM_BASE, CONFIG_DRAM_SIZE_MB * 1024);
//      printk(BIOS_INFO, "mmu_setup done\n");
	for(i = 0; i < 16384; i++)
		c[i] = i+32768;
	for(i = 0; i < 16384; i++)
		if (c[i] != i+32768)
			printk(BIOS_SPEW, "BADc[%02x]: %02x,", i, c[i]);
	for(i = 0; i < 1048576; i++)
		c[i] = 0;
	ret = init_default_cbfs_media(&cbfs);
	if (ret){
		printk(BIOS_ERR, "init_default_cbfs_media returned %d: HALT\n",
		       ret);
		while (1);
	}

	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(&cbfs, "fallback/coreboot_ram",
				      CBFS_TYPE_STAGE);
	printk(BIOS_ERR, "Stage: %p\n", stage);
	printk(BIOS_ERR, "loading stage %s @ 0x%x (0x%x bytes),entry @ 0x%p\n",
	       "ram stage",
	       (uint32_t) stage->load, stage->memlen,
	       (void *)(u32)stage->entry);

#if 0
	/* for reference and testing ... we should be able to remove soon */
//	c = (void *)(u32)(stage->load + stage->len);
	c = (void *)(u32)(stage->load);
	printk(BIOS_ERR, "memzero 0x%x words starting at %p\n",
	       (stage->memlen /*- stage->len*/)/4, c);
	for(i = 0; i < (stage->memlen /*- stage->len*/)/4; i++){
		printk(BIOS_INFO, "%p, ", &c[i]);
		c[i] = 0;
	}
#endif

	void *entry = cbfs_load_stage(&cbfs, "fallback/coreboot_ram");
	printk(BIOS_INFO, "entry is %p\n", entry);

	printk(BIOS_INFO, "sayonara, romstage!\n");
	stage_exit((unsigned long)entry);
}
