/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <endian.h>
#include <soc/emi.h>
#include <soc/spm.h>
#include <timer.h>

#define BUF_SIZE (16 * KiB)
static uint8_t spm_bin[BUF_SIZE] __aligned(8);

static int spm_register_init(void)
{
	u32 pcm_fsm_sta;

	write32(&mtk_spm->poweron_config_set,
		SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB | MD_BCLK_CG_EN_LSB);

	write32(&mtk_spm->spm_power_on_val1, POWER_ON_VAL1_DEF);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
		PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	pcm_fsm_sta = read32(&mtk_spm->pcm_fsm_sta);

	if ((pcm_fsm_sta & PCM_FSM_STA_MASK) != PCM_FSM_STA_DEF) {
		printk(BIOS_ERR, "PCM reset failed\n");
		return -1;
	}

	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
		EN_IM_SLEEP_DVS_LSB);
	write32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | EVENT_LOCK_EN_LSB |
		SPM_SRAM_ISOINT_B_LSB | MIF_APBEN_LSB |
		SCP_APB_INTERNAL_EN_LSB);
	write32(&mtk_spm->pcm_im_ptr, 0);
	write32(&mtk_spm->pcm_im_len, 0);

	write32(&mtk_spm->spm_clk_con,
		read32(&mtk_spm->spm_clk_con) | SYSCLK1_EN_CTRL |
		SPM_LOCK_INFRA_DCM_LSB | EXT_SRCCLKEN_MASK |
		CXO32K_REMOVE_EN_MD1_LSB |
		CLKSQ1_SEL_CTRL_LSB | SRCLKEN0_EN_LSB | SYSCLK1_SRC_MASK_B);

	write32(&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);

	write32(&mtk_spm->spm_irq_mask, ISRM_ALL);
	write32(&mtk_spm->spm_irq_sta, ISRC_ALL);
	write32(&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);

	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	write32(&mtk_spm->ddr_en_dbc_len,
		MD_DDR_EN_0_DBC_LEN |
		MD_DDR_EN_1_DBC_LEN |
		CONN_DDR_EN_DBC_LEN);

	clrsetbits32(&mtk_spm->spare_ack_mask,
		     SPARE_ACK_MASK_B_BIT1,
		     SPARE_ACK_MASK_B_BIT0);

	write32(&mtk_spm->sysrom_con, IFR_SRAMROM_ROM_PDN);
	write32(&mtk_spm->spm_pc_trace_con,
		SPM_PC_TRACE_OFFSET |
		SPM_PC_TRACE_HW_EN_LSB);

	setbits32(&mtk_spm->spare_src_req_mask, SPARE1_DDREN_MASK_B_LSB);

	return 0;
}

static int spm_code_swapping(void)
{
	u32 con1;

	con1 = read32(&mtk_spm->spm_wakeup_event_mask);

	write32(&mtk_spm->spm_wakeup_event_mask,
		con1 & ~WAKEUP_EVENT_MASK_B_BIT0);
	write32(&mtk_spm->spm_cpu_wakeup_event, 1);

	if (!wait_us(SPM_CORE_TIMEOUT,
		     read32(&mtk_spm->spm_irq_sta) & PCM_IRQ_ROOT_MASK_LSB)) {
		printk(BIOS_ERR,
		       "timeout: r15=%#x, pcmsta=%#x, irqsta=%#x [%d]\n",
		       read32(&mtk_spm->pcm_reg15_data),
		       read32(&mtk_spm->pcm_fsm_sta),
		       read32(&mtk_spm->spm_irq_sta),
		       SPM_CORE_TIMEOUT);
		return -1;
	}

	write32(&mtk_spm->spm_cpu_wakeup_event, 0);
	write32(&mtk_spm->spm_wakeup_event_mask, con1);
	return 0;
}

static int spm_reset_and_init_pcm(const struct pcm_desc *pcmdesc)
{
	u32 con1, pcm_fsm_sta;

	if (read32(&mtk_spm->pcm_reg1_data) == SPM_PCM_REG1_DATA_CHECK &&
	    read32(&mtk_spm->pcm_reg15_data) != SPM_PCM_REG15_DATA_CHECK) {
		if (spm_code_swapping())
			return -1;
		write32(&mtk_spm->spm_power_on_val0,
			read32(&mtk_spm->pcm_reg0_data));
	}

	write32(&mtk_spm->pcm_pwr_io_en, 0);

	clrsetbits32(&mtk_spm->pcm_con1,
		     PCM_TIMER_EN_LSB,
		     SPM_REGWR_CFG_KEY);

	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
		PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	pcm_fsm_sta = read32(&mtk_spm->pcm_fsm_sta);

	if ((pcm_fsm_sta & PCM_FSM_STA_MASK) != PCM_FSM_STA_DEF) {
		printk(BIOS_ERR, "reset pcm(PCM_FSM_STA=%#x)\n",
		       read32(&mtk_spm->pcm_fsm_sta));
		return -1;
	}

	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
		EN_IM_SLEEP_DVS_LSB);

	con1 = read32(&mtk_spm->pcm_con1) & PCM_WDT_WAKE_MODE_LSB;
	write32(&mtk_spm->pcm_con1, con1 | SPM_REGWR_CFG_KEY |
		EVENT_LOCK_EN_LSB | SPM_SRAM_ISOINT_B_LSB |
		(pcmdesc->replace ? 0 : IM_NONRP_EN_LSB) |
		MIF_APBEN_LSB | SCP_APB_INTERNAL_EN_LSB);

	return 0;
}

static void spm_load_pcm_code(const struct dyna_load_pcm *pcm)
{
	int i;

	write32(&mtk_spm->pcm_con1, read32(&mtk_spm->pcm_con1) |
		SPM_REGWR_CFG_KEY | IM_SLAVE_LSB);

	for (i = 0; i < pcm->desc.size; i++) {
		write32(&mtk_spm->pcm_im_host_rw_ptr,
			PCM_IM_HOST_EN_LSB | PCM_IM_HOST_W_EN_LSB | i);
		write32(&mtk_spm->pcm_im_host_rw_dat,
			(u32)*(pcm->buf + i));
	}
	write32(&mtk_spm->pcm_im_host_rw_ptr, 0);
}

static void spm_check_pcm_code(const struct dyna_load_pcm *pcm)
{
	int i;

	for (i = 0; i < pcm->desc.size; i++) {
		write32(&mtk_spm->pcm_im_host_rw_ptr, PCM_IM_HOST_EN_LSB | i);
		if ((read32(&mtk_spm->pcm_im_host_rw_dat)) !=
			(u32)*(pcm->buf + i))
			spm_load_pcm_code(pcm);
	}
	write32(&mtk_spm->pcm_im_host_rw_ptr, 0);
}

static void spm_kick_im_to_fetch(const struct dyna_load_pcm *pcm)
{
	u32 con0;

	spm_load_pcm_code(pcm);
	spm_check_pcm_code(pcm);

	printk(BIOS_DEBUG, "%s: ptr = %p\n", __func__, pcm->buf);
	printk(BIOS_DEBUG, "%s: len = %d\n", __func__, pcm->desc.size);

	con0 = read32(&mtk_spm->pcm_con0) & ~(IM_KICK_L_LSB | PCM_KICK_L_LSB);
	write32(&mtk_spm->pcm_con0, con0 | SPM_REGWR_CFG_KEY |
		PCM_CK_EN_LSB | IM_KICK_L_LSB);
	write32(&mtk_spm->pcm_con0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
}

static void spm_init_pcm_register(void)
{
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val0));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R0);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);
}

static void spm_init_event_vector(const struct pcm_desc *pcmdesc)
{
	for (int i = 0; i < PCM_EVENT_VECTOR_NUM; i++)
		write32(&mtk_spm->pcm_event_vector[i], pcmdesc->vector[i]);
}

static const char * const dyna_load_pcm_path[] = {
	[DYNA_LOAD_PCM_SUSPEND_LP4_3733] = "pcm_allinone_lp4_3733.bin",
	[DYNA_LOAD_PCM_SUSPEND_LP4_3200] = "pcm_allinone_lp4_3200.bin",
};

static int spm_load_firmware(enum dyna_load_pcm_index index,
			     struct dyna_load_pcm *pcm)
{
	/*
	 * Layout:
	 *   u16 firmware_size
	 *   u32 binary[firmware_size]
	 *   struct pcm_desc descriptor
	 *   char *version
	 */
	u16 firmware_size;
	int copy_size;
	const char *file_name = dyna_load_pcm_path[index];
	struct stopwatch sw;

	stopwatch_init(&sw);

	size_t file_size = cbfs_load(file_name, spm_bin, sizeof(spm_bin));

	if (file_size == 0) {
		printk(BIOS_ERR, "SPM binary %s not found\n", file_name);
		return -1;
	}

	int offset = 0;

	/* firmware size */
	copy_size = sizeof(firmware_size);
	memcpy(&firmware_size, spm_bin + offset, copy_size);
	printk(BIOS_DEBUG, "SPM: binary array size = %d\n", firmware_size);
	offset += copy_size;

	/* binary */
	assert(offset < file_size);
	copy_size = firmware_size * 4;
	pcm->buf = (u32 *)(spm_bin + offset);
	offset += copy_size;

	/* descriptor */
	assert(offset < file_size);
	copy_size = sizeof(struct pcm_desc);
	memcpy((void *)&(pcm->desc.size), spm_bin + offset, copy_size);
	offset += copy_size;

	/* version */
	/* The terminating character should be contained in the spm binary */
	assert(spm_bin[file_size - 1] == '\0');
	assert(offset < file_size);
	printk(BIOS_DEBUG, "SPM: version = %s\n", spm_bin + offset);

	printk(BIOS_INFO, "SPM binary loaded in %lld msecs\n",
	       stopwatch_duration_msecs(&sw));

	return 0;
}

static void spm_kick_pcm_to_run(void)
{
	uint32_t con0;

	write32(&mtk_spm->spm_mas_pause_mask_b, SPM_MAS_PAUSE_MASK_B_VAL);
	write32(&mtk_spm->spm_mas_pause2_mask_b, SPM_MAS_PAUSE2_MASK_B_VAL);
	write32(&mtk_spm->pcm_reg_data_ini, 0);

	write32(&mtk_spm->pcm_pwr_io_en, PCM_PWRIO_EN_R0 | PCM_PWRIO_EN_R7);

	printk(BIOS_DEBUG, "SPM: %s\n", __func__);

	/* check IM ready */
	while ((read32(&mtk_spm->pcm_fsm_sta) & IM_STATE_MASK) != IM_STATE)
		;

	/* kick PCM to run, and toggle PCM_KICK */
	con0 = read32(&mtk_spm->pcm_con0) & ~(IM_KICK_L_LSB | PCM_KICK_L_LSB);
	write32(&mtk_spm->pcm_con0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB |
		PCM_KICK_L_LSB);
	write32(&mtk_spm->pcm_con0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	printk(BIOS_DEBUG, "SPM: %s done\n", __func__);
}

int spm_init(void)
{
	struct pcm_desc *pcmdesc;
	enum dyna_load_pcm_index index;
	struct stopwatch sw;

	stopwatch_init(&sw);

	if (CONFIG(MT8183_DRAM_EMCP))
		index = DYNA_LOAD_PCM_SUSPEND_LP4_3733;
	else
		index = DYNA_LOAD_PCM_SUSPEND_LP4_3200;

	printk(BIOS_DEBUG, "SPM: pcm index = %d\n", index);

	struct dyna_load_pcm pcm;
	if (spm_load_firmware(index, &pcm)) {
		printk(BIOS_ERR, "SPM: firmware is not ready\n");
		printk(BIOS_ERR, "SPM: check dram type and firmware version\n");
		return -1;
	}

	pcmdesc = &pcm.desc;

	if (spm_register_init())
		return -1;

	if (spm_reset_and_init_pcm(pcmdesc))
		return -1;

	spm_kick_im_to_fetch(&pcm);
	spm_init_pcm_register();
	spm_init_event_vector(pcmdesc);
	spm_kick_pcm_to_run();

	printk(BIOS_INFO, "SPM: %s done in %lld msecs\n", __func__,
	       stopwatch_duration_msecs(&sw));

	return 0;
}
