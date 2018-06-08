/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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
#include <bootstate.h>
#include <chip.h>
#include <console/console.h>
#include <console/post_codes.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <reg_script.h>
#include <spi-generic.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/systemagent.h>
#include <stdlib.h>

#define CAMERA1_CLK		0x8000 /* Camera 1 Clock */
#define CAMERA2_CLK		0x8080 /* Camera 2 Clock */
#define CAM_CLK_EN		(1 << 1)
#define MIPI_CLK		(1 << 0)
#define HDPLL_CLK		(0 << 0)

static void pch_enable_isclk(void)
{
	pcr_or32(PID_ISCLK, CAMERA1_CLK, CAM_CLK_EN | MIPI_CLK);
	pcr_or32(PID_ISCLK, CAMERA2_CLK, CAM_CLK_EN | MIPI_CLK);
}

static void pch_handle_sideband(config_t *config)
{
	if (config->pch_isclk)
		pch_enable_isclk();
}

static void pch_finalize(void)
{
	device_t dev;
	uint32_t reg32;
	uint16_t tcobase, tcocnt;
	uint8_t *pmcbase;
	config_t *config;
	uint8_t reg8;

	/* TCO Lock down */
	tcobase = smbus_tco_regs();
	tcocnt = inw(tcobase + TCO1_CNT);
	tcocnt |= TCO_LOCK;
	outw(tcocnt, tcobase + TCO1_CNT);

	/*
	 * Disable ACPI PM timer based on dt policy
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO
	 */
	dev = PCH_DEV_PMC;
	config = dev->chip_info;
	pmcbase = pmc_mmio_regs();
	if (config->PmTimerDisabled) {
		reg8 = read8(pmcbase + PCH_PWRM_ACPI_TMR_CTL);
		reg8 |= (1 << 1);
		write8(pmcbase + PCH_PWRM_ACPI_TMR_CTL, reg8);
	}

	/* Disable XTAL shutdown qualification for low power idle. */
	if (config->s0ix_enable) {
		reg32 = read32(pmcbase + CPPMVRIC);
		reg32 |= XTALSDQDIS;
		write32(pmcbase + CPPMVRIC, reg32);
	}

	pch_handle_sideband(config);
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();

	printk(BIOS_DEBUG, "Finalizing SMM.\n");
	outb(APM_CNT_FINALIZE, APM_CNT);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
