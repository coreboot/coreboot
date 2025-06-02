/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/auxadc.h>
#include <soc/efuse.h>
#include <timer.h>

static struct mtk_auxadc_regs *const mtk_auxadc = (void *)AUXADC_BASE;

#define ADC_CALI_EN_A_SHIFT	20
#define ADC_CALI_EN_A_MASK	(0x1 << ADC_CALI_EN_A_SHIFT)

static int cali_oe;
static int cali_ge;
static int calibrated = 0;
static void mt_auxadc_update_cali(void)
{
	uint32_t cali_reg;
	int cali_ge_a;
	int cali_oe_a;

	cali_reg = read32(&mtk_efuse->adc_cali_reg);

	if ((cali_reg & ADC_CALI_EN_A_MASK) != 0) {
		cali_oe_a = (cali_reg & ADC_OE_A_MASK) >> ADC_OE_A_SHIFT;
		cali_ge_a = (cali_reg & ADC_GE_A_MASK) >> ADC_GE_A_SHIFT;
		cali_ge = cali_ge_a - 512;
		cali_oe = cali_oe_a - 512;
	}
}

static uint32_t auxadc_get_rawdata(int channel)
{
	setbits32(&AUXADC_SWCG_CLR_REG, BIT(AUXADC_SWCG_EN_BIT));
	assert(wait_ms(300, !(read32(&mtk_auxadc->con2) & 0x1)));

	clrbits32(&mtk_auxadc->con1, 1 << channel);
	assert(wait_ms(300, !(read32(&mtk_auxadc->data[channel]) & (1 << 12))));

	setbits32(&mtk_auxadc->con1, 1 << channel);
	udelay(25);
	assert(wait_ms(300, read32(&mtk_auxadc->data[channel]) & (1 << 12)));

	uint32_t value = read32(&mtk_auxadc->data[channel]) & 0x0FFF;

	setbits32(&AUXADC_SWCG_SET_REG, BIT(AUXADC_SWCG_EN_BIT));

	return value;
}

unsigned int auxadc_get_voltage_uv(unsigned int channel)
{
	uint32_t raw_value;
	assert(channel < 16);

	if (!calibrated) {
		mt_auxadc_update_cali();
		calibrated = 1;
	}

	/* 1.5V in 4096 steps */
	raw_value = auxadc_get_rawdata(channel);
	raw_value = raw_value - cali_oe;
	return (unsigned int)((int64_t)raw_value * 1500000 / (4096 + cali_ge));
}
