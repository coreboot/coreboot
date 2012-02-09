#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>

#if 0
#include <cpu/amd/gx1def.h>
#include <arch/io.h>

static void gx1_cpu_setup(void)
{
	unsigned char rreg;
	unsigned char cpu_table[] = {
		0xc1, 0x00,		/* NO SMIs */
		0xc3, 0x14,		/* Enable CPU config register */
		0x20, 0x00,		/* */
		0xb8, GX_BASE>>30,	/* Enable GXBASE address */
		0xc2, 0x00,
		0xe8, 0x98,
		0xc3, 0xf8,		/* Enable CPU config register */
		0x00, 0x00
		};
	unsigned char *cPtr = cpu_table;

	while(rreg = *cPtr++) {
		unsigned char rval = *cPtr++;
		outb(rreg, 0x22);
		outb(rval, 0x23);
		}

	outb(0xff, 0x22);		/* DIR1 -- Identification register 1 */
	if(inb(0x23) > 0x63) {		/* Rev greater than R3 */
		outb(0xe8, 0x22);
		outb(inb(0x23) | 0x20, 0x23);	/* Enable FPU Fast Mode */

		outb(0xf0, 0x22);
		outb(inb(0x23) | 0x02, 0x23);	/* Incrementor on */

		outb(0x20, 0x22);
		outb(inb(0x23) | 0x24, 0x23);	/* Bit 5 must be on */
						/* Bit 2 Incrementor margin 10 */

	}
}

static void gx1_gx_setup(void)
{
unsigned long gx_setup_table[] = {
	GX_BASE + DC_UNLOCK, 		DC_UNLOCK_MAGIC,
	GX_BASE + DC_GENERAL_CFG, 		0,
	GX_BASE + DC_UNLOCK, 		0,
	GX_BASE + BC_DRAM_TOP, 		0x3fffffff,
	GX_BASE + BC_XMAP_1, 		0x60,
	GX_BASE + BC_XMAP_2, 		0,
	GX_BASE + BC_XMAP_3, 		0,
	GX_BASE + MC_BANK_CFG, 		0x00700070,
	GX_BASE + MC_MEM_CNTRL1, 	XBUSARB,
	GX_BASE + MC_GBASE_ADD, 	0xff,
	0, 				0
	};

unsigned long *gxPtr = gx_setup_table;
unsigned long *gxdPtr;
unsigned long addr;

	while(addr = *gxPtr++) {
		gxdPtr = (unsigned long *)addr;
		*gxdPtr = *gxPtr++;
	}
}
#endif

static void geode_gx1_init(device_t dev)
{
#if 0
	gx1_cpu_setup();
	gx1_gx_setup();
#endif
	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init	= geode_gx1_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_CYRIX, 0x0540 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops	  = &cpu_dev_ops,
	.id_table = cpu_table,
};
