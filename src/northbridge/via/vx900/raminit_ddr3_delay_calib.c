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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <stdint.h>
#include "raminit.h"
#include <stdint.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <string.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <console/console.h>

#include "early_vx900.h"
#include "raminit.h"


typedef uint8_t BOOLEAN;
#define TRUE 	1
#define FALSE 	0

/*===================================================================
 * Function   : via_write_phys()
 * Precondition :
 * Input      :  addr
 *                  value
 * Output     : void
 * Purpose    :
 * Reference  : None
 * ===================================================================*/

static void via_write_phys(volatile uint32_t addr, volatile uint32_t value)
{
	volatile uint32_t *ptr;
	ptr = (volatile uint32_t *)addr;
	*ptr = (volatile uint32_t)value;
}

/*===================================================================
 * Function   : via_read_phys()
 * Precondition :
 * Input      :  addr
 * Output     : u32
 * Purpose    :
 * Reference  : None
 * ===================================================================*/

static uint32_t via_read_phys(volatile u32 addr)
{
	volatile uint32_t y;
	y = *(volatile uint32_t *)addr;
	return y;
}

/*===================================================================
 * Function   : DimmRead()
 * Precondition :
 * Input      :  x
 * Output     : u32
 * Purpose    :
 * Reference  : None
 * ===================================================================*/
/*
 * static uint32_t DimmRead(volatile uint32_t x)
 * {
 *	volatile uint32_t y;
 *	y = *(volatile uint32_t *)x;
 * 
 *	return y;
 }
 */
/*===================================================================
 * Function   : dram_base_test()
 * Precondition : this function used to verify memory
 * Input      :
 *                 BaseAdd,
 *                 length,
 *                 mode
 * Output     : u32
 * Purpose    :write into and read out to verify if dram is correct
 * Reference  : None
 * ===================================================================*/
#define   STEPSPAN        0x1000            //the span when test memory in spare mode
#define   TESTCOUNT       0x4                // the test count in each range when test memory in spare mode
#define   TEST_PATTERN    0x5A5A5A5A             //the test pattern

typedef  enum   __DRAM_TEST_MODE{
	EXTENSIVE,
	SPARE,
	MAXMODE
} DRAM_TEST_MODE;

static BOOLEAN dram_base_test(uint32_t base_addr, uint32_t len,
			      DRAM_TEST_MODE mode, uint8_t print_flag)
{
	uint32_t test_span;
	uint32_t d32, addr, addr2;
	uint8_t i, test_count;
	
	//decide the test mode is continous or step
	if (mode == EXTENSIVE) {
		//the test mode is continuos and must test each unit
		test_span = 4;
		test_count = 1;
	} else if (mode == SPARE) {
		// the test mode is step and test some unit
		test_span = STEPSPAN;
		test_count = TESTCOUNT;
	} else {
		printram("the test mode is error\n");
		return FALSE;
	}
	
	//write each test unit the value with TEST_PATTERN
	for (addr = base_addr; addr < base_addr + len; addr += test_span) {
		for (i = 0; i < test_count; i++)
			via_write_phys(addr + i * 4, TEST_PATTERN);
		if (print_flag) {
			if ((u32) addr % 0x10000000 == 0) {
				printram("Write in Addr = 0x%.8x", addr);
				printram("\n");
			}
		}
	}
	
	//compare each test unit with the value of TEST_PATTERN
	//and write it with compliment of TEST_PATTERN
	for (addr = base_addr; addr < base_addr + len; addr += test_span) {
		for (i = 0; i < test_count; i++) {
			d32 = via_read_phys(addr + i * 4);
			via_write_phys(addr + i * 4, (uint32_t) (~TEST_PATTERN));
			if (d32 != TEST_PATTERN) {
				addr2 = addr + i * 4;
				printram("TEST_PATTERN ERROR !!!!! 0x%.8x", addr2);
				printram(" : 0x%.8x", d32);
				printram(" \n");
				return FALSE;
			}
		}
		if (print_flag) {
			if ((u32) addr % 0x10000000 == 0) {
				printram("Write in Addr =");
				print_debug_hex32(addr);
				printram("\n");
			}
		}
	}
	
	//compare each test unit with the value of ~TEST_PATTERN
	for (addr = base_addr; addr < base_addr + len; addr += test_span) {
		for (i = (u8) (test_count); i > 0; i--) {
			d32 = via_read_phys(addr + (i - 1) * 4);
			if (d32 != ~TEST_PATTERN) {
				
				print_debug("~TEST_PATTERN ERROR !!!!! ");
				addr2 = addr + (i - 1) * 4;
				print_debug_hex32(addr2);
				printram(" : ");
				print_debug_hex32(d32);
				printram(" \n");
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

/*===================================================================
 * Function   : DumpRegisters()
 * Precondition :
 * Input      :
 *                pPCIPPI,
 *                DevNum,
 *                FuncNum
 * Output     : Void
 * Purpose    :
 * Reference  : None
 * ===================================================================*/
#if 0
static void dump_registers(uint8_t dev_num, uint8_t func_num)
{
	uint8_t i, j;
	uint8_t d8;
	
	d8 = 0;
	printk(BIOS_DEBUG, "Dev %01x Fun %01x:", dev_num, func_num);
	printram
	("\n     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
	printram
	("---------------------------------------------------\n");
	for (i = 0; i < 0x10; i++)
	{
		print_debug_hex8(i);
		for (j = 0; j < 0x10; j++)
		{
			d8 =pci_read_config8(PCI_DEV(0, dev_num, func_num), i * 0x10 + j);
			printram(" ");
			print_debug_hex8(d8);
		}
		printram("\n");
	}
	return;
}

#endif
static uint8_t shift_to_mask_bit(uint8_t MaskBit, uint8_t ShiftData)
{
	uint8_t value, index, buffer=0;
	
	for (index=0; index<8; index++)
	{
		buffer = MaskBit;
		if ((buffer >> index) & 0x01)
			break;
	}
	value = ShiftData << index;
	return value;
}

//----------------------------------------------------------------------------
//Ver	Date			Name	Note
//----------------------------------------------------------------------------
//R21
// 1. 3410-21-LNA-06	Add define to support RDSIT calibration limitation.
//R32
// 1. 3410-32-LNA-01    Add different raw cards support.
//R34
//3410-34-LNA-03    Add raw card C support. If user populates raw card C DIMM, set the raw card type to raw card A.
//R36
//3410-36-LNA-01    When detect Mezza Dimm, bios need to modify DRAM TRCD.
/***************************************************************************
 * Copyright (C) 2006 VIA Technologies, Inc. All Rights Reserved.
 * 
 * Information in this file is the intellectual property of
 * VIA Technologies, Inc., and may contains trade secrets that must be stored
 * and viewed confidentially.
 * 
 * 
 * Filename :      DramInit.h
 * Version    :     0.1
 * Date       :      20050401
 * Author     :   Bruce Cao
 * 
 * Description :  The header file of the DramInit.c
 *                     Noted by Jason Zhao in 20060627.
 * 
 * Others :  None
 ****************************************************************************/
#ifndef __DRAMINIT_H_
#define __DRAMINIT_H_



//#include "../vx900.h"
//#include "../vx900_cfg.h"


#define TRUE 	1
#define FALSE 	0


static void pci_modify_config8(device_t dev, unsigned int where,
		     u8 clr_mask, u8 set_mask)
{
	u8 reg8 = pci_read_config8(dev, where);
	reg8 &= ~clr_mask;
	reg8 |= set_mask;
	pci_write_config8(dev, where, reg8);
}


//==================================================================
// DDR3 Code Start{
	//==================================================================


#define SPD_MEMORY_TYPE              2   ;/*Memory type FPM,EDO,SDRAM,DDR,DDR2, DDR3*/
#define SPD_MODULE_TYPE              3
#define SPD_DENSITY_BANKS            4
#define SPD_ADDRESSING               5
#define SPD_NOMINAL_VOLTAGE          6
#define SPD_MODULE_ORG               7
#define SPD_BUS_WIDTH                8
#define SPD_FINE_TIMEBASE            9
#define SPD_MEDIUM_TIMEBASE_DIVIDEND 10
#define SPD_MEDIUM_TIMEBASE_DIVISOR  11
#define SPD_MIN_CYCLE_TIME           12
#define SPD_SUPPORTED_CL_LSB         14
#define SPD_SUPPORTED_CL_MSB         15
#define SPD_MIN_CL_TIME              16
#define SPD_MIN_TWR                  17
#define SPD_MIN_TRCD                 18
#define SPD_MIN_TRRD                 19
#define SPD_MIN_TRP	             		 20
#define SPD_UPPER_NIBBLE_TRAS_TRC    21
#define SPD_MIN_TRAS_LSB             22
#define SPD_MIN_TRC_LSB              23
#define SPD_MIN_TRFC_LSB             24
#define SPD_MIN_TRFC_MSB             25
#define SPD_MIN_TWTR                 26
#define SPD_MIN_TRTP                 27
#define SPD_UPPER_NIBBLE_TFAW        28
#define SPD_MIN_TFAW                 29
#define SPD_OPTIONAL_FEATURE         30
#define SPD_THERMAL_REFRESH          31
#define SPD_THERMAL_SENSOR           32
#define SPD_DEVICE_TYPE              33
#define SPD_RANK1_ADDRMAPPING        63

// According to SPD byte 7 to define.
#define	RAW_CARD_A	 0x0A // 2R x16
#define	RAW_CARD_B	 0x01 // 1R x8
#define RAW_CARD_C  0x02 // 1R x16
#define	RAW_CARD_F	 0x09 // 2R x8
//3410-32-LNA-01-start
#define RAW_CARD_O  0x00 // dram controller can not support x4 DRAM chiop, so use x4 DRAM chip definition to define no dram
//card type combination of DIMM1:DIMM0
#define RAW_CARD_AO ((RAW_CARD_A << 4) | RAW_CARD_O)
#define RAW_CARD_AA ((RAW_CARD_A << 4 )| RAW_CARD_A)
#define RAW_CARD_AB ((RAW_CARD_A << 4 )| RAW_CARD_B)
#define RAW_CARD_AF ((RAW_CARD_A << 4 )| RAW_CARD_F)
#define RAW_CARD_BO ((RAW_CARD_B << 4 )| RAW_CARD_O)
#define RAW_CARD_BA ((RAW_CARD_B << 4 )| RAW_CARD_A)
#define RAW_CARD_BB ((RAW_CARD_B << 4 )| RAW_CARD_B)
#define RAW_CARD_BF ((RAW_CARD_B << 4 )| RAW_CARD_F)
#define RAW_CARD_FO ((RAW_CARD_F << 4 )| RAW_CARD_O)
#define RAW_CARD_FA ((RAW_CARD_F << 4 )| RAW_CARD_A)
#define RAW_CARD_FB ((RAW_CARD_F << 4 )| RAW_CARD_B)
#define RAW_CARD_FF ((RAW_CARD_F << 4 )| RAW_CARD_F)
#define RAW_CARD_OO ((RAW_CARD_O << 4 )| RAW_CARD_O)
#define RAW_CARD_OA ((RAW_CARD_O << 4 )| RAW_CARD_A)
#define RAW_CARD_OB ((RAW_CARD_O << 4 )| RAW_CARD_B)
#define RAW_CARD_OF ((RAW_CARD_O << 4 )| RAW_CARD_F)


#define DRAMFREQ_533 		0x04
#define DRAMFREQ_667 		0x05
#define DRAMFREQ_800 		0x06
#define DRAMFREQ_1066 		0x07
#define DRAMFREQ_1333 		0x08

#define DRAMCYCLE_800 		2500
#define DRAMCYCLE_1066 		1875
#define DRAMCYCLE_1333 	  1500
#define DRAMCYCLE_1600 	  1250

#define CMD_2T 				0x00
#define CMD_1T 				0x01

#define BL4 				0x00
#define BL8 				0x01


#define MAA_BITMAP		0x0F
#define MAB_BITMAP		0xF0
#define CHA_BITMAP		MAA_BITMAP
#define CHB_BITMAP		MAB_BITMAP
#define ALL_BITMAP		CHA_BITMAP + CHB_BITMAP
#define ALL_EVEN		((ALL_BITMAP) & 0x55)
#define CHA_EVEN_BANK	( CHA_BITMAP & 0x55 )
#define CHB_EVEN_BANK	( CHB_BITMAP & 0x55 )


#define NOTREGDIMM 			0x00
#define ISREGDIMM 			0x01

#define RANK1_MAPPING_STANDARD 	0
#define RANK1_MAPPING_MIRRORED 	1

//---------------------------------------//
// 		MSR setting 					 //
//---------------------------------------//


//Typical MA mapping for a MRS command
//BA2 BA1 BA0	MA15 MA14 MA13 MA12 MA11 MA10 MA09 MA08 MA07 MA06 MA05 MA04 MA03 MA02 MA01 MA00
//19  18  17	x    x    x    14   13   20   12   11   10   9    8    7    6    5    4    3

// DDR3 MR0 (BA1=0, BA0=0)
// MA[12]		- DLL Control for Precharge PD
// MA[11:9]		- TWR
// MA[8]		- DLL Reset
// MA[7]		- Mode
// MA[3]		- Read Burst Type
// MA[6:4,2]    - CAS LAtency
// MA[1:0]  	- BL
//						BA1          BA0
#define DDR3_MR0		(0 << 18) + (0 << 17)

//							MA12
#define MR0_DDR3_DLLCONTROL	(1 << 14)

//							MA11	     MA10         MA9
#define MR0_DDR3_WR5		(0 << 13) + (0 << 20) + (1 << 12)
#define MR0_DDR3_WR6		(0 << 13) + (1 << 20) + (0 << 12)
#define MR0_DDR3_WR7		(0 << 13) + (1 << 20) + (1 << 12)
#define MR0_DDR3_WR8		(1 << 13) + (0 << 20) + (0 << 12)
#define MR0_DDR3_WR10		(1 << 13) + (0 << 20) + (1 << 12)
#define MR0_DDR3_WR12		(1 << 13) + (1 << 20) + (0 << 12)

//							MA8
#define MR0_DLL_RESET		(1 << 11)

//							MA7
#define MR0_TEST_MODE		(1 << 10)

//							MA6	     MA5         MA4        MA2
#define MR0_DDR3_CL5		(0 << 9) + (0 << 8) + (1 << 7) + (0 << 5)
#define MR0_DDR3_CL6		(0 << 9) + (1 << 8) + (0 << 7) + (0 << 5)
#define MR0_DDR3_CL7		(0 << 9) + (1 << 8) + (1 << 7) + (0 << 5)
#define MR0_DDR3_CL8		(1 << 9) + (0 << 8) + (0 << 7) + (0 << 5)
#define MR0_DDR3_CL9		(1 << 9) + (0 << 8) + (1 << 7) + (0 << 5)
#define MR0_DDR3_CL10		(1 << 9) + (1 << 8) + (0 << 7) + (0 << 5)
#define MR0_DDR3_CL11		(1 << 9) + (1 << 8) + (1 << 7) + (0 << 5)

//									MA3
#define MR0_DDR3_READ_BURST_INVL	(1 << 6)


//								MA1        MA0
#define MR0_DDR3_BL8			(0 << 4) + (0 << 3)
#define MR0_DDR3_ON_THE_FLY		(0 << 4) + (1 << 3)
#define MR0_DDR3_BL4			(1 << 4) + (0 << 3)


//Typical MA mapping for a MRS command
//BA2 BA1 BA0	MA15 MA14 MA13 MA12 MA11 MA10 MA09 MA08 MA07 MA06 MA05 MA04 MA03 MA02 MA01 MA00
//19  18  17	x    x    x    14   13   20   12   11   10   9    8    7    6    5    4    3

// DDR3 MR1 (BA1=0, BA0=1)
// MA[12]	- Qoff, output buffer enable/disable
// MA[11]	- TDQS Enable
// MA[7]		- Write Leveling enable
// MA[4:3]	- Additive Latency
// MA[9,6,2]	- Rtt_Nom
// MA[5,1]    	- Output Driver Impedance Control
// MA[0]  	- DLL Enable

//						BA1          BA0
#define DDR3_MR1		(0 << 18) + (1 << 17)

//							MA12
#define MR1_DDR3_QOFF_DIS	(1 << 14)

//							MA11
#define MR1_DDR3_TDQS_EN	(1 << 13)

//							MA7
#define MR1_DDR3_WLVL_EN	(1 << 10)

//							MA4           MA3
#define MR1_DDR3_AL_DIS		(0 << 7) + (0 << 6)
#define MR1_DDR3_AL_CL1		(0 << 7) + (1 << 6)
#define MR1_DDR3_AL_CL2		(1 << 7) + (0 << 6)


//							MA9          MA6           MA2
#define MR1_DDR3_RTTNOM_DIS	(0 << 12) + (0 << 9) + (0 << 5)
#define MR1_DDR3_RTTNOM_60OHM	(0 << 12) + (0 << 9) + (1 << 5)
#define MR1_DDR3_RTTNOM_120OHM	(0 << 12) + (1 << 9) + (0 << 5)
#define MR1_DDR3_RTTNOM_40OHM	(0 << 12) + (1 << 9) + (1 << 5)
#define MR1_DDR3_RTTNOM_20OHM	(1 << 12) + (0 << 9) + (0 << 5)
#define MR1_DDR3_RTTNOM_30OHM	(1 << 12) + (0 << 9) + (1 << 5)

//							MA5          MA1
#define MR1_DDR3_RZQ6		(0 << 8) + (0 << 4)
#define MR1_DDR3_RZQ7		(0 << 8) + (1 << 4)

//							MA0
#define MR1_DDR3_DLL_EN		(0 << 3)
#define MR1_DDR3_DLL_DIS	(1 << 3)
//Typical MA mapping for a MRS command
//BA2 BA1 BA0	MA15 MA14 MA13 MA12 MA11 MA10 MA09 MA08 MA07 MA06 MA05 MA04 MA03 MA02 MA01 MA00
//19  18  17	x    x    x    14   13   20   12   11   10   9    8    7    6    5    4    3

// DDR3 MR2 (BA1=1, BA0=0)
// MA[10:9]	- Rtt_WR
// MA[7]    	- Self-Refresh Temprerature (SRT) Range
// MA[6]    	- Auto Self-Refresh (ASR)
// MA[5:3]	- CAS Write Latency (CWL)
// MA[2:0]  	- Partial Array Self-Refresh (Optional)

//						BA1          BA0
#define DDR3_MR2		(1 << 18) + (0 << 17)

//							MA10          MA9
#define MR2_DDR3_RTTWR_DIS	(0 << 20) + (0 << 12)
#define MR2_DDR3_RTTWR_60OHM	(0 << 20) + (1 << 12)
#define MR2_DDR3_RTTWR_120OHM	(1 << 20) + (0 << 12)

//								MA7
#define MR2_DDR3_SRT_EXTENDED	(1 << 10)

//							MA6
#define MR2_DDR3_ASR_EN		(1 << 9)

//							MA5         MA4         MA3
#define MR2_DDR3_CWL_5		(0 << 8) + (0 << 7) + (0 << 6)
#define MR2_DDR3_CWL_6		(0 << 8) + (0 << 7) + (1 << 6)
#define MR2_DDR3_CWL_7		(0 << 8) + (1 << 7) + (0 << 6)
#define MR2_DDR3_CWL_8		(0 << 8) + (1 << 7) + (1 << 6)

//								MA2         MA1         MA0
#define MR2_DDR3_PASR_FULL		(0 << 5) + (0 << 4) + (0 << 3)
#define MR2_DDR3_PASR_HALFL		(0 << 5) + (0 << 4) + (1 << 3)
#define MR2_DDR3_PASR_QUATERL	(0 << 5) + (1 << 4) + (0 << 3)
#define MR2_DDR3_PASR_1_8_L		(0 << 5) + (1 << 4) + (1 << 3)
#define MR2_DDR3_PASR_3_4		(1 << 5) + (0 << 4) + (0 << 3)
#define MR2_DDR3_PASR_HALFH		(1 << 5) + (0 << 4) + (1 << 3)
#define MR2_DDR3_PASR_QUATEH	(1 << 5) + (1 << 4) + (0 << 3)
#define MR2_DDR3_PASR_1_8_H		(1 << 5) + (1 << 4) + (1 << 3)


//Typical MA mapping for a MRS command
//BA2 BA1 BA0	MA15 MA14 MA13 MA12 MA11 MA10 MA09 MA08 MA07 MA06 MA05 MA04 MA03 MA02 MA01 MA00
//19  18  17	x    x    x    14   13   20   12   11   10   9    8    7    6    5    4    3

// DDR3 MR3 (BA1=1, BA0=1)
// MA[2]    	- MPR
// MA[1:0]  	- MPR location

//						BA1          BA0
#define DDR3_MR3		(1 << 18) + (1 << 17)

//							MA2
#define MR3_DDR3_MPR_EN		(1 << 5)

//Action Type for MR0~3
#define MR0_INIT			0x00

#define MR1_INIT			0x00
#define MR1_WLVL_UNDERCALPR	0x01
#define MR1_WLVL_OTHERPR	0x02

#define MR2_INIT			0x00
#define MR2_RTTWR_DIS		0x01

#define MR3_INIT			0x00
#define MR3_MPR_ON			0x01




#define CAL_FIRST_DIMM  0
#define CAL_SECOND_DIMM 1

#define MODULE_TYPE_UDIMM 0x02
#define MODULE_TYPE_SODIMM 0x03

#define	IO_RDSIT 	 0
#define	IO_DQSI 	 1
#define	IO_DQO 		 2
#define	IO_DQSO 	 3

#define IOCTL_MANUAL 0
#define IOCTL_LOW    1
#define IOCTL_HIGH   2
#define IOCTL_CENTER 3



typedef struct{
	uint8_t	SI_Rdsit_Ref_Freq_1333;
	uint8_t SI_Rdsit_Ref_Freq_1066;
	uint8_t	SI_Rdsit_Ref_Freq_0800;
}VIA_RDSIT_REF_DRAM;




#define CHIP_MAX_SOCKETS 2
#define CHIP_MAX_RANKS CHIP_MAX_SOCKETS*2
#define CHIP_MAX_CHANNELS 1
#define CHANNEL_MAX_SOCKETS	2
#define CHANNEL_MAX_RANKS   CHANNEL_MAX_SOCKETS*2
#define  SPD_SIZE 128

typedef struct {
	uint8_t                                 SpdPresent;
	uint8_t                                   Buffer[SPD_SIZE];
} VIA_MEMINIT_SPD_DATA;

typedef struct {
	uint8_t	PR_CurrentSize;
	uint8_t	PR_VRank;
	uint8_t	PR_MergeRank;
	uint8_t	PR_MergeCount;
	uint8_t	PR_Dual_En;
	uint8_t	PR_Above4G;
	uint8_t	PR_DRAMWaste;
} VIA_VIRTUALRANK_INFO;


//;PR_Above4G, PR_DRAMWaste	Action
//;0		, 0			this VR in Below 4G
//;0		, 1			this VR in Below 4G and waste
//;1		, 0			this VR in Above 4G
//;1		, 1			this VR in Above 4G and part or all Address >= 16G

typedef struct {
	uint8_t	TotalSize_GreatThen2G;
	uint8_t	AllVR_LessThen2G;
	uint8_t	AllVR_GreatThen2G;
	uint8_t	TotalSizeOfPRsThatLessThen2G;
	uint8_t	PR_Size_Table;
	uint8_t	Virtual_Rank_Count ;
	uint16_t	Total_DRAM_Size;
} VIA_ABOVE4G_INFO;

typedef struct{
	//Tx DQ
	uint8_t TxDq_Lower_Bound;
	uint8_t TxDq_Center;
	uint8_t TxDq_Upper_Bound;
	//Tx DQS
	uint8_t TxDqs_Lower_Bound;
	uint8_t TxDqs_Center;
	uint8_t TxDqs_Upper_Bound;
}VIA_TX_IO_CALIBRATION_INFO;

typedef struct {
	uint8_t                                 BA_Number;
	uint8_t                                 RA_Number;
	uint8_t                                 CA_Number;
	uint8_t    			                	RankSize;	//Unit:64MHz
} VIA_RANK_INFO;

typedef struct {
	uint8_t                                 DQSI_LowBound;
	uint8_t                                 DQSI_HighBound;
} VIA_CHANNEL_INFO;


//========================================
// define function prototype
//========================================

//void dram_init(NB_CONFIGURATION *nb_cfg, DRAM_CONFIGURATION *dram_cfg);

//void dram_s3_init(NB_CONFIGURATION *nb_cfg, DRAM_CONFIGURATION *dram_cfg);

//========================================
// define macro
//========================================

//==================================================================
// DDR3 Code End}
//==================================================================

#endif //__DRAMINIT_H_


/*
 * 
 * VISA 0.5 DRAM Module
 * 
 */
//----------------------------------------------------------------------------
//Ver	Date			Name	Note
//----------------------------------------------------------------------------



//#include "../via_io_lib.h"

//#include "../vx900.h"
//#include "../vx900_cfg.h"
//#include "../stall.h"

//#include "si_dram_tbl.h"

//#include "dram_init.h"
//#include "dram_util.h"

#define D0F3VR0EA       0x40
#define D0F3VR1EA       0x41
#define D0F3VR2EA       0x42
#define D0F3VR3EA       0x43
#define D0F3VR0BA       0x48
#define D0F3VR1BA       0x49
#define D0F3VR2BA       0x4A
#define D0F3VR3BA       0x4B

#define D0F3MA          0x50
#define D0F3INLVSEL     0x52
#define D0F3INLVSEL_1   0x53
#define D0F3PVRMAP      0x54
#define D0F3PVRMAP_1    0x55
#define D0F3PVRMAP_2    0x56
#define D0F3PVRMAP_3    0x57

#define D0F3DPTA        0x60
#define D0F3DPCTL       0x61
#define D0F3REOPMCTL    0x63
#define D0F3REOPMCTL_1  0x64
#define D0F3DARBTIM     0x65
#define D0F3DQUARB      0x66
#define D0F3DIMCHSEL    0x67
#define D0F3PGCTIM      0x68
#define D0F3BAINLVCTL   0x69
#define D0F3RQREOCTL    0x6a
#define D0F3DINITCTL    0x6b
#define D0F3DTYPE       0x6c
#define D0F3INORSERC    0x6c
#define D0F3DATBURCTL   0x6e

#define D0F3IOTIMSEL	0x70
#define D0F3DITIMCTL    0x71
#define D0F3PGSCLSAL    0x73
#define D0F3RDPHCTL     0x74
#define D0F3DOTIMCTL    0x75
#define D0F3WDPHCTL     0x76
#define D0F3DQSOCTL     0x77
#define D0F3DQG0IOTC	0x78

#define D0F3SEGFSHAD    0x83
#define D0F3SMAPICDC    0x86
#define D0F3WMGCTL      0x88
#define D0F3DTSTMDC     0x8c
#define D0F3ADRIOCAL    0x8c
#define D0F3ADRIOCAL_1  0x8d
#define D0F3DQPIOCAL	0x8e

#define D0F3DCLKOPMD    0x90
#define D0F3RSELFREF2   0x98
#define D0F3PMBYREOQ    0x99
#define D0F3MCLKAEN     0x9b
#define D0F3ODTLTBL     0x9c
#define D0F3ODTCTL      0x9e
#define D0F3ODTCTL2     0x98

#define D0F3DTIMING5    0xc4
#define D0F3DTIMING6    0xc5
#define D0F3DTIMING7    0xc6
#define D0F3REFCNT	0xc7
#define D0F3DINIT1      0xcc

#define DRAM_ATTRIBUTE rank_layout

void io_timing_control(DRAM_ATTRIBUTE *dram_attr);


//==================================================================
// DDR3 Code Start{
//==================================================================


#define TX_DQ_Clock_Address_H  0x10
#define TX_DQ_Clock_Pattern    0x5A

#define RX_Input_Enable_Clock_Address_H 	0x20
#define RX_Input_Enable_Clock_Pattern   	0x5A

#define RX_Input_DQS_Clock_Address_H 	0x30
#define RX_Input_DQS_Clock_Pattern 	 	0x5A

#define MCU PCI_DEV(0, 0, 3)

#define D0F3IOTIMSEL_DMIOSEL3_0		0xf

#define D0F3IOTIMSEL_DMIOSEL_MANUAL	0x0
#define D0F3IOTIMSEL_DMIOSEL_CENTER	0x0
#define D0F3IOTIMSEL_DMIOSEL_LOWER	0x1
#define D0F3IOTIMSEL_DMIOSEL_UPPER	0x2

#define D0F3IOTIMSEL_DMIOSEL_DQSO	(0<<2)
#define D0F3IOTIMSEL_DMIOSEL_DQO	(1<<2)
#define D0F3IOTIMSEL_DMIOSEL_DQSI	(2<<2)
#define D0F3IOTIMSEL_DMIOSEL_RDSIT	(3<<2)


static void set_io_timing_si_register(uint8_t io_timing_mode, uint8_t reg_off, uint8_t nand_data, uint8_t or_data)
{
	uint8_t d8;
	
	switch (io_timing_mode) {
		
		case IO_RDSIT:
			pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_RDSIT+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		case IO_DQO:
			pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQO+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		case IO_DQSO:
			pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQSO+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			
		case IO_DQSI:
			pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQSI+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		default:
			break;
	}
	
	
	d8= pci_read_config8(MCU, reg_off);
	d8 = (d8 &(~nand_data))|or_data;
	
	switch (io_timing_mode)
	{
		case IO_RDSIT:
			pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_RDSIT+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		case IO_DQO:
			pci_modify_config8(MCU, D0F3IOTIMSEL,  D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQO+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		case IO_DQSO:
			pci_modify_config8(MCU, D0F3IOTIMSEL,  D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQSO+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		case IO_DQSI:
			pci_modify_config8(MCU, D0F3IOTIMSEL,  D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_DQSI+D0F3IOTIMSEL_DMIOSEL_MANUAL);
			break;
			
		default:
			break;
	}
	
	pci_write_config8(MCU, reg_off, d8);
}

static void dram_ioctl(uint8_t io_timing_mode, uint8_t io_selection_mode, uint8_t is_write, uint8_t byte_i, uint8_t  *d8)
{
	uint8_t or_data, and_data;
	
	or_data = 0;
	and_data = 0;
	switch(io_timing_mode)
	{
		case IO_RDSIT:
			or_data = D0F3IOTIMSEL_DMIOSEL_RDSIT;
			break;
			
		case IO_DQO:
			or_data = D0F3IOTIMSEL_DMIOSEL_DQO;
			break;
			
		case IO_DQSO:
			or_data = D0F3IOTIMSEL_DMIOSEL_DQSO;
			break;
			
		case IO_DQSI:
			or_data = D0F3IOTIMSEL_DMIOSEL_DQSI;
			break;
		default:
			break;
	}
	
	switch(io_selection_mode)
	{
		case IOCTL_MANUAL:
			or_data |= D0F3IOTIMSEL_DMIOSEL_MANUAL;
			break;
			
		case IOCTL_CENTER:
			or_data |= D0F3IOTIMSEL_DMIOSEL_CENTER;
			break;
			
		case IOCTL_LOW:
			or_data |= D0F3IOTIMSEL_DMIOSEL_LOWER;
			break;
			
		case IOCTL_HIGH:
			or_data |= D0F3IOTIMSEL_DMIOSEL_UPPER;
			break;
			
		default:
			break;
	}
	
	pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, or_data);

#define D0F3DITIMCTL_RSETDSIT	0x10
#define D0F3DITIMCTL_RSETDSIDLY	0x01
#define D0F3DOTIMCTL_RSETTXCLK	0x01


	if (io_selection_mode == IOCTL_MANUAL)
	{
		switch(io_timing_mode)
		{
			case IO_RDSIT:
				pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RSETDSIT, D0F3DITIMCTL_RSETDSIT);
				break;
				
			case IO_DQO:
			case IO_DQSO:
				pci_modify_config8(MCU, D0F3DOTIMCTL, D0F3DOTIMCTL_RSETTXCLK, D0F3DOTIMCTL_RSETTXCLK);
				break;
				
			case IO_DQSI:
				pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RSETDSIDLY, D0F3DITIMCTL_RSETDSIDLY);
				break;
				
			default:
				break;
		}
	}else
	{
		switch(io_timing_mode)
		{
			case IO_RDSIT:
				pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RSETDSIT, 0);
				break;
			case IO_DQO:
			case IO_DQSO:
				pci_modify_config8(MCU, D0F3DOTIMCTL,  D0F3DOTIMCTL_RSETTXCLK, 0);
				break;
				
			case IO_DQSI:
				pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RSETDSIDLY, 0);
				break;
				
			default:
				break;
		}
	}
	
	if (is_write){
		pci_write_config8(MCU, (D0F3DQG0IOTC + byte_i), *d8);
	}else
	{
		*d8 = pci_read_config8(MCU, (D0F3DQG0IOTC + byte_i));
	}
}

static const u8 OutputTiming_Tbl[8][8];
static const u8 OutputTiming_Tbl_Items = 0;
static const u8 ReadTiming_Tbl[8][8];
static const u8 ReadTiming_Tbl_Items = 0;

static const u8 SIDramMask = 0;
static const u8 SIDramAttr = 0;
static const u8 SIDramReg = 0;

#define Normal 0

static void preset_si_normal_table(DRAM_ATTRIBUTE *dram_attr)
{

	uint8_t 	   reg_i, or_data, freq_i;
	
	uint8_t io_timing_mode = IO_DQO;

	freq_i = 0;
	/*FIXME:
	switch (dram_attr->DRAMFreq) {
		case DRAMFREQ_800: 	freq_i = SIDramFreq800; break;
		case DRAMFREQ_1066: freq_i = SIDramFreq1066; break;
		case DRAMFREQ_1333: freq_i = SIDramFreq1333; break;
		default:			freq_i = SIDramFreq800; break;
	}
	*/
	
	for (reg_i=0; reg_i< OutputTiming_Tbl_Items; reg_i++)
	{
		
		or_data = shift_to_mask_bit(OutputTiming_Tbl[reg_i][SIDramMask], OutputTiming_Tbl[reg_i][freq_i]);
		switch (OutputTiming_Tbl[reg_i][SIDramAttr])
		{
			case Normal:
				set_io_timing_si_register( io_timing_mode, OutputTiming_Tbl[reg_i][SIDramReg], OutputTiming_Tbl[reg_i][SIDramMask], or_data);
				break;
			default:
				break;
		}
	}
	
	io_timing_mode = IO_RDSIT;
	for (reg_i=0; reg_i< ReadTiming_Tbl_Items; reg_i++)
	{
		or_data = shift_to_mask_bit(ReadTiming_Tbl[reg_i][SIDramMask], ReadTiming_Tbl[reg_i][freq_i]);
		switch (ReadTiming_Tbl[reg_i][SIDramAttr])
		{
			case Normal:
				set_io_timing_si_register( io_timing_mode, ReadTiming_Tbl[reg_i][SIDramReg], ReadTiming_Tbl[reg_i][SIDramMask], or_data);
				break;
			default:
				break;
		}
	}
}


static void preset_tx_dqo(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t i, d8;
	if(1)//(dram_attr->DRAMFreq >= DRAMFREQ_1066)
	{
		for (i = 0; i < 8 ; i++)
		{
			//Set DQO Rx70-78 bit 7 = 1 when DRAMFReq_1066
			d8 = 0x80;  //Set RDQWADJ   //3410-24-LNA-DRAM
			dram_ioctl( IO_DQSO, IOCTL_MANUAL, TRUE, i, &d8);//3410-24-LNA-DRAM
			d8 = 0x80;  //Set RDQWADJ
			dram_ioctl( IO_DQO, IOCTL_MANUAL, TRUE, i, &d8);
		}
	}
}

static void preset_rdswadvos(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t i, d8;
	
	if(1)//(dram_attr->DRAMFreq >= DRAMFREQ_1066)
	{
		for (i = 0; i < 8 ; i++)
		{
			//Set DQO Rx70-78 bit 7 = 1 when DRAMFReq_1066
			d8 = 0x80;
			dram_ioctl( IO_DQSO, IOCTL_MANUAL, TRUE, i, &d8);
			d8 = 0x80;
			dram_ioctl( IO_DQO, IOCTL_MANUAL, TRUE, i, &d8);
		}
	}
}

static void preset_for_lcu_hw(DRAM_ATTRIBUTE *dram_attr)
{
	//uint8_t Index, Value8;
	preset_tx_dqo(dram_attr);
}

#define D0F3DITIMCTL_RDLLDSIDLY 0x04
static void rx_input_delay_save(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t i = 0;
	uint8_t d8 = 0;
	
	//set Rx71[2] = 1
	pci_modify_config8(MCU, D0F3DITIMCTL,
			   D0F3DITIMCTL_RDLLDSIDLY, D0F3DITIMCTL_RDLLDSIDLY);
	
	for (i = 0; i < 8; i++){
		dram_ioctl( IO_DQSI, IOCTL_MANUAL, FALSE,i, &d8);
		dram_ioctl( IO_DQSI, IOCTL_MANUAL, TRUE,i, &d8);
	}
	
	//set Rx71[2] = 0
	pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RDLLDSIDLY, 0);
}

void set_vr_map(uint8_t		dimm_i, uint8_t		vr_i);
uint32_t get_ddr3_mr0(DRAM_ATTRIBUTE *dram_attr, uint8_t rank_i, uint8_t action_type);
uint32_t get_ddr3_mr1(DRAM_ATTRIBUTE *dram_attr, uint8_t rank_i, uint8_t action_type);
uint32_t get_ddr3_mr2(DRAM_ATTRIBUTE *dram_attr, uint8_t rank_i, uint8_t action_type);
uint32_t get_ddr3_mr3(DRAM_ATTRIBUTE *dram_attr, uint8_t rank_i, uint8_t action_type);
uint32_t rank1_address_mirror(uint32_t mrs_value);
void udelay(unsigned usecs);
static void rx_input_delay_hw(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t  save_rx52,save_rx53,save_rxc7;
	uint8_t  i,tmp;
	uint32_t buffer32=0, dummy_read_base=0x00;
	uint32_t mrs_set;
	
	//Step1.Save Rx52, 53 and set Rx52=33, Rx53=5B before RDSIT CAL, NeedeCheck
	save_rx52 = pci_read_config8(MCU, D0F3INLVSEL);
	pci_write_config8(MCU, D0F3INLVSEL, 0x33);
	
	save_rx53 = pci_read_config8(MCU, D0F3INLVSEL_1);
	pci_write_config8(MCU, D0F3INLVSEL_1, 0x5B);
	
	//save refresh counter value and disable it
	save_rxc7 = pci_read_config8(MCU, D0F3REFCNT);
	pci_write_config8(MCU, D0F3REFCNT, 0);
	
	//Step 2. Clear all VRank size
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		pci_write_config8(MCU, (D0F3VR0BA+i), 0);
		pci_write_config8(MCU, (D0F3VR0EA+i), 0);
	}
	
	// Step 3. Map ChA lowest physical rank to VR0 and ChB lowest physical rank to VR4
	//If DIMM exist, ChA, map to VR3; ChB, map to VR7
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map(i, 0x03);
		}
	}
	
	//Map the lowest rank to VR0/VR4
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x00);
			pci_write_config8(MCU, D0F3VR0EA, 0x01);
			break;
		}
	}


#define RDMTSA_31_24 			0xff
#define D0F3DINITCTL_RSDM2_0		0x07
#define D0F3DINITCTL_RSDM_NORMAL	0x00
#define D0F3DINITCTL_RSDM_PREA		0x02
#define D0F3DINITCTL_RSDM_MRS		0x03

#define D0F3DITIMCTL_RDMRLEN	0x80
#define D0F3DITIMCTL_RDRLDQPAT	0x40
	//Step 4. Set IO calibration address
	pci_modify_config8(MCU, D0F3ADRIOCAL_1 , RDMTSA_31_24, 0);
	pci_modify_config8(MCU, D0F3ADRIOCAL , 0xF0, RX_Input_DQS_Clock_Address_H);
	//Precharge all
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_PREA);
	buffer32 = *(uint32_t volatile *)(uint32_t)(dummy_read_base);	// Dummy Read
	udelay( 1000);	//delay 1ms
	
	//Step 5. Set MRS mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_MRS);
	
	//Step6.Enable read leveling and set return data format
	pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RDMRLEN|D0F3DITIMCTL_RDRLDQPAT, D0F3DITIMCTL_RDMRLEN);
	
	//Step 8. Trigger MRS3 with MPR enable
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			buffer32 = get_ddr3_mr3( dram_attr, i, MR3_MPR_ON);
			mrs_set = buffer32;
			//if (dram_attr->Rank1_Mapping[i] == RANK1_MAPPING_MIRRORED)
			//{
			//	mrs_set = rank1_address_mirror(buffer32);
			//}
			buffer32 = *(u32 volatile *)(u32)(dummy_read_base|mrs_set);	// Dummy Read
			outb(0x00, 0xed);
			break;
		}
	}
	
	//Step9. Set Normal mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_NORMAL);

#define D0F3ODTCTL_RODTEN	0X80
#define D0F3DITIMCTL_RSCNDSIDLY	0x02
	pci_modify_config8(MCU, D0F3ODTCTL, D0F3ODTCTL_RODTEN, D0F3ODTCTL_RODTEN);
	
	//Step10.Set D0F3Rx71[1:0]=10b
	pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RSCNDSIDLY|D0F3DITIMCTL_RSETDSIDLY, D0F3DITIMCTL_RSCNDSIDLY);
	
	//Step 11. Check if calibration is done
	do{
		tmp = 0xFF;
		tmp = pci_read_config8(MCU, D0F3DITIMCTL);
		tmp = tmp & D0F3DITIMCTL_RSCNDSIDLY;
	}while(tmp != 0);
	
	//Step 12. Set MRS mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_MRS);
	
	//Step 13. Trigger MRS3 with MPR disable
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			buffer32 = get_ddr3_mr3(dram_attr, i, MR3_INIT);
			mrs_set = buffer32;
			//if (dram_attr->Rank1_Mapping[i] == RANK1_MAPPING_MIRRORED)
			//{
			//	mrs_set = rank1_address_mirror(buffer32);
			//}
			buffer32 = *(u32 volatile *)(u32)(dummy_read_base|mrs_set);	// Dummy Read
			outb(0x00, 0xed);
			break;
		}
	}
	
	//Step 14. Set Normal mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_NORMAL);
	
	//Step 16. Set D0F3Rx71[7]=0 to disable read leveling
	pci_modify_config8(MCU, D0F3DITIMCTL, D0F3DITIMCTL_RDMRLEN, 0);
	
	//restore refresh counter value
	pci_write_config8(MCU, D0F3REFCNT, save_rxc7);
	
	//Restore Rx52, 53
	pci_write_config8(MCU, D0F3INLVSEL, save_rx52);
	pci_write_config8(MCU, D0F3INLVSEL_1, save_rx53);
}

static void tx_dws_clock_hw(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t  save_rx52, save_rx53, save_rxc7;
	uint8_t  i,tmp;
	uint32_t buffer32=0,dummy_read_base=0x00;
	uint32_t mrs_set;
	uint8_t  save_begin[CHIP_MAX_RANKS];
	uint8_t  save_end[CHIP_MAX_RANKS];
	uint8_t  save_rx9e,save_rx9c;
	uint8_t  save_rx54, save_rx55;
	uint8_t  cha_cal_rank_selected = 0;
	uint8_t  cal_rank_index = 0;
	
	//Step1.Save Rx52, 53 and set Rx52=33, Rx53=5B
	save_rx52 = pci_read_config8(MCU, D0F3INLVSEL);
	pci_write_config8(MCU, D0F3INLVSEL, 0x33);
	
	save_rx53 = pci_read_config8(MCU, D0F3INLVSEL_1);
	pci_write_config8(MCU, D0F3INLVSEL_1, 0x5B);
	
	//save all VR begin/End addr, and clear them
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		save_begin[i] = pci_read_config8(MCU, (D0F3VR0BA+i));
		pci_write_config8(MCU, (D0F3VR0BA+i), 0);
		save_end[i] = pci_read_config8(MCU, (D0F3VR0EA+i));
		pci_write_config8(MCU, (D0F3VR0EA+i), 0);
	}
	
	//save ODT Control register and disable ODT control
	save_rx9e = pci_read_config8(MCU, D0F3ODTCTL);
	pci_write_config8(MCU, D0F3ODTCTL, (save_rx9e & (~D0F3ODTCTL_RODTEN)));
	
	//save ODT Lookup Table and set 11100100b for ChA
	save_rx9c = pci_read_config8(MCU, D0F3ODTLTBL);
	pci_write_config8(MCU, D0F3ODTLTBL, 0xE4);
	
	//save refresh counter value and disable it
	save_rxc7 = pci_read_config8(MCU, D0F3REFCNT);
	pci_write_config8(MCU, D0F3REFCNT, 0);
	
	//save Rx54-57
	save_rx54 = pci_read_config8(MCU, D0F3PVRMAP);
	save_rx55 = pci_read_config8(MCU, D0F3PVRMAP_1);

#define D0F3BAINLVCTL_RPGEN	0x01
	//Disable Page Mode and Disable Page Mode of multibanks
	pci_modify_config8(MCU, D0F3BAINLVCTL, D0F3BAINLVCTL_RPGEN, 0);
	
	//set MRS mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_MRS);
	
	// Step 3. Map ChA lowest physical rank to VR0 and ChB lowest physical rank to VR4
	//If DIMM exist, ChA, map to VR3; ChB, map to VR7
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x03);
		}
	}
	
	//A Find the lowest rank existed on ChA and set MRS1 to (WLVL = enabled) and (Rtt_Nom = 120ohm)
	//Set other rank existed to (WLVL = enabled) and (Qoff = disabled)
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x00);
			pci_write_config8(MCU, D0F3VR0EA, 0x01);
			if (cha_cal_rank_selected == 0)
			{
				//set MRS1 to (WLVL = enabled) and (Rtt_Nom = 120ohm)
				buffer32 = get_ddr3_mr1( dram_attr, i, MR1_WLVL_UNDERCALPR);
				mrs_set = buffer32;
				//if (dram_attr->Rank1_Mapping[i] == RANK1_MAPPING_MIRRORED)
				//{
				//	mrs_set = rank1_address_mirror(buffer32);
				//}
				buffer32 = *(u32 volatile *)(u32)(dummy_read_base|mrs_set);	// Dummy Read
				outb(0x00, 0xed);
				cal_rank_index = i;
				cha_cal_rank_selected = 1;
			}else{
				//set MRS1 to (WLVL = enabled) and (Qoff = disabled)
				buffer32 = get_ddr3_mr1( dram_attr, i, MR1_WLVL_OTHERPR);
				mrs_set = buffer32;
				//if (dram_attr->Rank1_Mapping[i] == RANK1_MAPPING_MIRRORED)
				//{
				//	mrs_set = rank1_address_mirror( buffer32);
				//}
				buffer32 = *(u32 volatile *)(u32)(dummy_read_base|mrs_set);	// Dummy Read
				outb(0x00, 0xed);
			}
			//Map to VR3 and Clear End Address
			set_vr_map( i, 0x03);
			pci_write_config8(MCU, D0F3VR0EA, 0x00);
		}
	}

#define D0F3ODTCTL_RDMWLPR_Offset	2
#define D0F3ODTCTL_RDMWLPR1_0		0xc0

#define D0F3DOTIMCTL_RDMWLEN		0x04
	//Step 8. Set which physical rank is under write leveling
	tmp = cal_rank_index << D0F3ODTCTL_RDMWLPR_Offset;
	pci_modify_config8(MCU, D0F3ODTCTL, D0F3ODTCTL_RDMWLPR1_0, tmp);
	
	//Step 9. Start Write Leveling
	pci_modify_config8(MCU, D0F3DOTIMCTL, D0F3DOTIMCTL_RDMWLEN, D0F3DOTIMCTL_RDMWLEN);
	
	//Step10.Check if Write Leveling is done
	do{
		tmp = pci_read_config8(MCU, D0F3DOTIMCTL);
	}while((tmp & D0F3DOTIMCTL_RDMWLEN)!=0);
	
	//Step11.Stop Write Leveling
	pci_modify_config8(MCU, D0F3DOTIMCTL, D0F3DOTIMCTL_RDMWLEN, 0);
	
	//Step 17. Restore all existed ranks MR1 to normal value
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x00);
			pci_write_config8(MCU, D0F3VR0EA, 0x01);
			
			//set MRS1 to normal value
			buffer32 = get_ddr3_mr1( dram_attr, i, MR1_INIT);
			mrs_set = buffer32;
			//if (dram_attr->Rank1_Mapping[i] == RANK1_MAPPING_MIRRORED)
			//{
			//	mrs_set = rank1_address_mirror( buffer32);
			//}
			buffer32 = *(u32 volatile *)(u32)(dummy_read_base|mrs_set);	// Dummy Read
			outb(0x00, 0xed);
			
			//Map to VR3 and Clear End Address
			set_vr_map( i, 0x03);
			pci_write_config8(MCU, D0F3VR0EA, 0x00);
		}
	}
	
	//Step18.Assign PR number to VR number  <-Tony_debug maybe remove Step 18
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		set_vr_map( i, i);
	}
	
	//Restore Rx54-57
	pci_write_config8(MCU, D0F3PVRMAP_1, save_rx55);
	pci_write_config8(MCU, D0F3PVRMAP, save_rx54);
	
	//Step 19. Set Normal mode
	pci_modify_config8(MCU, D0F3DINITCTL, D0F3DINITCTL_RSDM2_0, D0F3DINITCTL_RSDM_NORMAL);
	
	//Enable Page Mode and Enable Page Mode of multibanks
	pci_modify_config8(MCU, D0F3BAINLVCTL, D0F3BAINLVCTL_RPGEN, D0F3BAINLVCTL_RPGEN
	);
	
	//Step 20. Restore auto refresh value
	pci_write_config8(MCU, D0F3REFCNT, save_rxc7);
	
	//Step 22. Restore ChA ODT lookup table
	pci_write_config8(MCU, D0F3ODTLTBL, save_rx9c);
	
	//Step 23. Restore ODT control
	pci_write_config8(MCU, D0F3ODTCTL, save_rx9e);
	
	//Restore all ranks' begin/end address
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		pci_write_config8(MCU, (D0F3VR0BA+i), save_begin[i]);
		pci_write_config8(MCU, (D0F3VR0EA+i), save_end[i]);
	}
	
	//Restore Rx52, 53
	pci_write_config8(MCU, D0F3INLVSEL, save_rx52);
	pci_write_config8(MCU, D0F3INLVSEL_1, save_rx53);
}

static void tx_dqs_clock_lcuhw(DRAM_ATTRIBUTE *dram_attr, uint8_t dimm_no)   //3410-32-LNA-01
{
	uint8_t i,tmp;
	uint8_t d8;
	
	//Step 2. Clear all VRank size
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		pci_write_config8(MCU, (D0F3VR0BA+i), 0);
		pci_write_config8(MCU, (D0F3VR0EA+i), 0);
	}
	
	
	//If DIMM exist, ChA, map to VR3; ChB, map to VR7
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x03);
		}
	}
	
	
	//Step 3. Set the begin/end address of the lowest virthual rank of channel A/B to 0~256
	//Because we will assign PR number to VR number, so we could check the PR rank map
	//to determine which VR we should program.
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			if(dimm_no == CAL_SECOND_DIMM)
			{
				pci_write_config8(MCU, D0F3PVRMAP_1, 0xB8);
				pci_write_config8(MCU, D0F3PVRMAP, 0xBB);
			}else    //3410-32-LNA-01
			{
				set_vr_map( i, 0x00);
			}
			pci_write_config8(MCU, D0F3VR0EA, 0x01);
			break;
		}
	}
	
#define D0F3BAINLVCTL_RNONPGMEN	0
#define RDMTSA_23_20		0xf0
#define D0F3DOTIMCTL_RSCNTXCLKS	0x20
	//Disable Page Mode and Disable Page Mode of multibanks, NeedCheck Why MUST use this???
	d8 = pci_read_config8(MCU, D0F3BAINLVCTL);
	pci_modify_config8(MCU, D0F3BAINLVCTL, D0F3BAINLVCTL_RNONPGMEN + D0F3BAINLVCTL_RPGEN, D0F3BAINLVCTL_RNONPGMEN);
	
	//Step 3. Program F3Rx8E, set IO calibration data pattern
	pci_write_config8(MCU, D0F3DQPIOCAL, 0x5A);
	
	//Step 4. Set F3Rx8D[7:0] and F3Rx8C[7:4] to the value
	//which is different with that for the upper calibration
	pci_modify_config8(MCU, D0F3ADRIOCAL_1,RDMTSA_31_24, 0);
	pci_modify_config8(MCU, D0F3ADRIOCAL,RDMTSA_23_20, 0);
	
	//Step 6. Start Calibration, set auto mode for Internal Clock Phase for DQ/DQS output
	pci_modify_config8(MCU, D0F3DOTIMCTL,D0F3DOTIMCTL_RSETTXCLK, 0);
	
	//Step 7. enable scan internal clock phase for DQS output
	pci_modify_config8(MCU, D0F3DOTIMCTL,D0F3DOTIMCTL_RSCNTXCLKS, D0F3DOTIMCTL_RSCNTXCLKS);
	
	//Step 8. Check if calibration is done
	do{
		tmp = pci_read_config8(MCU, D0F3DOTIMCTL);
	}while((tmp & D0F3DOTIMCTL_RSCNTXCLKS) != 0);
	
	//second test use different pattern
	//Step 3. Program F3Rx8E, set IO calibration data pattern
	pci_write_config8(MCU, D0F3DQPIOCAL, 0xA5);
	
	//Step 7. enable scan internal clock phase for DQS output
	pci_modify_config8(MCU, D0F3DOTIMCTL,D0F3DOTIMCTL_RSCNTXCLKS, D0F3DOTIMCTL_RSCNTXCLKS);
	
	//Step 8. Check if calibration is done
	do{
		tmp = pci_read_config8(MCU, D0F3DOTIMCTL);
	}while((tmp & D0F3DOTIMCTL_RSCNTXCLKS) != 0);
	
	//Enable Page Mode and Enable Page Mode of multibanks
	pci_write_config8(MCU, D0F3BAINLVCTL,d8);
	
}

static void tx_dq_clock_hw(DRAM_ATTRIBUTE *dram_attr, uint8_t diomm_no)
{
	uint8_t i,tmp;
	
	//Step 2. Clear all VRank size
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		pci_write_config8(MCU, (D0F3VR0BA+i), 0);
		pci_write_config8(MCU, (D0F3VR0EA+i), 0);
	}
	
	//If DIMM exist, ChA, map to VR3; ChB, map to VR7
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			set_vr_map( i, 0x03);
		}
	}
	
	//Step 3. Set the begin/end address of the lowest virthual rank of channel A/B to 0~256
	//Because we will assign PR number to VR number, so we could check the PR rank map
	//      to determine which VR we should program.
	for(i = 0; i < CHIP_MAX_RANKS; i++)
	{
		if(dram_attr->phys_rank_size[i] != 0)
		{
			if(diomm_no == CAL_SECOND_DIMM)
			{
				set_vr_map( 2, 0x00);    //set rank2
			}else
			{
				set_vr_map( i, 0x00);
			}
			pci_write_config8(MCU, D0F3VR0EA, 0x01);
			break;
		}
	}
	
	//Step 4. Set IO calibration address
	pci_modify_config8(MCU, D0F3ADRIOCAL_1,RDMTSA_31_24, 0);
	pci_modify_config8(MCU, D0F3ADRIOCAL,RDMTSA_23_20, 0);
	
	//step5.Set IO calibration pattern
	pci_write_config8(MCU, D0F3DQPIOCAL, 0x5A);
	//set Rx75[0]=0
	pci_modify_config8(MCU, D0F3DOTIMCTL,D0F3DOTIMCTL_RSETTXCLK, 0);
#define D0F3DOTIMCTL_RSCNTXCLKD 0x02
	//Step 6. Enable scan internal clock phase for DQ output
	pci_modify_config8(MCU, D0F3DOTIMCTL,D0F3DOTIMCTL_RSCNTXCLKD, D0F3DOTIMCTL_RSCNTXCLKD);
	
	//Step 7. Check if DQ output clock calibration is done
	do{
		tmp = pci_read_config8(MCU, D0F3DOTIMCTL);
	}while((tmp&D0F3DOTIMCTL_RSCNTXCLKD)!= 0x00);
}


static void rx_input_delay_cal_to_manual(DRAM_ATTRIBUTE *dram_attr, uint16_t * p)
{
	uint8_t  i, d8, d81;
	
	
	//for (i = 0; i < 8; i++){
		for (i = 0; i < 8; i++)
		{
			dram_ioctl(IO_DQSI,IOCTL_CENTER,FALSE,i,&d8);
			
			dram_ioctl(IO_DQSI,IOCTL_LOW,FALSE,i,&d81);
			if(d81 < 3)
			{
				if(i == 0x02 || i == 0x04)
				{
					d8 += 0x04;
				}else
				{
					d8 += 0x03;
				}
			}
			//3410-25-LNA-01-end
			
			//3410-24-LNA-DRAM-end
			//3410-31-LNA-03-start
			dram_ioctl(IO_DQSI,IOCTL_HIGH,FALSE,i,&d81);
			if (d81 > 0x38)
			{
				d8 -= 0x06;
			}else if(d81 > 0x30)
			{
				d8 -= 0x04;
			}
			
			if (d8 > 0x20)
			{
				d8 = 0x20;
			}
			
			p[i] += d8;//3410-42-LNA-02
		}
}

static void tx_dqs_clock_cal_to_manual(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t  i,d8;
	
	for (i = 0; i < 8; i++)
	{
		dram_ioctl(IO_DQSO,IOCTL_CENTER, FALSE, i, &d8);
		dram_ioctl(IO_DQSO,IOCTL_MANUAL, TRUE, i, &d8);
	}
}

static void tx_dq_clock_cal_to_mannual(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t  i,d8;
	
	for (i = 0; i < 8; i++)
	{
		dram_ioctl(IO_DQO,IOCTL_CENTER, FALSE, i, &d8);
		dram_ioctl(IO_DQO,IOCTL_MANUAL, TRUE, i, &d8);
	}
}


#define DQSI_THRESHOLD  0x10
//3410-30-LNA-02    #define	DQSI_THRESHOLD_1066	0x20		//3410-21-LNA-04
#define	DQSI_THRESHOLD_1066	0x10    //3410-30-LNA-02    //for OC 10%+Vcore=1.00V
static BOOLEAN check_dqsi_range(DRAM_ATTRIBUTE *dram_attr)
{
	uint8_t i, low, high;
	BOOLEAN flag;
	
	flag = TRUE;
	//check DQSI range, if one of 8 bytes is less than 10h, return FALSE.
	for(i = 8; i > 0; i--)
	{
		dram_ioctl(IO_DQSI,IOCTL_LOW,FALSE,i-1,&low);
		dram_ioctl(IO_DQSI,IOCTL_HIGH,FALSE,i-1,&high);
		
		if(1)//(dram_attr->DRAMFreq == DRAMFREQ_1066)
		{
			if((high -low) < DQSI_THRESHOLD_1066)
			{
				flag = FALSE;
				break;
			}
		}else {
			if ((high - low) < DQSI_THRESHOLD)
			{
				flag = FALSE;
				break;
			}
		}
	}
	
	return flag;
}

#define DQSO_THRESHOLD  0x12
static BOOLEAN check_dqso_range(void)
{
	//check DQSO range, if one of 8 bytes is less than 18, set CF = 1
	uint8_t i, low, high;
	BOOLEAN flag = TRUE;
	
	for(i = 0; i < 8; i++)
	{
		dram_ioctl(IO_DQSO,IOCTL_LOW,FALSE,i,&low);
		if(low == 0x7F)
		{
			printram("DQS lower bound error\n");
			flag = FALSE;
			break;
		}
		dram_ioctl(IO_DQSO,IOCTL_HIGH,FALSE,i,&high);
		if((high - low) < DQSO_THRESHOLD)
		{
			printram("DQS range error at %u with "
			"0x%.2x - 0x%.2x = 0x%.2x \n", i, high, low, high - low);
			flag = FALSE;
			break;
		}
	}
	return flag;
}


#define LOW_BOUND_OFFSET 0x0-0x5
#define HIGH_BOUND_OFFSET 0x5
#define DQO_THRESHOLD  0x09     //3410-36-LNA-03

static BOOLEAN check_dqo_range(void)
{
	uint8_t i, low,high;
	BOOLEAN flag;
	
	flag = TRUE;
	//check DQSI range, if one of 8 bytes is less than 9h, return FALSE.
	for(i = 8; i > 0; i--)
	{
		dram_ioctl(IO_DQO,IOCTL_LOW,FALSE,i-1,&low);
		if (low == 0x7F)
		{
			flag = FALSE;
			printram("DQ lower bound error\n");
			break;
		}
		dram_ioctl(IO_DQO,IOCTL_HIGH,FALSE,i-1,&high);
		if((high - low) < DQO_THRESHOLD)
		{
			flag = FALSE;
			printram("DQ Range error\n");
			break;
		}
	}
	
	return flag;
}


static void	rx_input_enable_si2(DRAM_ATTRIBUTE *dram_attr)
{
	//uint32_t count;
	//uint8_t d8;
	u8 dram_freq;
	
	dram_freq = DRAMFREQ_1066; //dram_attr->DRAMFreq;
	
	pci_modify_config8(MCU, D0F3IOTIMSEL, D0F3IOTIMSEL_DMIOSEL3_0, D0F3IOTIMSEL_DMIOSEL_RDSIT+D0F3IOTIMSEL_DMIOSEL_MANUAL);
	//4310-32-LNA-01-start
#if 0
	if(dram_attr->ModuleType == MODULE_TYPE_SODIMM)
	{
		switch(dram_attr->RawCardType)
		{
			case RAW_CARD_AO:
			case RAW_CARD_AA:
			case RAW_CARD_AB:
			case RAW_CARD_BO:
			case RAW_CARD_BA:
			case RAW_CARD_BB:
			case RAW_CARD_OA:
			case RAW_CARD_OB:
				for(count = 0; count < RdsitRefRawCardABTbl_Items; count++)
				{
					if(dram_freq == DRAMFREQ_1333)
					{
						d8 = RdsitRefRawCardABTbl[count][SI_Rdsit_Ref_Freq_1333_OFFSET];
					}else if(dram_freq == DRAMFREQ_1066)
					{
						d8 = RdsitRefRawCardABTbl[count][SI_Rdsit_Ref_Freq_1066_OFFSET];
					}else
					{
						d8 = RdsitRefRawCardABTbl[count][SI_Rdsit_Ref_Freq_0800_OFFSET];
					}
					pci_write_config8(MCU, (D0F3DQG0IOTC+count), d8);
				}
				break;
			case RAW_CARD_AF:
			case RAW_CARD_BF:
			case RAW_CARD_FA:
			case RAW_CARD_FB:
				for(count = 0; count < RdsitRefRawCardABFTbl_Items; count++)
				{
					if(dram_freq == DRAMFREQ_1333)
					{
						d8 = RdsitRefRawCardABFTbl[count][SI_Rdsit_Ref_Freq_1333_OFFSET];
					}else if(dram_freq == DRAMFREQ_1066)
					{
						d8 = RdsitRefRawCardABFTbl[count][SI_Rdsit_Ref_Freq_1066_OFFSET];
					}else
					{
						d8 = RdsitRefRawCardABFTbl[count][SI_Rdsit_Ref_Freq_0800_OFFSET];
					}
					pci_write_config8(MCU, (D0F3DQG0IOTC+count), d8);
				}
				break;
			case RAW_CARD_FO:
			case RAW_CARD_FF:
			case RAW_CARD_OF:
				for(count = 0; count < RdsitRefRawCardFTbl_Items; count++)
				{
					if(dram_freq == DRAMFREQ_1333)
					{
						d8 = RdsitRefRawCardFTbl[count][SI_Rdsit_Ref_Freq_1333_OFFSET];
					}else if(dram_freq == DRAMFREQ_1066)
					{
						d8 = RdsitRefRawCardFTbl[count][SI_Rdsit_Ref_Freq_1066_OFFSET];
					}else
					{
						d8 = RdsitRefRawCardFTbl[count][SI_Rdsit_Ref_Freq_0800_OFFSET];
					}
					pci_write_config8(MCU, (D0F3DQG0IOTC+count), d8);
				}
				
				break;
			default:
				break;
		}
		
	}    else
#endif
/* FIXME:
	{
		for(count = 0; count < RdsitRefUDIMMTbl_Items; count++)
		{
			if(dram_freq == DRAMFREQ_1333)
			{
				d8 = RdsitRefUDIMMTbl[count][SI_Rdsit_Ref_Freq_1333_OFFSET];
			}else if(dram_freq == DRAMFREQ_1066)
			{
				d8 = RdsitRefUDIMMTbl[count][SI_Rdsit_Ref_Freq_1066_OFFSET];
			}else
			{
				d8 = RdsitRefUDIMMTbl[count][SI_Rdsit_Ref_Freq_0800_OFFSET];
			}
			pci_write_config8(MCU, (D0F3DQG0IOTC+count), d8);
		}
	}
	*/
	pci_write_config8(MCU, 0x78,  0x28);
	pci_write_config8(MCU, 0x79,  0x1c);
	pci_write_config8(MCU, 0x7a,  0x28);
	pci_write_config8(MCU, 0x7b,  0x28);
	pci_write_config8(MCU, 0x7c,  0x2c);
	pci_write_config8(MCU, 0x7d,  0x30);
	pci_write_config8(MCU, 0x7e,  0x30);
	pci_write_config8(MCU, 0x7f,  0x34);
}

//3410-22-LNA-DRAM-end

//3410-32-LNA-01-start
static void set_average_setting(DRAM_ATTRIBUTE *dram_attr)
{
	/*FIXME:
	uint8_t i,d81, d82,d83, d84;
	//set TxDQS
	for (i = 0; i < 8; i++)
	{
		d81 = dram_attr->VIA_DRAM_AutoCal[CAL_FIRST_DIMM][i].TxDqs_Upper_Bound;
		d82 = dram_attr->VIA_DRAM_AutoCal[CAL_SECOND_DIMM][i].TxDqs_Upper_Bound;
		if( d81 > d82)
		{
			d81 = d82;
		}
		d83 = dram_attr->VIA_DRAM_AutoCal[CAL_FIRST_DIMM][i].TxDqs_Lower_Bound;
		d84 = dram_attr->VIA_DRAM_AutoCal[CAL_SECOND_DIMM][i].TxDqs_Lower_Bound;
		if(d83 < d84)
		{
			d83 = d84;
		}
		d81 += d83;
		d81 >>= 1;
		d82 = d81 & 0x1F;
		if(d82 >= 0x1C)
		{
			d81 -= 0x1C;
		}else
		{
			d81 += 0x04;
		}
		dram_ioctl(IO_DQSO,IOCTL_MANUAL,TRUE,i,&d81);
	}
	//set TxDQ
	for (i = 0; i < 8; i++)
	{
		d81 = dram_attr->VIA_DRAM_AutoCal[CAL_FIRST_DIMM][i].TxDq_Upper_Bound;
		d82 = dram_attr->VIA_DRAM_AutoCal[CAL_SECOND_DIMM][i].TxDq_Upper_Bound;
		if( d81 > d82)
		{
			d81 = d82;
		}
		d83 = dram_attr->VIA_DRAM_AutoCal[CAL_FIRST_DIMM][i].TxDq_Lower_Bound;
		d84 = dram_attr->VIA_DRAM_AutoCal[CAL_SECOND_DIMM][i].TxDq_Lower_Bound;
		if( d83 < d84)
		{
			d83 = d84;
		}
		d81 += d83;
		d81 >>= 1;
		d82 = d81 & 0x1F;
		if(d82 >= 0x14)
		{
			d81 -= 0x14;
		}else
		{
			d81 += 0x0C;
		}
		dram_ioctl(IO_DQO,IOCTL_MANUAL,TRUE,i,&d81);
	}
	*/
	
}
static void tx_dqs_clock_save_setting(DRAM_ATTRIBUTE *dram_attr, uint8_t dimm_no)
{
	/* FIXME:
	uint8_t i;
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQSO+D0F3IOTIMSEL_DMIOSEL_UPPER);
	//save TX DQS Upper Bound
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDqs_Upper_Bound = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	//save TX DQS Center Bound
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQSO+D0F3IOTIMSEL_DMIOSEL_CENTER);
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDqs_Center = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	//save TX DQS Lower Bound
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQSO+D0F3IOTIMSEL_DMIOSEL_LOWER);
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDqs_Lower_Bound = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	*/
}
static void tx_dq_clock_save_setting(DRAM_ATTRIBUTE *dram_attr, uint8_t dimm_no)
{
	/*FIXME:
	uint8_t i;
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQO+D0F3IOTIMSEL_DMIOSEL_UPPER);
	//save TX DQS Upper Bound
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDq_Upper_Bound = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	//save TX DQS Center Bound
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQO+D0F3IOTIMSEL_DMIOSEL_CENTER);
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDq_Center = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	//save TX DQS Lower Bound
	pci_modify_config8(MCU, D0F3IOTIMSEL,D0F3IOTIMSEL_DMIOSEL3_0,D0F3IOTIMSEL_DMIOSEL_DQO+D0F3IOTIMSEL_DMIOSEL_LOWER);
	for (i = 0; i < 8; i++)
	{
		dram_attr->VIA_DRAM_AutoCal[dimm_no][i].TxDq_Lower_Bound = pci_read_config8(MCU, (D0F3DQG0IOTC + i));
	}
	*/
}

//defined in init_dram_by_rank.c
void reinit_dram(DRAM_ATTRIBUTE *dram_attr);

#define USE_WLVL  0



//==================================================================
// DDR3 Code End}
//==================================================================


void set_vr_map(uint8_t		dimm_i, uint8_t		vr_i)
{
	uint8_t				vr_reg_off;
	uint8_t				d8;
	
	vr_reg_off = D0F3PVRMAP + ((dimm_i & 6) >> 1);
	
	d8 = pci_read_config8(MCU, vr_reg_off); // 0x54 - 0x55
	if (dimm_i & 8)
	{
		if (dimm_i & 0x10)
		{
			d8 = 0;
		} else
		{
			d8 = 0x88;
		}
	} else {
		if (dimm_i & 1)
		{
			d8 &= 0x0F;
			d8 |= (vr_i << 4);
			if (!(dimm_i & 0x10))
			{
				d8 |= 0x80;
			}
		} else {
			d8 &= 0xF0;
			d8 |= vr_i;
			if (!(dimm_i & 0x10))
			{
				d8 |= 0x08;
			}
		}
	}
	
	pci_write_config8( MCU, vr_reg_off, d8);	// 0x54 - 0x55
}

uint32_t get_ddr3_mr3(DRAM_ATTRIBUTE *dram_attr, uint8_t rank_i, uint8_t action_type)
{
	
	//action_type - action type [MR3_Init/MR3_MPR_On]
	
	u32		mr3_setting = DDR3_MR3;
	
	if (action_type == MR3_MPR_ON)
	{
		mr3_setting |= MR3_DDR3_MPR_EN;
	}
	return mr3_setting;
	
}

static void dbg_dump_0x78_0x7f(void)
{
	u8 i;
	for(i = 0x78; i < 0x80; i ++)
	{
		printram(" %.2x", pci_read_config8(MCU, i));
	}
	printram("\n");
}

static void dbg_dump_calib(const u8 what)
{
	u8 reg8;
	/* Dump lower bound */
	reg8 = ((what & 0x3) << 2) | 0x1;
	pci_write_config8(MCU, 0x70, reg8);
	printram("Lower bound : ");
	dbg_dump_0x78_0x7f();
	
	/* Dump average values */
	reg8 = ((what & 0x3) << 2);
	pci_write_config8(MCU, 0x70, reg8);
	printram("Average     : ");
	dbg_dump_0x78_0x7f();
	
	/* Dump upper bound */
	reg8 = ((what & 0x3) << 2) | 0x2;
	pci_write_config8(MCU, 0x70, reg8);
	printram("Upper bound : ");
	dbg_dump_0x78_0x7f();
}

void io_timing_control(DRAM_ATTRIBUTE *dram_attr)
{
	BOOLEAN    result, result1;
	uint16_t   cal_tx_num = 0;
	uint8_t    dimm_no;
	uint16_t   cal_rx_num = 0;
	uint8_t 	  i;
	uint16_t   my_dqsi[8];
	
	result = FALSE;
	result1 = FALSE;
	u32 ncals = 0;
	
	
	
	//Cal Rx
	preset_si_normal_table(dram_attr);
	rx_input_delay_save(dram_attr);
	rx_input_enable_si2( dram_attr);
	
	while(!(result&&result1))
	{
		ncals++;
		for(i = 0; i < 8; i++)
		{
			my_dqsi[i] = 0;
		}
		printram("R");
		for(cal_rx_num = 0; cal_rx_num < 8; cal_rx_num++)
		{
			rx_input_delay_hw(dram_attr);
			rx_input_delay_cal_to_manual( dram_attr, &my_dqsi[0]);
		}

		
		for(i = 0; i < 8; i++)
		{
			if(my_dqsi[i] & 0x0007)
			{
				my_dqsi[i] >>= 3;
				my_dqsi[i]++;
			}else
			{
				my_dqsi[i] >>= 3;
			}
			
			dram_ioctl(IO_DQSI,IOCTL_MANUAL, TRUE, i, (uint8_t *)&my_dqsi[i]);
		}
		
		result = check_dqsi_range(dram_attr);
		//enable Rx Manual mode
		pci_modify_config8(MCU,D0F3DITIMCTL,
				   D0F3DITIMCTL_RSETDSIDLY | D0F3DITIMCTL_RSETDSIT,
		     D0F3DITIMCTL_RSETDSIDLY | D0F3DITIMCTL_RSETDSIT);
		
		//Cal Tx
		dimm_no = CAL_FIRST_DIMM;
		while(1)
		{    //3410-32-LNA-01
			result =FALSE;     //3410-36-LNA-02
			result1 = FALSE;    //3410-36-LNA-02
			printram("T");
			for (cal_tx_num = 0; cal_tx_num < 300; cal_tx_num++)
			{
				if (USE_WLVL == 1)
				{
					preset_rdswadvos( dram_attr);
					tx_dws_clock_hw( dram_attr);
					tx_dqs_clock_cal_to_manual( dram_attr);
					tx_dq_clock_hw( dram_attr,dimm_no);
					tx_dq_clock_cal_to_mannual( dram_attr);
				}else{
					preset_for_lcu_hw( dram_attr);
					tx_dqs_clock_lcuhw( dram_attr,dimm_no);
					tx_dqs_clock_cal_to_manual( dram_attr);
					tx_dqs_clock_save_setting( dram_attr, dimm_no);
					
					result = check_dqso_range();
					
					if(!result)
					{
						printram("DQS range fail\n");
						dbg_dump_calib(0);
						continue;
					}
				
					tx_dq_clock_hw(dram_attr, dimm_no);
					tx_dq_clock_cal_to_mannual(dram_attr);
					tx_dq_clock_save_setting(dram_attr, dimm_no);
				}
			
				result = check_dqo_range();
				if(!result)
				{
					printram("DQ range fail\n");
					dbg_dump_calib(1);
					continue;
				}
				//enable Tx Manual mode
				pci_modify_config8(MCU,D0F3DOTIMCTL, D0F3DOTIMCTL_RSETTXCLK, D0F3DOTIMCTL_RSETTXCLK);
				
				result1 = dram_base_test( 0, 0x40,EXTENSIVE, FALSE);
				if(result&&result1)
				{
					break;
				}
				if((!result) || (!result1))
				{
					printram("f");
				}
			}
			if(result &&result1)
			{
				if(dimm_no == CAL_SECOND_DIMM)  //DIMM1 calibration is done?
				{
					set_average_setting( dram_attr);
					break;
				} else {
					if(0)//dram_attr->ChADIMMNumber == 2)    //3410-35-LNA-02
					{
						dimm_no = CAL_SECOND_DIMM;
					} else {
						break;
					}
				}
			} else {
				break;
			}
		}
		//after adjust DQSI
		//		reinit_dram(dram_attr);    //3410-36-LNA-02
	}
	
	pci_write_config8(MCU, D0F3IOTIMSEL, 0x04);
	printram("Calib loop did %u loops\n", ncals);
}