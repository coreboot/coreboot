/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/booker.h>
#include <soc/mcupm_plat.h>

#define REG_READ_ONLY_HASH_VALUE	(MCUCFG_BASE + 0x059C)
#define REG_MCUSYS_RESERVED_REG2	(MCUCFG_BASE + 0xFFE8)

#define POR_SBSX_CFG_CTL_OFFSET		(0x00450000 + 0x0A00)

#define POR_MTU_BASE			0x790000
#define POR_MTU_AUX_CTL_OFFSET		(POR_MTU_BASE + 0x0A08)
#define POR_MTU_TAG_ADDR_CTL_OFFSET	(POR_MTU_BASE + 0x0A40)
#define POR_MTU_TAG_ADDR_BASE_OFFSET	(POR_MTU_BASE + 0x0A48)
#define POR_MTU_TAG_ADDR_SHUTTER0_OFFSET	(POR_MTU_BASE + 0x0A50)
#define POR_MTU_TAG_ADDR_SHUTTER1_OFFSET	(POR_MTU_BASE + 0x0A58)
#define POR_MTU_TAG_ADDR_SHUTTER2_OFFSET	(POR_MTU_BASE + 0x0A60)

#define BIT_DISABLE_BOOKER_TAG		BIT(0)
#define BIT_DISABLE_CMO_PROP		BIT(3)

#define MTU_TYPE_MODE_0			0x5
#define MTU_SHUTTER0_SETTING		0x2222222222222200ULL
#define MTU_SHUTTER1_SETTING		0x2222222222222222ULL
#define MTU_SHUTTER2_SETTING		0x0000000002222222ULL

/* Booker instance base addresses */
static const uint32_t booker_base[] = {
	0x0A000000,
	0x0A800000,
	0x0B000000,
	0x0B800000,
};

/*
 * Configure booker and disable HN-D coherence request to avoid
 * receiving NDE(Non-data Error) before MMU enabled.
 */
void booker_init(void)
{
	int i;

	/* Enable CMO(cache maintenance operations) propagation */
	for (i = 0; i < ARRAY_SIZE(booker_base); i++)
		clrbits64p(booker_base[i] + POR_SBSX_CFG_CTL_OFFSET, BIT_DISABLE_CMO_PROP);

	dsb();
	isb();

	/* CHI Splitter - for Q-Channel setting */
	setbits32p(REG_MCUSYS_RESERVED_REG2, BIT(0));

	printk(BIOS_DEBUG, "[%s] AP hash rule: 0x%x\n",
	       __func__,
	       read32p(REG_READ_ONLY_HASH_VALUE));
}

void booker_mte_init(uint64_t mte_tag_addr)
{
	int i;

	/* Notify MCUPM to configure MTE on resume path. */
	setbits32p(MTE_ENABLE_REG, 0xF);

	printk(BIOS_DEBUG, "%s: MTE tag addr %#llx\n", __func__, mte_tag_addr);

	/* Setting MTU TAG */
	for (i = 0; i < ARRAY_SIZE(booker_base); i++) {
		write64p(booker_base[i] + POR_MTU_TAG_ADDR_CTL_OFFSET, MTU_TYPE_MODE_0);
		write64p(booker_base[i] + POR_MTU_TAG_ADDR_BASE_OFFSET, mte_tag_addr);
		write64p(booker_base[i] + POR_MTU_TAG_ADDR_SHUTTER0_OFFSET,
			 MTU_SHUTTER0_SETTING);
		write64p(booker_base[i] + POR_MTU_TAG_ADDR_SHUTTER1_OFFSET,
			 MTU_SHUTTER1_SETTING);
		write64p(booker_base[i] + POR_MTU_TAG_ADDR_SHUTTER2_OFFSET,
			 MTU_SHUTTER2_SETTING);
		setbits64p(booker_base[i] + POR_MTU_AUX_CTL_OFFSET, BIT_DISABLE_BOOKER_TAG);
	}
	dsb();
	isb();
}
