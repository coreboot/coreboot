/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,  MA 02110-1301 USA
 */

#include "AGESA.h"
#include "amdlib.h"
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#if CONFIG_WRITE_HIGH_TABLES
#include <cbmem.h>
#endif
#include <device/device.h>
#include <device/pci.h>
#ifndef __PRE_RAM__
#include <device/pci_ops.h>
#endif
#include <arch/io.h>
#include <arch/acpi.h>
#include <string.h>
#include "Porting.h"
#include "BiosCallOuts.h"
#include "s3_resume.h"
#include "agesawrapper.h"

#ifndef __PRE_RAM__
#include <spi.h>
#include <spi_flash.h>
#endif

void restore_mtrr(void)
{
	u32 msr;
	volatile UINT32 *msrPtr = (volatile UINT32 *)S3_DATA_MTRR_POS;
	msr_t msr_data;

	printk(BIOS_SPEW, "%s\n", __func__);

	disable_cache();

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Now restore the Fixed MTRRs */
	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(0x250, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(0x258, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(0x259, msr_data);

	for (msr = 0x268; msr <= 0x26F; msr++) {
		msr_data.lo = *msrPtr;
		msrPtr ++;
		msr_data.hi = *msrPtr;
		msrPtr ++;
		wrmsr(msr, msr_data);
	}

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Restore the Variable MTRRs */
	for (msr = 0x200; msr <= 0x20F; msr++) {
		msr_data.lo = *msrPtr;
		msrPtr ++;
		msr_data.hi = *msrPtr;
		msrPtr ++;
		wrmsr(msr, msr_data);
	}

	/* Restore SYSCFG MTRR */
	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(SYS_CFG, msr_data);
}

inline void *backup_resume(void)
{
	unsigned long high_ram_base;
	void *resume_backup_memory;

	/* Start address of high memory tables */
	high_ram_base = (u32) get_cbmem_toc();

	/*
	 * printk(BIOS_DEBUG, "CBMEM TOC is at: %x\n", (u32_t)high_ram_base);
	 * printk(BIOS_DEBUG, "CBMEM TOC 0-size:%x\n ",(u32_t)(high_ram_base + HIGH_MEMORY_SIZE + 4096));
	 */

	cbmem_reinit((u64) high_ram_base);

	resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	if (((u32) resume_backup_memory == 0)
	    || ((u32) resume_backup_memory == -1)) {
		printk(BIOS_ERR, "Error: resume_backup_memory: %x\n",
		       (u32) resume_backup_memory);
		for (;;) ;
	}

	return resume_backup_memory;
}

void move_stack_high_mem(void)
{
	void *high_stack;

	high_stack = cbmem_find(CBMEM_ID_RESUME_SCRATCH);
	memcpy(high_stack, (void *)BSP_STACK_BASE_ADDR,
		(CONFIG_HIGH_SCRATCH_MEMORY_SIZE - BIOS_HEAP_SIZE));

	__asm__
	    volatile ("add	%0, %%esp; add %0, %%ebp; invd"::"g"
		      (high_stack - BSP_STACK_BASE_ADDR)
		      :);
}

void OemAgesaSaveMtrr(void)
{
#ifndef __PRE_RAM__
	msr_t  msr_data;
	u32 nvram_pos = S3_DATA_MTRR_POS;
	u32 i;
	struct spi_flash *flash;

	spi_init();

	flash = spi_flash_probe(0, 0, 0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return;
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Fixed MTRRs */
	msr_data = rdmsr(0x250);

	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	msr_data = rdmsr(0x258);
	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	msr_data = rdmsr(0x259);
	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	for (i = 0x268; i < 0x270; i++) {
		msr_data = rdmsr(i);
		flash->write(flash, nvram_pos, 4, &msr_data.lo);
		nvram_pos += 4;
		flash->write(flash, nvram_pos, 4, &msr_data.hi);
		nvram_pos += 4;
	}

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Variable MTRRs */
	for (i = 0x200; i < 0x210; i++) {
		msr_data = rdmsr(i);
		flash->write(flash, nvram_pos, 4, &msr_data.lo);
		nvram_pos += 4;
		flash->write(flash, nvram_pos, 4, &msr_data.hi);
		nvram_pos += 4;
	}

	/* SYS_CFG */
	msr_data = rdmsr(0xC0010010);
	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	/* TOM */
	msr_data = rdmsr(0xC001001A);
	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	/* TOM2 */
	msr_data = rdmsr(0xC001001D);
	flash->write(flash, nvram_pos, 4, &msr_data.lo);
	nvram_pos += 4;
	flash->write(flash, nvram_pos, 4, &msr_data.hi);
	nvram_pos += 4;

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);

#endif
}

void OemAgesaGetS3Info(S3_DATA_TYPE S3DataType, u32 *DataSize, void **Data)
{
	AMD_CONFIG_PARAMS StdHeader;
	if (S3DataType == S3DataTypeNonVolatile) {
		*Data = (void *)S3_DATA_NONVOLATILE_POS;
		*DataSize = *(UINTN *) (*Data);
		*Data += 4;
	} else {
		*DataSize = *(UINTN *) S3_DATA_VOLATILE_POS;
		*Data = (void *) GetHeapBase(&StdHeader);
		memcpy((void *)(*Data), (void *)(S3_DATA_VOLATILE_POS + 4), *DataSize);
	}
}

#ifndef __PRE_RAM__
u32 OemAgesaSaveS3Info(S3_DATA_TYPE S3DataType, u32 DataSize, void *Data)
{

	u32 pos = S3_DATA_VOLATILE_POS;
	u32 data;
	u32 nvram_pos;
	struct spi_flash *flash;

	if (S3DataType == S3DataTypeNonVolatile) {
		pos = S3_DATA_NONVOLATILE_POS;
	} else {		/* S3DataTypeVolatile */
		pos = S3_DATA_VOLATILE_POS;
	}

	spi_init();
	flash = spi_flash_probe(0, 0, 0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		/* Dont make flow stop. */
		return AGESA_SUCCESS;
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	if (S3DataType == S3DataTypeNonVolatile) {
		flash->erase(flash, S3_DATA_NONVOLATILE_POS, 0x1000);
	} else {
		flash->erase(flash, S3_DATA_VOLATILE_POS, 0x6000);
	}

	nvram_pos = 0;
	flash->write(flash, nvram_pos + pos, sizeof(DataSize), &DataSize);

	for (nvram_pos = 0; nvram_pos < DataSize; nvram_pos += 4) {
		data = *(u32 *) (Data + nvram_pos);
		flash->write(flash, nvram_pos + pos + 4, sizeof(u32), (u32 *)(Data + nvram_pos));
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);

	return AGESA_SUCCESS;
}
#endif

void set_resume_cache(void)
{
	msr_t msr;

	/* disable fixed mtrr for now,  it will be enabled by mtrr restore */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrFixDramModEn);
	wrmsr(SYSCFG_MSR, msr);

	/* Enable caching for 0 - coreboot ram using variable mtrr */
	msr.lo = 0 | MTRR_TYPE_WRBACK;
	msr.hi = 0;
	wrmsr(MTRRphysBase_MSR(0), msr);
	msr.lo = ~(CONFIG_RAMTOP - 1) | MTRRphysMaskValid;
	msr.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRRphysMask_MSR(0), msr);

	/* Set the default memory type and disable fixed and enable variable MTRRs */
	msr.hi = 0;
	msr.lo = (1 << 11);
	wrmsr(MTRRdefType_MSR, msr);

	enable_cache();
}

void s3_resume(void)
{
	int status;

	printk(BIOS_DEBUG, "agesawrapper_amds3laterestore ");
	status = agesawrapper_amds3laterestore();
	if (status)
		printk(BIOS_DEBUG, "error level: %x \n", (u32) status);
	else
		printk(BIOS_DEBUG, "passed.\n");
}
