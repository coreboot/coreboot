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

#include <AGESA.h>
#include <Lib/amdlib.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cbmem.h>
#include <device/device.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <string.h>
#include "Porting.h"
#include "BiosCallOuts.h"
#include "s3_resume.h"
#include "agesawrapper.h"

#ifndef __PRE_RAM__
#include <spi-generic.h>
#include <spi_flash.h>
#endif

/* The size needs to be 4k aligned, which is the sector size of most flashes. */
#define S3_DATA_VOLATILE_SIZE		0x6000
#define S3_DATA_MTRR_SIZE		0x1000
#define S3_DATA_NONVOLATILE_SIZE	0x1000

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME) && \
	(S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE + S3_DATA_NONVOLATILE_SIZE) > CONFIG_S3_DATA_SIZE
#error "Please increase the value of S3_DATA_SIZE"
#endif

static void get_s3nv_data(S3_DATA_TYPE S3DataType, u32 *pos, u32 *len)
{
	/* FIXME: Find file from CBFS. */
	u32 s3_data = CONFIG_S3_DATA_POS;

	switch (S3DataType) {
	case S3DataTypeVolatile:
		*pos = s3_data;
		*len = S3_DATA_VOLATILE_SIZE;
		break;
	case S3DataTypeMTRR:
		*pos = s3_data + S3_DATA_VOLATILE_SIZE;
		*len = S3_DATA_MTRR_SIZE;
		break;
	case S3DataTypeNonVolatile:
		*pos = s3_data + S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE;
		*len = S3_DATA_NONVOLATILE_SIZE;
		break;
	default:
		*pos = 0;
		*len = 0;
		break;
	}
}

void restore_mtrr(void)
{
	u32 msr;
	volatile UINT32 *msrPtr;
	msr_t msr_data;

	printk(BIOS_SPEW, "%s\n", __func__);

	u32 pos, size;
	get_s3nv_data(S3DataTypeMTRR, &pos, &size);
	msrPtr = (UINT32 *)pos;

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

void *backup_resume(void)
{
	void *resume_backup_memory;

	if (cbmem_recovery(1))
		return NULL;

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

#ifndef __PRE_RAM__
static void write_mtrr(struct spi_flash *flash, u32 *p_nvram_pos, unsigned idx)
{
	msr_t  msr_data;
	msr_data = rdmsr(idx);

#if CONFIG_AMD_SB_SPI_TX_LEN >= 8
	flash->write(flash, *p_nvram_pos, 8, &msr_data);
	*p_nvram_pos += 8;
#else
	flash->write(flash, *p_nvram_pos, 4, &msr_data.lo);
	*p_nvram_pos += 4;
	flash->write(flash, *p_nvram_pos, 4, &msr_data.hi);
	*p_nvram_pos += 4;
#endif
}
#endif

void OemAgesaSaveMtrr(void)
{
#ifndef __PRE_RAM__
	msr_t  msr_data;
	u32 i;
	struct spi_flash *flash;

	u32 pos, size;
	get_s3nv_data(S3DataTypeMTRR, &pos, &size);

	spi_init();

	flash = spi_flash_probe(0, 0, 0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return;
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	flash->erase(flash, pos, size);
	u32 nvram_pos = pos;

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Fixed MTRRs */
	write_mtrr(flash, &nvram_pos, 0x250);
	write_mtrr(flash, &nvram_pos, 0x258);
	write_mtrr(flash, &nvram_pos, 0x259);

	for (i = 0x268; i < 0x270; i++)
		write_mtrr(flash, &nvram_pos, i);

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYS_CFG);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYS_CFG, msr_data);

	/* Variable MTRRs */
	for (i = 0x200; i < 0x210; i++)
		write_mtrr(flash, &nvram_pos, i);

	/* SYS_CFG */
	write_mtrr(flash, &nvram_pos, 0xC0010010);
	/* TOM */
	write_mtrr(flash, &nvram_pos, 0xC001001A);
	/* TOM2 */
	write_mtrr(flash, &nvram_pos, 0xC001001D);

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);

#endif
}

void OemAgesaGetS3Info(S3_DATA_TYPE S3DataType, u32 *DataSize, void **Data)
{
	AMD_CONFIG_PARAMS StdHeader;

	u32 pos, size;
	get_s3nv_data(S3DataType, &pos, &size);

	if (S3DataType == S3DataTypeNonVolatile) {
		*DataSize = *(UINT32 *) pos;
		*Data = (void *) (pos + sizeof(UINT32));
	} else if (S3DataType == S3DataTypeVolatile) {
		u32 len = *(UINT32 *) pos;
		void *src = (void *) (pos + sizeof(UINT32));
		void *dst = (void *) GetHeapBase(&StdHeader);
		memcpy(dst, src, len);
		*DataSize = len;
		*Data = dst;
	}
}

#ifndef __PRE_RAM__
u32 OemAgesaSaveS3Info(S3_DATA_TYPE S3DataType, u32 DataSize, void *Data)
{
	struct spi_flash *flash;

	u32 pos, size;
	get_s3nv_data(S3DataType, &pos, &size);

	spi_init();
	flash = spi_flash_probe(0, 0, 0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		/* Dont make flow stop. */
		return AGESA_SUCCESS;
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	flash->erase(flash, pos, size);
	flash->write(flash, pos, sizeof(DataSize), &DataSize);

	u32 nvram_pos;
	for (nvram_pos = 0; nvram_pos < DataSize - CONFIG_AMD_SB_SPI_TX_LEN; nvram_pos += CONFIG_AMD_SB_SPI_TX_LEN) {
		flash->write(flash, nvram_pos + pos + 4, CONFIG_AMD_SB_SPI_TX_LEN, (u8 *)(Data + nvram_pos));
	}
	flash->write(flash, nvram_pos + pos + 4, DataSize % CONFIG_AMD_SB_SPI_TX_LEN, (u8 *)(Data + nvram_pos));

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
