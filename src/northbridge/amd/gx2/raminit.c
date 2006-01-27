#include <cpu/amd/gx2def.h>

/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express 
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */

/* 	SDRAM initialization for GX1 - translated from Christer Weinigel's 
	assembler version into C.

	Hamish Guthrie 10/4/2005 hamish@prodigi.ch
*/
/* just converted to GX2 by ron minnich -- this is probably mostly wrong 
 * I am just putting in a placeholder to start building gx2 support
 */

#define NUM_REFRESH 8
#define TEST_DATA1 0x05A5A5A5A
#define TEST_DATA2 0x0DEADBEEF

void setGX2Mem(unsigned int addr, unsigned int data)
{
	writel(data, (volatile void *)addr);
}

unsigned int getGX2Mem(unsigned int addr)
{
	return (unsigned int)readl((const volatile void *)addr);
}

void do_refresh(void)
{
unsigned int tval, i;

	outb(0x71, 0x80);
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL1);
	tval |= RFSHTST;
	for(i=0; i>NUM_REFRESH; i++)
		setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	outb(0x72, 0x80);
}
	

void enable_dimm(void)
{
unsigned int tval, i;

	outb(0x73, 0x80);

	/* start SDCLCK's */
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL1);
	tval &= ~SDCLKSTRT;
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	tval |= SDCLKSTRT;
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);

	/* Unmask SDCLK's */
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL2);
	tval &= ~(SDCLK_MASK | SDCLKOUT_MASK);
	setGX2Mem(GX_BASE + MC_MEM_CNTRL2, tval);
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL2);

	/* Wait for clocks to unmask */
	for(i=0; i<5000; i++)
		outb(0, 0xed);

	/* Refresh memory */
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL1);
	tval |= RFSHTST;
	for(i=0; i<NUM_REFRESH; i++)
		setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	tval &= ~RFSHTST;

	/* Start the SDCLK's */
	tval &= ~PROGRAM_SDRAM;
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	tval |= PROGRAM_SDRAM | 0x00002000;   /* Set refresh timing */
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	tval &= ~PROGRAM_SDRAM;
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);

	/* Refresh memory again */	
	tval = getGX2Mem(GX_BASE + MC_MEM_CNTRL1);
	tval |= RFSHTST;
	for(i=0; i>NUM_REFRESH; i++)
		setGX2Mem(GX_BASE + MC_MEM_CNTRL1, tval);
	
	for(i=0; i<2000; i++)
		outb(0, 0xed);
	outb(0x74, 0x80);
}

static unsigned int size_dimm(int dimm_shift)
{
int bank_cfg = 0x700;		/* MC_BANK_CFG for 512M */
unsigned int offset = 0x10000000;	/* Offset 256M */
int failed_flag = 1;

	do {
		setGX2Mem(0, TEST_DATA1);
		setGX2Mem(offset, TEST_DATA2);
		setGX2Mem(0x100, 0);		/* Clear the bus */
		if (getGX2Mem(0) != TEST_DATA1) {
			setGX2Mem(GX_BASE + MC_BANK_CFG,
			  	getGX2Mem(GX_BASE + MC_BANK_CFG) & ~(DIMM_SZ << dimm_shift));
			bank_cfg -= 0x100;
			setGX2Mem(GX_BASE + MC_BANK_CFG,
				getGX2Mem(GX_BASE + MC_BANK_CFG) | (bank_cfg << dimm_shift));
			do_refresh();
			offset >>= 1;
		} else {
			failed_flag = 0;
			break;
		}
	} while (bank_cfg >= 0);

	if (failed_flag)
		return (0x0070 << dimm_shift);
	else
		return(getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_SZ << dimm_shift));
		
}

static unsigned int module_banks(int dimm_shift)
{
int page_size = 0x800;			/* Smallest page = 1K * 2 banks */
int comp_banks;

#if 0
	print_debug("MC_BANK_CFG = ");
	print_debug_hex32(getGX2Mem(GX_BASE + MC_BANK_CFG));
	print_debug("\r\n");
#endif

	/* retrieve the page size from the MC register */
	page_size <<= (((getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_PG_SZ << dimm_shift)) >> dimm_shift) >> 4);

#if 0
	print_debug("    page_size = ");
	print_debug_hex32(page_size);
	print_debug("\r\n");
#endif

	comp_banks = (((getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_COMP_BNK << dimm_shift)) >> dimm_shift) >> 12);
	page_size <<= comp_banks;

	setGX2Mem(0, TEST_DATA1);
	setGX2Mem(page_size, TEST_DATA2);
	setGX2Mem(0x100, 0);		/* Clear the bus */
	if (getGX2Mem(page_size) != TEST_DATA2) {
		setGX2Mem(GX_BASE + MC_BANK_CFG,
			  getGX2Mem(GX_BASE + MC_BANK_CFG) & ~(DIMM_MOD_BNK << dimm_shift));
		do_refresh();
	}
#if 0
	print_debug("MC_BANK_CFG = ");
	print_debug_hex32(getGX2Mem(GX_BASE + MC_BANK_CFG));
	print_debug("\r\n");
#endif
	return(getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_MOD_BNK << dimm_shift));
}

static unsigned int component_banks(int dimm_shift)
{
int page_size = 0x800;			/* Smallest page = 1K * 2 banks */

#if 0
	print_debug("MC_BANK_CFG = ");
	print_debug_hex32(getGX2Mem(GX_BASE + MC_BANK_CFG));
	print_debug("\r\n");
#endif

	page_size = page_size << (((getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_PG_SZ << dimm_shift)) >> dimm_shift) >> 4);

#if 0
	print_debug("    page_size = ");
	print_debug_hex32(page_size);
	print_debug("\r\n");
#endif

	setGX2Mem(0, TEST_DATA1);
	setGX2Mem(page_size, TEST_DATA2);
	setGX2Mem(0x100, 0);		/* Clear the bus */
	if (getGX2Mem(0) != TEST_DATA1) {
		setGX2Mem(GX_BASE + MC_BANK_CFG,
			  getGX2Mem(GX_BASE + MC_BANK_CFG) & ~(DIMM_COMP_BNK << dimm_shift));
		do_refresh();
	}
#if 0
	print_debug("MC_BANK_CFG = ");
	print_debug_hex32(getGX2Mem(GX_BASE + MC_BANK_CFG));
	print_debug("\r\n");
#endif
	return(getGX2Mem(GX_BASE + MC_BANK_CFG) & (DIMM_COMP_BNK << dimm_shift));
}

static unsigned int page_size(int dimm_shift)
{
unsigned int page_test_offset = 0x2000;
unsigned int temp;
int page_size_config = 0x40;
unsigned int probe_config;

	do {
		setGX2Mem(0, TEST_DATA1);
		setGX2Mem(page_test_offset, TEST_DATA2);
		setGX2Mem(0x100, 0);
		temp = getGX2Mem(0);
		setGX2Mem(0, 0);
		if(temp == TEST_DATA1) {
#if 0
			print_debug("    Page size Config = ");
			print_debug_hex32(page_size_config << dimm_shift);
			print_debug("\r\n");
#endif
			return(page_size_config << dimm_shift);
			}
		
		temp = ~(DIMM_PG_SZ << dimm_shift);

		probe_config = getGX2Mem(GX_BASE + MC_BANK_CFG);
		probe_config &= temp;

		page_size_config -= 0x10;
		page_size_config <<= dimm_shift;

		probe_config |= page_size_config;

		page_size_config >>= dimm_shift;

		page_test_offset >>= 1;

		setGX2Mem(GX_BASE + MC_BANK_CFG, probe_config);
		do_refresh();
	} while (page_size_config >= 0);

	return 0x70;
}

static int dimm_detect(int dimm_shift)
{
unsigned int test;

	print_debug("Probing for DIMM");
	print_debug_char((dimm_shift >> 4) + 0x30);
	print_debug("\r\n");

	setGX2Mem(0, TEST_DATA1);
	setGX2Mem(0x100, 0);
	test = getGX2Mem(0);
	setGX2Mem(0, 0);

	if (test != TEST_DATA1)
		return 0;

	print_debug("    Found DIMM");
	print_debug_char((dimm_shift >> 4) + 0x30);
	print_debug("\r\n");

	return 1;
}

static int size_memory(int dimm_shift, unsigned int mem_config)
{

	if (!dimm_detect(dimm_shift))
		return (mem_config);

	mem_config &= (~(DIMM_PG_SZ << dimm_shift));
	mem_config |= (page_size(dimm_shift));

	print_debug("    Page Size:       ");
	print_debug_hex32(0x400 << ((mem_config & (DIMM_PG_SZ << dimm_shift)) >> (dimm_shift + 4)));
	print_debug("\r\n");

	/* Now do component banks detection */

	mem_config &= (~(DIMM_COMP_BNK << dimm_shift));
	mem_config |= (component_banks(dimm_shift));

	print_debug("    Component Banks: ");
	print_debug_char((((mem_config & (DIMM_COMP_BNK << dimm_shift)) >> (dimm_shift + 12)) ? 4 : 2) + 0x30);
	print_debug("\r\n");

	/* Now do module banks */

	mem_config &= (~(DIMM_MOD_BNK << dimm_shift));
	mem_config |= (module_banks(dimm_shift));
	
	print_debug("    Module Banks:    ");
	print_debug_char((((mem_config & (DIMM_MOD_BNK << dimm_shift)) >> (dimm_shift + 14)) ? 2 : 1) + 0x30);
	print_debug("\r\n");

	mem_config &= (~(DIMM_SZ << dimm_shift));
	mem_config |= (size_dimm(dimm_shift));
	
	print_debug("    DIMM size:       ");
	print_debug_hex32(1 << 
		((mem_config & (DIMM_SZ << dimm_shift)) >> (dimm_shift + 8)) + 22);
	print_debug("\r\n");

	return (mem_config);
}

static void sdram_init(void) 
{
unsigned int mem_config = 0x00700070;

	print_debug("Setting up default parameters for memory\r\n");
	outb(0x70, 0x80);

	setGX2Mem(GX_BASE + MC_MEM_CNTRL2, 0x000007d8); /* Disable all CLKS, Shift = 3 */
	setGX2Mem(GX_BASE + MC_MEM_CNTRL1, 0x92140000); /* MD_DS=2, MA_DS=2, CNTL_DS=2 SDCLKRATE=4 */
	setGX2Mem(GX_BASE + MC_BANK_CFG,   0x00700070); /* No DIMMS installed */
	setGX2Mem(GX_BASE + MC_SYNC_TIM1,  0x3a733225); /* LTMODE=3, RC=10, RAS=7, RP=3, RCD=3, RRD=2, DPL=2 */
	setGX2Mem(GX_BASE + MC_BANK_CFG,   0x57405740); /* Largest DIMM size 
						0x4000 -- 2 module banks
						0x1000 -- 4 component banks
						0x0700 -- DIMM size 512MB
						0x0040 -- Page Size 16kB */

	enable_dimm();

	print_debug("Sizing memory\r\n");

	setGX2Mem(GX_BASE + MC_BANK_CFG, 0x00705740);
	do_refresh();
	mem_config = size_memory(0, mem_config);
	setGX2Mem(GX_BASE + MC_BANK_CFG, 0x57400070);
	do_refresh();
	mem_config = size_memory(16, mem_config);

	print_debug("MC_BANK_CFG = ");
	print_debug_hex32(mem_config);
	print_debug("\r\n");

	setGX2Mem(GX_BASE + MC_BANK_CFG, mem_config);
	enable_dimm();
	outb(0x7e, 0x80);
}
