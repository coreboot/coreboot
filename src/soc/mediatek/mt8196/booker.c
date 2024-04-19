/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/booker.h>

#define REG_READ_ONLY_HASH_VALUE	(MCUCFG_BASE + 0x059C)
#define REG_MCUSYS_RESERVED_REG2	(MCUCFG_BASE + 0xFFE8)

#define POR_SBSX_CFG_CTL_OFFSET		(0x00450000 + 0x0A00)
#define INSTANCE0_SBSX_POR_SBSX_CFG_CTL	(0x0A000000 + POR_SBSX_CFG_CTL_OFFSET)
#define INSTANCE1_SBSX_POR_SBSX_CFG_CTL	(0x0A800000 + POR_SBSX_CFG_CTL_OFFSET)
#define INSTANCE2_SBSX_POR_SBSX_CFG_CTL	(0x0B000000 + POR_SBSX_CFG_CTL_OFFSET)
#define INSTANCE3_SBSX_POR_SBSX_CFG_CTL	(0x0B800000 + POR_SBSX_CFG_CTL_OFFSET)
#define BIT_DISABLE_CMO_PROP		BIT(3)

/*
 * Configure booker and disable HN-D coherence request to avoid
 * receiving NDE(Non-data Error) before MMU enabled.
 */
void booker_init(void)
{
	/* Enable CMO(cache maintenance operations) propagation */
	clrbits64p(INSTANCE0_SBSX_POR_SBSX_CFG_CTL, BIT_DISABLE_CMO_PROP);
	clrbits64p(INSTANCE1_SBSX_POR_SBSX_CFG_CTL, BIT_DISABLE_CMO_PROP);
	clrbits64p(INSTANCE2_SBSX_POR_SBSX_CFG_CTL, BIT_DISABLE_CMO_PROP);
	clrbits64p(INSTANCE3_SBSX_POR_SBSX_CFG_CTL, BIT_DISABLE_CMO_PROP);
	dsb();
	isb();

	/* CHI Splitter - for Q-Channel setting */
	setbits32p(REG_MCUSYS_RESERVED_REG2, BIT(0));

	printk(BIOS_DEBUG, "[%s] AP hash rule: 0x%x\n",
	       __func__,
	       read32p(REG_READ_ONLY_HASH_VALUE));
}
