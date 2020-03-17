/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <delay.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/lcc-reg.h>
#include <device/mmio.h>

typedef struct {
	void *gcc_apcs_regs;
	void *lcc_pll0_regs;
	void *lcc_ahbix_regs;
	void *lcc_mi2s_regs;
	void *lcc_pll_regs;
} IpqLccClocks;

typedef struct __packed {
	uint32_t apcs;
} IpqLccGccRegs;

typedef struct __packed {
	uint32_t mode;
	uint32_t l_val;
	uint32_t m_val;
	uint32_t n_val;
	uint32_t UNUSED;
	uint32_t config;
	uint32_t status;
} IpqLccPll0Regs;

typedef struct __packed {
	uint32_t ns;
	uint32_t md;
	uint32_t UNUSED;
	uint32_t status;
} IpqLccAhbixRegs;

typedef struct __packed {
	uint32_t ns;
	uint32_t md;
	uint32_t status;
} IpqLccMi2sRegs;

typedef struct __packed {
	uint32_t pri;
	uint32_t sec;
} IpqLccPllRegs;

struct lcc_freq_tbl {
	unsigned int freq;
	unsigned int pd;
	unsigned int m;
	unsigned int n;
	unsigned int d;
};

static const struct lcc_freq_tbl lcc_mi2s_freq_tbl[] = {
	{  1024000, 4,  1,  96, 8 },
	{  1411200, 4,  2, 139, 8 },
	{  1536000, 4,  1,  64, 8 },
	{  2048000, 4,  1,  48, 8 },
	{  2116800, 4,  2,  93, 8 },
	{  2304000, 4,  2,  85, 8 },
	{  2822400, 4,  6, 209, 8 },
	{  3072000, 4,  1,  32, 8 },
	{  3175200, 4,  1,  31, 8 },
	{  4096000, 4,  1,  24, 8 },
	{  4233600, 4,  9, 209, 8 },
	{  4608000, 4,  3,  64, 8 },
	{  5644800, 4, 12, 209, 8 },
	{  6144000, 4,  1,  16, 8 },
	{  6350400, 4,  2,  31, 8 },
	{  8192000, 4,  1,  12, 8 },
	{  8467200, 4, 18, 209, 8 },
	{  9216000, 4,  3,  32, 8 },
	{ 11289600, 4, 24, 209, 8 },
	{ 12288000, 4,  1,   8, 8 },
	{ 12700800, 4, 27, 209, 8 },
	{ 13824000, 4,  9,  64, 8 },
	{ 16384000, 4,  1,   6, 8 },
	{ 16934400, 4, 41, 238, 8 },
	{ 18432000, 4,  3,  16, 8 },
	{ 22579200, 2, 24, 209, 8 },
	{ 24576000, 4,  1,   4, 8 },
	{ 27648000, 4,  9,  32, 8 },
	{ 33868800, 4, 41, 119, 8 },
	{ 36864000, 4,  3,   8, 8 },
	{ 45158400, 1, 24, 209, 8 },
	{ 49152000, 4,  1,   2, 8 },
	{ 50803200, 1, 27, 209, 8 },
	{ }
};

static int lcc_init_enable_pll0(IpqLccClocks *bus)
{
	IpqLccGccRegs *gcc_regs = bus->gcc_apcs_regs;
	IpqLccPll0Regs *pll0_regs = bus->lcc_pll0_regs;
	IpqLccPllRegs *pll_regs = bus->lcc_pll_regs;
	uint32_t regval;

	regval = 0;
	regval = 15 << LCC_PLL0_L_SHIFT & LCC_PLL0_L_MASK;
	write32(&pll0_regs->l_val, regval);

	regval = 0;
	regval = 145 << LCC_PLL0_M_SHIFT & LCC_PLL0_M_MASK;
	write32(&pll0_regs->m_val, regval);

	regval = 0;
	regval = 199 << LCC_PLL0_N_SHIFT & LCC_PLL0_N_MASK;
	write32(&pll0_regs->n_val, regval);

	regval = 0;
	regval |= LCC_PLL0_CFG_LV_MAIN_ENABLE;
	regval |= LCC_PLL0_CFG_FRAC_ENABLE;
	write32(&pll0_regs->config, regval);

	regval = 0;
	regval |= LCC_PLL_PCLK_SRC_PRI;
	write32(&pll_regs->pri, regval);

	regval = 0;
	regval |= 1 << LCC_PLL0_MODE_BIAS_CNT_SHIFT &
			LCC_PLL0_MODE_BIAS_CNT_MASK;
	regval |= 8 << LCC_PLL0_MODE_LOCK_CNT_SHIFT &
			LCC_PLL0_MODE_LOCK_CNT_MASK;
	write32(&pll0_regs->mode, regval);

	regval = read32(&gcc_regs->apcs);
	regval |= GCC_PLL_APCS_PLL4_ENABLE;
	write32(&gcc_regs->apcs, regval);

	regval = read32(&pll0_regs->mode);
	regval |= LCC_PLL0_MODE_FSM_VOTE_ENABLE;
	write32(&pll0_regs->mode, regval);

	mdelay(1);

	regval = read32(&pll0_regs->status);
	if (regval & LCC_PLL0_STAT_ACTIVE_MASK)
		return 0;

	printk(BIOS_ERR, "%s: error enabling PLL4 clock\n", __func__);
	return 1;
}

static int lcc_init_enable_ahbix(IpqLccClocks *bus)
{
	IpqLccAhbixRegs *ahbix_regs = bus->lcc_ahbix_regs;
	uint32_t regval;

	regval = 0;
	regval |= 1 << LCC_AHBIX_MD_M_VAL_SHIFT & LCC_AHBIX_MD_M_VAL_MASK;
	regval |= 252 << LCC_AHBIX_MD_NOT_2D_VAL_SHIFT &
			LCC_AHBIX_MD_NOT_2D_VAL_MASK;
	write32(&ahbix_regs->md, regval);

	regval = 0;
	regval |= 253 << LCC_AHBIX_NS_N_VAL_SHIFT & LCC_AHBIX_NS_N_VAL_MASK;
	regval |= LCC_AHBIX_NS_CRC_ENABLE;
	regval |= LCC_AHBIX_NS_GFM_SEL_MNC;
	regval |= LCC_AHBIX_NS_MNC_CLK_ENABLE;
	regval |= LCC_AHBIX_NS_MNC_ENABLE;
	regval |= LCC_AHBIX_NS_MNC_MODE_DUAL;
	regval |= LCC_AHBIX_NS_PREDIV_BYPASS;
	regval |= LCC_AHBIX_NS_MN_SRC_LPA;
	write32(&ahbix_regs->ns, regval);

	mdelay(1);

	regval = read32(&ahbix_regs->status);
	if (regval & LCC_AHBIX_STAT_AIF_CLK_MASK)
		return 0;

	printk(BIOS_ERR, "%s: error enabling AHBIX clock\n", __func__);
	return 1;
}

static int lcc_init_mi2s(IpqLccClocks *bus, unsigned int freq)
{
	IpqLccMi2sRegs *mi2s_regs = bus->lcc_mi2s_regs;
	uint32_t regval;
	uint8_t pd, m, n, d;
	unsigned int i;

	i = 0;
	while (lcc_mi2s_freq_tbl[i].freq != 0) {
		if (lcc_mi2s_freq_tbl[i].freq == freq)
			break;
		++i;
	}
	if (lcc_mi2s_freq_tbl[i].freq == 0) {
		printk(BIOS_ERR, "%s: invalid frequency given: %u\n",
		       __func__, freq);
		return 1;
	}

	switch (lcc_mi2s_freq_tbl[i].pd) {
	case 1:
		pd = LCC_MI2S_NS_PREDIV_BYPASS;
		break;
	case 2:
		pd = LCC_MI2S_NS_PREDIV_DIV2;
		break;
	case 4:
		pd = LCC_MI2S_NS_PREDIV_DIV4;
		break;
	default:
		printk(BIOS_ERR, "%s: invalid prediv found: %u\n", __func__,
				lcc_mi2s_freq_tbl[i].pd);
		return 1;
	}

	m = lcc_mi2s_freq_tbl[i].m;
	n = ~(lcc_mi2s_freq_tbl[i].n - m);
	d = ~(lcc_mi2s_freq_tbl[i].d * 2);

	regval = 0;
	regval |= m << LCC_MI2S_MD_M_VAL_SHIFT & LCC_MI2S_MD_M_VAL_MASK;
	regval |= d << LCC_MI2S_MD_NOT_2D_VAL_SHIFT &
			LCC_MI2S_MD_NOT_2D_VAL_MASK;
	write32(&mi2s_regs->md, regval);

	regval = 0;
	regval |= n << LCC_MI2S_NS_N_VAL_SHIFT & LCC_MI2S_NS_N_VAL_MASK;
	regval |= LCC_MI2S_NS_BIT_DIV_DIV4;
	regval |= LCC_MI2S_NS_MNC_CLK_ENABLE;
	regval |= LCC_MI2S_NS_MNC_ENABLE;
	regval |= LCC_MI2S_NS_MNC_MODE_DUAL;
	regval |= pd;
	regval |= LCC_MI2S_NS_MN_SRC_LPA;
	write32(&mi2s_regs->ns, regval);

	return 0;
}

static int lcc_enable_mi2s(IpqLccClocks *bus)
{
	IpqLccMi2sRegs *mi2s_regs = bus->lcc_mi2s_regs;
	uint32_t regval;

	regval = read32(&mi2s_regs->ns);
	regval |= LCC_MI2S_NS_OSR_CXC_ENABLE;
	regval |= LCC_MI2S_NS_BIT_CXC_ENABLE;
	write32(&mi2s_regs->ns, regval);

	udelay(10);

	regval = read32(&mi2s_regs->status);
	if (regval & LCC_MI2S_STAT_OSR_CLK_MASK)
		if (regval & LCC_MI2S_STAT_BIT_CLK_MASK)
			return 0;

	printk(BIOS_ERR, "%s: error enabling MI2S clocks: %u\n",
	       __func__, regval);
	return 1;
}

int audio_clock_config(unsigned int frequency)
{
	IpqLccClocks bus = {
		.gcc_apcs_regs = (void *)(MSM_GCC_BASE + GCC_PLL_APCS_REG),
		.lcc_pll0_regs = (void *)(MSM_LPASS_LCC_BASE + LCC_PLL0_MODE_REG),
		.lcc_ahbix_regs = (void *)(MSM_LPASS_LCC_BASE + LCC_AHBIX_NS_REG),
		.lcc_mi2s_regs = (void *)(MSM_LPASS_LCC_BASE + LCC_MI2S_NS_REG),
		.lcc_pll_regs = (void *)(MSM_LPASS_LCC_BASE + LCC_PLL_PCLK_REG),
	};

	if (lcc_init_enable_pll0(&bus))
		return 1;
	if (lcc_init_enable_ahbix(&bus))
		return 1;
	if (lcc_init_mi2s(&bus, frequency))
		return 1;
	if (lcc_enable_mi2s(&bus))
		return 1;

	return 0;
}
