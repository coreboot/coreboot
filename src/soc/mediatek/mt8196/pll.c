/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 14.1
 */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/pll.h>
#include <timer.h>

#define VLP_FM_WAIT_TIME_MS	40

#define FMIN		(1700UL * MHz)
#define FMAX		(3800UL * MHz)
#define FIN_RATE	(26 * MHz)
#define CON1_PCW_CHG	BIT(31)
#define POSTDIV_VAL_MAX	5
#define POSTDIV_MASK	0x7
#define POSTDIV_SHIFT	24
#define PCW_FBITS	14

#define FM_PLL_CK		0
#define FM_PLL_CKDIV_CK		1

enum mux_id {
	CLK_CK_AXI_SEL,
	CLK_CK_MEM_SUB_SEL,
	CLK_CK_IO_NOC_SEL,
	CLK_CK_P_AXI_SEL,
	CLK_CK_PEXTP0_AXI_SEL,
	CLK_CK_PEXTP1_USB_AXI_SEL,
	CLK_CK_P_FMEM_SUB_SEL,
	CLK_CK_PEXPT0_MEM_SUB_SEL,
	CLK_CK_PEXTP1_USB_MEM_SUB_SEL,
	CLK_CK_P_NOC_SEL,
	CLK_CK_EMI_N_SEL,
	CLK_CK_EMI_S_SEL,
	CLK_CK_EMI_SLICE_N_SEL,
	CLK_CK_EMI_SLICE_S_SEL,
	CLK_CK_AP2CONN_HOST_SEL,
	CLK_CK_ATB_SEL,
	CLK_CK_CIRQ_SEL,
	CLK_CK_PBUS_156M_SEL,
	CLK_CK_NOC_LOW_SEL,
	CLK_CK_NOC_MID_SEL,
	CLK_CK_EFUSE_SEL,
	CLK_CK_MCL3GIC_SEL,
	CLK_CK_MCINFRA_SEL,
	CLK_CK_DSP_SEL,
	CLK_CK_MFG_REF_SEL,
	CLK_CK_MFGSC_REF_SEL,
	CLK_CK_MFG_EB_SEL,
	CLK_CK_UART_SEL,
	CLK_CK_SPI0_BCLK_SEL,
	CLK_CK_SPI1_BCLK_SEL,
	CLK_CK_SPI2_BCLK_SEL,
	CLK_CK_SPI3_BCLK_SEL,
	CLK_CK_SPI4_BCLK_SEL,
	CLK_CK_SPI5_BCLK_SEL,
	CLK_CK_SPI6_BCLK_SEL,
	CLK_CK_SPI7_BCLK_SEL,
	CLK_CK_MSDC_MACRO_1P_SEL,
	CLK_CK_MSDC_MACRO_2P_SEL,
	CLK_CK_MSDC30_1_SEL,
	CLK_CK_MSDC30_2_SEL,
	CLK_CK_DISP_PWM_SEL,
	CLK_CK_USB_TOP_1P_SEL,
	CLK_CK_USB_XHCI_1P_SEL,
	CLK_CK_USB_FMCNT_P1_SEL,
	CLK_CK_I2C_P_SEL,
	CLK_CK_I2C_EAST_SEL,
	CLK_CK_I2C_WEST_SEL,
	CLK_CK_I2C_NORTH_SEL,
	CLK_CK_AES_UFSFDE_SEL,
	CLK_CK_SEL,
	CLK_CK_MBIST_SEL,
	CLK_CK_PEXTP_MBIST_SEL,
	CLK_CK_AUD_1_SEL,
	CLK_CK_AUD_2_SEL,
	CLK_CK_ADSP_SEL,
	CLK_CK_ADSP_UARTHUB_BCLK_SEL,
	CLK_CK_DPMAIF_MAIN_SEL,
	CLK_CK_PWM_SEL,
	CLK_CK_MCUPM_SEL,
	CLK_CK_SFLASH_SEL,
	CLK_CK_IPSEAST_SEL,
	CLK_CK_IPSWEST_SEL,
	CLK_CK_TL_SEL,
	CLK_CK_TL_P1_SEL,
	CLK_CK_TL_P2_SEL,
	CLK_CK_EMI_INTERFACE_546_SEL,
	CLK_CK_SDF_SEL,
	CLK_CK_UARTHUB_BCLK_SEL,
	CLK_CK_DPSW_CMP_26M_SEL,
	CLK_CK_SMAPCK_SEL,
	CLK_CK_SSR_PKA_SEL,
	CLK_CK_SSR_DMA_SEL,
	CLK_CK_SSR_KDF_SEL,
	CLK_CK_SSR_RNG_SEL,
	CLK_CK_SPU0_SEL,
	CLK_CK_SPU1_SEL,
	CLK_CK_DXCC_SEL,
	CLK_CK_SPU0_BOOT_SEL,
	CLK_CK_SPU1_BOOT_SEL,
	CLK_CK_SGMII0_REF_325M_SEL,
	CLK_CK_SGMII0_REG_SEL,
	CLK_CK_SGMII1_REF_325M_SEL,
	CLK_CK_SGMII1_REG_SEL,
	CLK_CK_GMAC_312P5M_SEL,
	CLK_CK_GMAC_125M_SEL,
	CLK_CK_GMAC_RMII_SEL,
	CLK_CK_GMAC_62P4M_PTP_SEL,
	CLK_CK_DUMMY1_SEL,
	CLK_CK_DUMMY2_SEL,
};

enum cksys2_mux_id {
	CLK_CK2_SENINF0_SEL,
	CLK_CK2_SENINF1_SEL,
	CLK_CK2_SENINF2_SEL,
	CLK_CK2_SENINF3_SEL,
	CLK_CK2_SENINF4_SEL,
	CLK_CK2_SENINF5_SEL,
	CLK_CK2_IMG1_SEL,
	CLK_CK2_IPE_SEL,
	CLK_CK2_CAM_SEL,
	CLK_CK2_CAMTM_SEL,
	CLK_CK2_DPE_SEL,
	CLK_CK2_VDEC_SEL,
	CLK_CK2_CCUSYS_SEL,
	CLK_CK2_CCUTM_SEL,
	CLK_CK2_VENC_SEL,
	CLK_CK2_DVO_SEL,
	CLK_CK2_DVO_FAVT_SEL,
	CLK_CK2_DP1_SEL,
	CLK_CK2_DP0_SEL,
	CLK_CK2_DISP_SEL,
	CLK_CK2_MDP_SEL,
	CLK_CK2_MMINFRA_SEL,
	CLK_CK2_MMINFRA_SNOC_SEL,
	CLK_CK2_MMUP_SEL,
	CLK_CK2_DUMMY1_SEL,
	CLK_CK2_DUMMY2_SEL,
	CLK_CK2_MMINFRA_AO_SEL,
};

enum vlp_mux_id {
	CLK_VLP_CK_SCP_SEL,
	CLK_VLP_CK_SCP_SPI_SEL,
	CLK_VLP_CK_SCP_IIC_SEL,
	CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL,
	CLK_VLP_CK_PWRAP_ULPOSC_SEL,
	CLK_VLP_CK_SPMI_M_TIA_32K_SEL,
	CLK_VLP_CK_APXGPT_26M_BCLK_SEL,
	CLK_VLP_CK_DPSW_SEL,
	CLK_VLP_CK_DPSW_CENTRAL_SEL,
	CLK_VLP_CK_SPMI_M_MST_SEL,
	CLK_VLP_CK_DVFSRC_SEL,
	CLK_VLP_CK_PWM_VLP_SEL,
	CLK_VLP_CK_AXI_VLP_SEL,
	CLK_VLP_CK_SYSTIMER_26M_SEL,
	CLK_VLP_CK_SSPM_SEL,
	CLK_VLP_CK_SRCK_SEL,
	CLK_VLP_CK_CAMTG0_SEL,
	CLK_VLP_CK_CAMTG1_SEL,
	CLK_VLP_CK_CAMTG2_SEL,
	CLK_VLP_CK_CAMTG3_SEL,
	CLK_VLP_CK_CAMTG4_SEL,
	CLK_VLP_CK_CAMTG5_SEL,
	CLK_VLP_CK_CAMTG6_SEL,
	CLK_VLP_CK_CAMTG7_SEL,
	CLK_VLP_CK_IPS_SEL,
	CLK_VLP_CK_SSPM_26M_SEL,
	CLK_VLP_CK_ULPOSC_SSPM_SEL,
	CLK_VLP_CK_VLP_PBUS_26M_SEL,
	CLK_VLP_CK_DEBUG_ERR_FLAG_SEL,
	CLK_VLP_CK_DPMSRDMA_SEL,
	CLK_VLP_CK_VLP_PBUS_156M_SEL,
	CLK_VLP_CK_SPM_SEL,
	CLK_VLP_CK_MMINFRA_VLP_SEL,
	CLK_VLP_CK_USB_TOP_SEL,
	CLK_VLP_CK_USB_XHCI_SEL,
	CLK_VLP_CK_NOC_VLP_SEL,
	CLK_VLP_CK_AUDIO_H_SEL,
	CLK_VLP_CK_AUD_ENGEN1_SEL,
	CLK_VLP_CK_AUD_ENGEN2_SEL,
	CLK_VLP_CK_AUD_INTBUS_SEL,
	CLK_VLP_CK_SPVLP_26M_SEL,
	CLK_VLP_CK_SPU0_VLP_SEL,
	CLK_VLP_CK_SPU1_VLP_SEL,
	CLK_VLP_CK_VLP_DUMMY1_SEL,
	CLK_VLP_CK_VLP_DUMMY2_SEL,
};

#define MUX_UPD(_id, _sys, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	[_id] = {				\
		.reg = &_sys->_clk_cfg.cfg,	\
		.set_reg = &_sys->_clk_cfg.set,	\
		.clr_reg = &_sys->_clk_cfg.clr,	\
		.mux_shift = _mux_shift,	\
		.mux_width = _mux_width,	\
		.upd_reg = &_sys->_upd_reg,	\
		.upd_shift = _upd_shift,	\
	}

#define CKSYS_MUX_UPD(_id, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	MUX_UPD(_id, mtk_topckgen, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)

#define CKSYS2_MUX_UPD(_id, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	MUX_UPD(_id, mtk_topckgen2, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)

#define VLP_MUX_UPD(_id, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	MUX_UPD(_id, mtk_vlpsys, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)

struct mux_sel {
	u8 id;
	u8 sel;
};

static const struct mux muxes[] = {
	CKSYS_MUX_UPD(CLK_CK_AXI_SEL, clk_cfg[0], 0, 3, clk_cfg_update[0], 0),
	CKSYS_MUX_UPD(CLK_CK_MEM_SUB_SEL, clk_cfg[0], 8, 4, clk_cfg_update[0], 1),
	CKSYS_MUX_UPD(CLK_CK_IO_NOC_SEL, clk_cfg[0], 16, 3, clk_cfg_update[0], 2),
	CKSYS_MUX_UPD(CLK_CK_P_AXI_SEL, clk_cfg[0], 24, 3, clk_cfg_update[0], 3),
	CKSYS_MUX_UPD(CLK_CK_PEXTP0_AXI_SEL, clk_cfg[1], 0, 3, clk_cfg_update[0], 4),
	CKSYS_MUX_UPD(CLK_CK_PEXTP1_USB_AXI_SEL, clk_cfg[1], 8, 3, clk_cfg_update[0], 5),
	CKSYS_MUX_UPD(CLK_CK_P_FMEM_SUB_SEL, clk_cfg[1], 16, 4, clk_cfg_update[0], 6),
	CKSYS_MUX_UPD(CLK_CK_PEXPT0_MEM_SUB_SEL, clk_cfg[1], 24, 4, clk_cfg_update[0], 7),
	CKSYS_MUX_UPD(CLK_CK_PEXTP1_USB_MEM_SUB_SEL, clk_cfg[2], 0, 4, clk_cfg_update[0], 8),
	CKSYS_MUX_UPD(CLK_CK_P_NOC_SEL, clk_cfg[2], 8, 4, clk_cfg_update[0], 9),
	CKSYS_MUX_UPD(CLK_CK_EMI_N_SEL, clk_cfg[2], 16, 3, clk_cfg_update[0], 10),
	CKSYS_MUX_UPD(CLK_CK_EMI_S_SEL, clk_cfg[2], 24, 3, clk_cfg_update[0], 11),
	CKSYS_MUX_UPD(CLK_CK_EMI_SLICE_N_SEL, clk_cfg[3], 0, 2, clk_cfg_update[0], 12),
	CKSYS_MUX_UPD(CLK_CK_EMI_SLICE_S_SEL, clk_cfg[3], 8, 2, clk_cfg_update[0], 13),
	CKSYS_MUX_UPD(CLK_CK_AP2CONN_HOST_SEL, clk_cfg[3], 16, 1, clk_cfg_update[0], 14),
	CKSYS_MUX_UPD(CLK_CK_ATB_SEL, clk_cfg[3], 24, 2, clk_cfg_update[0], 15),
	CKSYS_MUX_UPD(CLK_CK_CIRQ_SEL, clk_cfg[4], 0, 2, clk_cfg_update[0], 16),
	CKSYS_MUX_UPD(CLK_CK_PBUS_156M_SEL, clk_cfg[4], 8, 2, clk_cfg_update[0], 17),
	CKSYS_MUX_UPD(CLK_CK_NOC_LOW_SEL, clk_cfg[4], 16, 3, clk_cfg_update[0], 18),
	CKSYS_MUX_UPD(CLK_CK_NOC_MID_SEL, clk_cfg[4], 24, 4, clk_cfg_update[0], 19),
	CKSYS_MUX_UPD(CLK_CK_EFUSE_SEL, clk_cfg[5], 0, 1, clk_cfg_update[0], 20),
	CKSYS_MUX_UPD(CLK_CK_MCL3GIC_SEL, clk_cfg[5], 8, 2, clk_cfg_update[0], 21),
	CKSYS_MUX_UPD(CLK_CK_MCINFRA_SEL, clk_cfg[5], 16, 3, clk_cfg_update[0], 22),
	CKSYS_MUX_UPD(CLK_CK_DSP_SEL, clk_cfg[5], 24, 3, clk_cfg_update[0], 23),
	CKSYS_MUX_UPD(CLK_CK_MFG_REF_SEL, clk_cfg[6], 0, 1, clk_cfg_update[0], 24),
	CKSYS_MUX_UPD(CLK_CK_MFGSC_REF_SEL, clk_cfg[6], 8, 1, clk_cfg_update[0], 25),
	CKSYS_MUX_UPD(CLK_CK_MFG_EB_SEL, clk_cfg[6], 16, 2, clk_cfg_update[0], 26),
	CKSYS_MUX_UPD(CLK_CK_UART_SEL, clk_cfg[6], 24, 2, clk_cfg_update[0], 27),
	CKSYS_MUX_UPD(CLK_CK_SPI0_BCLK_SEL, clk_cfg[7], 0, 3, clk_cfg_update[0], 28),
	CKSYS_MUX_UPD(CLK_CK_SPI1_BCLK_SEL, clk_cfg[7], 8, 3, clk_cfg_update[0], 29),
	CKSYS_MUX_UPD(CLK_CK_SPI2_BCLK_SEL, clk_cfg[7], 16, 3, clk_cfg_update[0], 30),
	CKSYS_MUX_UPD(CLK_CK_SPI3_BCLK_SEL, clk_cfg[7], 24, 3, clk_cfg_update[1], 0),
	CKSYS_MUX_UPD(CLK_CK_SPI4_BCLK_SEL, clk_cfg[8], 0, 3, clk_cfg_update[1], 1),
	CKSYS_MUX_UPD(CLK_CK_SPI5_BCLK_SEL, clk_cfg[8], 8, 3, clk_cfg_update[1], 2),
	CKSYS_MUX_UPD(CLK_CK_SPI6_BCLK_SEL, clk_cfg[8], 16, 3, clk_cfg_update[1], 3),
	CKSYS_MUX_UPD(CLK_CK_SPI7_BCLK_SEL, clk_cfg[8], 24, 3, clk_cfg_update[1], 4),
	CKSYS_MUX_UPD(CLK_CK_MSDC_MACRO_1P_SEL, clk_cfg[9], 0, 3, clk_cfg_update[1], 5),
	CKSYS_MUX_UPD(CLK_CK_MSDC_MACRO_2P_SEL, clk_cfg[9], 8, 3, clk_cfg_update[1], 6),
	CKSYS_MUX_UPD(CLK_CK_MSDC30_1_SEL, clk_cfg[9], 16, 3, clk_cfg_update[1], 7),
	CKSYS_MUX_UPD(CLK_CK_MSDC30_2_SEL, clk_cfg[9], 24, 3, clk_cfg_update[1], 8),
	CKSYS_MUX_UPD(CLK_CK_DISP_PWM_SEL, clk_cfg[10], 0, 3, clk_cfg_update[1], 9),
	CKSYS_MUX_UPD(CLK_CK_USB_TOP_1P_SEL, clk_cfg[10], 8, 1, clk_cfg_update[1], 10),
	CKSYS_MUX_UPD(CLK_CK_USB_XHCI_1P_SEL, clk_cfg[10], 16, 1, clk_cfg_update[1], 11),
	CKSYS_MUX_UPD(CLK_CK_USB_FMCNT_P1_SEL, clk_cfg[10], 24, 1, clk_cfg_update[1], 12),
	CKSYS_MUX_UPD(CLK_CK_I2C_P_SEL, clk_cfg[11], 0, 3, clk_cfg_update[1], 13),
	CKSYS_MUX_UPD(CLK_CK_I2C_EAST_SEL, clk_cfg[11], 8, 3, clk_cfg_update[1], 14),
	CKSYS_MUX_UPD(CLK_CK_I2C_WEST_SEL, clk_cfg[11], 16, 3, clk_cfg_update[1], 15),
	CKSYS_MUX_UPD(CLK_CK_I2C_NORTH_SEL, clk_cfg[11], 24, 3, clk_cfg_update[1], 16),
	CKSYS_MUX_UPD(CLK_CK_AES_UFSFDE_SEL, clk_cfg[12], 0, 3, clk_cfg_update[1], 17),
	CKSYS_MUX_UPD(CLK_CK_SEL, clk_cfg[12], 8, 3, clk_cfg_update[1], 18),
	CKSYS_MUX_UPD(CLK_CK_MBIST_SEL, clk_cfg[12], 16, 2, clk_cfg_update[1], 19),
	CKSYS_MUX_UPD(CLK_CK_PEXTP_MBIST_SEL, clk_cfg[12], 24, 1, clk_cfg_update[1], 20),
	CKSYS_MUX_UPD(CLK_CK_AUD_1_SEL, clk_cfg[13], 0, 1, clk_cfg_update[1], 21),
	CKSYS_MUX_UPD(CLK_CK_AUD_2_SEL, clk_cfg[13], 8, 1, clk_cfg_update[1], 22),
	CKSYS_MUX_UPD(CLK_CK_ADSP_SEL, clk_cfg[13], 16, 1, clk_cfg_update[1], 23),
	CKSYS_MUX_UPD(CLK_CK_ADSP_UARTHUB_BCLK_SEL, clk_cfg[13], 24, 2, clk_cfg_update[1], 24),
	CKSYS_MUX_UPD(CLK_CK_DPMAIF_MAIN_SEL, clk_cfg[14], 0, 4, clk_cfg_update[1], 25),
	CKSYS_MUX_UPD(CLK_CK_PWM_SEL, clk_cfg[14], 8, 2, clk_cfg_update[1], 26),
	CKSYS_MUX_UPD(CLK_CK_MCUPM_SEL, clk_cfg[14], 16, 3, clk_cfg_update[1], 27),
	CKSYS_MUX_UPD(CLK_CK_SFLASH_SEL, clk_cfg[14], 24, 2, clk_cfg_update[1], 28),
	CKSYS_MUX_UPD(CLK_CK_IPSEAST_SEL, clk_cfg[15], 0, 3, clk_cfg_update[1], 29),
	CKSYS_MUX_UPD(CLK_CK_IPSWEST_SEL, clk_cfg[15], 8, 3, clk_cfg_update[1], 30),
	CKSYS_MUX_UPD(CLK_CK_TL_SEL, clk_cfg[15], 16, 2, clk_cfg_update[2], 0),
	CKSYS_MUX_UPD(CLK_CK_TL_P1_SEL, clk_cfg[15], 24, 2, clk_cfg_update[2], 1),
	CKSYS_MUX_UPD(CLK_CK_TL_P2_SEL, clk_cfg[16], 0, 2, clk_cfg_update[2], 2),
	CKSYS_MUX_UPD(CLK_CK_EMI_INTERFACE_546_SEL, clk_cfg[16], 8, 1, clk_cfg_update[2], 3),
	CKSYS_MUX_UPD(CLK_CK_SDF_SEL, clk_cfg[16], 16, 3, clk_cfg_update[2], 4),
	CKSYS_MUX_UPD(CLK_CK_UARTHUB_BCLK_SEL, clk_cfg[16], 24, 2, clk_cfg_update[2], 5),
	CKSYS_MUX_UPD(CLK_CK_DPSW_CMP_26M_SEL, clk_cfg[17], 0, 1, clk_cfg_update[2], 6),
	CKSYS_MUX_UPD(CLK_CK_SMAPCK_SEL, clk_cfg[17], 8, 1, clk_cfg_update[2], 7),
	CKSYS_MUX_UPD(CLK_CK_SSR_PKA_SEL, clk_cfg[17], 16, 3, clk_cfg_update[2], 8),
	CKSYS_MUX_UPD(CLK_CK_SSR_DMA_SEL, clk_cfg[17], 24, 3, clk_cfg_update[2], 9),
	CKSYS_MUX_UPD(CLK_CK_SSR_KDF_SEL, clk_cfg[18], 0, 2, clk_cfg_update[2], 10),
	CKSYS_MUX_UPD(CLK_CK_SSR_RNG_SEL, clk_cfg[18], 8, 2, clk_cfg_update[2], 11),
	CKSYS_MUX_UPD(CLK_CK_SPU0_SEL, clk_cfg[18], 16, 3, clk_cfg_update[2], 12),
	CKSYS_MUX_UPD(CLK_CK_SPU1_SEL, clk_cfg[18], 24, 3, clk_cfg_update[2], 13),
	CKSYS_MUX_UPD(CLK_CK_DXCC_SEL, clk_cfg[19], 0, 2, clk_cfg_update[2], 14),
	CKSYS_MUX_UPD(CLK_CK_SPU0_BOOT_SEL, clk_cfg[19], 8, 1, clk_cfg_update[2], 15),
	CKSYS_MUX_UPD(CLK_CK_SPU1_BOOT_SEL, clk_cfg[19], 16, 1, clk_cfg_update[2], 16),
	CKSYS_MUX_UPD(CLK_CK_SGMII0_REF_325M_SEL, clk_cfg[19], 24, 1, clk_cfg_update[2], 17),
	CKSYS_MUX_UPD(CLK_CK_SGMII0_REG_SEL, clk_cfg[20], 0, 1, clk_cfg_update[2], 18),
	CKSYS_MUX_UPD(CLK_CK_SGMII1_REF_325M_SEL, clk_cfg[20], 8, 1, clk_cfg_update[2], 19),
	CKSYS_MUX_UPD(CLK_CK_SGMII1_REG_SEL, clk_cfg[20], 16, 1, clk_cfg_update[2], 20),
	CKSYS_MUX_UPD(CLK_CK_GMAC_312P5M_SEL, clk_cfg[20], 24, 1, clk_cfg_update[2], 21),
	CKSYS_MUX_UPD(CLK_CK_GMAC_125M_SEL, clk_cfg[21], 0, 1, clk_cfg_update[2], 22),
	CKSYS_MUX_UPD(CLK_CK_GMAC_RMII_SEL, clk_cfg[21], 8, 1, clk_cfg_update[2], 23),
	CKSYS_MUX_UPD(CLK_CK_GMAC_62P4M_PTP_SEL, clk_cfg[21], 16, 2, clk_cfg_update[2], 24),
	CKSYS_MUX_UPD(CLK_CK_DUMMY1_SEL, clk_cfg[21], 24, 2, clk_cfg_update[2], 25),
	CKSYS_MUX_UPD(CLK_CK_DUMMY2_SEL, clk_cfg[22], 0, 2, clk_cfg_update[2], 26),
};

static const struct mux_sel mux_sels[] = {
	{ .id = CLK_CK_AXI_SEL, .sel = 5 },
	{ .id = CLK_CK_MEM_SUB_SEL, .sel = 10 },
	{ .id = CLK_CK_IO_NOC_SEL, .sel = 5 },
	{ .id = CLK_CK_P_AXI_SEL, .sel = 7 },
	{ .id = CLK_CK_PEXTP0_AXI_SEL, .sel = 7 },
	{ .id = CLK_CK_PEXTP1_USB_AXI_SEL, .sel = 7 },
	{ .id = CLK_CK_P_FMEM_SUB_SEL, .sel = 10 },
	{ .id = CLK_CK_PEXPT0_MEM_SUB_SEL, .sel = 10 },
	{ .id = CLK_CK_PEXTP1_USB_MEM_SUB_SEL, .sel = 10 },
	{ .id = CLK_CK_P_NOC_SEL, .sel = 11 },
	{ .id = CLK_CK_EMI_N_SEL, .sel = 5 },
	{ .id = CLK_CK_EMI_S_SEL, .sel = 5 },
	{ .id = CLK_CK_EMI_SLICE_N_SEL, .sel = 2 },
	{ .id = CLK_CK_EMI_SLICE_S_SEL, .sel = 2 },
	{ .id = CLK_CK_AP2CONN_HOST_SEL, .sel = 1 },
	{ .id = CLK_CK_ATB_SEL, .sel = 3 },
	{ .id = CLK_CK_CIRQ_SEL, .sel = 2 },
	{ .id = CLK_CK_PBUS_156M_SEL, .sel = 3 },
	{ .id = CLK_CK_NOC_LOW_SEL, .sel = 5 },
	{ .id = CLK_CK_NOC_MID_SEL, .sel = 7 },
	{ .id = CLK_CK_EFUSE_SEL, .sel = 0 },
	{ .id = CLK_CK_MCL3GIC_SEL, .sel = 3 },
	{ .id = CLK_CK_MCINFRA_SEL, .sel = 5 },
	{ .id = CLK_CK_DSP_SEL, .sel = 5 },
	{ .id = CLK_CK_MFG_REF_SEL, .sel = 0 },
	{ .id = CLK_CK_MFGSC_REF_SEL, .sel = 0 },
	{ .id = CLK_CK_MFG_EB_SEL, .sel = 3 },
	{ .id = CLK_CK_UART_SEL, .sel = 0 },
	{ .id = CLK_CK_SPI0_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI1_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI2_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI3_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI4_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI5_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI6_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_SPI7_BCLK_SEL, .sel = 7 },
	{ .id = CLK_CK_MSDC_MACRO_1P_SEL, .sel = 4 },
	{ .id = CLK_CK_MSDC_MACRO_2P_SEL, .sel = 4 },
	{ .id = CLK_CK_MSDC30_1_SEL, .sel = 4 },
	{ .id = CLK_CK_MSDC30_2_SEL, .sel = 4 },
	{ .id = CLK_CK_DISP_PWM_SEL, .sel = 5 },
	{ .id = CLK_CK_USB_TOP_1P_SEL, .sel = 1 },
	{ .id = CLK_CK_USB_XHCI_1P_SEL, .sel = 1 },
	{ .id = CLK_CK_USB_FMCNT_P1_SEL, .sel = 1 },
	{ .id = CLK_CK_I2C_P_SEL, .sel = 2 },
	{ .id = CLK_CK_I2C_EAST_SEL, .sel = 2 },
	{ .id = CLK_CK_I2C_WEST_SEL, .sel = 2 },
	{ .id = CLK_CK_I2C_NORTH_SEL, .sel = 2 },
	{ .id = CLK_CK_AES_UFSFDE_SEL, .sel = 5 },
	{ .id = CLK_CK_SEL, .sel = 6 },
	{ .id = CLK_CK_MBIST_SEL, .sel = 3 },
	{ .id = CLK_CK_PEXTP_MBIST_SEL, .sel = 1 },
	{ .id = CLK_CK_AUD_1_SEL, .sel = 1 },
	{ .id = CLK_CK_AUD_2_SEL, .sel = 1 },
	{ .id = CLK_CK_ADSP_SEL, .sel = 1 },
	{ .id = CLK_CK_ADSP_UARTHUB_BCLK_SEL, .sel = 0 },
	{ .id = CLK_CK_DPMAIF_MAIN_SEL, .sel = 3 },
	{ .id = CLK_CK_PWM_SEL, .sel = 0 },
	{ .id = CLK_CK_MCUPM_SEL, .sel = 4 },
	{ .id = CLK_CK_SFLASH_SEL, .sel = 2 },
	{ .id = CLK_CK_IPSEAST_SEL, .sel = 1 },
	{ .id = CLK_CK_IPSWEST_SEL, .sel = 1 },
	{ .id = CLK_CK_TL_SEL, .sel = 2 },
	{ .id = CLK_CK_TL_P1_SEL, .sel = 3 },
	{ .id = CLK_CK_TL_P2_SEL, .sel = 2 },
	{ .id = CLK_CK_EMI_INTERFACE_546_SEL, .sel = 1 },
	{ .id = CLK_CK_SDF_SEL, .sel = 5 },
	{ .id = CLK_CK_UARTHUB_BCLK_SEL, .sel = 0 },
	{ .id = CLK_CK_DPSW_CMP_26M_SEL, .sel = 1 },
	{ .id = CLK_CK_SMAPCK_SEL, .sel = 1 },
	{ .id = CLK_CK_SSR_PKA_SEL, .sel = 5 },
	{ .id = CLK_CK_SSR_DMA_SEL, .sel = 5 },
	{ .id = CLK_CK_SSR_KDF_SEL, .sel = 3 },
	{ .id = CLK_CK_SSR_RNG_SEL, .sel = 3 },
	{ .id = CLK_CK_SPU0_SEL, .sel = 1 },
	{ .id = CLK_CK_SPU1_SEL, .sel = 1 },
	{ .id = CLK_CK_DXCC_SEL, .sel = 0 },
	{ .id = CLK_CK_SPU0_BOOT_SEL, .sel = 1 },
	{ .id = CLK_CK_SPU1_BOOT_SEL, .sel = 1 },
	{ .id = CLK_CK_SGMII0_REF_325M_SEL, .sel = 1 },
	{ .id = CLK_CK_SGMII0_REG_SEL, .sel = 1 },
	{ .id = CLK_CK_SGMII1_REF_325M_SEL, .sel = 1 },
	{ .id = CLK_CK_SGMII1_REG_SEL, .sel = 1 },
	{ .id = CLK_CK_GMAC_312P5M_SEL, .sel = 1 },
	{ .id = CLK_CK_GMAC_125M_SEL, .sel = 1 },
	{ .id = CLK_CK_GMAC_RMII_SEL, .sel = 1 },
	{ .id = CLK_CK_GMAC_62P4M_PTP_SEL, .sel = 2 },
	{ .id = CLK_CK_DUMMY1_SEL, .sel = 3 },
	{ .id = CLK_CK_DUMMY2_SEL, .sel = 3 },
};

static const struct mux cksys2_muxes[] = {
	CKSYS2_MUX_UPD(CLK_CK2_SENINF0_SEL, cksys2_clk_cfg[0], 0, 4,
		       cksys2_clk_cfg_update, 0),
	CKSYS2_MUX_UPD(CLK_CK2_SENINF1_SEL, cksys2_clk_cfg[0], 8, 4,
		       cksys2_clk_cfg_update, 1),
	CKSYS2_MUX_UPD(CLK_CK2_SENINF2_SEL, cksys2_clk_cfg[0], 16, 4,
		       cksys2_clk_cfg_update, 2),
	CKSYS2_MUX_UPD(CLK_CK2_SENINF3_SEL, cksys2_clk_cfg[0], 24, 4,
		       cksys2_clk_cfg_update, 3),
	CKSYS2_MUX_UPD(CLK_CK2_SENINF4_SEL, cksys2_clk_cfg[1], 0, 4,
		       cksys2_clk_cfg_update, 4),
	CKSYS2_MUX_UPD(CLK_CK2_SENINF5_SEL, cksys2_clk_cfg[1], 8, 4,
		       cksys2_clk_cfg_update, 5),
	CKSYS2_MUX_UPD(CLK_CK2_IMG1_SEL, cksys2_clk_cfg[1], 16, 4,
		       cksys2_clk_cfg_update, 6),
	CKSYS2_MUX_UPD(CLK_CK2_IPE_SEL, cksys2_clk_cfg[1], 24, 4,
		       cksys2_clk_cfg_update, 7),
	CKSYS2_MUX_UPD(CLK_CK2_CAM_SEL, cksys2_clk_cfg[2], 0, 4,
		       cksys2_clk_cfg_update, 8),
	CKSYS2_MUX_UPD(CLK_CK2_CAMTM_SEL, cksys2_clk_cfg[2], 8, 3,
		       cksys2_clk_cfg_update, 9),
	CKSYS2_MUX_UPD(CLK_CK2_DPE_SEL, cksys2_clk_cfg[2], 16, 4,
		       cksys2_clk_cfg_update, 10),
	CKSYS2_MUX_UPD(CLK_CK2_VDEC_SEL, cksys2_clk_cfg[2], 24, 4,
		       cksys2_clk_cfg_update, 11),
	CKSYS2_MUX_UPD(CLK_CK2_CCUSYS_SEL, cksys2_clk_cfg[3], 0, 4,
		       cksys2_clk_cfg_update, 12),
	CKSYS2_MUX_UPD(CLK_CK2_CCUTM_SEL, cksys2_clk_cfg[3], 8, 3,
		       cksys2_clk_cfg_update, 13),
	CKSYS2_MUX_UPD(CLK_CK2_VENC_SEL, cksys2_clk_cfg[3], 16, 4,
		       cksys2_clk_cfg_update, 14),
	CKSYS2_MUX_UPD(CLK_CK2_DVO_SEL, cksys2_clk_cfg[3], 24, 3,
		       cksys2_clk_cfg_update, 15),
	CKSYS2_MUX_UPD(CLK_CK2_DVO_FAVT_SEL, cksys2_clk_cfg[4], 0, 3,
		       cksys2_clk_cfg_update, 16),
	CKSYS2_MUX_UPD(CLK_CK2_DP1_SEL, cksys2_clk_cfg[4], 8, 3,
		       cksys2_clk_cfg_update, 17),
	CKSYS2_MUX_UPD(CLK_CK2_DP0_SEL, cksys2_clk_cfg[4], 16, 3,
		       cksys2_clk_cfg_update, 18),
	CKSYS2_MUX_UPD(CLK_CK2_DISP_SEL, cksys2_clk_cfg[4], 24, 4,
		       cksys2_clk_cfg_update, 19),
	CKSYS2_MUX_UPD(CLK_CK2_MDP_SEL, cksys2_clk_cfg[5], 0, 4,
		       cksys2_clk_cfg_update, 20),
	CKSYS2_MUX_UPD(CLK_CK2_MMINFRA_SEL, cksys2_clk_cfg[5], 8, 4,
		       cksys2_clk_cfg_update, 21),
	CKSYS2_MUX_UPD(CLK_CK2_MMINFRA_SNOC_SEL, cksys2_clk_cfg[5], 16, 4,
		       cksys2_clk_cfg_update, 22),
	CKSYS2_MUX_UPD(CLK_CK2_MMUP_SEL, cksys2_clk_cfg[5], 24, 3,
		       cksys2_clk_cfg_update, 23),
	CKSYS2_MUX_UPD(CLK_CK2_DUMMY1_SEL, cksys2_clk_cfg[6], 0, 2,
		       cksys2_clk_cfg_update, 24),
	CKSYS2_MUX_UPD(CLK_CK2_DUMMY2_SEL, cksys2_clk_cfg[6], 8, 2,
		       cksys2_clk_cfg_update, 25),
	CKSYS2_MUX_UPD(CLK_CK2_MMINFRA_AO_SEL, cksys2_clk_cfg[6], 16, 2,
		       cksys2_clk_cfg_update, 26),
};

static const struct mux_sel cksys2_mux_sels[] = {
	{ .id = CLK_CK2_SENINF0_SEL, .sel = 9 },
	{ .id = CLK_CK2_SENINF1_SEL, .sel = 9 },
	{ .id = CLK_CK2_SENINF2_SEL, .sel = 9 },
	{ .id = CLK_CK2_SENINF3_SEL, .sel = 9 },
	{ .id = CLK_CK2_SENINF4_SEL, .sel = 9 },
	{ .id = CLK_CK2_SENINF5_SEL, .sel = 9 },
	{ .id = CLK_CK2_IMG1_SEL, .sel = 7 },
	{ .id = CLK_CK2_IPE_SEL, .sel = 10 },
	{ .id = CLK_CK2_CAM_SEL, .sel = 14 },
	{ .id = CLK_CK2_CAMTM_SEL, .sel = 4 },
	{ .id = CLK_CK2_DPE_SEL, .sel = 8 },
	{ .id = CLK_CK2_VDEC_SEL, .sel = 12 },
	{ .id = CLK_CK2_CCUSYS_SEL, .sel = 12 },
	{ .id = CLK_CK2_CCUTM_SEL, .sel = 4 },
	{ .id = CLK_CK2_VENC_SEL, .sel = 13 },
	{ .id = CLK_CK2_DVO_SEL, .sel = 4 },
	{ .id = CLK_CK2_DVO_FAVT_SEL, .sel = 6 },
	{ .id = CLK_CK2_DP1_SEL, .sel = 4 },
	{ .id = CLK_CK2_DP0_SEL, .sel = 4 },
	{ .id = CLK_CK2_DISP_SEL, .sel = 8 },
	{ .id = CLK_CK2_MDP_SEL, .sel = 12 },
	{ .id = CLK_CK2_MMINFRA_SEL, .sel = 13 },
	{ .id = CLK_CK2_MMINFRA_SNOC_SEL, .sel = 11 },
	{ .id = CLK_CK2_MMUP_SEL, .sel = 7 },
	{ .id = CLK_CK2_DUMMY1_SEL, .sel = 3 },
	{ .id = CLK_CK2_DUMMY2_SEL, .sel = 3 },
	{ .id = CLK_CK2_MMINFRA_AO_SEL, .sel = 2 },
};

static const struct mux vlp_muxes[] = {
	VLP_MUX_UPD(CLK_VLP_CK_SCP_SEL, vlp_clk_cfg[0], 0, 3,
		    vlp_clk_cfg_update[0], 0),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_SPI_SEL, vlp_clk_cfg[0], 8, 2,
		    vlp_clk_cfg_update[0], 1),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_IIC_SEL, vlp_clk_cfg[0], 16, 2,
		    vlp_clk_cfg_update[0], 2),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL, vlp_clk_cfg[0], 24, 3,
		    vlp_clk_cfg_update[0], 3),
	VLP_MUX_UPD(CLK_VLP_CK_PWRAP_ULPOSC_SEL, vlp_clk_cfg[1], 0, 2,
		    vlp_clk_cfg_update[0], 4),
	VLP_MUX_UPD(CLK_VLP_CK_SPMI_M_TIA_32K_SEL, vlp_clk_cfg[1], 8, 3,
		    vlp_clk_cfg_update[0], 5),
	VLP_MUX_UPD(CLK_VLP_CK_APXGPT_26M_BCLK_SEL, vlp_clk_cfg[1], 16, 1,
		    vlp_clk_cfg_update[0], 6),
	VLP_MUX_UPD(CLK_VLP_CK_DPSW_SEL, vlp_clk_cfg[1], 24, 2,
		    vlp_clk_cfg_update[0], 7),
	VLP_MUX_UPD(CLK_VLP_CK_DPSW_CENTRAL_SEL, vlp_clk_cfg[2], 0, 2,
		    vlp_clk_cfg_update[0], 8),
	VLP_MUX_UPD(CLK_VLP_CK_SPMI_M_MST_SEL, vlp_clk_cfg[2], 8, 2,
		    vlp_clk_cfg_update[0], 9),
	VLP_MUX_UPD(CLK_VLP_CK_DVFSRC_SEL, vlp_clk_cfg[2], 16, 1,
		    vlp_clk_cfg_update[0], 10),
	VLP_MUX_UPD(CLK_VLP_CK_PWM_VLP_SEL, vlp_clk_cfg[2], 24, 3,
		    vlp_clk_cfg_update[0], 11),
	VLP_MUX_UPD(CLK_VLP_CK_AXI_VLP_SEL, vlp_clk_cfg[3], 0, 3,
		    vlp_clk_cfg_update[0], 12),
	VLP_MUX_UPD(CLK_VLP_CK_SYSTIMER_26M_SEL, vlp_clk_cfg[3], 8, 1,
		    vlp_clk_cfg_update[0], 13),
	VLP_MUX_UPD(CLK_VLP_CK_SSPM_SEL, vlp_clk_cfg[3], 16, 3,
		    vlp_clk_cfg_update[0], 14),
	VLP_MUX_UPD(CLK_VLP_CK_SRCK_SEL, vlp_clk_cfg[3], 24, 1,
		    vlp_clk_cfg_update[0], 15),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG0_SEL, vlp_clk_cfg[4], 0, 4,
		    vlp_clk_cfg_update[0], 16),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG1_SEL, vlp_clk_cfg[4], 8, 4,
		    vlp_clk_cfg_update[0], 17),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG2_SEL, vlp_clk_cfg[4], 16, 4,
		    vlp_clk_cfg_update[0], 18),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG3_SEL, vlp_clk_cfg[4], 24, 4,
		    vlp_clk_cfg_update[0], 19),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG4_SEL, vlp_clk_cfg[5], 0, 4,
		    vlp_clk_cfg_update[0], 20),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG5_SEL, vlp_clk_cfg[5], 8, 4,
		    vlp_clk_cfg_update[0], 21),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG6_SEL, vlp_clk_cfg[5], 16, 4,
		    vlp_clk_cfg_update[0], 22),
	VLP_MUX_UPD(CLK_VLP_CK_CAMTG7_SEL, vlp_clk_cfg[5], 24, 4,
		    vlp_clk_cfg_update[0], 23),
	VLP_MUX_UPD(CLK_VLP_CK_IPS_SEL, vlp_clk_cfg[6], 0, 2,
		    vlp_clk_cfg_update[0], 24),
	VLP_MUX_UPD(CLK_VLP_CK_SSPM_26M_SEL, vlp_clk_cfg[6], 8, 1,
		    vlp_clk_cfg_update[0], 25),
	VLP_MUX_UPD(CLK_VLP_CK_ULPOSC_SSPM_SEL, vlp_clk_cfg[6], 16, 2,
		    vlp_clk_cfg_update[0], 26),
	VLP_MUX_UPD(CLK_VLP_CK_VLP_PBUS_26M_SEL, vlp_clk_cfg[6], 24, 1,
		    vlp_clk_cfg_update[0], 27),
	VLP_MUX_UPD(CLK_VLP_CK_DEBUG_ERR_FLAG_SEL, vlp_clk_cfg[7], 0, 1,
		    vlp_clk_cfg_update[0], 28),
	VLP_MUX_UPD(CLK_VLP_CK_DPMSRDMA_SEL, vlp_clk_cfg[7], 8, 1,
		    vlp_clk_cfg_update[0], 29),
	VLP_MUX_UPD(CLK_VLP_CK_VLP_PBUS_156M_SEL, vlp_clk_cfg[7], 16, 2,
		    vlp_clk_cfg_update[0], 30),
	VLP_MUX_UPD(CLK_VLP_CK_SPM_SEL, vlp_clk_cfg[7], 24, 1,
		    vlp_clk_cfg_update[1], 0),
	VLP_MUX_UPD(CLK_VLP_CK_MMINFRA_VLP_SEL, vlp_clk_cfg[8], 0, 2,
		    vlp_clk_cfg_update[1], 1),
	VLP_MUX_UPD(CLK_VLP_CK_USB_TOP_SEL, vlp_clk_cfg[8], 8, 1,
		    vlp_clk_cfg_update[1], 2),
	VLP_MUX_UPD(CLK_VLP_CK_USB_XHCI_SEL, vlp_clk_cfg[8], 16, 1,
		    vlp_clk_cfg_update[1], 3),
	VLP_MUX_UPD(CLK_VLP_CK_NOC_VLP_SEL, vlp_clk_cfg[8], 24, 2,
		    vlp_clk_cfg_update[1], 4),
	VLP_MUX_UPD(CLK_VLP_CK_AUDIO_H_SEL, vlp_clk_cfg[9], 0, 2,
		    vlp_clk_cfg_update[1], 5),
	VLP_MUX_UPD(CLK_VLP_CK_AUD_ENGEN1_SEL, vlp_clk_cfg[9], 8, 2,
		    vlp_clk_cfg_update[1], 6),
	VLP_MUX_UPD(CLK_VLP_CK_AUD_ENGEN2_SEL, vlp_clk_cfg[9], 16, 2,
		    vlp_clk_cfg_update[1], 7),
	VLP_MUX_UPD(CLK_VLP_CK_AUD_INTBUS_SEL, vlp_clk_cfg[9], 24, 2,
		    vlp_clk_cfg_update[1], 8),
	VLP_MUX_UPD(CLK_VLP_CK_SPVLP_26M_SEL, vlp_clk_cfg[10], 0, 1,
		    vlp_clk_cfg_update[1], 9),
	VLP_MUX_UPD(CLK_VLP_CK_SPU0_VLP_SEL, vlp_clk_cfg[10], 8, 3,
		    vlp_clk_cfg_update[1], 10),
	VLP_MUX_UPD(CLK_VLP_CK_SPU1_VLP_SEL, vlp_clk_cfg[10], 16, 3,
		    vlp_clk_cfg_update[1], 11),
	VLP_MUX_UPD(CLK_VLP_CK_VLP_DUMMY1_SEL, vlp_clk_cfg[10], 24, 3,
		    vlp_clk_cfg_update[1], 12),
	VLP_MUX_UPD(CLK_VLP_CK_VLP_DUMMY2_SEL, vlp_clk_cfg[11], 0, 3,
		    vlp_clk_cfg_update[1], 13),
};

static const struct mux_sel vlp_mux_sels[] = {
	{ .id = CLK_VLP_CK_SCP_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_SPI_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_IIC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_PWRAP_ULPOSC_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_SPMI_M_TIA_32K_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_APXGPT_26M_BCLK_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_DPSW_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_DPSW_CENTRAL_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_SPMI_M_MST_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_DVFSRC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_PWM_VLP_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_AXI_VLP_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_SYSTIMER_26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SSPM_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_SRCK_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_CAMTG0_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG1_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG2_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG3_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG4_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG5_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG6_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_CAMTG7_SEL, .sel = 7 },
	{ .id = CLK_VLP_CK_IPS_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_SSPM_26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_ULPOSC_SSPM_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_VLP_PBUS_26M_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_DEBUG_ERR_FLAG_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_DPMSRDMA_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_VLP_PBUS_156M_SEL, .sel = 3 },
	{ .id = CLK_VLP_CK_SPM_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_MMINFRA_VLP_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_USB_TOP_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_USB_XHCI_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_NOC_VLP_SEL, .sel = 2 },
	{ .id = CLK_VLP_CK_AUDIO_H_SEL, .sel = 3 },
	{ .id = CLK_VLP_CK_AUD_ENGEN1_SEL, .sel = 3 },
	{ .id = CLK_VLP_CK_AUD_ENGEN2_SEL, .sel = 3 },
	{ .id = CLK_VLP_CK_AUD_INTBUS_SEL, .sel = 3 },
	{ .id = CLK_VLP_CK_SPVLP_26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SPU0_VLP_SEL, .sel = 6 },
	{ .id = CLK_VLP_CK_SPU1_VLP_SEL, .sel = 6 },
	{ .id = CLK_VLP_CK_VLP_DUMMY1_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_VLP_DUMMY2_SEL, .sel = 4 },
};

enum pll_id {
	CLK_APMIXED_MAINPLL,
	CLK_APMIXED_UNIVPLL,
	CLK_APMIXED_MSDCPLL,
	CLK_APMIXED_ADSPPLL,
	CLK_APMIXED2_MAINPLL2,
	CLK_APMIXED2_UNIVPLL2,
	CLK_APMIXED2_MMPLL2,
	CLK_APMIXED2_IMGPLL,
	CLK_APMIXED2_TVDPLL1,
	CLK_APMIXED2_TVDPLL2,
	CLK_APMIXED2_TVDPLL3,
	CLK_VLP_APLL1,
	CLK_VLP_APLL2,
};

enum mcusys_pll_id {
	CLK_CPLL_ARMPLL_LL,
	CLK_CPBL_ARMPLL_BL,
	CLK_CPB_ARMPLL_B,
	CLK_CCIPLL,
	CLK_PTPPLL,
};

enum mfg_pll_id {
	CLK_MFG_AO_MFGPLL,
	CLK_MFGSC0_AO_MFGPLL_SC0,
	CLK_MFGSC1_AO_MFGPLL_SC1,
};

struct rate {
	u8 id;
	u32 rate;
};

static const struct rate pll_rates[] = {
	{ .id = CLK_APMIXED_MAINPLL, .rate = MAINPLL_HZ },
	{ .id = CLK_APMIXED_UNIVPLL, .rate = UNIVPLL_HZ },
	{ .id = CLK_APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = CLK_APMIXED_ADSPPLL, .rate = ADSPPLL_HZ },
	{ .id = CLK_APMIXED2_MAINPLL2, .rate = MAINPLL2_HZ },
	{ .id = CLK_APMIXED2_UNIVPLL2, .rate = UNIVPLL2_HZ },
	{ .id = CLK_APMIXED2_MMPLL2, .rate = MMPLL2_HZ },
	{ .id = CLK_APMIXED2_IMGPLL, .rate = IMGPLL_HZ },
	{ .id = CLK_APMIXED2_TVDPLL1, .rate = TVDPLL1_HZ },
	{ .id = CLK_APMIXED2_TVDPLL2, .rate = TVDPLL2_HZ },
	{ .id = CLK_APMIXED2_TVDPLL3, .rate = TVDPLL3_HZ },
	{ .id = CLK_VLP_APLL1, .rate = VLP_APLL1_HZ },
	{ .id = CLK_VLP_APLL2, .rate = VLP_APLL2_HZ },
};

static const struct rate mcusys_pll_rates[] = {
	{ .id = CLK_CPLL_ARMPLL_LL, .rate = ARMPLL_LL_HZ },
	{ .id = CLK_CPBL_ARMPLL_BL, .rate = ARMPLL_BL_HZ },
	{ .id = CLK_CPB_ARMPLL_B, .rate = ARMPLL_B_HZ },
	{ .id = CLK_CCIPLL, .rate = CCIPLL_HZ },
	{ .id = CLK_PTPPLL, .rate = PTPPLL_HZ },
};

static const struct rate mfg_pll_rates[] = {
	{ .id = CLK_MFG_AO_MFGPLL, .rate = MFGPLL_HZ },
	{ .id = CLK_MFGSC0_AO_MFGPLL_SC0, .rate = MFGPLL_SC0_HZ },
	{ .id = CLK_MFGSC1_AO_MFGPLL_SC1, .rate = MFGPLL_SC1_HZ },
};

static const u32 pll_div_rate[] = {
	3800UL * MHz,
	1900 * MHz,
	950 * MHz,
	475 * MHz,
	237500 * KHz,
	0,
};

#define PLL_SYS(_id, _sys, _reg, _rstb, _pcwbits, _div_reg, _div_shift,	\
	    _pcw_reg, _pcw_shift, _div_rate)				\
	[_id] = {							\
		.reg = &_sys->_reg,					\
		.rstb_shift = _rstb,					\
		.pcwbits = _pcwbits,					\
		.div_reg = &_sys->_div_reg,				\
		.div_shift = _div_shift,				\
		.pcw_reg = &_sys->_pcw_reg,				\
		.pcw_shift = _pcw_shift,				\
		.div_rate = _div_rate,					\
	}

static const struct pll plls[] = {
	PLL_SYS(CLK_APMIXED_MAINPLL, mtk_apmixed, mainpll_con[0], 23, 22,
		mainpll_con[1], 24, mainpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_UNIVPLL, mtk_apmixed, univpll_con[0], 23, 22,
		univpll_con[1], 24, univpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_MSDCPLL, mtk_apmixed, msdcpll_con[0], NO_RSTB_SHIFT, 22,
		msdcpll_con[1], 24, msdcpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_ADSPPLL, mtk_apmixed, adsppll_con[0], NO_RSTB_SHIFT, 22,
		adsppll_con[1], 24, adsppll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_MAINPLL2, mtk_apmixed2, mainpll2_con[0], 23, 22,
		mainpll2_con[1], 24, mainpll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_UNIVPLL2, mtk_apmixed2, univpll2_con[0], 23, 22,
		univpll2_con[1], 24, univpll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_MMPLL2, mtk_apmixed2, mmpll2_con[0], 23, 22,
		mmpll2_con[1], 24, mmpll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_IMGPLL, mtk_apmixed2, imgpll_con[0], 23, 22,
		imgpll_con[1], 24, imgpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_TVDPLL1, mtk_apmixed2, tvdpll1_con[0], NO_RSTB_SHIFT, 22,
		tvdpll1_con[1], 24, tvdpll1_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_TVDPLL2, mtk_apmixed2, tvdpll2_con[0], NO_RSTB_SHIFT, 22,
		tvdpll2_con[1], 24, tvdpll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED2_TVDPLL3, mtk_apmixed2, tvdpll3_con[0], NO_RSTB_SHIFT, 22,
		tvdpll3_con[1], 24, tvdpll3_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_VLP_APLL1, mtk_vlpsys, vlp_apll1_con[0], NO_RSTB_SHIFT, 32,
		vlp_apll1_con[1], 24, vlp_apll1_con[2], 0, pll_div_rate),
	PLL_SYS(CLK_VLP_APLL2, mtk_vlpsys, vlp_apll2_con[0], NO_RSTB_SHIFT, 32,
		vlp_apll2_con[1], 24, vlp_apll2_con[2], 0, pll_div_rate),
};

static const struct pll mcusys_plls[] = {
	PLL_SYS(CLK_CPLL_ARMPLL_LL, mtk_armpll_ll, pll_con[0], NO_RSTB_SHIFT, 22,
		pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_CPBL_ARMPLL_BL, mtk_armpll_bl, pll_con[0], NO_RSTB_SHIFT, 22,
		pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_CPB_ARMPLL_B, mtk_armpll_b, pll_con[0], NO_RSTB_SHIFT, 22,
		pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_CCIPLL, mtk_ccipll, pll_con[0], NO_RSTB_SHIFT, 22,
		pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_PTPPLL, mtk_ptppll, pll_con[0], NO_RSTB_SHIFT, 22,
		pll_con[1], 24, pll_con[1], 0, pll_div_rate),
};

static const struct pll mfg_plls[] = {
	PLL_SYS(CLK_MFG_AO_MFGPLL, mtk_mfgpll, pll_con[3],
		NO_RSTB_SHIFT, 22, pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_MFGSC0_AO_MFGPLL_SC0, mtk_mfgpll_sc0, pll_con[3],
		NO_RSTB_SHIFT, 22, pll_con[1], 24, pll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_MFGSC1_AO_MFGPLL_SC1, mtk_mfgpll_sc1, pll_con[3],
		NO_RSTB_SHIFT, 22, pll_con[1], 24, pll_con[1], 0, pll_div_rate),
};

enum fmeter_id {
	VLP_CKSYS_CTRL,
	MFGPLL_CTRL,
	MFGPLL_SC0_CTRL,
	MFGPLL_SC1_CTRL,
	CCIPLL_CTRL,
	ARMPLL_LL_CTRL,
	ARMPLL_BL_CTRL,
	ARMPLL_B_CTRL,
	PTPPLL_CTRL,
};

struct fmeter_data {
	u8 id;
	void *pll_con0;
	void *pll_con1;
	void *pll_con5;
	void *con0;
	void *con1;
};

static const struct fmeter_data fmd[] = {
	[VLP_CKSYS_CTRL] = {
		VLP_CKSYS_CTRL, NULL, NULL, NULL,
		&mtk_vlpsys->vlp_fqmtr_con[0], &mtk_vlpsys->vlp_fqmtr_con[1] },
	[MFGPLL_CTRL] = {
		MFGPLL_CTRL,
		&mtk_mfgpll->pll_con[0], &mtk_mfgpll->pll_con[1],
		&mtk_mfgpll->pll_con[5], &mtk_mfgpll->fqmtr_con[0],
		&mtk_mfgpll->fqmtr_con[1] },
	[MFGPLL_SC0_CTRL] = {
		MFGPLL_SC0_CTRL,
		&mtk_mfgpll_sc0->pll_con[0], &mtk_mfgpll_sc0->pll_con[1],
		&mtk_mfgpll_sc0->pll_con[5], &mtk_mfgpll_sc0->fqmtr_con[0],
		&mtk_mfgpll_sc0->fqmtr_con[1] },
	[MFGPLL_SC1_CTRL] = {
		MFGPLL_SC1_CTRL,
		&mtk_mfgpll_sc1->pll_con[0], &mtk_mfgpll_sc1->pll_con[1],
		&mtk_mfgpll_sc1->pll_con[5], &mtk_mfgpll_sc1->fqmtr_con[0],
		&mtk_mfgpll_sc1->fqmtr_con[1] },
	[CCIPLL_CTRL] = {
		CCIPLL_CTRL,
		&mtk_ccipll->pll_con[0], &mtk_ccipll->pll_con[1], 0,
		&mtk_ccipll->fqmtr_con[0], &mtk_ccipll->fqmtr_con[1] },
	[ARMPLL_LL_CTRL] = {
		ARMPLL_LL_CTRL,
		&mtk_armpll_ll->pll_con[0], &mtk_armpll_ll->pll_con[1], 0,
		&mtk_armpll_ll->fqmtr_con[0], &mtk_armpll_ll->fqmtr_con[1] },
	[ARMPLL_BL_CTRL] = {
		ARMPLL_BL_CTRL,
		&mtk_armpll_bl->pll_con[0], &mtk_armpll_bl->pll_con[1], 0,
		&mtk_armpll_bl->fqmtr_con[0], &mtk_armpll_bl->fqmtr_con[1] },
	[ARMPLL_B_CTRL] = {
		ARMPLL_B_CTRL,
		&mtk_armpll_b->pll_con[0], &mtk_armpll_b->pll_con[1], 0,
		&mtk_armpll_b->fqmtr_con[0], &mtk_armpll_b->fqmtr_con[1] },
	[PTPPLL_CTRL] = {
		PTPPLL_CTRL,
		&mtk_ptppll->pll_con[0], &mtk_ptppll->pll_con[1], 0,
		&mtk_ptppll->fqmtr_con[0], &mtk_ptppll->fqmtr_con[1] },
};

#define PM_DUMMY	63

#define VOTE_OFS(ofs)	((void *)(uintptr_t)(VOTE_BASE + ofs))

#define VOTE_CTRL			VOTE_OFS(0x150C)
#define VOTE_DCM			VOTE_OFS(0x1510)
#define VOTE_REG_EN0			VOTE_OFS(0x1514)
#define VOTE_REG_EN1			VOTE_OFS(0x1518)
#define VOTE_REG_EN6			VOTE_OFS(0x152C)
#define VOTE_APB_M_REG0			VOTE_OFS(0x1530)
#define VOTE_APB_M_REG5			VOTE_OFS(0x1544)
#define VOTE_TIMEOUT0			VOTE_OFS(0x154C)
#define VOTE_TIMEOUT1			VOTE_OFS(0x1550)
#define VOTE_REG_KEY			VOTE_OFS(0x1560)
#define VOTE_MTCMOS_MODE_SEL_0		VOTE_OFS(0x1570)
#define VOTE_MTCMOS_MODE_SEL_1		VOTE_OFS(0x1574)
#define VOTE_MUX_MAPPING_LINK_EN_0	VOTE_OFS(0x1578)
#define VOTE_MUX_MAPPING_LINK_EN_1	VOTE_OFS(0x157C)
#define VOTE_MUX_MODE_SEL_0		VOTE_OFS(0x1580)
#define VOTE_MUX_MODE_SEL_1		VOTE_OFS(0x1584)
#define VOTE_MUX_MAPPING_LINK_EN_2	VOTE_OFS(0x1588)
#define VOTE_MUX_MODE_SEL_2		VOTE_OFS(0x158C)
#define VOTE_MTCMOS_DIS_MUX_0		VOTE_OFS(0x1590)
#define VOTE_MTCMOS_DIS_MUX_1		VOTE_OFS(0x1594)

#define VOTE_LOCK_REG_CFG	0x80000000

#define VOTE_CG_MAP_MTCMOS(x)	VOTE_OFS(0x15A0 + (x) * 0x4)
#define VOTE_CG_SET_ADDR(x)	VOTE_OFS(0x1600 + (x) * 0x8)
#define VOTE_CG_CLR_ADDR(x)	VOTE_OFS(0x1604 + (x) * 0x8)

#define VOTE_CG_MTCMOS_DUMMY_RANGE_START1	0
#define VOTE_CG_MTCMOS_DUMMY_RANGE_END1		14
#define VOTE_CG_MTCMOS_DUMMY_RANGE_START2	30
#define VOTE_CG_MTCMOS_DUMMY_RANGE_END2		49

#define VOTE_MAP(_id, _base, _set_val, _clr_val)\
	[_id] = {			\
		.map_base = _base,	\
		.set_addr = _set_val,	\
		.clr_addr = _clr_val,	\
	}

struct range {
	u32 start;
	u32 end;
};

struct vote_map {
	u32 map_base;
	u16 set_addr;
	u16 clr_addr;
};

struct cg_mtcmos_map {
	u8 cg_idx;
	u8 mtcmos_idx;
};

static const struct vote_map vote_maps[] = {
	VOTE_MAP(0, IMP_IIC_WRAP_N_BASE, 0xE04, 0xE08),
	VOTE_MAP(1, PERICFG_AO_BASE, 0x30, 0x2C),
	VOTE_MAP(2, SSR_TOP_BASE, 0x0, 0x0),
	VOTE_MAP(3, CKSYS_BASE, 0x0078, 0x0074),
	VOTE_MAP(4, CKSYS_BASE, 0x0088, 0x0084),
	VOTE_MAP(5, CKSYS_BASE, 0x0098, 0x0094),
	VOTE_MAP(6, CKSYS_BASE, 0x00C8, 0x00C4),
	VOTE_MAP(7, CKSYS_BASE, 0x0118, 0x0114),
	VOTE_MAP(8, CKSYS_BASE, 0x0128, 0x0124),
	VOTE_MAP(9, VLP_CKSYS_BASE, 0x0058, 0x0054),
	VOTE_MAP(10, VLP_CKSYS_BASE, 0x0068, 0x0064),
};

static const struct range vote_cg_mtcmos_dummy_range[] = {
	{ VOTE_CG_MTCMOS_DUMMY_RANGE_START1, VOTE_CG_MTCMOS_DUMMY_RANGE_END1 },
	{ VOTE_CG_MTCMOS_DUMMY_RANGE_START2, VOTE_CG_MTCMOS_DUMMY_RANGE_END2 },
};

enum {
	PM_ISP_TRAW,
	PM_ISP_DIP,
	PM_ISP_MAIN,
	PM_ISP_VCORE,
	PM_ISP_WPE_EIS,
	PM_ISP_WPE_TNR,
	PM_ISP_WPE_LITE,
	PM_VDE0,
	PM_VDE1,
	PM_VDE_VCORE0,
	PM_VEN0,
	PM_VEN1,
	PM_VEN2,
	PM_CAM_MRAW,
	PM_CAM_RAWA,
	PM_CAM_RAWB,
	PM_CAM_RAWC,
	PM_CAM_RMSA,
	PM_CAM_RMSB,
	PM_CAM_RMSC,
	PM_CAM_MAIN,
	PM_CAM_VCORE,
	PM_CAM_CCU,
	PM_CAM_CCU_AO,
	PM_DISP_VCORE,
	PM_DIS0,
	PM_DIS1,
	PM_OVL0,
	PM_OVL1,
	PM_DISP_EDPTX,
	PM_DISP_DPTX,
	PM_MML0,
	PM_MML1,
	PM_MMINFRA0,
	PM_MMINFRA1,
	PM_MMINFRA_AO,
	PM_DP_TX,
	PM_CSI_BS_RX,
	PM_CSI_LS_RX,
	PM_DSI_PHY0,
	PM_DSI_PHY1,
	PM_DSI_PHY2,
	PM_DUMMY1 = 63,
	PM_DUMMY2,
};

enum {
	CLK_SENINF0_SEL,
	CLK_SENINF1_SEL,
	CLK_SENINF2_SEL,
	CLK_SENINF3_SEL,
	CLK_SENINF4_SEL,
	CLK_SENINF5_SEL,
	CLK_IMG1_SEL,
	CLK_IPE_SEL,
	CLK_CAM_SEL,
	CLK_CAMTM_SEL,
	CLK_DPE_SEL,
	CLK_VDEC_SEL,
	CLK_CCUSYS_SEL,
	CLK_CCUTM_SEL,
	CLK_VENC_SEL,
	CLK_DVO_SEL,
	CLK_DVO_FAVT_SEL,
	CLK_DP1_SEL,
	CLK_DP0_SEL,
	CLK_DISP_SEL,
	CLK_MDP_SEL,
	CLK_MMINFRA_SEL,
	CLK_MMINFRA_SNOC_SEL,
	CLK_MMUP_SEL,
	CLK_MMINFRA_AO_SEL,
};

#define MMVOTE_OFS(ofs)	((void *)(uintptr_t)(MMVOTE_BASE + ofs))

#define MMVOTE_MTCMOS_MODE_SEL_0	MMVOTE_OFS(0x1570)
#define MMVOTE_MTCMOS_DIS_MUX_0		MMVOTE_OFS(0x1590)
#define MMVOTE_MTCMOS_DIS_MUX_1		MMVOTE_OFS(0x1594)

#define MMVOTE_PRE_CLK_MUX(x)		MMVOTE_OFS(0x4180 + (x) * 0x4)
#define MMVOTE_MTCMOS_MAP_MTCMOS(x)	MMVOTE_OFS(0x5484 + (x) * 0x4)
#define MMVOTE_MTCMOS_MAP_MUX(t, x)	MMVOTE_OFS(0x5108 + (t) * 0xc + (x) * 0x4)

#define MMVOTE_CTRL		MMVOTE_OFS(0x150C)
#define MMVOTE_REG_EN0		MMVOTE_OFS(0x1514)
#define MMVOTE_REG_EN1		MMVOTE_OFS(0x1518)
#define MMVOTE_REG_EN6		MMVOTE_OFS(0x152C)
#define MMVOTE_APB_M_REG0	MMVOTE_OFS(0x1530)
#define MMVOTE_APB_M_REG5	MMVOTE_OFS(0x1544)
#define MMVOTE_TIMEOUT0		MMVOTE_OFS(0x154C)
#define MMVOTE_TIMEOUT1		MMVOTE_OFS(0x1550)
#define MMVOTE_REG_KEY		MMVOTE_OFS(0x1560)

#define MMVOTE_UNLOCK_REG_CFG	0x10907

#define MMVOTE_CG_MAP_MTCMOS(x)	MMVOTE_OFS(0x15A0 + (x) * 0x4)
#define MMVOTE_CG_SET_ADDR(x)	MMVOTE_OFS(0x1600 + (x) * 0x8)
#define MMVOTE_CG_CLR_ADDR(x)	MMVOTE_OFS(0x1604 + (x) * 0x8)

#define MMVOTE_CG_MTCMOS_MAP_COUNT	50

#define MMVOTE_CG_MTCMOS_DUMMY_RANGE_START1	0
#define MMVOTE_CG_MTCMOS_DUMMY_RANGE_END1	28
#define MMVOTE_CG_MTCMOS_DUMMY_RANGE_START2	30
#define MMVOTE_CG_MTCMOS_DUMMY_RANGE_END2	49

static const struct vote_map mmvote_maps[] = {
	VOTE_MAP(0, MMVOTE_CAM_MAIN_R1A_BASE, 0x8, 0x4),
	VOTE_MAP(1, MMVOTE_CAM_MAIN_R1A_BASE, 0xC0, 0xC0),
	VOTE_MAP(2, MMVOTE_MMSYS1_CONFIG_BASE, 0x108, 0x104),
	VOTE_MAP(3, MMVOTE_MMSYS1_CONFIG_BASE, 0x118, 0x114),
	VOTE_MAP(4, MMVOTE_MMSYS_CONFIG_BASE, 0x108, 0x104),
	VOTE_MAP(5, MMVOTE_MMSYS_CONFIG_BASE, 0x118, 0x114),
	VOTE_MAP(6, MMVOTE_DISP_VDISP_AO_CONFIG_BASE, 0x108, 0x104),
	VOTE_MAP(7, MMVOTE_IMGSYS_MAIN_BASE, 0x58, 0x54),
	VOTE_MAP(8, MMVOTE_IMGSYS_MAIN_BASE, 0x8, 0x4),
	VOTE_MAP(9, MMVOTE_IMG_VCORE_D1A_BASE, 0x8, 0x4),
	VOTE_MAP(10, MMVOTE_OVLSYS1_CONFIG_BASE, 0x108, 0x104),
	VOTE_MAP(11, MMVOTE_OVLSYS1_CONFIG_BASE, 0x118, 0x114),
	VOTE_MAP(12, MMVOTE_OVLSYS_CONFIG_BASE, 0x108, 0x104),
	VOTE_MAP(13, MMVOTE_OVLSYS_CONFIG_BASE, 0x118, 0x114),
	VOTE_MAP(14, MMVOTE_CCU_MAIN_BASE, 0x8, 0x4),
	VOTE_MAP(15, MMVOTE_VDEC_GCON_BASE, 0x8, 0xC),
	VOTE_MAP(16, MMVOTE_VDEC_GCON_BASE, 0x200, 0x204),
	VOTE_MAP(17, MMVOTE_VDEC_GCON_BASE, 0x0, 0x4),
	VOTE_MAP(18, MMVOTE_VDEC_SOC_GCON_BASE, 0x8, 0xC),
	VOTE_MAP(19, MMVOTE_VDEC_SOC_GCON_BASE, 0x200, 0x204),
	VOTE_MAP(20, MMVOTE_VDEC_SOC_GCON_BASE, 0x0, 0x4),
	VOTE_MAP(21, MMVOTE_VDEC_SOC_GCON_BASE, 0x1EC, 0x1EC),
	VOTE_MAP(22, MMVOTE_VDEC_SOC_GCON_BASE, 0x1E0, 0x1E0),
	VOTE_MAP(23, MMVOTE_VENC_GCON_BASE, 0x4, 0x8),
	VOTE_MAP(24, MMVOTE_VENC_GCON_BASE, 0x14, 0x10),
	VOTE_MAP(25, MMVOTE_VENC_GCON_CORE1_BASE, 0x4, 0x8),
	VOTE_MAP(26, MMVOTE_VENC_GCON_CORE1_BASE, 0x14, 0x10),
	VOTE_MAP(27, MMVOTE_VENC_GCON_CORE2_BASE, 0x4, 0x8),
	VOTE_MAP(28, MMVOTE_VENC_GCON_CORE2_BASE, 0x14, 0x10),
	VOTE_MAP(30, CKSYS_GP2_BASE, 0x0028, 0x0024),
	VOTE_MAP(31, CKSYS_GP2_BASE, 0x0038, 0x0034),
	VOTE_MAP(32, CKSYS_GP2_BASE, 0x0048, 0x0044),
	VOTE_MAP(33, CKSYS_GP2_BASE, 0x0058, 0x0054),
	VOTE_MAP(34, CKSYS_GP2_BASE, 0x0068, 0x0064),
	VOTE_MAP(35, CKSYS_GP2_BASE, 0x0078, 0x0074),
};

static const struct range mmvote_cg_mtcmos_dummy_range[] = {
	{ MMVOTE_CG_MTCMOS_DUMMY_RANGE_START1, MMVOTE_CG_MTCMOS_DUMMY_RANGE_END1 },
	{ MMVOTE_CG_MTCMOS_DUMMY_RANGE_START2, MMVOTE_CG_MTCMOS_DUMMY_RANGE_END2 },
};

static const struct range mmvote_mtcmos_mtcmos_dummy_range[] = {
	{ PM_ISP_TRAW, PM_DSI_PHY2 },
	{ PM_DUMMY1, PM_DUMMY2 },
};

#define CG_VDEC_SOC_GCON_1	19

static const struct cg_mtcmos_map mmvote_cg_mtcmos_table[] = {
	{ CG_VDEC_SOC_GCON_1, PM_VDE_VCORE0 },
	{ CG_VDEC_SOC_GCON_1, PM_VDE0 },
};

struct mtcmos_mux_map {
	u8 mtcmos_idx;
	u8 mux_idx;
};

static const struct mtcmos_mux_map mmvote_mtcmos_mux_table[] = {
	{ PM_ISP_TRAW, CLK_IMG1_SEL },
	{ PM_ISP_DIP, CLK_IMG1_SEL },
	{ PM_ISP_MAIN, CLK_IMG1_SEL },
	{ PM_ISP_MAIN, CLK_IPE_SEL },
	{ PM_ISP_VCORE, CLK_IMG1_SEL },
	{ PM_ISP_VCORE, CLK_IPE_SEL },
	{ PM_ISP_WPE_EIS, CLK_IMG1_SEL },
	{ PM_ISP_WPE_TNR, CLK_IMG1_SEL },
	{ PM_ISP_WPE_LITE, CLK_IMG1_SEL },
	{ PM_VDE0, CLK_VDEC_SEL },
	{ PM_VDE1, CLK_VDEC_SEL },
	{ PM_VDE_VCORE0, CLK_VDEC_SEL },
	{ PM_VEN0, CLK_VENC_SEL },
	{ PM_VEN1, CLK_VENC_SEL },
	{ PM_VEN2, CLK_VENC_SEL },
	{ PM_CAM_MRAW, CLK_DPE_SEL },
	{ PM_CAM_RAWA, CLK_CAM_SEL },
	{ PM_CAM_RAWB, CLK_CAM_SEL },
	{ PM_CAM_RAWC, CLK_CAM_SEL },
	{ PM_CAM_RMSA, CLK_CAM_SEL },
	{ PM_CAM_RMSB, CLK_CAM_SEL },
	{ PM_CAM_RMSC, CLK_CAM_SEL },
	{ PM_CAM_MAIN, CLK_CAM_SEL },
	{ PM_CAM_VCORE, CLK_CAM_SEL },
	{ PM_CAM_VCORE, CLK_CAMTM_SEL },
	{ PM_CAM_VCORE, CLK_CCUSYS_SEL },
	{ PM_CAM_CCU, CLK_CCUSYS_SEL },
	{ PM_CAM_CCU, CLK_CCUTM_SEL },
	{ PM_CAM_CCU_AO, CLK_CCUSYS_SEL },
	{ PM_DISP_VCORE, CLK_DISP_SEL },
	{ PM_DIS0, CLK_DISP_SEL },
	{ PM_DIS1, CLK_DISP_SEL },
	{ PM_OVL0, CLK_DISP_SEL },
	{ PM_OVL1, CLK_DISP_SEL },
	{ PM_DISP_EDPTX, CLK_DISP_SEL },
	{ PM_DISP_DPTX, CLK_DISP_SEL },
	{ PM_MML0, CLK_MDP_SEL },
	{ PM_MML1, CLK_MDP_SEL },
	{ PM_MMINFRA0, CLK_MMINFRA_SEL },
	{ PM_MMINFRA0, CLK_MMINFRA_SNOC_SEL },
	{ PM_MMINFRA1, CLK_MMINFRA_SEL },
	{ PM_MMINFRA1, CLK_MMINFRA_SNOC_SEL },
	{ PM_MMINFRA_AO, CLK_MMINFRA_AO_SEL },
	{ PM_CSI_BS_RX, CLK_SENINF2_SEL },
	{ PM_CSI_BS_RX, CLK_SENINF3_SEL },
	{ PM_CSI_BS_RX, CLK_SENINF4_SEL },
	{ PM_CSI_BS_RX, CLK_SENINF5_SEL },
	{ PM_CSI_LS_RX, CLK_SENINF0_SEL },
	{ PM_CSI_LS_RX, CLK_SENINF1_SEL },
};

#define CG_MTCMOS_FILED_WIDTH	7
#define CG_MTCMOS_FIELD_COUNT	4
#define CG_MTCMOS_FIELD_MASK	GENMASK(6, 0)

#define MTCMOS_MTCMOS_FILED_WIDTH	8
#define MTCMOS_MTCMOS_FIELD_COUNT	4
#define MTCMOS_MTCMOS_FIELD_MASK	GENMASK(7, 0)
#define MTCMOS_LEVEL_SHIFT		6

static void vote_init(void)
{
	u32 group;
	u32 offset;
	void *addr;
	u32 start, end;

	/* Vote map */
	for (int i = 0; i < ARRAY_SIZE(vote_maps); i++) {
		if (!vote_maps[i].map_base)
			continue;

		write32(VOTE_CG_SET_ADDR(i), vote_maps[i].map_base + vote_maps[i].set_addr);
		write32(VOTE_CG_CLR_ADDR(i), vote_maps[i].map_base + vote_maps[i].clr_addr);
	}

	/* VOTE_REG_EN0 0x1514 = 0x89FF8001 */
	write32(VOTE_REG_EN0, 0x89FF8001);

	/* VOTE_REG_EN1 0x1518 = 0x1FF */
	write32(VOTE_REG_EN1, 0x1FF);

	/* VOTE_REG_EN6 0x152C = 0x1400 */
	write32(VOTE_REG_EN6, BIT(12) | BIT(10));

	/* VOTE_DCM 0x1510 = 0x1E7 */
	write32(VOTE_DCM, 0x1E7);

	/* VOTE_APB_M_REG0 0x1530 = 0x6 */
	write32(VOTE_APB_M_REG0, 0x6);

	/* VOTE_CTRL 0x150C = 0x80000001 */
	write32(VOTE_CTRL, BIT(31) | BIT(0));

	/* Disable Timeout:0x0 */
	write32(VOTE_TIMEOUT0, 0);
	write32(VOTE_TIMEOUT1, 0);

	/* VOTE_APB_M_REG0 0x1530[25:24] = 0x3 */
	setbits32(VOTE_APB_M_REG0, BIT(25) | BIT(24));

	/* CG map MTCMOS */
	for (int i = 0; i < ARRAY_SIZE(mmvote_cg_mtcmos_dummy_range); i++) {
		start = vote_cg_mtcmos_dummy_range[i].start;
		end = vote_cg_mtcmos_dummy_range[i].end;

		for (int cg_idx = start; cg_idx <= end; cg_idx++) {
			group = cg_idx / CG_MTCMOS_FIELD_COUNT;
			offset = cg_idx % CG_MTCMOS_FIELD_COUNT;
			offset *= CG_MTCMOS_FILED_WIDTH;
			addr = VOTE_CG_MAP_MTCMOS(group);

			clrsetbits32(addr, CG_MTCMOS_FIELD_MASK << offset, PM_DUMMY << offset);
		}
	}

	/* Link setup */
	write32(VOTE_MTCMOS_MODE_SEL_0, 0);
	write32(VOTE_MTCMOS_MODE_SEL_1, 0);
	write32(VOTE_MUX_MAPPING_LINK_EN_0, 0);
	write32(VOTE_MUX_MAPPING_LINK_EN_1, 0);
	write32(VOTE_MUX_MODE_SEL_0, 0);
	write32(VOTE_MUX_MODE_SEL_1, 0);
	write32(VOTE_MUX_MAPPING_LINK_EN_2, 0);
	write32(VOTE_MUX_MODE_SEL_2, 0);
	write32(VOTE_MTCMOS_DIS_MUX_0, 0xFFFFFFFF);
	write32(VOTE_MTCMOS_DIS_MUX_1, 0xFFFFFFFF);

	/* Lock donmain */
	write32(VOTE_REG_KEY, VOTE_LOCK_REG_CFG);

	/* Vote enable */
	write32(VOTE_APB_M_REG5, BIT(0));
}

static void mmvote_init(void)
{
	u32 group;
	u32 offset;
	u32 level_offset;
	void *addr;
	u32 start, end;

	/* Vote setup */
	for (int i = 0; i < ARRAY_SIZE(mmvote_maps); i++) {
		if (!mmvote_maps[i].map_base)
			continue;

		write32(MMVOTE_CG_SET_ADDR(i),
			mmvote_maps[i].map_base + mmvote_maps[i].set_addr);
		write32(MMVOTE_CG_CLR_ADDR(i),
			mmvote_maps[i].map_base + mmvote_maps[i].clr_addr);

	}

	/* MMVOTE_REG_EN0 0x1514 = 0xBDFF8001 */
	write32(MMVOTE_REG_EN0, 0xBDFF8001);

	/* MMVOTE_REG_EN1 0x1518 = 0x1CB */
	write32(MMVOTE_REG_EN1, 0x1CB);

	/* MMVOTE_REG_EN6 0x152C = 0x1400 */
	write32(MMVOTE_REG_EN6, BIT(12) | BIT(10));

	/* MMVOTE_APB_M_REG0 0x1530 = 0x7 */
	write32(MMVOTE_APB_M_REG0, BIT(2) | BIT(1) | BIT(0));

	/* MMVOTE_CTRL 0x150C = 0x80000001 */
	write32(MMVOTE_CTRL, BIT(31) | BIT(0));

	/* Disable Timeout: 0x0 */
	write32(MMVOTE_TIMEOUT0, 0);
	write32(MMVOTE_TIMEOUT1, 0);

	/* MMVOTE_APB_M_REG0 0x1530[25:24] = 0x3 */
	setbits32(MMVOTE_APB_M_REG0, BIT(25) | BIT(24));

	/* CG map MTCMOS */
	for (int i = 0; i < ARRAY_SIZE(mmvote_cg_mtcmos_dummy_range); i++) {
		start = mmvote_cg_mtcmos_dummy_range[i].start;
		end = mmvote_cg_mtcmos_dummy_range[i].end;

		for (int cg_idx = start; cg_idx <= end; cg_idx++) {
			group = cg_idx / CG_MTCMOS_FIELD_COUNT;
			offset = cg_idx % CG_MTCMOS_FIELD_COUNT;
			offset *= CG_MTCMOS_FILED_WIDTH;
			addr = MMVOTE_CG_MAP_MTCMOS(group);

			clrsetbits32(addr, CG_MTCMOS_FIELD_MASK << offset, PM_DUMMY1 << offset);
		}
	}

	for (int i = 0; i < ARRAY_SIZE(mmvote_cg_mtcmos_table); i++) {
		group = mmvote_cg_mtcmos_table[i].cg_idx / CG_MTCMOS_FIELD_COUNT;
		offset = mmvote_cg_mtcmos_table[i].cg_idx % CG_MTCMOS_FIELD_COUNT;
		offset *= CG_MTCMOS_FILED_WIDTH;
		addr = MMVOTE_CG_MAP_MTCMOS(group);

		clrsetbits32(addr,
			     CG_MTCMOS_FIELD_MASK << offset,
			     mmvote_cg_mtcmos_table[i].mtcmos_idx << offset);
	}

	/* MTCMOS map MTCMOS */
	for (int i = 0; i < ARRAY_SIZE(mmvote_mtcmos_mtcmos_dummy_range); i++) {
		start = mmvote_mtcmos_mtcmos_dummy_range[i].start;
		end = mmvote_mtcmos_mtcmos_dummy_range[i].end;

		for (int mtcmos_idx = start; mtcmos_idx <= end; mtcmos_idx++) {
			group = mtcmos_idx / MTCMOS_MTCMOS_FIELD_COUNT;
			offset = mtcmos_idx % MTCMOS_MTCMOS_FIELD_COUNT;
			offset *= MTCMOS_MTCMOS_FILED_WIDTH;
			level_offset = offset + MTCMOS_LEVEL_SHIFT;
			addr = MMVOTE_MTCMOS_MAP_MTCMOS(group);

			clrsetbits32(addr, MTCMOS_MTCMOS_FIELD_MASK << offset,
				     mtcmos_idx << offset | 3 << level_offset);
		}
	}

	/* MTCMOS map MUX */
	for (int i = 0; i < ARRAY_SIZE(mmvote_mtcmos_mux_table); i++) {
		group = mmvote_mtcmos_mux_table[i].mux_idx / 32;
		addr = MMVOTE_MTCMOS_MAP_MUX(mmvote_mtcmos_mux_table[i].mtcmos_idx, group);

		setbits32(addr, BIT(mmvote_mtcmos_mux_table[i].mux_idx % 32));
	}
	write32(MMVOTE_PRE_CLK_MUX(0), 0x1785FC0);
	write32(MMVOTE_PRE_CLK_MUX(1), 0);

	/* Link Setup */
	write32(MMVOTE_MTCMOS_DIS_MUX_0, 0x0);
	write32(MMVOTE_MTCMOS_DIS_MUX_1, 0x3E0);
	write32(MMVOTE_MTCMOS_MODE_SEL_0, 0xFFBFFFFF);

	/* Lock domain */
	write32(MMVOTE_REG_KEY, MMVOTE_UNLOCK_REG_CFG);

	/* mmvote enable */
	write32(MMVOTE_APB_M_REG5, BIT(0));
}

static u32 mt_get_subsys_freq(const struct fmeter_data *fm_data, u32 id)
{
	u32 freq, output;
	u32 clk_div = 1, post_div = 1;
	bool ckdiv_en = false;

	if (fm_data->pll_con0 != NULL) {
		ckdiv_en = !!(read32(fm_data->pll_con0) & BIT(16));
		setbits32(fm_data->pll_con0, BIT(16) | BIT(12));
	}

	if (fm_data->id == MFGPLL_CTRL || fm_data->id == MFGPLL_SC0_CTRL ||
	    fm_data->id == MFGPLL_SC1_CTRL)
		setbits32(fm_data->pll_con5, BIT(4));

	write32(fm_data->con0, 0);
	setbits32(fm_data->con0, BIT(15));

	if (fm_data->id == VLP_CKSYS_CTRL)
		clrsetbits32(fm_data->con0, GENMASK(20, 16), id << 16);
	else
		clrsetbits32(fm_data->con0, GENMASK(2, 0), id);

	clrsetbits32(fm_data->con1, GENMASK(31, 16), 0x1FF << 16);

	clrbits32(fm_data->con0, GENMASK(31, 24));

	setbits32(fm_data->con0, BIT(12));
	setbits32(fm_data->con0, BIT(4));
	clrbits32(fm_data->con0, BIT(1) | BIT(0));

	/* wait frequency meter finish */
	if (fm_data->id == VLP_CKSYS_CTRL) {
		udelay(VLP_FM_WAIT_TIME_MS);
	} else if (!wait_us(1000, !(read32(fm_data->con0) & BIT(4)))) {
		printk(BIOS_ERR, "mtcmos_vote disable timeout\n");
		return 0;
	}

	output = read32(fm_data->con1) & 0xFFFF;
	freq = output * 26000 / 512;

	if (fm_data->pll_con0 != NULL && fm_data->id != VLP_CKSYS_CTRL && id == FM_PLL_CKDIV_CK)
		clk_div = (read32(fm_data->pll_con0) & GENMASK(10, 7)) >> 7;

	if (clk_div == 0)
		clk_div = 1;

	if (fm_data->pll_con1 != NULL && fm_data->id != VLP_CKSYS_CTRL && id == FM_PLL_CKDIV_CK)
		post_div = BIT((read32(fm_data->pll_con1) & GENMASK(26, 24)) >> 24);

	freq = freq * clk_div / post_div;

	if (fm_data->pll_con0 != NULL) {
		if (ckdiv_en)
			clrbits32(fm_data->pll_con0, BIT(12));
		else
			clrbits32(fm_data->pll_con0, BIT(16) | BIT(12));
	}

	if (fm_data->id == MFGPLL_CTRL || fm_data->id == MFGPLL_SC0_CTRL ||
	    fm_data->id == MFGPLL_SC1_CTRL)
		clrbits32(fm_data->pll_con5, BIT(4));

	write32(fm_data->con0, 0x8000);

	printk(BIOS_INFO, "meter[%d:%d] = %d Khz(output: %d, clk_div: %d, post_div: %d)\n",
	       fm_data->id, id, freq, output, clk_div, post_div);

	return freq;
}

u32 mt_get_vlpck_freq(u32 id)
{
	return mt_get_subsys_freq(&fmd[VLP_CKSYS_CTRL], id);
}

static inline u32 mt_get_cpu_freq(u32 id)
{
	return mt_get_subsys_freq(&fmd[id], FM_PLL_CKDIV_CK);
}

static inline u32 mt_get_gpu_freq(u32 id)
{
	return mt_get_subsys_freq(&fmd[id], FM_PLL_CK);
}

static void mt_dump_cpu_freq(void)
{
	mt_get_cpu_freq(ARMPLL_B_CTRL);
	mt_get_cpu_freq(ARMPLL_BL_CTRL);
	mt_get_cpu_freq(ARMPLL_LL_CTRL);
	mt_get_cpu_freq(CCIPLL_CTRL);
	mt_get_cpu_freq(PTPPLL_CTRL);
}

static void mt_dump_gpu_freq(void)
{
	mt_get_gpu_freq(MFGPLL_CTRL);
	mt_get_gpu_freq(MFGPLL_SC0_CTRL);
	mt_get_gpu_freq(MFGPLL_SC1_CTRL);
}

void pll_set_pcw_change(const struct pll *pll)
{
	setbits32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	write32(&mtk_apmixed->ref_clk_req_protected_con, 0);
	write32(&mtk_apmixed2->gp2_ref_clk_req_protected_con, 0);
	write32(&mtk_vlpsys->vlp_ref_clk_req_protected_con, 0);

	/* Set xPLL frequency */
	for (i = 0; i < ARRAY_SIZE(pll_rates); i++)
		pll_set_rate(&plls[pll_rates[i].id], pll_rates[i].rate);

	setbits32(&mtk_apmixed->univpll_con[0], BIT(17));
	setbits32(&mtk_apmixed2->univpll2_con[0], BIT(17));

	setbits32(&mtk_vlpsys->vlp_ap_pll_con3, BIT(5) | BIT(0));

	clrbits32(&mtk_mcusys->cpu_src_clk_config, BIT(0));
	clrbits32(&mtk_mcusys->dsu_pcsm_clk_src_config, BIT(0));
	clrbits32(&mtk_mcusys->infra_clk_src_config, BIT(0));

	/* [1]: clk_en = 0 */
	clrbits32(&mtk_mcusys->cpu_src_clk_config, BIT(1));
	clrbits32(&mtk_mcusys->dsu_pcsm_clk_src_config, BIT(1));
	clrbits32(&mtk_mcusys->infra_clk_src_config, BIT(1));
	clrbits32(&mtk_mcusys->ses_clk_free_ck_en, BIT(1));

	/* MCUSYS PLL set frequency */
	for (i = 0; i < ARRAY_SIZE(mcusys_pll_rates); i++)
		pll_set_rate(&mcusys_plls[mcusys_pll_rates[i].id], mcusys_pll_rates[i].rate);

	/* PLL all enable */
	write32(&mtk_apmixed->pllen_all_set, 0xF);
	write32(&mtk_apmixed2->gp2_pllen_all_set, 0x7F);
	write32(&mtk_vlpsys->vlp_pllen_apll_set, 0x3);

	setbits32(&mtk_ccipll->pll_con[0], BIT(0));
	setbits32(&mtk_armpll_ll->pll_con[0], BIT(0));
	setbits32(&mtk_armpll_bl->pll_con[0], BIT(0));
	setbits32(&mtk_armpll_b->pll_con[0], BIT(0));
	setbits32(&mtk_ptppll->pll_con[0], BIT(0));

	setbits32(&mtk_spm_mtcmos->spm2gpupm_con, BIT(4));

	/* Set MFGPLLs Frequency */
	for (i = 0; i < ARRAY_SIZE(mfg_pll_rates); i++)
		pll_set_rate(&mfg_plls[mfg_pll_rates[i].id], mfg_pll_rates[i].rate);

	setbits32(&mtk_mfgpll->pll_con[0], BIT(0));
	setbits32(&mtk_mfgpll_sc0->pll_con[0], BIT(0));
	setbits32(&mtk_mfgpll_sc1->pll_con[0], BIT(0));

	/* Wait PLL stable (20us) */
	udelay(PLL_EN_DELAY);

	setbits32(&mtk_mcusys->ses_clk_free_ck_en, BIT(1));
	setbits32(&mtk_mcusys->cpu_src_clk_config, BIT(2) | BIT(1) | BIT(0));
	setbits32(&mtk_mcusys->dsu_pcsm_clk_src_config, BIT(2) | BIT(1) | BIT(0));
	setbits32(&mtk_mcusys->infra_clk_src_config, BIT(2) | BIT(1) | BIT(0));

	/* pll all rstb */
	write32(&mtk_apmixed->pll_div_rstb_all_set, 0x3);
	write32(&mtk_apmixed2->gp2_pll_div_rstb_all_set, 0xF);

	setbits32(&mtk_apmixed->mainpll_con[0], 0xFF000000);
	setbits32(&mtk_apmixed->univpll_con[0], 0xFF000000);

	clrsetbits32(&mtk_mcusys->cpu_plldiv0_cfg, GENMASK(10, 8), 0x100);
	clrsetbits32(&mtk_mcusys->cpu_plldiv1_cfg, GENMASK(10, 8), 0x100);
	clrsetbits32(&mtk_mcusys->cpu_plldiv2_cfg, GENMASK(10, 8), 0x100);
	clrsetbits32(&mtk_mcusys->bus_plldiv_cfg, GENMASK(10, 8), 0x100);

	/* [10]: toggle mux update */
	setbits32(&mtk_mcusys->cpu_plldiv0_cfg, BIT(10));
	setbits32(&mtk_mcusys->cpu_plldiv1_cfg, BIT(10));
	setbits32(&mtk_mcusys->cpu_plldiv2_cfg, BIT(10));
	setbits32(&mtk_mcusys->bus_plldiv_cfg, BIT(10));

	clrbits32(&mtk_apinfra_io_ctrl->clk_io_intx_bus_ctrl, BIT(4) | BIT(2));

	clrbits32(&mtk_apinfra_io_noc->clk_io_noc_ctrl, BIT(4) | BIT(2));

	/* free run */
	clrbits32(&mtk_apinfra_mem_ctrl->vdnr_dcm_mem_intx_bus_ctrl, GENMASK(4, 2));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[0], BIT(7) | BIT(4));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[1], BIT(21));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[2], BIT(22));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[3], BIT(20));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[4], BIT(24));
	clrbits32(&mtk_apinfra_mem_ctrl->clk_mem_intx_bus_ctrl[5], BIT(23));
	clrbits32(&mtk_apinfra_mem_noc->vdnr_mem_intf_par_bus_ctrl, BIT(4) | BIT(2));

	/* BUS idle to cksys protect release */
	write32(&mtk_topckgen->clk_prot_idle_reg, 0xFFFFFFFF);
	write32(&mtk_topckgen2->cksys2_clk_prot_idle_reg, 0xFFFFFFFF);
	write32(&mtk_vlpsys->vlp_clk_prot_idle_reg, 0xFFFFFFFF);

	setbits32(&mtk_peri->vdnr_dcm_top_peri_par_bus_u_peri_par_bus_ctrl,
		  BIT(14) | BIT(11) | BIT(8) | BIT(5));

	setbits32(&mtk_usb->vdnr_dcm_top_usb_bus_u_usb_bus_ctrl, BIT(10) | BIT(7) | BIT(4));

	write32(&mtk_topckgen->clk_cfg[0].clr, 0x7);
	write32(&mtk_topckgen->clk_cfg[0].set, 0x5);
	write32(&mtk_topckgen->clk_cfg_update[0], 0x1);

	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		pll_mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	write32(&mtk_topckgen->clk_cfg_update[0], 0x7FFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[1], 0x7FFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[2], 0x7FFF);

	for (i = 0; i < ARRAY_SIZE(cksys2_mux_sels); i++)
		pll_mux_set_sel(&cksys2_muxes[cksys2_mux_sels[i].id], cksys2_mux_sels[i].sel);

	write32(&mtk_topckgen2->cksys2_clk_cfg_update, 0x7FFFFFF);

	for (i = 0; i < ARRAY_SIZE(vlp_mux_sels); i++)
		pll_mux_set_sel(&vlp_muxes[vlp_mux_sels[i].id], vlp_mux_sels[i].sel);

	write32(&mtk_vlpsys->vlp_clk_cfg_update[0], 0x7FFFFFFF);
	write32(&mtk_vlpsys->vlp_clk_cfg_update[1], 0x3FFF);

	mt_dump_cpu_freq();
	mt_dump_gpu_freq();

	write32(&mtk_apifrbus->apifrbus_ao_mem_reg_module_cg.clr, 0x10);

	write32(&mtk_pericfg->pericfg_ao_peri_cg_0_set, 0x40000);
	write32(&mtk_pericfg->pericfg_ao_peri_cg_1_clr, 0x01FFFA44);
	write32(&mtk_pericfg->pericfg_ao_peri_cg_1_set, 0x00000002);
	write32(&mtk_pericfg->pericfg_ao_peri_cg_2_clr, 0x1C0000);

	setbits32(&mtk_ssr->ssr_top_ssr_top_clk_cfg[0], BIT(24) | BIT(16) | BIT(8) | BIT(0));
	setbits32(&mtk_ssr->ssr_top_ssr_top_clk_cfg[1], BIT(24));
}

void mt_pll_set_tvd_pll1_freq(u32 freq)
{
	const struct pll *pll = &plls[CLK_APMIXED2_TVDPLL3];

	pll_set_rate(pll, freq);
}

void mt_pll_edp_mux_set_sel(u32 sel)
{
	pll_mux_set_sel(&cksys2_muxes[CLK_CK2_DVO_SEL], sel);
}

void mt_pll_post_init(void)
{
	/* fenc setup */
	write32(&mtk_topckgen->clk_prot_idle_all_reg[0], 0);
	write32(&mtk_topckgen->clk_prot_idle_all_reg[1], 0);
	write32(&mtk_topckgen->clk_prot_idle_all_reg[2], 0);
	write32(&mtk_topckgen->clk_prot_idle_all_inv_reg[0], 0);
	write32(&mtk_topckgen->clk_prot_idle_all_inv_reg[1], 0);
	write32(&mtk_topckgen->clk_prot_idle_all_inv_reg[2], 0);
	write32(&mtk_topckgen->clk_prot_vote_ck_en_reg[0], 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_prot_vote_ck_en_reg[1], 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_prot_vote_ck_en_reg[2], 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_prot_spm_ck_en_reg, 0xFFFFFFFF);
	write32(&mtk_topckgen2->cksys2_clk_prot_idle_all_reg, 0);
	write32(&mtk_topckgen2->cksys2_clk_prot_idle_all_inv_reg, 0);
	write32(&mtk_topckgen2->cksys2_clk_prot_vote_ck_en_reg, 0xFFFFFFFF);
	write32(&mtk_topckgen2->cksys2_clk_prot_spm_ck_en_reg, 0xFFFFFFFF);
	write32(&mtk_vlpsys->vlp_clk_prot_idle_all_reg[0], 0);
	write32(&mtk_vlpsys->vlp_clk_prot_idle_all_reg[1], 0);
	write32(&mtk_vlpsys->vlp_clk_prot_idle_all_inv_reg[0], 0);
	write32(&mtk_vlpsys->vlp_clk_prot_idle_all_inv_reg[1], 0);
	write32(&mtk_apmixed->fenc_protected_con, 0xFFFFFFFF);
	write32(&mtk_apmixed2->gp2_fenc_protected_con, 0xFFF7FFFF);
	write32(&mtk_vlpsys->vlp_fenc_protected_con, 0xFFFFFFFF);
	write32(&mtk_apmixed->child_enable_mask_con, 0xFFFFFFFF);
	write32(&mtk_apmixed2->gp2_child_enable_mask_con, 0xFFFFFFFF);
	write32(&mtk_vlpsys->vlp_child_enable_mask_con, 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_fenc_bypass_reg[0], 0);
	write32(&mtk_topckgen->clk_fenc_bypass_reg[1], 0);
	write32(&mtk_topckgen->clk_fenc_bypass_reg[2], 0);
	write32(&mtk_topckgen2->cksys2_clk_fenc_bypass_reg, 0);
	write32(&mtk_vlpsys->vlp_clk_fenc_bypass_reg[0], 0);
	write32(&mtk_vlpsys->vlp_clk_fenc_bypass_reg[1], 0);
	write32(&mtk_apmixed->fenc_bypass_con, 0);
	write32(&mtk_apmixed2->gp2_fenc_bypass_con, 0);
	write32(&mtk_vlpsys->vlp_fenc_bypass_con, 0);
	write32(&mtk_apmixed->pllen_all_clr, 0xFFFFFFFF);
	write32(&mtk_apmixed2->gp2_pllen_all_clr, 0xFFFFFFFF);
	write32(&mtk_vlpsys->vlp_pllen_apll_clr, 0x3);

	/* power down unused mux */
	write32(&mtk_topckgen->clk_cfg[3].set, 0x8080);
	write32(&mtk_topckgen->clk_cfg[4].set, 0x80800000);
	write32(&mtk_topckgen->clk_cfg[6].set, 0x8000);
	write32(&mtk_topckgen->clk_cfg[9].set, 0x8080);
	write32(&mtk_topckgen->clk_cfg[12].set, 0x80800000);
	write32(&mtk_topckgen->clk_cfg[15].set, 0x8000);
	write32(&mtk_topckgen->clk_cfg[16].set, 0x800000);
	write32(&mtk_topckgen->clk_cfg[18].set, 0x80000000);
	write32(&mtk_topckgen->clk_cfg[19].set, 0x80808000);
	write32(&mtk_topckgen->clk_cfg[20].set, 0x80808080);
	write32(&mtk_topckgen->clk_cfg[21].set, 0x80808080);
	write32(&mtk_topckgen->clk_cfg[22].set, 0x80);

	write32(&mtk_vlpsys->vlp_clk_cfg[6].set, 0x80);
	write32(&mtk_vlpsys->vlp_clk_cfg[10].set, 0x80800000);
	write32(&mtk_vlpsys->vlp_clk_cfg[11].set, 0x80);

	write32(&mtk_topckgen2->cksys2_clk_cfg[0].set, 0x80808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[1].set, 0x80808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[2].set, 0x80808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[3].set, 0x80808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[4].set, 0x80808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[5].set, 0x808080);
	write32(&mtk_topckgen2->cksys2_clk_cfg[6].set, 0x80808080);

	vote_init();
	mmvote_init();
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	const struct pll *pll = &mcusys_plls[CLK_CPLL_ARMPLL_LL];

	clrbits32(&mtk_mcusys->cpu_plldiv0_cfg, GENMASK(10, 8));
	setbits32(&mtk_mcusys->cpu_plldiv0_cfg, BIT(10));

	clrbits32(pll->reg, MT8196_PLL_EN);
	pll_set_rate(pll, freq);
	setbits32(pll->reg, MT8196_PLL_EN);

	clrsetbits32(&mtk_mcusys->cpu_plldiv0_cfg, GENMASK(10, 8), BIT(8));
	setbits32(&mtk_mcusys->cpu_plldiv0_cfg, BIT(10));
}
