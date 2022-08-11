/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/lastbus.h>

static unsigned long preisys_dump_offset[] = {
	0x500, /* PERIBUS_DBG0 */
	0x504, /* PERIBUS_DBG1 */
	0x508, /* PERIBUS_DBG2 */
	0x50C, /* PERIBUS_DBG3 */
	0x510, /* PERIBUS_DBG4 */
	0x514, /* PERIBUS_DBG5 */
	0x518, /* PERIBUS_DBG6 */
	0x51C, /* PERIBUS_DBG7 */
	0x520, /* PERIBUS_DBG8 */
	0x524, /* PERIBUS_DBG9 */
	0x528, /* PERIBUS_DBG10 */
	0x52C, /* PERIBUS_DBG11 */
	0x530, /* PERIBUS_DBG12 */
	0x534, /* PERIBUS_DBG13 */
	0x538, /* PERIBUS_DBG14 */
	0x53C, /* PERIBUS_DBG15 */
	0x580, /* PERIBUS_DBG16 */
	0x584, /* PERIBUS_DBG17 */
};

static void lastbus_setup(void)
{
	/* peri lastbus init */
	write32p(PERICFG_BASE + BUS_PERI_R0, PERISYS_TIMEOUT);
	write32p(PERICFG_BASE + BUS_PERI_R1, PERISYS_ENABLE);

	/* infra lastbus init */
	write32p(INFRACFG_AO_BASE + BUS_INFRA_CTRL, INFRASYS_CONFIG);
}

static void lastbus_dump(void)
{
	unsigned int i;
	uintptr_t reg;

	if (read32p(INFRACFG_AO_BASE + BUS_INFRA_CTRL) & 0x1) {
		printk(BIOS_DEBUG, "** Dump lastbus infra debug registers start **\n");
		for (i = 0; i < INFRA_NUM; i++) {
			reg = INFRACFG_AO_BASE + BUS_INFRA_SNAPSHOT + 4 * i;
			printk(BIOS_DEBUG, "%08x\n", read32p(reg));
		}
	}

	if (read32p(PERICFG_BASE + BUS_PERI_R1) & 0x1) {
		printk(BIOS_DEBUG, "** Dump lastbus peri debug registers start **\n");
		for (i = 0; i < PERI_NUM; i++) {
			reg = PERICFG_BASE + preisys_dump_offset[i];
			printk(BIOS_DEBUG, "%08x\n", read32p(reg));
		}
	}
}

void lastbus_init(void)
{
	lastbus_dump();
	lastbus_setup();
}
