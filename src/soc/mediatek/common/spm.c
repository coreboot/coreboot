/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mcu_common.h>
#include <soc/spm.h>
#include <soc/symbols.h>
#include <string.h>
#include <timer.h>

#define SPMFW_HEADER_SIZE 16

__weak void spm_extern_initialize(void) { /* do nothing */ }

static void spm_set_sysclk_settle(void)
{
	write32(&mtk_spm->spm_clk_settle, SPM_SYSCLK_SETTLE);
}

static void spm_kick_im_to_fetch(const struct dyna_load_pcm *pcm)
{
	uintptr_t ptr;
	u32 dmem_words;
	u32 pmem_words;
	u32 total_words;
	u32 pmem_start;
	u32 dmem_start;

	ptr = (uintptr_t)pcm->buf + SPM_SYSTEM_BASE_OFFSET;
	pmem_words = pcm->desc.pmem_words;
	total_words = pcm->desc.total_words;
	dmem_words = total_words - pmem_words;
	pmem_start = pcm->desc.pmem_start;
	dmem_start = pcm->desc.dmem_start;

	printk(BIOS_DEBUG, "%s: ptr = %#lx, pmem/dmem words = %#x/%#x\n",
	       __func__, (long)ptr, pmem_words, dmem_words);

	/* DMA needs 16-byte aligned source data. */
	assert(ptr % 16 == 0);

	write32(&mtk_spm->md32pcm_dma0_src, ptr);
	write32(&mtk_spm->md32pcm_dma0_dst, pmem_start);
	write32(&mtk_spm->md32pcm_dma0_wppt, pmem_words);
	write32(&mtk_spm->md32pcm_dma0_wpto, dmem_start);
	write32(&mtk_spm->md32pcm_dma0_count, total_words);
	write32(&mtk_spm->md32pcm_dma0_con, MD32PCM_DMA0_CON_VAL);
	write32(&mtk_spm->md32pcm_dma0_start, MD32PCM_DMA0_START_VAL);

	setbits32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
}

static void spm_init_pcm_register(void)
{
	/* Init r0 with POWER_ON_VAL0 */
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val0));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R0);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* Init r7 with POWER_ON_VAL1 */
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);
}

static void spm_set_pcm_flags(const struct pwr_ctrl *pwrctrl)
{
	u32 pcm_flags = pwrctrl->pcm_flags, pcm_flags1 = pwrctrl->pcm_flags1;

	/* Set PCM flags and data */
	if (pwrctrl->pcm_flags_cust_clr != 0)
		pcm_flags &= ~pwrctrl->pcm_flags_cust_clr;
	if (pwrctrl->pcm_flags_cust_set != 0)
		pcm_flags |= pwrctrl->pcm_flags_cust_set;
	if (pwrctrl->pcm_flags1_cust_clr != 0)
		pcm_flags1 &= ~pwrctrl->pcm_flags1_cust_clr;
	if (pwrctrl->pcm_flags1_cust_set != 0)
		pcm_flags1 |= pwrctrl->pcm_flags1_cust_set;

	write32(&mtk_spm->spm_sw_flag_0, pcm_flags);
	write32(&mtk_spm->spm_sw_flag_1, pcm_flags1);
	write32(&mtk_spm->spm_sw_rsv_7, pcm_flags);
	write32(&mtk_spm->spm_sw_rsv_8, pcm_flags1);
}

static void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl)
{
	/* Waiting for loading SPMFW done*/
	while (read32(&mtk_spm->md32pcm_dma0_rlct) != 0x0)
		;

	/* Init register to match PCM expectation */
	write32(&mtk_spm->spm_bus_protect_mask_b, SPM_BUS_PROTECT_MASK_B_DEF);
	write32(&mtk_spm->spm_bus_protect2_mask_b,
		SPM_BUS_PROTECT2_MASK_B_DEF);
	write32(&mtk_spm->pcm_reg_data_ini, 0);

	spm_set_pcm_flags(pwrctrl);

	/* Kick PCM to run (only toggle PCM_KICK) */
	setbits32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Reset md32pcm */
	SET32_BITFIELDS(&mtk_spm->md32pcm_cfgreg_sw_rstn,
			MD32PCM_CFGREG_SW_RSTN_RESET, 1);

	/* Waiting for SPM init done */
	udelay(SPM_INIT_DONE_US);
}

static void spm_parse_firmware(struct mtk_mcu *mcu)
{
	size_t file_size, copy_size;
	int offset;
	u16 firmware_size;

	struct dyna_load_pcm *pcm = (struct dyna_load_pcm *)mcu->priv;
	file_size = mcu->run_size;

	/*
	 * spmfw layout:
	 *   u16 firmware_size
	 *   u32 binary[firmware_size]
	 *   struct pcm_desc descriptor
	 *   char *version
	 */

	/* Firmware size */
	offset = 0;
	copy_size = sizeof(firmware_size);
	memcpy(&firmware_size, mcu->load_buffer + offset, copy_size);
	printk(BIOS_DEBUG, "SPM: binary array size = %#x\n", firmware_size);

	/* Binary */
	offset = SPMFW_HEADER_SIZE; /* binary start offset */
	copy_size = firmware_size * sizeof(u32);
	assert(offset < file_size);
	pcm->buf = (u8 *)(mcu->load_buffer + offset);

	/* Descriptor */
	offset += copy_size;
	assert(offset < file_size);
	copy_size = sizeof(pcm->desc);
	memcpy(&pcm->desc, mcu->load_buffer + offset, copy_size);

	/* Firmware size and total words need to be the same */
	assert(firmware_size == pcm->desc.total_words);

	/* Version */
	offset += copy_size;
	assert(offset < file_size);
	printk(BIOS_INFO, "SPM: spmfw (version %.*s)\n",
	       (int)(file_size - offset),
	       (u8 *)mcu->load_buffer + offset);
}

static void reset_spm(struct mtk_mcu *mcu)
{
	struct dyna_load_pcm *pcm = (struct dyna_load_pcm *)mcu->priv;
	const struct pwr_ctrl *spm_init_ctrl = get_pwr_ctrl();

	spm_parse_firmware(mcu);
	spm_reset_and_init_pcm();
	spm_kick_im_to_fetch(pcm);
	spm_init_pcm_register();
	spm_set_wakeup_event(spm_init_ctrl);
	spm_kick_pcm_to_run(spm_init_ctrl);
}

static struct mtk_mcu spm = {
	.firmware_name = CONFIG_SPM_FIRMWARE,
	.reset = reset_spm,
};

void spm_code_swapping(void)
{
	u32 mask;

	mask = read32(&mtk_spm->spm_wakeup_event_mask);
	write32(&mtk_spm->spm_wakeup_event_mask,
		mask & ~SPM_WAKEUP_EVENT_MASK_BIT0);
	write32(&mtk_spm->spm_cpu_wakeup_event, 1);
	write32(&mtk_spm->spm_cpu_wakeup_event, 0);
	write32(&mtk_spm->spm_wakeup_event_mask, mask);
}

int spm_init(void)
{
	struct dyna_load_pcm pcm;
	struct stopwatch sw;
	const struct pwr_ctrl *spm_init_ctrl = get_pwr_ctrl();

	stopwatch_init(&sw);

	spm_register_init();
	spm_set_power_control(spm_init_ctrl);
	spm_set_sysclk_settle();
	spm_extern_initialize();

	spm.load_buffer = _dram_dma;
	spm.buffer_size = REGION_SIZE(dram_dma);
	spm.priv = (void *)&pcm;

	if (mtk_init_mcu(&spm)) {
		printk(BIOS_ERR, "SPM: %s: failed in mtk_init_mcu\n", __func__);
		return -1;
	}

	printk(BIOS_INFO, "SPM: %s done in %ld msecs, spm pc = %#x\n",
	       __func__, stopwatch_duration_msecs(&sw),
	       read32(&mtk_spm->md32pcm_pc));

	return 0;
}
