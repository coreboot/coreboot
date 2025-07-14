/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.1
 */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/pll.h>
#include <soc/spm.h>
#include <soc/spm_mtcmos.h>

#define VLP_FM_WAIT_TIME	40

enum mux_id {
	CLK_TOP_AXI_SEL,
	CLK_TOP_AXI_PERI_SEL,
	CLK_TOP_AXI_U_SEL,
	CLK_TOP_BUS_AXIMEM_SEL,
	CLK_TOP_DISP0_SEL,
	CLK_TOP_MMINFRA_SEL,
	CLK_TOP_UART_SEL,
	CLK_TOP_SPI0_SEL,
	CLK_TOP_SPI1_SEL,
	CLK_TOP_SPI2_SEL,
	CLK_TOP_SPI3_SEL,
	CLK_TOP_SPI4_SEL,
	CLK_TOP_SPI5_SEL,
	CLK_TOP_MSDC_MACRO_0P_SEL,
	CLK_TOP_MSDC50_0_HCLK_SEL,
	CLK_TOP_MSDC50_0_SEL,
	CLK_TOP_AES_MSDCFDE_SEL,
	CLK_TOP_MSDC_MACRO_1P_SEL,
	CLK_TOP_MSDC30_1_SEL,
	CLK_TOP_MSDC30_1_HCLK_SEL,
	CLK_TOP_MSDC_MACRO_2P_SEL,
	CLK_TOP_MSDC30_2_SEL,
	CLK_TOP_MSDC30_2_HCLK_SEL,
	CLK_TOP_AUD_INTBUS_SEL,
	CLK_TOP_ATB_SEL,
	CLK_TOP_DISP_PWM_SEL,
	CLK_TOP_USB_TOP_P0_SEL,
	CLK_TOP_USB_XHCI_P0_SEL,
	CLK_TOP_USB_TOP_P1_SEL,
	CLK_TOP_USB_XHCI_P1_SEL,
	CLK_TOP_USB_TOP_P2_SEL,
	CLK_TOP_USB_XHCI_P2_SEL,
	CLK_TOP_USB_TOP_P3_SEL,
	CLK_TOP_USB_XHCI_P3_SEL,
	CLK_TOP_USB_TOP_P4_SEL,
	CLK_TOP_USB_XHCI_P4_SEL,
	CLK_TOP_I2C_SEL,
	CLK_TOP_AUD_ENGEN1_SEL,
	CLK_TOP_AUD_ENGEN2_SEL,
	CLK_TOP_AES_UFSFDE_SEL,
	CLK_TOP_U_SEL,
	CLK_TOP_U_MBIST_SEL,
	CLK_TOP_AUD_1_SEL,
	CLK_TOP_AUD_2_SEL,
	CLK_TOP_PWM_SEL,
	CLK_TOP_AUDIO_H_SEL,
	CLK_TOP_MCUPM_SEL,
	CLK_TOP_MEM_SUB_SEL,
	CLK_TOP_MEM_SUB_PERI_SEL,
	CLK_TOP_MEM_SUB_U_SEL,
	CLK_TOP_EMI_N_SEL,
	CLK_TOP_DXCC_SEL,
	CLK_TOP_DP_SEL,
	CLK_TOP_EDP_SEL,
	CLK_TOP_EDP_FAVT_SEL,
	CLK_TOP_SFLASH_SEL,
	CLK_TOP_ECC_SEL,
	CLK_TOP_APLL_I2SIN0_MCK_SEL,
	CLK_TOP_APLL_I2SIN1_MCK_SEL,
	CLK_TOP_APLL_I2SIN4_MCK_SEL,
	CLK_TOP_APLL_I2SOUT0_MCK_SEL,
	CLK_TOP_APLL_I2SOUT1_MCK_SEL,
	CLK_TOP_APLL_I2SOUT4_MCK_SEL,
	CLK_TOP_APLL_TDMOUT_MCK_SEL,
};

enum vlp_mux_id {
	CLK_VLP_CK_SCP_SEL,
	CLK_VLP_CK_PWRAP_ULPOSC_SEL,
	CLK_VLP_CK_SPMI_P_MST_SEL,
	CLK_VLP_CK_DVFSRC_SEL,
	CLK_VLP_CK_PWM_VLP_SEL,
	CLK_VLP_CK_AXI_VLP_SEL,
	CLK_VLP_CK_SYSTIMER_26M_SEL,
	CLK_VLP_CK_SSPM_SEL,
	CLK_VLP_CK_SSPM_F26M_SEL,
	CLK_VLP_CK_SRCK_SEL,
	CLK_VLP_CK_SCP_SPI_SEL,
	CLK_VLP_CK_SCP_IIC_SEL,
	CLK_VLP_CK_SCP_SPI_HIGH_SPD_SEL,
	CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL,
	CLK_VLP_CK_SSPM_ULPOSC_SEL,
	CLK_VLP_CK_APXGPT_26M_SEL,
	CLK_VLP_CK_AUD_ADC_SEL,
	CLK_VLP_CK_KP_IRQ_GEN_SEL
};

struct mux_sel {
	u8 id;
	u8 sel;
};

struct vlp_mux_sel {
	u8 id;
	u8 sel;
};

#define MUX(_id, _sys, _clk_cfg, _mux_shift, _mux_width)\
	[_id] = {					\
		.reg = &_sys->_clk_cfg.cfg,		\
		.set_reg = &_sys->_clk_cfg.set,		\
		.clr_reg = &_sys->_clk_cfg.clr,		\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
	}

#define MUX_UPD(_id, _sys, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	[_id] = {					\
		.reg = &_sys->_clk_cfg.cfg,		\
		.set_reg = &_sys->_clk_cfg.set,		\
		.clr_reg = &_sys->_clk_cfg.clr,		\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
		.upd_reg = &_sys->_upd_reg,		\
		.upd_shift = _upd_shift,		\
	}

#define CKSYS_MUX(_id, _clk_cfg, _mux_shift, _mux_width)\
	MUX(_id, mtk_topckgen, _clk_cfg, _mux_shift, _mux_width)

#define CKSYS_MUX_UPD(_id, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	MUX_UPD(_id, mtk_topckgen, _clk_cfg, _mux_shift, _mux_width, _upd_reg, _upd_shift)

#define VLP_MUX_UPD(_id, _clk_cfg, _mux_shift, _mux_width, _upd_shift)\
	MUX_UPD(_id, mtk_vlpsys, _clk_cfg, _mux_shift, _mux_width, vlp_clk_cfg_update, \
		_upd_shift)

static const struct mux muxes[] = {
	CKSYS_MUX_UPD(CLK_TOP_AXI_SEL, clk_cfg[0], 0, 3, clk_cfg_update[0], 0),
	CKSYS_MUX_UPD(CLK_TOP_AXI_PERI_SEL, clk_cfg[0], 8, 2, clk_cfg_update[0], 1),
	CKSYS_MUX_UPD(CLK_TOP_AXI_U_SEL, clk_cfg[0], 16, 2, clk_cfg_update[0], 2),
	CKSYS_MUX_UPD(CLK_TOP_BUS_AXIMEM_SEL, clk_cfg[0], 24, 3, clk_cfg_update[0], 3),
	CKSYS_MUX_UPD(CLK_TOP_DISP0_SEL, clk_cfg[1], 0, 4, clk_cfg_update[0], 4),
	CKSYS_MUX_UPD(CLK_TOP_MMINFRA_SEL, clk_cfg[1], 8, 4, clk_cfg_update[0], 5),
	CKSYS_MUX_UPD(CLK_TOP_UART_SEL, clk_cfg[1], 16, 1, clk_cfg_update[0], 6),
	CKSYS_MUX_UPD(CLK_TOP_SPI0_SEL, clk_cfg[1], 24, 3, clk_cfg_update[0], 7),
	CKSYS_MUX_UPD(CLK_TOP_SPI1_SEL, clk_cfg[2], 0, 3, clk_cfg_update[0], 8),
	CKSYS_MUX_UPD(CLK_TOP_SPI2_SEL, clk_cfg[2], 8, 3, clk_cfg_update[0], 9),
	CKSYS_MUX_UPD(CLK_TOP_SPI3_SEL, clk_cfg[2], 16, 3, clk_cfg_update[0], 10),
	CKSYS_MUX_UPD(CLK_TOP_SPI4_SEL, clk_cfg[2], 24, 3, clk_cfg_update[0], 11),
	CKSYS_MUX_UPD(CLK_TOP_SPI5_SEL, clk_cfg[3], 0, 3, clk_cfg_update[0], 12),
	CKSYS_MUX_UPD(CLK_TOP_MSDC_MACRO_0P_SEL, clk_cfg[3], 8, 2, clk_cfg_update[0], 13),
	CKSYS_MUX_UPD(CLK_TOP_MSDC50_0_HCLK_SEL, clk_cfg[3], 16, 2, clk_cfg_update[0], 14),
	CKSYS_MUX_UPD(CLK_TOP_MSDC50_0_SEL, clk_cfg[3], 24, 3, clk_cfg_update[0], 15),
	CKSYS_MUX_UPD(CLK_TOP_AES_MSDCFDE_SEL, clk_cfg[4], 0, 3, clk_cfg_update[0], 16),
	CKSYS_MUX_UPD(CLK_TOP_MSDC_MACRO_1P_SEL, clk_cfg[4], 8, 2, clk_cfg_update[0], 17),
	CKSYS_MUX_UPD(CLK_TOP_MSDC30_1_SEL, clk_cfg[4], 16, 3, clk_cfg_update[0], 18),
	CKSYS_MUX_UPD(CLK_TOP_MSDC30_1_HCLK_SEL, clk_cfg[4], 24, 2, clk_cfg_update[0], 19),
	CKSYS_MUX_UPD(CLK_TOP_MSDC_MACRO_2P_SEL, clk_cfg[5], 0, 2, clk_cfg_update[0], 20),
	CKSYS_MUX_UPD(CLK_TOP_MSDC30_2_SEL, clk_cfg[5], 8, 3, clk_cfg_update[0], 21),
	CKSYS_MUX_UPD(CLK_TOP_MSDC30_2_HCLK_SEL, clk_cfg[5], 16, 2, clk_cfg_update[0], 22),
	CKSYS_MUX_UPD(CLK_TOP_AUD_INTBUS_SEL, clk_cfg[5], 24, 2, clk_cfg_update[0], 23),
	CKSYS_MUX_UPD(CLK_TOP_ATB_SEL, clk_cfg[6], 0, 2, clk_cfg_update[0], 24),
	CKSYS_MUX_UPD(CLK_TOP_DISP_PWM_SEL, clk_cfg[6], 8, 3, clk_cfg_update[0], 25),
	CKSYS_MUX_UPD(CLK_TOP_USB_TOP_P0_SEL, clk_cfg[6], 16, 2, clk_cfg_update[0], 26),
	CKSYS_MUX_UPD(CLK_TOP_USB_XHCI_P0_SEL, clk_cfg[6], 24, 2, clk_cfg_update[0], 27),
	CKSYS_MUX_UPD(CLK_TOP_USB_TOP_P1_SEL, clk_cfg[7], 0, 2, clk_cfg_update[0], 28),
	CKSYS_MUX_UPD(CLK_TOP_USB_XHCI_P1_SEL, clk_cfg[7], 8, 2, clk_cfg_update[0], 29),
	CKSYS_MUX_UPD(CLK_TOP_USB_TOP_P2_SEL, clk_cfg[7], 16, 2, clk_cfg_update[0], 30),
	CKSYS_MUX_UPD(CLK_TOP_USB_XHCI_P2_SEL, clk_cfg[7], 24, 2, clk_cfg_update[1], 0),
	CKSYS_MUX_UPD(CLK_TOP_USB_TOP_P3_SEL, clk_cfg[8], 0, 2, clk_cfg_update[1], 1),
	CKSYS_MUX_UPD(CLK_TOP_USB_XHCI_P3_SEL, clk_cfg[8], 8, 2, clk_cfg_update[1], 2),
	CKSYS_MUX_UPD(CLK_TOP_USB_TOP_P4_SEL, clk_cfg[8], 16, 2, clk_cfg_update[1], 3),
	CKSYS_MUX_UPD(CLK_TOP_USB_XHCI_P4_SEL, clk_cfg[8], 24, 2, clk_cfg_update[1], 4),
	CKSYS_MUX_UPD(CLK_TOP_I2C_SEL, clk_cfg[9], 0, 2, clk_cfg_update[1], 5),
	CKSYS_MUX_UPD(CLK_TOP_AUD_ENGEN1_SEL, clk_cfg[9], 24, 2, clk_cfg_update[1], 8),
	CKSYS_MUX_UPD(CLK_TOP_AUD_ENGEN2_SEL, clk_cfg[10], 0, 2, clk_cfg_update[1], 9),
	CKSYS_MUX_UPD(CLK_TOP_AES_UFSFDE_SEL, clk_cfg[10], 8, 3, clk_cfg_update[1], 10),
	CKSYS_MUX_UPD(CLK_TOP_U_SEL, clk_cfg[10], 16, 3, clk_cfg_update[1], 11),
	CKSYS_MUX_UPD(CLK_TOP_U_MBIST_SEL, clk_cfg[10], 24, 2, clk_cfg_update[1], 12),
	CKSYS_MUX_UPD(CLK_TOP_AUD_1_SEL, clk_cfg[11], 0, 1, clk_cfg_update[1], 13),
	CKSYS_MUX_UPD(CLK_TOP_AUD_2_SEL, clk_cfg[11], 8, 1, clk_cfg_update[1], 14),
	CKSYS_MUX_UPD(CLK_TOP_PWM_SEL, clk_cfg[12], 0, 1, clk_cfg_update[1], 17),
	CKSYS_MUX_UPD(CLK_TOP_AUDIO_H_SEL, clk_cfg[12], 8, 2, clk_cfg_update[1], 18),
	CKSYS_MUX_UPD(CLK_TOP_MCUPM_SEL, clk_cfg[12], 16, 2, clk_cfg_update[1], 19),
	CKSYS_MUX_UPD(CLK_TOP_MEM_SUB_SEL, clk_cfg[12], 24, 4, clk_cfg_update[1], 20),
	CKSYS_MUX_UPD(CLK_TOP_MEM_SUB_PERI_SEL, clk_cfg[13], 0, 3, clk_cfg_update[1], 21),
	CKSYS_MUX_UPD(CLK_TOP_MEM_SUB_U_SEL, clk_cfg[13], 8, 3, clk_cfg_update[1], 22),
	CKSYS_MUX_UPD(CLK_TOP_EMI_N_SEL, clk_cfg[13], 16, 3, clk_cfg_update[1], 23),
	CKSYS_MUX_UPD(CLK_TOP_DXCC_SEL, clk_cfg[15], 24, 2, clk_cfg_update[2], 1),
	CKSYS_MUX_UPD(CLK_TOP_DP_SEL, clk_cfg[16], 16, 3, clk_cfg_update[2], 4),
	CKSYS_MUX_UPD(CLK_TOP_EDP_SEL, clk_cfg[16], 24, 3, clk_cfg_update[2], 5),
	CKSYS_MUX_UPD(CLK_TOP_EDP_FAVT_SEL, clk_cfg_17, 0, 3, clk_cfg_update[2], 6),
	CKSYS_MUX_UPD(CLK_TOP_SFLASH_SEL, clk_cfg_18, 0, 3, clk_cfg_update[2], 10),
	CKSYS_MUX_UPD(CLK_TOP_ECC_SEL, clk_cfg_19, 8, 3, clk_cfg_update[2], 15),
};

static const struct mux_sel mux_sels[] = {
	{ .id = CLK_TOP_AXI_SEL, .sel = 2 },
	{ .id = CLK_TOP_AXI_PERI_SEL, .sel = 2 },
	{ .id = CLK_TOP_AXI_U_SEL, .sel = 2 },
	{ .id = CLK_TOP_BUS_AXIMEM_SEL, .sel = 2 },
	{ .id = CLK_TOP_DISP0_SEL, .sel = 11 },
	{ .id = CLK_TOP_MMINFRA_SEL, .sel = 14 },
	{ .id = CLK_TOP_UART_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI0_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI1_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI2_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI3_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI4_SEL, .sel = 1 },
	{ .id = CLK_TOP_SPI5_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC_MACRO_0P_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC50_0_HCLK_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC50_0_SEL, .sel = 1 },
	{ .id = CLK_TOP_AES_MSDCFDE_SEL, .sel = 4 },
	{ .id = CLK_TOP_MSDC_MACRO_1P_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC30_1_SEL, .sel = 4 },
	{ .id = CLK_TOP_MSDC30_1_HCLK_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC_MACRO_2P_SEL, .sel = 1 },
	{ .id = CLK_TOP_MSDC30_2_SEL, .sel = 4 },
	{ .id = CLK_TOP_MSDC30_2_HCLK_SEL, .sel = 1 },
	{ .id = CLK_TOP_AUD_INTBUS_SEL, .sel = 1 },
	{ .id = CLK_TOP_ATB_SEL, .sel = 1 },
	{ .id = CLK_TOP_DISP_PWM_SEL, .sel = 6 },
	{ .id = CLK_TOP_USB_TOP_P0_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_XHCI_P0_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_TOP_P1_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_XHCI_P1_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_TOP_P2_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_XHCI_P2_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_TOP_P3_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_XHCI_P3_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_TOP_P4_SEL, .sel = 1 },
	{ .id = CLK_TOP_USB_XHCI_P4_SEL, .sel = 1 },
	{ .id = CLK_TOP_I2C_SEL, .sel = 2 },
	{ .id = CLK_TOP_AUD_ENGEN1_SEL, .sel = 2 },
	{ .id = CLK_TOP_AUD_ENGEN2_SEL, .sel = 2 },
	{ .id = CLK_TOP_AES_UFSFDE_SEL, .sel = 1 },
	{ .id = CLK_TOP_U_SEL, .sel = 5 },
	{ .id = CLK_TOP_U_MBIST_SEL, .sel = 3 },
	{ .id = CLK_TOP_AUD_1_SEL, .sel = 1 },
	{ .id = CLK_TOP_AUD_2_SEL, .sel = 1 },
	{ .id = CLK_TOP_PWM_SEL, .sel = 0 },
	{ .id = CLK_TOP_AUDIO_H_SEL, .sel = 3 },
	{ .id = CLK_TOP_MCUPM_SEL, .sel = 2 },
	{ .id = CLK_TOP_MEM_SUB_SEL, .sel = 9 },
	{ .id = CLK_TOP_MEM_SUB_PERI_SEL, .sel = 7 },
	{ .id = CLK_TOP_MEM_SUB_U_SEL, .sel = 7 },
	{ .id = CLK_TOP_EMI_N_SEL, .sel = 2 },
	{ .id = CLK_TOP_DXCC_SEL, .sel = 1 },
	{ .id = CLK_TOP_DP_SEL, .sel = 4 },
	{ .id = CLK_TOP_EDP_SEL, .sel = 4 },
	{ .id = CLK_TOP_EDP_FAVT_SEL, .sel = 4 },
	{ .id = CLK_TOP_SFLASH_SEL, .sel = 2 },
	{ .id = CLK_TOP_ECC_SEL, .sel = 5 },
};

static const struct mux vlp_muxes[] = {
	VLP_MUX_UPD(CLK_VLP_CK_SCP_SEL, vlp_clk_cfg[0], 0, 4, 0),
	VLP_MUX_UPD(CLK_VLP_CK_PWRAP_ULPOSC_SEL, vlp_clk_cfg[0], 8, 3, 1),
	VLP_MUX_UPD(CLK_VLP_CK_SPMI_P_MST_SEL, vlp_clk_cfg[0], 16, 4, 2),
	VLP_MUX_UPD(CLK_VLP_CK_DVFSRC_SEL, vlp_clk_cfg[0], 24, 1, 3),
	VLP_MUX_UPD(CLK_VLP_CK_PWM_VLP_SEL, vlp_clk_cfg[1], 0, 3, 4),
	VLP_MUX_UPD(CLK_VLP_CK_AXI_VLP_SEL, vlp_clk_cfg[1], 8, 3, 5),
	VLP_MUX_UPD(CLK_VLP_CK_SYSTIMER_26M_SEL, vlp_clk_cfg[1], 16, 1, 6),
	VLP_MUX_UPD(CLK_VLP_CK_SSPM_SEL, vlp_clk_cfg[1], 24, 3, 7),
	VLP_MUX_UPD(CLK_VLP_CK_SSPM_F26M_SEL, vlp_clk_cfg[2], 0, 1, 8),
	VLP_MUX_UPD(CLK_VLP_CK_SRCK_SEL, vlp_clk_cfg[2], 8, 1, 9),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_SPI_SEL, vlp_clk_cfg[2], 16, 2, 10),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_IIC_SEL, vlp_clk_cfg[2], 24, 2, 11),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_SPI_HIGH_SPD_SEL, vlp_clk_cfg[3], 0, 2, 12),
	VLP_MUX_UPD(CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL, vlp_clk_cfg[3], 8, 2, 13),
	VLP_MUX_UPD(CLK_VLP_CK_SSPM_ULPOSC_SEL, vlp_clk_cfg[3], 16, 2, 14),
	VLP_MUX_UPD(CLK_VLP_CK_APXGPT_26M_SEL, vlp_clk_cfg[3], 24, 1, 15),
	VLP_MUX_UPD(CLK_VLP_CK_AUD_ADC_SEL, vlp_clk_cfg[5], 16, 2, 22),
	VLP_MUX_UPD(CLK_VLP_CK_KP_IRQ_GEN_SEL, vlp_clk_cfg[5], 24, 3, 23),
};

static const struct vlp_mux_sel vlp_mux_sels[] = {
	{ .id = CLK_VLP_CK_SCP_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_PWRAP_ULPOSC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SPMI_P_MST_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_DVFSRC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_PWM_VLP_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_AXI_VLP_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_SYSTIMER_26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SSPM_SEL, .sel = 4 },
	{ .id = CLK_VLP_CK_SSPM_F26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SRCK_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_SPI_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_IIC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_SPI_HIGH_SPD_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SCP_IIC_HIGH_SPD_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_SSPM_ULPOSC_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_APXGPT_26M_SEL, .sel = 0 },
	{ .id = CLK_VLP_CK_AUD_ADC_SEL, .sel = 1 },
	{ .id = CLK_VLP_CK_KP_IRQ_GEN_SEL, .sel = 4 }
};

enum pll_id {
	CLK_APMIXED_ARMPLL_LL,
	CLK_APMIXED_ARMPLL_BL,
	CLK_APMIXED_CCIPLL,
	CLK_APMIXED_MAINPLL,
	CLK_APMIXED_UNIVPLL,
	CLK_APMIXED_MMPLL,
	CLK_APMIXED_MFGPLL,
	CLK_APMIXED_APLL1,
	CLK_APMIXED_APLL2,
	CLK_APMIXED_EMIPLL,
	CLK_APMIXED_APUPLL2,
	CLK_APMIXED_APUPLL,
	CLK_APMIXED_TVDPLL1,
	CLK_APMIXED_TVDPLL2,
	CLK_APMIXED_ETHPLL,
	CLK_APMIXED_MSDCPLL,
	CLK_APMIXED_UFSPLL
};

struct rate {
	u8 id; /* enum pll_id */
	u32 rate;
};

static const struct rate pll_rates[] = {
	{ CLK_APMIXED_ARMPLL_LL, ARMPLL_LL_HZ },
	{ CLK_APMIXED_ARMPLL_BL, ARMPLL_BL_HZ },
	{ CLK_APMIXED_CCIPLL, CCIPLL_HZ },
	{ CLK_APMIXED_MAINPLL, MAINPLL_HZ },
	{ CLK_APMIXED_UNIVPLL, UNIVPLL_HZ },
	{ CLK_APMIXED_MMPLL, MMPLL_HZ },
	{ CLK_APMIXED_MFGPLL, MFGPLL_HZ },
	{ CLK_APMIXED_APLL1, APLL1_HZ },
	{ CLK_APMIXED_APLL2, APLL2_HZ },
	{ CLK_APMIXED_EMIPLL, EMIPLL_HZ },
	{ CLK_APMIXED_APUPLL2, APUPLL2_HZ },
	{ CLK_APMIXED_APUPLL, APUPLL_HZ },
	{ CLK_APMIXED_TVDPLL1, TVDPLL1_HZ },
	{ CLK_APMIXED_TVDPLL2, TVDPLL2_HZ },
	{ CLK_APMIXED_ETHPLL, ETHPLL_HZ },
	{ CLK_APMIXED_MSDCPLL, MSDCPLL_HZ },
	{ CLK_APMIXED_UFSPLL, UFSPLL_HZ }
};

static const u32 pll_div_rate[] = {
	3800UL * MHz,
	1900 * MHz,
	950 * MHz,
	475 * MHz,
	237500 * KHz,
	0,
};

#define PLL_SYS(_id, _reg, _rstb, _pcwbits, _div_reg, _div_shift,	\
	_pcw_reg, _pcw_shift, _div_rate)				\
	[_id] = {							\
		.reg = &mtk_apmixed->_reg,				\
		.rstb_shift = _rstb,					\
		.pcwbits = _pcwbits,					\
		.div_reg = &mtk_apmixed->_div_reg,			\
		.div_shift = _div_shift,				\
		.pcw_reg = &mtk_apmixed->_pcw_reg,			\
		.pcw_shift = _pcw_shift,				\
		.div_rate = _div_rate,					\
	}

static const struct pll plls[] = {
	PLL_SYS(CLK_APMIXED_ARMPLL_LL, armpll_ll_con[0], NO_RSTB_SHIFT, 22,
		armpll_ll_con[1], 24, armpll_ll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_ARMPLL_BL, armpll_bl_con[0], NO_RSTB_SHIFT, 22,
		armpll_bl_con[1], 24, armpll_bl_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_CCIPLL, ccipll_con[0], NO_RSTB_SHIFT, 22,
		ccipll_con[1], 24, ccipll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_MAINPLL, mainpll_con[0], 23, 22,
		mainpll_con[1], 24, mainpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_UNIVPLL, univpll_con[0], 23, 22,
		univpll_con[1], 24, univpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_MMPLL, mmpll_con[0], 23, 22,
		mmpll_con[1], 24, mmpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_MFGPLL, mfgpll_con[0], NO_RSTB_SHIFT, 22,
		mfgpll_con[1], 24, mfgpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_APLL1, apll1_con[0], NO_RSTB_SHIFT, 32,
		apll1_con[1], 24, apll1_con[2], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_APLL2, apll2_con[0], NO_RSTB_SHIFT, 32,
		apll2_con[1], 24, apll2_con[2], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_EMIPLL, emipll_con[0], NO_RSTB_SHIFT, 22,
		emipll_con[1], 24, emipll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_APUPLL2, apupll2_con[0], NO_RSTB_SHIFT, 22,
		apupll2_con[1], 24, apupll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_APUPLL, apupll_con[0], NO_RSTB_SHIFT, 22,
		apupll_con[1], 24, apupll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_TVDPLL1, tvdpll1_con[0], NO_RSTB_SHIFT, 22,
		tvdpll1_con[1], 24, tvdpll1_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_TVDPLL2, tvdpll2_con[0], NO_RSTB_SHIFT, 22,
		tvdpll2_con[1], 24, tvdpll2_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_ETHPLL, ethpll_con[0], NO_RSTB_SHIFT, 22,
		ethpll_con[1], 24, ethpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_MSDCPLL, msdcpll_con[0], NO_RSTB_SHIFT, 22,
		msdcpll_con[1], 24, msdcpll_con[1], 0, pll_div_rate),
	PLL_SYS(CLK_APMIXED_UFSPLL, ufspll_con[0], NO_RSTB_SHIFT, 22,
		ufspll_con[1], 24, ufspll_con[1], 0, pll_div_rate),
};

struct pll_reg_list {
	const char *name;
	void *div_reg;
	void *con0;
	void *con1;
};

struct fmeter_data {
	u8 id;
	void *pll_con0;
	void *pll_con1;
	void *con0;
	void *con1;
};

static const struct fmeter_data fmd[] = {
	[APLL1_CTRL] = {APLL1_CTRL, &mtk_apmixed->apll1_con[0]},
	[APLL2_CTRL] = {APLL2_CTRL, &mtk_apmixed->apll2_con[0]},
	[ARMPLL_BL_CTRL] = {ARMPLL_BL_CTRL, &mtk_apmixed->armpll_bl_con[0]},
	[ARMPLL_LL_CTRL] = {ARMPLL_LL_CTRL, &mtk_apmixed->armpll_ll_con[0]},
	[CCIPLL_CTRL] = {CCIPLL_CTRL, &mtk_apmixed->ccipll_con[0]},
	[MAINPLL_CTRL] = {MAINPLL_CTRL, &mtk_apmixed->mainpll_con[0]},
	[MMPLL_CTRL] = {MMPLL_CTRL, &mtk_apmixed->mmpll_con[0]},
	[MSDCPLL_CTRL] = {MSDCPLL_CTRL, &mtk_apmixed->msdcpll_con[0]},
	[UFSPLL_CTRL] = {UFSPLL_CTRL, &mtk_apmixed->ufspll_con[0]},
	[UNIVPLL_CTRL] = {UNIVPLL_CTRL, &mtk_apmixed->univpll_con[0]},
	[EMIPLL_CTRL] = {EMIPLL_CTRL, &mtk_apmixed->emipll_con[0]},
	[TVDPLL1_CTRL] = {TVDPLL1_CTRL, &mtk_apmixed->tvdpll1_con[0]},
	[TVDPLL2_CTRL] = {TVDPLL2_CTRL, &mtk_apmixed->tvdpll2_con[0]},
	[MFGPLL_CTRL] = {MFGPLL_CTRL, &mtk_apmixed->mfgpll_con[0]},
	[ETHPLL_CTRL] = {ETHPLL_CTRL, &mtk_apmixed->ethpll_con[0]},
	[APUPLL_CTRL] = {APUPLL_CTRL, &mtk_apmixed->apupll_con[0]},
	[APUPLL2_CTRL] = {APUPLL2_CTRL, &mtk_apmixed->apupll2_con[0]},
	[VLP_CKSYS_TOP_CTRL] = {VLP_CKSYS_TOP_CTRL, 0, 0,
				&mtk_vlpsys->vlp_fqmtr_con[0],
				&mtk_vlpsys->vlp_fqmtr_con[1]},
};

static u32 mt_get_subsys_freq(const struct fmeter_data fm_data, u32 id)
{
	u32 output = 0, ckdiv_en = 0;
	u32 temp;

	if (fm_data.pll_con0 != 0) {
		/* check ckdiv_en */
		if (read32(fm_data.pll_con0) & 0x00010000)
			ckdiv_en = 1;
		/* pll con0[19] = 1, pll con0[16] = 1, pll con0[12] = 1 */
		/* select pll_ckdiv, enable pll_ckdiv, enable test clk */
		setbits32(fm_data.pll_con0, BIT(19) | BIT(16) | BIT(12));
	}

	/* CLK26CALI_0[15]: rst 1 -> 0 */
	write32(fm_data.con0, 0);
	/* CLK26CALI_0[15]: rst 0 -> 1 */
	setbits32(fm_data.con0, BIT(15));

	/* vlp freq meter sel ckgen[20:16] */
	/* other subsys freq meter sel ckgen[2:0] */
	if (fm_data.id == VLP_CKSYS_TOP_CTRL)
		clrsetbits32(fm_data.con0, GENMASK(20, 16), id << 16);
	else
		clrsetbits32(fm_data.con0, GENMASK(2, 0), id);

	clrsetbits32(fm_data.con1, GENMASK(31, 16), 0x1FF << 16);

	/* select divider?dvt set zero */
	clrbits32(fm_data.con0, GENMASK(31, 24));

	setbits32(fm_data.con0, BIT(12));
	setbits32(fm_data.con0, BIT(4));
	/* fmeter con0[1:0] = 0 */
	/* choose test clk */
	clrbits32(fm_data.con0, BIT(1) | BIT(0));

	/* wait frequency meter finish */
	if (fm_data.id == VLP_CKSYS_TOP_CTRL)
		udelay(VLP_FM_WAIT_TIME);
	else
		wait_us(1000, !(read32(fm_data.con0) & BIT(4)));

	temp = read32(fm_data.con1) & 0xFFFF;
	output = ((temp * 26000)) / 512; /* Khz */

	if (fm_data.pll_con0 != 0) {
		/* pll con0[19] = 0, pll con0[12] = 0 */
		if (ckdiv_en)
			clrbits32(fm_data.pll_con0, BIT(19) | BIT(12));
		else
			clrbits32(fm_data.pll_con0, BIT(19) | BIT(16) | BIT(12));
	}

	write32(fm_data.con0, 0x8000);

	printk(BIOS_INFO, "subsys meter[%d:%d] = %d Khz\n", fm_data.id, id, output);

	return output;
}

u32 mt_get_vlpck_freq(u32 id)
{
	return mt_get_subsys_freq(fmd[VLP_CKSYS_TOP_CTRL], id);
}

void mt_set_topck_default(void)
{
	int i;

	write32(&mtk_topckgen->clk_cfg[0].clr, 0xFFFFFFF0);
	for (i = 1; i <= 16; i++)
		write32(&mtk_topckgen->clk_cfg[i].clr, 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_cfg_17.clr, 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_cfg_18.clr, 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_cfg_19.clr, 0xFFFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[0], 0x7FFFFFFE);
	write32(&mtk_topckgen->clk_cfg_update[1], 0x7FFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[2], 0x0000FFFF);
}

void pll_set_pcw_change(const struct pll *pll)
{
	setbits32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	printk(BIOS_INFO, "Pll init start...\n");

	spm_power_on();

	printk(BIOS_INFO, "pll control start...\n");
	/* [0]=1 (CLKSQ_EN) + Default Value to avoid using BootROM's setting */
	setbits32(&mtk_apmixed->ap_clksq_con0, BIT(0));

	/* Wait 100us */
	udelay(100);

	/* [2]=1 (CLKSQ_LPF_EN) */
	setbits32(&mtk_apmixed->ap_clksq_con0, BIT(2));

	for (i = 0; i < ARRAY_SIZE(pll_rates); i++)
		pll_set_rate(&plls[pll_rates[i].id], pll_rates[i].rate);

	write32(&mtk_apmixed->apll1_tuner_con0, 0x6F28BD4D);
	write32(&mtk_apmixed->apll2_tuner_con0, 0x78FD5266);
	setbits32(&mtk_apmixed->emipll_con[0], BIT(8));
	setbits32(&mtk_apmixed->mfgpll_con[0], BIT(8));

	/* PLL all enable */
	write32(&mtk_apmixed->pllen_all_set, 0x0007FFFC);

	/* Wait PLL stable (20us) */
	udelay(20);

	/* turn on pll div en */
	setbits32(&mtk_apmixed->mainpll_con[0], GENMASK(31, 24));
	setbits32(&mtk_apmixed->univpll_con[0], GENMASK(31, 24));
	udelay(20);

	/* pll all rstb */
	write32(&mtk_apmixed->pll_div_rstb_all_set, 0x00000007);

	printk(BIOS_INFO, "pll control done...\n");

	/* [10:9] muxsel: switch to PLL speed */
	clrsetbits32(&mtk_cpu_plldiv_cfg->cpu_plldiv_0_cfg0, GENMASK(10, 9), BIT(9));

	/* [10:9] muxsel: switch to PLL speed */
	clrsetbits32(&mtk_cpu_plldiv_cfg->cpu_plldiv_1_cfg0, GENMASK(10, 9), BIT(9));

	/* [10:9] muxsel: switch to PLL speed */
	clrsetbits32(&mtk_bus_plldiv_cfg->bus_plldiv_cfg0, GENMASK(10, 9), BIT(9));

	/* Infra DCM divider */
	setbits32(&mtk_infra_ao_bcrm->vdnr_dcm_infra_par_bus_ctrl_0, BIT(6) | BIT(3));

	/* Peri DCM divider */
	setbits32(&mtk_peri_ao_bcrm->vdnr_dcm_peri_par_bus_ctrl_0, BIT(10) | BIT(7) | BIT(4));

	printk(BIOS_INFO, "mux switch control start...\n");

	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		pll_mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* update mux */
	write32(&mtk_topckgen->clk_cfg_update[0], 0x7FFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[1], 0x7FFFFFFF);
	write32(&mtk_topckgen->clk_cfg_update[2], 0x0000FFFF);

	/* femisys_dvfs_ck_gfmux_sel = 1(emipll_ck) */
	write32(&mtk_topckgen->clk_mem_dfs_cfg, 0x1);

	for (i = 0; i < ARRAY_SIZE(vlp_mux_sels); i++)
		pll_mux_set_sel(&vlp_muxes[vlp_mux_sels[i].id], vlp_mux_sels[i].sel);

	/* update mux */
	write32(&mtk_vlpsys->vlp_clk_cfg_update, 0x00FFFFFF);

	printk(BIOS_INFO, "mux switch control done...\n");

	printk(BIOS_INFO, "Pll init Done!!\n");
}

void mt_pll_post_init(void)
{
	/* for CG */
	printk(BIOS_INFO, "subsysCG enable start...\n");

	/* TOPCKGEN CG Clear */
	write32(&mtk_topckgen->clk_misc_cfg_3.clr, 0x00010000);
	write32(&mtk_topckgen->clk_misc_cfg_3.set, 0xDF3DFCFF);
	/* INFRACFG_AO CG Clear */
	write32(&mtk_infracfg_ao->infracfg_ao_module_cg_0_clr, 0x10000000);
	write32(&mtk_infracfg_ao->infracfg_ao_module_cg_1_clr, 0x21000000);
	write32(&mtk_infracfg_ao->infracfg_ao_module_cg_2_clr, 0x08000000);
	write32(&mtk_infracfg_ao->infracfg_ao_module_cg_3_clr, 0x02000000);
	/* PERICFG_AO CG Clear */
	write32(&mtk_pericfg_ao->pericfg_ao_peri_cg_0_clr, 0x3FFFFFFF);
	write32(&mtk_pericfg_ao->pericfg_ao_peri_cg_1_clr, 0x3DBDFBF6);
	write32(&mtk_pericfg_ao->pericfg_ao_peri_cg_2_clr, 0x0FFFFFFB);
	/* UFSCFG_AO_REG CG Clear */
	write32(&mtk_ufscfg_ao->ufscfg_ao_reg_ufs_ao_cg_0_clr, 0x0000007F);
	/* IMP_IIC_WRAP_WS CG Clear */
	write32(&mtk_imp_iic_wrap_ws->imp_iic_wrap_ws_ap_clock_cg_clr, 0x00000001);
	/* IMP_IIC_WRAP_E CG Clear */
	write32(&mtk_imp_iic_wrap_e->imp_iic_wrap_e_ap_clock_cg_clr, 0x00000003);
	/* IMP_IIC_WRAP_S CG Clear */
	write32(&mtk_imp_iic_wrap_s->imp_iic_wrap_s_ap_clock_cg_clr, 0x0000000F);
	/* IMP_IIC_WRAP_EN CG Clear */
	write32(&mtk_imp_iic_wrap_en->imp_iic_wrap_en_ap_clock_cg_clr, 0x00000003);
	/* VLP_CK CG Clear */
	write32(&mtk_vlpsys->vlp_clk_cfg_30_set, 0x00000832);
	/* SCP_IIC CG Clear */
	write32(&mtk_scp_iic->scp_iic_ccu_clock_cg_set, 0x00000003);
	/* SCP CG Clear */
	setbits32(&mtk_scp->scp_ap_spi_cg, BIT(1) | BIT(0));
	/* VLPCFG_AO_REG CG Clear */
	clrbits32(&mtk_vlpcfg_ao->vlpcfg_ao_reg_debugtop_vlpao_ctrl, BIT(8));
	/* VLPCFG_REG CG Clear */
	setbits32(&mtk_vlpcfg->vlp_test_ck_ctrl,
		  BIT(28) | BIT(24) | BIT(23) | BIT(22) | BIT(21) | BIT(20) |
		  BIT(18) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) |
		  BIT(8) | BIT(7) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
	/* DVFSRC_TOP CG Clear */
	setbits32(&mtk_dvfsrc_top->dvfsrc_top_dvfsrc_basic_control, BIT(0));

	/* DBGAO CG Clear */
	setbits32(&mtk_dbgao->dbgao_atb, BIT(0));
	/* DEM CG Clear*/
	setbits32(&mtk_dem->dem_atb, BIT(0));
	setbits32(&mtk_dem->dem_dbgbusclk_en, BIT(0));
	setbits32(&mtk_dem->dem_dbgsysclk_en, BIT(0));
	/* UFSCFG_PDN_REG CG Clear */
	write32(&mtk_ufscfg_pdn->ufscfg_pdn_reg_ufs_pdn_cg_0_clr, 0x0000002B);

	/* MMINFRA_CONFIG CG Clear */
	write32(&mtk_mminfra_config->mminfra_config_mminfra_cg_0_clr, 0x00000007);
	write32(&mtk_mminfra_config->mminfra_config_mminfra_cg_1_clr, 0x00020000);
	/* GCE CG Clear */
	clrbits32(&mtk_gce_d->gce_gce_ctl_int0, BIT(16));

	printk(BIOS_INFO, "subsysCG enable done...\n");
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_cpu_plldiv_cfg->cpu_plldiv_0_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable armpll_ll frequency output */
	write32(&mtk_apmixed->pllen_all_clr, 0x40000);

	/* raise armpll_ll frequency */
	pll_set_rate(&plls[CLK_APMIXED_ARMPLL_LL], freq);

	/* enable armpll_ll frequency output */
	write32(&mtk_apmixed->pllen_all_set, 0x40000);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to armpll_ll */
	clrsetbits32(&mtk_cpu_plldiv_cfg->cpu_plldiv_0_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_raise_cci_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_bus_plldiv_cfg->bus_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable ccipll frequency output */
	write32(&mtk_apmixed->pllen_all_clr, 0x10000);

	/* raise ccipll frequency */
	pll_set_rate(&plls[CLK_APMIXED_CCIPLL], freq);

	/* enable ccipll frequency output */
	write32(&mtk_apmixed->pllen_all_set, 0x10000);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to ccipll */
	clrsetbits32(&mtk_bus_plldiv_cfg->bus_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_set_tvd_pll1_freq(u32 freq)
{
	/* disable tvdpll frequency output */
	write32(&mtk_apmixed->pllen_all_clr, 0x200);

	/* set tvdpll frequency */
	pll_set_rate(&plls[CLK_APMIXED_TVDPLL2], freq);

	/* enable tvdpll frequency output */
	write32(&mtk_apmixed->pllen_all_set, 0x200);
	udelay(PLL_EN_DELAY);
}

void mt_pll_edp_mux_set_sel(u32 sel)
{
	pll_mux_set_sel(&muxes[CLK_TOP_EDP_SEL], sel);
}
