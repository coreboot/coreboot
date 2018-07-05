/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/
#include <bdk.h>
#include <libbdk-hal/if/bdk-if.h>
#include <libbdk-hal/bdk-mdio.h>
#include <libbdk-hal/bdk-twsi.h>

/* This code is an optional part of the BDK. It is only linked in
 *     if BDK_REQUIRE() needs it */
//BDK_REQUIRE(TWSI);
BDK_REQUIRE_DEFINE(XFI);

/*
Rate Select Settings
Mode State			: 6/8 
Rate Select State	: 0 
RSEL1				: 0 
RSEL0				: 0 
Ref Clock Gen(MHz)	: 156.25
Data Rate(Gbps)		: 10.3125
Description			: 10 GbE


Data Rate Detection Configuration Registers

Mode Pin Settings:
Mode State	: 0 
MODE1		: 0 
MODE0		: 0 
Mode		: Two-wire serial interface mode

LOS Pin Strap Mode Settings
Mode State			: 2/6/8 
State				: 4 
LOS1				: Float
LOS0				: Float 
LOS Amplitude(mVpp)	: 20 
LOS Hysteresis(dB)	: 2

Input Equalization Retimer Mode Settings
Mode State		: 6/8
EQ State		: 0 
EQ1				: 0 
EQ0				: 0 
EQ(dB)			: Auto 
DFE				: Auto 
Comment			: Full Auto

Input Equalization Re-Driver Mode Settings
Mode State		: 
EQ State		: 0 
EQ1				: 0 
EQ0				: 0 
EQ(dB)			: Auto 
DFE				: APowered Down 
Comment			: Analog EQ Only



Output De-Emphasis Retimer Mode Settings
Mode State		: 6/8 
DE State		: 3 
TX1				: Float 
TX0				: 0 
PRE  c(-1) mA	: -1 
MAIN c( 0) mA	: 15
POST c(+1) mA	: 4 
DC Amplitude(mV): 500
De-Emphasis(dB)	: -6.02
Comment			: 


Output De-Emphasis Re-Driver Mode Settings
Mode State		: 2 
DE State		: 3 
TX1				: Float 
TX0				: 0 
Frequency(Gbps)	: 10.3125 
DC Amplitude(mV): 600
De-Emphasis(dB)	: 4 
Comment			: 10GbE


*/

static int debug = 0;

#define xfi_printf(fmt, args...)		\
	do {					\
		if(debug == 1){			\
			printf(fmt, ##args);	\
		}				\
	} while(0)


int bdk_xfi_vsc7224_dump(int twsi_id, int unit){
	bdk_node_t node=0;
	uint8_t dev_addr=0x10 + unit;
	uint16_t internal_addr=0x7F;
	int num_bytes=2;
	int ia_width_bytes=1;
	uint64_t data=0;
	int p, i;
	uint64_t result[0x100] = {0};

	uint64_t pagenum[9] = {0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x30, 0x31, 0x40};

	for(p=0; p < (sizeof(pagenum)/sizeof(pagenum[0])); p++){
		data = pagenum[p];
		bdk_twsix_write_ia(node, twsi_id, dev_addr, internal_addr, num_bytes, ia_width_bytes, data);
		for(i=0x80; i<=0xFF; i++){
			result[i] = 0x00;
			result[i] = bdk_twsix_read_ia(node, twsi_id, dev_addr, (uint16_t)i, num_bytes, ia_width_bytes);
		}
		for(i=0x80; i<=0xFF; i++){
			if(i==0x80){
				printf("\npage_%02X[0x100] = {\n", (uint8_t)pagenum[p]);
			}
			if(i % 8 == 0){
				printf("/* 0x%2X */", i);
			}
			printf(" 0x%04X,", (uint16_t)result[i]);
			if(i==0xFF){
				printf("};");
			}
			if((i+1) % 8 == 0){
				printf("\n");
			}
		}
	}

	return 0;
}

/* XFI ReTimer/ReDriver Mode Settings */

/*
power down regs:
Page Reg Position Mask val RegFieldName
0x00 0x89 b07 0x0080 1 PD_INBUF
0x00 0x8A b10 0x0400 1 PD_DFECRU
0x00 0x8A b01 0x0002 1 PD_DFE
0x00 0x8A b00 0x0001 1 PD_DFEADAPT
0x00 0x97 b15 0x8000 1 ASYN_SYNN
0x00 0x97 b09 0x0200 1 PD_OD
0x00 0xA0 b11 0x0800 1 PD_LOS
0x00 0xA4 b15 0x8000 1 PD_CH
0x00 0xB5 b07 0x0080 1 PD_INBUF 
0x00 0xB9 b15 0x8000 1 ASYN_SYNN 
0x00 0xB9 b09 0x0200 1 PD_OD
0x00 0xBF b07 0x0080 1 PD_INBUF 
0x00 0xF0 b15 0x8000 1 ASYN_SYNN
0x00 0xF0 b09 0x0200 1 PD_OD
0x00 0xF6 b07 0x0080 1 PD_INBUF
0x00 0xFA b15 0x8000 1 ASYN_SYNN
0x00 0xFA b09 0x0200 1 PD_OD
*/
struct regmap{
	short int page;
	unsigned char reg;
	unsigned short int retimer;
	unsigned short int redriver;
};

/* This table only applies to SFF8104 */
struct regmap xfiregmap[64] = {
//CH 0
{0x00, 0x84, 0x0800, 0x0000}, //EQTABLE_DCOFF0 (0n_84)
{0x00, 0x8A, 0x7000, 0x0400}, //DFECRU_CTRL (0n_8A)
{0x00, 0x8B, 0x4060, 0x0000}, //DFECRU_CFVF_CFAP (0n_8B)
{0x00, 0x90, 0xDE85, 0x0000}, //DFECRU_DFEAUTO (0n_90)
{0x00, 0x91, 0x2020, 0x0000}, //DFECRU_BTMX_BFMX (0n_91)
{0x00, 0x92, 0x0860, 0x0000}, //DFECRU_DXMX_TRMX (0n_92)
{0x00, 0x93, 0x6000, 0x0000}, //DFECRU_TRMN_ERRI (0n_93)
{0x00, 0x94, 0x0001, 0x0000}, //DFECRU_DFEMODE (0n_94)
{0x00, 0x95, 0x0008, 0x0000}, //DFECRU_RATESEL (0n_95)
{0x00, 0x97, 0x0000, 0x8080}, //OUTDRVCTRL (0n_97)
{0x00, 0x99, 0x001E, 0x0014}, //KR_MAINTAP (0n_99)
{0x00, 0x9A, 0x000B, 0x0000}, //KR_PRETAP (0n_9A)
{0x00, 0x9B, 0x0010, 0x0000}, //KR_POSTTAP (0n_9B)
{0x00, 0x9E, 0x03E8, 0x07D0}, //LOSASSRT (0n_9E)
{0x00, 0x9F, 0x04EA, 0x09D5}, //LOSDASSRT (0n_9F)
{0x00, 0xB2, 0x0888, 0x0000}, //NA

//CH 1
{0x01, 0x84, 0x0800, 0x0000},
{0x01, 0x8A, 0x7000, 0x0400},
{0x01, 0x8B, 0x4060, 0x0000},
{0x01, 0x90, 0xDE85, 0x0000},
{0x01, 0x91, 0x2020, 0x0000},
{0x01, 0x92, 0x0860, 0x0000},
{0x01, 0x93, 0x6000, 0x0000},
{0x01, 0x94, 0x0001, 0x0000},
{0x01, 0x95, 0x0008, 0x0000},
{0x01, 0x97, 0x0000, 0x8080},
{0x01, 0x99, 0x001E, 0x0014},
{0x01, 0x9A, 0x000B, 0x0000},
{0x01, 0x9B, 0x0010, 0x0000},
{0x01, 0x9E, 0x03E8, 0x07D0},
{0x01, 0x9F, 0x04EA, 0x09D5},
{0x01, 0xB2, 0x0888, 0x0000},

//POWER_DOWN Channel 2 and 3
{0x02, 0x8A, 0x0400, 0x0400},
{0x02, 0xA4, 0x8000, 0x8000},
{0x03, 0x8A, 0x0400, 0x0400},
{0x03, 0xA4, 0x8000, 0x8000},

{0x30, 0x80, 0x3453, 0x0000}, //FSYNM_NVAL (3f_80)
{0x30, 0x81, 0x00F6, 0x0000}, //FSYNFVAL_MSB (3f_81)
{0x30, 0x82, 0x8800, 0x0000}, //FSYNFVAL_LSB (3f_82)
{0x30, 0x83, 0x000F, 0x0000}, //FSYNRVAL_MSB (3f_83)
{0x30, 0x84, 0xB5E0, 0x0000}, //FSYNRVAL_LSB (3f_84)
{0x30, 0x85, 0x0000, 0x0400}, //FSYNTST (3f_85)

{0x40, 0x80, 0x4C00, 0x0000}, //ANMUXSEL (40_80)
{0x40, 0x81, 0x4000, 0x0000}, //DGMUXCTRL (40_81)
{0x40, 0x82, 0x7800, 0xC000}, //RCKINCTRL (40_82)
{0x40, 0x84, 0x0020, 0x0000}, //CHRCKSEL (40_84)

{-1, 0, 0, 0},
};

int bdk_vsc7224_modeset(int twsi_id, int unit, int xfi_mode){
	bdk_node_t node=0;
	uint8_t dev_addr=0x10 + unit;
	uint16_t internal_addr=0x7F;
	uint16_t page=0;
	int num_bytes=2;
	int ia_width_bytes=1;
	uint64_t data=0;
	int val=0;
	int ret = 0, r=0;
	uint16_t reg = 0;

	if(xfi_mode==0){
	    printf("XFI Mode Retimer\n");
	}else{
	    printf("XFI Mode Redriver\n");
	}

	while(xfiregmap[r].page != -1){
	    page = xfiregmap[r].page;
	    reg  = xfiregmap[r].reg;
	    if(xfi_mode==0){
		data = xfiregmap[r].retimer;
	    }else{
		data = xfiregmap[r].redriver;
	    }
	    ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, internal_addr, num_bytes, ia_width_bytes, (uint64_t)page);
	    if(ret !=0){
		printf("XFI init Error\n");
		break;
	    }
	    ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, reg, 	   num_bytes, ia_width_bytes, data);
	    if(ret !=0){
		printf("XFI init Error\n");
		break;
	    }
	    val = bdk_twsix_read_ia(node, twsi_id, dev_addr,  reg, num_bytes, ia_width_bytes);
	    if(val == -1){
		printf("XFI Read Reg Failed @ page:reg :: %2X:%2X \n",page, reg);
		break;
	    }else{
		xfi_printf(" Page: reg: data: val :: %2X:%2X:%04X:%04X\n", page, reg, (uint16_t)data, val);
	    }
	    r++;
	}

	return ret;
}


int bdk_vsc7224_regmap_modeget(int twsi_id, int unit){
	bdk_node_t node=0;
	uint8_t dev_addr=0x10 + unit;
	uint16_t internal_addr=0x7F;
	uint16_t page=0;
	int num_bytes=2;
	int ia_width_bytes=1;
	//uint64_t data=0;
	uint16_t reg = 0;
	int ret = 0, r=0;
	int data;

	printf("\n===========================================\n");
	printf("Page  :Reg    :Value  :Retimer :Redriver\n");
	printf("===========================================\n");
	while(xfiregmap[r].page != -1){
	    page = xfiregmap[r].page;
	    reg  = xfiregmap[r].reg;

	    ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, internal_addr, num_bytes, ia_width_bytes, (uint64_t)page);
	    if(ret !=0){
		printf("XFI init Error\n");
		break;
	    }
	    data = bdk_twsix_read_ia(node, twsi_id, dev_addr,  reg, num_bytes, ia_width_bytes);
	    if(data == -1){
		printf("XFI Read Reg Failed @ page:reg :: %2X:%2X \n",page, reg);
		break;
	    }
	    printf("     %02X:     %02X:   %04X:    %04X:    %04X\n", page, reg, (uint16_t)data, xfiregmap[r].retimer, xfiregmap[r].redriver);
	    r++;
	}
	printf("=======================================\n");

	return ret;
}

int bdk_vsc7224_wp_regs(int twsi_id, int unit, int xfi_wp){
	bdk_node_t node=0;
	uint8_t dev_addr=0x10 + unit;
	uint16_t internal_addr=0x7E;
	uint16_t data=0x0000;
	int num_bytes=2;
	int ia_width_bytes=1;
	int ret =0;

	if(xfi_wp == 1){
		data = 0x0000;
	}else{
		data = 0xFFFF;
	}

	ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, internal_addr, num_bytes, ia_width_bytes, (uint64_t)data);
	if(ret !=0){
		printf("XFI VSC7224  Write Protect Error\n");
	}

	return ret;
}

int bdk_vsc7224_set_reg(int twsi_id, int unit, int page, int reg, int val){
	bdk_node_t node=0;
	uint8_t dev_addr=0x10 + unit;
	uint16_t internal_addr = reg;
	int num_bytes=2;
	int ia_width_bytes=1;
	int ret=0;

	xfi_printf(" Unit: Page: reg: val :: %02x:%2X:%2X:%04X\n", unit, page, reg, val & 0xFFFF);
	ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, 0x7F, num_bytes, ia_width_bytes, (uint64_t)(page & 0xFF));
	if (ret) {
		printf("XFI VSC7224  TWSI Set Page Register Error\n");
	}

	ret = bdk_twsix_write_ia(node, twsi_id, dev_addr, internal_addr, num_bytes, ia_width_bytes, (uint64_t)(val & 0xFFFF));
	if (ret) {
		printf("XFI VSC7224  TWSI Set Register Error\n");
	}

	return ret;
}

int bdk_vsc7224_debug(int _debug){
	debug =_debug;
	return 0;
}
