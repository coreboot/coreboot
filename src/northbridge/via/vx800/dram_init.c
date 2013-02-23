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

CB_STATUS DDR2_DRAM_INIT(void)
{
	u8 i;
	u32 RamSize;
	DRAM_SYS_ATTR DramAttr;

	PRINT_DEBUG_MEM("DRAM_INIT \r");

	memset(&DramAttr, 0, sizeof(DRAM_SYS_ATTR));
	/*Step1 DRAM Detection; DDR1 or DDR2; Get SPD Data; Rank Presence;64 or 128bit; Unbuffered or registered; 1T or 2T */
	DRAMDetect(&DramAttr);

	//Step2 set Frequency; calculate CL and Frequncy from SPD data; set the Frequency
	DRAMFreqSetting(&DramAttr);
	//Step3 Set DRAM Timing; CL, tRP, tRCD, tRAS, tRFC, tRRD, tWR, tWTR, tRTP
	DRAMTimingSetting(&DramAttr);
	//Step4 DRDY
	DRAMDRDYSetting(&DramAttr);
	//Step5 Burst length
	DRAMBurstLength(&DramAttr);
	//Step6 DRAM Driving Adjustment
	DRAMDriving(&DramAttr);
	//Step7 duty cycle control
	DutyCycleCtrl(&DramAttr);
	//Step8 DRAM clock phase and delay control
	DRAMClkCtrl(&DramAttr);
	//Step9 set register before init DRAM device
	DRAMRegInitValue(&DramAttr);
	//Step10 DDR and DDR2 initialize process
	DRAMInitializeProc(&DramAttr);

	//Step13 Interleave function in rankmap.c
	DRAMBankInterleave(&DramAttr);
	//Step14 Sizing
	DRAMSizingMATypeM(&DramAttr);

	//Step11 Search DQS and DQ output delay
	DRAMDQSOutputSearch(&DramAttr);
	//Step12 Search DQS  input delay
	DRAMDQSInputSearch(&DramAttr);

	//Step15 DDR fresh counter setting
	DRAMRefreshCounter(&DramAttr);
	//Step16 Final register setting for improve performance
	DRAMRegFinalValue(&DramAttr);

	RamSize = 0;
	for (i = 0; i < MAX_RANKS; i++) {
		if (DramAttr.RankSize[i] == 0) {
			continue;
		}
		RamSize += DramAttr.RankSize[i];
	}
	PRINT_DEBUG_MEM("RamSize=");
	PRINT_DEBUG_MEM_HEX32(RamSize);
	PRINT_DEBUG_MEM("\r");
	DumpRegisters(0, 3);
	//BOOLEAN bTest = DramBaseTest( M1, RamSize - M1 * 2,SPARE, FALSE);
	/* the memory can not correct work, this is because the user set the incorrect memory
	   parameter from setup interface.so we must set the boot mode to recovery mode, let
	   the system to reset and use the spd value to initialize the memory */
	SetUMARam();
	return CB_SUCCESS;
}
