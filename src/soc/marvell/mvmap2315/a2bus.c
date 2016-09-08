/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/a2bus.h>

const u32 NUM_SIO = 11;
const u32 SIO_UIDS[] = {15, 14, 13, 12, 10, 8, 7, 5, 3, 1, 0};
const u32 SP_UID = 4;

static void a2bus_enable_and_check(void)
{
	u32 reg;
	u32 checkval;

	printk(BIOS_DEBUG, "Entering %s\n", __func__);

	/* FIXME - remove once default fixed */
	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win0);
	reg |= MVMAP2315_CCU_RGF_WIN0_AHR <<
	       MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win0, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win0);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win0, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN0_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN0_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN0_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win0) == checkval)
		printk(BIOS_DEBUG, " win0 checks\n");
	else
		printk(BIOS_DEBUG, " win0 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win0), checkval);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win1);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win1, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN1_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN1_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN1_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win1) == checkval)
		printk(BIOS_DEBUG, " win1 checks\n");
	else
		printk(BIOS_DEBUG, " win1 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win1), checkval);

	/* FIXME - remove once default fixed */
	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win2);
	reg |= MVMAP2315_CCU_RGF_WIN2_UID <<
	       MVMAP2315_CCU_RGF_WIN_UID_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win2, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win2);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win2, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN2_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN2_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN2_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win2) == checkval)
		printk(BIOS_DEBUG, " win2 checks\n");
	else
		printk(BIOS_DEBUG, " win2 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win2), checkval);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win3);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win3, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN3_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN3_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN3_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win3) == checkval)
		printk(BIOS_DEBUG, " win3 checks\n");
	else
		printk(BIOS_DEBUG, " win3 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win3), checkval);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win4);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win4, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN4_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN4_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN4_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win4) == checkval)
		printk(BIOS_DEBUG, " win4 checks\n");
	else
		printk(BIOS_DEBUG, " win4 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win4), checkval);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win5);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win5, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN5_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN5_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN5_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win5) == checkval)
		printk(BIOS_DEBUG, " win5 checks\n");
	else
		printk(BIOS_DEBUG, " win5 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win5), checkval);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win6);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win6, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN6_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN6_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN6_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win6) == checkval)
		printk(BIOS_DEBUG, " win6 checks\n");
	else
		printk(BIOS_DEBUG, " win6 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win6), checkval);

	/* FIXME - remove once default fixed */
	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win7);
	reg |= MVMAP2315_CCU_RGF_WIN7_UID <<
	       MVMAP2315_CCU_RGF_WIN_UID_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win7, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_rgf_win7);
	reg |= MVMAP2315_CCU_RGF_WIN_RGFWINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_rgf_win7, reg);

	checkval = ((MVMAP2315_CCU_RGF_WIN7_AHR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN7_ALR) <<
		MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT)
		| ((MVMAP2315_CCU_RGF_WIN7_UID) <<
		MVMAP2315_CCU_RGF_WIN_UID_SHIFT)
		| MVMAP2315_CCU_RGF_WIN_RGFWINEN;

	if (read32(&mvmap2315_a2bus_ccu->ccu_rgf_win7) == checkval)
		printk(BIOS_DEBUG, " win7 checks\n");
	else
		printk(BIOS_DEBUG, " win7 no go: 0x%08x != 0x%08x\n",
		       read32(&mvmap2315_a2bus_ccu->ccu_rgf_win7), checkval);

	printk(BIOS_DEBUG, "Exiting %s\n", __func__);
}

static void a2bus_open_map(void)
{
	u32 reg;

	printk(BIOS_DEBUG, "Entering %s\n", __func__);

	/* send traffic by defaul to AON (smc0) */
	reg = read32(&mvmap2315_a2bus_ccu->ccu_win_gcr);
	reg &= ~MVMAP2315_CCU_WIN_CR_WINTARGET_MASK;
	reg |= 2 << MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win_gcr, reg);

	printk(BIOS_DEBUG, "after CCU_WIN_GCR\n");

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win1_alr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRLOW_MASK;
	reg |= MVMAP2315_CCU_WIN1_ALR << MVMAP2315_CCU_WIN_ALR_ADDRLOW_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win1_alr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win1_ahr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRHIGH_MASK;
	reg |= MVMAP2315_CCU_WIN1_AHR << MVMAP2315_CCU_WIN_AHR_ADDRHIGH_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win1_ahr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win1_cr);
	reg &= ~MVMAP2315_CCU_WIN_CR_WINTARGET_MASK;
	reg |= MVMAP2315_CCU_WIN1_UID << MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win1_cr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win1_cr);
	reg |= MVMAP2315_CCU_CR_WINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_win1_cr, reg);

	printk(BIOS_DEBUG, "after CCU_WIN1\n");

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win2_alr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRLOW_MASK;
	reg |= MVMAP2315_CCU_WIN2_ALR << MVMAP2315_CCU_WIN_ALR_ADDRLOW_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win2_alr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win2_ahr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRHIGH_MASK;
	reg |= MVMAP2315_CCU_WIN2_AHR << MVMAP2315_CCU_WIN_AHR_ADDRHIGH_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win2_ahr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win2_cr);
	reg &= ~MVMAP2315_CCU_WIN_CR_WINTARGET_MASK;
	reg |= MVMAP2315_CCU_WIN2_UID << MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win2_cr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win2_cr);
	reg &= ~MVMAP2315_CCU_CR_WINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_win2_cr, reg);

	printk(BIOS_DEBUG, "after CCU_WIN2\n");

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win3_alr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRLOW_MASK;
	reg |= MVMAP2315_CCU_WIN3_ALR << MVMAP2315_CCU_WIN_ALR_ADDRLOW_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win3_alr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win3_ahr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRHIGH_MASK;
	reg |= MVMAP2315_CCU_WIN3_AHR << MVMAP2315_CCU_WIN_AHR_ADDRHIGH_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win3_ahr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win3_cr);
	reg &= ~MVMAP2315_CCU_WIN_CR_WINTARGET_MASK;
	reg |= MVMAP2315_CCU_WIN3_UID << MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win3_cr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win3_cr);
	reg |= MVMAP2315_CCU_CR_WINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_win3_cr, reg);

	printk(BIOS_DEBUG, "after CCU_WIN3\n");

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win4_alr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRLOW_MASK;
	reg |= MVMAP2315_CCU_WIN4_ALR << MVMAP2315_CCU_WIN_ALR_ADDRLOW_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win4_alr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win4_ahr);
	reg &= ~MVMAP2315_CCU_WIN_ALR_ADDRHIGH_MASK;
	reg |= MVMAP2315_CCU_WIN4_AHR << MVMAP2315_CCU_WIN_AHR_ADDRHIGH_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win4_ahr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win4_cr);
	reg &= ~MVMAP2315_CCU_WIN_CR_WINTARGET_MASK;
	reg |= MVMAP2315_CCU_WIN4_UID << MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT;
	write32(&mvmap2315_a2bus_ccu->ccu_win4_cr, reg);

	reg = read32(&mvmap2315_a2bus_ccu->ccu_win4_cr);
	reg |= MVMAP2315_CCU_CR_WINEN;
	write32(&mvmap2315_a2bus_ccu->ccu_win4_cr, reg);

	printk(BIOS_DEBUG, "after CCU_WIN4\n");

	/* open all register space */
	a2bus_enable_and_check();

	printk(BIOS_DEBUG, "Exiting %s\n", __func__);
}

void init_a2bus_config(void)
{
	u32 reg;
	int i;

	/* Open up the A2 address map */
	a2bus_open_map();

	/* set the bit to enable long non-modifiable */
	for (i = 0; i < NUM_SIO; i++) {
		printk(BIOS_DEBUG, "Writing LTC_ForceModifiable to SIO %d\n",
		       SIO_UIDS[i]);

		reg = read32(&mvmap2315_a2bus_banked->ccu_b_idacr);
		reg |= SIO_UIDS[i] << MVMAP2315_CCU_B_IDACR_PRIVATEUID_SHIFT;
		write32(&mvmap2315_a2bus_banked->ccu_b_idacr, reg);

		reg = read32(&mvmap2315_a2bus_banked->ccu_b_ltc_cr);
		reg |= MVMAP2315_CCU_B_LTC_CR_USERDMODROB;
		write32(&mvmap2315_a2bus_banked->ccu_b_ltc_cr, reg);
	}

	printk(BIOS_DEBUG, "Writing LTC_ForceModifiable to SP\n");

	reg = read32(&mvmap2315_a2bus_banked->ccu_b_idacr);
	reg |= SP_UID << MVMAP2315_CCU_B_IDACR_PRIVATEUID_SHIFT;
	write32(&mvmap2315_a2bus_banked->ccu_b_idacr, reg);

	reg = read32(&mvmap2315_a2bus_banked->ccu_b_ltc_cr);
	reg |= MVMAP2315_CCU_B_LTC_CR_USERDMODROB;
	write32(&mvmap2315_a2bus_banked->ccu_b_ltc_cr, reg);

	printk(BIOS_DEBUG, "FIXME - DISABLING MVN\n");

	reg = read32(&mvmap2315_a2bus_alias6->ccu_b_smc_mvn);
	reg &= ~MVMAP2315_CCU_B_SMC_MVN_MVNENABLE;
	write32(&mvmap2315_a2bus_alias6->ccu_b_smc_mvn, reg);

	reg = read32(&mvmap2315_a2bus_alias9->ccu_b_smc_mvn);
	reg &= ~MVMAP2315_CCU_B_SMC_MVN_MVNENABLE;
	write32(&mvmap2315_a2bus_alias9->ccu_b_smc_mvn, reg);

	reg = read32(&mvmap2315_a2bus_alias11->ccu_b_smc_mvn);
	reg &= ~MVMAP2315_CCU_B_SMC_MVN_MVNENABLE;
	write32(&mvmap2315_a2bus_alias11->ccu_b_smc_mvn, reg);
}
