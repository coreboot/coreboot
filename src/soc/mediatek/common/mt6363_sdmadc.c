/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * These values are used by MediaTek internally.
 * We can find these registers in "MT6363TP_PMIC_Design_Notice_for_MT8196G_V0.2"
 * Chapter number: 2.15.
 * The setting values are provided by MeidaTek designers.
 */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6363.h>
#include <soc/mt6363_sdmadc.h>
#include <soc/pmif.h>
#include <timer.h>

#define ERR_INVALID_ARGS        (-8)
#define ERR_TIMED_OUT           (-13)

#define AUXADC_RDY_BIT		BIT(15)
/* 0.385us(2.89MHz) x 42T (SPL(100K=29T)+ADC_compare(12T)+sync(1T)) ~= 16us */
#define AUXADC_AVG_TIME_US	16
#define AUXADC_POLL_TIME_US	100
#define AUXADC_TIMEOUT_US	32000
#define VOLT_FULL		1840

#define EXT_THR_SEL_DEF_VALUE	0x80
#define EXT_THR_PURES_SHIFT	3

#define SDMADC_CHAN_SPEC(src_sel) \
{ \
	.hw_info = { \
		.ref_volt = VOLT_FULL, \
		.min_time = 128 * AUXADC_AVG_TIME_US, \
		.max_time = AUXADC_TIMEOUT_US, \
		.poll_time = AUXADC_POLL_TIME_US, \
		.ratio = { 1, 1 }, \
		.enable_reg = MT6363_AUXADC_RQST1, \
		.enable_mask = BIT(4), \
		.ready_reg = PMIC_AUXADC_ADC_OUT_CH12_L_ADDR, \
		.ready_mask = AUXADC_RDY_BIT, \
		.res = 15, \
	}, \
	.sdmadc_hw_info = { \
		.set_reg = PMIC_SDMADC_EXT_THR_SRC_SEL_ADDR, \
		.cmd = EXT_THR_SEL_DEF_VALUE | src_sel, \
	}, \
}

static const struct auxadc_chan_spec mt6363_sdmadc_chan_specs[] = {
	[AUXADC_CHAN_VIN1] = SDMADC_CHAN_SPEC(1),
	[AUXADC_CHAN_VIN2] = SDMADC_CHAN_SPEC(2),
	[AUXADC_CHAN_VIN3] = SDMADC_CHAN_SPEC(3),
	[AUXADC_CHAN_VIN4] = SDMADC_CHAN_SPEC(4),
	[AUXADC_CHAN_VIN5] = SDMADC_CHAN_SPEC(5),
	[AUXADC_CHAN_VIN6] = SDMADC_CHAN_SPEC(6),
	[AUXADC_CHAN_VIN7] = SDMADC_CHAN_SPEC(7),
};

_Static_assert(ARRAY_SIZE(mt6363_sdmadc_chan_specs) == AUXADC_CHAN_MAX,
	       "Wrong array size for mt6363_sdmadc_chan_specs");

int mt6363_sdmadc_read(enum auxadc_channel channel, int *val,
		       enum sdmadc_pures pures, enum auxadc_val_type type)
{
	u8 wdata;
	u32 regval;
	u32 elapsed = 0;
	const struct auxadc_chan_spec *chan;

	if (channel < AUXADC_CHAN_VIN1 || channel >= AUXADC_CHAN_MAX) {
		printk(BIOS_ERR, "[%s] Invalid channel %d\n", __func__, channel);
		return ERR_INVALID_ARGS;
	}

	mt6363_init_pmif_arb();
	chan = &mt6363_sdmadc_chan_specs[channel];

	wdata = chan->sdmadc_hw_info.cmd | (pures << EXT_THR_PURES_SHIFT);
	mt6363_write8(chan->sdmadc_hw_info.set_reg, wdata);

	wdata = chan->hw_info.enable_mask;
	mt6363_write8(chan->hw_info.enable_reg, wdata);

	udelay(chan->hw_info.min_time);
	elapsed += chan->hw_info.min_time;

	while (1) {
		regval = mt6363_read16(chan->hw_info.ready_reg);
		if (regval & chan->hw_info.ready_mask)
			break;
		if (elapsed > chan->hw_info.max_time) {
			printk(BIOS_ERR, "[%s] Auxadc read time out", __func__);
			return  ERR_TIMED_OUT;
		}
		udelay(chan->hw_info.poll_time);
		elapsed += chan->hw_info.poll_time;
	}

	regval &= (BIT(chan->hw_info.res) - 1);
	switch (type) {
	case AUXADC_VAL_RAW:
		*val = regval;
		break;
	case AUXADC_VAL_PROCESSED:
		*val = ((regval * chan->hw_info.ratio[0] * chan->hw_info.ref_volt) /
			chan->hw_info.ratio[1]) >> chan->hw_info.res;
		break;
	}
	return 0;
}
