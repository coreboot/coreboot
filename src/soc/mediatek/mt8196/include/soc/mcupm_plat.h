/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_INCLUDE_SOC_MCUPM_PLAT_H__
#define __SOC_MEDIATEK_MT8196_INCLUDE_SOC_MCUPM_PLAT_H__

#include <soc/addressmap.h>
#include <soc/mcupm_plat.h>
#include <types.h>

#define MCUPM_SW_RSTN				(MCUCFG_BASE + 0x00240000)
#define CLK_CFG_14				(CKSYS_BASE + 0x000000F0)

/*
 * mcusys_par_wrap config
 */
/* CPU_EB_CLK_SRC_CONFIG */
#define CPUEB_CLK_SRC				(MCUCFG_BASE + 0x000002E8)

#define CPUEB_SPMC_STATUS			(MCUCFG_BASE + 0x00000B48)
#define CPUEB_FSM_STATE_ON			0xF
#define CPUEB_FSM_STATE_OFF			0x0
#define CPUEB_FSM_STATE_RET			0x1D
#define CPUEB_FSM_STATE_FUNC_RET		0x13

#define CPUEB_SPMC_SEL				(MCUCFG_BASE + 0x00000B4C)

#define MCU_PORT_SET_W1C_0			(IFRBUS_AO_REG_BUS_BASE + 0x00000708)
#define CPUEB_PROTECT_EN_0			BIT(8)
#define CPUEB_PROTECT_EN_1			BIT(9)
#define MCU_PORT_SET_R0_0			(IFRBUS_AO_REG_BUS_BASE + 0x0000070C)
#define CPUEB_PROTECT_RDY_0			BIT(8)
#define CPUEB_PROTECT_RDY_1			BIT(9)

/*
 * SPM config:
 * Enable register access key, POWERON_CONFIG_EN = 0x0B160001
 */
#define POWERON_CONFIG_EN_MCU			(SPM_BASE + 0x000)
#define SPM_PROJECT_CODE			0xB16
#define SPM_REGWR_CFG_KEY			(SPM_PROJECT_CODE << 16)
/* POWERON_CONFIG_EN */
#define BCLK_CG_EN_LSB				BIT(0)
/* CPUEB SPMC Control */
/* SPM and MCUPM SPMC sideband control */
#define SPM_MCUPM_SPMC_CON			(SPM_BASE + 0x288)
/* 1'b0, SPM power state, 0: invalid, 1: valid */
#define CPUEB_STATE_VALID			BIT(0)
/* 1'b0, power state request for CPUEB */
#define REQ_PWR_ON				BIT(1)
/* 1'b0, memory retention state request for CPUEB */
#define REQ_MEM_RET				BIT(2)
/* 1'b0, spmc finish operation, 0:unfinish, 1:finish */
#define CPUEB_STATE_FINISH_ACK			BIT(31)

/* 512K Bytes */
#define MCUPM_SRAM_SIZE				(512 * KiB)

/* GPR */
#define MCUPM_GPR_SIZE				0x00000070 /* 112 Bytes */
/* IPI */
#define IPI_NUMBER				16
#define IPI_MBOX_TOTAL				IPI_NUMBER

/* MCUPM REG */
#define MCUPM_CFGREG_SW_RSTN_SW_RSTN		BIT(0)
#define MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK	(BIT(1)|BIT(2)|BIT(3)|BIT(4))

#define DEVAPC_INFRA_SECU_AO_SEC_REPLACE_0	(DEVAPC_INFRA_SECU_AO_BASE + 0x0300)
#define POLLING_MCU_RETRY_COUNTS		10
#define POLLING_ACK_RETRY_COUNTS		10
#define ABNORMALBOOT_REG_STATUS			0x0
#define WARMBOOT_REG_STATUS			0x0
#define MCUPM_RSTN_RESET			0x1F
#define MCUPM_RSTN_RSTN_INIT			0x0

#define CPUEB_SPMC_STATUS_OFFSET		3
#define CPUEB_SPMC_STATUS_MASK			0x3F

#define SRAM_GPR_SIZE				0x4 /* 4 Bytes */

#define MBOX_SLOT_SIZE				0x4
/* 0x14 = 20 slots = 20*4Bytes = 80 Bytes */
#define SRAM_SLOT_NUM				0x14
#define PIN_S_NUM				SRAM_SLOT_NUM
#define PIN_R_NUM				SRAM_SLOT_NUM
#define MBOX_TABLE_NUM				(PIN_S_NUM + PIN_R_NUM)

#define GPR_BASE_ADDR_MCU(x)			(MCUPM_SRAM_BASE + \
						 MCUPM_SRAM_SIZE - \
						 (IPI_MBOX_TOTAL * MBOX_TABLE_NUM * MBOX_SLOT_SIZE) - \
						 MCUPM_GPR_SIZE + \
						 ((x) * SRAM_GPR_SIZE))

#define ABNORMALBOOT_REG			GPR_BASE_ADDR_MCU(1)
#define WARMBOOT_REG				GPR_BASE_ADDR_MCU(23)

#endif /* __SOC_MEDIATEK_MT8196_INCLUDE_SOC_MCUPM_PLAT_H__ */
