/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/clkbuf.h>
#include <soc/pmif.h>
#if CONFIG(SRCLKEN_RC_SUPPORT)
#include <soc/srclken_rc.h>
#endif

#define BUFTAG			"[CLKBUF]"
#define buf_info(fmt, arg ...)	printk(BIOS_INFO, BUFTAG "%s,%d: " fmt, \
					__func__, __LINE__, ## arg)

#define _buf_clrset32_impl(addr, clear, set) \
	buf_write(addr, (buf_read(addr) & ~((uint32_t)(clear))) | (set))

#define BUF_SET_FIELDS(addr, ...) \
	_BF_IMPL(_buf_clrset32_impl, addr, __VA_ARGS__)
#define BUF_READ_FIELD(addr, name) \
	EXTRACT_BITFIELD(buf_read(addr), name)

#define PMIC_CW00_INIT_VAL	0x4005 /* 0100 0000 0000 0101 */
#define PMIC_CW09_INIT_VAL	0x01F0 /* 0000 0001 1111 0000 */

static struct pmif *pmif_arb;

static u32 buf_read(u32 addr)
{
	u32 rdata;

	if (!pmif_arb)
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);
	pmif_arb->read(pmif_arb, 0, addr, &rdata);

	return rdata;
}

static void buf_write(u32 addr, u32 wdata)
{
	if (!pmif_arb)
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);
	pmif_arb->write(pmif_arb, 0, addr, wdata);
}

static void dump_clkbuf_log(void)
{
	u32 pmic_cw00, pmic_cw09, pmic_cw12, pmic_cw13, pmic_cw15, pmic_cw19,
	    top_spi_con1, ldo_vrfck_op_en, ldo_vbbck_op_en, ldo_vrfck_en,
	    ldo_vbbck_en, vrfck_hv_en;

	pmic_cw00 = BUF_READ_FIELD(PMIC_RG_DCXO_CW00, PMIC_REG_COMMON);
	pmic_cw09 = BUF_READ_FIELD(PMIC_RG_DCXO_CW09, PMIC_REG_COMMON);
	pmic_cw12 = BUF_READ_FIELD(PMIC_RG_DCXO_CW12, PMIC_REG_COMMON);
	pmic_cw13 = BUF_READ_FIELD(PMIC_RG_DCXO_CW13, PMIC_REG_COMMON);
	pmic_cw15 = BUF_READ_FIELD(PMIC_RG_DCXO_CW15, PMIC_REG_COMMON);
	pmic_cw19 = BUF_READ_FIELD(PMIC_RG_DCXO_CW19, PMIC_REG_COMMON);
	buf_info("DCXO_CW00/09/12/13/15/19=%#x %#x %#x %#x %#x %#x\n",
		pmic_cw00, pmic_cw09, pmic_cw12,
		pmic_cw13, pmic_cw15, pmic_cw19);

	top_spi_con1 = BUF_READ_FIELD(PMIC_RG_TOP_SPI_CON1, PMIC_RG_SRCLKEN_IN3_EN);
	ldo_vrfck_op_en = BUF_READ_FIELD(PMIC_RG_LDO_VRFCK_OP_EN,
					  PMIC_RG_LDO_VRFCK_HW14_OP_EN);
	ldo_vbbck_op_en = BUF_READ_FIELD(PMIC_RG_LDO_VBBCK_OP_EN,
					  PMIC_RG_LDO_VBBCK_HW14_OP_EN);
	ldo_vrfck_en = BUF_READ_FIELD(PMIC_RG_LDO_VRFCK_CON0, PMIC_RG_LDO_VRFCK_EN);
	ldo_vbbck_en = BUF_READ_FIELD(PMIC_RG_LDO_VBBCK_CON0, PMIC_RG_LDO_VBBCK_EN);
	buf_info("spi_con1/ldo_rf_op/ldo_bb_op/ldo_rf_en/ldo_bb_en=%#x %#x %#x %#x %#x\n",
		top_spi_con1, ldo_vrfck_op_en, ldo_vbbck_op_en,
		ldo_vrfck_en, ldo_vbbck_en);

	vrfck_hv_en = BUF_READ_FIELD(PMIC_RG_DCXO_ADLDO_BIAS_ELR_0, PMIC_RG_VRFCK_HV_EN);
	buf_info("clk buf vrfck_hv_en=%#x\n", vrfck_hv_en);
}

int clk_buf_init(void)
{
	/* Dump registers before setting */
	dump_clkbuf_log();

	/* Unlock pmic key */
	BUF_SET_FIELDS(PMIC_TOP_TMA_KEY, PMIC_REG_COMMON, PMIC_TOP_TMA_KEY_UNLOCK);

	/* 1.1 Set VRFCK input supply(11.ac mode) */
	BUF_SET_FIELDS(PMIC_RG_DCXO_ADLDO_BIAS_ELR_0, PMIC_RG_VRFCK_HV_EN, 0x0);

	/* 1.2.0 Set VRFCK En = 0 */
	BUF_SET_FIELDS(PMIC_RG_LDO_VRFCK_CON0, PMIC_RG_LDO_VRFCK_EN, 0x0);
	/* 1.2.1 Set VRFCK1 as power src */
	BUF_SET_FIELDS(PMIC_RG_LDO_VRFCK_ELR, PMIC_RG_LDO_VRFCK_ANA_SEL, 0x1);

	/* 1.2.2 Switch LDO-RFCK to LDO-RFCK1 */
	BUF_SET_FIELDS(PMIC_RG_DCXO_ADLDO_BIAS_ELR_0, PMIC_RG_VRFCK_NDIS_EN, 0x0);
	BUF_SET_FIELDS(PMIC_RG_DCXO_ADLDO_BIAS_ELR_1, PMIC_RG_VRFCK_1_NDIS_EN, 0x1);

	/* 1.2.0 Set VRFCK En = 1 */
	BUF_SET_FIELDS(PMIC_RG_LDO_VRFCK_CON0, PMIC_RG_LDO_VRFCK_EN, 0x1);

	/* 1.2.3 Lock pmic key */
	BUF_SET_FIELDS(PMIC_TOP_TMA_KEY, PMIC_REG_COMMON, 0x0);

	/* Enable XO LDO */
	BUF_SET_FIELDS(PMIC_RG_LDO_VRFCK_OP_EN_SET, PMIC_RG_LDO_VRFCK_HW14_OP_EN, 0x1);
	BUF_SET_FIELDS(PMIC_RG_LDO_VBBCK_OP_EN_SET, PMIC_RG_LDO_VBBCK_HW14_OP_EN, 0x1);
	BUF_SET_FIELDS(PMIC_RG_LDO_VRFCK_CON0, PMIC_RG_LDO_VRFCK_EN, 0x0);
	BUF_SET_FIELDS(PMIC_RG_LDO_VBBCK_CON0, PMIC_RG_LDO_VBBCK_EN, 0x0);

	/* Enable 26M control */
	if (!CONFIG(SRCLKEN_RC_SUPPORT)) {
		/* Legacy co-clock mode */
		BUF_SET_FIELDS(PMIC_RG_TOP_SPI_CON1, PMIC_RG_SRCLKEN_IN3_EN, 0x0);

		BUF_SET_FIELDS(PMIC_RG_DCXO_CW00, PMIC_REG_COMMON, PMIC_CW00_INIT_VAL);
		BUF_SET_FIELDS(PMIC_RG_DCXO_CW09, PMIC_REG_COMMON, PMIC_CW09_INIT_VAL);
	} else {
		/* New co-clock mode */
		/* All XO mode should set to 2'b01 */
		BUF_SET_FIELDS(PMIC_RG_DCXO_CW00, PMIC_REG_COMMON, PMIC_CW00_INIT_VAL);
		BUF_SET_FIELDS(PMIC_RG_DCXO_CW09, PMIC_REG_COMMON, PMIC_CW09_INIT_VAL);

		/* 1. Update control mapping table */
		BUF_SET_FIELDS(PMIC_RG_XO_BUF_CTL0, PMIC_RG_XO_VOTE, 0x005);
		BUF_SET_FIELDS(PMIC_RG_XO_BUF_CTL1, PMIC_RG_XO_VOTE, 0x0);
		BUF_SET_FIELDS(PMIC_RG_XO_BUF_CTL2, PMIC_RG_XO_VOTE, 0x0);
		BUF_SET_FIELDS(PMIC_RG_XO_BUF_CTL3, PMIC_RG_XO_VOTE, 0x0);
		BUF_SET_FIELDS(PMIC_RG_XO_BUF_CTL4, PMIC_RG_XO_VOTE, 0x0);
		/* Wait 100us */
		udelay(100);

		/* 2. Switch to new control mode */
		BUF_SET_FIELDS(PMIC_RG_DCXO_CW08, PMIC_RG_XO_PMIC_TOP_DIG_SW, 0x0);
	}

	/* Check if the setting is ok */
	dump_clkbuf_log();

	return 0;
}
