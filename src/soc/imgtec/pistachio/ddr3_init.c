/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include <soc/ddr_init.h>
#include <soc/ddr_private_reg.h>

/*
 * Configuration for the Winbond W631GG6KB part using
 * Synopsys DDR uMCTL and DDR Phy
 */
int init_ddr3(void)
{
	uint32_t temp_rw_val;

	temp_rw_val = read32(TOPLEVEL_REGS + DDR_CTRL_OFFSET);
	/* Set CLK_EN = 1 */
	temp_rw_val |= 0x2;
	write32(TOPLEVEL_REGS + DDR_CTRL_OFFSET, temp_rw_val);
	read32(TOPLEVEL_REGS + DDR_CTRL_OFFSET);
	/*
	 * Reset the AXI bridge and DDR Controller in case any spurious
	 * writes have already happened to DDR
	 */
	/* Drive the 3 resets low */
	write32(TOPLEVEL_REGS + DDR_CTRL_OFFSET, 0x00000002);
	read32(TOPLEVEL_REGS + DDR_CTRL_OFFSET);
	read32(TOPLEVEL_REGS + DDR_CTRL_OFFSET);

	/* And release */
	write32(TOPLEVEL_REGS + DDR_CTRL_OFFSET, 0x0000000F);
	/* Dummy read to fence the access between the reset above and
	 * the DDR controller writes below
	 */
	read32(TOPLEVEL_REGS + DDR_CTRL_OFFSET);

	/* Timings for 400MHz
	 * therefore 200MHz (5ns) uMCTL (Internal) Rate
	 */
	/* TOGCNT1U: Toggle Counter 1U Register: 1us 200h C8h */
	write32(DDR_PCTL + DDR_PCTL_TOGCNT1U_OFFSET, 0x000000C8);
	/* TINIT: t_init Timing Register: at least 200us 200h C8h */
	write32(DDR_PCTL + DDR_PCTL_TINIT_OFFSET, 0x000000C8);
	/* TRSTH: Reset High Time Register DDR3 ONLY */
	write32(DDR_PCTL + DDR_PCTL_TRSTH_OFFSET, 0x000001F4);
	/* TOGCNT100N: Toggle Counter 100N Register: 20d, 14h*/
	write32(DDR_PCTL + DDR_PCTL_TOGG_CNTR_100NS_OFFSET, 0x00000014);
	/* DTUAWDT DTU Address Width Register
	 * 1:0   column_addr_width Def 10 - 7  3 10 bits
	 * 4:3   bank_addr_width   Def  3 - 2  1  3 bits (8 bank)
	 * 7:6   row_addr_width    Def 14 - 13 1  3 bits
	 * 10:9  number_ranks      Def  1 -  1 0  0 1 Rank
	 */
	write32(DDR_PCTL + DDR_PCTL_DTUAWDT_OFFSET, 0x0000004B);
	/* MCFG
	 * 0 BL 1 -> 8 fixed
	 * 1 RDRIMM 0
	 * 2 BL8 Burst Terminate 0
	 * 3 2T = 0
	 * 4 Multi Rank 0
	 * 5 DDR3 En 1
	 * 6 LPDDR S4 En
	 * 7 BST En 0, 1 for LPDDR2/3
	 * 15:8 Power down Idle, passed by argument
	 * 16 Power Down Type, passed by argument
	 * 17 Power Down Exit 0 = slow, 1 = fast, pba
	 * 19:18 tFAW 45ns = 9 clk 5*2 -1 1h
	 * 21:20 mDDR/LPDDR2 BL 0
	 * 23:22 mDDR/LPDDR2 Enable 0
	 * 31:24 mDDR/LPDDR2/3 Dynamic Clock Stop 0
	 */
	write32(DDR_PCTL + DDR_PCTL_MCFG_OFFSET, 0x00060021);
	/* MCFG1: Memory Configuration-1 Register
	 * c7:0 sr_idle Self Refresh Idle Entery 32 * nclks 14h, set 0 for BUB
	 * 10:8 Fine tune MCFG.19:18 -1
	 * 15:11 Reserved
	 * 23:16 Hardware Idle Period NA 0
	 * 30:24 Reserved
	 * 31 c_active_in_pin exit auto clk stop NA 0
	 */
	write32(DDR_PCTL + DDR_PCTL_MCFG1_OFFSET, 0x00000100);
	/* DCR DRAM Config
	 * 2:0 SDRAM => DDR3 3
	 * 3 DDR 8 Bank 1
	 * 6:4 Primary DQ DDR3 Only 0
	 * 7 Multi-Purpose Register DDR3 Only 0
	 * 9:8 DDRTYPE LPDDR2 00
	 * 26:10 Reserved
	 * 27 NOSRA No Simultaneous Rank Access 0
	 * 28 DDR 2T 0
	 * 29 UDIMM NA 0
	 * 30 RDIMM NA 0
	 * 31 TPD LPDDR2 0
	 */
	write32(DDR_PHY + DDRPHY_DCR_OFFSET, 0x0000000B);
	/* Generate to use with PHY and PCTL
	 * MR0 : DDR3 mode register 0
	 * 1:0 BL 8 fixed 00
	 * 3 BT Sequential 0 Interleaved 1 = 0
	 * 6:4,2 CL 6
	 * 7 TM Normal 0
	 * 8 DLL Reset 1 (self Clearing)
	 * 11:9 WR 15 ns 6 (010)
	 * 12 PD Slow 1 Fast 0 0
	 * 15:13 RSVD RSVD
	 * 31:16 Reserved
	 */
	write32(DDR_PHY + DDRPHY_MR_OFFSET, 0x00001520);
	/* MR1 : DDR3 mode register 1
	 * Generate to use with PHY and PCTL
	 * 0 DE DLL Enable 0 Disable 1
	 * 5,1 DIC Output Driver RZQ/6
	 * 9,6,2 ODT RZQ/4
	 * 4:3 AL = 0
	 * 7 write leveling enabled 0
	 * 10 DQS 0 diff, 1 single = 0
	 * 11 TDQS NA 0
	 * 12 QOFF Normal mode 0
	 * 15:13 RSVD
	 * 31:16 Reserved
	 */
	write32(DDR_PHY + DDRPHY_EMR_OFFSET, 0x00000004);
	/* MR2 : DDR3 mode register 2
	 * Generate to use with PHY and PCTL
	 * 2:0 PASR, NA 000
	 * 3 CWL 000 (5) tck = 22.5ns
	 * 6 auto self-refresh 1
	 * 7 SRT normal 0
	 * 8 RSVD
	 * 10:9 dynamic ODT 10 RZQ/2
	 * 31:11 Reserved
	 */
	write32(DDR_PHY + DDRPHY_EMR2_OFFSET, 0x00000440);
	/* MR3: DDR3 mode register 3
	 * 1:0 MPRLOC 00
	 * 2 MPR 0
	 */
	write32(DDR_PHY + DDRPHY_EMR3_OFFSET, 0x00000000);
	/* DTAR : Data Training Register
	 * 11:0  Data Training Column Address
	 * 27:12 Data Training Row Address
	 * 30:28 Data Training Bank Address
	 * 31    Data Training Use MPR (DDR3 Only)
	 */
	write32(DDR_PHY + DDRPHY_DTAR_OFFSET, 0x00000000);
	/* DSGCR
	 * 0 PUREN Def 1
	 * 1 BDISEN Def 1
	 * 2 ZUEN Def 1
	 * 3 LPIOPD DEf 1 0
	 * 4 LPDLLPD DEf 1 0
	 * 7:5 DQSGX DQS Extention set to 1 - advised by Synopsys
	 * 10:8 DQSGE DQS Early Gate - 1 - advised by Sysnopsys
	 * 11 NOBUB No Bubbles, adds latency 1
	 * 12 FXDLAT Fixed Read Latency 0
	 * 15:13 Reserved
	 * 19:16 CKEPDD CKE Power Down 0000
	 * 23:20 ODTPDD ODT Power Down 0000
	 * 24 NL2PD Power Down Non LPDDR2 pins 0
	 * 25 NL2OE Output Enable Non LPDDR2 pins 1
	 * 26 TPDPD LPDDR Only 0
	 * 27 TPDOE LPDDR Only 1
	 * 28 CKOE Output Enable Clk's 1
	 * 29 ODTOE Output Enable ODT 1
	 * 30 RSTOE RST# Output Enable 1
	 * 31 CKEOE CKE Output Enable 1
	 */
	write32(DDR_PHY + DDRPHY_DSGCR_OFFSET, 0xFA000927);
	/* Sysnopsys advised 500R pullup/pulldown DQS DQSN */
	write32(DDR_PHY + DDRPHY_DXCCR_OFFSET, 0x00000C40);
	/* DTPR0 : DRAM Timing Params 0
	 * 1:0 tMRD 0
	 * 4:2 tRTP 2
	 * 7:5 tWTR 4
	 * 11:8 tRP 6
	 * 15:12 tRCD 6
	 * 20:16 tRAS 15
	 * 24:21 tRRD 4 for x16
	 * 30:25 tRC 21
	 * 31 tCCD 0 BL/2 Cas to Cas
	 */
	write32(DDR_PHY + DDRPHY_DTPR0_OFFSET, 0x2A8F6688);
	/* DTPR1 : DRAM Timing Params 1
	 * 1:0 ODT On/Off Del Std 0
	 * 2 tRTW Rd2Wr Del 0 std 1 +1 0
	 * 8:3 tFAW 20 Clk
	 * 10:9 tMOD DDR3 Only 15
	 * 11 tRTODT DDR3 Only 0
	 * 15:12 Reserved
	 * 23:16 tRFC 160ns 64 ref 131
	 * 26:24 tDQSCK LPDDR2 only 1
	 * 29:27 tDQSCKmax 1
	 * 31:30 Reserved
	 */
	write32(DDR_PHY + DDRPHY_DTPR1_OFFSET, 0x094006A0);
	/* DTPR2 : DRAM Timing Params 2
	 * 9:0 tXS exit SR def 512d
	 * 14:10 tXP PD Exit Del 8 5
	 * 18:15 tCKE CKE Min pulse 5
	 * 28:19 tDLLK DLL Lock time 512d
	 * 32:29 Reserved
	 */
	write32(DDR_PHY + DDRPHY_DTPR2_OFFSET, 0x10029600);
	/* PTR0 : PHY Timing Params 0
	 * 5:0 tDLLRST Def 27
	 * 17:6 tDLLLOCK Def 2750
	 * 21:18 tITMSRST Def 8
	 * 31:22 Reserved 0
	 */
	write32(DDR_PHY + DDRPHY_PTR0_OFFSET, 0x0022AF9B);
	/* PTR1 : PHY Timing Params 1
	 * 18:0 : tDINITO DRAM Init time 500us 200,000 Dec 0x30D40
	 * 29:19 : tDINIT1 DRAM Init time tRFC + 10ns 68
	 */
	write32(DDR_PHY + DDRPHY_PTR1_OFFSET, 0x02230D40);
	/* DQS gating configuration: passive windowing mode */
	/*
	 * PGCR: PHY General cofiguration register
	 * 0 ITM DDR mode: 0
	 * 1 DQS gading configuration: passive windowing 1
	 * 2 DQS drift compensation: not supported in passive windowing 0
	 * 4:3 DQS drift limit 0
	 * 8:5 Digital test output select 0
	 * 11:9 CK Enable: one bit for each 3 CK pair: 0x7
	 * 13:12 CK Disable values: 0x2
	 * 14 CK Invert 0
	 * 15 IO loopback 0
	 * 17:16 I/O DDR mode 0
	 * 21:18 Ranks enable by training: 0xF
	 * 23:22 Impedance clock divider select 0x2
	 * 24 Power down disable 1
	 * 28:25 Refresh during training 0
	 * 29 loopback DQS shift 0
	 * 30 loopback DQS gating 0
	 * 31 loopback mode 0
	 */
	write32(DDR_PHY + DDRPHY_PGCR_OFFSET, 0x01BC2E02);

	/* PGSR : Wait for INIT/DLL/Z Done from Power on Reset */
	if (wait_for_completion(DDR_PHY + DDRPHY_PGSR_OFFSET, 0x00000007))
		return DDR_TIMEOUT;
	/* PIR : PHY controlled init */
	write32(DDR_PHY + DDRPHY_PIR_OFFSET, 0x0000001F);
	/* PGSR : Wait for DRAM Init Done */
	if (wait_for_completion(DDR_PHY + DDRPHY_PGSR_OFFSET, 0x00000007))
			return DDR_TIMEOUT;
	/* PIR : controller DRAM initialization */
	write32(DDR_PHY + DDRPHY_PIR_OFFSET, 0x00040001);
	/* PGSR : Wait for DRAM Init Done */
	if (wait_for_completion(DDR_PHY + DDRPHY_PGSR_OFFSET, 0x0000000F))
			return DDR_TIMEOUT;
	/********************************************************************/
	/* DF1STAT0 : wait for DFI_INIT_COMPLETE */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_DFISTAT0_OFFSET,
						0x00000001))
		return DDR_TIMEOUT;
	/* POWCTL : Start the memory Power Up seq*/
	write32(DDR_PCTL + DDR_PCTL_POWCTL_OFFSET, 0x80000001);
	/* POWSTAT : wait for POWER_UP_DONE */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_POWSTAT_OFFSET,
						0x00000001))
		return DDR_TIMEOUT;
	/*
	 * TREFI : t_refi Timing Register 1X
	 * 12:0 t_refi 7.8us in 100ns 0x4E
	 * 15:13 Reserved 0
	 * 18:16 num_add_ref 0
	 * 30:19 Reserved 0
	 * 31 Update 1
	 */
	write32(DDR_PCTL + DDR_PCTL_TREFI_OFFSET, 0x8000004E);
	/* TMRD : t_mrd Timing Register -- Range 2 to 4*/
	write32(DDR_PCTL + DDR_PCTL_TMRD_OFFSET, 0x00000004);
	/*
	 * TRFC : t_rfc Timing Register -- Range 15 to 131
	 * 195ns / 2.5ns 78 x4E
	 */
	write32(DDR_PCTL + DDR_PCTL_TRFC_OFFSET, 0x0000004E);
	/* TRP : t_rp Timing Register -- Range 3 to 7
	 * 4:0 tRP 12.5 / 2.5 = 5 6 For Now 6-6-6
	 * 17:16 rpea_extra DDR3 - value 0
	 */
	write32(DDR_PCTL + DDR_PCTL_TRP_OFFSET, 0x00000006);
	/* TAL : Additive Latency Register -- AL in MR1 */
	write32(DDR_PCTL + DDR_PCTL_TAL_OFFSET, 0x00000000);
	/* TCL : CAS Latency Timing Register -- CASL in MR0 6-6-6 */
	write32(DDR_PCTL + DDR_PCTL_TCL_OFFSET, 0x00000006);
	/* TCWL : CAS Write Latency Register   --CASL-1 */
	write32(DDR_PCTL + DDR_PCTL_TCWL_OFFSET, 0x00000005);
	/* TRAS : Activate to Precharge cmd time  15 45ns / 2.5ns = 18d */
	write32(DDR_PCTL + DDR_PCTL_TRAS_OFFSET, 0x0000000F);
	/* TRC : Min. ROW cycle time  21
	 * 57.5ns / 2.5ns = 23d Playing safe 24
	 */
	write32(DDR_PCTL + DDR_PCTL_TRC_OFFSET, 0x00000015);
	/* TRCD : Row to Column Delay  # Range 3 to 7 (TCL = TRCD)
	 * 12.5ns / 2.5ns = 5 but running 6-6-6  6
	 */
	write32(DDR_PCTL + DDR_PCTL_TRCD_OFFSET, 0x00000006);
	/* TRRD : Row to Row delay     -- Range 2 to 6
	 * 2K Page 10ns / 2.5ns = 4
	 */
	write32(DDR_PCTL + DDR_PCTL_TRRD_OFFSET, 0x00000004);
	/* TRTP : Read to Precharge time -- Range 2 to 4
	 * Largest 4 or 7.5ns / 2.5ns = 4
	 */
	write32(DDR_PCTL + DDR_PCTL_TRTP_OFFSET, 0x00000004);
	/* TWR : Write recovery time   -- WR in MR0: 15ns / 2.5ns = 6 */
	write32(DDR_PCTL + DDR_PCTL_TWR_OFFSET, 0x00000006);
	/* TWTR : Write to read turn around time  -- Range 2 to 4
	 * Largest 4 or 7.5ns / 2.5ns = 4
	 */
	write32(DDR_PCTL + DDR_PCTL_TWTR_OFFSET, 0x00000004);
	/* TEXSR : Exit Self Refresh to first valid cmd: tXS 512 */
	write32(DDR_PCTL + DDR_PCTL_TEXSR_OFFSET, 0x00000200);
	/* TXP : Exit Power Down to first valid cmd
	 * tXP 2, Settingto 3 to match PHY
	 */
	write32(DDR_PCTL + DDR_PCTL_TXP_OFFSET, 0x00000003);
	/* TDQS : t_dqs Timing Register
	 * DQS additional turn around Rank 2 Rank (1 Rank) Def 1
	 */
	write32(DDR_PCTL + DDR_PCTL_TDQS_OFFSET, 0x00000001);
	/* TRTW : Read to Write turn around time Def 3
	 * Actual gap t_bl + t_rtw
	 */
	write32(DDR_PCTL + DDR_PCTL_TRTW_OFFSET, 0x00000003);
	/* TCKE : CKE min pulse width DEf 3 */
	write32(DDR_PCTL + DDR_PCTL_TCKE_OFFSET, 0x00000003);
	/* TXPDLL : Slow Exit Power Down to first valid cmd delay
	 * tXARDS 10+AL = 10
	 */
	write32(DDR_PCTL + DDR_PCTL_TXPDLL_OFFSET, 0x0000000A);
	/* TCKESR : Min CKE Low width for Self refresh entry to exit
	 * t_ckesr = 0 DDR2
	 */
	write32(DDR_PCTL + DDR_PCTL_TCKESR_OFFSET, 0x00000004);
	/* TMOD : MRS to any Non-MRS command -- Range 0 to 31 */
	write32(DDR_PCTL + DDR_PCTL_TMOD_OFFSET, 0x0000000F);
	/* TZQCS : SDRAM ZQ Calibration Short Period */
	write32(DDR_PCTL + DDR_PCTL_TZQCS_OFFSET, 0x00000040);
	/* TZQCL : SDRAM ZQ Calibration Long Period */
	write32(DDR_PCTL + DDR_PCTL_TZQCL_OFFSET, 0x00000200);
	/* SCFG : State Configuration Register  (Enabling Self Refresh)
	 * 0    LP_en Leave Off for Bring Up   0
	 * 5:1  Reserved
	 * 6    Synopsys Internal Only         0
	 * 7    Enale PHY indication of LP Opportunity 1
	 * 11:8 bbflags_timing max UPCTL_TCU_SED_P - tRP (16 - 6) Use 4
	 * 16:12 Additional delay on accertion of ac_pdd 4
	 * 31:17 Reserved
	 */
	write32(DDR_PCTL + DDR_PCTL_SCFG_OFFSET, 0x00004480);
	/* TREFI_MEM_DDR3 */
	write32(DDR_PCTL + DDR_PCTL_TREFI_MEM_DDR3_OFFSET, 0x00000C30);

	/* DFITPHYWRLAT : Write cmd to dfi_wrdata_en */
	write32(DDR_PCTL + DDR_PCTL_DFIWRLAT_OFFSET, 0x00000002);
	/* DFITRDDATAEN : Read cmd to dfi_rddata_en */
	write32(DDR_PCTL + DDR_PCTL_DFITRDDATAEN_OFFSET, 0x00000002);
	/*
	 * DFITPHYWRDATA : dfi_wrdata_en to drive wr data
	 * DFI Clks wrdata_en to wrdata Def 1
	 */
	write32(DDR_PCTL + DDR_PCTL_DFITPHYWRDATA_OFFSET, 0x00000000);
	/*
	 * DFITPHYRDLAT : dfi_rddata_en to dfi_rddata_valid
	 * DFI clks max rddata_en to rddata_valid Def 15
	 */
	write32(DDR_PCTL + DDR_PCTL_DFITPHYRDLAT_OFFSET, 0x00000008);
	/* DFISTCFG0 : Drive various DFI signals appropriately
	 * 0 dfi_init_start            1
	 * 1 dfi_freq_ratio_en         1
	 * 2 dfi_data_byte_disable_en  1
	 */
	write32(DDR_PCTL + DDR_PCTL_DFISTCFG0_OFFSET, 0x00000007);
	/* DFISTCFG1 : Enable various DFI support
	 * 0 dfi_dram_clk_disable_en 1
	 * 1 dfi_dram_clk_disable_en_pdp only lPDDR 0
	 */
	write32(DDR_PCTL + DDR_PCTL_DFISTCFG1_OFFSET, 0x00000001);
	/* DFISTCFG2 : Enable Parity and asoc interrupt
	 * 0 dfi_parity_in Enable 1
	 * 1 Interrupt on dfi_parity_error 1
	 */
	write32(DDR_PCTL + DDR_PCTL_DFISTCFG2_OFFSET, 0x00000003);
	/* DFILPCFG0 : DFI Low Power Interface Configuration
	 * 0     Enable DFI LP IF during PD 1
	 * 3:1   Reserved
	 * 7:4   DFI tlp_wakeup time        0
	 * 8     Enable DFI LP IF during SR 1
	 * 11:9  Reserved
	 * 15:12 dfi_lp_wakeup in SR        0
	 * 19:16 tlp_resp DFI 2.1 recomend  7
	 * 23:20 Reserved
	 * 24    Enable DFI LP in Deep Power Down 0
	 * 27:25 Reserved
	 * 31:28 DFI LP Deep Power Down Value 0
	 */
	write32(DDR_PCTL + DDR_PCTL_DFILPCFG0_OFFSET, 0x00070101);
	/* DFIODTCFG : DFI ODT Configuration
	 * Only Enabled on Rank0 Writes
	 * 0 rank0_odt_read_nsel  0
	 * 1 rank0_odt_read_sel   0
	 * 2 rank0_odt_write_nsel 0
	 * 3 rank0_odt_write_sel  1
	 */
	write32(DDR_PCTL + DDR_PCTL_DFIODTCFG_OFFSET, 0x00000008);
	/* DFIODTCFG1 : DFI ODT Configuration
	 * 4:0   odt_lat_w     0
	 * 12:8  odt_lat_r     0 Def
	 * 4:0   odt_len_bl8_w 6 Def
	 * 12:8  odt_len_bl8_r 6 Def
	 */
	write32(DDR_PCTL + DDR_PCTL_DFIODTCFG1_OFFSET, 0x060600000);

	/* Memory initialization */
	/* MCMD : PREA, Addr 0 Bank 0 Rank 0 Del 0
	 * 3:0 cmd_opcode PREA 00001
	 * 16:4 cmd_addr 0
	 * 19:17 bank_addr 0
	 * 23:20 rank_sel 0 0001
	 * 27:24 cmddelay 0
	 * 30:24 Reserved
	 */
	/* MCMD: MR2 */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80004403);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00004403))
		return DDR_TIMEOUT;
	/* MCMD: MR3 */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80000003);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00000003))
		return DDR_TIMEOUT;
	/* MCMD: MR1 */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80000043);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00000043))
		return DDR_TIMEOUT;
	/* MCMD: MR0 */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80015203);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00015203))
		return DDR_TIMEOUT;
	/* MCMD: ZQS cmd, long 5 short 4 */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80104005);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00104005))
		return DDR_TIMEOUT;
	/* MCMD: deselect command */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x80100000);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x00100000))
		return DDR_TIMEOUT;
	/* MCMD: deselect command */
	write32(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x8010000A);
	/* MRS cmd wait for completion */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_MCMD_OFFSET, 0x0010000A))
		return DDR_TIMEOUT;

	/* DCFG : DRAM Density 256 Mb 16 Bit IO Width
	 * 1:0  Devicw Width 1 x8, 2 x16, 3 x32  2
	 * 5:2  Density 2Gb = 5
	 * 6    Dram Type (MDDR/LPDDR2) Only  0
	 * 7    Reserved 0
	 * 10:8 Address Map R/B/C = 1
	 * 31:11 Reserved
	 */
	write32(DDR_PCTL + DDR_PCTL_DCFG_OFFSET, 0x00000116);
	/* PCFG_0 : Port 0 AXI config  */
	write32(DDR_PCTL + DDR_PCTL_PCFG0_OFFSET, 0x000800A0);
	/* SCTL : UPCTL switch INIT CONFIG State */
	write32(DDR_PCTL + DDR_PCTL_SCTL_OFFSET, 0x00000001);
	/* STAT : Wait for Switch INIT to Config State */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_STAT_OFFSET, 0x00000001))
		return DDR_TIMEOUT;
	/* STAT : Wait for Switch INIT to Config State */
	write32(DDR_PCTL + DDR_PCTL_CMDTSTATEN_OFFSET, 0x00000001);
	/* STAT : Wait for Switch INIT to Config State */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_CMDTSTAT_OFFSET,
							0x00000001))
		return DDR_TIMEOUT;
	/* Use PHY for DRAM init */
	write32(DDR_PHY + DDRPHY_PIR_OFFSET, 0x00000181);
	/* STAT : Wait for Switch INIT to Config State */
	if (wait_for_completion(DDR_PHY + DDRPHY_PGSR_OFFSET, 0x00000001F))
		return DDR_TIMEOUT;
	/* Disable Impedance Calibration */
	write32(DDR_PHY + DDRPHY_ZQ0CR0_OFFSET, 0x3000014A);
	write32(DDR_PHY + DDRPHY_ZQ1CR0_OFFSET, 0x3000014A);

	/* SCTL : UPCTL switch Config to ACCESS State */
	write32(DDR_PCTL + DDR_PCTL_SCTL_OFFSET, 0x00000002);
	/* STAT : Wait for switch CFG -> GO State */
	if (wait_for_completion(DDR_PCTL + DDR_PCTL_STAT_OFFSET, 0x3))
		return DDR_TIMEOUT;

	return 0;
}
