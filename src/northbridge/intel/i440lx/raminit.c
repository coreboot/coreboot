/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2009 Maciej Pijanka <maciej.pijanka@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <spd.h>
#include <delay.h>
#include <stdlib.h>
#include "i440lx.h"

/*-----------------------------------------------------------------------------
Macros and definitions.
-----------------------------------------------------------------------------*/

/* Uncomment this to enable debugging output. */

/* Debugging macros. */
#if CONFIG_DEBUG_RAM_SETUP
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX8(x)	print_debug_hex8(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#define PRINT_DEBUG_HEX32(x)	print_debug_hex32(x)
#define DUMPNORTH()		dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX8(x)
#define PRINT_DEBUG_HEX16(x)
#define PRINT_DEBUG_HEX32(x)
#define DUMPNORTH()
#endif

#define NB PCI_DEV(0, 0, 0)

/* DRAMXC[7:5] - DRAM extended control register (SMS). */
#define RAM_COMMAND_NORMAL	0x0
#define RAM_COMMAND_NOP		0x1 // (NOPCE)
#define RAM_COMMAND_PRECHARGE	0x2 // ABPCE
#define RAM_COMMAND_MRS		0x3 // MRSCE
#define RAM_COMMAND_CBR		0x4 // CBRC
// rest are reserved

/* Table format: register, bitmask, value. */
static const long register_values[] = {
	// ~0x02 == bit 9
	//  0x04 == bit 10
	// BASE is 0x8A but we dont want bit 9 or 10 have ENABLED so 0x8C
	PACCFG + 1, 0x38, 0x8c,

	DBC, 0x00, 0xC3,

	DRT,   0x00, 0xFF,
	DRT+1, 0x00, 0xFF,

	DRAMC, 0x00, 0x00, /* disable refresh for now. */
	DRAMT, 0x00, 0x00,

	PAM0, 0x00, 0x30, // everything is a mem
	PAM1, 0x00, 0x33,
	PAM2, 0x00, 0x33,
	PAM3, 0x00, 0x33,
	PAM4, 0x00, 0x33,
	PAM5, 0x00, 0x33,
	PAM6, 0x00, 0x33,

	/* Set the DRBs to zero for now, this will be fixed later. */
	DRB0, 0x00, 0x00,
	DRB1, 0x00, 0x00,
	DRB2, 0x00, 0x00,
	DRB3, 0x00, 0x00,
	DRB4, 0x00, 0x00,
	DRB5, 0x00, 0x00,
	DRB6, 0x00, 0x00,
	DRB7, 0x00, 0x00,

	/* No memory holes. */
	FDHC, 0x00, 0x00,
};

/*-----------------------------------------------------------------------------
SDRAM configuration functions.
-----------------------------------------------------------------------------*/

/**
 * Send the specified RAM command to all DIMMs.
 *
 * @param command The RAM command to send to the DIMM(s).
 */
static void do_ram_command(u32 command)
{
	int i, caslatency;
	u8 dimm_start, dimm_end;
	u16 reg16;
	u32 addr, addr_offset;

	/* Configure the RAM command. */
	reg16 = pci_read_config16(NB, DRAMXC);
	reg16 &= 0xff1f;		/* Clear bits 7-5. */
	reg16 |= (u16) (command << 5);	/* Write command into bits 7-5. */
	pci_write_config16(NB, DRAMXC, reg16);

	/*
	 * RAM_COMMAND_NORMAL affects only the memory controller and
	 * doesn't need to be "sent" to the DIMMs.
	 */
	if (command == RAM_COMMAND_NORMAL)
		return;

	/* Send the RAM command to each row of memory. */
	dimm_start = 0;
	for (i = 0; i < (DIMM_SOCKETS * 2); i++) {
                addr_offset = 0;
                caslatency = 3; /* TODO: Dynamically get CAS latency later. */

		/* before translation it is
		 *
		 * M[02:00] Burst Length
		 * M[03:03] Burst Type
		 * M[06:04] Cas Latency
		 *          000 - Reserved
		 *          001 - Reserved
		 *          010 - CAS 2
		 *          011 - CAS 3
		 *          100 - Reserved
		 *          101 - Reserved
		 *          110 - Reserved
		 *          111 - Reserved
		 * M[08:07] Op Mode
		 *          Must Be 00b (Defined mode)
		 * M[09:09] Write Burst Mode
		 *          0 - Programmed burst length
		 *          1 - Single location access
		 * M[11:10] Reserved
                 *          write 0 to ensure compatibility with....
		 */

		/* seems constructed value will be right shifted by 3 bit, thus constructed value
		 * must be left shifted by 3
		 * so possible formula is (caslatency <<4)|(burst_type << 1)|(burst length)
		 * then << 3 shift to compensate shift in Memory Controller
		 */
		if (command == RAM_COMMAND_MRS) {
			if (caslatency == 3)
				addr_offset = 0x1d0;
			if (caslatency == 2)
				addr_offset = 0x150;
		}

		dimm_end = pci_read_config8(NB, DRB + i);

		addr = (dimm_start * 8 * 1024 * 1024) + addr_offset;
		if (dimm_end > dimm_start) {
#if 0
			PRINT_DEBUG("    Sending RAM command 0x");
			PRINT_DEBUG_HEX16(reg16);
			PRINT_DEBUG(" to 0x");
			PRINT_DEBUG_HEX32(addr);
			PRINT_DEBUG("\n");
#endif

			read32(addr);
		}

		/* Set the start of the next DIMM. */
		dimm_start = dimm_end;
	}
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions.
-----------------------------------------------------------------------------*/

static void spd_enable_refresh(void)
{
	uint8_t reg;

	reg = pci_read_config8(NB, DRAMC);

	/* this chipset offer only two choices regarding refresh
	 * refresh disabled, or refresh normal
	 */

	pci_write_config8(NB, DRAMC, reg | 0x01);
	reg = pci_read_config8(NB, DRAMC);

	PRINT_DEBUG("spd_enable_refresh: dramc = 0x");
	PRINT_DEBUG_HEX8(reg);
	PRINT_DEBUG("\n");
}

/*-----------------------------------------------------------------------------
Public interface.
-----------------------------------------------------------------------------*/

static void northbridge_init(void)
{
	uint32_t reg32;

	reg32 = pci_read_config32(NB, APBASE);
	reg32 &= 0xe8000000U;
	pci_write_config32(NB, APBASE, reg32);

#if CONFIG_DEBUG_RAM_SETUP
	/*
	 * apbase dont get set still, no idea what i have doing wrong yet,
	 * i am almost sure that somehow i set it by mistake once, but can't
	 * repeat that.
	 */
	reg32 = pci_read_config32(NB, APBASE);
	PRINT_DEBUG("APBASE ");
	PRINT_DEBUG_HEX32(reg32);
	PRINT_DEBUG("\n");
#endif
}


/**
 * This routine sets RAM controller inside northbridge to known state
 *
 */
static void sdram_set_registers(void)
{
	int i, max;

	/* nice banner with FSB shown? do we have
	 * any standart policy about such things?
	 */
#if 0
	uint16_t reg16;
	reg16 = pci_read_config16(NB, PACCFG);
	printk(BIOS_DEBUG, "i82443LX Host Freq: 6%C MHz\n", (reg16 & 0x4000) ? '0' : '6');
#endif

	PRINT_DEBUG("Northbridge prior to SDRAM init:\n");
	DUMPNORTH();

	northbridge_init();

	max = ARRAY_SIZE(register_values);

	/* Set registers as specified in the register_values[] array. */
	for (i = 0; i < max; i += 3) {
		uint8_t reg,tmp;
		reg = pci_read_config8(NB, register_values[i]);
		reg &= register_values[i + 1];
		reg |= register_values[i + 2] & ~(register_values[i + 1]);
		pci_write_config8(NB, register_values[i], reg);

		/*
		 * i am not sure if that is needed, but was usefull
		 * for me to confirm what got written
		 */
#if CONFIG_DEBUG_RAM_SETUP
		PRINT_DEBUG("    Set register 0x");
		PRINT_DEBUG_HEX8(register_values[i]);
		PRINT_DEBUG(" to 0x");
		PRINT_DEBUG_HEX8(reg);
		tmp = pci_read_config8(NB, register_values[i]);
		PRINT_DEBUG(" readed 0x");
		PRINT_DEBUG_HEX8(tmp);
		if (tmp == reg) {
			PRINT_DEBUG(" OK ");
		} else {
			PRINT_DEBUG(" FAIL ");
		}
		PRINT_DEBUG("\n");
#endif
	}

	PRINT_DEBUG("Northbridge atexit sdram set registers\n");
	DUMPNORTH();
}


static void sdram_set_spd_registers(void)
{
	int i;
	u16 memsize = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		uint16_t ds = 0; // dimm size
		int j;
		/* this code skips second bank on each socket (no idea how to fix it now)
		 */

		PRINT_DEBUG("DIMM");
		PRINT_DEBUG_HEX8(i);
		PRINT_DEBUG(" rows: ");
		PRINT_DEBUG_HEX8(spd_read_byte(DIMM0 + i, SPD_NUM_DIMM_BANKS) & 0xFF);
		PRINT_DEBUG(" rowsize: ");
		PRINT_DEBUG_HEX8(spd_read_byte(DIMM0 + i, SPD_DENSITY_OF_EACH_ROW_ON_MODULE) & 0xFF);
		PRINT_DEBUG(" modulesize: ");

		j = spd_read_byte(DIMM0 + i, SPD_NUM_DIMM_BANKS);
		if (j < 0)
			j = 0;
		else
			ds = j;

		j = spd_read_byte(DIMM0 + i, SPD_DENSITY_OF_EACH_ROW_ON_MODULE);

		if (j < 0)
			j = 0;
		else
			ds = ds * (j >> 1); // convert from 4MB to 8MB units in place


		/* This is more or less crude hack
		 * allowing to run this target under qemu (even if that is not really
		 * same hardware emulated),
		 * probably some kconfig expert option should be added to enable/disable
		 * this nicelly
		 */
#if 0
		if (ds == 0 && memsize == 0)
			ds = 0x8;
#endif


		// todo: support for bank with not equal sizes as per jedec standart?

		/*
		 * because density is reported in units of 4Mbyte
		 * and rows in device are just value,
		 * and for setting registers we need value in 8Mbyte units
		 */

		PRINT_DEBUG_HEX16(ds);
		PRINT_DEBUG("\n");

		memsize += ds;

		pci_write_config8(NB, DRB + (2*i), memsize);
		pci_write_config8(NB, DRB + (2*i) + 1, memsize);
		if (ds > 0) {
			/* i have no idea why pci_read_config16 not work for
			 * me correctly here
			 */
			ds = pci_read_config8(NB, DRT+1);
			ds <<=8;
			ds |= pci_read_config8(NB, DRT);

			PRINT_DEBUG("DRT ");
			PRINT_DEBUG_HEX16(ds);

			ds &= ~(0x01 << (4 * i));

			PRINT_DEBUG(" ");
			PRINT_DEBUG_HEX16(ds);
			PRINT_DEBUG("\n");

			/*
			 * modify DRT register if current row isn't empty
			 * code assume its SDRAM plugged (should check if its sdram or EDO,
			 * edo would have 0x00 as constand instead 0x10 for SDRAM
			 * also this code is buggy because ignores second row of each dimm socket
			 */

			/* and as above write_config16 not work here too)
			 * pci_write_config16(NB, DRT, j);
			 */

			pci_write_config8(NB, DRT+1, ds >> 8);
			pci_write_config8(NB, DRT, ds & 0xFF);
		}
	}

#if 0
	PRINT_DEBUG("Mem: 0x");
	PRINT_DEBUG_HEX16(memsize * 8);
	PRINT_DEBUG(" MB\n");

	if (memsize == 0) {
		/* maybe we should use some nice die/hlt sequence with printing on console
		 * that no memory found, get lost, i can't run?
		 * maybe such event_handler can be commonly defined routine to decrease
		 * code duplication?
		 */
		PRINT_DEBUG("No memory detected via SPD\n");
		PRINT_DEBUG("Reverting to hardcoded 64M single side dimm in first bank\n");
	}
#endif

	/* Set DRAMC. Don't enable refresh for now. */
	pci_write_config8(NB, DRAMC, 0x00);

	/* Cas latency 3, and other shouldbe properly from spd too */
	pci_write_config8(NB, DRAMT, 0xAC);

	/* TODO? */
	pci_write_config8(NB, PCI_LATENCY_TIMER, 0x40);

	// set drive strength
	pci_write_config32(NB, MBSC, 0x00000000);
}

static void sdram_enable(void)
{
	int i;

	/* 0. Wait until power/voltages and clocks are stable (200us). */
	udelay(200);

	/* 1. Apply NOP. Wait 200 clock cycles (clock might be 60 or 66 Mhz). */
	PRINT_DEBUG("RAM Enable 1: Apply NOP\n");
	do_ram_command(RAM_COMMAND_NOP);
	udelay(200);

	/* 2. Precharge all. Wait tRP. */
	PRINT_DEBUG("RAM Enable 2: Precharge all\n");
	do_ram_command(RAM_COMMAND_PRECHARGE);
	udelay(1);

	/* 3. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG("RAM Enable 3: CBR\n");
	for (i = 0; i < 8; i++) {
		do_ram_command(RAM_COMMAND_CBR);
		udelay(1);
	}

	/* 4. Mode register set. Wait two memory cycles. */
	PRINT_DEBUG("RAM Enable 4: Mode register set\n");
	do_ram_command(RAM_COMMAND_MRS);
	udelay(2);

	/* 5. Normal operation. */
	PRINT_DEBUG("RAM Enable 5: Normal operation\n");
	do_ram_command(RAM_COMMAND_NORMAL);
	udelay(1);

	/* 6. Finally enable refresh. */
	PRINT_DEBUG("RAM Enable 6: Enable refresh\n");
	pci_write_config8(NB, DRAMC, 0x01);
	spd_enable_refresh();
	udelay(1);

	PRINT_DEBUG("Northbridge following SDRAM init:\n");
}

