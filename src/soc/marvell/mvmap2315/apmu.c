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
#include <soc/apmu.h>
#include <soc/clock.h>

void apmu_start(void)
{
	u32 reg;

	printk(BIOS_DEBUG, "APMU init...\n");

	/* TODO: remove in real silicon */
	write32(&mvmap2315_apmu_pwrctl->scratch0, 0x900dc0de);

	/* Dedicate LCM area to the APMU */
	reg = MVMAP2315_LCM_START_BANK |
	      MVMAP2315_LCM_END_BANK |
	      MVMAP2315_LCM_DIRACCESS_EN;
	write32(&mvmap2315_lcm_regs->lcm_access, reg);

	/* Taking APMU out of reset */
	setbits_le32(&mvmap2315_apmu_clk->apaonclk_apmucpu_clkgenconfig,
		     MVMAP2315_APMU_CLK_EN);

	setbits_le32(&mvmap2315_apmu_clk->apaonclk_apmucpu_clkgenconfig,
		     MVMAP2315_APMU_CLK_RSTN);
}

static int apmu_sending_cmd(u32 cmd)
{
	int success;
	u32 timeout = 0;

	printk(BIOS_DEBUG, "sending cmd: %x\n", cmd);

	/* Send a sync command */
	write32(&mvmap2315_apmu_pwrctl->pwrcmdfifo,
		cmd | MVMAP2315_APMU_PWRCMDFIFO_DONEIRQ);

	while (!(read32(&mvmap2315_apmu_pwrctl->intstatus) &
	       (MVMAP2315_APMU_ISTCLR_DONE |
	       MVMAP2315_APMU_ISTCLR_ERROR)) &&
	       (timeout < MVMAP2315_APMU_PWRCMDFIFO_TIMEOUT))
		timeout++;

	if (timeout >= MVMAP2315_APMU_PWRCMDFIFO_TIMEOUT) {
		printk(BIOS_DEBUG, "cmd Timed out\n");
		success = -2;
	}

	if (read32(&mvmap2315_apmu_pwrctl->intstatus) &
					MVMAP2315_APMU_ISTCLR_DONE) {
		printk(BIOS_DEBUG, "Completed cmd: %x\n",
		       read32(&mvmap2315_apmu_pwrctl->doneirq));

		setbits_le32(&mvmap2315_apmu_pwrctl->intstatusclear,
			     MVMAP2315_APMU_ISTCLR_DONE);
		success = 0;

	} else if (read32(&mvmap2315_apmu_pwrctl->intstatus) &
					MVMAP2315_APMU_ISTCLR_ERROR) {
		printk(BIOS_DEBUG, "Error cmd: %x\n",
		       read32(&mvmap2315_apmu_pwrctl->errorirq));

		setbits_le32(&mvmap2315_apmu_pwrctl->intstatusclear,
			     MVMAP2315_APMU_ISTCLR_ERROR);

		success = -1;
	}

	return success;
}

int apmu_set_dev(u32 dev, u32 state)
{
	u32 cmd;

	cmd = MVMAP2315_APMU_PWRCMDFIFO_DONEIRQ;
	cmd |= MVMAP2315_APMU_OPCODE_PWRST << MVMAP2315_APMU_OPCODE_SHIFT;
	cmd |= state << MVMAP2315_APMU_DEV_STATE_SHIFT;
	cmd |= dev << MVMAP2315_APMU_DEV_OBJECT_SHIFT;

	return apmu_sending_cmd(cmd);
}

int apmu_set_pll(u32 dev, u32 state, u32 freq)
{
	u32 cmd;

	cmd = MVMAP2315_APMU_PWRCMDFIFO_DONEIRQ;
	cmd |= MVMAP2315_APMU_OPCODE_PWRST << MVMAP2315_APMU_OPCODE_SHIFT;
	cmd |= freq << MVMAP2315_APMU_DEV_FREQ_SHIFT;
	cmd |= state << MVMAP2315_APMU_DEV_STATE_SHIFT;
	cmd |= dev << MVMAP2315_APMU_DEV_OBJECT_SHIFT;

	return apmu_sending_cmd(cmd);
}


int apmu_set_clk(u32 clk, u32 state, u32 div, u32 src)
{
	u32 cmd;

	cmd = MVMAP2315_APMU_PWRCMDFIFO_DONEIRQ;
	cmd |= MVMAP2315_APMU_OPCODE_CLKST << MVMAP2315_APMU_OPCODE_SHIFT;
	cmd |= state << MVMAP2315_APMU_CLK_STATE_SHIFT;
	cmd |= div << MVMAP2315_APMU_CLK_DIV_SHIFT;
	cmd |= src << MVMAP2315_APMU_CLK_SOURCE_SHIFT;
	cmd |= clk << MVMAP2315_APMU_CLK_OBJECT_SHIFT;

	return apmu_sending_cmd(cmd);
}
