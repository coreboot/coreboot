/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <string.h>
#include <soc/qupv3_config_common.h>
#include <console/console.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qcom_qup_se.h>
#include <soc/addressmap.h>

static struct elf_se_hdr *fw_list[SE_PROTOCOL_MAX];
static struct gsi_fw_hdr *gsi_hdr;

void qupv3_se_fw_load_and_init(unsigned int bus, unsigned int protocol,
							unsigned int mode)
{
	uint32_t i;
	uint32_t reg_value;
	const uint8_t *cfg_idx_arr;
	const uint32_t *cfg_val_arr;
	const uint32_t *fw_val_arr;
	struct elf_se_hdr *hdr;
	struct qup_regs *regs = qup[bus].regs;
	static const char * const filename[] = {
		[SE_PROTOCOL_SPI] = "fallback/spi_fw",
		[SE_PROTOCOL_UART] = "fallback/uart_fw",
		[SE_PROTOCOL_I2C] = "fallback/i2c_fw",
	};

	if (protocol >= SE_PROTOCOL_MAX || !filename[protocol])
		die("*ERROR*  * INVALID PROTOCOL ***\n");

	if (!fw_list[protocol]) {
		fw_list[protocol] = cbfs_map(filename[protocol], NULL);
		if (!fw_list[protocol])
			die("*ERROR*  * cbfs_map failed ***\n");
	}

	hdr = fw_list[protocol];
	assert(hdr->magic == SEFW_MAGIC_HEADER)

	cfg_idx_arr = (const uint8_t *)hdr + hdr->cfg_idx_offset;
	cfg_val_arr = (const uint32_t *)((uint8_t *)hdr + hdr->cfg_val_offset);
	fw_val_arr = (const uint32_t *)((uint8_t *)hdr + hdr->fw_offset);

	/* Unlock SE for FW loading */
	write32(&regs->se_geni_fw_multilock_protns, 0x0);
	write32(&regs->se_geni_fw_multilock_msa, 0x0);

	/* First, ensure GENI FW is disabled */
	write32(&regs->geni_output_ctrl, 0x0);
	clrbits_le32(&regs->geni_dfs_if_cfg, GENI_DFS_IF_CFG_DFS_IF_EN_BMSK);
	setbits_le32(&regs->geni_cgc_ctrl, GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK
			| GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK);
	write32(&regs->se_geni_clk_ctrl, 0x0);
	clrbits_le32(&regs->geni_cgc_ctrl, GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK
			| GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK);


	/* HPG section 3.1.7.1 */
	if (protocol != SE_PROTOCOL_UART) {
		setbits_le32(&regs->geni_dfs_if_cfg,
				GENI_DFS_IF_CFG_DFS_IF_EN_BMSK);
		/* configure clock dfsr */
		clock_configure_dfsr(bus);
	}

	/* HPG section 3.1.7.2 */
	/* No Init Required */

	/* HPG section 3.1.7.3 */
	write32(&regs->dma_general_cfg,
			DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK |
		    DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK |
		    DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK |
		    DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK);
	write32(&regs->geni_cgc_ctrl, DEFAULT_CGC_EN);

	/* HPG section 3.1.7.4 */
	write32(&regs->geni_init_cfg_revision, hdr->cfg_version);
	write32(&regs->geni_s_init_cfg_revision, hdr->cfg_version);

	assert(cfg_idx_arr[hdr->cfg_size_in_items - 1] * sizeof(uint32_t) <=
		MAX_OFFSET_CFG_REG);

	for (i = 0; i < hdr->cfg_size_in_items; i++)
		write32(&regs->geni_cfg_reg0 + cfg_idx_arr[i],
					cfg_val_arr[i]);

	/* HPG section 3.1.7.9 */
	/* non-UART configuration, UART driver can configure as desired for UART
	 */
	write32(&regs->geni_rx_rfr_watermark_reg, FIFO_DEPTH - 2);

	/* HPG section 3.1.7.5 */
	/* Don't change any SPI polarity, client driver will handle this */
	setbits_le32(&regs->geni_output_ctrl, DEFAULT_IO_OUTPUT_CTRL_MSK);

	/* HPG section 3.1.7.6 */
	reg_value = read32(&regs->geni_dma_mode_en);
	if (mode == GSI) {
		reg_value |= GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK;
		write32(&regs->geni_dma_mode_en, reg_value);
		write32(&regs->se_irq_en, 0x0);
		write32(&regs->se_gsi_event_en, SE_GSI_EVENT_EN_BMSK);
	} else if (mode == FIFO) {
		reg_value &= ~GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK;
		write32(&regs->geni_dma_mode_en, reg_value);
		write32(&regs->se_irq_en, SE_IRQ_EN_RMSK);
		write32(&regs->se_gsi_event_en, 0x0);
	} else if (mode == CPU_DMA) {
		reg_value |= GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK;
		write32(&regs->geni_dma_mode_en, reg_value);
		write32(&regs->se_irq_en, SE_IRQ_EN_RMSK);
		write32(&regs->se_gsi_event_en, 0x0);
	}

	/* HPG section 3.1.7.7 */
	write32(&regs->geni_m_irq_enable,
				M_COMMON_GENI_M_IRQ_EN);
	reg_value = S_CMD_OVERRUN_EN | S_ILLEGAL_CMD_EN |
				S_CMD_CANCEL_EN | S_CMD_ABORT_EN |
				S_GP_IRQ_0_EN | S_GP_IRQ_1_EN |
				S_GP_IRQ_2_EN | S_GP_IRQ_3_EN |
				S_RX_FIFO_WR_ERR_EN | S_RX_FIFO_RD_ERR_EN;
	write32(&regs->geni_s_irq_enable, reg_value);

	/* HPG section 3.1.7.8 */
	/* GPI/DMA mode */
	reg_value = DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK |
		    DMA_TX_IRQ_EN_SET_SBE_EN_SET_BMSK |
		    DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK;
	write32(&regs->dma_tx_irq_en_set, reg_value);

	reg_value = DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_BMSK |
		    DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK |
		    DMA_RX_IRQ_EN_SET_SBE_EN_SET_BMSK |
		    DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK;
	write32(&regs->dma_rx_irq_en_set, reg_value);

	/* HPG section 3.1.7.10 */
	reg_value = (hdr->serial_protocol << FW_REV_PROTOCOL_SHFT) |
		    (hdr->fw_version & 0xFF <<
				FW_REV_VERSION_SHFT);
	write32(&regs->se_geni_fw_revision, reg_value);

	reg_value =
	    (hdr->serial_protocol << FW_REV_PROTOCOL_SHFT) |
	    (hdr->fw_version & 0xFF <<
		FW_REV_VERSION_SHFT);
	write32(&regs->se_s_fw_revision, reg_value);

	assert(hdr->fw_size_in_items <= SIZE_GENI_FW_RAM);

	memcpy((&regs->se_geni_cfg_ramn), fw_val_arr,
	       hdr->fw_size_in_items * sizeof(uint32_t));

	/* HPG section 3.1.7.12 */
	write32(&regs->geni_force_default_reg, 0x1);
	setbits_le32(&regs->geni_cgc_ctrl, GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK
				|GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK);
	setbits_le32(&regs->se_geni_clk_ctrl, GENI_CLK_CTRL_SER_CLK_SEL_BMSK);
	clrbits_le32(&regs->geni_cgc_ctrl,
		(GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK |
		GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK));

	/* HPG section 3.1.7.13 */
	/* GSI/DMA mode */
	setbits_le32(&regs->se_dma_if_en, DMA_IF_EN_DMA_IF_EN_BMSK);

	/* HPG section 3.1.7.14 */
	reg_value = read32(&regs->se_fifo_if_disable);
	if ((mode == MIXED) || (mode == FIFO))
		reg_value &= ~FIFO_IF_DISABLE;
	else
		reg_value |= FIFO_IF_DISABLE;
	write32(&regs->se_fifo_if_disable, reg_value);
	write32(&regs->se_geni_clk_ctrl, 0x1);

	/* Lock SE from FW loading */
	write32(&regs->se_geni_fw_multilock_protns, 0x1);
	write32(&regs->se_geni_fw_multilock_msa, 0x1);
}

void gpi_firmware_load(int addr)
{
	uint32_t i;
	uint32_t regVal = 0;
	struct gsi_fw_iep *fwIep;
	struct gsi_fw_iram *fwIRam;
	struct gsi_regs *regs = (struct gsi_regs *)(uintptr_t)addr;
	static const char * const filename = "fallback/gsi_fw";

	/* Assign firmware header base */
	if (!gsi_hdr) {
		gsi_hdr = cbfs_map(filename, NULL);
		if (!gsi_hdr)
			die("*ERROR*  * cbfs_map() failed ***\n");
	}

	assert(gsi_hdr->magic == GSI_FW_MAGIC_HEADER)

	/* Assign IEP entry base */
	fwIep = (struct gsi_fw_iep *)(((uint8_t *)gsi_hdr) +
			gsi_hdr->iep_offset);
	/* Assign firmware IRAM entry base */
	fwIRam = (struct gsi_fw_iram *)(((uint8_t *)gsi_hdr) +
			gsi_hdr->fw_offset);

	clrbits_le32(&regs->gsi_cgc_ctrl, GSI_CGC_CTRL_REGION_2_HW_CGC_EN_BMSK);
	write32(&regs->gsi_periph_base_lsb, 0);
	write32(&regs->gsi_periph_base_msb, 0);

	/* Load IEP */
	for (i = 0; i < gsi_hdr->iep_size_in_items; i++) {
		/* Check if offset does not exceed GSI address space size */
		if (fwIep[i].offset < GSI_REG_BASE_SIZE)
			write32((void *)&regs->gsi_cfg + fwIep[i].offset,
							fwIep[i].value);
	}

	/* Load firmware in IRAM */
	assert((gsi_hdr->fw_size_in_items * 2) < (GSI_INST_RAM_n_MAX_n + 1))

	/* Program Firmware version */
	write32(&regs->gsi_manager_mcs_code_ver, fwIRam->iram_dword0);

	memcpy((&regs->gsi_inst_ramn), (void *)fwIRam,
		gsi_hdr->fw_size_in_items * GSI_FW_BYTES_PER_LINE);
	setbits_le32(&regs->gsi_mcs_cfg, GSI_MCS_CFG_MCS_ENABLE_BMSK);
	setbits_le32(&regs->gsi_cfg, GSI_CFG_DOUBLE_MCS_CLK_FREQ_BMSK
					| GSI_CFG_GSI_ENABLE_BMSK);

	write32(&regs->gsi_ee_n_scratch_0_addr, 0x0);
	write32(&regs->ee_n_gsi_ee_generic_cmd, 0x81);

	do {
		regVal = read32(&regs->gsi_ee_n_scratch_0_addr);
	} while (regVal > 1);
}

static void qup_common_init(int addr)
{
	struct qupv3_common_reg *qupv3_common;
	/* HPG section 3.1.2 */
	qupv3_common = (struct qupv3_common_reg *)(uintptr_t)addr;
	setbits32(&qupv3_common->qupv3_common_cfg_reg,
			QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_BMSK);

	/* HPG section 3.1.7.3 */
	setbits32(&qupv3_common->qupv3_se_ahb_m_cfg_reg,
			QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK);
}

void qupv3_fw_init(void)
{
	uint8_t i;

	/* Turn on all QUP clocks */
	for (i = 0; i < QUPV3_SE_MAX; i++)
		clock_enable_qup(i);

	qup_common_init(QUP_WRAP0_BASE);
	qup_common_init(QUP_WRAP1_BASE);
}
