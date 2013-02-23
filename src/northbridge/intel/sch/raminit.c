/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

#include <string.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <spd.h>
#include "raminit.h"
#include "sch.h"

#define DEBUG_RAM_SETUP
#define SOFTSTRSP(base, off) *((volatile u8 *)((base) + (off)))

/* Debugging macros. */
#if defined(DEBUG_RAM_SETUP)
#define PRINTK_DEBUG(x...)	printk(BIOS_DEBUG, x)
#else
#define PRINTK_DEBUG(x...)
#endif

#define BOOT_MODE_RESUME	1
#define BOOT_MODE_NORMAL	0

#include "port_access.c"

static void detect_fsb(struct sys_info *sysinfo)
{
	u32 reg32;

	reg32 = sch_port_access_read(5, 3, 4);
	if (reg32 & BIT(3))
		sysinfo->fsb_frequency = 533;
	else
		sysinfo->fsb_frequency = 400;
}

static u32 detect_softstrap_base(void)
{
	u32 reg32, base_addr;

	reg32 = sch_port_access_read(4, 0x71, 2);
	reg32 &= 0x700;
	reg32 = reg32 >> 7;
	switch (reg32) {
	case 7:
		base_addr = 0xFFFB0000;
		break;
	case 6:
		base_addr = 0xFFFC0000;
		break;
	case 5:
		base_addr = 0xFFFD0000;
		break;
	case 4:
		base_addr = 0xFFFE0000;
		break;
	default:
		base_addr = 0;
		die("No valid softstrap base found.\n");
	}
	return base_addr;
}

static void detect_softstraps(struct sys_info *sysinfo)
{
	u8 reg8, temp;
	u32 sbase = detect_softstrap_base();

	reg8 = SOFTSTRSP(sbase, 0x87f2);
	sysinfo->ranks = reg8;
	if (reg8 == 0) {
		sysinfo->ram_param_source = RAM_PARAM_SOURCE_SPD;
		/* FIXME: Implement SPD reading. */
		die("No support for reading DIMM config from SPD yet!");
		return;
	} else {
		sysinfo->ram_param_source = RAM_PARAM_SOURCE_SOFTSTRAP;
		/* Timings from soft strap */
		reg8 = SOFTSTRSP(sbase, 0x87f0);
		temp = reg8 & 0x30;
		temp = temp >> 4;
		sysinfo->cl = temp;
		temp = reg8 & 0x0c;
		temp = temp >> 2;
		sysinfo->trcd = temp;
		temp = reg8 & 0x03;
		sysinfo->trp = temp;

		/* Geometry from Softstrap */
		reg8 = SOFTSTRSP(sbase, 0x87f1);

		temp = reg8 & 0x06;
		temp = temp >> 1;
		sysinfo->device_density = temp;

		temp = reg8 & 0x01;
		sysinfo->data_width = temp;

		/* Refresh rate default 7.8us */
		sysinfo->refresh = 3;
	}
}

static void program_sch_dram_data(struct sys_info *sysinfo)
{
	u32 reg32;

	/*
	 * Program DRP DRAM Rank Population and Interface Register as per data
	 * in sysinfo SCH port 1 register 0..0xFF.
	 */
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DRP, 4);
	reg32 &= ~(DRP_FIELDS);	/* Clear all DRP fields we'll change. */
	/* Rank0 Device Width, Density, Enable */
	reg32 |= sysinfo->data_width | (sysinfo->device_density << 1) | (1 << 3);
	/* Rank1 Device Width, Density, Enable */
	reg32 |= (sysinfo->data_width << 4)
		 | ((sysinfo->device_density) << 5) | (1 << 7);
	sch_port_access_write(SCH_MSG_DUNIT_PORT,
			      SCH_MSG_DUNIT_REG_DRP, 4, reg32);

	/*
	 * Program DTR DRAM Timing Register as per data in sysinfo SCH port 1
	 * register 1.
	 *
	 * tRD_dly = 2 (15:13 = 010b)
	 * 0X3F
	 */
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DTR, 4);
	reg32 &= ~(DTR_FIELDS);	/* Clear all DTR fields we'll change. */

	reg32 = (sysinfo->trp);
	reg32 |= (sysinfo->trcd) << 2;
	reg32 |= (sysinfo->cl) << 4;
	reg32 |= 0X4000;	/* tRD_dly = 2 (15:13 = 010b) */
	sch_port_access_write(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DTR, 4,
			      reg32);

	/*
	 * DCO DRAM Controller Operation Register as per data in sysinfo
	 * SCH port 1 register 2 0xF.
	 */
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DCO, 4);
	reg32 &= ~(DCO_FIELDS);	/* Clear all DTR fields we'll change. */

	if (sysinfo->fsb_frequency == 533)
		reg32 |= 1;
	else
		reg32 &= ~(BIT(0));
	reg32 = 0x006911c;	// FIXME ?

	sch_port_access_write(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DCO, 4,
			      reg32);
}

static void program_dll_config(struct sys_info *sysinfo)
{
	if (sysinfo->fsb_frequency == 533) {
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x21, 4, 0x46464646);
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x22, 4, 0x46464646);
	} else {
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x21, 4, 0x58585858);
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x22, 4, 0x58585858);
	}
	sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x23, 4, 0x2222);
	if (sysinfo->fsb_frequency == 533)
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x20, 4, 0x993B);
	else
		sch_port_access_write(SCH_MSG_DUNIT_PORT, 0x20, 4, 0xCC3B);
}

static void do_jedec_init(struct sys_info *sysinfo)
{
	u32 reg32, rank, cmd, temp, num_ranks;

	/* Performs JEDEC memory initializattion for all memory rows */
	/* Set CKE0/1 low */
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DRP, 4);
	reg32 |= DRP_CKE_DIS;
	sch_port_access_write(SCH_MSG_DUNIT_PORT,
			      SCH_MSG_DUNIT_REG_DRP, 4, reg32);
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DRP, 4);
	rank = 0;
	num_ranks = sysinfo->ranks;

	do {
		/* Start clocks */
		reg32 = sch_port_access_read(SCH_MSG_DUNIT_PORT,
					     SCH_MSG_DUNIT_REG_DRP, 4);
		reg32 &= ~(DRP_SCK_DIS); /* Enable all SCK/SCKB by def. */
		sch_port_access_write(1, SCH_MSG_DUNIT_REG_DRP, 4, reg32);
		/* Program misc. SCH registers on rank 0 initialization. */
		reg32 = sch_port_access_read(SCH_MSG_DUNIT_PORT,
					     SCH_MSG_DUNIT_REG_DRP, 4);
		if (rank == 0)
			program_dll_config(sysinfo);

		printk(BIOS_DEBUG, "Setting up RAM \n");

		/*
		 * Wait 200us
		 * reg32 = inb(ACPI_BASE + 8); PM1 Timer
		 * reg32 &=0xFFFFFF;
		 * reg32 +=0x2EE;
		 * do {
		 * 	reg32 = inb(ACPI_BASE + 8);PM1 Timer
		 * 	reg32 &= 0xFFFFFF;
		 * } while (reg32 < 0x2EE);
		 */

		/* Apply NOP. */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_NOP;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);
		/* Set CKE=high. */
		reg32 = sch_port_access_read(SCH_MSG_DUNIT_PORT,
					     SCH_MSG_DUNIT_REG_DRP, 4);
		reg32 &= 0xFFFF9FFF; /* Clear both the CKE static disables. */
		sch_port_access_write(SCH_MSG_DUNIT_PORT,
				      SCH_MSG_DUNIT_REG_DRP, 4, reg32);
		/*
		 * Wait 400ns (not needed when executing from flash).
		 * Precharge all.
		 */
		reg32 = sch_port_access_read(SCH_MSG_DUNIT_PORT,
					     SCH_MSG_DUNIT_REG_DRP, 4);
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_PALL;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/*
		 * EMRS(2); High temp self refresh=disabled,
		 * partial array self refresh=full.
		 */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_EMRS2;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* EMRS(3) (no command). */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_EMRS3;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* EMRS(1); Enable DLL (Leave all bits in the command at 0). */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_EMRS1;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* MRS; Reset DLL (Set memory address bit 8). */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_MRS;
		cmd |= (SCH_JEDEC_DLLRESET << SCH_DRAMINIT_ADDR_OFFSET);
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* Precharge all. */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_PALL;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* Issue 2 auto-refresh commands. */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_AREF;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* MRS command including tCL, tWR, burst length (always 4). */
		cmd = rank;
		cmd |= (SCH_DRAMINIT_CMD_MRS + JEDEC_STATIC_PARAM); /* Static param */
		temp = sysinfo->cl;
		temp += TCL_LOW;	/* Adjust for the TCL base. */
		temp = temp << ((SCH_JEDEC_CL_OFFSET
		      + SCH_DRAMINIT_ADDR_OFFSET)); /* Ready the CAS latency */
		cmd |= temp;
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/*
		 * Wait 200 clocks (max of 1us, so no need to delay).
		 * Issue EMRS(1):OCD default.
		 */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_EMRS1;
		cmd |= (SCH_JEDEC_OCD_DEFAULT << SCH_DRAMINIT_ADDR_OFFSET);
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);

		/* Issue EMRS(1): OCD cal. mode exit. */
		cmd = rank;
		cmd |= SCH_DRAMINIT_CMD_EMRS1;
		cmd |= (SCH_JEDEC_DQS_DIS << SCH_DRAMINIT_ADDR_OFFSET);
		sch_port_access_write_ram_cmd(SCH_OPCODE_DRAMINIT,
					      SCH_MSG_DUNIT_PORT, 0, cmd);
		rank += SCH_DRAMINIT_RANK_MASK;
		num_ranks--;
	} while (num_ranks);
}

/**
 * @param boot_mode 0 = normal, 1 = resume
 */
void sdram_initialize(int boot_mode)
{
	struct sys_info sysinfo;
	u32 reg32;

	printk(BIOS_DEBUG, "Setting up RAM controller.\n");

	memset(&sysinfo, 0, sizeof(sysinfo));

	detect_fsb(&sysinfo);
	detect_softstraps(&sysinfo);

	program_sch_dram_data(&sysinfo);

	/* cold boot */
	if (boot_mode == BOOT_MODE_NORMAL)
		do_jedec_init(&sysinfo);
	else
		program_dll_config(&sysinfo);

	/* RAM init complete. */
	reg32 =
	    sch_port_access_read(SCH_MSG_DUNIT_PORT, SCH_MSG_DUNIT_REG_DCO, 4);
	reg32 |= DCO_IC;
	reg32 |= ((sysinfo.refresh) << 2);
	reg32 = 0x006919c;
	sch_port_access_write(SCH_MSG_DUNIT_PORT,
			      SCH_MSG_DUNIT_REG_DCO, 4, reg32);

	/* Setting up TOM. */
	reg32 = 0x10000000;
	reg32 = reg32 >> sysinfo.data_width;
	reg32 = reg32 << sysinfo.device_density;
	reg32 = reg32 << sysinfo.ranks;
	reg32 = 0x40000000;
	sch_port_access_write(2, 8, 4, reg32);

	/* Resume mode. */
	if (boot_mode == BOOT_MODE_RESUME)
		sch_port_access_write_ram_cmd(SCH_OPCODE_WAKEFULLON,
					      SCH_MSG_DUNIT_PORT, 0, 0);

	sch_port_access_write(2, 0, 4, 0x98);
	sch_port_access_write(2, 3, 4, 0x7);
	sch_port_access_write(3, 2, 4, 0x408);
	sch_port_access_write(4, 0x71, 4, 0x600);
}
