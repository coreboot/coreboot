#include	<ppc.h>
#include	<mem.h>
#include	<console/console.h>

#define SDRAM0_CFGADDR	0x010
#define SDRAM0_CFGDATA	0x011

#define SDRAM0_B0CR	0x40
#define SDRAM0_B1CR	0x44
#define SDRAM0_B2CR	0x48
#define SDRAM0_B3CR	0x4C

#define BA_MASK		0xffc00000
#define BA_SHIFT	22
#define SZ_MASK		0x000e0000
#define SZ_SHIFT	17
#define AM_MASK		0x0000e000
#define AM_SHIFT	13
#define BE_MASK		0x00000001
#define BE_SHIFT	0

struct mem_range	mem_bank[4];

void
getmemrange(int bank, unsigned int reg)
{
	unsigned int val;
	unsigned int size;

	mtdcr(SDRAM0_CFGADDR, reg);
	val = mfdcr(SDRAM0_CFGDATA);

	if ((val & BE_MASK) == 0)
	{
		mem_bank[bank].basek = 0;
		mem_bank[bank].sizek = 0;
		return;
	}

	size = ((val & BA_MASK) >> BA_SHIFT);
	mem_bank[bank].basek = size * 1024;

	size = 4 << ((val & SZ_MASK) >> SZ_SHIFT);
	mem_bank[bank].sizek = size * 1024;

	printk_info("mem bank %d: basek %d, sizek %d\n", bank, mem_bank[bank].basek, mem_bank[bank].sizek);
}

struct mem_range *
sizeram(void)
{
	getmemrange(0, SDRAM0_B0CR);
	getmemrange(1, SDRAM0_B1CR);
	getmemrange(2, SDRAM0_B2CR);
	getmemrange(3, SDRAM0_B3CR);

	return mem_bank;
}

