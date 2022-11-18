/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <device/pci.h>
#include <delay.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/iosf.h>
#include <soc/iomap.h>
#include <soc/ramstage.h>
#include <soc/modphy_table.h>

#define IOSF_READ(op_read, port) \
	(IOSF_OPCODE(op_read) | IOSF_PORT(port))
#define IOSF_WRITE(op_write, port) \
	(IOSF_OPCODE(op_write) | IOSF_PORT(port))

static void program_modphy_table(struct modphy_entry *table)
{
	u32 tmp;

	for (; table->port; ++table) {
		tmp = iosf_read_port(IOSF_READ(table->op_read, table->port), table->reg);
		iosf_write_port(IOSF_WRITE(table->op_write, table->port), table->reg,
			(tmp & table->mask) | table->value);
	}
}

static void gpio_sc_sdcard_workaround(void)
{
	setbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 0));
	setbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 2));
	clrbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 1));
	clrbits32((char *)IO_BASE_ADDRESS + 0x690, (1 << 3));
	udelay(100);
	clrbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 0));
	udelay(100);
	write32((char *)IO_BASE_ADDRESS + 0x830, 0x78480);
	udelay(40);
	write32((char *)IO_BASE_ADDRESS + 0x830, 0x78080);
	setbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 0));
	udelay(100);
	setbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 1));
	clrbits32((char *)IO_BASE_ADDRESS + 0x698, (1 << 2));
	clrsetbits32((char *)IO_BASE_ADDRESS + 0x690, 7, (1 << 0));
}

#define BUNIT_BALIMIT0	0x0b
#define AUNIT_AVCCTL	0x21
#define AUNIT_ACFCACV	0x60
#define CUNIT_ACCESS_CTRL_VIOL	0x41
#define CUINT_SSA_REGIONAL_TRUNKGATE_CTL	0x43
#define TUNIT_CTL	0x03
#define TUNIT_MISC_CTL	0x04

static void ssa_safe_config(void)
{
	u32 tmp;

	tmp = iosf_bunit_read(BUNIT_BALIMIT0);
	iosf_bunit_write(BUNIT_BALIMIT0, (tmp & 0xC0D0D0D0) | 0x1F2F2F2F);

	tmp = iosf_aunit_read(AUNIT_AVCCTL);
	iosf_aunit_write(AUNIT_AVCCTL, tmp | 0x80000100);

	tmp = iosf_aunit_read(AUNIT_ACFCACV);
	iosf_aunit_write(AUNIT_ACFCACV, tmp & 0x7FFFFFFF);

	tmp = iosf_cunit_read(CUNIT_ACCESS_CTRL_VIOL);
	iosf_cunit_write(CUNIT_ACCESS_CTRL_VIOL, tmp & 0x7FFFFFFF);

	iosf_cunit_write(CUINT_SSA_REGIONAL_TRUNKGATE_CTL, 0x70008);

	tmp = iosf_cpu_bus_read(TUNIT_CTL);
	iosf_cpu_bus_write(TUNIT_CTL, tmp | 0x110430);

	tmp = iosf_cpu_bus_read(TUNIT_MISC_CTL);
	iosf_cpu_bus_write(TUNIT_MISC_CTL, tmp | 0x40010);
}

#define R_PCH_PMC_MTPMC1 0xb0

/*
 * Replacement for refcode.elf
 */
void baytrail_run_reference_code(void)
{
	u32 tmp;
	size_t pollcnt;

	printk(BIOS_DEBUG, "ModPHY init entry\n");

	if (pci_read_config8(pcidev_on_root(LPC_DEV, LPC_FUNC), REVID) < RID_B_STEPPING_START) {
		printk(BIOS_DEBUG, "SOC A0/A1 ModPhy Table programming\n");
		program_modphy_table(reva0_modphy_table);
	} else {
		printk(BIOS_DEBUG, "SOC B0 and later ModPhy Table programming\n");
		program_modphy_table(revb0_modphy_table);
	}

	setbits32((char *)PMC_BASE_ADDRESS + R_PCH_PMC_MTPMC1, 8);

	for (pollcnt = 0; pollcnt < 10; ++pollcnt) {
		tmp = read32((char *)PMC_BASE_ADDRESS + R_PCH_PMC_MTPMC1);
		printk(BIOS_DEBUG, "Polling bit3 of R_PCH_PMC_MTPMC1 = %x\n", tmp);
		if (!(tmp & 8))
			break;
	}

	gpio_sc_sdcard_workaround();
	ssa_safe_config();

	printk(BIOS_DEBUG, "ModPHY init done\n");
}
