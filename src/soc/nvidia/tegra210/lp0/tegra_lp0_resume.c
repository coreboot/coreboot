/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 * Copyright 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <compiler.h>

/* Function unit addresses. */
enum {
	UP_TAG_BASE = 0x60000000,
	TIMER_BASE = 0x60005000,
	CLK_RST_BASE = 0x60006000,
	FLOW_CTLR_BASE = 0x60007000,
	SECURE_BOOT_BASE = 0x6000C200,
	TEGRA_EVP_BASE = 0x6000f000,
	APB_MISC_BASE = 0x70000000,
	PINMUX_BASE = 0x70003000,
	PMC_CTLR_BASE = 0x7000e400,
	MC_CTLR_BASE = 0x70019000,
	FUSE_BASE = 0x7000F800,
	TEGRA_SDMMC1_BASE = 0x700b0000,
	TEGRA_SDMMC3_BASE = 0x700b0400,
	EMC_BASE = 0x7001B000,
	I2C5_BASE = 0x7000D000,
	I2S_BASE = 0x702d1000
};

/* UP tag registers. */
static uint32_t *up_tag_ptr = (void *)(UP_TAG_BASE + 0x0);
enum {
	UP_TAG_AVP = 0xaaaaaaaa
};


/*  APB Misc JTAG Configuration Register */
static uint32_t *misc_pp_config_ctl_ptr = (void *)(APB_MISC_BASE + 0x24);
enum {
	PP_CONFIG_CTL_TBE = 0x1 << 7,
	PP_CONFIG_CTL_JTAG = 0x1 << 6
};

static uint32_t *misc_gp_asdbgreg_ptr = (void *)(APB_MISC_BASE + 0x810);
enum {
	CFG2TMC_RAM_SVOP_PDP_MASK = 0x3 << 24,
	CFG2TMC_RAM_SVOP_PDP_VAL_2 = 0x2 << 24,
};


/* PINMUX registers. */
static uint32_t *pinmux_pwr_i2c_scl_ptr = (void *)(PINMUX_BASE + 0xdc);
static uint32_t *pinmux_pwr_i2c_sda_ptr = (void *)(PINMUX_BASE + 0xe0);
static uint32_t *pinmux_dvfs_pwm_ptr = (void *)(PINMUX_BASE + 0x184);
static uint32_t *pinmux_gpio_pa6_ptr = (void *)(PINMUX_BASE + 0x244);
enum {
	E_INPUT = 1 << 6,
	TRISTATE = 1 << 4,
	PM_CLDVFS = 1,
	PM_I2CPMU = 0
};

/* Timer registers. */
static uint32_t *timer_us_ptr = (void *)(TIMER_BASE + 0x10);
static uint32_t *timer_us_cfg_ptr = (void *)(TIMER_BASE + 0x14);


/* Clock and reset controller registers. */
static uint32_t *clk_rst_rst_devices_l_ptr = (void *)(CLK_RST_BASE + 0x4);
enum {
	SWR_TRIG_SYS_RST = 0x1 << 2
};

static uint32_t *clk_rst_rst_devices_u_ptr = (void *)(CLK_RST_BASE + 0xc);

static uint32_t *clk_rst_cclkg_burst_policy_ptr = (void *)(CLK_RST_BASE + 0x368);
enum {
	CCLKG_PLLP_BURST_POLICY = 0x20004444
};

static uint32_t *clk_rst_cclklp_burst_policy_ptr = (void *)(CLK_RST_BASE + 0x370);
enum {
	CCLKLP_PLLP_BURST_POLICY = 0x20004444
};

static uint32_t *clk_rst_super_cclkg_div_ptr = (void *)(CLK_RST_BASE + 0x36c);
static uint32_t *clk_rst_super_cclklp_div_ptr = (void *)(CLK_RST_BASE + 0x374);
enum {
	SUPER_CDIV_ENB = 0x1 << 31
};

static uint32_t *clk_rst_osc_ctrl_ptr = (void *)(CLK_RST_BASE + 0x50);
enum {
	OSC_XOE = 0x1 << 0,
	OSC_XOFS_SHIFT = 4,
	OSC_XOFS_MASK = 0x3f << OSC_XOFS_SHIFT,
	OSC_FREQ_SHIFT = 28,
	OSC_FREQ_MASK = 0xf << OSC_FREQ_SHIFT
};

static uint32_t *clk_rst_pllx_base_ptr = (void *)(CLK_RST_BASE + 0xe0);
enum {
	PLLX_ENABLE = 0x1 << 30
};

static uint32_t *clk_rst_clk_source_i2c5_ptr = (void *)(CLK_RST_BASE + 0x128);
enum {
	I2C5_CLK_DIVISOR = 4
};

static uint32_t *clk_rst_rst_dev_h_set_ptr = (void *)(CLK_RST_BASE + 0x308);
enum {
	I2C5_RST = 0x1 << 15
};

static uint32_t *clk_rst_rst_dev_h_clr_ptr = (void *)(CLK_RST_BASE + 0x30c);

static uint32_t *clk_rst_rst_dev_u_clr_ptr = (void *)(CLK_RST_BASE + 0x314);
enum {
	SWR_CSITE_RST = 0x1 << 9
};

static uint32_t *clk_rst_rst_dev_v_clr_ptr = (void *)(CLK_RST_BASE + 0x434);
enum {
	MSELECT_RST = 0x1 << 3
};

static uint32_t *clk_rst_clk_enb_l_set_ptr = (void *)(CLK_RST_BASE + 0x320);
enum {
	CLK_ENB_CPU = 0x1 << 0
};

static uint32_t *clk_rst_clk_enb_h_set_ptr = (void *)(CLK_RST_BASE + 0x328);
enum {
	CLK_ENB_I2C5 = 0x1 << 15
};

static uint32_t *clk_rst_clk_out_enb_u_set_ptr =
	(void *)(CLK_RST_BASE + 0x330);
enum {
	CLK_ENB_CSITE = 0x1 << 9
};

static uint32_t *clk_rst_cpu_softrst_ctrl2_ptr =
	(void *)(CLK_RST_BASE + 0x388);
enum {
	CAR2PMC_CPU_ACK_WIDTH_SHIFT = 0,
	CAR2PMC_CPU_ACK_WIDTH_MASK = 0xfff << CAR2PMC_CPU_ACK_WIDTH_SHIFT
};

static uint32_t *clk_rst_clk_enb_v_set_ptr = (void *)(CLK_RST_BASE + 0x440);
enum {
	CLK_ENB_CPUG = 0x1 << 0,
};

static uint32_t *clk_rst_clk_enb_y_set_ptr = (void *)(CLK_RST_BASE + 0x29c);
enum {
	CLK_ENB_PLLP_OUT_CPU = 0x1 << 31
};

static uint32_t *clk_rst_cpug_cmplx_clr_ptr =
	(void *)(CLK_RST_BASE + 0x454);
enum {
	CLR_CPURESET0 = 0x1 << 0,
	CLR_CPURESET1 = 0x1 << 1,
	CLR_CPURESET2 = 0x1 << 2,
	CLR_CPURESET3 = 0x1 << 3,
	CLR_DBGRESET0 = 0x1 << 12,
	CLR_DBGRESET1 = 0x1 << 13,
	CLR_DBGRESET2 = 0x1 << 14,
	CLR_DBGRESET3 = 0x1 << 15,
	CLR_CORERESET0 = 0x1 << 16,
	CLR_CORERESET1 = 0x1 << 17,
	CLR_CORERESET2 = 0x1 << 18,
	CLR_CORERESET3 = 0x1 << 19,
	CLR_CXRESET0 = 0x1 << 20,
	CLR_CXRESET1 = 0x1 << 21,
	CLR_CXRESET2 = 0x1 << 22,
	CLR_CXRESET3 = 0x1 << 23,
	CLR_L2RESET = 0x1 << 24,
	CLR_NONCPURESET = 0x1 << 29,
	CLR_PRESETDBG = 0x1 << 30
};

static uint32_t *clk_rst_spare_reg0_ptr =
	(void *)(CLK_RST_BASE + 0x55c);
enum {
	CLK_M_DIVISOR_MASK = 0x3 << 2,
	CLK_M_DIVISOR_BY_2 = 0x1 << 2
};

static uint32_t *clk_rst_lvl2_clk_gate_ovra_ptr = (void *)(CLK_RST_BASE + 0xf8);
static uint32_t *clk_rst_lvl2_clk_gate_ovrb_ptr = (void *)(CLK_RST_BASE + 0xfc);
static uint32_t *clk_rst_lvl2_clk_gate_ovrc_ptr = (void *)(CLK_RST_BASE + 0x3a0);
static uint32_t *clk_rst_lvl2_clk_gate_ovrd_ptr = (void *)(CLK_RST_BASE + 0x3a4);
static uint32_t *clk_rst_lvl2_clk_gate_ovre_ptr = (void *)(CLK_RST_BASE + 0x554);

static uint32_t *clk_rst_clk_out_enb_l_ptr = (void *)(CLK_RST_BASE + 0x10);
static uint32_t *clk_rst_clk_out_enb_h_ptr = (void *)(CLK_RST_BASE + 0x14);
static uint32_t *clk_rst_clk_out_enb_u_ptr = (void *)(CLK_RST_BASE + 0x18);
static uint32_t *clk_rst_clk_out_enb_v_ptr = (void *)(CLK_RST_BASE + 0x360);
static uint32_t *clk_rst_clk_out_enb_w_ptr = (void *)(CLK_RST_BASE + 0x364);
static uint32_t *clk_rst_clk_out_enb_x_ptr = (void *)(CLK_RST_BASE + 0x280);
static uint32_t *clk_rst_clk_out_enb_y_ptr = (void *)(CLK_RST_BASE + 0x298);

static uint32_t *clk_rst_clk_enb_l_clr_ptr = (void *)(CLK_RST_BASE + 0x324);
static uint32_t *clk_rst_clk_enb_h_clr_ptr = (void *)(CLK_RST_BASE + 0x32c);
static uint32_t *clk_rst_clk_enb_u_clr_ptr = (void *)(CLK_RST_BASE + 0x334);
static uint32_t *clk_rst_clk_enb_v_clr_ptr = (void *)(CLK_RST_BASE + 0x444);
static uint32_t *clk_rst_clk_enb_w_clr_ptr = (void *)(CLK_RST_BASE + 0x44c);
static uint32_t *clk_rst_clk_enb_x_clr_ptr = (void *)(CLK_RST_BASE + 0x288);
static uint32_t *clk_rst_clk_enb_y_clr_ptr = (void *)(CLK_RST_BASE + 0x2a0);

#define MBIST_CLK_ENB_L_0	0x80000130
#define MBIST_CLK_ENB_H_0	0x020000C1
#define MBIST_CLK_ENB_U_0	0x01F00200
#define MBIST_CLK_ENB_V_0	0x80400008
#define MBIST_CLK_ENB_W_0	0x002000FC
#define MBIST_CLK_ENB_X_0	0x23004780
#define MBIST_CLK_ENB_Y_0	0x00000300

static uint32_t *clk_rst_clk_enb_v_ptr = (void *)(CLK_RST_BASE + 0x440);
enum {
	CLK_ENB_MSELECT = 0x1 << 3
};

static uint32_t *clk_rst_clk_enb_w_set_ptr = (void *)(CLK_RST_BASE + 0x448);
enum {
	CLK_ENB_MC1 = 0x1 << 30,
	CLK_ENB_DVFS = 0x1 << 27
};

static uint32_t *clk_rst_clk_source_mselect_ptr = (void *)(CLK_RST_BASE + 0x3b4);
enum {
	CLK_SRC_PLLP_OUT0 = (0x0 << 29),
	MSELECT_CLK_DIVISOR_4 = 6
};

static uint32_t *clk_rst_clk_dvfs_ref_ptr = (void *)(CLK_RST_BASE + 0x62c);
enum {
	DVFS_REF_CLK_DIVISOR = 0xe
};

static uint32_t *clk_rst_clk_dvfs_soc_ptr = (void *)(CLK_RST_BASE + 0x630);
enum {
	DVFS_SOC_CLK_DIVISOR = 0xe
};

/* Flow controller registers. */
static uint32_t *flow_ctlr_halt_cop_events_ptr =
	(void *)(FLOW_CTLR_BASE + 0x4);
enum {
	EVENT_MSEC = 0x1 << 24,
	EVENT_JTAG = 0x1 << 28,
	FLOW_MODE_SHIFT = 29,
	FLOW_MODE_STOP = 2 << FLOW_MODE_SHIFT,
};

static uint32_t *flow_ctlr_ram_repair_ptr =
	(void *)(FLOW_CTLR_BASE + 0x40);
static uint32_t *flow_ctlr_ram_repair_cluster1_ptr =
	(void *)(FLOW_CTLR_BASE + 0x58);
enum {
	RAM_REPAIR_REQ = 0x1 << 0,
	RAM_REPAIR_STS = 0x1 << 1,
};

static uint32_t *flow_ctlr_bpmp_cluster_control_ptr =
	(void *)(FLOW_CTLR_BASE + 0x98);
enum {
	ACTIVE_SLOW = 0x1 << 0
};


/* Power management controller registers. */
enum {
	PARTID_CRAIL = 0,
	PARTID_CE1 = 9,
	PARTID_CE2 = 10,
	PARTID_CE3 = 11,
	PARTID_CE0 = 14,
	PARTID_C0NC = 15,
};

static uint32_t *pmc_dpd_sample_ptr = (void *)(PMC_CTLR_BASE + 0x20);
static uint32_t *pmc_clamp_status_ptr = (void *)(PMC_CTLR_BASE + 0x2c);

static uint32_t *pmc_pwrgate_toggle_ptr = (void *)(PMC_CTLR_BASE + 0x30);
enum {
	PWRGATE_TOGGLE_START = 0x1 << 8
};

static uint32_t *pmc_remove_clamping_cmd_ptr = (void *)(PMC_CTLR_BASE + 0x34);

static uint32_t *pmc_pwrgate_status_ptr = (void *)(PMC_CTLR_BASE + 0x38);

static uint32_t *pmc_cpupwrgood_timer_ptr = (void *)(PMC_CTLR_BASE + 0xc8);

static uint32_t *pmc_odmdata_ptr = (void *)(PMC_CTLR_BASE + 0xa0);

static uint32_t *pmc_scratch4_ptr = (void *)(PMC_CTLR_BASE + 0x60);
enum {
	PMC_WAKEUP_CLUSTER_LPCPU = 1 << 31
};

static uint32_t *pmc_scratch190_ptr = (void *)(PMC_CTLR_BASE + 0x818);

/* SCRATCH201 bit 1 is used to designate 77621 PMIC for CPU rail. */
static uint32_t *pmc_scratch201_ptr = (void *)(PMC_CTLR_BASE + 0x844);
enum {
	PMIC_77621 = 0x1 << 1
};

static uint32_t *pmc_secure_scratch34_ptr = (void *)(PMC_CTLR_BASE + 0x368);
static uint32_t *pmc_secure_scratch35_ptr = (void *)(PMC_CTLR_BASE + 0x36c);

static uint32_t *pmc_osc_edpd_over_ptr = (void *)(PMC_CTLR_BASE + 0x1a4);
enum {
	PMC_XOFS_SHIFT = 1,
	PMC_XOFS_MASK = 0x3f << PMC_XOFS_SHIFT
};

static uint32_t *pmc_sticky_bits_ptr = (void *)(PMC_CTLR_BASE + 0x2c0);
enum {
	HDA_LPBK_DIS = 1 << 0,
};

static uint32_t *pmc_set_sw_clamp_ptr = (void *)(PMC_CTLR_BASE + 0x47c);

/* Memory controller registers. */
static uint32_t *mc_intstatus_ptr = (void *)(MC_CTLR_BASE);
static uint32_t *mc_intmask_ptr = (void *)(MC_CTLR_BASE + 0x4);
static uint32_t *mc_video_protect_size_mb_ptr = (void *)(MC_CTLR_BASE + 0x64c);

static uint32_t *mc_video_protect_reg_ctrl_ptr =
	(void *)(MC_CTLR_BASE + 0x650);
enum {
	VPR_WR_ACCESS_DISABLE = 0x1 << 0,
	VPR_ALLOW_TZ_WR_ACCESS = 0x1 << 1
};
/* FUSE registers */
static uint32_t *fuse_security_mode_ptr = (void *)(FUSE_BASE + 0x1a0);
enum {
	SECURITY_MODE = 0x1 << 0
};

/* SECURE_BOOT registers */
static uint32_t *sb_pfcfg_ptr = (void *)(SECURE_BOOT_BASE + 0x8);
enum {
	SECURE_BOOT_DEBUG_CONFIG = 0x1 << 3
};

static uint32_t *sb_aa64_reset_low = (void *)(SECURE_BOOT_BASE + 0x30);
static uint32_t *sb_aa64_reset_high = (void *)(SECURE_BOOT_BASE + 0x34);


/* EMC registers */
static uint32_t *pmacro_cfg_pm_global = (void *)(EMC_BASE + 0xc30);
enum {
	DISABLE_CFG_BYTE0 = 0x1 << 16,
	DISABLE_CFG_BYTE1 = 0x1 << 17,
	DISABLE_CFG_BYTE2 = 0x1 << 18,
	DISABLE_CFG_BYTE3 = 0x1 << 19,
	DISABLE_CFG_BYTE4 = 0x1 << 20,
	DISABLE_CFG_BYTE5 = 0x1 << 21,
	DISABLE_CFG_BYTE6 = 0x1 << 22,
	DISABLE_CFG_BYTE7 = 0x1 << 23,
	DISABLE_CFG_BYTES = 0xff << 16,
	ENABLE_CFG_BYTES = 0 << 16,
	DISABLE_CFG_CMD0 = 0x1 << 24,
	DISABLE_CFG_CMD1 = 0x1 << 25,
	DISABLE_CFG_CMD2 = 0x1 << 26,
	DISABLE_CFG_CMD3 = 0x1 << 27,
};

static uint32_t *pmacro_training_ctrl_0_ptr = (void *)(EMC_BASE + 0xcf8);
static uint32_t *pmacro_training_ctrl_1_ptr = (void *)(EMC_BASE + 0xcfc);
enum {
	TRAINING_E_WRPTR = 0x1 << 3
};

static uint32_t *fbio_cfg7_ptr = (void *)(EMC_BASE + 0x584);
enum {
	CH1_ENABLE = 0x1 << 2
};

/* I2C5 registers */
static uint32_t *i2c5_cnfg_ptr = (void *)(I2C5_BASE + 0x0);
enum {
	I2C_DEBOUNCE_CNT_4 = 2 << 12,
	I2C_NEW_MASTER_FSM = 1 << 11,
	I2C_SEND = 1 << 9,
	I2C_LENGTH_2_BYTES = 1 << 1
};

static uint32_t *i2c5_cmd_addr0_ptr = (void *)(I2C5_BASE + 0x4);
static uint32_t *i2c5_cmd_data1_ptr = (void *)(I2C5_BASE + 0xc);
static uint32_t *i2c5_status_ptr = (void *)(I2C5_BASE + 0x1c);
enum {
	I2C_STATUS_BUSY = 1 << 8,
	I2C_STATUS_CMD1_STAT_MASK = 0xf << 0,
	I2C_STATUS_CMD1_XFER_SUCCESS = 0 << 0
};

static uint32_t *i2c5_config_load_ptr = (void *)(I2C5_BASE + 0x8c);
enum {
	MSTR_CONFIG_LOAD = 1 << 0
};

#define MAX77620_I2C_ADDR	(0x3c << 1)
#define MAX77620_GPIO5_DATA	(0x3b | (0x9 << 8))

#define MAX77621_I2C_ADDR	(0x1b << 1)
#define MAX77621_VOUT_REG	0x0
#define MAX77621_VOUT_VAL	(0x80 | 0x27)
#define MAX77621_VOUT_DATA	(MAX77621_VOUT_REG | (MAX77621_VOUT_VAL << 8))


/* Utility functions. */

static __always_inline void __noreturn halt(void)
{
	for (;;);
}

static inline uint32_t read32(const void *addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void write32(void *addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

static inline void setbits32(uint32_t bits, void *addr)
{
	write32(addr, read32(addr) | bits);
}

static inline void clrbits32(uint32_t bits, void *addr)
{
	write32(addr, read32(addr) & ~bits);
}

static void __noreturn reset(void)
{
	write32(clk_rst_rst_devices_l_ptr, SWR_TRIG_SYS_RST);
	halt();
}

static void udelay(unsigned usecs)
{
	uint32_t start = read32(timer_us_ptr);
	while (read32(timer_us_ptr) - start < usecs)
		;
}

/* UART related defines */
static uint32_t *uart_clk_out_enb_regs[4] = {
	(uint32_t *)0x60006010,
	(uint32_t *)0x60006010,
	(uint32_t *)0x60006014,
	(uint32_t *)0x60006018
};

static uint32_t *uart_rst_devices_regs[4] = {
	(uint32_t *)0x60006004,
	(uint32_t *)0x60006004,
	(uint32_t *)0x60006008,
	(uint32_t *)0x6000600c
};

static uint32_t uart_enable_mask[4] = {
	1 << 6,
	1 << 7,
	1 << 23,
	1 << 1
};

static uint32_t *uart_clk_source_regs[4] = {
	(uint32_t *)0x60006178,
	(uint32_t *)0x6000617c,
	(uint32_t *)0x600061a0,
	(uint32_t *)0x600061c0
};

static uint32_t *uart_base_regs[4] = {
	(uint32_t *)0x70006000,
	(uint32_t *)0x70006040,
	(uint32_t *)0x70006200,
	(uint32_t *)0x70006300
};

enum {
	UART_THR_DLAB = 0x0,
	UART_IER_DLAB = 0x1,
	UART_IIR_FCR = 0x2,
	UART_LCR = 0x3
};
enum {
	UART_RATE_115200 = (408000000/115200/16), /* based on 408000000 PLLP */
	FCR_TX_CLR = 0x4,	/* bit 2 of FCR : clear TX FIFO */
	FCR_RX_CLR = 0x2,	/* bit 1 of FCR : clear RX FIFO */
	FCR_EN_FIFO = 0x1,	/* bit 0 of FCR : enable TX & RX FIFO */
	LCR_DLAB = 0x80,	/* bit 7 of LCR : Divisor Latch Access Bit */
	LCR_WD_SIZE_8 = 0x3	/* bit 1:0 of LCR : word length of 8 */
};

static void enable_uart(void)
{
	uint32_t *uart_clk_enb_reg;
	uint32_t *uart_rst_reg;
	uint32_t *uart_clk_source;
	uint32_t uart_port;
	uint32_t uart_mask;
	uint32_t *uart_base;

	/*
	 * Read odmdata (stored in pmc->odmdata) to determine debug uart port.
	 *
	 * Bits 15-17 of odmdata contains debug uart port.
	 *  0 : UARTA
	 *  1 : UARTB
	 *  2 : UARTC
	 *  3 : UARTD
	 */
	uart_port = (read32(pmc_odmdata_ptr) >> 15) & 0x7;

	/* Default to UARTA if uart_port is out of range */
	if (uart_port >= 4)
		uart_port = 0;

	uart_clk_enb_reg = uart_clk_out_enb_regs[uart_port];
	uart_rst_reg = uart_rst_devices_regs[uart_port];
	uart_mask = uart_enable_mask[uart_port];
	uart_clk_source = uart_clk_source_regs[uart_port];
	uart_base = uart_base_regs[uart_port];

	/* Enable UART clock */
	setbits32(uart_mask, uart_clk_enb_reg);

	/* Reset and unreset UART */
	setbits32(uart_mask, uart_rst_reg);
	clrbits32(uart_mask, uart_rst_reg);

	/* Program UART clock source: PLLP (408000000) */
	write32(uart_clk_source, 0);

	/* Program 115200n8 to the uart port */
	/* baud-rate of 115200 */
	write32((uart_base + UART_LCR), LCR_DLAB);
	write32((uart_base + UART_THR_DLAB), (UART_RATE_115200 & 0xff));
	write32((uart_base + UART_IER_DLAB), (UART_RATE_115200 >> 8));
	/* 8-bit and no parity */
	write32((uart_base + UART_LCR), LCR_WD_SIZE_8);
	/* enable and clear RX/TX FIFO */
	write32((uart_base + UART_IIR_FCR),
		(FCR_TX_CLR + FCR_RX_CLR + FCR_EN_FIFO));
}

/* Accessors. */

/* Jtag configuration. */

static void enable_jtag(void)
{
	write32(misc_pp_config_ctl_ptr, PP_CONFIG_CTL_JTAG);
}

/* Clock configuration. */

static void config_oscillator(void)
{
	/*
	 * Read oscillator drive strength from OSC_EDPD_OVER.XOFS and copy
	 * to OSC_CTRL.XOFS and set XOE.
	 */
	uint32_t xofs = (read32(pmc_osc_edpd_over_ptr) &
		    PMC_XOFS_MASK) >> PMC_XOFS_SHIFT;

	uint32_t osc_ctrl = read32(clk_rst_osc_ctrl_ptr);
	osc_ctrl &= ~OSC_XOFS_MASK;
	osc_ctrl |= (xofs << OSC_XOFS_SHIFT);
	osc_ctrl |= OSC_XOE;
	write32(clk_rst_osc_ctrl_ptr, osc_ctrl);
}

static void enable_select_cpu_clocks(void)
{
	/* Enable the CPU complex clock. */
	write32(clk_rst_clk_enb_l_set_ptr, CLK_ENB_CPU);
	write32(clk_rst_clk_enb_v_set_ptr, CLK_ENB_CPUG);
	udelay(10);

	/* Select CPU complex clock source. */
	write32(clk_rst_cclkg_burst_policy_ptr, CCLKG_PLLP_BURST_POLICY);
	write32(clk_rst_cclklp_burst_policy_ptr, CCLKLP_PLLP_BURST_POLICY);
	udelay(10);
}


/* Function unit configuration. */

static void config_core_sight(void)
{
	/* Enable the CoreSight clock. */
	write32(clk_rst_clk_out_enb_u_set_ptr, CLK_ENB_CSITE);

	/*
	 * De-assert CoreSight reset.
	 * NOTE: We're leaving the CoreSight clock on the oscillator for
	 *       now. It will be restored to its original clock source
	 *       when the CPU-side restoration code runs.
	 */
	write32(clk_rst_rst_dev_u_clr_ptr, SWR_CSITE_RST);
}


/* RAM repair */

void ram_repair(void)
{
	/* Request Cluster0 RAM repair. */
	setbits32(RAM_REPAIR_REQ, flow_ctlr_ram_repair_ptr);
	/* Poll for Cluster0 RAM repair status. */
	while (!(read32(flow_ctlr_ram_repair_ptr) & RAM_REPAIR_STS))
		;
}


/* Power. */

static void power_on_partition(unsigned id)
{
	uint32_t bit = 0x1 << id;
	if (!(read32(pmc_pwrgate_status_ptr) & bit)) {
		/* Partition is not on. Turn it on. */
		write32(pmc_pwrgate_toggle_ptr, id | PWRGATE_TOGGLE_START);

		/* Wait until the partition is powerd on. */
		while (!(read32(pmc_pwrgate_status_ptr) & bit))
			;

		/* Wait until clamp is off. */
		while (read32(pmc_clamp_status_ptr) & bit)
			;
	}
}

static void config_hda_lpbk_dis(void)
{
	/* Set HDA_LPBK_DIS bit in APBDEV_PMC_STICKY_BITS_0 register */
	if (read32(fuse_security_mode_ptr) & SECURITY_MODE)
		setbits32(HDA_LPBK_DIS, pmc_sticky_bits_ptr);
}

static void set_gpio_pa6_input_mode(void)
{
	setbits32(E_INPUT, pinmux_gpio_pa6_ptr);
}

static void set_clk_m(void)
{
	uint32_t spare;

	/* set clk_m frequency to 19.2MHz: set divisor to 2. */
	spare = read32(clk_rst_spare_reg0_ptr);
	spare &= ~CLK_M_DIVISOR_MASK;
	spare |= CLK_M_DIVISOR_BY_2;
	write32(clk_rst_spare_reg0_ptr, spare);

	/*
	 * Restore TIMERUS config for 19.2MHz. (19.2 = 96/5 = 0x60/5)
	 * USEC_DIVIDEND(15:8) = 5-1; USEC_DIVISOR(7:0) = 0x60-1
	 */
	write32(timer_us_cfg_ptr, 0x045f);
}

static void restore_ram_svop(void)
{
	uint32_t asdbgreg;

	asdbgreg = read32(misc_gp_asdbgreg_ptr);
	asdbgreg &= ~CFG2TMC_RAM_SVOP_PDP_MASK;
	asdbgreg |= CFG2TMC_RAM_SVOP_PDP_VAL_2;
	write32(misc_gp_asdbgreg_ptr, asdbgreg);
}

static void set_pmacro_training_wrptr(void)
{
	/* disable writes to BYTES 7-0 of pad macro */
	write32(pmacro_cfg_pm_global, DISABLE_CFG_BYTES);

	/* Set E_WRPTR mode on Channel 0 and 1 */
	write32(pmacro_training_ctrl_0_ptr, TRAINING_E_WRPTR);
	write32(pmacro_training_ctrl_1_ptr, TRAINING_E_WRPTR);

	/* Re-enable writes to BYTE0-7 */
	write32(pmacro_cfg_pm_global, ENABLE_CFG_BYTES);
}

static uint32_t *i2s_0_master = (void *)(I2S_BASE + 0x0a0);
static uint32_t *i2s_1_master = (void *)(I2S_BASE + 0x1a0);
static uint32_t *i2s_2_master = (void *)(I2S_BASE + 0x2a0);
static uint32_t *i2s_3_master = (void *)(I2S_BASE + 0x3a0);
static uint32_t *i2s_4_master = (void *)(I2S_BASE + 0x4a0);

static uint32_t *i2s_0_slcg = (void *)(I2S_BASE + 0x088);
static uint32_t *i2s_1_slcg = (void *)(I2S_BASE + 0x188);
static uint32_t *i2s_2_slcg = (void *)(I2S_BASE + 0x288);
static uint32_t *i2s_3_slcg = (void *)(I2S_BASE + 0x388);
static uint32_t *i2s_4_slcg = (void *)(I2S_BASE + 0x488);

static uint32_t *clk_rst_ape_clear = (void *)(CLK_RST_BASE + 0x2ac);
static uint32_t *clk_rst_ape_set = (void *)(CLK_RST_BASE + 0x2a8);

static void mbist_workaround(void)
{
	uint32_t clks_to_be_cleared;
	uint32_t i2s_read;

	write32(clk_rst_ape_clear, 0x40);
	udelay(2);

	i2s_read = read32(i2s_0_master);
	i2s_read |= 0x400;
	write32(i2s_0_master, i2s_read);

	i2s_read = read32(i2s_0_slcg);
	i2s_read &= ~1;
	write32(i2s_0_slcg, i2s_read);

	i2s_read = read32(i2s_1_master);
	i2s_read |= 0x400;
	write32(i2s_1_master, i2s_read);

	i2s_read = read32(i2s_1_slcg);
	i2s_read &= ~1;
	write32(i2s_1_slcg, i2s_read);

	i2s_read = read32(i2s_2_master);
	i2s_read |= 0x400;
	write32(i2s_2_master, i2s_read);

	i2s_read = read32(i2s_2_slcg);
	i2s_read &= ~1;
	write32(i2s_2_slcg, i2s_read);

	i2s_read = read32(i2s_3_master);
	i2s_read |= 0x400;
	write32(i2s_3_master, i2s_read);

	i2s_read = read32(i2s_3_slcg);
	i2s_read &= ~1;
	write32(i2s_3_slcg, i2s_read);

	i2s_read = read32(i2s_4_master);
	i2s_read |= 0x400;
	write32(i2s_4_master, i2s_read);

	i2s_read = read32(i2s_4_slcg);
	i2s_read &= ~1;
	write32(i2s_4_slcg, i2s_read);

	udelay(2);

	write32(clk_rst_lvl2_clk_gate_ovra_ptr, 0);
	write32(clk_rst_lvl2_clk_gate_ovrb_ptr, 0);
	write32(clk_rst_lvl2_clk_gate_ovrc_ptr, 0x00000002);
	write32(clk_rst_lvl2_clk_gate_ovrd_ptr, 0x01000000);	/* QSPI OVR=1 */
	write32(clk_rst_lvl2_clk_gate_ovre_ptr, 0x00000c00);


	clks_to_be_cleared = read32(clk_rst_clk_out_enb_l_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_L_0;
	write32(clk_rst_clk_enb_l_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_h_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_H_0;
	write32(clk_rst_clk_enb_h_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_u_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_U_0;
	write32(clk_rst_clk_enb_u_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_v_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_V_0;
	write32(clk_rst_clk_enb_v_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_w_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_W_0;
	write32(clk_rst_clk_enb_w_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_x_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_X_0;
	write32(clk_rst_clk_enb_x_clr_ptr, clks_to_be_cleared);

	clks_to_be_cleared = read32(clk_rst_clk_out_enb_y_ptr);
	clks_to_be_cleared &= ~MBIST_CLK_ENB_Y_0;
	write32(clk_rst_clk_enb_y_clr_ptr, clks_to_be_cleared);

	if (read32(fbio_cfg7_ptr) & CH1_ENABLE)
		/* if Dual Channel enable MC1 clock */
		write32(clk_rst_clk_enb_w_set_ptr, CLK_ENB_MC1);
}

static uint32_t *sdmmc1_vendor_io_trim = (void *)(TEGRA_SDMMC1_BASE + 0x1ac);
static uint32_t *sdmmc3_vendor_io_trim = (void *)(TEGRA_SDMMC3_BASE + 0x1ac);
static uint32_t *sdmmc1_comppadctrl = (void *)(TEGRA_SDMMC1_BASE + 0x1e0);
static uint32_t *sdmmc3_comppadctrl = (void *)(TEGRA_SDMMC3_BASE + 0x1e0);

enum {
	SDMMC1_DEV_L = 0x1 << 14,
	SDMMC3_DEV_U = 0x1 << 5,
	PAD_E_INPUT_COMPPADCTRL = 0x1 << 31,
	SEL_VREG_VENDOR_IO_TRIM = 0x1 << 2
};

static void low_power_sdmmc_pads(void)
{
	/* Enable SDMMC1 clock */
	setbits32(SDMMC1_DEV_L, clk_rst_clk_out_enb_l_ptr);
	udelay(2);
	/* Unreset SDMMC1 */
	clrbits32(SDMMC1_DEV_L, clk_rst_rst_devices_l_ptr);

	/* Clear SEL_VREG bit and PAD_E_INPUT bit of SDMMC1 */
	clrbits32(SEL_VREG_VENDOR_IO_TRIM, sdmmc1_vendor_io_trim);
	clrbits32(PAD_E_INPUT_COMPPADCTRL, sdmmc1_comppadctrl);
	/* Read the last accessed SDMMC1 register then disable SDMMC1 clock */
	read32(sdmmc1_comppadctrl);
	/* Disable SDMMC1 clock, but keep SDMMC1 un-reset */
	clrbits32(SDMMC1_DEV_L, clk_rst_clk_out_enb_l_ptr);

	/* Enable SDMMC3 clock */
	setbits32(SDMMC3_DEV_U, clk_rst_clk_out_enb_u_ptr);
	udelay(2);
	/* Unreset SDMMC3 */
	clrbits32(SDMMC3_DEV_U, clk_rst_rst_devices_u_ptr);

	/* Clear SEL_VREG bit and PAD_E_INPUT bit of SDMMC3 */
	clrbits32(SEL_VREG_VENDOR_IO_TRIM, sdmmc3_vendor_io_trim);
	clrbits32(PAD_E_INPUT_COMPPADCTRL, sdmmc3_comppadctrl);
	/* Read the last accessed SDMMC3 register then disable SDMMC3 clock */
	read32(sdmmc3_comppadctrl);
	/* Disable SDMMC3 clock, but keep SDMMC3 un-reset */
	clrbits32(SDMMC3_DEV_U, clk_rst_clk_out_enb_u_ptr);
}

static void config_mselect(void)
{
	/* Set MSELECT clock source to PLL_P with 1:4 divider */
	write32(clk_rst_clk_source_mselect_ptr,
		(CLK_SRC_PLLP_OUT0 | MSELECT_CLK_DIVISOR_4));
	/* Enable clock to MSELECT */
	write32(clk_rst_clk_enb_v_ptr, CLK_ENB_MSELECT);
	/* Bring MSELECT out of reset, after 2 microsecond wait */
	udelay(2);
	write32(clk_rst_rst_dev_v_clr_ptr, MSELECT_RST);
}

/* Routine to do i2c send of 'data' to 'addr' */
static void i2c_send(uint32_t addr, uint32_t data)
{
	uint32_t delay;

	write32(i2c5_cmd_addr0_ptr, addr);
	write32(i2c5_cmd_data1_ptr, data);

	write32(i2c5_cnfg_ptr, (I2C_DEBOUNCE_CNT_4 | I2C_NEW_MASTER_FSM |
				I2C_LENGTH_2_BYTES));

	write32(i2c5_config_load_ptr, MSTR_CONFIG_LOAD);
	delay = 0;
	while (read32(i2c5_config_load_ptr) & MSTR_CONFIG_LOAD) {
		udelay(1);
		if (++delay > 100)
			reset();
	}

	write32(i2c5_cnfg_ptr, (I2C_DEBOUNCE_CNT_4 | I2C_NEW_MASTER_FSM |
				I2C_SEND | I2C_LENGTH_2_BYTES));

	/* Check busy */
	delay = 0;
	while (read32(i2c5_status_ptr) & I2C_STATUS_BUSY) {
		udelay(1);
		if (++delay > 1000)
			reset();
	}

	/* Check xfer successful; */
	if (read32(i2c5_status_ptr) & I2C_STATUS_CMD1_STAT_MASK)
		reset();
}

/* Entry point. */

void lp0_resume(void)
{
	uint32_t orig_timer;

	/* If not on the AVP, reset. */
	if (read32(up_tag_ptr) != UP_TAG_AVP)
		reset();

	/* Enable JTAG */
	enable_jtag();

	/* Set HDA_LPBK_DIS bit in APBDEV_PMC_STICKY_BITS_0 register */
	config_hda_lpbk_dis();

	/*
	 * From T210 TRM:
	 *   8.9.1.2 Deep Sleep Exit:
	 *     5.a: Set the E_INPUT bit of the PINMUX_AUX_GPIO_PA6_0 register
	 *          to Logic 1.
	 */
	set_gpio_pa6_input_mode();

	config_oscillator();

	/* set clk_m frequency to 19.2MHz */
	set_clk_m();

	/* Restore RAM SVOP setting */
	restore_ram_svop();

	/* Bad qpop value on cmd pad macros removes clock gating from IB path */
	set_pmacro_training_wrptr();

	/* Restore CAR CE's, SLCG overrides */
	mbist_workaround();

	/* Configure unused SDMMC1/3 pads for low power leakage */
	low_power_sdmmc_pads();

	/*
	 * Find out which CPU (slow or fast) to wake up. The default setting
	 * in flow controller is to wake up GCPU
	 */
	if (read32(pmc_scratch4_ptr) & PMC_WAKEUP_CLUSTER_LPCPU) {
		setbits32(ACTIVE_SLOW, flow_ctlr_bpmp_cluster_control_ptr);
	}

	/* Set the CPU reset vector */
	write32(sb_aa64_reset_low, (read32(pmc_secure_scratch34_ptr) | 1));
	write32(sb_aa64_reset_high, read32(pmc_secure_scratch35_ptr));

	/* Program SUPER_CCLK_DIVIDER. */
	write32(clk_rst_super_cclkg_div_ptr, SUPER_CDIV_ENB);
	write32(clk_rst_super_cclklp_div_ptr, SUPER_CDIV_ENB);

	config_core_sight();

	/* Set MSELECT clock source to PLL_P with 1:4 divider */
	config_mselect();

	/* Enable UART */
	enable_uart();

	/* Disable PLLX since it isn't used in the kernel as CPU clk source. */
	clrbits32(PLLX_ENABLE, clk_rst_pllx_base_ptr);

	/* Set CAR2PMC_CPU_ACK_WIDTH to 0 */
	clrbits32(CAR2PMC_CPU_ACK_WIDTH_MASK, clk_rst_cpu_softrst_ctrl2_ptr);

	/* Clear PMC_SCRATCH190 */
	clrbits32(1, pmc_scratch190_ptr);

	/* Clear PMC_DPD_SAMPLE */
	write32(pmc_dpd_sample_ptr, 0);
	udelay(10);

	/* Clear the MC_INTSTATUS if MC_INTMASK was 0. */
	if (!read32(mc_intmask_ptr)) {
		uint32_t mc_intst_val = read32(mc_intstatus_ptr);
		if (mc_intst_val)
			write32(mc_intstatus_ptr, mc_intst_val);
	}

	/*
	 * Set both _ACCESS bits so that kernel/secure code
	 * can reconfig VPR careveout as needed from the TrustZone.
	 */
	write32(mc_video_protect_size_mb_ptr, 0);
	write32(mc_video_protect_reg_ctrl_ptr,
		VPR_WR_ACCESS_DISABLE | VPR_ALLOW_TZ_WR_ACCESS);

	/* Tristate CLDVFS PWM */
	write32(pinmux_dvfs_pwm_ptr, (TRISTATE | PM_CLDVFS));

	/* Restore PWR I2C pinmux configuration */
	write32(pinmux_pwr_i2c_scl_ptr, (E_INPUT | PM_I2CPMU));
	write32(pinmux_pwr_i2c_sda_ptr, (E_INPUT | PM_I2CPMU));

	/* Enable CLDVFS clock */
	write32(clk_rst_clk_enb_w_set_ptr, CLK_ENB_DVFS);

	/* Set CLDVFS clock source and divider */
	write32(clk_rst_clk_dvfs_ref_ptr, DVFS_REF_CLK_DIVISOR);
	write32(clk_rst_clk_dvfs_soc_ptr, DVFS_SOC_CLK_DIVISOR);

	/* Enable PWR I2C controller */
	write32(clk_rst_clk_enb_h_set_ptr, CLK_ENB_I2C5);
	write32(clk_rst_rst_dev_h_set_ptr, I2C5_RST);
	udelay(5);
	write32(clk_rst_clk_source_i2c5_ptr, CLK_SRC_PLLP_OUT0 | I2C5_CLK_DIVISOR);
	write32(clk_rst_rst_dev_h_clr_ptr, I2C5_RST);

	/*
	 * Turn on CPU rail:
	 * SCRATCH201[1] is being used to identify CPU PMIC in warmboot code.
	 * 0 : OVR2
	 * 1 : MAX77621
	 */
	if (read32(pmc_scratch201_ptr) & PMIC_77621)
		/* Set CPU rail 0.85V */
		i2c_send(MAX77621_I2C_ADDR, MAX77621_VOUT_DATA);
	else
		/* Enable GPIO5 on MAX77620 PMIC */
		i2c_send(MAX77620_I2C_ADDR, MAX77620_GPIO5_DATA);

	/* Disable PWR I2C */
	write32(clk_rst_rst_dev_h_set_ptr, I2C5_RST);
	write32(clk_rst_clk_enb_h_clr_ptr, CLK_ENB_I2C5);

	/* Delay before removing clamp */
	udelay(2000);

	/*
	 * Reprogram PMC_CPUPWRGOOD_TIMER register:
	 *
	 * XXX This is a fragile assumption. XXX
	 * The kernel prepares PMC_CPUPWRGOOD_TIMER based on a 32768Hz clock.
	 * Note that PMC_CPUPWRGOOD_TIMER is running at pclk.
	 *
	 * We need to reprogram PMC_CPUPWRGOOD_TIMER based on the current pclk
	 * which is at 204Mhz (pclk = sclk = pllp_out2) after BootROM. Multiply
	 * PMC_CPUPWRGOOD_TIMER by 204M / 32K.
	 *
	 * Save the original PMC_CPUPWRGOOD_TIMER register which we need to
	 * restore after the CPU is powered up.
	 */
	orig_timer = read32(pmc_cpupwrgood_timer_ptr);
	write32(pmc_cpupwrgood_timer_ptr, orig_timer * (204000000 / 32768));

	/* Power on CRAIL in PMC */
	power_on_partition(PARTID_CRAIL);

	/* Remove SW controlled clamp */
	write32(pmc_set_sw_clamp_ptr, 0);
	write32(pmc_remove_clamping_cmd_ptr, (1 << PARTID_CRAIL));
	/* Wait until clamp is off. */
	while (read32(pmc_clamp_status_ptr) & (1 << PARTID_CRAIL))
		;

	/* Disable CLDVFS clock */
	write32(clk_rst_clk_enb_w_clr_ptr, CLK_ENB_DVFS);

	/* Perform fast cluster RAM repair. */
	if (!(read32(flow_ctlr_bpmp_cluster_control_ptr) & ACTIVE_SLOW))
		ram_repair();

	/* Power up the non-CPU partition. */
	power_on_partition(PARTID_C0NC);

	/* Enable PLLP branch going to CPU */
	write32(clk_rst_clk_enb_y_set_ptr, CLK_ENB_PLLP_OUT_CPU);
	udelay(2);

	/* Enable the CPU complex clocks */
	enable_select_cpu_clocks();
	udelay(10);

	/* Take non-cpu OUT of reset */
	write32(clk_rst_cpug_cmplx_clr_ptr, CLR_NONCPURESET);

	/* Power up the CPU0 partition. */
	power_on_partition(PARTID_CE0);

	/* Restore the original PMC_CPUPWRGOOD_TIMER. */
	write32(pmc_cpupwrgood_timer_ptr, orig_timer);

	/* Clear software controlled reset */
	write32(clk_rst_cpug_cmplx_clr_ptr, (CLR_CPURESET0 | CLR_CORERESET0));

	/* Halt the AVP. */
	while (1)
		write32(flow_ctlr_halt_cop_events_ptr,
		        FLOW_MODE_STOP | EVENT_JTAG);
}


/* Header. */

extern uint8_t blob_data;
extern uint8_t blob_data_size;
extern uint8_t blob_total_size;

struct lp0_header {
	uint32_t length_insecure;	// Insecure total length.
	uint32_t reserved[3];
	uint8_t rsa_modulus[256];	// RSA key modulus.
	uint8_t aes_signature[16];	// AES signature.
	uint8_t rsa_signature[256];	// RSA-PSS signature.
	uint8_t random_aes_block[16];	// Random data, may be zero.
	uint32_t length_secure;		// Secure total length.
	uint32_t destination;		// Where to load the blob in iRAM.
	uint32_t entry_point;		// Entry point for the blob.
	uint32_t code_length;		// Length of just the data.
} __packed;

struct lp0_header header __attribute__((section(".header"))) =
{
	.length_insecure = (uintptr_t)&blob_total_size,
	.length_secure = (uintptr_t)&blob_total_size,
	.destination = (uintptr_t)&blob_data,
	.entry_point = (uintptr_t)&lp0_resume,
	.code_length = (uintptr_t)&blob_data_size
};
