/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

void DutyCycleCtrl(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, FreqId, i;

	if (DIMMFREQ_800 == DramAttr->DramFreq)
		FreqId = 2;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		FreqId = 3;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		FreqId = 4;
	else if (DIMMFREQ_400 == DramAttr->DramFreq)
		FreqId = 5;
	else
		FreqId = 5;

	if (DramAttr->RankNumChA > 0) { /* 1 rank */
		for (i = 0; i < DUTY_CYCLE_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
						ChA_Duty_Control_DDR2[i][0]);
			Data &= ChA_Duty_Control_DDR2[i][1]; /* mask */
			Data |= ChA_Duty_Control_DDR2[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
					  ChA_Duty_Control_DDR2[i][0], Data);
		}
	}

	if (1 == ENABLE_CHC) { /* 1 rank */
		for (i = 0; i < DUTY_CYCLE_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
						ChB_Duty_Control_DDR2[i][0]);
			Data &= ChB_Duty_Control_DDR2[i][1]; /* mask */
			Data |= ChB_Duty_Control_DDR2[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
					  ChB_Duty_Control_DDR2[i][0], Data);
		}
	}
}

/*
 * DRAM clock phase and delay control
 */

/* Subroutine list */
void ClkPhsCtrlFBMDDR2(DRAM_SYS_ATTR *DramAttr);
void WrtDataPhsCtrl(DRAM_SYS_ATTR *DramAttr);
void DQDQSOutputDlyCtrl(DRAM_SYS_ATTR *DramAttr);
void DQSInputCaptureCtrl(DRAM_SYS_ATTR *DramAttr);
void DCLKPhsCtrl(DRAM_SYS_ATTR *DramAttr);

void DRAMClkCtrl(DRAM_SYS_ATTR *DramAttr)
{
	/* Write data clock phase control. */
	WrtDataPhsCtrl(DramAttr);
	/* Clock phase control */
	ClkPhsCtrlFBMDDR2(DramAttr);
	 /**/ DQDQSOutputDlyCtrl(DramAttr);
	 /**/ DQSInputCaptureCtrl(DramAttr);
	DCLKPhsCtrl(DramAttr);
}

void ClkPhsCtrlFBMDDR2(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, FreqId, i;

	if (DramAttr->DramFreq == DIMMFREQ_800)
		FreqId = 2;
	else if (DramAttr->DramFreq == DIMMFREQ_667)
		FreqId = 3;
	else if (DramAttr->DramFreq == DIMMFREQ_533)
		FreqId = 4;
	else if (DramAttr->DramFreq == DIMMFREQ_400)
		FreqId = 5;
	else
		FreqId = 5;

	/* Channel A */
	// 2~4 Rank
	if (DramAttr->RankNumChA == 1) { /* 1 rank */
		for (i = 0; i < 3; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChA_Clk_Phase_Table_1R[i][0]);
			Data &= DDR2_ChA_Clk_Phase_Table_1R[i][1]; /* mask */
			Data |= DDR2_ChA_Clk_Phase_Table_1R[i][FreqId];	/* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChA_Clk_Phase_Table_1R[i][0], Data);
		}
	} else if (DramAttr->RankNumChA > 1) {	/* 2~4 Rank */
		for (i = 0; i < 3; i++) {
			Data = pci_read_config8(MEMCTRL,
				     DDR2_ChA_Clk_Phase_Table_2R[i][0]);
			Data &= DDR2_ChA_Clk_Phase_Table_2R[i][1]; /* mask */
			Data |= DDR2_ChA_Clk_Phase_Table_2R[i][FreqId];	/* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChA_Clk_Phase_Table_2R[i][0], Data);
		}
	}
#if ENABLE_CHB
	if (DramAttr->RankNumChB > 0) { /* 1 rank */
		for (i = 0; i < 3; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChB_Clk_Phase_Table_1R[i][0]);
			Data &= DDR2_ChB_Clk_Phase_Table_1R[i][1]; /* mask */
			Data |= DDR2_ChB_Clk_Phase_Table_1R[i][FreqId];	/* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChB_Clk_Phase_Table_1R[i][0], Data);
		}
	}
#endif
}

void WrtDataPhsCtrl(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, FreqId, i;

	if (DIMMFREQ_800 == DramAttr->DramFreq)
		FreqId = 2;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		FreqId = 3;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		FreqId = 4;
	else if (DIMMFREQ_400 == DramAttr->DramFreq)
		FreqId = 5;
	else
		FreqId = 5;

	if (DramAttr->RankNumChA > 0) { /* 1 rank */
		for (i = 0; i < WrtData_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChA_WrtData_Phase_Table[i][0]);
			Data &= DDR2_ChA_WrtData_Phase_Table[i][1]; /* mask */
			Data |= DDR2_ChA_WrtData_Phase_Table[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChA_WrtData_Phase_Table[i][0], Data);
		}
	}
#if ENABLE_CHB
	if (DramAttr->RankNumChB > 0) {	/* 1 rank */
		for (i = 0; i < WrtData_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChB_WrtData_Phase_Table[i][0]);
			Data &= DDR2_ChB_WrtData_Phase_Table[i][1]; /* mask */
			Data |= DDR2_ChB_WrtData_Phase_Table[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChB_WrtData_Phase_Table[i][0], Data);
		}
	}
#endif
	Data = pci_read_config8(MEMCTRL, 0x8C);
	Data &= 0xFC;
	Data |= 0x03;
	pci_write_config8(MEMCTRL, 0x8C, Data);
}

void DQDQSOutputDlyCtrl(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, FreqId;

	if (DIMMFREQ_400 == DramAttr->DramFreq)
		FreqId = 0;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		FreqId = 1;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		FreqId = 2;
	else if (DIMMFREQ_800 == DramAttr->DramFreq)
		FreqId = 2;
	else
		FreqId = 0;

	if (DramAttr->RankNumChA > 0) {
		Data = DDR2_CHA_DQ_DQS_Delay_Table[FreqId][0];
		pci_write_config8(MEMCTRL, 0xf0, Data);

		Data = DDR2_CHA_DQ_DQS_Delay_Table[FreqId][1];
		pci_write_config8(MEMCTRL, 0xf1, Data);

		Data = DDR2_CHA_DQ_DQS_Delay_Table[FreqId][2];
		pci_write_config8(MEMCTRL, 0xf2, Data);

		Data = DDR2_CHA_DQ_DQS_Delay_Table[FreqId][3];
		pci_write_config8(MEMCTRL, 0xf3, Data);
	}
#if ENABLE_CHB
	if (DramAttr->RankNumChB > 0) {
		Data = DDR2_CHB_DQ_DQS_Delay_Table[FreqId][0];
		pci_write_config8(MEMCTRL, 0xf4, Data);

		Data = DDR2_CHB_DQ_DQS_Delay_Table[FreqId][1];
		pci_write_config8(MEMCTRL, 0xf5, Data);

		Data = DDR2_CHB_DQ_DQS_Delay_Table[FreqId][2];
		pci_write_config8(MEMCTRL, 0xf6, Data);

		Data = DDR2_CHB_DQ_DQS_Delay_Table[FreqId][3];
		pci_write_config8(MEMCTRL, 0xf7, Data);
	}
#endif
}

void DQSInputCaptureCtrl(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data, FreqId, i;

	if (DIMMFREQ_800 == DramAttr->DramFreq)
		FreqId = 2;
	else if (DIMMFREQ_667 == DramAttr->DramFreq)
		FreqId = 3;
	else if (DIMMFREQ_533 == DramAttr->DramFreq)
		FreqId = 4;
	else if (DIMMFREQ_400 == DramAttr->DramFreq)
		FreqId = 5;
	else
		FreqId = 2;

	Data = 0x8A;
	pci_write_config8(MEMCTRL, 0x77, Data);

	if (DramAttr->RankNumChA > 0) { /* 1 rank */
		for (i = 0; i < DQS_INPUT_CAPTURE_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChA_DQS_Input_Capture_Tbl[i][0]);
			Data &= DDR2_ChA_DQS_Input_Capture_Tbl[i][1]; /* mask */
			Data |= DDR2_ChA_DQS_Input_Capture_Tbl[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
				DDR2_ChA_DQS_Input_Capture_Tbl[i][0], Data);
		}
	}
#if ENABLE_CHB
	if (DramAttr->RankNumChB > 0) { /* 1 rank */
		for (i = 0; i < DQS_INPUT_CAPTURE_REG_NUM; i++) {
			Data = pci_read_config8(MEMCTRL,
				DDR2_ChB_DQS_Input_Capture_Tbl[i][0]);
			Data &= DDR2_ChB_DQS_Input_Capture_Tbl[i][1]; /* mask */
			Data |= DDR2_ChB_DQS_Input_Capture_Tbl[i][FreqId]; /* set val */
			pci_write_config8(MEMCTRL,
			        DDR2_ChB_DQS_Input_Capture_Tbl[i][0], Data);
		}
	}
#endif
}

/*
 * This is very important, if you don't set it correctly, DRAM will be
 * unreliable,
 *
 * Set DCLK Phase control(Reg99H[6:1]) according the DDRII in the DIMM.
 */
void DCLKPhsCtrl(DRAM_SYS_ATTR *DramAttr)
{
	u8 Data;

	Data = 0;       /* TODO: Can be dropped? */
	Data = pci_read_config8(MEMCTRL, 0x99);
	Data &= 0xE1;
	/* DDR in Dimm1, MCLKOA[4,3,0] will output MCLK */
	if (DramAttr->RankPresentMap & 0x03)
		Data |= 0x09 << 1;
	/* DDR in Dimm2, MCLKOA[5,2,1] will output MCLK */
	if (DramAttr->RankPresentMap & 0x0C)
		Data |= 0x06 << 1;

	pci_write_config8(MEMCTRL, 0x99, Data);
}
