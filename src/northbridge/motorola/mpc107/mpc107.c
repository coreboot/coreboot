/*
 * (C) Copyright 2001
 * Humboldt Solutions Ltd, adrian@humboldt.co.uk.
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
#include <stdint.h>
#include <ppc.h>
#include <string.h>
#include <printk.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <timer.h>
#include <clock.h>
#include <mem.h>
#include "i2c.h"
#include "mpc107.h"

#define NUM_DIMMS	1
#define NUM_BANKS	2

void mpc107_init(void);

void 
memory_init(void)
{
	struct sdram_dimm_info dimms[NUM_DIMMS];
	struct sdram_bank_info banks[NUM_BANKS];

	mpc107_init();
	mpc107_probe_dimms(NUM_DIMMS, dimms, banks);
	(void)mpc107_config_memory(NUM_BANKS, banks, 2);
}

struct mem_range *
sizeram(void)
{
	int i;
	struct sdram_dimm_info dimms[NUM_DIMMS];
	struct sdram_bank_info banks[NUM_BANKS];
	static struct mem_range meminfo;

	meminfo.basek = 0;
	meminfo.sizek = 0;

	mpc107_probe_dimms(NUM_DIMMS, dimms, banks);

	for (i = 0; i < NUM_BANKS; i++)
		meminfo.sizek += banks[i].size;

	meminfo.sizek >>= 10;

	return &meminfo;
}

/*
 * Configure the MPC107 with the most pessimistic settings. These
 * are modified by reading the SPD EEPROM and adjusting accordingly.
 * One thing to note. The SPD settings can be wrong, as 
 * was the case for my machine. I could only configure 32Mb out of
 * 128Mb using these settings.
*/
void
mpc107_init(void)
{
	uint16_t reg16;
	uint32_t reg32;
	uint32_t is32bits;
	
        /* 
	 * PCI Cmd 
	 */
	pci_ppc_write_config16(0, 0, 0x04, 0x0006);
        
        /*
	 * PCI Stat 
	 */
	reg16 = pci_ppc_read_config16(0, 0, 0x06);
	reg16 |= 0xffff;
	pci_ppc_write_config16(0, 0, 0x06, reg16);
	
        /* 
	 * PICR1 
	 *      0x00400000	burst read wait states = 1
	 *      0x00040000	processor type = 603/750
	 *      0x00002000	enable LocalBusSlave
	 *      0x00001000	enable Flash write
	 *      0x00000800	enable MCP* assertion
	 *      0x00000400	enable TEA* assertion
	 *      0x00000200	enable data bus parking
	 *      0x00000080	restrict flash write width
	 *      0x00000040	enable PCI store gathering
	 *      0x00000010	enable loop-snoop
	 *      0x00000008	enable address bus parking
	 *      0x00000004	enable speculative PCI reads
	 */
	reg32 = pci_ppc_read_config32(0, 0, 0xa8);

	/*
	 * Preserve RCS0/Addr Map bits
	 */
	reg32 &= 0x11;

	reg32 |= 0xff041a98;
	pci_ppc_write_config32(0, 0, 0xa8, reg32);

        /* 
	 * PICR2 
	 *      0x20000000	no Serialize Config cycles
	 *      0x08000000	No PCI Snoop cycles
	 *      0x04000000	FF0 is Local ROM
	 *      0x02000000	Flash write lockout
	 *      0x00000000	snoop wt states = 0
	 *      0x00040000	snoop wt states = 1
	 *      0x00080000	snoop wt states = 2
	 *      0x000c0000	snoop wt states = 3
	 *      0x00000000	addr phase wt states = 0
	 *      0x00000004	addr phase wt states = 1
	 *      0x00000008	addr phase wt states = 2
	 *      0x0000000c	addr phase wt states = 3
	 */
	reg32 = 0x04000000;
	pci_ppc_write_config32(0, 0, 0xac, reg32);

        /*
	 * EUMBBAR 
	 */
	reg32 = 0xfc000000;
	pci_ppc_write_config32(0, 0, 0x78, reg32);
	
        /*
	 * MCCR1 - Set MEMGO bit later!
	 *      0x75800000	Safe local ROM = 10+3 clocks
	 *      0x73800000	Fast local ROM = 7+3 clocks
	 *      0x00100000	Burst ROM/Flash enable
	 *      0x00040000	Self-refresh enable
	 *      0x00020000	EDO/FP enable (else SDRAM)
	 *      0x00010000	Parity check
	 *      0x0000FFFF	16Mbit/2 bank SDRAM
	 *      0x0000AAAA	256Mbit/4 bank SDRAM
	 *      0x00005555	64Mbit/2 bank SDRAM
	 *      0x00000000	64Mbit/4 bank SDRAM
	 */
	reg32 = pci_ppc_read_config32(0, 0, 0xf0);
	is32bits = (reg32 & 0x00400000) == 0;
	reg32 &= 0x00080000; /* Preserve MEMGO bit in case we're in RAM */
	reg32 |= 0x75800000;
	pci_ppc_write_config32(0, 0, 0xf0, reg32);

        /* 
	 * MCCR2 
	 *      0x40000000	TS_WAIT_TIMER = 3 clocks
	 *      0x04000000	ASRISE = 2 clocks
	 *      0x00400000	ASFALL = 2 clocks
	 *      0x00100000	SDRAM parity (else ECC)
	 *      0x00080000	SDRAM inline writes
	 *      0x00040000	SDRAM inline reads
	 *      0x00020000	ECC enable
	 *      0x00010000	EDO (else FP)
	 *      0x000004cc	Refresh 33MHz bus (307 clocks)
	 *      0x00000cc4	Refresh 66MHz bus (817 clocks)
	 *      0x000010fc	Refresh 83MHz bus (1087 clocks)
	 *      0x0000150c	Refresh 100MHz bus (1347 clocks)
	 *      0x00001d2c	Refresh 133MHz bus (1867 clocks)
	 *      0x00000002	Reserve a page
	 *      0x00000001	RWM parity
	 */
	reg32 = 0x044004cc;
	pci_ppc_write_config32(0, 0, 0xf4, reg32);

	/*
	 * MCCR3
	 *	0x70000000	BSTOPRE_M = 7
	 *      0x08000000	REFREC = 8 clocks
	 *      0x00400000	RDLAT = 4 clocks
	 *      0x00300000	RDLAT = 3 clocks
	 */
	reg32 = 0x78400000;
	pci_ppc_write_config32(0, 0, 0xf8, reg32);

        /* 
	 * MCCR4 
	 *	0x30000000	PRETOACT = 3 clocks
	 *      0x05000000	ACTOPRE = 5 clocks
	 *      0x00800000	Enable 8-beat burst (32-bit bus)
	 *      0x00400000	Enable Inline ECC/Parity
	 *      0x00200000	Enable Extended ROM (RCS2/RCS3)
	 *      0x00100000	Registered buffers
	 *      0x00000000	BSTOPRE_U = 0
	 *      0x00020000	Change RCS1 to 8-bit mode
	 *      0x00008000	Registered DIMMs
	 *      0x00003000	CAS Latencey (CL=3)
	 *      0x00002000	CAS Latencey (CL=2)
	 *      0x00000200	Sequential wrap/4-beat burst
	 *      0x00000300	Sequential wrap/8-beat burst
	 *      0x00000030	Reserve a page
	 *      0x00000009	RWM parity
	 */
	reg32 = 0x35323239;
	if (!is32bits)
		reg32 |= 0x0300;
	pci_ppc_write_config32(0, 0, 0xfc, reg32);

	/*
	 * MSAR1/MSAR2/MESAR1/MESAR2
	 *
	 * Assume each memory block is 32Mb. This is
	 * most likely NOT correct.
	 */
	pci_ppc_write_config32(0, 0, 0x80, 0xc0804000);
	pci_ppc_write_config32(0, 0, 0x84, 0xc0804000);
	pci_ppc_write_config32(0, 0, 0x88, 0x00000000);
	pci_ppc_write_config32(0, 0, 0x8c, 0x01010101);

	/*
	 * MEAR1/MEAR2/MEEAR1/MEEAR2
	 */
	pci_ppc_write_config32(0, 0, 0x90, 0xffbf7f3f);
	pci_ppc_write_config32(0, 0, 0x94, 0xffbf7f3f);
	pci_ppc_write_config32(0, 0, 0x98, 0x00000000);
	pci_ppc_write_config32(0, 0, 0x9c, 0x10101010);

	/*
	 * ODCR
	 *	0x80	PCI I/O 50 ohms 
	 *	0x40	CPU I/O 50 ohms
	 *	0x30	Mem I/O 8 ohms
	 *	0x20	Mem I/O 13 ohms
	 *	0x10	Mem I/O 20 ohms
	 *	0x00	Mem I/O 40 ohms
	 *	0x0c	PCIClk 8 ohms
	 *	0x08	PCIClk 13 ohms
	 *	0x04	PCIClk 20 ohms
	 *	0x00	PCIClk 40 ohms
	 *	0x03	MemClk 8 ohms
	 *	0x02	MemClk 13.3 ohms
	 *	0x01	MemClk 20 ohms
	 *	0x00	MemClk 40 ohms
	 */
	pci_ppc_write_config8(0, 0, 0x73, 0xd1);

        /* 
	 * CDCR 
	 *	0x8000	PCI_SYNC_OUT disabled
	 *      0x7c00	PCI_CLK disabled
	 *      0x0300	CPU_CLK 8 ohms
	 *      0x0200	CPU_CLK 13 ohms
	 *      0x0100	CPU_CLK 20 ohms
	 *      0x0000	CPU_CLK 40 ohms
	 *      0x0080	SDRAM_SYNC_OUT disabled
	 *      0x0078	SDRAM_CLK disabled
	 *      0x0004	CPU_CLK0 disabled
	 *      0x0002	CPU_CLK1 disabled
	 *      0x0001	CPU_CLK2 disabled
	*/
	pci_ppc_write_config16(0, 0, 0x74, 0xfd00);

	/*
	 * MICR
	 *      0x80	MCP 1=open-drain, 0=output
	 *      0x40	SRESET 1=open-drain, 0=output
	 *      0x20	QACK 1=high-Z, 0=output
	 */
	pci_ppc_write_config8(0, 0, 0x76, 0x40);

	/*
	 * MBEN
	 *      0x02	Enable bank 1
	 *      0x01	Enable bank 0
	 */
	pci_ppc_write_config8(0, 0, 0xa0, 0x03);

        /* 
	 * PGMAX
	 *      0x32	33MHz value w/ROMFAL=8
	 */
	pci_ppc_write_config8(0, 0, 0xa3, 0x32);

	/*
	 * Wait 200us
	 */
	udelay(200);

	/*
	 * Now set memgo bit in MCCR1
	 */
	reg32 = pci_ppc_read_config32(0, 0, 0xf0);
	reg32 |= 0x00080000; /* MEMGO=1 */
	pci_ppc_write_config32(0, 0, 0xf0, reg32);

	/*
	 * Wait again
	 */

	udelay(10000);
}

/*
 * Configure real memory settings.
 */
unsigned long 
mpc107_config_memory(int no_banks, sdram_bank_info * bank, int for_real)
{
	int i, j;
	char ignore[8];
	/* Convert bus clock to cycle time in 100ns units */
	unsigned cycle_time = 10 * (2500000000U / get_timer_freq());
	/* Approximate */
	unsigned access_time = cycle_time - 300;
	unsigned cas_latency = 0;
	unsigned rdlat;
	unsigned refint;
	unsigned refrec;
	unsigned acttorw, acttopre;
	unsigned pretoact, bstopre;
	enum sdram_error_detect error_detect;
	uint32_t mccr1;
	uint32_t mccr2;
	uint32_t mccr3;
	uint32_t mccr4;
	uint8_t bank_enable;
	uint32_t memstart1, memstart2;
	uint32_t extmemstart1, extmemstart2;
	uint32_t memend1, memend2;
	uint32_t extmemend1, extmemend2;
	uint32_t address;

	/* Set up the ignore mask */
	for(i = 0; i < no_banks; i++)
		ignore[i] = (bank[i].size == 0);
	    
	/* Pick best CAS latency possible */
	for (i = 0; i < no_banks; i++)
	{
		if (! ignore[i])
		{
			for (j = 0; j < 3; j++) 
			{
				if (cycle_time >= bank[i].cycle_time[j] &&
					access_time >= bank[i].access_time[j]) 
				{
					cas_latency = bank[i].cas_latency[j];
					break;
				}
			}
		}
	}
	if (!cas_latency)
		return 0;

	/* For various parameters there is a risk of clashing between banks */
	error_detect = (for_real > 1) ? ERRORS_ECC : ERRORS_NONE;
	for (i = 0; i < no_banks; i++)
	{
		if (! ignore[i])
		{
			{
				for (j = 0; j < 3; j++)
					if (bank[i].cas_latency[j] == cas_latency)
						break;
				if (j == 3)
				{
					ignore[i] = 1;
					if (! for_real)
						printk_info("Disabling memory bank %d (cas latency)\n", i);
				}
				if (bank[i].error_detect < error_detect) 
					error_detect = bank[i].error_detect;
			}
		}
	}

	/* Read in configuration of port X */
	mccr1 = pci_ppc_read_config32(0, 0, 0xf0);
	mccr2 = pci_ppc_read_config32(0, 0, 0xf4);
	mccr4 = pci_ppc_read_config32(0, 0, 0xfc);
	mccr1 &= 0xfff00000;
	mccr2 &= 0xffe00000;
	mccr3 = 0;
	mccr4 &= 0x00230000;

	pretoact = 0;
	acttorw = 0;
	acttopre = 0;
	for (i = 0; i < no_banks; i++)
		if (! ignore[i])
		{
			int rowcode = -1;
			if (for_real)
			{
				bank[i].actual_detect = error_detect;
				bank[i].actual_cas = cas_latency;
			}
	
			switch (bank[i].row_bits) {
			case 13:
				if (bank[i].internal_banks == 4)
					rowcode = 2;
				else if (bank[i].internal_banks == 2)
					rowcode = 1;
				break;
			case 12:
				if (bank[i].internal_banks == 4)
					rowcode = 0;
				else if (bank[i].internal_banks == 2)
					rowcode = 1;
				break;
			case 11:
				if (bank[i].internal_banks == 4)
					rowcode = 0;
				else if (bank[i].internal_banks == 2)
					rowcode = 3;
				break;
			}
			if (rowcode == -1) {
				ignore[i] = 1;
				if (! for_real)
					printk_info("Memory bank %d disabled: row bits %d and banks %d not supported\n", i, bank[i].row_bits, bank[i].internal_banks);
			} else
				mccr1 |= rowcode << (2 * i);

			/* Update worst case settings */
			if (! ignore[i]) {
				if (bank[i].min_row_precharge > pretoact)
					pretoact = bank[i].min_row_precharge;
				if (bank[i].min_ras_to_cas > acttorw)
					acttorw = bank[i].min_ras_to_cas;
				if (bank[i].min_ras > acttopre)
					acttopre = bank[i].min_ras;
			}
		}

	/* Now convert to clock cycles, rounding up */
	pretoact = (100 * pretoact + cycle_time - 1) / cycle_time;
	acttopre = (100 * acttopre + cycle_time - 1) / cycle_time;
	acttorw = (100 * acttorw + cycle_time - 1) / cycle_time;
	refrec = acttopre;
	bstopre = 0x240;		/* Set conservative values, because we can't derive */
	refint = 1000;

	if (error_detect == ERRORS_ECC)
	{
		rdlat = cas_latency + 2;
		mccr4 |= 0x00400000;
		mccr2 |= 0x000c0001;
	}
	else
	{
		rdlat = cas_latency + 1;
		mccr4 |= 0x00100000;
	}
    
	if (pretoact > 16 || acttopre > 16 || acttorw > 16)
		if (! for_real)
			printk_info("Timings out of range\n");
	mccr4 |= ((pretoact & 0x0f) << 28) | ((acttopre & 0xf) << 24) |
		((acttorw & 0x0f) << 4) |
		((bstopre & 0x003) << 18) | ((bstopre & 0x3c0) >> 6) |
		(cas_latency << 12) | 0x00000200 /* burst length */ ;
	mccr3 |= ((bstopre & 0x03c) << 26) |
		((refrec & 0x0f) << 24) | (rdlat << 20);
	mccr2 |= refint << 2;
	mccr1 |= 0x00080000;	/* memgo */

	address = 0;
	memstart1 = memstart2 = 0;
	extmemstart1 = extmemstart2 = 0;
	memend1 = memend2 = 0;
	extmemend1 = extmemend2 = 0;
	bank_enable = 0;
	for (i = 0; i < no_banks; i++) {
		if (! ignore[i]) {
			uint32_t end = address + bank[i].size - 1;
			bank_enable |= 1 << i;
			if (i < 4) {
				memstart1 |= ((address >> 20) & 0xff) << (8 * i);
				extmemstart1 |= ((address >> 28) & 0x03) << (8 * i);
				memend1 |= ((end >> 20) & 0xff) << (8 * i);
				extmemend1 |= ((end >> 28) & 0x03) << (8 * i);
			} else {
				int k = i - 4;
				memstart2 |= ((address >> 20) & 0xff) << (8 * k);
				extmemstart2 |= ((address >> 28) & 0x03) << (8 * k);
				memend2 |= ((end >> 20) & 0xff) << (8 * k);
				extmemend2 |= ((end >> 28) & 0x03) << (8 * k);
			}
			address += bank[i].size;
		}
	}

	if (for_real)
	{
		/*
		 * Mask MEMGO bit before setting MCCR1
		 */
		mccr1 &= ~0x80000;
		printk_info("MCCR1 = 0x%08x\n", mccr1);
		pci_ppc_write_config32(0, 0, 0xf0, mccr1);

		printk_info("MBEN = 0x%02x\n", bank_enable);
		pci_ppc_write_config8(0, 0, 0xa0, bank_enable);
		printk_info("MSAR1 = 0x%08x\n", memstart1);
		pci_ppc_write_config32(0, 0, 0x80, memstart1);
		printk_info("MSAR2 = 0x%08x\n", memstart2);
		pci_ppc_write_config32(0, 0, 0x84, memstart2);
		printk_info("MSAR3 = 0x%08x\n", extmemstart1);
		pci_ppc_write_config32(0, 0, 0x88, extmemstart1);
		printk_info("MSAR4 = 0x%08x\n", extmemstart2);
		pci_ppc_write_config32(0, 0, 0x8c, extmemstart2);
		printk_info("MEAR1 = 0x%08x\n", memend1);
		pci_ppc_write_config32(0, 0, 0x90, memend1);
		printk_info("MEAR2 = 0x%08x\n", memend2);
		pci_ppc_write_config32(0, 0, 0x94, memend2);
		printk_info("MEAR3 = 0x%08x\n", extmemend1);
		pci_ppc_write_config32(0, 0, 0x98, extmemend1);
		printk_info("MEAR4 = 0x%08x\n", extmemend2);
		pci_ppc_write_config32(0, 0, 0x9c, extmemend2);
		printk_info("MCCR2 = 0x%08x\n", mccr2);
		pci_ppc_write_config32(0, 0, 0xf4, mccr2);
		printk_info("MCCR3 = 0x%08x\n", mccr3);
		pci_ppc_write_config32(0, 0, 0xf8, mccr3);
		printk_info("MCCR4 = 0x%08x\n", mccr4);
		pci_ppc_write_config32(0, 0, 0xfc, mccr4);

		udelay(200);

		/*
		 * Set MEMGO bit
		 */
		mccr1 |= 0x80000;
		printk_info("MCCR1 = 0x%08x\n", mccr1);
		pci_ppc_write_config32(0, 0, 0xf0, mccr1);

		udelay(10000);
	}
    
	return address;
}

static int
i2c_wait(unsigned timeout, int writing)
{
	uint32_t x;
	while (((x = readl(MPC107_BASE + MPC107_I2CSR)) & (MPC107_I2C_CSR_MCF | MPC107_I2C_CSR_MIF))
		!= (MPC107_I2C_CSR_MCF | MPC107_I2C_CSR_MIF)) {
		if (ticks_since_boot() > timeout)
			return -1;
	}
	
	if (x & MPC107_I2C_CSR_MAL) {
		return -1;
	}
	if (writing && (x & MPC107_I2C_CSR_RXAK)) {
		printk_info("No RXAK\n");
		/* generate stop */
		writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
		return -1;
	}
	writel(0, MPC107_BASE + MPC107_I2CSR);
	return 0;
}

static void
mpc107_i2c_start(struct i2c_bus *bus)
{
	/* Set clock */
	writel(0x1031, MPC107_BASE + MPC107_I2CFDR);
	/* Clear arbitration */
	writel(0, MPC107_BASE + MPC107_I2CSR);
}

static void 
mpc107_i2c_stop(struct i2c_bus *bus)
{
	/* After last DIMM shut down I2C */
	writel(0x0, MPC107_BASE + MPC107_I2CCR);
}

static int 
mpc107_i2c_byte_write(struct i2c_bus *bus, int target, int address, uint8_t data)
{
    	unsigned timeout = ticks_since_boot() + 3 * get_hz();

	/* Must wait here for clocks to start */
        udelay(25000);
	/* Start with MEN */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	/* Start as master */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX, MPC107_BASE + MPC107_I2CCR);
	/* Write target byte */
	writel(target, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Write data address byte */
	writel(address, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;
	
	/* Write data byte */
    	writel(data, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;
	
	/* generate stop */
    	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
    return 0;
}

static int 
mpc107_i2c_master_write(struct i2c_bus *bus, int target, int address, const uint8_t *data, int length)
{
    	unsigned count;
	for(count = 0; count < length; count++)
	{
	    if (mpc107_i2c_byte_write(bus, target, address, data[count]) < 0)
		return -1;
	}
	return count;
}

#define DIMM_LENGTH 0xfff

static int 
mpc107_i2c_master_read(struct i2c_bus *bus, int target, int address,
	    uint8_t *data, int length)
{
    	unsigned timeout = ticks_since_boot() + 3 * get_hz();
    	unsigned count;

	/* Must wait here for clocks to start */
        udelay(25000);
	/* Start with MEN */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	/* Start as master */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX, MPC107_BASE + MPC107_I2CCR);
	/* Write target byte */
	writel(target, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Write data address byte */
	writel(address, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 1) < 0)
	    return -1;

	/* Switch to read - restart */
	writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_MTX | MPC107_I2C_CCR_RSTA, MPC107_BASE + MPC107_I2CCR);
	/* Write target address byte - this time with the read flag set */
	writel(target | 1, MPC107_BASE + MPC107_I2CDR);

	if (i2c_wait(timeout, 0) < 0)
	    return -1;

	if (length == 1)
	    writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
	else
	    writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA, MPC107_BASE + MPC107_I2CCR);
    	/* Dummy read */
	readl(MPC107_BASE + MPC107_I2CDR);

	count = 0;
	while (count < length) {

		if (i2c_wait(timeout, 0) < 0)
			return -1;
		
		/* Generate txack on next to last byte */
		if (count == length - 2)
			writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_MSTA | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
		/* Generate stop on last byte */
		if (count == length - 1)
			writel(MPC107_I2C_CCR_MEN | MPC107_I2C_CCR_TXAK, MPC107_BASE + MPC107_I2CCR);
		data[count] = readl(MPC107_BASE + MPC107_I2CDR);
		if (count == 0 && length == DIMM_LENGTH) {
			if (data[0] == 0xff) {
				printk_debug("I2C device not present\n");
				length = 3;
			} else {
				length = data[0];
				if (length < 3)
					length = 3;
			}
		}
		count++;
	}
	
	/* Finish with disable master */
	writel(MPC107_I2C_CCR_MEN, MPC107_BASE + MPC107_I2CCR);
	return length;
}

i2c_fn mpc107_i2c_fn = {
	mpc107_i2c_start, mpc107_i2c_stop,
	mpc107_i2c_master_write, mpc107_i2c_master_read
};

/*
 * Find dimm information.
 */
void
mpc107_probe_dimms(int no_dimms, sdram_dimm_info *dimms, sdram_bank_info * bank)
{
	unsigned char data[256];
	unsigned dimm;
	
	printk_debug("Probing DIMMS...\n");
	
	mpc107_i2c_start(NULL);
	
	for(dimm = 0; dimm < no_dimms; dimm++)
	{
		dimms[dimm].number = dimm;
		dimms[dimm].bank1 = bank + dimm*NUM_BANKS;
		dimms[dimm].bank2 = bank + dimm*NUM_BANKS + 1;
		bank[dimm*NUM_BANKS].size = 0;
		bank[dimm*NUM_BANKS+1].size = 0;
		bank[dimm*NUM_BANKS].number = 0;
		bank[dimm*NUM_BANKS+1].number = 1;
	}
	
	
	for (dimm = 0; dimm < no_dimms; dimm ++) {
		unsigned limit = mpc107_i2c_master_read(NULL, 0xa0 + 2*dimm, 0,
			data, DIMM_LENGTH);
		
		if (limit > 3) {
			sdram_dimm_to_bank_info(data, dimms + dimm, 1);
			memcpy(dimms[dimm].part_number, data + 73, 18);
			dimms[dimm].part_number[18] = 0;
			printk_debug("Part Number: %s\n", dimms[dimm].part_number);
		}
	}
	
	mpc107_i2c_stop(NULL);
}
