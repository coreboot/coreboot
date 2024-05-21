/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/mcu_common.h>
#include <soc/mcupm.h>
#include <soc/mcupm_plat.h>
#include <soc/symbols.h>

static int eb_sleep_protect(void)
{
	/* Set MCU_PORT_SET_W1C_0[9:8] = 2'b11 */
	setbits32p(MCU_PORT_SET_W1C_0, CPUEB_PROTECT_EN_0 | CPUEB_PROTECT_EN_1);

	/* Poll MCU_PORT_SET_R0_0[9:8] = 2'b00 */
	if (!retry(POLLING_MCU_RETRY_COUNTS,
		   (read32p(MCU_PORT_SET_R0_0) &
		    (CPUEB_PROTECT_RDY_0 | CPUEB_PROTECT_RDY_1)) == 0, udelay(1))) {
		printk(BIOS_ERR,
		       "[EB_SPMC] CPUEB_PROTECT_RDY not set after %d us, value %#x\n",
		       POLLING_MCU_RETRY_COUNTS, read32p(MCU_PORT_SET_R0_0));
		return -1;
	}

	return 0;
}

static int set_cpueb_state(bool state_valid)
{
	if (state_valid)
		setbits32p(SPM_MCUPM_SPMC_CON, CPUEB_STATE_VALID);
	else
		clrbits32p(SPM_MCUPM_SPMC_CON, CPUEB_STATE_VALID);

	if (!retry(POLLING_ACK_RETRY_COUNTS,
		   ((read32p(SPM_MCUPM_SPMC_CON) & CPUEB_STATE_FINISH_ACK) != 0) == state_valid,
		   udelay(1))) {
		printk(BIOS_ERR, "[EB_SPMC] Polling ACK timeout, %#x\n",
		       read32p(SPM_MCUPM_SPMC_CON));
		return -1;
	}

	return 0;
}

static int eb_spmc_spm(void)
{
	/* Unlock SPM POWERON_CONFIG_EN */
	setbits32p(POWERON_CONFIG_EN_MCU, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Set REQ_PWR_ON = 1 and REQ_MEM_RET = 0 */
	setbits32p(SPM_MCUPM_SPMC_CON, REQ_PWR_ON);

	clrbits32p(SPM_MCUPM_SPMC_CON, REQ_MEM_RET);

	/* Set CPUEB_STATE_VALID = 1 */
	if (set_cpueb_state(true))
		return -1;

	/* Set CPUEB_STATE_VALID = 0 */
	if (set_cpueb_state(false))
		return -1;

	return eb_sleep_protect();
}

static int eb_spmc(void)
{
	uint32_t spmc_status = (read32p(CPUEB_SPMC_STATUS) >> CPUEB_SPMC_STATUS_OFFSET) &
			       CPUEB_SPMC_STATUS_MASK;
	int ret;

	switch (spmc_status) {
	case CPUEB_FSM_STATE_OFF:
	case CPUEB_FSM_STATE_ON:
	case CPUEB_FSM_STATE_RET:
	case CPUEB_FSM_STATE_FUNC_RET:
		printk(BIOS_INFO, "[EB_SPMC] spmc_status = %#x\n", spmc_status);
		ret = eb_spmc_spm();
		break;
	default:
		printk(BIOS_ERR, "[EB_SPMC] EB SPMC in wrong state, spmc_status = %#x\n",
		       spmc_status);
		ret = -1;
		break;
	}

	return ret;
}

static void reset_mcupm(struct mtk_mcu *mcu)
{
	/* Clear abnormal boot register */
	write32p(ABNORMALBOOT_REG, ABNORMALBOOT_REG_STATUS);
	write32p(WARMBOOT_REG, WARMBOOT_REG_STATUS);
	write32(&mcupm_reg->sw_rstn, MCUPM_RSTN_RESET);

	write32p(MCUPM_SW_RSTN,
		 MCUPM_CFGREG_SW_RSTN_SW_RSTN | MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK);
}

static struct mtk_mcu mcupm = {
	.firmware_name = CONFIG_MCUPM_FIRMWARE,
	.run_address = (void *)MCUPM_SRAM_BASE,
	.reset = reset_mcupm,
};

void mcupm_init(void)
{
	mcupm.load_buffer = _dram_dma;
	mcupm.buffer_size = REGION_SIZE(dram_dma);

	/* Set CPUEB as secure master */
	printk(BIOS_INFO, "Before: DEVAPC_MCUPM = %#x\n",
	       read32p(DEVAPC_INFRA_SECU_AO_SEC_REPLACE_0));
	setbits32p(DEVAPC_INFRA_SECU_AO_SEC_REPLACE_0, BIT(15));
	printk(BIOS_INFO, "After: DEVAPC_MCUPM = %#x\n",
	       read32p(DEVAPC_INFRA_SECU_AO_SEC_REPLACE_0));

	if (eb_spmc() != 0)
		die("%s: eb sram power on failed\n", __func__);
	write32(&mcupm_reg->sw_rstn, MCUPM_RSTN_RSTN_INIT);
	if (mtk_init_mcu(&mcupm))
		die("%s: failed\n", __func__);
}
