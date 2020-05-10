/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <soc/iomap.h>
#include <soc/gsbi.h>
#include <console/console.h>

static inline void *gsbi_ctl_reg_addr(gsbi_id_t gsbi_id)
{
	switch (gsbi_id) {
	case GSBI_ID_1:
		return GSBI1_CTL_REG;
	case GSBI_ID_2:
		return GSBI2_CTL_REG;
	case GSBI_ID_3:
		return GSBI3_CTL_REG;
	case GSBI_ID_4:
		return GSBI4_CTL_REG;
	case GSBI_ID_5:
		return GSBI5_CTL_REG;
	case GSBI_ID_6:
		return GSBI6_CTL_REG;
	case GSBI_ID_7:
		return GSBI7_CTL_REG;
	default:
		printk(BIOS_ERR, "Unsupported GSBI%d\n", gsbi_id);
		return 0;
	}
}

gsbi_return_t gsbi_init(gsbi_id_t gsbi_id, gsbi_protocol_t protocol)
{
	unsigned int reg_val;
	unsigned int m = 1;
	unsigned int n = 4;
	unsigned int pre_div = 4;
	unsigned int src = 3;
	unsigned int mnctr_mode = 2;
	void *gsbi_ctl = gsbi_ctl_reg_addr(gsbi_id);

	if (!gsbi_ctl)
		return GSBI_ID_ERROR;

	write32(GSBI_HCLK_CTL(gsbi_id),
		(1 << GSBI_HCLK_CTL_GATE_ENA) | (1 << GSBI_HCLK_CTL_BRANCH_ENA));

	if (gsbi_init_board(gsbi_id))
		return GSBI_UNSUPPORTED;

	write32(GSBI_QUP_APSS_NS_REG(gsbi_id), 0);
	write32(GSBI_QUP_APSS_MD_REG(gsbi_id), 0);

	reg_val = ((m & GSBI_QUP_APPS_M_MASK) << GSBI_QUP_APPS_M_SHFT) |
		  ((~n & GSBI_QUP_APPS_D_MASK) << GSBI_QUP_APPS_D_SHFT);
	write32(GSBI_QUP_APSS_MD_REG(gsbi_id), reg_val);

	reg_val = (((~(n - m)) & GSBI_QUP_APPS_N_MASK) <<
					GSBI_QUP_APPS_N_SHFT) |
		  ((mnctr_mode & GSBI_QUP_APPS_MNCTR_MODE_MSK) <<
				 GSBI_QUP_APPS_MNCTR_MODE_SFT) |
		  (((pre_div - 1) & GSBI_QUP_APPS_PRE_DIV_MSK) <<
				 GSBI_QUP_APPS_PRE_DIV_SFT) |
		  (src & GSBI_QUP_APPS_SRC_SEL_MSK);
	write32(GSBI_QUP_APSS_NS_REG(gsbi_id), reg_val);

	reg_val |= (1 << GSBI_QUP_APPS_ROOT_ENA_SFT) |
		   (1 << GSBI_QUP_APPS_MNCTR_EN_SFT);
	write32(GSBI_QUP_APSS_NS_REG(gsbi_id), reg_val);

	reg_val |= (1 << GSBI_QUP_APPS_BRANCH_ENA_SFT);
	write32(GSBI_QUP_APSS_NS_REG(gsbi_id), reg_val);

	/*Select i2c protocol*/
	write32(gsbi_ctl,
		((GSBI_CTL_PROTO_I2C & GSBI_CTL_PROTO_CODE_MSK) << GSBI_CTL_PROTO_CODE_SFT));

	return GSBI_SUCCESS;
}
