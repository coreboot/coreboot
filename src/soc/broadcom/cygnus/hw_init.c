/*
 * Copyright (C) 2015 Broadcom Corporation
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

#include <arch/io.h>
#include <delay.h>
#include <console/console.h>
#include <soc/tz.h>
#include <soc/hw_init.h>

/*****************************************************************************
 * TrustZone
 *****************************************************************************/
#define IHOST_SCU_SECURE_ACCESS			0x19020054

#define SMAU_NIC_IDM_TZ_BASE			0x180150a0
#define SMAU_DDR_TZ_BASE			0x18015200
#define SMAU_FLASH0_TZ_BASE			0x18015300
#define SMAU_FLASH1_TZ_BASE			0x18015308
#define SMAU_FLASH2_TZ_BASE			0x18015310
#define SMAU_FLASH3_TZ_BASE			0x18015318
#define SMAU_TZ_BASE_ENABLE			0x00000001

#define CRMU_IPROC_ADDR_RANGE0_LOW		0x03024c30
#define CRMU_IPROC_ADDR_RANGE0_HIGH		0x03024c34
#define CRMU_ADDR_MASK				0xffffff00
#define CRMU_ADDR_VALID				0x00000001
#define CRMU_ADDR_START				0x03010000
#define CRMU_ADDR_END				0x03100000

static void scu_ns_config(void)
{
	/*
	 * Enable NS SCU access to ARM global timer, private timer, and
	 * components
	 */
	write32((void *)IHOST_SCU_SECURE_ACCESS, 0xFFF);
}

static void smau_ns_config(void)
{
	unsigned int val;

	/* Disable SMAU NIC IDM TZ */
	val = read32((void *)SMAU_NIC_IDM_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_NIC_IDM_TZ_BASE, val);

	/*
	 * Disable DDR TZ base
	 *
	 * This means the entire DDR is marked as NONSECURE (NS)
	 *
	 * NOTE: In the future, multiple regions of DDR may need to be marked
	 * as SECURE for secure OS and other TZ usages
	 */
	val = read32((void *)SMAU_DDR_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_DDR_TZ_BASE, val);


	/*
	 * Disable flash TZ support
	 *
	 * The entire flash is currently marked as NS
	 *
	 * NOTE: In the future, multiple regions of flash may need to be marked
	 * as SECURE for secure OS and other TZ firmware/data storage
	 */

	/* Flash 0: ROM */
	val = read32((void *)SMAU_FLASH0_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_FLASH0_TZ_BASE, val);

	/* Flash 1: QSPI */
	val = read32((void *)SMAU_FLASH1_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_FLASH1_TZ_BASE, val);

	/* Flash 2: NAND */
	val = read32((void *)SMAU_FLASH2_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_FLASH2_TZ_BASE, val);

	/* Flash 3: PNOR */
	val = read32((void *)SMAU_FLASH3_TZ_BASE);
	val &= ~SMAU_TZ_BASE_ENABLE;
	write32((void *)SMAU_FLASH3_TZ_BASE, val);
}

static void crmu_ns_config(void)
{

	/*
	 * Currently opens up the entire CRMU to allow iPROC NS access
	 *
	 * NOTE: In the future, we might want to protect particular CRMU
	 * sub-blocks to allow SECURE access only. That can be done by
	 * programming the CRMU IPROC address range registers. Up to 4 access
	 * windows can be created
	 */
	write32((void *)CRMU_IPROC_ADDR_RANGE0_LOW,
		(CRMU_ADDR_START & CRMU_ADDR_MASK) | CRMU_ADDR_VALID);
	write32((void *)CRMU_IPROC_ADDR_RANGE0_HIGH,
		(CRMU_ADDR_END & CRMU_ADDR_MASK) | CRMU_ADDR_VALID);
}

static void tz_init(void)
{
	/* Configure the Cygnus for non-secure access */
	/* ARM Cortex A9 SCU NS access configuration */
	scu_ns_config();

	/* SMAU NS related configurations */
	smau_ns_config();

	/* CRMU NS related configurations */
	crmu_ns_config();

	/*
	 * Configure multiple masters and slaves to run in NS
	 */
	tz_set_non_virtual_slaves_security(0xFFFFFFFF, TZ_STATE_NON_SECURE);
	tz_set_periph_security(0xFFFFFFFF, TZ_STATE_NON_SECURE);
	tz_set_masters_security(0xFFFFFFFF, TZ_STATE_NON_SECURE);
	tz_set_wrapper_security(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				TZ_STATE_NON_SECURE);
	tz_set_cfg_slaves_security(0xFFFFFFFF, TZ_STATE_NON_SECURE);

	tz_set_ext_slaves_security(0xFFFFFFFF, TZ_STATE_NON_SECURE);

	/* configure sec peripherals to be accessed from non-secure world */
	tz_set_sec_periphs_security(0xFFFFFFFF &
				    ~(CYGNUS_sec_periph_APBz_sotp |
				      CYGNUS_sec_periph_APBz_tzpc),
				    TZ_STATE_NON_SECURE);

	/* default sram to non-secure */
	tz_set_sram_sec_region(0);
}

/*****************************************************************************
 * DMAC
 *****************************************************************************/
#define DMAC_M0_IDM_RESET_CONTROL	0x1810f800
#define DMAC_RESET_MASK			0x00000001
#define DMAC_RESET_TIMEOUT		1000

static void dmac_init(void)
{
	unsigned int val, timeout;

	/* bring the DMAC block out of reset */
	val = read32((void *)DMAC_M0_IDM_RESET_CONTROL);
	val |= DMAC_RESET_MASK;
	write32((void *)DMAC_M0_IDM_RESET_CONTROL, val);
	udelay(10);
	val &= ~DMAC_RESET_MASK;
	write32((void *)DMAC_M0_IDM_RESET_CONTROL, val);

	timeout = 0;
	while (read32((void *)DMAC_M0_IDM_RESET_CONTROL) & DMAC_RESET_MASK) {
		udelay(1);
		if (timeout++ > DMAC_RESET_TIMEOUT)
			die("Failed to bring PL330 DMAC out of reset\n");
	}
}

/*****************************************************************************
 * Neon
 *****************************************************************************/
#define CRU_CONTROL				0x1800e000
#define CRU_CONTROL_NEON_RESET_N		0x00000040

#define CRU_IHOST_PWRDWN_EN			0x1800e004
#define CRU_IHOST_PWRDWN_EN_CLAMPON_NEON	0x00100000
#define CRU_IHOST_PWRDWN_EN_PWRON_NEON		0x00200000
#define CRU_IHOST_PWRDWN_EN_PWROK_NEON		0x00400000

#define CRU_IHOST_PWRDWN_STATUS			0x1800e008
#define CRU_IHOST_PWRDWN_STATUS_PWRON_NEON	0x00200000
#define CRU_IHOST_PWRDWN_STATUS_PWROK_NEON	0x00400000

#define CRU_STATUS_DELAY_US			1
#define CRU_MAX_RETRY_COUNT			10
#define CRU_RETRY_INTVL_US			1

static void neon_init(void)
{
	unsigned int i, val;

	/* put Neon into reset */
	val = read32((void *)CRU_CONTROL);
	val &= ~CRU_CONTROL_NEON_RESET_N;
	write32((void *)CRU_CONTROL, val);

	/* assert the power on register bit */
	val = read32((void *)CRU_IHOST_PWRDWN_EN);
	val |= CRU_IHOST_PWRDWN_EN_PWRON_NEON;
	write32((void *)CRU_IHOST_PWRDWN_EN, val);

	/* wait for power on */
	i = 0;
	while (!(read32((void *)CRU_IHOST_PWRDWN_STATUS) &
		 CRU_IHOST_PWRDWN_STATUS_PWRON_NEON)) {
		udelay(CRU_RETRY_INTVL_US);
		if (i++ >= CRU_MAX_RETRY_COUNT)
			die("Failed to ack NEON power on\n");
	}

	udelay(CRU_STATUS_DELAY_US);

	/* assert the power ok register bit */
	val = read32((void *)CRU_IHOST_PWRDWN_EN);
	val |= CRU_IHOST_PWRDWN_EN_PWROK_NEON;
	write32((void *)CRU_IHOST_PWRDWN_EN, val);

	/* wait for power ok */
	i = 0;
	while (!(read32((void *)CRU_IHOST_PWRDWN_STATUS) &
		 CRU_IHOST_PWRDWN_STATUS_PWROK_NEON)) {
		udelay(CRU_RETRY_INTVL_US);
		if (i++ >= CRU_MAX_RETRY_COUNT)
			die("Failed to ack NEON power ok\n");
	}

	udelay(CRU_STATUS_DELAY_US);

	/* clamp off for the NEON block */
	val = read32((void *)CRU_IHOST_PWRDWN_EN);
	val &= ~CRU_IHOST_PWRDWN_EN_CLAMPON_NEON;
	write32((void *)CRU_IHOST_PWRDWN_EN, val);

	udelay(CRU_STATUS_DELAY_US);

	/* bring NEON out of reset */
	val = read32((void *)CRU_CONTROL);
	val |= CRU_CONTROL_NEON_RESET_N;
	write32((void *)CRU_CONTROL, val);
}

/*****************************************************************************
 * PCIe
 *****************************************************************************/
#define CRMU_PCIE_CFG			0x0301d0a0
#define PCIE0_LNK_PHY_IDDQ		0x00000004
#define PCIE1_LNK_PHY_IDDQ		0x00000400

static void pcie_init(void)
{
	unsigned int val;

	/*
	 * Force all AFEs of both PCIe0 and PCIe1 to be powered down, including
	 * pad biasing
	 *
	 * This brings down the PCIe interfaces to the lowest possible power
	 * mode
	 */
	val = read32((void *)CRMU_PCIE_CFG);
	val |= PCIE1_LNK_PHY_IDDQ | PCIE0_LNK_PHY_IDDQ;
	write32((void *)CRMU_PCIE_CFG, val);
}

/*****************************************************************************
 * M0
 *****************************************************************************/
#define CRMU_MCU_ACCESS_CONTROL			0x03024c00
#define CRMU_MCU_ACCESS_MODE_SECURE		0x2

static void M0_init(void)
{
	/* Set M0 as a secure master */
	write32((void *)CRMU_MCU_ACCESS_CONTROL, CRMU_MCU_ACCESS_MODE_SECURE);
}

/*****************************************************************************
 * CCU
 *****************************************************************************/
#define IHOST_PROC_CLK_WR_ACCESS		0x19000000
#define IHOST_PROC_CLK_POLICY_FREQ		0x19000008
#define IHOST_PROC_CLK_POLICY_CTL		0x1900000c
#define IHOST_PROC_CLK_POLICY0_MASK		0x19000010
#define IHOST_PROC_CLK_POLICY1_MASK		0x19000014
#define IHOST_PROC_CLK_POLICY2_MASK		0x19000018
#define IHOST_PROC_CLK_POLICY3_MASK		0x1900001c
#define IHOST_PROC_CLK_INTEN			0x19000020
#define IHOST_PROC_CLK_INTSTAT			0x19000024
#define IHOST_PROC_CLK_LVM_EN			0x19000034
#define IHOST_PROC_CLK_LVM0_3			0x19000038
#define IHOST_PROC_CLK_LVM4_7			0x1900003c
#define IHOST_PROC_CLK_VLT0_3			0x19000040
#define IHOST_PROC_CLK_VLT4_7			0x19000044
#define IHOST_PROC_CLK_BUS_QUIESC		0x19000100
#define IHOST_PROC_CLK_CORE0_CLKGATE		0x19000200
#define IHOST_PROC_CLK_CORE1_CLKGATE		0x19000204
#define IHOST_PROC_CLK_ARM_SWITCH_CLKGATE	0x19000210
#define IHOST_PROC_CLK_ARM_PERIPH_CLKGATE	0x19000300
#define IHOST_PROC_CLK_APB0_CLKGATE		0x19000400
#define IHOST_PROC_CLK_PL310_DIV		0x19000a00
#define IHOST_PROC_CLK_PL310_TRIGGER		0x19000a04
#define IHOST_PROC_CLK_ARM_SWITCH_DIV		0x19000a08
#define IHOST_PROC_CLK_ARM_SWITCH_TRIGGER	0x19000a0c
#define IHOST_PROC_CLK_APB_DIV			0x19000a10
#define IHOST_PROC_CLK_APB_DIV_TRIGGER		0x19000a14
#define IHOST_PROC_CLK_PLLARMA			0x19000c00
#define IHOST_PROC_CLK_PLLARMB			0x19000c04
#define IHOST_PROC_CLK_PLLARMC			0x19000c08
#define IHOST_PROC_CLK_PLLARMCTRL0		0x19000c0c
#define IHOST_PROC_CLK_PLLARMCTRL1		0x19000c10
#define IHOST_PROC_CLK_PLLARMCTRL2		0x19000c14
#define IHOST_PROC_CLK_PLLARMCTRL3		0x19000c18
#define IHOST_PROC_CLK_PLLARMCTRL4		0x19000c1c
#define IHOST_PROC_CLK_PLLARMCTRL5		0x19000c20
#define IHOST_PROC_CLK_PLLARM_OFFSET		0x19000c24
#define IHOST_PROC_CLK_ARM_DIV			0x19000e00
#define IHOST_PROC_CLK_ARM_SEG_TRG		0x19000e04
#define IHOST_PROC_CLK_ARM_SEG_TRG_OVERRIDE	0x19000e08
#define IHOST_PROC_CLK_PLL_DEBUG		0x19000e10
#define IHOST_PROC_CLK_ACTIVITY_MON1		0x19000e20
#define IHOST_PROC_CLK_ACTIVITY_MON2		0x19000e24
#define IHOST_PROC_CLK_CLKGATE_DBG		0x19000e40
#define IHOST_PROC_CLK_APB_CLKGATE_DBG1		0x19000e48
#define IHOST_PROC_CLK_CLKMON			0x19000e64
#define IHOST_PROC_CLK_POLICY_DBG		0x19000ec0
#define IHOST_PROC_CLK_TGTMASK_DBG1		0x19000ec4
#define IHOST_PROC_RST_WR_ACCESS		0x19000f00
#define IHOST_PROC_RST_SOFT_RSTN		0x19000f04
#define IHOST_PROC_RST_A9_CORE_SOFT_RSTN	0x19000f08

#define WR_ACCESS_PRIVATE_ACCESS_MODE		0x80000000

static uint32_t ccu_reg[] = {
	IHOST_PROC_CLK_WR_ACCESS,
	IHOST_PROC_CLK_POLICY_FREQ,
	IHOST_PROC_CLK_POLICY_CTL,
	IHOST_PROC_CLK_POLICY0_MASK,
	IHOST_PROC_CLK_POLICY1_MASK,
	IHOST_PROC_CLK_POLICY2_MASK,
	IHOST_PROC_CLK_POLICY3_MASK,
	IHOST_PROC_CLK_INTEN,
	IHOST_PROC_CLK_INTSTAT,
	IHOST_PROC_CLK_LVM_EN,
	IHOST_PROC_CLK_LVM0_3,
	IHOST_PROC_CLK_LVM4_7,
	IHOST_PROC_CLK_VLT0_3,
	IHOST_PROC_CLK_VLT4_7,
	IHOST_PROC_CLK_BUS_QUIESC,
	IHOST_PROC_CLK_CORE0_CLKGATE,
	IHOST_PROC_CLK_CORE1_CLKGATE,
	IHOST_PROC_CLK_ARM_SWITCH_CLKGATE,
	IHOST_PROC_CLK_ARM_PERIPH_CLKGATE,
	IHOST_PROC_CLK_APB0_CLKGATE,
	IHOST_PROC_CLK_PL310_DIV,
	IHOST_PROC_CLK_PL310_TRIGGER,
	IHOST_PROC_CLK_ARM_SWITCH_DIV,
	IHOST_PROC_CLK_ARM_SWITCH_TRIGGER,
	IHOST_PROC_CLK_APB_DIV,
	IHOST_PROC_CLK_APB_DIV_TRIGGER,
	IHOST_PROC_CLK_PLLARMA,
	IHOST_PROC_CLK_PLLARMB,
	IHOST_PROC_CLK_PLLARMC,
	IHOST_PROC_CLK_PLLARMCTRL0,
	IHOST_PROC_CLK_PLLARMCTRL1,
	IHOST_PROC_CLK_PLLARMCTRL2,
	IHOST_PROC_CLK_PLLARMCTRL3,
	IHOST_PROC_CLK_PLLARMCTRL4,
	IHOST_PROC_CLK_PLLARMCTRL5,
	IHOST_PROC_CLK_PLLARM_OFFSET,
	IHOST_PROC_CLK_ARM_DIV,
	IHOST_PROC_CLK_ARM_SEG_TRG,
	IHOST_PROC_CLK_ARM_SEG_TRG_OVERRIDE,
	IHOST_PROC_CLK_PLL_DEBUG,
	IHOST_PROC_CLK_ACTIVITY_MON1,
	IHOST_PROC_CLK_ACTIVITY_MON2,
	IHOST_PROC_CLK_CLKGATE_DBG,
	IHOST_PROC_CLK_APB_CLKGATE_DBG1,
	IHOST_PROC_CLK_CLKMON,
	IHOST_PROC_CLK_POLICY_DBG,
	IHOST_PROC_CLK_TGTMASK_DBG1,
	IHOST_PROC_RST_WR_ACCESS,
	IHOST_PROC_RST_SOFT_RSTN,
	IHOST_PROC_RST_A9_CORE_SOFT_RSTN
};

#define CCU_REG_TABLE_SIZE (sizeof(ccu_reg)/sizeof(ccu_reg[0]))

/* Set priv_access_mode field to unrestricted (0) */
static void ccu_init(void)
{
	uint32_t val;
	uint32_t i;
	for (i = 0; i < CCU_REG_TABLE_SIZE; i++) {
		val = read32((void *)(ccu_reg[i]));
		val &= ~WR_ACCESS_PRIVATE_ACCESS_MODE;
		write32((void *)(ccu_reg[i]), val);
	}
}

/*****************************************************************************
 * LCD
 *****************************************************************************/
#define ASIU_TOP_CLK_GATING_CTRL			0x180aa024
#define ASIU_TOP_CLK_GATING_CTRL_LCD_CLK_GATE_EN	0x00000010
#define ASIU_TOP_CLK_GATING_CTRL_MIPI_DSI_CLK_GATE_EN	0x00000008
#define ASIU_TOP_CLK_GATING_CTRL_GFX_CLK_GATE_EN	0x00000001
#define ASIU_TOP_CLK_GATING_CTRL_AUD_CLK_GATE_EN	0x00000002

static void lcd_init(void)
{
	unsigned int val;

	/* make sure the LCD clock is ungated */
	val = read32((void *)ASIU_TOP_CLK_GATING_CTRL);
	val |= ASIU_TOP_CLK_GATING_CTRL_LCD_CLK_GATE_EN;
	write32((void *)ASIU_TOP_CLK_GATING_CTRL, val);
}

/*******************************************************************
 * Default priority settings in Cygnus
 *
 *   Master Name                Default Priority
 *   ====================       =================
 *   ihost_m0                   12
 *   mhost0_m0                  12
 *   mhost1_m0              `   12
 *   pcie0_m0                   9
 *   pcie0_m1                   9
 *   cmicd_m0                   7
 *   amac_m0                    7
 *   amac_m1                    7
 *   ext_m0 (LCD)               5
 *   ext_m1 (V3D)               5
 *   sdio_m0                    3
 *   sdio_m1                    3
 *   usb2h_m0                   3
 *   usb2d_m0                   3
 *   dmu_m0                     3
 *   a9jtag_m0                  0
 *
 *****************************************************************************/

#define AXIIC_EXT_M0_READ_QOS			0x1a057100
#define AXIIC_EXT_M0_READ_MASK			0x0000000f
#define AXIIC_EXT_M0_WRITE_QOS			0x1a057104
#define AXIIC_EXT_M0_WRITE_MASK			0x0000000f

static void lcd_qos_init(unsigned int qos)
{
	unsigned int val;

	val = read32((void *)AXIIC_EXT_M0_READ_QOS);
	val &= ~AXIIC_EXT_M0_READ_MASK;
	val |= (qos & AXIIC_EXT_M0_READ_MASK);
	write32((void *)AXIIC_EXT_M0_READ_QOS, val);

	val = read32((void *)AXIIC_EXT_M0_WRITE_QOS);
	val &= ~AXIIC_EXT_M0_WRITE_MASK;
	val |= (qos & AXIIC_EXT_M0_WRITE_MASK);
	write32((void *)AXIIC_EXT_M0_WRITE_QOS, val);
}

/*****************************************************************************
 * V3D
 *****************************************************************************/
static void v3d_init(void)
{
	unsigned int val;

	/* make sure the V3D clock is ungated */
	val = read32((void *)ASIU_TOP_CLK_GATING_CTRL);
	val |= ASIU_TOP_CLK_GATING_CTRL_MIPI_DSI_CLK_GATE_EN |
	       ASIU_TOP_CLK_GATING_CTRL_GFX_CLK_GATE_EN;
	write32((void *)ASIU_TOP_CLK_GATING_CTRL, val);
}

/*****************************************************************************
 * Audio
 *****************************************************************************/
#define CRMU_PLL_AON_CTRL				0x0301c020
#define CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_PLL	0x00000800
#define CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_BG	0x00000400
#define CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_LDO	0x00000200
#define CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_ISO_IN	0x00000100

static void audio_init(void)
{
	unsigned int val;

	/* Ungate (enable) audio clock. */
	val = read32((void *)ASIU_TOP_CLK_GATING_CTRL);
	val |= ASIU_TOP_CLK_GATING_CTRL_AUD_CLK_GATE_EN;
	write32((void *)ASIU_TOP_CLK_GATING_CTRL, val);

	/* Power on audio GEN PLL, LDO, and BG. Input isolation = normal. */
	val = read32((void *)CRMU_PLL_AON_CTRL);
	val |= CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_BG;
	val |= CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_LDO;
	val |= CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_PWRON_PLL;
	val &= ~CRMU_PLL_AON_CTRL_ASIU_AUDIO_GENPLL_ISO_IN;
	write32((void *)CRMU_PLL_AON_CTRL, val);
}

/*****************************************************************************
 * SDIO
 *****************************************************************************/
#define CRMU_SDIO_1P8_FAIL_CONTROL	0x0301c0a0
#define UHS1_18V_VREG_FAIL		0x00000001

#define SDIO_IDM0_IO_CONTROL_DIRECT	0x18116408
#define SDIO_IDM1_IO_CONTROL_DIRECT	0x18117408
#define SDIO_CMD_COMFLICT_DISABLE	0x00400000
#define SDIO_FEEDBACK_CLK_EN		0x00200000
#define SDIO_CLK_ENABLE			0x00000001

#define CDRU_SDIO0_IO_CONTROL		0x0301d144
#define CDRU_SDIO1_IO_CONTROL		0x0301d140
#define INPUT_DISABLE			0x00000080
#define SLEW_RATE_ENABLE		0x00000040
#define PULL_UP_ENABLE			0x00000020
#define PULL_DOWN_ENABLE		0x00000010
#define HYSTERESIS_ENABLE		0x00000008
#define SDIO_DEFAULT_DRIVE_STRENGTH	0x4		/* 8 mA */

#define SDIO_IDM0_IDM_RESET_CONTROL	0x18116800
#define SDIO_IDM1_IDM_RESET_CONTROL	0x18117800
#define SDIO_RESET_MASK			0x00000001
#define SDIO_RESET_TIMEOUT		1000

#define CRMU_SDIO_CONTROL0		0x0301d088
#define CRMU_SDIO_CONTROL1		0x0301d08c
#define CRMU_SDIO_CONTROL2		0x0301d090
#define CRMU_SDIO_CONTROL3		0x0301d094
#define CRMU_SDIO_CONTROL4		0x0301d098
#define CRMU_SDIO_CONTROL5		0x0301d09c

/*
 * SDIO_CAPS_L
 *
 * Field		Bit(s)
 * ===========================
 * DDR50		31
 * SDR104		30
 * SDR50		29
 * SLOTTYPE		28:27
 * ASYNCHIRQ		26
 * SYSBUS64		25
 * V18			24
 * V3			23
 * V33			22
 * SUPRSM		21
 * SDMA			20
 * HSPEED		19
 * ADMA2		18
 * EXTBUSMED		17
 * MAXBLK		16:15
 * BCLK			14:7
 * TOUT			6
 * TOUTFREQ		5:0
 */
#define SDIO_CAPS_L			0xA17C0000

/*
 * SDIO_CAPS_H
 *
 * Field		Bit(s)
 * ===========================
 * reserved		31:20
 * SPIBLOCKMODE		19
 * SPIMODE_CAP		18
 * CLOCKMULT		17:10
 * RETUNE_MODE		9:8
 * USETUNE_SDR50	7
 * TMRCNT_RETUNE	6:3
 * DRVR_TYPED		2
 * DRVR_TYPEC		1
 * DRVR_TYPEA		0
 */
#define SDIO_CAPS_H			0x000C0087

/*
 * Preset value
 *
 * Field		Bit(s)
 * ===========================
 * Driver Strength	12:11
 * Clock Generator	10
 * SDCLK Frequeency	9:0
 */

/*
 * SDIO_PRESETVAL1
 *
 * Field		Bit(s)	Description
 * ============================================================
 * DDR50_PRESET		25:13	Preset Value for DDR50
 * DEFAULT_PRESET	12:0	Preset Value for Default Speed
 */
#define SDIO_PRESETVAL1		0x01004004

/*
 * SDIO_PRESETVAL2
 *
 * Field		Bit(s)	Description
 * ============================================================
 * HIGH_SPEED_PRESET	25:13	Preset Value for High Speed
 * INIT_PRESET		12:0	Preset Value for Initialization
 */
#define SDIO_PRESETVAL2		0x01004100

/*
 * SDIO_PRESETVAL3
 *
 * Field		Bit(s)	Description
 * ============================================================
 * SDR104_PRESET	25:13	Preset Value for SDR104
 * SDR12_PRESET		12:0	Preset Value for SDR12
 */
#define SDIO_PRESETVAL3		0x00000004

/*
 * SDIO_PRESETVAL4
 *
 * Field		Bit(s)	Description
 * ============================================================
 * SDR25_PRESET		25:13	Preset Value for SDR25
 * SDR50_PRESET		12:0	Preset Value for SDR50
 */
#define SDIO_PRESETVAL4		0x01005001

static void sdio_ctrl_init(unsigned int idx)
{
	unsigned int sdio_idm_io_control_direct_reg;
	unsigned int cdru_sdio_io_control_reg;
	unsigned int sdio_idm_reset_control_reg;
	unsigned int val, timeout;

	switch (idx) {
	case 0:
		sdio_idm_io_control_direct_reg = SDIO_IDM0_IO_CONTROL_DIRECT;
		cdru_sdio_io_control_reg = CDRU_SDIO0_IO_CONTROL;
		sdio_idm_reset_control_reg = SDIO_IDM0_IDM_RESET_CONTROL;
		break;
	case 1:
		sdio_idm_io_control_direct_reg = SDIO_IDM1_IO_CONTROL_DIRECT;
		cdru_sdio_io_control_reg = CDRU_SDIO1_IO_CONTROL;
		sdio_idm_reset_control_reg = SDIO_IDM1_IDM_RESET_CONTROL;
		break;
	default:
		return;
	}

	/*
	 * Disable the cmd conflict error interrupt and enable feedback clock
	 */
	val = read32((void *)sdio_idm_io_control_direct_reg);
	val |= SDIO_CMD_COMFLICT_DISABLE | SDIO_FEEDBACK_CLK_EN |
	       SDIO_CLK_ENABLE;
	write32((void *)sdio_idm_io_control_direct_reg, val);

	/*
	 * Set drive strength, enable hysteresis and slew rate control
	 */
	val = SDIO_DEFAULT_DRIVE_STRENGTH |
		HYSTERESIS_ENABLE | SLEW_RATE_ENABLE;
	write32((void *)cdru_sdio_io_control_reg, val);

	/* Reset SDIO controller */
	val = read32((void *)sdio_idm_reset_control_reg);
	val |= SDIO_RESET_MASK;
	write32((void *)sdio_idm_reset_control_reg, SDIO_RESET_MASK);
	udelay(10);
	val &= ~SDIO_RESET_MASK;
	write32((void *)sdio_idm_reset_control_reg, val);

	timeout = 0;
	while (read32((void *)sdio_idm_reset_control_reg) & SDIO_RESET_MASK) {
		udelay(1);
		if (timeout++ > SDIO_RESET_TIMEOUT)
			die("Failed to bring SDIO out of reset\n");
	}
}

static void sdio_init(void)
{
	unsigned int val;

	/*
	 * Configure SDIO host controller capabilities
	 * (common setting for all SDIO controllers)
	 */
	write32((void *)CRMU_SDIO_CONTROL0, SDIO_CAPS_H);
	write32((void *)CRMU_SDIO_CONTROL1, SDIO_CAPS_L);
	/*
	 * Configure SDIO host controller preset values
	 * (common setting for all SDIO controllers)
	 */
	write32((void *)CRMU_SDIO_CONTROL2, SDIO_PRESETVAL1);
	write32((void *)CRMU_SDIO_CONTROL3, SDIO_PRESETVAL2);
	write32((void *)CRMU_SDIO_CONTROL4, SDIO_PRESETVAL3);
	write32((void *)CRMU_SDIO_CONTROL5, SDIO_PRESETVAL4);

	/*
	 * The sdhci driver attempts to change the SDIO IO voltage for UHS-I
	 * cards by setting the EN1P8V in control2 register then checks the
	 * outcome by reading it back.
	 * Cygnus does not have an internal regulator for the SDIO IO voltage
	 * but can be configured to indicate success (leave EN1P8V set)
	 * or failure (clear EN1P8V).
	 *
	 * Clear CRMU_SDIO_UHS1_18V_VREG_FAIL in CRMU_SDIO_1P8_FAIL_CONTROL
	 * register to indicate success.
	 * (common setting for all SDIO controllers)
	 */
	val = read32((void *)CRMU_SDIO_1P8_FAIL_CONTROL);
	val &= ~UHS1_18V_VREG_FAIL;
	write32((void *)CRMU_SDIO_1P8_FAIL_CONTROL, val);

	/*
	 * Initialize each SDIO controller
	 */
	sdio_ctrl_init(0);
	sdio_ctrl_init(1);
}

void hw_init(void)
{
	tz_init();
	printk(BIOS_INFO, "trustzone initialized\n");
	dmac_init();
	printk(BIOS_INFO, "PL330 DMAC initialized\n");
	lcd_init();
	lcd_qos_init(15);
	printk(BIOS_INFO, "LCD initialized\n");
	v3d_init();
	printk(BIOS_INFO, "V3D initialized\n");
	audio_init();
	printk(BIOS_INFO, "audio initialized\n");
	neon_init();
	printk(BIOS_INFO, "neon initialized\n");
	pcie_init();
	printk(BIOS_INFO, "PCIe initialized\n");
	M0_init();
	printk(BIOS_INFO, "M0 initialized\n");
	ccu_init();
	printk(BIOS_INFO, "CCU initialized\n");
	sdio_init();
	printk(BIOS_INFO, "SDIO initialized\n");
}
