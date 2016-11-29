/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <stddef.h>
#include <arch/cpu.h>
#include <lib.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/early_variables.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/acpi.h>
#include <soc/baytrail.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <soc/pmc.h>
#include <soc/spi.h>
#include <version.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>

/* Return 0, 3, 4 or 5 to indicate the previous sleep state. */
uint32_t chipset_prev_sleep_state(uint32_t clear)
{
	/* Default to S0. */
	uint32_t prev_sleep_state = ACPI_S0;
	uint32_t pm1_sts;
	uint32_t pm1_cnt;
	uint32_t gen_pmcon1;

	/* Read Power State */
	pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	gen_pmcon1 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON1));

	printk(BIOS_DEBUG, "PM1_STS = 0x%x PM1_CNT = 0x%x GEN_PMCON1 = 0x%x\n",
		pm1_sts, pm1_cnt, gen_pmcon1);

	if (pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(pm1_cnt)) {
		case ACPI_S3:
			if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S4:
			prev_sleep_state = ACPI_S4;
			break;

		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}
		/* If set Clear SLP_TYP. */
		if (clear == 1) {
			outl(pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
		}
	}

	if (gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR)) {
		prev_sleep_state = ACPI_S5;
	}

	return prev_sleep_state;
}

static void program_base_addresses(void)
{
	uint32_t reg;

	/* Memory Mapped IO registers. */
	reg = PMC_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, PBASE, reg);
	reg = IO_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, IOBASE, reg);
	reg = ILB_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, IBASE, reg);
	reg = SPI_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, SBASE, reg);
	reg = MPHY_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, MPBASE, reg);
	reg = PUNIT_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, PUBASE, reg);
	reg = RCBA_BASE_ADDRESS | RCBA_ENABLE;
	pci_write_config32(LPC_BDF, RCBA, reg);

	/* IO Port Registers. */
	reg = ACPI_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, ABASE, reg);
	reg = GPIO_BASE_ADDRESS | SET_BAR_ENABLE;
	pci_write_config32(LPC_BDF, GBASE, reg);
}

static void spi_init(void)
{
	uint32_t *scs = (uint32_t *)(SPI_BASE_ADDRESS + SCS);
	uint32_t *bcr = (uint32_t *)(SPI_BASE_ADDRESS + BCR);
	uint32_t reg;

	/* Disable generating SMI when setting WPD bit. */
	write32(scs, read32(scs) & ~SMIWPEN);
	/*
	 * Enable caching and prefetching in the SPI controller. Disable
	 * the SMM-only BIOS write and set WPD bit.
	 */
	reg = (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH | BCR_WPD;
	reg &= ~EISS;
	write32(bcr, reg);
}

static void baytrail_rtc_init(void)
{
	uint32_t *pbase = (uint32_t *)(pci_read_config32(LPC_BDF, PBASE) & 0xfffffff0);
	uint32_t gen_pmcon1 = read32(pbase + (GEN_PMCON1/sizeof(u32)));
	int rtc_failed = !!(gen_pmcon1 & RPS);

	if (rtc_failed) {
		printk(BIOS_DEBUG,
			"RTC Failure detected.  Resetting Date to %s\n",
			coreboot_dmi_date);

		write32((uint32_t *)(DEFAULT_PBASE + GEN_PMCON1), gen_pmcon1 & ~RPS);
	}

	cmos_init(rtc_failed);
}

/* Entry from cache-as-ram.inc. */
void main(FSP_INFO_HEADER *fsp_info_header)
{
	uint32_t *func_dis = (uint32_t *)(PMC_BASE_ADDRESS + FUNC_DIS);
	uint32_t *func_dis2 = (uint32_t *)(PMC_BASE_ADDRESS + FUNC_DIS2);
	uint32_t fd_mask = 0;
	uint32_t fd2_mask = 0;

	post_code(0x40);

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	program_base_addresses();

	post_code(0x41);
	tco_disable();

	post_code(0x42);
	byt_config_com1_and_enable();

	post_code(0x43);
	console_init();

	spi_init();
	baytrail_rtc_init();

	/* Call into mainboard. */
	early_mainboard_romstage_entry();

	set_max_freq();

	post_code(0x44);

	/* Program any required function disables */
	get_func_disables(&fd_mask, &fd2_mask);

	if (fd_mask != 0) {
		write32(func_dis, read32(func_dis) | fd_mask);
		/* Ensure posted write hits. */
		read32(func_dis);
	}

	if (fd2_mask != 0) {
		write32(func_dis2, read32(func_dis2) | fd2_mask);
		/* Ensure posted write hits. */
		read32(func_dis2);
	}

	post_code(0x47);

	timestamp_add_now(TS_BEFORE_INITRAM);

  /*
   * Call early init to initialize memory and chipset. This function returns
   * to the romstage_main_continue function with a pointer to the HOB
   * structure.
   */
	post_code(0x48);
	printk(BIOS_DEBUG, "Starting the Intel FSP (early_init)\n");
	fsp_early_init(fsp_info_header);
	die("Uh Oh! fsp_early_init should not return here.\n");
}

/*******************************************************************************
 * The FSP early_init function returns to this function.
 * Memory is setup and the stack is set by the FSP.
 */
void romstage_main_continue(EFI_STATUS status, void *hob_list_ptr)
{
	int cbmem_was_initted;
	void *cbmem_hob_ptr;
	uint32_t prev_sleep_state;

	timestamp_add_now(TS_AFTER_INITRAM);

	post_code(0x4a);
	printk(BIOS_DEBUG, "%s status: %x  hob_list_ptr: %x\n",
		__func__, (u32) status, (u32) hob_list_ptr);

#if IS_ENABLED(CONFIG_USBDEBUG_IN_ROMSTAGE)
	/* FSP reconfigures USB, so reinit it to have debug */
	usbdebug_init();
#endif	/* IS_ENABLED(CONFIG_USBDEBUG_IN_ROMSTAGE) */

	printk(BIOS_DEBUG, "FSP Status: 0x%0x\n", (u32)status);

	/* Get previous sleep state again and clear */
	prev_sleep_state = chipset_prev_sleep_state(1);
	printk(BIOS_DEBUG, "%s: prev_sleep_state = S%d\n", __func__, prev_sleep_state);

	report_platform_info();

	post_code(0x4b);

	late_mainboard_romstage_entry();
	post_code(0x4c);

	/* if S3 resume skip RAM check */
	if (prev_sleep_state != ACPI_S3) {
		quick_ram_check();
		post_code(0x4d);
	}

	cbmem_was_initted = !cbmem_recovery(prev_sleep_state == ACPI_S3);

	/* Save the HOB pointer in CBMEM to be used in ramstage*/
	cbmem_hob_ptr = cbmem_add (CBMEM_ID_HOB_POINTER, sizeof(*hob_list_ptr));
	*(u32*)cbmem_hob_ptr = (u32)hob_list_ptr;
	post_code(0x4e);

	romstage_handoff_init(prev_sleep_state == ACPI_S3);

	post_code(0x4f);

	/* Load the ramstage. */
	run_ramstage();
	while (1);
}

uint64_t get_initial_timestamp(void)
{
	return 0;
}
