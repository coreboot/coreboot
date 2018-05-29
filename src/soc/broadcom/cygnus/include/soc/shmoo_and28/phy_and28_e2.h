/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SOC_BROADCOM_CYGNUS_PHY_AND28_E2_H__
#define __SOC_BROADCOM_CYGNUS_PHY_AND28_E2_H__

/**
 * m = memory, c = core, r = register, f = field, d = data.
 */
#if !defined(GET_FIELD) && !defined(SET_FIELD)
#define BRCM_ALIGN(c, r, f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c, r, f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c, r, f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c, r, f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m, c, r, f) \
	((((m) & BRCM_MASK(c, r, f)) >> BRCM_SHIFT(c, r, f)) << BRCM_ALIGN(c, r, f))

#define SET_FIELD(m, c, r, f, d) \
	((m) = (((m) & ~BRCM_MASK(c, r, f)) | ((((d) >> BRCM_ALIGN(c, r, f)) << \
	 BRCM_SHIFT(c, r, f)) & BRCM_MASK(c, r, f))) \
	)

#define SET_TYPE_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, c##_##d)
#define SET_NAME_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, c##_##r##_##f##_##d)
#define SET_VALUE_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, d)

#endif /* GET & SET */

/***************************************************************************
 *DDR34_CORE_PHY_CONTROL_REGS - DDR34 CORE DDR34 Address/Comand control registers
 ***************************************************************************/
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION     0x00000000 /* Address & Control revision register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS   0x00000004 /* PHY PLL status register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG   0x00000008 /* PHY PLL configuration register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1 0x0000000c /* PHY PLL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2 0x00000010 /* PHY PLL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3 0x00000014 /* PHY PLL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS 0x00000018 /* PHY PLL integer divider register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER 0x0000001c /* PHY PLL fractional divider register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL 0x00000020 /* PHY PLL spread spectrum control register */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT 0x00000024 /* PHY PLL spread spectrum limit register */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL  0x00000028 /* Aux Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL 0x0000002c /* Idle mode pad control register */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0 0x00000030 /* Idle mode pad enable register */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1 0x00000034 /* Idle mode pad enable register */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL 0x00000038 /* PVT Compensation control register */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL 0x0000003c /* pad rx and tx characteristics control register */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG  0x00000040 /* DRAM configuration register */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1 0x00000044 /* DRAM timing register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2 0x00000048 /* DRAM timing register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3 0x0000004c /* DRAM timing register #3 */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4 0x00000050 /* DRAM timing register #4 */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE 0x00000060 /* PHY VDL calibration control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1 0x00000064 /* PHY VDL calibration status register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2 0x00000068 /* PHY VDL calibration status register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL 0x0000006c /* PHY VDL delay monitoring control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF 0x00000070 /* PHY VDL delay monitoring reference register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS 0x00000074 /* PHY VDL delay monitoring status register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE 0x00000078 /* PHY VDL delay monitoring override register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL 0x0000007c /* PHY VDL delay monitoring output control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS 0x00000080 /* PHY VDL delay monitoring output status register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR 0x00000084 /* PHY VDL delay monitoring output status clear register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00 0x00000090 /* DDR interface signal AD[00] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01 0x00000094 /* DDR interface signal AD[01] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02 0x00000098 /* DDR interface signal AD[02] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03 0x0000009c /* DDR interface signal AD[03] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04 0x000000a0 /* DDR interface signal AD[04] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05 0x000000a4 /* DDR interface signal AD[05] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06 0x000000a8 /* DDR interface signal AD[06] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07 0x000000ac /* DDR interface signal AD[07] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08 0x000000b0 /* DDR interface signal AD[08] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09 0x000000b4 /* DDR interface signal AD[09] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10 0x000000b8 /* DDR interface signal AD[10] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11 0x000000bc /* DDR interface signal AD[11] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12 0x000000c0 /* DDR interface signal AD[12] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13 0x000000c4 /* DDR interface signal AD[13] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14 0x000000c8 /* DDR interface signal AD[14] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15 0x000000cc /* DDR interface signal AD[15] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0 0x000000d0 /* DDR interface signal BA[0] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1 0x000000d4 /* DDR interface signal BA[1] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2 0x000000d8 /* DDR interface signal BA[2] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0 0x000000dc /* DDR interface signal AUX[0] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1 0x000000e0 /* DDR interface signal AUX[1] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2 0x000000e4 /* DDR interface signal AUX[2] VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0 0x000000e8 /* DDR interface signal CS0 VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1 0x000000ec /* DDR interface signal CS1 VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR 0x000000f0 /* DDR interface signal PAR VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N 0x000000f4 /* DDR interface signal RAS_N VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N 0x000000f8 /* DDR interface signal CAS_N VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE 0x000000fc /* DDR interface signal CKE0 VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N 0x00000100 /* DDR interface signal RST_N VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT 0x00000104 /* DDR interface signal ODT0 VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N 0x00000108 /* DDR interface signal WE_N VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P 0x0000010c /* DDR interface signal DDR_CK-P VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N 0x00000110 /* DDR interface signal DDR_CK-N VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL 0x00000114 /* DDR interface signal Write Leveling CLK VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL 0x00000118 /* DDR interface signal Write Leveling Capture Enable VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH      0x00000130 /* Refresh engine controller */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL   0x00000134 /* Update VDL control register */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1 0x00000138 /* Update VDL snoop control register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2 0x0000013c /* Update VDL snoop control register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1 0x00000140 /* DRAM Command Register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1 0x00000144 /* DRAM AUX_N Command Register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2 0x00000148 /* DRAM Command Register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2 0x0000014c /* DRAM AUX_N Command Register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3 0x00000150 /* DRAM Command Register #3 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3 0x00000154 /* DRAM AUX_N Command Register #3 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4 0x00000158 /* DRAM Command Register #4 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4 0x0000015c /* DRAM AUX_N Command Register #4 */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER 0x00000160 /* DRAM Command Timer Register */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0    0x00000164 /* DDR3/DDR4/GDDR5 Mode Register 0 and LPDDR Mode Register 1 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1    0x00000168 /* DDR3/DDR4/GDDR5 Mode Register 1 and LPDDR Mode Register 2 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2    0x0000016c /* DDR3/DDR4/GDDR5 Mode Register 2 and LPDDR Mode Register 3 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3    0x00000170 /* DDR3/DDR4/GDDR5 Mode Register 3 and LPDDR Mode Register 9 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4    0x00000174 /* DDR4/GDDR5 Mode Register 4 and LPDDR Mode Register 10 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5    0x00000178 /* DDR4/GDDR5 Mode Register 5 and LPDDR Mode Register 16 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6    0x0000017c /* DDR4/GDDR5 Mode Register 6 and LPDDR Mode Register 17 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7    0x00000180 /* DDR4/GDDR5 Mode Register 7 and LPDDR Mode Register 41 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8    0x00000184 /* GDDR5 Mode Register 8 and LPDDR Mode Register 42 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15   0x00000188 /* GDDR5 Mode Register 15 and LPDDR Mode Register 48 */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63   0x0000018c /* LPDDR Mode Register 63 */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR  0x00000190 /* DDR4 Alert status clear register */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS 0x00000194 /* DDR4 Alert status register */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY    0x00000198 /* DDR4 CA parity control register */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL 0x0000019c /* GDDR5 CA playback control register */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0 0x000001a0 /* LPDDR3 and GDDR5 CA playback status register0 */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL 0x000001ac /* Write leveling control register */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS 0x000001b0 /* Write leveling status register */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL 0x000001b4 /* Read enable test cycle control register */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS 0x000001b8 /* Read enable test cycle status register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED 0x000001c0 /* Traffic generator seed register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1 0x000001c4 /* Traffic generator address register #1 */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2 0x000001c8 /* Traffic generator address register #2 */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL 0x000001cc /* Traffic generator control register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL 0x000001d0 /* Traffic generator data control register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK 0x000001d4 /* Traffic generator DQ mask register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK 0x000001d8 /* Traffic generator ECC DQ mask register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS 0x000001dc /* Traffic generator status register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS 0x000001e0 /* Traffic generator DQ status register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS 0x000001e4 /* Traffic generator ECC DQ status register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL 0x000001e8 /* Traffic generator error count control register */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS 0x000001ec /* Traffic generator error count status register */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL 0x000001f0 /* Virtual VTT Control and Status register */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS 0x000001f4 /* Virtual VTT Control and Status register */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS 0x000001f8 /* Virtual VTT Connections register */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE 0x000001fc /* Virtual VTT Override register */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL 0x00000200 /* VREF DAC Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL 0x00000204 /* PhyBist Control Register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED 0x00000208 /* PhyBist Seed Register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK 0x0000020c /* PhyBist Command/Address Bus Mask */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS 0x00000210 /* PhyBist General Status Register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS 0x00000214 /* PhyBist Per-Bit Control Pad Status Register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS 0x00000218 /* PhyBist Byte Lane #0 Status Register */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS 0x0000021c /* PhyBist Byte Lane #1 Status Register */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL 0x00000230 /* Standby Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE 0x00000234 /* Freeze-on-error enable register */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL 0x00000238 /* Debug Mux Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL    0x0000023c /* DFI Interface Ownership Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL 0x00000240 /* Write ODT Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL 0x00000244 /* ABI and PAR Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL       0x00000248 /* ZQ Calibration Control register */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL 0x0000024c /* Ring-Osc control register */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS 0x00000250 /* Ring-Osc count register */
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG 0x0000011c /* Address and Control Spare register */

/***************************************************************************
 *DDR34_CORE_PHY_BYTE_LANE_0 - DDR34 CORE DDR34 Byte Lane #0 control registers
 ***************************************************************************/
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P 0x00000400 /* Write channel DQS-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N 0x00000404 /* Write channel DQS-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0 0x00000408 /* Write channel DQ0 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1 0x0000040c /* Write channel DQ1 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2 0x00000410 /* Write channel DQ2 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3 0x00000414 /* Write channel DQ3 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4 0x00000418 /* Write channel DQ4 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5 0x0000041c /* Write channel DQ5 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6 0x00000420 /* Write channel DQ6 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7 0x00000424 /* Write channel DQ7 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM 0x00000428 /* Write channel DM VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC 0x0000042c /* Write channel EDC VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP 0x00000430 /* Read channel DQSP VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN 0x00000434 /* Read channel DQSP VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P 0x00000438 /* Read channel DQ0-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N 0x0000043c /* Read channel DQ0-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P 0x00000440 /* Read channel DQ1-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N 0x00000444 /* Read channel DQ1-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P 0x00000448 /* Read channel DQ2-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N 0x0000044c /* Read channel DQ2-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P 0x00000450 /* Read channel DQ3-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N 0x00000454 /* Read channel DQ3-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P 0x00000458 /* Read channel DQ4-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N 0x0000045c /* Read channel DQ4-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P 0x00000460 /* Read channel DQ5-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N 0x00000464 /* Read channel DQ5-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P 0x00000468 /* Read channel DQ6-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N 0x0000046c /* Read channel DQ6-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P 0x00000470 /* Read channel DQ7-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N 0x00000474 /* Read channel DQ7-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP 0x00000478 /* Read channel DM-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN 0x0000047c /* Read channel DM-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP 0x00000480 /* Read channel EDC-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN 0x00000484 /* Read channel EDC-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0 0x00000488 /* Read channel CS_N[0] read enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1 0x0000048c /* Read channel CS_N[1] read enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL 0x00000490 /* DDR interface signal Write Leveling CLK VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL 0x00000494 /* DDR interface signal Write Leveling Capture Enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC 0x000004a0 /* Read enable bit-clock cycle delay control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC 0x000004a4 /* Write leveling bit-clock cycle delay control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL  0x000004b0 /* Read channel datapath control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR 0x000004b4 /* Read fifo address pointer register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DATA 0x000004b8 /* Read fifo data register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI 0x000004bc /* Read fifo dm/dbi register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS 0x000004c0 /* Read fifo status register */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR 0x000004c4 /* Read fifo status clear register */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL 0x000004c8 /* Idle mode SSTL pad control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL 0x000004cc /* SSTL pad drive characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL 0x000004d0 /* SSTL read enable pad drive characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL 0x000004d4 /* pad rx and tx characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE 0x000004d8 /* Write cycle preamble control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL   0x000004e0 /* Read channel ODT control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL 0x000004f0 /* GDDR5M EDC digital phase detector control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS 0x000004f4 /* GDDR5M EDC digital phase detector status register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL 0x000004f8 /* GDDR5M EDC digital phase detector output signal control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS 0x000004fc /* GDDR5M EDC digital phase detector output signal status register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR 0x00000500 /* GDDR5M EDC digital phase detector output signal status clear register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL 0x00000504 /* GDDR5M EDC signal path CRC control register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS 0x00000508 /* GDDR5M EDC signal path CRC status register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT 0x0000050c /* GDDR5M EDC signal path CRC counter register */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR 0x00000510 /* GDDR5M EDC signal path CRC counter register */
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG  0x00000514 /* Byte-Lane Spare register */

/***************************************************************************
 *DDR34_CORE_PHY_BYTE_LANE_1 - DDR34 CORE DDR34 Byte Lane #1 control registers
 ***************************************************************************/
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P 0x00000600 /* Write channel DQS-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N 0x00000604 /* Write channel DQS-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0 0x00000608 /* Write channel DQ0 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1 0x0000060c /* Write channel DQ1 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2 0x00000610 /* Write channel DQ2 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3 0x00000614 /* Write channel DQ3 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4 0x00000618 /* Write channel DQ4 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5 0x0000061c /* Write channel DQ5 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6 0x00000620 /* Write channel DQ6 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7 0x00000624 /* Write channel DQ7 VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM 0x00000628 /* Write channel DM VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC 0x0000062c /* Write channel EDC VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP 0x00000630 /* Read channel DQSP VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN 0x00000634 /* Read channel DQSP VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P 0x00000638 /* Read channel DQ0-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N 0x0000063c /* Read channel DQ0-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P 0x00000640 /* Read channel DQ1-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N 0x00000644 /* Read channel DQ1-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P 0x00000648 /* Read channel DQ2-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N 0x0000064c /* Read channel DQ2-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P 0x00000650 /* Read channel DQ3-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N 0x00000654 /* Read channel DQ3-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P 0x00000658 /* Read channel DQ4-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N 0x0000065c /* Read channel DQ4-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P 0x00000660 /* Read channel DQ5-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N 0x00000664 /* Read channel DQ5-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P 0x00000668 /* Read channel DQ6-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N 0x0000066c /* Read channel DQ6-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P 0x00000670 /* Read channel DQ7-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N 0x00000674 /* Read channel DQ7-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP 0x00000678 /* Read channel DM-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN 0x0000067c /* Read channel DM-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP 0x00000680 /* Read channel EDC-P VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN 0x00000684 /* Read channel EDC-N VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0 0x00000688 /* Read channel CS_N[0] read enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1 0x0000068c /* Read channel CS_N[1] read enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL 0x00000690 /* DDR interface signal Write Leveling CLK VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL 0x00000694 /* DDR interface signal Write Leveling Capture Enable VDL control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC 0x000006a0 /* Read enable bit-clock cycle delay control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC 0x000006a4 /* Write leveling bit-clock cycle delay control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL  0x000006b0 /* Read channel datapath control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR 0x000006b4 /* Read fifo address pointer register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DATA 0x000006b8 /* Read fifo data register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI 0x000006bc /* Read fifo dm/dbi register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS 0x000006c0 /* Read fifo status register */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR 0x000006c4 /* Read fifo status clear register */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL 0x000006c8 /* Idle mode SSTL pad control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL 0x000006cc /* SSTL pad drive characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL 0x000006d0 /* SSTL read enable pad drive characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL 0x000006d4 /* pad rx and tx characteristics control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE 0x000006d8 /* Write cycle preamble control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL   0x000006e0 /* Read channel ODT control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL 0x000006f0 /* GDDR5M EDC digital phase detector control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS 0x000006f4 /* GDDR5M EDC digital phase detector status register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL 0x000006f8 /* GDDR5M EDC digital phase detector output signal control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS 0x000006fc /* GDDR5M EDC digital phase detector output signal status register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR 0x00000700 /* GDDR5M EDC digital phase detector output signal status clear register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL 0x00000704 /* GDDR5M EDC signal path CRC control register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS 0x00000708 /* GDDR5M EDC signal path CRC status register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT 0x0000070c /* GDDR5M EDC signal path CRC counter register */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR 0x00000710 /* GDDR5M EDC signal path CRC counter register */
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG  0x00000714 /* Byte-Lane Spare register */

/***************************************************************************
 *REVISION - Address & Control revision register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: reserved0 [31:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_reserved0_MASK        0xfe000000
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_reserved0_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_reserved0_BITS        7
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_reserved0_SHIFT       25

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: PERFORMANCE [24:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_PERFORMANCE_MASK      0x01800000
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_PERFORMANCE_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_PERFORMANCE_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_PERFORMANCE_SHIFT     23

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: TECHNOLOGY [22:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_TECHNOLOGY_MASK       0x00700000
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_TECHNOLOGY_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_TECHNOLOGY_BITS       3
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_TECHNOLOGY_SHIFT      20

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: WB [19:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_WB_MASK               0x00080000
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_WB_ALIGN              0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_WB_BITS               1
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_WB_SHIFT              19

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: BITS [18:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_BITS_MASK             0x00070000
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_BITS_ALIGN            0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_BITS_BITS             3
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_BITS_SHIFT            16

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: MAJOR [15:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MAJOR_MASK            0x0000ff00
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MAJOR_ALIGN           0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MAJOR_BITS            8
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MAJOR_SHIFT           8
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MAJOR_DEFAULT         0x000000e2

/* DDR34_CORE_PHY_CONTROL_REGS :: REVISION :: MINOR [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MINOR_MASK            0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MINOR_ALIGN           0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MINOR_BITS            8
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MINOR_SHIFT           0
#define DDR34_CORE_PHY_CONTROL_REGS_REVISION_MINOR_DEFAULT         0x00000001

/***************************************************************************
 *PLL_STATUS - PHY PLL status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: reserved0 [31:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved0_MASK      0xff000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved0_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved0_BITS      8
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved0_SHIFT     24

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: CLOCK_GEN [23:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCK_GEN_MASK      0x00f00000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCK_GEN_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCK_GEN_BITS      4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCK_GEN_SHIFT     20
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCK_GEN_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: reserved1 [19:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved1_MASK      0x000e0000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved1_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved1_BITS      3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_reserved1_SHIFT     17

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: LOCK_LOST [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_LOST_MASK      0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_LOST_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_LOST_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_LOST_SHIFT     16

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: CLOCKING_8X [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_8X_MASK    0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_8X_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_8X_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_8X_SHIFT   15

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: CLOCKING_4X [14:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_4X_MASK    0x00004000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_4X_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_4X_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_4X_SHIFT   14

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: CLOCKING_2X [13:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_2X_MASK    0x00002000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_2X_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_2X_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_CLOCKING_2X_SHIFT   13

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: STATUS [12:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_STATUS_MASK         0x00001ffe
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_STATUS_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_STATUS_BITS         12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_STATUS_SHIFT        1

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_STATUS :: LOCK [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_MASK           0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS_LOCK_SHIFT          0

/***************************************************************************
 *PLL_CONFIG - PHY PLL configuration register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: reserved0 [31:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved0_MASK      0xf0000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved0_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved0_BITS      4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved0_SHIFT     28

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: reserved_for_eco1 [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved_for_eco1_MASK 0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved_for_eco1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved_for_eco1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved_for_eco1_SHIFT 27
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved_for_eco1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: CK_LDO_REF_CTRL [26:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_REF_CTRL_MASK 0x06000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_REF_CTRL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_REF_CTRL_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_REF_CTRL_SHIFT 25
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_REF_CTRL_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: CK_LDO_BIAS [24:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_BIAS_MASK    0x01800000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_BIAS_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_BIAS_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_BIAS_SHIFT   23
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_CK_LDO_BIAS_DEFAULT 0x00000003

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: PLL_LDO_REF_SEL [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_SEL_MASK 0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_SEL_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_SEL_SHIFT 22
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: PLL_LDO_REF_CTRL [21:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_CTRL_MASK 0x00300000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_CTRL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_CTRL_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_CTRL_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_REF_CTRL_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: PLL_LDO_BIAS [19:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_BIAS_MASK   0x000c0000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_BIAS_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_BIAS_BITS   2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_BIAS_SHIFT  18
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PLL_LDO_BIAS_DEFAULT 0x00000003

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: HOLD [17:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_HOLD_MASK           0x00020000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_HOLD_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_HOLD_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_HOLD_SHIFT          17
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_HOLD_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: ENABLE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_ENABLE_MASK         0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_ENABLE_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_ENABLE_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_ENABLE_SHIFT        16
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_ENABLE_DEFAULT      0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: reserved2 [15:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved2_MASK      0x0000c000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved2_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved2_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved2_SHIFT     14

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: FB_OFFSET [13:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_FB_OFFSET_MASK      0x00003f00
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_FB_OFFSET_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_FB_OFFSET_BITS      6
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_FB_OFFSET_SHIFT     8
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_FB_OFFSET_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: reserved3 [07:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved3_MASK      0x000000e0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved3_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved3_BITS      3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved3_SHIFT     5

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: RESET_POST_DIV [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_POST_DIV_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_POST_DIV_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_POST_DIV_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_POST_DIV_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_POST_DIV_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: reserved4 [03:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved4_MASK      0x0000000c
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved4_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved4_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_reserved4_SHIFT     2

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: RESET [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_MASK          0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_SHIFT         1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_RESET_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONFIG :: PWRDN [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PWRDN_MASK          0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PWRDN_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PWRDN_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PWRDN_SHIFT         0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG_PWRDN_DEFAULT       0x00000000

/***************************************************************************
 *PLL_CONTROL1 - PHY PLL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL1 :: reserved0 [31:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_reserved0_MASK    0xfffffc00
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_reserved0_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_reserved0_BITS    22
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_reserved0_SHIFT   10

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL1 :: I_KP [09:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KP_MASK         0x000003c0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KP_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KP_BITS         4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KP_SHIFT        6
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KP_DEFAULT      0x00000005

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL1 :: I_KI [05:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KI_MASK         0x00000038
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KI_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KI_BITS         3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KI_SHIFT        3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KI_DEFAULT      0x00000002

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL1 :: I_KA [02:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KA_MASK         0x00000007
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KA_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KA_BITS         3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KA_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL1_I_KA_DEFAULT      0x00000000

/***************************************************************************
 *PLL_CONTROL2 - PHY PLL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: VCO_RANGE [31:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_RANGE_MASK    0xc0000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_RANGE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_RANGE_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_RANGE_SHIFT   30
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_RANGE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: UNUSED2 [29:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED2_MASK      0x20000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED2_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED2_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED2_SHIFT     29
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED2_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: NDIV_RELOCK [28:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_NDIV_RELOCK_MASK  0x10000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_NDIV_RELOCK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_NDIV_RELOCK_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_NDIV_RELOCK_SHIFT 28
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_NDIV_RELOCK_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: FAST_LOCK [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_FAST_LOCK_MASK    0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_FAST_LOCK_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_FAST_LOCK_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_FAST_LOCK_SHIFT   27
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_FAST_LOCK_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: VCO_FB_DIV2 [26:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_FB_DIV2_MASK  0x04000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_FB_DIV2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_FB_DIV2_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_FB_DIV2_SHIFT 26
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_VCO_FB_DIV2_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: POST_CTRL_RESETB [25:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_POST_CTRL_RESETB_MASK 0x03000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_POST_CTRL_RESETB_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_POST_CTRL_RESETB_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_POST_CTRL_RESETB_SHIFT 24
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_POST_CTRL_RESETB_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: PWM_RATE [23:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_PWM_RATE_MASK     0x00c00000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_PWM_RATE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_PWM_RATE_BITS     2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_PWM_RATE_SHIFT    22
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_PWM_RATE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: STAT_MODE [21:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_MODE_MASK    0x00300000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_MODE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_MODE_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_MODE_SHIFT   20
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: UNUSED1 [19:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED1_MASK      0x000c0000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED1_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED1_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED1_SHIFT     18
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_UNUSED1_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: STAT_UPDATE [17:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_UPDATE_MASK  0x00020000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_UPDATE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_UPDATE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_UPDATE_SHIFT 17
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_UPDATE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: STAT_SELECT [16:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_SELECT_MASK  0x0001c000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_SELECT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_SELECT_BITS  3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_SELECT_SHIFT 14
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_SELECT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: STAT_RESET [13:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_RESET_MASK   0x00002000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_RESET_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_RESET_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_RESET_SHIFT  13
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_STAT_RESET_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: DCO_CTRL_BYPASS_ENABLE [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ENABLE_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ENABLE_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL2 :: DCO_CTRL_BYPASS [11:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_MASK 0x00000fff
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2_DCO_CTRL_BYPASS_DEFAULT 0x00000000

/***************************************************************************
 *PLL_CONTROL3 - PHY PLL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_CONTROL3 :: PLL_CONTROL [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3_PLL_CONTROL_MASK  0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3_PLL_CONTROL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3_PLL_CONTROL_BITS  32
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3_PLL_CONTROL_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL3_PLL_CONTROL_DEFAULT 0x00000000

/***************************************************************************
 *PLL_DIVIDERS - PHY PLL integer divider register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: reserved0 [31:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved0_MASK    0xf0000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved0_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved0_BITS    4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved0_SHIFT   28

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: MDIV [27:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_MDIV_MASK         0x0ff00000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_MDIV_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_MDIV_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_MDIV_SHIFT        20
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_MDIV_DEFAULT      0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: reserved1 [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved1_MASK    0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved1_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved1_BITS    4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved1_SHIFT   16

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: PDIV [15:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_PDIV_MASK         0x0000f000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_PDIV_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_PDIV_BITS         4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_PDIV_SHIFT        12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_PDIV_DEFAULT      0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: reserved2 [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved2_MASK    0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved2_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved2_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_reserved2_SHIFT   10

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_DIVIDERS :: NDIV_INT [09:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_NDIV_INT_MASK     0x000003ff
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_NDIV_INT_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_NDIV_INT_BITS     10
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_NDIV_INT_SHIFT    0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS_NDIV_INT_DEFAULT  0x00000020

/***************************************************************************
 *PLL_FRAC_DIVIDER - PHY PLL fractional divider register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_FRAC_DIVIDER :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_FRAC_DIVIDER :: NDIV_FRAC [19:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_NDIV_FRAC_MASK 0x000fffff
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_NDIV_FRAC_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_NDIV_FRAC_BITS 20
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_NDIV_FRAC_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER_NDIV_FRAC_DEFAULT 0x00000000

/***************************************************************************
 *PLL_SS_CONTROL - PHY PLL spread spectrum control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_CONTROL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved0_MASK  0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved0_BITS  12
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_CONTROL :: SSC_STEP [19:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_STEP_MASK   0x000ffff0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_STEP_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_STEP_BITS   16
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_STEP_SHIFT  4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_STEP_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_CONTROL :: reserved1 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved1_MASK  0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved1_BITS  3
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_reserved1_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_CONTROL :: SSC_MODE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_MODE_MASK   0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_MODE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_MODE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_MODE_SHIFT  0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_CONTROL_SSC_MODE_DEFAULT 0x00000000

/***************************************************************************
 *PLL_SS_LIMIT - PHY PLL spread spectrum limit register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_LIMIT :: reserved0 [31:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved0_MASK    0xfc000000
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved0_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved0_BITS    6
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved0_SHIFT   26

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_LIMIT :: SSC_LIMIT [25:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_SSC_LIMIT_MASK    0x03fffff0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_SSC_LIMIT_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_SSC_LIMIT_BITS    22
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_SSC_LIMIT_SHIFT   4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_SSC_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PLL_SS_LIMIT :: reserved1 [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved1_MASK    0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved1_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved1_BITS    4
#define DDR34_CORE_PHY_CONTROL_REGS_PLL_SS_LIMIT_reserved1_SHIFT   0

/***************************************************************************
 *AUX_CONTROL - Aux Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: reserved0 [31:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved0_MASK     0xffe00000
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved0_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved0_BITS     11
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved0_SHIFT    21

/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: IS_ODT [20:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_ODT_MASK        0x001f0000
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_ODT_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_ODT_BITS        5
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_ODT_SHIFT       16
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_ODT_DEFAULT     0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved1_MASK     0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved1_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved1_BITS     3
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved1_SHIFT    13

/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: IS_CS [12:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_CS_MASK         0x00001f00
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_CS_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_CS_BITS         5
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_CS_SHIFT        8
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_CS_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: reserved2 [07:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved2_MASK     0x000000e0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved2_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved2_BITS     3
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_reserved2_SHIFT    5

/* DDR34_CORE_PHY_CONTROL_REGS :: AUX_CONTROL :: IS_AD [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_AD_MASK         0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_AD_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_AD_BITS         5
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_AD_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_AUX_CONTROL_IS_AD_DEFAULT      0x00000000

/***************************************************************************
 *IDLE_PAD_CONTROL - Idle mode pad control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: IDLE [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDLE_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDLE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDLE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDLE_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDLE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: DIB_MODE [30:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DIB_MODE_MASK 0x40000000
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DIB_MODE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DIB_MODE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DIB_MODE_SHIFT 30
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DIB_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: reserved0 [29:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_reserved0_MASK 0x3ffffff0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_reserved0_BITS 26
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_reserved0_SHIFT 4

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: RXENB [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_RXENB_MASK    0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_RXENB_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_RXENB_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_RXENB_SHIFT   3
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_RXENB_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: IDDQ [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDDQ_MASK     0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDDQ_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDDQ_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDDQ_SHIFT    2
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_IDDQ_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: DOUT_N [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_N_MASK   0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_N_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_N_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_N_SHIFT  1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_N_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_CONTROL :: DOUT_P [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_P_MASK   0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_P_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_P_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_P_SHIFT  0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_CONTROL_DOUT_P_DEFAULT 0x00000000

/***************************************************************************
 *IDLE_PAD_ENABLE0 - Idle mode pad enable register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_ENABLE0 :: reserved0 [31:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_reserved0_MASK 0xfffff800
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_reserved0_BITS 21
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_reserved0_SHIFT 11

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_ENABLE0 :: IO_IDLE_ENABLE [10:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_IO_IDLE_ENABLE_MASK 0x000007ff
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_IO_IDLE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_IO_IDLE_ENABLE_BITS 11
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_IO_IDLE_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0_IO_IDLE_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *IDLE_PAD_ENABLE1 - Idle mode pad enable register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_ENABLE1 :: reserved0 [31:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_reserved0_MASK 0xffc00000
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_reserved0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_reserved0_SHIFT 22

/* DDR34_CORE_PHY_CONTROL_REGS :: IDLE_PAD_ENABLE1 :: IO_IDLE_ENABLE [21:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_IO_IDLE_ENABLE_MASK 0x003fffff
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_IO_IDLE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_IO_IDLE_ENABLE_BITS 22
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_IO_IDLE_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1_IO_IDLE_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *DRIVE_PAD_CTL - PVT Compensation control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: reserved0 [31:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_reserved0_MASK   0xc0000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_reserved0_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_reserved0_BITS   2
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_reserved0_SHIFT  30

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_PD_IDLE_STRENGTH [29:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_IDLE_STRENGTH_MASK 0x3e000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_IDLE_STRENGTH_SHIFT 25
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_ND_IDLE_STRENGTH [24:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_IDLE_STRENGTH_MASK 0x01f00000
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_IDLE_STRENGTH_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_PD_TERM_STRENGTH [19:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_TERM_STRENGTH_MASK 0x000f8000
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_TERM_STRENGTH_SHIFT 15
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_ND_TERM_STRENGTH [14:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_TERM_STRENGTH_MASK 0x00007c00
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_TERM_STRENGTH_SHIFT 10
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_PD_STRENGTH [09:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_STRENGTH_MASK 0x000003e0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_STRENGTH_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_CONTROL_REGS :: DRIVE_PAD_CTL :: ADDR_CTL_ND_STRENGTH [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_STRENGTH_MASK 0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_STRENGTH_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ADDR_CTL_ND_STRENGTH_DEFAULT 0x0000001f

/***************************************************************************
 *STATIC_PAD_CTL - pad rx and tx characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: reserved0 [31:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved0_MASK  0xf0000000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved0_BITS  4
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved0_SHIFT 28

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: AUTO_OEB [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_AUTO_OEB_MASK   0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_AUTO_OEB_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_AUTO_OEB_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_AUTO_OEB_SHIFT  27
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_AUTO_OEB_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_GDDR5 [26:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_GDDR5_MASK 0x04000000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_GDDR5_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_GDDR5_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_GDDR5_SHIFT 26
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_GDDR5_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_LPDDR [25:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_LPDDR_MASK 0x02000000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_LPDDR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_LPDDR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_LPDDR_SHIFT 25
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_LPDDR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_CLK1 [24:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK1_MASK  0x01000000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK1_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK1_SHIFT 24
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_CLK0 [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK0_MASK  0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK0_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK0_SHIFT 23
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CLK0_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_ODT [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_ODT_MASK   0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_ODT_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_ODT_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_ODT_SHIFT  22
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_ODT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_PAR [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_PAR_MASK   0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_PAR_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_PAR_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_PAR_SHIFT  21
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_PAR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_BA [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_BA_MASK    0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_BA_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_BA_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_BA_SHIFT   20
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_BA_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_AUX2 [19:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX2_MASK  0x00080000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX2_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX2_SHIFT 19
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX2_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_AUX1 [18:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX1_MASK  0x00040000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX1_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX1_SHIFT 18
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_AUX0 [17:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX0_MASK  0x00020000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX0_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX0_SHIFT 17
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_AUX0_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_CS1 [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CS1_MASK   0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CS1_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CS1_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CS1_SHIFT  16
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_CS1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A15 [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A15_MASK   0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A15_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A15_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A15_SHIFT  15
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A15_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A14 [14:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A14_MASK   0x00004000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A14_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A14_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A14_SHIFT  14
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A14_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A13 [13:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A13_MASK   0x00002000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A13_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A13_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A13_SHIFT  13
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A13_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A12 [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A12_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A12_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A12_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A12_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A12_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A11 [11:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A11_MASK   0x00000800
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A11_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A11_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A11_SHIFT  11
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A11_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A10 [10:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A10_MASK   0x00000400
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A10_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A10_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A10_SHIFT  10
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A10_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: IDDQ_A09 [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A09_MASK   0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A09_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A09_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A09_SHIFT  9
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_IDDQ_A09_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: reserved1 [08:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved1_MASK  0x000001fc
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved1_BITS  7
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_reserved1_SHIFT 2

/* DDR34_CORE_PHY_CONTROL_REGS :: STATIC_PAD_CTL :: RX_MODE [01:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_RX_MODE_MASK    0x00000003
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_RX_MODE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_RX_MODE_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_RX_MODE_SHIFT   0
#define DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL_RX_MODE_DEFAULT 0x00000000

/***************************************************************************
 *DRAM_CONFIG - DRAM configuration register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: INIT_MODE [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_INIT_MODE_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_INIT_MODE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_INIT_MODE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_INIT_MODE_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_INIT_MODE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: reserved0 [30:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved0_MASK     0x70000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved0_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved0_BITS     3
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved0_SHIFT    28

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: ECC_ENABLED [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ECC_ENABLED_MASK   0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ECC_ENABLED_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ECC_ENABLED_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ECC_ENABLED_SHIFT  27
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ECC_ENABLED_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: SPLIT_DQ_BUS [26:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_SPLIT_DQ_BUS_MASK  0x04000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_SPLIT_DQ_BUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_SPLIT_DQ_BUS_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_SPLIT_DQ_BUS_SHIFT 26
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_SPLIT_DQ_BUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: BUS16 [25:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS16_MASK         0x02000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS16_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS16_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS16_SHIFT        25
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS16_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: BUS8 [24:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS8_MASK          0x01000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS8_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS8_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS8_SHIFT         24
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BUS8_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: reserved1 [23:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved1_MASK     0x00ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved1_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved1_BITS     8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_reserved1_SHIFT    16

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: EDC_MODE [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_EDC_MODE_MASK      0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_EDC_MODE_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_EDC_MODE_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_EDC_MODE_SHIFT     15
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_EDC_MODE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: RDQS_MODE [14:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_RDQS_MODE_MASK     0x00004000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_RDQS_MODE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_RDQS_MODE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_RDQS_MODE_SHIFT    14
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_RDQS_MODE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: GROUP_BITS [13:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_GROUP_BITS_MASK    0x00003000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_GROUP_BITS_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_GROUP_BITS_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_GROUP_BITS_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_GROUP_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: BANK_BITS [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BANK_BITS_MASK     0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BANK_BITS_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BANK_BITS_BITS     2
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BANK_BITS_SHIFT    10
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_BANK_BITS_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: COL_BITS [09:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_COL_BITS_MASK      0x00000300
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_COL_BITS_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_COL_BITS_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_COL_BITS_SHIFT     8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_COL_BITS_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: ROW_BITS [07:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ROW_BITS_MASK      0x000000f0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ROW_BITS_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ROW_BITS_BITS      4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ROW_BITS_SHIFT     4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_ROW_BITS_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_CONFIG :: DRAM_TYPE [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_DRAM_TYPE_MASK     0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_DRAM_TYPE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_DRAM_TYPE_BITS     4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_DRAM_TYPE_SHIFT    0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG_DRAM_TYPE_DEFAULT  0x00000000

/***************************************************************************
 *DRAM_TIMING1 - DRAM timing register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING1 :: TRAS [31:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRAS_MASK         0xff000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRAS_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRAS_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRAS_SHIFT        24
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRAS_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING1 :: TRRD [23:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRRD_MASK         0x00ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRRD_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRRD_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRRD_SHIFT        16
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRRD_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING1 :: TRP [15:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRP_MASK          0x0000ff00
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRP_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRP_BITS          8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRP_SHIFT         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRP_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING1 :: TRCD [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRCD_MASK         0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRCD_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRCD_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRCD_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1_TRCD_DEFAULT      0x00000000

/***************************************************************************
 *DRAM_TIMING2 - DRAM timing register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING2 :: TRTP [31:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TRTP_MASK         0xff000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TRTP_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TRTP_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TRTP_SHIFT        24
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TRTP_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING2 :: TWR [23:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TWR_MASK          0x00ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TWR_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TWR_BITS          8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TWR_SHIFT         16
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TWR_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING2 :: TCWL [15:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCWL_MASK         0x0000ff00
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCWL_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCWL_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCWL_SHIFT        8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCWL_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING2 :: TCAS [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCAS_MASK         0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCAS_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCAS_BITS         8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCAS_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2_TCAS_DEFAULT      0x00000000

/***************************************************************************
 *DRAM_TIMING3 - DRAM timing register #3
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING3 :: reserved0 [31:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_reserved0_MASK    0xff000000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_reserved0_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_reserved0_BITS    8
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_reserved0_SHIFT   24

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING3 :: TCAL [23:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TCAL_MASK         0x00f00000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TCAL_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TCAL_BITS         4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TCAL_SHIFT        20
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TCAL_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING3 :: TRTW [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRTW_MASK         0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRTW_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRTW_BITS         4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRTW_SHIFT        16
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRTW_DEFAULT      0x00000004

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING3 :: TWTR [15:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TWTR_MASK         0x0000f000
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TWTR_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TWTR_BITS         4
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TWTR_SHIFT        12
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TWTR_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING3 :: TRFC [11:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRFC_MASK         0x00000fff
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRFC_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRFC_BITS         12
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRFC_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3_TRFC_DEFAULT      0x00000000

/***************************************************************************
 *DRAM_TIMING4 - DRAM timing register #4
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DRAM_TIMING4 :: temp [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4_temp_MASK         0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4_temp_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4_temp_BITS         32
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4_temp_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4_temp_DEFAULT      0x00000000

/***************************************************************************
 *VDL_CALIBRATE - PHY VDL calibration control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: reserved0 [31:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_reserved0_MASK   0xffffffc0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_reserved0_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_reserved0_BITS   26
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_reserved0_SHIFT  6

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: HALF_STEPS [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_HALF_STEPS_MASK  0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_HALF_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_HALF_STEPS_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_HALF_STEPS_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_HALF_STEPS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: UPDATE_FAST [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_FAST_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_FAST_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_FAST_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_FAST_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_FAST_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: UPDATE_REGS [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_REGS_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_REGS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_REGS_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_REGS_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_UPDATE_REGS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: CALIB_FTM2 [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_FTM2_MASK  0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_FTM2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_FTM2_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_FTM2_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_FTM2_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: CALIB_PHYBIST [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_PHYBIST_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_PHYBIST_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_PHYBIST_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_PHYBIST_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_PHYBIST_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIBRATE :: CALIB_ONCE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_ONCE_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_ONCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_ONCE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_ONCE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE_CALIB_ONCE_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CALIB_STATUS1 - PHY VDL calibration status register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: reserved0 [31:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved0_MASK 0xfffc0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved0_SHIFT 18

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_TOTAL_STEPS [17:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_TOTAL_STEPS_MASK 0x0003ff00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_TOTAL_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_TOTAL_STEPS_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_TOTAL_STEPS_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_TOTAL_STEPS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: reserved1 [07:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved1_MASK 0x000000c0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_reserved1_SHIFT 6

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_BUS_ERROR [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_BUS_ERROR_MASK 0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_BUS_ERROR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_BUS_ERROR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_BUS_ERROR_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_BUS_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_REGS_DONE [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_REGS_DONE_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_REGS_DONE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_REGS_DONE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_REGS_DONE_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_REGS_DONE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_LOCK_6B [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_6B_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_6B_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_6B_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_6B_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_6B_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_LOCK_4B [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_4B_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_4B_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_4B_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_4B_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_4B_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_LOCK_2B [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_2B_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_2B_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_2B_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_2B_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_LOCK_2B_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS1 :: CALIB_IDLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_IDLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_IDLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_IDLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_IDLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1_CALIB_IDLE_DEFAULT 0x00000001

/***************************************************************************
 *VDL_CALIB_STATUS2 - PHY VDL calibration status register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS2 :: reserved0 [31:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved0_MASK 0xffc00000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved0_SHIFT 22

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS2 :: CALIB_4B_STEPS [21:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_4B_STEPS_MASK 0x003ff000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_4B_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_4B_STEPS_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_4B_STEPS_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_4B_STEPS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS2 :: reserved1 [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved1_MASK 0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_reserved1_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CALIB_STATUS2 :: CALIB_2B_STEPS [09:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_2B_STEPS_MASK 0x000003ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_2B_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_2B_STEPS_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_2B_STEPS_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS2_CALIB_2B_STEPS_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_CONTROL - PHY VDL delay monitoring control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: reserved0 [31:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved0_MASK 0xffc00000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved0_SHIFT 22

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: INTERVAL [21:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_INTERVAL_MASK 0x003fff00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_INTERVAL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_INTERVAL_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_INTERVAL_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_INTERVAL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: reserved1 [07:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved1_MASK 0x000000f0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved1_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_reserved1_SHIFT 4

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: UPDATE [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_UPDATE_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_UPDATE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_UPDATE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_UPDATE_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_UPDATE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: FORCE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_FORCE_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_FORCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_FORCE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_FORCE_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: DATA_RATE [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_DATA_RATE_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_DATA_RATE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_DATA_RATE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_DATA_RATE_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_DATA_RATE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_REF - PHY VDL delay monitoring reference register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_REF :: reserved0 [31:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved0_MASK 0xffc00000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved0_SHIFT 22

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_REF :: MONITOR_4B_STEPS [21:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_4B_STEPS_MASK 0x003ff000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_4B_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_4B_STEPS_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_4B_STEPS_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_4B_STEPS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_REF :: reserved1 [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved1_MASK 0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_reserved1_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_REF :: MONITOR_2B_STEPS [09:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_2B_STEPS_MASK 0x000003ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_2B_STEPS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_2B_STEPS_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_2B_STEPS_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_REF_MONITOR_2B_STEPS_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_STATUS - PHY VDL delay monitoring status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: reserved0 [31:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved0_MASK 0xe0000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved0_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved0_SHIFT 29

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: MONITOR_BUS_ERROR [28:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_BUS_ERROR_MASK 0x10000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_BUS_ERROR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_BUS_ERROR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_BUS_ERROR_SHIFT 28
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_BUS_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: reserved1 [27:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved1_MASK 0x0e000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved1_SHIFT 25

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: MONITOR_ADJ [24:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_ADJ_MASK 0x01f00000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_ADJ_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_ADJ_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_ADJ_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_ADJ_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: MONITOR_CHANGE [19:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_CHANGE_MASK 0x000ff000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_CHANGE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_CHANGE_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_CHANGE_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_CHANGE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: reserved2 [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved2_MASK 0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved2_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_reserved2_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_STATUS :: MONITOR_TOTAL [09:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_TOTAL_MASK 0x000003ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_TOTAL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_TOTAL_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_TOTAL_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_STATUS_MONITOR_TOTAL_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_OVERRIDE - PHY VDL delay monitoring override register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_BUSY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_BUSY_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_BUSY_SHIFT 31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_FORCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_FORCE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_FORCE_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: reserved1 [15:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved1_MASK 0x0000fe00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved1_BITS 7
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved1_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: ADJ [08:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ADJ_MASK  0x000001f0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ADJ_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ADJ_BITS  5
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ADJ_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ADJ_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: reserved2 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved2_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_reserved2_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OVERRIDE :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OVERRIDE_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_OUT_CONTROL - PHY VDL delay monitoring output control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_CONTROL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_CONTROL :: LOWER_LIMIT [19:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_LOWER_LIMIT_MASK 0x000ff000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_LOWER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_LOWER_LIMIT_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_LOWER_LIMIT_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_LOWER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_CONTROL :: UPPER_LIMIT [11:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_UPPER_LIMIT_MASK 0x00000ff0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_UPPER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_UPPER_LIMIT_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_UPPER_LIMIT_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_UPPER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_CONTROL :: reserved1 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_reserved1_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_OUT_STATUS - PHY VDL delay monitoring output status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: reserved0 [31:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved0_MASK 0xff000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved0_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved0_SHIFT 24

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: MONITOR_CHANGE [23:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_CHANGE_MASK 0x00ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_CHANGE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_CHANGE_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_CHANGE_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_CHANGE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: reserved1 [15:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved1_MASK 0x0000c000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved1_SHIFT 14

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: MONITOR_TOTAL [13:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_TOTAL_MASK 0x00003ff0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_TOTAL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_TOTAL_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_TOTAL_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_MONITOR_TOTAL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: reserved2 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved2_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_reserved2_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS :: VALID [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_VALID_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_VALID_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_VALID_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_VALID_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_VALID_DEFAULT 0x00000000

/***************************************************************************
 *VDL_MONITOR_OUT_STATUS_CLEAR - PHY VDL delay monitoring output status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_reserved0_BITS 31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_MONITOR_OUT_STATUS_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_CLEAR_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_CLEAR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_CLEAR_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_MONITOR_OUT_STATUS_CLEAR_CLEAR_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD00 - DDR interface signal AD[00] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD00 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD01 - DDR interface signal AD[01] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD01 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD02 - DDR interface signal AD[02] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD02 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD03 - DDR interface signal AD[03] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD03 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD04 - DDR interface signal AD[04] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD04 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD05 - DDR interface signal AD[05] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD05 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD06 - DDR interface signal AD[06] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD06 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD07 - DDR interface signal AD[07] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD07 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD08 - DDR interface signal AD[08] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD08 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD09 - DDR interface signal AD[09] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD09 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD10 - DDR interface signal AD[10] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD10 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD11 - DDR interface signal AD[11] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD11 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD12 - DDR interface signal AD[12] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD12 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD13 - DDR interface signal AD[13] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD13 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD14 - DDR interface signal AD[14] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD14 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AD15 - DDR interface signal AD[15] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AD15 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_BA0 - DDR interface signal BA[0] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA0 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_BA1 - DDR interface signal BA[1] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA1 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_BA2 - DDR interface signal BA[2] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_BA2 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AUX0 - DDR interface signal AUX[0] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX0 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AUX1 - DDR interface signal AUX[1] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX1 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_AUX2 - DDR interface signal AUX[2] VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_AUX2 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_CS0 - DDR interface signal CS0 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS0 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_CS1 - DDR interface signal CS1 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CS1 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_PAR - DDR interface signal PAR VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_PAR :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RAS_N - DDR interface signal RAS_N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_BUSY_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_BUSY_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_BUSY_SHIFT   31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_FORCE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_FORCE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_FORCE_SHIFT  16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RAS_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_CAS_N - DDR interface signal CAS_N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_BUSY_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_BUSY_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_BUSY_SHIFT   31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_FORCE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_FORCE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_FORCE_SHIFT  16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CAS_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_CKE - DDR interface signal CKE0 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_CKE :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RST_N - DDR interface signal RST_N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_BUSY_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_BUSY_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_BUSY_SHIFT   31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_FORCE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_FORCE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_FORCE_SHIFT  16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_RST_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_ODT - DDR interface signal ODT0 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_ODT :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WE_N - DDR interface signal WE_N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_BUSY_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_BUSY_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_BUSY_SHIFT    31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_FORCE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_FORCE_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_FORCE_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_WE_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_DDR_CK_P - DDR interface signal DDR_CK-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_BUSY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_BUSY_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_BUSY_SHIFT 31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_FORCE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_P :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_DDR_CK_N - DDR interface signal DDR_CK-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_BUSY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_BUSY_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_BUSY_SHIFT 31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_FORCE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CONTROL_DDR_CK_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_DDR_CK_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CLK_CONTROL - DDR interface signal Write Leveling CLK VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_CLK_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_CLK_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_LDE_CONTROL - DDR interface signal Write Leveling Capture Enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_BUSY_MASK      0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_BUSY_SHIFT     31
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved0_BITS 14
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_FORCE_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_FORCE_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_FORCE_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_FORCE_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_FORCE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_ADJ_EN_MASK    0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_ADJ_EN_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_ADJ_EN_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_ADJ_EN_SHIFT   12
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: VDL_LDE_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_VDL_STEP_MASK  0x000001ff
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_VDL_STEP_BITS  9
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VDL_LDE_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *REFRESH - Refresh engine controller
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: REFRESH :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_reserved0_MASK         0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_reserved0_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_reserved0_BITS         15
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_reserved0_SHIFT        17

/* DDR34_CORE_PHY_CONTROL_REGS :: REFRESH :: ENABLE [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_ENABLE_MASK            0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_ENABLE_ALIGN           0
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_ENABLE_BITS            1
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_ENABLE_SHIFT           16
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_ENABLE_DEFAULT         0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: REFRESH :: PERIOD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_PERIOD_MASK            0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_PERIOD_ALIGN           0
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_PERIOD_BITS            16
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_PERIOD_SHIFT           0
#define DDR34_CORE_PHY_CONTROL_REGS_REFRESH_PERIOD_DEFAULT         0x00000000

/***************************************************************************
 *UPDATE_VDL - Update VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL :: reserved0 [31:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved0_MASK      0xffffffc0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved0_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved0_BITS      26
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved0_SHIFT     6

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL :: MODE [05:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_MODE_MASK           0x00000030
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_MODE_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_MODE_BITS           2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_MODE_SHIFT          4
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_MODE_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL :: reserved1 [03:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved1_MASK      0x0000000c
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved1_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved1_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_reserved1_SHIFT     2

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL :: DISABLE_INPUT [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_DISABLE_INPUT_MASK  0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_DISABLE_INPUT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_DISABLE_INPUT_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_DISABLE_INPUT_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_DISABLE_INPUT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_ENABLE_MASK         0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_ENABLE_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_ENABLE_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_ENABLE_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_ENABLE_DEFAULT      0x00000000

/***************************************************************************
 *UPDATE_VDL_SNOOP1 - Update VDL snoop control register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: reserved0 [31:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved0_MASK 0xc0000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved0_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved0_SHIFT 30

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: MODE [29:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MODE_MASK    0x30000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MODE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MODE_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MODE_SHIFT   28
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: reserved1 [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved1_MASK 0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved1_SHIFT 27

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: MASK [26:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MASK_MASK    0x07ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MASK_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MASK_BITS    11
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MASK_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_MASK_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: reserved2 [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved2_MASK 0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved2_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved2_SHIFT 15

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: CMD [14:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_CMD_MASK     0x00007ff0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_CMD_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_CMD_BITS     11
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_CMD_SHIFT    4
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_CMD_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: reserved3 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved3_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved3_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved3_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_reserved3_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP1 :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_ENABLE_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_ENABLE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP1_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *UPDATE_VDL_SNOOP2 - Update VDL snoop control register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: reserved0 [31:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved0_MASK 0xc0000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved0_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved0_SHIFT 30

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: MODE [29:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MODE_MASK    0x30000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MODE_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MODE_BITS    2
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MODE_SHIFT   28
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: reserved1 [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved1_MASK 0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved1_SHIFT 27

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: MASK [26:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MASK_MASK    0x07ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MASK_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MASK_BITS    11
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MASK_SHIFT   16
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_MASK_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: reserved2 [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved2_MASK 0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved2_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved2_SHIFT 15

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: CMD [14:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_CMD_MASK     0x00007ff0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_CMD_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_CMD_BITS     11
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_CMD_SHIFT    4
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_CMD_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: reserved3 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved3_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved3_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved3_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_reserved3_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: UPDATE_VDL_SNOOP2 :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_ENABLE_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_ENABLE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_UPDATE_VDL_SNOOP2_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *COMMAND_REG1 - DRAM Command Register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: MCP [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_MCP_MASK          0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_MCP_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_MCP_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_MCP_SHIFT         31
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_MCP_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: CS [30:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CS_MASK           0x60000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CS_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CS_BITS           2
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CS_SHIFT          29
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CS_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: AUX [28:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AUX_MASK          0x1f000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AUX_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AUX_BITS          5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AUX_SHIFT         24
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AUX_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: ACT [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_ACT_MASK          0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_ACT_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_ACT_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_ACT_SHIFT         23
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_ACT_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: WE [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_WE_MASK           0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_WE_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_WE_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_WE_SHIFT          22
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_WE_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: CAS [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CAS_MASK          0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CAS_SHIFT         21
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_CAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: RAS [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_RAS_MASK          0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_RAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_RAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_RAS_SHIFT         20
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_RAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: BA [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_BA_MASK           0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_BA_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_BA_BITS           4
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_BA_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_BA_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG1 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AD_MASK           0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AD_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AD_BITS           16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AD_SHIFT          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG1_AD_DEFAULT        0x00000000

/***************************************************************************
 *COMMAND_AUX_REG1 - DRAM AUX_N Command Register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG1 :: reserved0 [31:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_reserved0_MASK 0xffffffe0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_reserved0_BITS 27
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_reserved0_SHIFT 5

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG1 :: AUX [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_AUX_MASK      0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_AUX_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_AUX_BITS      5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_AUX_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG1_AUX_DEFAULT   0x00000000

/***************************************************************************
 *COMMAND_REG2 - DRAM Command Register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: MCP [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_MCP_MASK          0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_MCP_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_MCP_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_MCP_SHIFT         31
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_MCP_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: CS [30:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CS_MASK           0x60000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CS_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CS_BITS           2
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CS_SHIFT          29
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CS_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: AUX [28:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AUX_MASK          0x1f000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AUX_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AUX_BITS          5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AUX_SHIFT         24
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AUX_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: ACT [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_ACT_MASK          0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_ACT_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_ACT_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_ACT_SHIFT         23
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_ACT_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: WE [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_WE_MASK           0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_WE_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_WE_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_WE_SHIFT          22
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_WE_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: CAS [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CAS_MASK          0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CAS_SHIFT         21
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_CAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: RAS [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_RAS_MASK          0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_RAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_RAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_RAS_SHIFT         20
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_RAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: BA [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_BA_MASK           0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_BA_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_BA_BITS           4
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_BA_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_BA_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG2 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AD_MASK           0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AD_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AD_BITS           16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AD_SHIFT          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG2_AD_DEFAULT        0x00000000

/***************************************************************************
 *COMMAND_AUX_REG2 - DRAM AUX_N Command Register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG2 :: reserved0 [31:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_reserved0_MASK 0xffffffe0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_reserved0_BITS 27
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_reserved0_SHIFT 5

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG2 :: AUX [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_AUX_MASK      0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_AUX_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_AUX_BITS      5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_AUX_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG2_AUX_DEFAULT   0x00000000

/***************************************************************************
 *COMMAND_REG3 - DRAM Command Register #3
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: MCP [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_MCP_MASK          0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_MCP_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_MCP_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_MCP_SHIFT         31
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_MCP_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: CS [30:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CS_MASK           0x60000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CS_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CS_BITS           2
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CS_SHIFT          29
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CS_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: AUX [28:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AUX_MASK          0x1f000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AUX_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AUX_BITS          5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AUX_SHIFT         24
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AUX_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: ACT [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_ACT_MASK          0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_ACT_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_ACT_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_ACT_SHIFT         23
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_ACT_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: WE [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_WE_MASK           0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_WE_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_WE_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_WE_SHIFT          22
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_WE_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: CAS [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CAS_MASK          0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CAS_SHIFT         21
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_CAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: RAS [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_RAS_MASK          0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_RAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_RAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_RAS_SHIFT         20
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_RAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: BA [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_BA_MASK           0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_BA_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_BA_BITS           4
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_BA_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_BA_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG3 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AD_MASK           0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AD_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AD_BITS           16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AD_SHIFT          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG3_AD_DEFAULT        0x00000000

/***************************************************************************
 *COMMAND_AUX_REG3 - DRAM AUX_N Command Register #3
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG3 :: reserved0 [31:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_reserved0_MASK 0xffffffe0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_reserved0_BITS 27
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_reserved0_SHIFT 5

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG3 :: AUX [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_AUX_MASK      0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_AUX_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_AUX_BITS      5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_AUX_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG3_AUX_DEFAULT   0x00000000

/***************************************************************************
 *COMMAND_REG4 - DRAM Command Register #4
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: MCP [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_MCP_MASK          0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_MCP_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_MCP_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_MCP_SHIFT         31
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_MCP_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: CS [30:29] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CS_MASK           0x60000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CS_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CS_BITS           2
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CS_SHIFT          29
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CS_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: AUX [28:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AUX_MASK          0x1f000000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AUX_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AUX_BITS          5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AUX_SHIFT         24
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AUX_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: ACT [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_ACT_MASK          0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_ACT_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_ACT_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_ACT_SHIFT         23
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_ACT_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: WE [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_WE_MASK           0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_WE_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_WE_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_WE_SHIFT          22
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_WE_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: CAS [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CAS_MASK          0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CAS_SHIFT         21
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_CAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: RAS [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_RAS_MASK          0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_RAS_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_RAS_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_RAS_SHIFT         20
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_RAS_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: BA [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_BA_MASK           0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_BA_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_BA_BITS           4
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_BA_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_BA_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG4 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AD_MASK           0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AD_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AD_BITS           16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AD_SHIFT          0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG4_AD_DEFAULT        0x00000000

/***************************************************************************
 *COMMAND_AUX_REG4 - DRAM AUX_N Command Register #4
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG4 :: reserved0 [31:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_reserved0_MASK 0xffffffe0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_reserved0_BITS 27
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_reserved0_SHIFT 5

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_AUX_REG4 :: AUX [04:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_AUX_MASK      0x0000001f
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_AUX_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_AUX_BITS      5
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_AUX_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_AUX_REG4_AUX_DEFAULT   0x00000000

/***************************************************************************
 *COMMAND_REG_TIMER - DRAM Command Timer Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG_TIMER :: reserved0 [31:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_reserved0_MASK 0xffff0000
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_reserved0_BITS 16
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_reserved0_SHIFT 16

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG_TIMER :: INIT_VAL [15:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_INIT_VAL_MASK 0x0000ff00
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_INIT_VAL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_INIT_VAL_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_INIT_VAL_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_INIT_VAL_DEFAULT 0x0000000f

/* DDR34_CORE_PHY_CONTROL_REGS :: COMMAND_REG_TIMER :: COUNT [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_COUNT_MASK   0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_COUNT_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_COUNT_BITS   8
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_COUNT_SHIFT  0
#define DDR34_CORE_PHY_CONTROL_REGS_COMMAND_REG_TIMER_COUNT_DEFAULT 0x00000000

/***************************************************************************
 *MODE_REG0 - DDR3/DDR4/GDDR5 Mode Register 0 and LPDDR Mode Register 1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG0 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG0 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG0 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG0_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG1 - DDR3/DDR4/GDDR5 Mode Register 1 and LPDDR Mode Register 2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG1 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG1 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG1 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG1_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG2 - DDR3/DDR4/GDDR5 Mode Register 2 and LPDDR Mode Register 3
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG2 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG2 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG2 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG2_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG3 - DDR3/DDR4/GDDR5 Mode Register 3 and LPDDR Mode Register 9
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG3 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG3 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG3 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG3_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG4 - DDR4/GDDR5 Mode Register 4 and LPDDR Mode Register 10
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG4 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG4 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG4 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG4_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG5 - DDR4/GDDR5 Mode Register 5 and LPDDR Mode Register 16
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG5 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG5 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG5 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG5_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG6 - DDR4/GDDR5 Mode Register 6 and LPDDR Mode Register 17
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG6 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG6 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG6 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG6_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG7 - DDR4/GDDR5 Mode Register 7 and LPDDR Mode Register 41
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG7 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG7 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG7 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG7_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG8 - GDDR5 Mode Register 8 and LPDDR Mode Register 42
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG8 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_reserved0_MASK       0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_reserved0_BITS       15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_reserved0_SHIFT      17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG8 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_VALID_MASK           0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_VALID_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_VALID_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_VALID_SHIFT          16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_VALID_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG8 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_AD_MASK              0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_AD_ALIGN             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_AD_BITS              16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_AD_SHIFT             0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG8_AD_DEFAULT           0x00000000

/***************************************************************************
 *MODE_REG15 - GDDR5 Mode Register 15 and LPDDR Mode Register 48
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG15 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_reserved0_MASK      0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_reserved0_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_reserved0_BITS      15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_reserved0_SHIFT     17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG15 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_VALID_MASK          0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_VALID_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_VALID_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_VALID_SHIFT         16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_VALID_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG15 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_AD_MASK             0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_AD_ALIGN            0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_AD_BITS             16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_AD_SHIFT            0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG15_AD_DEFAULT          0x00000000

/***************************************************************************
 *MODE_REG63 - LPDDR Mode Register 63
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG63 :: reserved0 [31:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_reserved0_MASK      0xfffe0000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_reserved0_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_reserved0_BITS      15
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_reserved0_SHIFT     17

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG63 :: VALID [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_VALID_MASK          0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_VALID_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_VALID_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_VALID_SHIFT         16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_VALID_DEFAULT       0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: MODE_REG63 :: AD [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_AD_MASK             0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_AD_ALIGN            0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_AD_BITS             16
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_AD_SHIFT            0
#define DDR34_CORE_PHY_CONTROL_REGS_MODE_REG63_AD_DEFAULT          0x00000000

/***************************************************************************
 *ALERT_CLEAR - DDR4 Alert status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: ALERT_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_reserved0_MASK     0xfffffffe
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_reserved0_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_reserved0_BITS     31
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_reserved0_SHIFT    1

/* DDR34_CORE_PHY_CONTROL_REGS :: ALERT_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_CLEAR_MASK         0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_CLEAR_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_CLEAR_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_CLEAR_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_CLEAR_CLEAR_DEFAULT      0x00000000

/***************************************************************************
 *ALERT_STATUS - DDR4 Alert status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: ALERT_STATUS :: reserved0 [31:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_reserved0_MASK    0xfffffffe
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_reserved0_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_reserved0_BITS    31
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_reserved0_SHIFT   1

/* DDR34_CORE_PHY_CONTROL_REGS :: ALERT_STATUS :: STATUS [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_STATUS_MASK       0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_STATUS_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_STATUS_BITS       1
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_STATUS_SHIFT      0
#define DDR34_CORE_PHY_CONTROL_REGS_ALERT_STATUS_STATUS_DEFAULT    0x00000000

/***************************************************************************
 *CA_PARITY - DDR4 CA parity control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PARITY :: reserved0 [31:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_reserved0_MASK       0xfffffffc
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_reserved0_BITS       30
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_reserved0_SHIFT      2

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PARITY :: ERROR [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ERROR_MASK           0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ERROR_ALIGN          0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ERROR_BITS           1
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ERROR_SHIFT          1
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ERROR_DEFAULT        0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PARITY :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ENABLE_MASK          0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ENABLE_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ENABLE_BITS          1
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ENABLE_SHIFT         0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PARITY_ENABLE_DEFAULT       0x00000000

/***************************************************************************
 *CA_PLAYBACK_CONTROL - GDDR5 CA playback control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_CONTROL :: reserved0 [31:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved0_MASK 0xfffff000
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved0_BITS 20
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved0_SHIFT 12

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_CONTROL :: COUNT [11:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_COUNT_MASK 0x00000ff0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_COUNT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_COUNT_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_COUNT_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_COUNT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_CONTROL :: reserved1 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_reserved1_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_CONTROL :: SAMPLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_SAMPLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_SAMPLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_SAMPLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_SAMPLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_CONTROL_SAMPLE_DEFAULT 0x00000000

/***************************************************************************
 *CA_PLAYBACK_STATUS0 - LPDDR3 and GDDR5 CA playback status register0
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_STATUS0 :: VALID [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_VALID_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_VALID_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_VALID_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_VALID_SHIFT 31
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_VALID_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_STATUS0 :: reserved0 [30:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved0_MASK 0x7c000000
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved0_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved0_SHIFT 26

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_STATUS0 :: DATA1 [25:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA1_MASK 0x03ff0000
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA1_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA1_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_STATUS0 :: reserved1 [15:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved1_MASK 0x0000fc00
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved1_BITS 6
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_reserved1_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: CA_PLAYBACK_STATUS0 :: DATA0 [09:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA0_MASK 0x000003ff
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA0_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_CA_PLAYBACK_STATUS0_DATA0_DEFAULT 0x00000000

/***************************************************************************
 *WRITE_LEVELING_CONTROL - Write leveling control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: reserved0 [31:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved0_MASK 0xffff0000
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved0_BITS 16
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved0_SHIFT 16

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: COUNT [15:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_COUNT_MASK 0x0000ff00
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_COUNT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_COUNT_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_COUNT_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_COUNT_DEFAULT 0x0000000f

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: reserved1 [07:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved1_MASK 0x000000f8
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved1_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_reserved1_SHIFT 3

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: SAMPLE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_SAMPLE_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_SAMPLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_SAMPLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_SAMPLE_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_SAMPLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: CONTINUOUS [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_CONTINUOUS_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_CONTINUOUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_CONTINUOUS_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_CONTINUOUS_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_CONTINUOUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *WRITE_LEVELING_STATUS - Write leveling status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_STATUS :: reserved0 [31:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved0_MASK 0xffffc000
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved0_BITS 18
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved0_SHIFT 14

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_STATUS :: EDC [13:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_EDC_MASK 0x00003e00
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_EDC_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_EDC_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_EDC_SHIFT 9
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_EDC_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_STATUS :: STATUS [08:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_STATUS_MASK 0x000001f0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_STATUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_STATUS_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_STATUS_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_STATUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_STATUS :: reserved1 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved1_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_reserved1_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_LEVELING_STATUS :: VALID [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_VALID_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_VALID_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_VALID_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_VALID_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_LEVELING_STATUS_VALID_DEFAULT 0x00000000

/***************************************************************************
 *READ_ENABLE_CONTROL - Read enable test cycle control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: reserved0 [31:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved0_MASK 0xffffe000
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved0_BITS 19
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved0_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: TEST_CYCLE [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_TEST_CYCLE_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_TEST_CYCLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_TEST_CYCLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_TEST_CYCLE_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_TEST_CYCLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: SELECT [11:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_SELECT_MASK 0x00000f00
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_SELECT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_SELECT_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_SELECT_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_SELECT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: reserved1 [07:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved1_MASK 0x000000c0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_reserved1_SHIFT 6

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: CS_N [05:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_CS_N_MASK  0x00000030
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_CS_N_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_CS_N_BITS  2
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_CS_N_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_CS_N_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: EDC_DATA [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_DATA_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_DATA_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_DATA_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_DATA_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_DATA_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: EDC_PHASE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_PHASE_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_PHASE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_PHASE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_PHASE_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_EDC_PHASE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: DQS [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_DQS_MASK   0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_DQS_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_DQS_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_DQS_SHIFT  1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_DQS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *READ_ENABLE_STATUS - Read enable test cycle status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: DATA [19:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_DATA_MASK   0x000ff000
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_DATA_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_DATA_BITS   8
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_DATA_SHIFT  12
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_DATA_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: reserved1 [11:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved1_MASK 0x00000fc0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved1_BITS 6
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved1_SHIFT 6

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: BL1_STATUS [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL1_STATUS_MASK 0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL1_STATUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL1_STATUS_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL1_STATUS_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL1_STATUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: BL0_STATUS [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL0_STATUS_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL0_STATUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL0_STATUS_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL0_STATUS_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_BL0_STATUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: reserved2 [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved2_MASK 0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved2_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved2_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_reserved2_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: READ_ENABLE_STATUS :: VALID [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_VALID_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_VALID_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_VALID_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_VALID_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_READ_ENABLE_STATUS_VALID_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_LFSR_SEED - Traffic generator seed register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_LFSR_SEED :: SEED [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED_SEED_MASK 0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED_SEED_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED_SEED_BITS 32
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED_SEED_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_LFSR_SEED_SEED_DEFAULT 0xba5eba11

/***************************************************************************
 *TRAFFIC_GEN_ADDRESS1 - Traffic generator address register #1
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS1 :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS1 :: BANK [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_BANK_MASK 0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_BANK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_BANK_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_BANK_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_BANK_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS1 :: ROW [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_ROW_MASK  0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_ROW_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_ROW_BITS  16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_ROW_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS1_ROW_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_ADDRESS2 - Traffic generator address register #2
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS2 :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS2 :: BANK [19:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_BANK_MASK 0x000f0000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_BANK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_BANK_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_BANK_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_BANK_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ADDRESS2 :: ROW [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_ROW_MASK  0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_ROW_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_ROW_BITS  16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_ROW_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ADDRESS2_ROW_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_CONTROL - Traffic generator control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: reserved0 [31:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_reserved0_MASK 0xffffe000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_reserved0_BITS 19
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_reserved0_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: DIAG_WRO [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: DIAG_WRO_RD [11:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_RD_MASK 0x00000800
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_RD_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_RD_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_RD_SHIFT 11
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WRO_RD_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: DIAG_WR_RD [10:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WR_RD_MASK 0x00000400
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WR_RD_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WR_RD_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WR_RD_SHIFT 10
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_DIAG_WR_RD_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: WR_NOISE [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_WR_NOISE_MASK 0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_WR_NOISE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_WR_NOISE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_WR_NOISE_SHIFT 9
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_WR_NOISE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: RD_NOISE [08:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_NOISE_MASK 0x00000100
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_NOISE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_NOISE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_NOISE_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_NOISE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: CLEAR_DRAM [07:07] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_CLEAR_DRAM_MASK 0x00000080
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_CLEAR_DRAM_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_CLEAR_DRAM_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_CLEAR_DRAM_SHIFT 7
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_CLEAR_DRAM_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: MASK_DM [06:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MASK_DM_MASK 0x00000040
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MASK_DM_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MASK_DM_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MASK_DM_SHIFT 6
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MASK_DM_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: INIT_LFSR [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_INIT_LFSR_MASK 0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_INIT_LFSR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_INIT_LFSR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_INIT_LFSR_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_INIT_LFSR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: FIFO [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_FIFO_MASK  0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_FIFO_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_FIFO_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_FIFO_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_FIFO_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: MPR [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MPR_MASK   0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MPR_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MPR_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MPR_SHIFT  3
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_MPR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: RD_WR [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_WR_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_WR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_WR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_WR_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_WR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: RD_EN [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_EN_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_EN_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_EN_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_RD_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_CONTROL_ENABLE_SHIFT 0

/***************************************************************************
 *TRAFFIC_GEN_DATA_CONTROL - Traffic generator data control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_DATA_CONTROL :: reserved0 [31:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_reserved0_MASK 0xffc00000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_reserved0_BITS 10
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_reserved0_SHIFT 22

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_DATA_CONTROL :: PATTERN [21:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_PATTERN_MASK 0x00300000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_PATTERN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_PATTERN_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_PATTERN_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_PATTERN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_DATA_CONTROL :: LENGTH [19:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_LENGTH_MASK 0x000fffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_LENGTH_BITS 20
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_LENGTH_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DATA_CONTROL_LENGTH_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_DQ_MASK - Traffic generator DQ mask register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_DQ_MASK :: MASK [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK_MASK_MASK  0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK_MASK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK_MASK_BITS  32
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK_MASK_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_MASK_MASK_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_ECC_DQ_MASK - Traffic generator ECC DQ mask register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ECC_DQ_MASK :: reserved0 [31:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_reserved0_MASK 0xfffffff0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_reserved0_BITS 28
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_reserved0_SHIFT 4

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ECC_DQ_MASK :: MASK [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_MASK_MASK 0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_MASK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_MASK_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_MASK_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_DQ_MASK_MASK_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_STATUS - Traffic generator status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_STATUS :: reserved0 [31:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_reserved0_BITS 31
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_reserved0_SHIFT 1

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_STATUS :: BUSY [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_BUSY_MASK   0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_BUSY_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_BUSY_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_BUSY_SHIFT  0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_STATUS_BUSY_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_DQ_STATUS - Traffic generator DQ status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_DQ_STATUS :: STATUS [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS_STATUS_MASK 0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS_STATUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS_STATUS_BITS 32
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS_STATUS_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_DQ_STATUS_STATUS_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_ECC_STATUS - Traffic generator ECC DQ status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ECC_STATUS :: reserved0 [31:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_reserved0_MASK 0xfffffff0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_reserved0_BITS 28
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_reserved0_SHIFT 4

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ECC_STATUS :: STATUS [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_STATUS_MASK 0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_STATUS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_STATUS_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_STATUS_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ECC_STATUS_STATUS_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_ERR_CNT_CONTROL - Traffic generator error count control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_CONTROL :: reserved0 [31:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved0_MASK 0xfffffe00
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved0_BITS 23
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved0_SHIFT 9

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_CONTROL :: DQ_SEL [08:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_DQ_SEL_MASK 0x000001f0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_DQ_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_DQ_SEL_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_DQ_SEL_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_DQ_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_CONTROL :: reserved1 [03:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved1_MASK 0x0000000c
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_reserved1_SHIFT 2

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_CONTROL :: CLEAR [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_CLEAR_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_CLEAR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_CLEAR_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_CLEAR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *TRAFFIC_GEN_ERR_CNT_STATUS - Traffic generator error count status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_STATUS :: reserved0 [31:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_reserved0_MASK 0xffff0000
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_reserved0_BITS 16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_reserved0_SHIFT 16

/* DDR34_CORE_PHY_CONTROL_REGS :: TRAFFIC_GEN_ERR_CNT_STATUS :: COUNT [15:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_COUNT_MASK 0x0000ffff
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_COUNT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_COUNT_BITS 16
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_COUNT_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_TRAFFIC_GEN_ERR_CNT_STATUS_COUNT_DEFAULT 0x00000000

/***************************************************************************
 *VIRTUAL_VTT_CONTROL - Virtual VTT Control and Status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: reserved0 [31:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved0_MASK 0xfffff000
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved0_BITS 20
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved0_SHIFT 12

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: reserved_for_eco1 [11:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved_for_eco1_MASK 0x00000f00
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved_for_eco1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved_for_eco1_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved_for_eco1_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_reserved_for_eco1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: MAX_NOISE [07:07] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_MAX_NOISE_MASK 0x00000080
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_MAX_NOISE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_MAX_NOISE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_MAX_NOISE_SHIFT 7
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_MAX_NOISE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: LOW_NOISE [06:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_NOISE_MASK 0x00000040
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_NOISE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_NOISE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_NOISE_SHIFT 6
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_NOISE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: LOW_VTT [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_VTT_MASK 0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_VTT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_VTT_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_VTT_SHIFT 5
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_LOW_VTT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: HIGH_VTT [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_HIGH_VTT_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_HIGH_VTT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_HIGH_VTT_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_HIGH_VTT_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_HIGH_VTT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: ERROR_RESET [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ERROR_RESET_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ERROR_RESET_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ERROR_RESET_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ERROR_RESET_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ERROR_RESET_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: ENABLE_CTL_IDLE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CTL_IDLE_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CTL_IDLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CTL_IDLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CTL_IDLE_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CTL_IDLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: ENABLE_CS_IDLE [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CS_IDLE_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CS_IDLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CS_IDLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CS_IDLE_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CS_IDLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONTROL :: ENABLE_CKE_IDLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CKE_IDLE_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CKE_IDLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CKE_IDLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CKE_IDLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL_ENABLE_CKE_IDLE_DEFAULT 0x00000000

/***************************************************************************
 *VIRTUAL_VTT_STATUS - Virtual VTT Control and Status register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_STATUS :: reserved0 [31:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_reserved0_MASK 0xfff80000
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_reserved0_BITS 13
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_reserved0_SHIFT 19

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_STATUS :: ERROR [18:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_MASK  0x0007fff8
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_BITS  16
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_STATUS :: ERROR_LOW [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_LOW_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_LOW_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_LOW_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_LOW_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_LOW_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_STATUS :: ERROR_HIGH [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_HIGH_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_HIGH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_HIGH_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_HIGH_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_ERROR_HIGH_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_STATUS :: READY [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_READY_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_READY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_READY_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_READY_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_STATUS_READY_DEFAULT 0x00000000

/***************************************************************************
 *VIRTUAL_VTT_CONNECTIONS - Virtual VTT Connections register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONNECTIONS :: reserved0 [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_reserved0_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_reserved0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_reserved0_SHIFT 31

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_CONNECTIONS :: MASK [30:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_MASK_MASK 0x7fffffff
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_MASK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_MASK_BITS 31
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_MASK_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONS_MASK_DEFAULT 0x1fffffff

/***************************************************************************
 *VIRTUAL_VTT_OVERRIDE - Virtual VTT Override register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_OVERRIDE :: reserved0 [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_reserved0_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_reserved0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_reserved0_SHIFT 31

/* DDR34_CORE_PHY_CONTROL_REGS :: VIRTUAL_VTT_OVERRIDE :: MASK [30:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_MASK_MASK 0x7fffffff
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_MASK_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_MASK_BITS 31
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_MASK_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDE_MASK_DEFAULT 0x0000ffff

/***************************************************************************
 *VREF_DAC_CONTROL - VREF DAC Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_reserved0_BITS 12
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: AUX_GT_INT [19:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_AUX_GT_INT_MASK 0x00080000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_AUX_GT_INT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_AUX_GT_INT_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_AUX_GT_INT_SHIFT 19
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_AUX_GT_INT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: TESTOUT_MUX_CTL [18:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TESTOUT_MUX_CTL_MASK 0x00060000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TESTOUT_MUX_CTL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TESTOUT_MUX_CTL_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TESTOUT_MUX_CTL_SHIFT 17
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TESTOUT_MUX_CTL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: TEST [16:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TEST_MASK     0x00010000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TEST_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TEST_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TEST_SHIFT    16
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_TEST_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: PDN3 [15:15] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN3_MASK     0x00008000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN3_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN3_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN3_SHIFT    15
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN3_DEFAULT  0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: PDN2 [14:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN2_MASK     0x00004000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN2_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN2_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN2_SHIFT    14
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN2_DEFAULT  0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: PDN1 [13:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN1_MASK     0x00002000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN1_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN1_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN1_SHIFT    13
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN1_DEFAULT  0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: PDN0 [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN0_MASK     0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN0_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN0_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN0_SHIFT    12
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_PDN0_DEFAULT  0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: DAC1 [11:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_MASK     0x00000fc0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_BITS     6
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_SHIFT    6
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_DEFAULT  0x00000020

/* DDR34_CORE_PHY_CONTROL_REGS :: VREF_DAC_CONTROL :: DAC0 [05:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC0_MASK     0x0000003f
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC0_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC0_BITS     6
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC0_SHIFT    0
#define DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC0_DEFAULT  0x00000020

/***************************************************************************
 *PHYBIST_CNTRL - PhyBist Control Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: reserved0 [31:30] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved0_MASK   0xc0000000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved0_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved0_BITS   2
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved0_SHIFT  30

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: CLK_PAD_ENB [29:28] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_CLK_PAD_ENB_MASK 0x30000000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_CLK_PAD_ENB_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_CLK_PAD_ENB_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_CLK_PAD_ENB_SHIFT 28
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_CLK_PAD_ENB_DEFAULT 0x00000002

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: reserved1 [27:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved1_MASK   0x08000000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved1_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved1_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved1_SHIFT  27

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: FORCE_DQ_ERROR_SEL [26:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DQ_ERROR_SEL_MASK 0x07000000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DQ_ERROR_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DQ_ERROR_SEL_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DQ_ERROR_SEL_SHIFT 24
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DQ_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: reserved2 [23:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved2_MASK   0x00800000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved2_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved2_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved2_SHIFT  23

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: FORCE_BL_ERROR_SEL [22:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_BL_ERROR_SEL_MASK 0x00700000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_BL_ERROR_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_BL_ERROR_SEL_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_BL_ERROR_SEL_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_BL_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: reserved3 [19:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved3_MASK   0x000e0000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved3_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved3_BITS   3
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved3_SHIFT  17

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: FORCE_CTL_ERROR_SEL [16:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SEL_MASK 0x0001f000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SEL_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SEL_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: reserved4 [11:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved4_MASK   0x00000c00
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved4_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved4_BITS   2
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_reserved4_SHIFT  10

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: FORCE_DAT_ERROR [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DAT_ERROR_MASK 0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DAT_ERROR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DAT_ERROR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DAT_ERROR_SHIFT 9
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_DAT_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: FORCE_CTL_ERROR [08:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_MASK 0x00000100
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_FORCE_CTL_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: SSO [07:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_SSO_MASK         0x000000c0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_SSO_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_SSO_BITS         2
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_SSO_SHIFT        6
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_SSO_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: LENGTH [05:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_LENGTH_MASK      0x00000030
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_LENGTH_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_LENGTH_BITS      2
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_LENGTH_SHIFT     4
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_LENGTH_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: MODE [03:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_MODE_MASK        0x0000000e
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_MODE_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_MODE_BITS        3
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_MODE_SHIFT       1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_MODE_DEFAULT     0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CNTRL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_ENABLE_MASK      0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_ENABLE_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_ENABLE_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_ENABLE_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CNTRL_ENABLE_DEFAULT   0x00000000

/***************************************************************************
 *PHYBIST_SEED - PhyBist Seed Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_SEED :: SEED [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED_SEED_MASK         0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED_SEED_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED_SEED_BITS         32
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED_SEED_SHIFT        0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_SEED_SEED_DEFAULT      0xba5eba11

/***************************************************************************
 *PHYBIST_CA_MASK - PhyBist Command/Address Bus Mask
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CA_MASK :: MASK [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK_MASK_MASK      0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK_MASK_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK_MASK_BITS      32
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK_MASK_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CA_MASK_MASK_DEFAULT   0x00000000

/***************************************************************************
 *PHYBIST_STATUS - PhyBist General Status Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_STATUS :: reserved0 [31:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_reserved0_MASK  0xfffffff0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_reserved0_BITS  28
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_reserved0_SHIFT 4

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_STATUS :: DAT_PASS [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_PASS_MASK   0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_PASS_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_PASS_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_PASS_SHIFT  3

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_STATUS :: CTL_PASS [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_PASS_MASK   0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_PASS_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_PASS_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_PASS_SHIFT  2

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_STATUS :: DAT_DONE [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_DONE_MASK   0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_DONE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_DONE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_DAT_DONE_SHIFT  1

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_STATUS :: CTL_DONE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_DONE_MASK   0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_DONE_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_DONE_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_STATUS_CTL_DONE_SHIFT  0

/***************************************************************************
 *PHYBIST_CTL_STATUS - PhyBist Per-Bit Control Pad Status Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CTL_STATUS :: reserved0 [31:31] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_reserved0_MASK 0x80000000
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_reserved0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_reserved0_SHIFT 31

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_CTL_STATUS :: CTL_ERRORS [30:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_CTL_ERRORS_MASK 0x7fffffff
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_CTL_ERRORS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_CTL_ERRORS_BITS 31
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_CTL_STATUS_CTL_ERRORS_SHIFT 0

/***************************************************************************
 *PHYBIST_BL0_STATUS - PhyBist Byte Lane #0 Status Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL0_STATUS :: reserved0 [31:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_reserved0_MASK 0xfffffc00
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_reserved0_BITS 22
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_reserved0_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL0_STATUS :: EDC [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_EDC_MASK    0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_EDC_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_EDC_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_EDC_SHIFT   9

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL0_STATUS :: DM [08:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DM_MASK     0x00000100
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DM_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DM_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DM_SHIFT    8

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL0_STATUS :: DQ [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DQ_MASK     0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DQ_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DQ_BITS     8
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL0_STATUS_DQ_SHIFT    0

/***************************************************************************
 *PHYBIST_BL1_STATUS - PhyBist Byte Lane #1 Status Register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL1_STATUS :: reserved0 [31:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_reserved0_MASK 0xfffffc00
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_reserved0_BITS 22
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_reserved0_SHIFT 10

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL1_STATUS :: EDC [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_EDC_MASK    0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_EDC_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_EDC_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_EDC_SHIFT   9

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL1_STATUS :: DM [08:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DM_MASK     0x00000100
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DM_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DM_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DM_SHIFT    8

/* DDR34_CORE_PHY_CONTROL_REGS :: PHYBIST_BL1_STATUS :: DQ [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DQ_MASK     0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DQ_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DQ_BITS     8
#define DDR34_CORE_PHY_CONTROL_REGS_PHYBIST_BL1_STATUS_DQ_SHIFT    0

/***************************************************************************
 *STANDBY_CONTROL - Standby Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: reserved0 [31:23] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_reserved0_MASK 0xff800000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_reserved0_BITS 9
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_reserved0_SHIFT 23

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: STANDBY_READY [22:22] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_READY_MASK 0x00400000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_READY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_READY_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_READY_SHIFT 22
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_READY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: STANDBY_EXIT_PIN_EN [21:21] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_EXIT_PIN_EN_MASK 0x00200000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_EXIT_PIN_EN_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_EXIT_PIN_EN_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_EXIT_PIN_EN_SHIFT 21
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_EXIT_PIN_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: STANDBY_ACTIVE [20:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ACTIVE_MASK 0x00100000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ACTIVE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ACTIVE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ACTIVE_SHIFT 20
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ACTIVE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: ARMED [19:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_ARMED_MASK     0x00080000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_ARMED_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_ARMED_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_ARMED_SHIFT    19
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_ARMED_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: WARMSTART [18:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_WARMSTART_MASK 0x00040000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_WARMSTART_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_WARMSTART_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_WARMSTART_SHIFT 18
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_WARMSTART_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: PWRDOWN_LDO_BIAS [17:16] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_BIAS_MASK 0x00030000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_BIAS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_BIAS_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_BIAS_SHIFT 16
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_BIAS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: PWRDOWN_LDO_VOLTS [15:14] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_VOLTS_MASK 0x0000c000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_VOLTS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_VOLTS_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_VOLTS_SHIFT 14
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_LDO_VOLTS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: PWRDOWN_SKIP_MRS [13:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_SKIP_MRS_MASK 0x00002000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_SKIP_MRS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_SKIP_MRS_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_SKIP_MRS_SHIFT 13
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_SKIP_MRS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: PWRDOWN_RST_N [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_RST_N_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_RST_N_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_RST_N_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_RST_N_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_RST_N_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: PWRDOWN_CKE [11:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_CKE_MASK 0x00000800
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_CKE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_CKE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_CKE_SHIFT 11
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_PWRDOWN_CKE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: LDO_BIAS [10:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_BIAS_MASK  0x00000600
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_BIAS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_BIAS_BITS  2
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_BIAS_SHIFT 9
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_BIAS_DEFAULT 0x00000003

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: LDO_VOLTS [08:07] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_VOLTS_MASK 0x00000180
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_VOLTS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_VOLTS_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_VOLTS_SHIFT 7
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_LDO_VOLTS_DEFAULT 0x00000003

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: SKIP_MRS [06:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_SKIP_MRS_MASK  0x00000040
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_SKIP_MRS_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_SKIP_MRS_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_SKIP_MRS_SHIFT 6
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_SKIP_MRS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: RST_N [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_RST_N_MASK     0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_RST_N_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_RST_N_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_RST_N_SHIFT    5
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_RST_N_DEFAULT  0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: CKE [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_CKE_MASK       0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_CKE_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_CKE_BITS       1
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_CKE_SHIFT      4
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_CKE_DEFAULT    0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: STANDBY_CONTROL :: STANDBY [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_MASK   0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_BITS   4
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_SHIFT  0
#define DDR34_CORE_PHY_CONTROL_REGS_STANDBY_CONTROL_STANDBY_DEFAULT 0x00000000

/***************************************************************************
 *DEBUG_FREEZE_ENABLE - Freeze-on-error enable register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: reserved0 [31:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_reserved0_MASK 0xffffffe0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_reserved0_BITS 27
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_reserved0_SHIFT 5

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: WLECC [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WLECC_MASK 0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WLECC_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WLECC_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WLECC_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WLECC_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: WL1_BL1 [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL1_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL1_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: WL1_BL0 [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL0_MASK 0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL0_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL1_BL0_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: WL0_BL1 [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL1_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL1_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_FREEZE_ENABLE :: WL0_BL0 [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL0_MASK 0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL0_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_FREEZE_ENABLE_WL0_BL0_DEFAULT 0x00000000

/***************************************************************************
 *DEBUG_MUX_CONTROL - Debug Mux Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_MUX_CONTROL :: reserved0 [31:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved0_MASK 0xfffff800
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved0_BITS 21
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved0_SHIFT 11

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_MUX_CONTROL :: BYTE_SEL [10:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_BYTE_SEL_MASK 0x00000700
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_BYTE_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_BYTE_SEL_BITS 3
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_BYTE_SEL_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_BYTE_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_MUX_CONTROL :: reserved1 [07:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved1_MASK 0x000000c0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved1_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_reserved1_SHIFT 6

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_MUX_CONTROL :: PHASE_SEL [05:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_PHASE_SEL_MASK 0x00000030
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_PHASE_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_PHASE_SEL_BITS 2
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_PHASE_SEL_SHIFT 4
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_PHASE_SEL_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DEBUG_MUX_CONTROL :: SOURCE_SEL [03:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_SOURCE_SEL_MASK 0x0000000f
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_SOURCE_SEL_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_SOURCE_SEL_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_SOURCE_SEL_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_DEBUG_MUX_CONTROL_SOURCE_SEL_DEFAULT 0x00000000

/***************************************************************************
 *DFI_CNTRL - DFI Interface Ownership Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: reserved0 [31:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_reserved0_MASK       0xfffffc00
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_reserved0_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_reserved0_BITS       22
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_reserved0_SHIFT      10

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: SELF_REFRESH_CS1 [09:09] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS1_MASK 0x00000200
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS1_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS1_SHIFT 9
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS1_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: SELF_REFRESH_CS0 [08:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS0_MASK 0x00000100
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS0_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS0_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_SELF_REFRESH_CS0_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: DFI_CS1 [07:07] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS1_MASK         0x00000080
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS1_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS1_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS1_SHIFT        7
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS1_DEFAULT      0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: DFI_CS0 [06:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS0_MASK         0x00000040
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS0_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS0_BITS         1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS0_SHIFT        6
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CS0_DEFAULT      0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: DFI_RST_N [05:05] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_RST_N_MASK       0x00000020
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_RST_N_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_RST_N_BITS       1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_RST_N_SHIFT      5
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_RST_N_DEFAULT    0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: DFI_CKE1 [04:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE1_MASK        0x00000010
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE1_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE1_BITS        1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE1_SHIFT       4
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE1_DEFAULT     0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: DFI_CKE0 [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE0_MASK        0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE0_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE0_BITS        1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE0_SHIFT       3
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_DFI_CKE0_DEFAULT     0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: ACK_ENABLE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_ENABLE_MASK      0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_ENABLE_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_ENABLE_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_ENABLE_SHIFT     2
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_ENABLE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: ACK_STATUS [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_STATUS_MASK      0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_STATUS_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_STATUS_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_STATUS_SHIFT     1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ACK_STATUS_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: DFI_CNTRL :: ASSERT_REQ [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ASSERT_REQ_MASK      0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ASSERT_REQ_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ASSERT_REQ_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ASSERT_REQ_SHIFT     0
#define DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL_ASSERT_REQ_DEFAULT   0x00000001

/***************************************************************************
 *WRITE_ODT_CNTRL - Write ODT Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: reserved0 [31:13] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_reserved0_MASK 0xffffe000
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_reserved0_BITS 19
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_reserved0_SHIFT 13

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: ODT_FORCE_VALUE [12:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_VALUE_MASK 0x00001000
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_VALUE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_VALUE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_VALUE_SHIFT 12
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_VALUE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: ODT_FORCE [11:11] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_MASK 0x00000800
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_SHIFT 11
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_FORCE_DEFAULT 0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: ODT_ENABLE [10:10] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_ENABLE_MASK 0x00000400
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_ENABLE_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_ENABLE_SHIFT 10
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: ODT_LENGTH [09:06] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_LENGTH_MASK 0x000003c0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_LENGTH_BITS 4
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_LENGTH_SHIFT 6
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_LENGTH_DEFAULT 0x00000004

/* DDR34_CORE_PHY_CONTROL_REGS :: WRITE_ODT_CNTRL :: ODT_DELAY [05:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_DELAY_MASK 0x0000003f
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_DELAY_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_DELAY_BITS 6
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_DELAY_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL_ODT_DELAY_DEFAULT 0x00000010

/***************************************************************************
 *ABI_PAR_CNTRL - ABI and PAR Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: ABI_PAR_CNTRL :: reserved0 [31:04] */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_reserved0_MASK   0xfffffff0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_reserved0_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_reserved0_BITS   28
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_reserved0_SHIFT  4

/* DDR34_CORE_PHY_CONTROL_REGS :: ABI_PAR_CNTRL :: PAR_INCLUDE_AUX [03:03] */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_INCLUDE_AUX_MASK 0x00000008
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_INCLUDE_AUX_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_INCLUDE_AUX_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_INCLUDE_AUX_SHIFT 3
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_INCLUDE_AUX_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ABI_PAR_CNTRL :: PAR_ENABLE [02:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_ENABLE_MASK  0x00000004
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_ENABLE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_ENABLE_SHIFT 2
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_PAR_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ABI_PAR_CNTRL :: ABI_INCLUDE_AUX [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_INCLUDE_AUX_MASK 0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_INCLUDE_AUX_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_INCLUDE_AUX_BITS 1
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_INCLUDE_AUX_SHIFT 1
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_INCLUDE_AUX_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ABI_PAR_CNTRL :: ABI_ENABLE [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_ENABLE_MASK  0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_ENABLE_BITS  1
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_ABI_PAR_CNTRL_ABI_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *ZQ_CAL - ZQ Calibration Control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved0_MASK          0xfff00000
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved0_ALIGN         0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved0_BITS          12
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved0_SHIFT         20

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_PCOMP_STATUS [19:19] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_STATUS_MASK    0x00080000
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_STATUS_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_STATUS_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_STATUS_SHIFT   19
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_STATUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_NCOMP_STATUS [18:18] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_STATUS_MASK    0x00040000
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_STATUS_ALIGN   0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_STATUS_BITS    1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_STATUS_SHIFT   18
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_STATUS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_IDDQ [17:17] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_IDDQ_MASK            0x00020000
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_IDDQ_ALIGN           0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_IDDQ_BITS            1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_IDDQ_SHIFT           17
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_IDDQ_DEFAULT         0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_DRIVE_P [16:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_P_MASK         0x0001f000
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_P_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_P_BITS         5
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_P_SHIFT        12
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_P_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_DRIVE_N [11:07] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_N_MASK         0x00000f80
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_N_ALIGN        0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_N_BITS         5
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_N_SHIFT        7
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_DRIVE_N_DEFAULT      0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: reserved_for_padding1 [06:02] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved_for_padding1_MASK 0x0000007c
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved_for_padding1_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved_for_padding1_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_reserved_for_padding1_SHIFT 2

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_PCOMP_ENB [01:01] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_ENB_MASK       0x00000002
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_ENB_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_ENB_BITS       1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_ENB_SHIFT      1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_PCOMP_ENB_DEFAULT    0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: ZQ_CAL :: ZQ_NCOMP_ENB [00:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_ENB_MASK       0x00000001
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_ENB_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_ENB_BITS       1
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_ENB_SHIFT      0
#define DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL_ZQ_NCOMP_ENB_DEFAULT    0x00000001

/***************************************************************************
 *RO_PROC_MON_CTL - Ring-Osc control register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: reserved0 [31:27] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_reserved0_MASK 0xf8000000
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_reserved0_BITS 5
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_reserved0_SHIFT 27

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: BUSY [26:26] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_BUSY_MASK      0x04000000
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_BUSY_ALIGN     0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_BUSY_BITS      1
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_BUSY_SHIFT     26
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_BUSY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: TRIGGER [25:25] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_TRIGGER_MASK   0x02000000
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_TRIGGER_ALIGN  0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_TRIGGER_BITS   1
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_TRIGGER_SHIFT  25
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_TRIGGER_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: RESET [24:24] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_RESET_MASK     0x01000000
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_RESET_ALIGN    0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_RESET_BITS     1
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_RESET_SHIFT    24
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_RESET_DEFAULT  0x00000001

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: SEL [23:12] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_SEL_MASK       0x00fff000
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_SEL_ALIGN      0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_SEL_BITS       12
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_SEL_SHIFT      12
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_SEL_DEFAULT    0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_CTL :: EN [11:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_EN_MASK        0x00000fff
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_EN_ALIGN       0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_EN_BITS        12
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_EN_SHIFT       0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_CTL_EN_DEFAULT     0x00000000

/***************************************************************************
 *RO_PROC_MON_STATUS - Ring-Osc count register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_STATUS :: CLK_COUNT [31:08] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_CLK_COUNT_MASK 0xffffff00
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_CLK_COUNT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_CLK_COUNT_BITS 24
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_CLK_COUNT_SHIFT 8
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_CLK_COUNT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_CONTROL_REGS :: RO_PROC_MON_STATUS :: RO_COUNT [07:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_RO_COUNT_MASK 0x000000ff
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_RO_COUNT_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_RO_COUNT_BITS 8
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_RO_COUNT_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_RO_PROC_MON_STATUS_RO_COUNT_DEFAULT 0x00000000

/***************************************************************************
 *AC_SPARE_REG - Address and Control Spare register
 ***************************************************************************/
/* DDR34_CORE_PHY_CONTROL_REGS :: AC_SPARE_REG :: reserved_for_eco0 [31:00] */
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG_reserved_for_eco0_MASK 0xffffffff
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG_reserved_for_eco0_ALIGN 0
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG_reserved_for_eco0_BITS 32
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG_reserved_for_eco0_SHIFT 0
#define DDR34_CORE_PHY_CONTROL_REGS_AC_SPARE_REG_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQS_P - Write channel DQS-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_BUSY_MASK  0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_BUSY_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_P :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQS_N - Write channel DQS-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_BUSY_MASK  0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_BUSY_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQS_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ0 - Write channel DQ0 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ0 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ1 - Write channel DQ1 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ1 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ2 - Write channel DQ2 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ2 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ3 - Write channel DQ3 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ3 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ4 - Write channel DQ4 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ4 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ5 - Write channel DQ5 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ5 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ6 - Write channel DQ6 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ6 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ7 - Write channel DQ7 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DQ7 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DM - Write channel DM VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_BUSY_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_BUSY_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_BUSY_SHIFT    31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_FORCE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_FORCE_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_FORCE_SHIFT   16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_DM :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_EDC - Write channel EDC VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_WR_EDC :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQSP - Read channel DQSP VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQSN - Read channel DQSP VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQSN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ0P - Read channel DQ0-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ0N - Read channel DQ0-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ0N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ1P - Read channel DQ1-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ1N - Read channel DQ1-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ1N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ2P - Read channel DQ2-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ2N - Read channel DQ2-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ2N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ3P - Read channel DQ3-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ3N - Read channel DQ3-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ3N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ4P - Read channel DQ4-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ4N - Read channel DQ4-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ4N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ5P - Read channel DQ5-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ5N - Read channel DQ5-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ5N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ6P - Read channel DQ6-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ6N - Read channel DQ6-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ6N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ7P - Read channel DQ7-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ7N - Read channel DQ7-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DQ7N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DMP - Read channel DM-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DMN - Read channel DM-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_DMN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EDCP - Read channel EDC-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EDCN - Read channel EDC-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EDCN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EN_CS0 - Read channel CS_N[0] read enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS0 :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EN_CS1 - Read channel CS_N[1] read enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CONTROL_RD_EN_CS1 :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CLK_CONTROL - DDR interface signal Write Leveling CLK VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved1_MASK  0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved1_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_ADJ_EN_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_ADJ_EN_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_ADJ_EN_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_ADJ_EN_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_ADJ_EN_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved2_MASK  0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved2_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_CLK_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_VDL_STEP_MASK   0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_VDL_STEP_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_VDL_STEP_BITS   9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_VDL_STEP_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_CLK_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_LDE_CONTROL - DDR interface signal Write Leveling Capture Enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved1_MASK  0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved1_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_ADJ_EN_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_ADJ_EN_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_ADJ_EN_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_ADJ_EN_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_ADJ_EN_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved2_MASK  0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved2_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_0 :: VDL_LDE_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_VDL_STEP_MASK   0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_VDL_STEP_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_VDL_STEP_BITS   9
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_VDL_STEP_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_VDL_LDE_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *RD_EN_DLY_CYC - Read enable bit-clock cycle delay control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_BUSY_MASK         0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_BUSY_ALIGN        0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_BUSY_BITS         1
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_BUSY_SHIFT        31
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_BUSY_DEFAULT      0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved0_MASK    0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved0_BITS    14
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved0_SHIFT   17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_FORCE_MASK        0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_FORCE_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_FORCE_BITS        1
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_FORCE_SHIFT       16
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_FORCE_DEFAULT     0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: reserved1 [15:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved1_MASK    0x0000ff00
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved1_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved1_BITS    8
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_reserved1_SHIFT   8

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: CS1_CYCLES [07:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS1_CYCLES_MASK   0x000000f0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS1_CYCLES_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS1_CYCLES_BITS   4
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS1_CYCLES_SHIFT  4
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS1_CYCLES_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DLY_CYC :: CS0_CYCLES [03:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS0_CYCLES_MASK   0x0000000f
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS0_CYCLES_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS0_CYCLES_BITS   4
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS0_CYCLES_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC_CS0_CYCLES_DEFAULT 0x00000000

/***************************************************************************
 *WR_CHAN_DLY_CYC - Write leveling bit-clock cycle delay control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_CHAN_DLY_CYC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_CHAN_DLY_CYC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_CHAN_DLY_CYC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_CHAN_DLY_CYC :: reserved1 [15:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved1_MASK  0x0000fff8
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved1_BITS  13
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_reserved1_SHIFT 3

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_CHAN_DLY_CYC :: CYCLES [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_CYCLES_MASK     0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_CYCLES_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_CYCLES_BITS     3
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_CYCLES_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC_CYCLES_DEFAULT  0x00000000

/***************************************************************************
 *READ_CONTROL - Read channel datapath control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_CONTROL :: reserved0 [31:05] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved0_MASK     0xffffffe0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved0_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved0_BITS     27
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved0_SHIFT    5

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_CONTROL :: MODE [04:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_MODE_MASK          0x00000010
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_MODE_ALIGN         0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_MODE_BITS          1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_MODE_SHIFT         4
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_MODE_DEFAULT       0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_CONTROL :: reserved1 [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved1_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved1_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved1_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_reserved1_SHIFT    3

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_CONTROL :: RD_DATA_DLY [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_RD_DATA_DLY_MASK   0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_RD_DATA_DLY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_RD_DATA_DLY_BITS   3
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_RD_DATA_DLY_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL_RD_DATA_DLY_DEFAULT 0x00000007

/***************************************************************************
 *READ_FIFO_ADDR - Read fifo address pointer register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_ADDR :: reserved0 [31:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_reserved0_MASK   0xfffffff8
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_reserved0_BITS   29
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_reserved0_SHIFT  3

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_ADDR :: ADDR [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_ADDR_MASK        0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_ADDR_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_ADDR_BITS        3
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_ADDR_SHIFT       0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_ADDR_ADDR_DEFAULT     0x00000000

/***************************************************************************
 *READ_FIFO_DATA - Read fifo data register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_DATA :: DATA [31:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DATA_DATA_MASK        0xffffffff
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DATA_DATA_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DATA_DATA_BITS        32
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DATA_DATA_SHIFT       0

/***************************************************************************
 *READ_FIFO_DM_DBI - Read fifo dm/dbi register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_DM_DBI :: reserved0 [31:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_reserved0_MASK 0xfffffff0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_reserved0_BITS 28
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_reserved0_SHIFT 4

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_DM_DBI :: DM_DBI [03:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_DM_DBI_MASK    0x0000000f
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_DM_DBI_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_DM_DBI_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_DM_DBI_DM_DBI_SHIFT   0

/***************************************************************************
 *READ_FIFO_STATUS - Read fifo status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_STATUS :: reserved0 [31:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_reserved0_MASK 0xfffffffc
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_reserved0_BITS 30
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_reserved0_SHIFT 2

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_STATUS :: OVERFLOW [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_OVERFLOW_MASK  0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_OVERFLOW_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_OVERFLOW_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_OVERFLOW_SHIFT 1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_OVERFLOW_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_STATUS :: UNDERFLOW [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_UNDERFLOW_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_UNDERFLOW_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_UNDERFLOW_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_UNDERFLOW_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_STATUS_UNDERFLOW_DEFAULT 0x00000000

/***************************************************************************
 *READ_FIFO_CLEAR - Read fifo status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_reserved0_MASK  0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_reserved0_BITS  31
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_0 :: READ_FIFO_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_CLEAR_MASK      0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_CLEAR_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_CLEAR_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_CLEAR_SHIFT     0
#define DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR_CLEAR_DEFAULT   0x00000000

/***************************************************************************
 *IDLE_PAD_CONTROL - Idle mode SSTL pad control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: IDLE [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDLE_MASK      0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDLE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDLE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDLE_SHIFT     31
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDLE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: reserved0 [30:20] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved0_MASK 0x7ff00000
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved0_BITS 11
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: AUTO_DQ_RXENB_MODE [19:18] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_MASK 0x000c0000
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_SHIFT 18
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: AUTO_DQ_IDDQ_MODE [17:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_MASK 0x00030000
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: reserved_for_padding1 [15:15] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved_for_padding1_MASK 0x00008000
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved_for_padding1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved_for_padding1_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_reserved_for_padding1_SHIFT 15

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: IO_IDLE_ENABLE [14:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_MASK 0x00007ff0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_BITS 11
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: RXENB [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_RXENB_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_RXENB_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_RXENB_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_RXENB_SHIFT    3
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_RXENB_DEFAULT  0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: IDDQ [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDDQ_MASK      0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDDQ_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDDQ_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDDQ_SHIFT     2
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_IDDQ_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: DOUT_N [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_N_MASK    0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_N_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_N_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_N_SHIFT   1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_N_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: IDLE_PAD_CONTROL :: DOUT_P [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_P_MASK    0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_P_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_P_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_P_SHIFT   0
#define DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL_DOUT_P_DEFAULT 0x00000000

/***************************************************************************
 *DRIVE_PAD_CTL - SSTL pad drive characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: reserved0 [31:30] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_reserved0_MASK    0xc0000000
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_reserved0_BITS    2
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_reserved0_SHIFT   30

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_PD_IDLE_STRENGTH [29:25] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_MASK 0x3e000000
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_SHIFT 25
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_ND_IDLE_STRENGTH [24:20] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_MASK 0x01f00000
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_SHIFT 20
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_PD_TERM_STRENGTH [19:15] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_MASK 0x000f8000
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_SHIFT 15
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_ND_TERM_STRENGTH [14:10] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_MASK 0x00007c00
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_SHIFT 10
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_PD_STRENGTH [09:05] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_STRENGTH_MASK 0x000003e0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_STRENGTH_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: DRIVE_PAD_CTL :: BL_ND_STRENGTH [04:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_STRENGTH_MASK 0x0000001f
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_STRENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL_BL_ND_STRENGTH_DEFAULT 0x0000001f

/***************************************************************************
 *RD_EN_DRIVE_PAD_CTL - SSTL read enable pad drive characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DRIVE_PAD_CTL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_reserved0_BITS 12
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DRIVE_PAD_CTL :: EDC_RD_EN_PD_STRENGTH [19:15] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_MASK 0x000f8000
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_SHIFT 15
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DRIVE_PAD_CTL :: EDC_RD_EN_ND_STRENGTH [14:10] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_MASK 0x00007c00
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_SHIFT 10
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DRIVE_PAD_CTL :: BL_RD_EN_PD_STRENGTH [09:05] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_MASK 0x000003e0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_0 :: RD_EN_DRIVE_PAD_CTL :: BL_RD_EN_ND_STRENGTH [04:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_MASK 0x0000001f
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_DEFAULT 0x0000001f

/***************************************************************************
 *STATIC_PAD_CTL - pad rx and tx characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: reserved0 [31:21] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved0_MASK   0xffe00000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved0_BITS   11
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved0_SHIFT  21

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: DQS_MODE [20:20] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_MODE_MASK    0x00100000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_MODE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_MODE_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_MODE_SHIFT   20
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: reserved1 [19:18] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved1_MASK   0x000c0000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved1_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved1_BITS   2
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved1_SHIFT  18

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: EDC_MODE [17:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_EDC_MODE_MASK    0x00030000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_EDC_MODE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_EDC_MODE_BITS    2
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_EDC_MODE_SHIFT   16
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_EDC_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: reserved2 [15:15] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved2_MASK   0x00008000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved2_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved2_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved2_SHIFT  15

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: WDBI_ENABLE [14:14] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_WDBI_ENABLE_MASK 0x00004000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_WDBI_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_WDBI_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_WDBI_ENABLE_SHIFT 14
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_WDBI_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: RDBI_ENABLE [13:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RDBI_ENABLE_MASK 0x00002000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RDBI_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RDBI_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RDBI_ENABLE_SHIFT 13
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RDBI_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: DM_MODE [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DM_MODE_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DM_MODE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DM_MODE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DM_MODE_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DM_MODE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: reserved3 [11:10] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved3_MASK   0x00000c00
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved3_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved3_BITS   2
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved3_SHIFT  10

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: DQS_TX_DIS [09:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_TX_DIS_MASK  0x00000200
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_TX_DIS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_TX_DIS_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_TX_DIS_SHIFT 9
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_DQS_TX_DIS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: reserved4 [08:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved4_MASK   0x000001fc
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved4_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved4_BITS   7
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_reserved4_SHIFT  2

/* DDR34_CORE_PHY_BYTE_LANE_0 :: STATIC_PAD_CTL :: RX_MODE [01:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RX_MODE_MASK     0x00000003
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RX_MODE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RX_MODE_BITS     2
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RX_MODE_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_0_STATIC_PAD_CTL_RX_MODE_DEFAULT  0x00000000

/***************************************************************************
 *WR_PREAMBLE_MODE - Write cycle preamble control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: reserved0 [31:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved0_MASK 0xffff0000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved0_BITS 16
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved0_SHIFT 16

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: DQ_POSTAM_BITS [15:14] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_MASK 0x0000c000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_SHIFT 14
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: DQ_PREAM_BITS [13:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_PREAM_BITS_MASK 0x00003000
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_PREAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_PREAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_PREAM_BITS_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQ_PREAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: DQS [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_MASK       0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_BITS       4
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_SHIFT      8
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_DEFAULT    0x0000000e

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: reserved1 [07:05] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved1_MASK 0x000000e0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_reserved1_SHIFT 5

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: DQS_POSTAM_BITS [04:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_MASK 0x00000018
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: WR_PREAMBLE_MODE :: DQS_PREAM_BITS [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_PREAM_BITS_MASK 0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_PREAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_PREAM_BITS_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_PREAM_BITS_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE_DQS_PREAM_BITS_DEFAULT 0x00000002

/***************************************************************************
 *ODT_CONTROL - Read channel ODT control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: reserved_for_padding0 [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved_for_padding0_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved_for_padding0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved_for_padding0_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved_for_padding0_SHIFT 31

/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: reserved1 [30:10] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved1_MASK      0x7ffffc00
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved1_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved1_BITS      21
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_reserved1_SHIFT     10

/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: ODT_ENABLE [09:09] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_ENABLE_MASK     0x00000200
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_ENABLE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_ENABLE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_ENABLE_SHIFT    9
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_ENABLE_DEFAULT  0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: ODT_DELAY [08:06] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_DELAY_MASK      0x000001c0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_DELAY_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_DELAY_BITS      3
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_DELAY_SHIFT     6
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_DELAY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: ODT_POST_LENGTH [05:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_POST_LENGTH_MASK 0x00000038
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_POST_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_POST_LENGTH_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_POST_LENGTH_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_POST_LENGTH_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_0 :: ODT_CONTROL :: ODT_PRE_LENGTH [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_PRE_LENGTH_MASK 0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_PRE_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_PRE_LENGTH_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_PRE_LENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL_ODT_PRE_LENGTH_DEFAULT 0x00000003

/***************************************************************************
 *EDC_DPD_CONTROL - GDDR5M EDC digital phase detector control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: reserved0 [31:06] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_reserved0_MASK  0xffffffc0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_reserved0_BITS  26
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_reserved0_SHIFT 6

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: DQS_PHASE_OVERRIDE_VALUE [05:05] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_MASK 0x00000020
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: DQS_PHASE_OVERRIDE_ENABLE [04:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_MASK 0x00000010
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: SET_ADJ_EN [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_SET_ADJ_EN_MASK 0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_SET_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_SET_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_SET_ADJ_EN_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_SET_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: UPDATE [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_UPDATE_MASK     0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_UPDATE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_UPDATE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_UPDATE_SHIFT    2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_UPDATE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: MONITOR [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_MONITOR_MASK    0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_MONITOR_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_MONITOR_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_MONITOR_SHIFT   1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_MONITOR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_CONTROL :: INIT [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_INIT_MASK       0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_INIT_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_INIT_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_INIT_SHIFT      0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_CONTROL_INIT_DEFAULT    0x00000000

/***************************************************************************
 *EDC_DPD_STATUS - GDDR5M EDC digital phase detector status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: reserved0 [31:25] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved0_MASK   0xfe000000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved0_BITS   7
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved0_SHIFT  25

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: CURRENT_DQS_PHASE [24:24] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_DQS_PHASE_MASK 0x01000000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_DQS_PHASE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_DQS_PHASE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_DQS_PHASE_SHIFT 24
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_DQS_PHASE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: CURRENT_VDL_SETTING [23:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_VDL_SETTING_MASK 0x00ff0000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_VDL_SETTING_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_CURRENT_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved1_MASK   0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved1_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved1_BITS   3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_reserved1_SHIFT  13

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: INIT_DQS_PHASE [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DQS_PHASE_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DQS_PHASE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DQS_PHASE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DQS_PHASE_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DQS_PHASE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: INIT_VDL_SETTING [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_VDL_SETTING_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_VDL_SETTING_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: EDGE_ERROR [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_EDGE_ERROR_MASK  0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_EDGE_ERROR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_EDGE_ERROR_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_EDGE_ERROR_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_EDGE_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: MONITOR_BUSY [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_MONITOR_BUSY_MASK 0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_MONITOR_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_MONITOR_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_MONITOR_BUSY_SHIFT 2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_MONITOR_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: INIT_BUSY [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_BUSY_MASK   0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_BUSY_SHIFT  1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_STATUS :: INIT_DONE [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DONE_MASK   0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DONE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DONE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DONE_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_STATUS_INIT_DONE_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_CONTROL - GDDR5M EDC digital phase detector output signal control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_CONTROL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved0_BITS 12
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_CONTROL :: LOWER_LIMIT [19:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_MASK 0x000ff000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_CONTROL :: UPPER_LIMIT [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_CONTROL :: reserved1 [03:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_reserved1_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_STATUS - GDDR5M EDC digital phase detector output signal status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS :: reserved0 [31:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved0_MASK 0xfffff000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved0_BITS 20
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved0_SHIFT 12

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS :: VDL_SETTING [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VDL_SETTING_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VDL_SETTING_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS :: reserved1 [03:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_reserved1_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS :: VALID [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VALID_MASK   0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VALID_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VALID_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VALID_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_VALID_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_STATUS_CLEAR - GDDR5M EDC digital phase detector output signal status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_reserved0_BITS 31
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_DPD_OUT_STATUS_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_DEFAULT 0x00000000

/***************************************************************************
 *EDC_CRC_CONTROL - GDDR5M EDC signal path CRC control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: reserved1 [15:11] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved1_MASK  0x0000f800
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved1_BITS  5
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved1_SHIFT 11

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: CRCWL [10:08] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCWL_MASK      0x00000700
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCWL_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCWL_BITS      3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCWL_SHIFT     8
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCWL_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: reserved2 [07:06] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved2_MASK  0x000000c0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved2_BITS  2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved2_SHIFT 6

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: CRCRL [05:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCRL_MASK      0x00000030
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCRL_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCRL_BITS      2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCRL_SHIFT     4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_CRCRL_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: reserved3 [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved3_MASK  0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved3_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved3_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_reserved3_SHIFT 3

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: COUNT_MODE [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_COUNT_MODE_MASK 0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_COUNT_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_COUNT_MODE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_COUNT_MODE_SHIFT 2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_COUNT_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: ENABLE_WR [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_WR_MASK  0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_WR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_WR_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_WR_SHIFT 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_WR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_CONTROL :: ENABLE_RD [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_RD_MASK  0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_RD_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_RD_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_RD_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_CONTROL_ENABLE_RD_DEFAULT 0x00000000

/***************************************************************************
 *EDC_CRC_STATUS - GDDR5M EDC signal path CRC status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS :: reserved0 [31:04] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_reserved0_MASK   0xfffffff0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_reserved0_BITS   28
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_reserved0_SHIFT  4

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS :: WR_FAIL [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_FAIL_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_FAIL_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_FAIL_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_FAIL_SHIFT    3
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_FAIL_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS :: WR_PASS [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_PASS_MASK     0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_PASS_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_PASS_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_PASS_SHIFT    2
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_WR_PASS_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS :: RD_FAIL [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_FAIL_MASK     0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_FAIL_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_FAIL_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_FAIL_SHIFT    1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_FAIL_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS :: RD_PASS [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_PASS_MASK     0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_PASS_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_PASS_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_PASS_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_RD_PASS_DEFAULT  0x00000000

/***************************************************************************
 *EDC_CRC_COUNT - GDDR5M EDC signal path CRC counter register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_COUNT :: reserved0 [31:28] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved0_MASK    0xf0000000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved0_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved0_SHIFT   28

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_COUNT :: WR_COUNT [27:16] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_WR_COUNT_MASK     0x0fff0000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_WR_COUNT_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_WR_COUNT_BITS     12
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_WR_COUNT_SHIFT    16
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_WR_COUNT_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_COUNT :: reserved1 [15:12] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved1_MASK    0x0000f000
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved1_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved1_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_reserved1_SHIFT   12

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_COUNT :: RD_COUNT [11:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_RD_COUNT_MASK     0x00000fff
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_RD_COUNT_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_RD_COUNT_BITS     12
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_RD_COUNT_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_COUNT_RD_COUNT_DEFAULT  0x00000000

/***************************************************************************
 *EDC_CRC_STATUS_CLEAR - GDDR5M EDC signal path CRC counter register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_reserved0_BITS 31
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_0 :: EDC_CRC_STATUS_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_CLEAR_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_CLEAR_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_CLEAR_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_EDC_CRC_STATUS_CLEAR_CLEAR_DEFAULT 0x00000000

/***************************************************************************
 *BL_SPARE_REG - Byte-Lane Spare register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_0 :: BL_SPARE_REG :: reserved_for_eco0 [31:00] */
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG_reserved_for_eco0_MASK 0xffffffff
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG_reserved_for_eco0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG_reserved_for_eco0_BITS 32
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG_reserved_for_eco0_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_0_BL_SPARE_REG_reserved_for_eco0_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQS_P - Write channel DQS-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_BUSY_MASK  0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_BUSY_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_P :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQS_N - Write channel DQS-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_BUSY_MASK  0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_BUSY_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQS_N :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ0 - Write channel DQ0 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ0 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ1 - Write channel DQ1 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ1 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ2 - Write channel DQ2 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ2 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ3 - Write channel DQ3 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ3 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ4 - Write channel DQ4 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ4 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ5 - Write channel DQ5 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ5 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ6 - Write channel DQ6 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ6 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DQ7 - Write channel DQ7 VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DQ7 :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_DM - Write channel DM VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_BUSY_MASK     0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_BUSY_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_BUSY_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_BUSY_SHIFT    31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_BUSY_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_FORCE_MASK    0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_FORCE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_FORCE_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_FORCE_SHIFT   16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_ADJ_EN_MASK   0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_ADJ_EN_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_ADJ_EN_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_ADJ_EN_SHIFT  12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_DM :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_WR_EDC - Write channel EDC VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved2_MASK 0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved2_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_WR_EDC :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_VDL_STEP_MASK 0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_VDL_STEP_BITS 9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQSP - Read channel DQSP VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQSN - Read channel DQSP VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQSN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ0P - Read channel DQ0-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ0N - Read channel DQ0-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ0N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ1P - Read channel DQ1-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ1N - Read channel DQ1-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ1N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ2P - Read channel DQ2-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ2N - Read channel DQ2-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ2N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ3P - Read channel DQ3-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ3N - Read channel DQ3-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ3N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ4P - Read channel DQ4-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ4N - Read channel DQ4-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ4N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ5P - Read channel DQ5-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ5N - Read channel DQ5-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ5N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ6P - Read channel DQ6-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ6N - Read channel DQ6-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ6N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ7P - Read channel DQ7-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7P :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DQ7N - Read channel DQ7-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DQ7N :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DMP - Read channel DM-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_DMN - Read channel DM-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_BUSY_MASK    0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_BUSY_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_BUSY_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_BUSY_SHIFT   31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_FORCE_MASK   0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_FORCE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_FORCE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_FORCE_SHIFT  16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_ADJ_EN_MASK  0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_ADJ_EN_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_DMN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EDCP - Read channel EDC-P VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCP :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EDCN - Read channel EDC-N VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_BUSY_MASK   0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_BUSY_SHIFT  31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_FORCE_MASK  0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_FORCE_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EDCN :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EN_CS0 - Read channel CS_N[0] read enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS0 :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CONTROL_RD_EN_CS1 - Read channel CS_N[1] read enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_BUSY_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_BUSY_SHIFT 31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved0_MASK 0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved0_BITS 14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_FORCE_MASK 0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_FORCE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_FORCE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_FORCE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_FORCE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved1_MASK 0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_ADJ_EN_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_ADJ_EN_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: reserved2 [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved2_MASK 0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved2_BITS 4
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_reserved2_SHIFT 8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CONTROL_RD_EN_CS1 :: VDL_STEP [07:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_VDL_STEP_MASK 0x000000ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_VDL_STEP_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_VDL_STEP_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_VDL_STEP_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_CLK_CONTROL - DDR interface signal Write Leveling CLK VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved1_MASK  0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved1_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_ADJ_EN_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_ADJ_EN_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_ADJ_EN_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_ADJ_EN_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_ADJ_EN_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved2_MASK  0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved2_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_CLK_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_VDL_STEP_MASK   0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_VDL_STEP_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_VDL_STEP_BITS   9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_VDL_STEP_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_CLK_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *VDL_LDE_CONTROL - DDR interface signal Write Leveling Capture Enable VDL control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved1_MASK  0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved1_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved1_SHIFT 13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: ADJ_EN [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_ADJ_EN_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_ADJ_EN_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_ADJ_EN_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_ADJ_EN_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_ADJ_EN_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: reserved2 [11:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved2_MASK  0x00000e00
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved2_BITS  3
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_reserved2_SHIFT 9

/* DDR34_CORE_PHY_BYTE_LANE_1 :: VDL_LDE_CONTROL :: VDL_STEP [08:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_VDL_STEP_MASK   0x000001ff
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_VDL_STEP_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_VDL_STEP_BITS   9
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_VDL_STEP_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_VDL_LDE_CONTROL_VDL_STEP_DEFAULT 0x00000000

/***************************************************************************
 *RD_EN_DLY_CYC - Read enable bit-clock cycle delay control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_BUSY_MASK         0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_BUSY_ALIGN        0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_BUSY_BITS         1
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_BUSY_SHIFT        31
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_BUSY_DEFAULT      0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved0_MASK    0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved0_BITS    14
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved0_SHIFT   17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_FORCE_MASK        0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_FORCE_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_FORCE_BITS        1
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_FORCE_SHIFT       16
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_FORCE_DEFAULT     0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: reserved1 [15:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved1_MASK    0x0000ff00
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved1_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved1_BITS    8
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_reserved1_SHIFT   8

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: CS1_CYCLES [07:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS1_CYCLES_MASK   0x000000f0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS1_CYCLES_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS1_CYCLES_BITS   4
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS1_CYCLES_SHIFT  4
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS1_CYCLES_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DLY_CYC :: CS0_CYCLES [03:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS0_CYCLES_MASK   0x0000000f
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS0_CYCLES_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS0_CYCLES_BITS   4
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS0_CYCLES_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC_CS0_CYCLES_DEFAULT 0x00000000

/***************************************************************************
 *WR_CHAN_DLY_CYC - Write leveling bit-clock cycle delay control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_CHAN_DLY_CYC :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_CHAN_DLY_CYC :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_CHAN_DLY_CYC :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_CHAN_DLY_CYC :: reserved1 [15:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved1_MASK  0x0000fff8
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved1_BITS  13
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_reserved1_SHIFT 3

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_CHAN_DLY_CYC :: CYCLES [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_CYCLES_MASK     0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_CYCLES_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_CYCLES_BITS     3
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_CYCLES_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC_CYCLES_DEFAULT  0x00000000

/***************************************************************************
 *READ_CONTROL - Read channel datapath control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_CONTROL :: reserved0 [31:05] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved0_MASK     0xffffffe0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved0_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved0_BITS     27
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved0_SHIFT    5

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_CONTROL :: MODE [04:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_MODE_MASK          0x00000010
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_MODE_ALIGN         0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_MODE_BITS          1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_MODE_SHIFT         4
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_MODE_DEFAULT       0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_CONTROL :: reserved1 [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved1_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved1_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved1_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_reserved1_SHIFT    3

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_CONTROL :: RD_DATA_DLY [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_RD_DATA_DLY_MASK   0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_RD_DATA_DLY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_RD_DATA_DLY_BITS   3
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_RD_DATA_DLY_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL_RD_DATA_DLY_DEFAULT 0x00000007

/***************************************************************************
 *READ_FIFO_ADDR - Read fifo address pointer register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_ADDR :: reserved0 [31:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_reserved0_MASK   0xfffffff8
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_reserved0_BITS   29
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_reserved0_SHIFT  3

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_ADDR :: ADDR [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_ADDR_MASK        0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_ADDR_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_ADDR_BITS        3
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_ADDR_SHIFT       0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_ADDR_ADDR_DEFAULT     0x00000000

/***************************************************************************
 *READ_FIFO_DATA - Read fifo data register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_DATA :: DATA [31:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DATA_DATA_MASK        0xffffffff
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DATA_DATA_ALIGN       0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DATA_DATA_BITS        32
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DATA_DATA_SHIFT       0

/***************************************************************************
 *READ_FIFO_DM_DBI - Read fifo dm/dbi register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_DM_DBI :: reserved0 [31:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_reserved0_MASK 0xfffffff0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_reserved0_BITS 28
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_reserved0_SHIFT 4

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_DM_DBI :: DM_DBI [03:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_DM_DBI_MASK    0x0000000f
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_DM_DBI_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_DM_DBI_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_DM_DBI_DM_DBI_SHIFT   0

/***************************************************************************
 *READ_FIFO_STATUS - Read fifo status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_STATUS :: reserved0 [31:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_reserved0_MASK 0xfffffffc
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_reserved0_BITS 30
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_reserved0_SHIFT 2

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_STATUS :: OVERFLOW [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_OVERFLOW_MASK  0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_OVERFLOW_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_OVERFLOW_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_OVERFLOW_SHIFT 1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_OVERFLOW_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_STATUS :: UNDERFLOW [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_UNDERFLOW_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_UNDERFLOW_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_UNDERFLOW_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_UNDERFLOW_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_STATUS_UNDERFLOW_DEFAULT 0x00000000

/***************************************************************************
 *READ_FIFO_CLEAR - Read fifo status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_reserved0_MASK  0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_reserved0_BITS  31
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_1 :: READ_FIFO_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_CLEAR_MASK      0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_CLEAR_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_CLEAR_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_CLEAR_SHIFT     0
#define DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR_CLEAR_DEFAULT   0x00000000

/***************************************************************************
 *IDLE_PAD_CONTROL - Idle mode SSTL pad control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: IDLE [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDLE_MASK      0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDLE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDLE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDLE_SHIFT     31
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDLE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: reserved0 [30:20] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved0_MASK 0x7ff00000
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved0_BITS 11
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: AUTO_DQ_RXENB_MODE [19:18] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_MASK 0x000c0000
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_SHIFT 18
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_RXENB_MODE_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: AUTO_DQ_IDDQ_MODE [17:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_MASK 0x00030000
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_AUTO_DQ_IDDQ_MODE_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: reserved_for_padding1 [15:15] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved_for_padding1_MASK 0x00008000
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved_for_padding1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved_for_padding1_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_reserved_for_padding1_SHIFT 15

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: IO_IDLE_ENABLE [14:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_MASK 0x00007ff0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_BITS 11
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IO_IDLE_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: RXENB [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_RXENB_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_RXENB_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_RXENB_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_RXENB_SHIFT    3
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_RXENB_DEFAULT  0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: IDDQ [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDDQ_MASK      0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDDQ_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDDQ_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDDQ_SHIFT     2
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_IDDQ_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: DOUT_N [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_N_MASK    0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_N_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_N_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_N_SHIFT   1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_N_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: IDLE_PAD_CONTROL :: DOUT_P [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_P_MASK    0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_P_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_P_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_P_SHIFT   0
#define DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL_DOUT_P_DEFAULT 0x00000000

/***************************************************************************
 *DRIVE_PAD_CTL - SSTL pad drive characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: reserved0 [31:30] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_reserved0_MASK    0xc0000000
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_reserved0_BITS    2
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_reserved0_SHIFT   30

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_PD_IDLE_STRENGTH [29:25] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_MASK 0x3e000000
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_SHIFT 25
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_ND_IDLE_STRENGTH [24:20] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_MASK 0x01f00000
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_SHIFT 20
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_IDLE_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_PD_TERM_STRENGTH [19:15] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_MASK 0x000f8000
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_SHIFT 15
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_ND_TERM_STRENGTH [14:10] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_MASK 0x00007c00
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_SHIFT 10
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_TERM_STRENGTH_DEFAULT 0x00000006

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_PD_STRENGTH [09:05] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_STRENGTH_MASK 0x000003e0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_STRENGTH_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: DRIVE_PAD_CTL :: BL_ND_STRENGTH [04:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_STRENGTH_MASK 0x0000001f
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_STRENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL_BL_ND_STRENGTH_DEFAULT 0x0000001f

/***************************************************************************
 *RD_EN_DRIVE_PAD_CTL - SSTL read enable pad drive characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DRIVE_PAD_CTL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_reserved0_BITS 12
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DRIVE_PAD_CTL :: EDC_RD_EN_PD_STRENGTH [19:15] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_MASK 0x000f8000
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_SHIFT 15
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DRIVE_PAD_CTL :: EDC_RD_EN_ND_STRENGTH [14:10] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_MASK 0x00007c00
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_SHIFT 10
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_EDC_RD_EN_ND_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DRIVE_PAD_CTL :: BL_RD_EN_PD_STRENGTH [09:05] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_MASK 0x000003e0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_PD_STRENGTH_DEFAULT 0x0000001f

/* DDR34_CORE_PHY_BYTE_LANE_1 :: RD_EN_DRIVE_PAD_CTL :: BL_RD_EN_ND_STRENGTH [04:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_MASK 0x0000001f
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_BITS 5
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL_BL_RD_EN_ND_STRENGTH_DEFAULT 0x0000001f

/***************************************************************************
 *STATIC_PAD_CTL - pad rx and tx characteristics control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: reserved0 [31:21] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved0_MASK   0xffe00000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved0_BITS   11
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved0_SHIFT  21

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: DQS_MODE [20:20] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_MODE_MASK    0x00100000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_MODE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_MODE_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_MODE_SHIFT   20
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: reserved1 [19:18] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved1_MASK   0x000c0000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved1_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved1_BITS   2
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved1_SHIFT  18

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: EDC_MODE [17:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_EDC_MODE_MASK    0x00030000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_EDC_MODE_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_EDC_MODE_BITS    2
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_EDC_MODE_SHIFT   16
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_EDC_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: reserved2 [15:15] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved2_MASK   0x00008000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved2_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved2_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved2_SHIFT  15

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: WDBI_ENABLE [14:14] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_WDBI_ENABLE_MASK 0x00004000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_WDBI_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_WDBI_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_WDBI_ENABLE_SHIFT 14
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_WDBI_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: RDBI_ENABLE [13:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RDBI_ENABLE_MASK 0x00002000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RDBI_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RDBI_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RDBI_ENABLE_SHIFT 13
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RDBI_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: DM_MODE [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DM_MODE_MASK     0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DM_MODE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DM_MODE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DM_MODE_SHIFT    12
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DM_MODE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: reserved3 [11:10] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved3_MASK   0x00000c00
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved3_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved3_BITS   2
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved3_SHIFT  10

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: DQS_TX_DIS [09:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_TX_DIS_MASK  0x00000200
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_TX_DIS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_TX_DIS_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_TX_DIS_SHIFT 9
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_DQS_TX_DIS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: reserved4 [08:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved4_MASK   0x000001fc
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved4_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved4_BITS   7
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_reserved4_SHIFT  2

/* DDR34_CORE_PHY_BYTE_LANE_1 :: STATIC_PAD_CTL :: RX_MODE [01:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RX_MODE_MASK     0x00000003
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RX_MODE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RX_MODE_BITS     2
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RX_MODE_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_1_STATIC_PAD_CTL_RX_MODE_DEFAULT  0x00000000

/***************************************************************************
 *WR_PREAMBLE_MODE - Write cycle preamble control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: reserved0 [31:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved0_MASK 0xffff0000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved0_BITS 16
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved0_SHIFT 16

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: DQ_POSTAM_BITS [15:14] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_MASK 0x0000c000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_SHIFT 14
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_POSTAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: DQ_PREAM_BITS [13:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_PREAM_BITS_MASK 0x00003000
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_PREAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_PREAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_PREAM_BITS_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQ_PREAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: DQS [11:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_MASK       0x00000f00
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_BITS       4
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_SHIFT      8
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_DEFAULT    0x0000000e

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: reserved1 [07:05] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved1_MASK 0x000000e0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_reserved1_SHIFT 5

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: DQS_POSTAM_BITS [04:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_MASK 0x00000018
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_BITS 2
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_POSTAM_BITS_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: WR_PREAMBLE_MODE :: DQS_PREAM_BITS [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_PREAM_BITS_MASK 0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_PREAM_BITS_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_PREAM_BITS_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_PREAM_BITS_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE_DQS_PREAM_BITS_DEFAULT 0x00000002

/***************************************************************************
 *ODT_CONTROL - Read channel ODT control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: reserved_for_padding0 [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved_for_padding0_MASK 0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved_for_padding0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved_for_padding0_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved_for_padding0_SHIFT 31

/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: reserved1 [30:10] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved1_MASK      0x7ffffc00
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved1_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved1_BITS      21
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_reserved1_SHIFT     10

/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: ODT_ENABLE [09:09] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_ENABLE_MASK     0x00000200
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_ENABLE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_ENABLE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_ENABLE_SHIFT    9
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_ENABLE_DEFAULT  0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: ODT_DELAY [08:06] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_DELAY_MASK      0x000001c0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_DELAY_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_DELAY_BITS      3
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_DELAY_SHIFT     6
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_DELAY_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: ODT_POST_LENGTH [05:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_POST_LENGTH_MASK 0x00000038
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_POST_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_POST_LENGTH_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_POST_LENGTH_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_POST_LENGTH_DEFAULT 0x00000001

/* DDR34_CORE_PHY_BYTE_LANE_1 :: ODT_CONTROL :: ODT_PRE_LENGTH [02:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_PRE_LENGTH_MASK 0x00000007
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_PRE_LENGTH_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_PRE_LENGTH_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_PRE_LENGTH_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL_ODT_PRE_LENGTH_DEFAULT 0x00000003

/***************************************************************************
 *EDC_DPD_CONTROL - GDDR5M EDC digital phase detector control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: reserved0 [31:06] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_reserved0_MASK  0xffffffc0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_reserved0_BITS  26
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_reserved0_SHIFT 6

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: DQS_PHASE_OVERRIDE_VALUE [05:05] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_MASK 0x00000020
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_SHIFT 5
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_VALUE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: DQS_PHASE_OVERRIDE_ENABLE [04:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_MASK 0x00000010
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_DQS_PHASE_OVERRIDE_ENABLE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: SET_ADJ_EN [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_SET_ADJ_EN_MASK 0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_SET_ADJ_EN_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_SET_ADJ_EN_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_SET_ADJ_EN_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_SET_ADJ_EN_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: UPDATE [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_UPDATE_MASK     0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_UPDATE_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_UPDATE_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_UPDATE_SHIFT    2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_UPDATE_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: MONITOR [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_MONITOR_MASK    0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_MONITOR_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_MONITOR_BITS    1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_MONITOR_SHIFT   1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_MONITOR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_CONTROL :: INIT [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_INIT_MASK       0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_INIT_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_INIT_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_INIT_SHIFT      0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_CONTROL_INIT_DEFAULT    0x00000000

/***************************************************************************
 *EDC_DPD_STATUS - GDDR5M EDC digital phase detector status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: reserved0 [31:25] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved0_MASK   0xfe000000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved0_BITS   7
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved0_SHIFT  25

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: CURRENT_DQS_PHASE [24:24] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_DQS_PHASE_MASK 0x01000000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_DQS_PHASE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_DQS_PHASE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_DQS_PHASE_SHIFT 24
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_DQS_PHASE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: CURRENT_VDL_SETTING [23:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_VDL_SETTING_MASK 0x00ff0000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_VDL_SETTING_SHIFT 16
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_CURRENT_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: reserved1 [15:13] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved1_MASK   0x0000e000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved1_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved1_BITS   3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_reserved1_SHIFT  13

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: INIT_DQS_PHASE [12:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DQS_PHASE_MASK 0x00001000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DQS_PHASE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DQS_PHASE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DQS_PHASE_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DQS_PHASE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: INIT_VDL_SETTING [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_VDL_SETTING_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_VDL_SETTING_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: EDGE_ERROR [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_EDGE_ERROR_MASK  0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_EDGE_ERROR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_EDGE_ERROR_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_EDGE_ERROR_SHIFT 3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_EDGE_ERROR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: MONITOR_BUSY [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_MONITOR_BUSY_MASK 0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_MONITOR_BUSY_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_MONITOR_BUSY_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_MONITOR_BUSY_SHIFT 2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_MONITOR_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: INIT_BUSY [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_BUSY_MASK   0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_BUSY_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_BUSY_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_BUSY_SHIFT  1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_BUSY_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_STATUS :: INIT_DONE [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DONE_MASK   0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DONE_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DONE_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DONE_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_STATUS_INIT_DONE_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_CONTROL - GDDR5M EDC digital phase detector output signal control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_CONTROL :: reserved0 [31:20] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved0_MASK 0xfff00000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved0_BITS 12
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved0_SHIFT 20

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_CONTROL :: LOWER_LIMIT [19:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_MASK 0x000ff000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_SHIFT 12
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_LOWER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_CONTROL :: UPPER_LIMIT [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_UPPER_LIMIT_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_CONTROL :: reserved1 [03:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_reserved1_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_CONTROL :: ENABLE [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_ENABLE_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_ENABLE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_ENABLE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_ENABLE_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_CONTROL_ENABLE_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_STATUS - GDDR5M EDC digital phase detector output signal status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS :: reserved0 [31:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved0_MASK 0xfffff000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved0_BITS 20
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved0_SHIFT 12

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS :: VDL_SETTING [11:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VDL_SETTING_MASK 0x00000ff0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VDL_SETTING_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VDL_SETTING_BITS 8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VDL_SETTING_SHIFT 4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VDL_SETTING_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS :: reserved1 [03:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved1_MASK 0x0000000e
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved1_BITS 3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_reserved1_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS :: VALID [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VALID_MASK   0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VALID_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VALID_BITS   1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VALID_SHIFT  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_VALID_DEFAULT 0x00000000

/***************************************************************************
 *EDC_DPD_OUT_STATUS_CLEAR - GDDR5M EDC digital phase detector output signal status clear register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_reserved0_BITS 31
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_DPD_OUT_STATUS_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_DPD_OUT_STATUS_CLEAR_CLEAR_DEFAULT 0x00000000

/***************************************************************************
 *EDC_CRC_CONTROL - GDDR5M EDC signal path CRC control register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: BUSY [31:31] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_BUSY_MASK       0x80000000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_BUSY_ALIGN      0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_BUSY_BITS       1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_BUSY_SHIFT      31
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_BUSY_DEFAULT    0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: reserved0 [30:17] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved0_MASK  0x7ffe0000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved0_BITS  14
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved0_SHIFT 17

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: FORCE [16:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_FORCE_MASK      0x00010000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_FORCE_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_FORCE_BITS      1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_FORCE_SHIFT     16
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_FORCE_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: reserved1 [15:11] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved1_MASK  0x0000f800
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved1_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved1_BITS  5
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved1_SHIFT 11

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: CRCWL [10:08] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCWL_MASK      0x00000700
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCWL_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCWL_BITS      3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCWL_SHIFT     8
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCWL_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: reserved2 [07:06] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved2_MASK  0x000000c0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved2_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved2_BITS  2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved2_SHIFT 6

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: CRCRL [05:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCRL_MASK      0x00000030
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCRL_ALIGN     0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCRL_BITS      2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCRL_SHIFT     4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_CRCRL_DEFAULT   0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: reserved3 [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved3_MASK  0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved3_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved3_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_reserved3_SHIFT 3

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: COUNT_MODE [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_COUNT_MODE_MASK 0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_COUNT_MODE_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_COUNT_MODE_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_COUNT_MODE_SHIFT 2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_COUNT_MODE_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: ENABLE_WR [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_WR_MASK  0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_WR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_WR_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_WR_SHIFT 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_WR_DEFAULT 0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_CONTROL :: ENABLE_RD [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_RD_MASK  0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_RD_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_RD_BITS  1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_RD_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_CONTROL_ENABLE_RD_DEFAULT 0x00000000

/***************************************************************************
 *EDC_CRC_STATUS - GDDR5M EDC signal path CRC status register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS :: reserved0 [31:04] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_reserved0_MASK   0xfffffff0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_reserved0_ALIGN  0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_reserved0_BITS   28
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_reserved0_SHIFT  4

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS :: WR_FAIL [03:03] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_FAIL_MASK     0x00000008
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_FAIL_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_FAIL_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_FAIL_SHIFT    3
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_FAIL_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS :: WR_PASS [02:02] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_PASS_MASK     0x00000004
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_PASS_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_PASS_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_PASS_SHIFT    2
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_WR_PASS_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS :: RD_FAIL [01:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_FAIL_MASK     0x00000002
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_FAIL_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_FAIL_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_FAIL_SHIFT    1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_FAIL_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS :: RD_PASS [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_PASS_MASK     0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_PASS_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_PASS_BITS     1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_PASS_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_RD_PASS_DEFAULT  0x00000000

/***************************************************************************
 *EDC_CRC_COUNT - GDDR5M EDC signal path CRC counter register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_COUNT :: reserved0 [31:28] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved0_MASK    0xf0000000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved0_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved0_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved0_SHIFT   28

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_COUNT :: WR_COUNT [27:16] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_WR_COUNT_MASK     0x0fff0000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_WR_COUNT_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_WR_COUNT_BITS     12
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_WR_COUNT_SHIFT    16
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_WR_COUNT_DEFAULT  0x00000000

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_COUNT :: reserved1 [15:12] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved1_MASK    0x0000f000
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved1_ALIGN   0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved1_BITS    4
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_reserved1_SHIFT   12

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_COUNT :: RD_COUNT [11:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_RD_COUNT_MASK     0x00000fff
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_RD_COUNT_ALIGN    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_RD_COUNT_BITS     12
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_RD_COUNT_SHIFT    0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_COUNT_RD_COUNT_DEFAULT  0x00000000

/***************************************************************************
 *EDC_CRC_STATUS_CLEAR - GDDR5M EDC signal path CRC counter register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS_CLEAR :: reserved0 [31:01] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_reserved0_MASK 0xfffffffe
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_reserved0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_reserved0_BITS 31
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_reserved0_SHIFT 1

/* DDR34_CORE_PHY_BYTE_LANE_1 :: EDC_CRC_STATUS_CLEAR :: CLEAR [00:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_CLEAR_MASK 0x00000001
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_CLEAR_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_CLEAR_BITS 1
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_CLEAR_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_EDC_CRC_STATUS_CLEAR_CLEAR_DEFAULT 0x00000000

/***************************************************************************
 *BL_SPARE_REG - Byte-Lane Spare register
 ***************************************************************************/
/* DDR34_CORE_PHY_BYTE_LANE_1 :: BL_SPARE_REG :: reserved_for_eco0 [31:00] */
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG_reserved_for_eco0_MASK 0xffffffff
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG_reserved_for_eco0_ALIGN 0
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG_reserved_for_eco0_BITS 32
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG_reserved_for_eco0_SHIFT 0
#define DDR34_CORE_PHY_BYTE_LANE_1_BL_SPARE_REG_reserved_for_eco0_DEFAULT 0x00000000

#endif /* __SOC_BROADCOM_CYGNUS_PHY_AND28_E2_H__ */

/* End of File */
