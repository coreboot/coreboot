/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

/* Call-backs */
#include <delay.h>
u16 mctGet_NVbits(u8 index)
{
	u16 val = 0;

	switch (index) {
	case NV_PACK_TYPE:
#if CONFIG_CPU_SOCKET_TYPE == 0x10	/* Socket F */
		val = 0;
#elif CONFIG_CPU_SOCKET_TYPE == 0x11   /* AM2r2 */
		val = 1;
//#elif SYSTEM_TYPE == MOBILE
//		val = 2;
#endif
		break;
	case NV_MAX_NODES:
		val = MAX_NODES_SUPPORTED;
		break;
	case NV_MAX_DIMMS:
		//val = MAX_DIMMS_SUPPORTED;
		val = 8;
		break;
	case NV_MAX_MEMCLK:
		/* Maximum platform supported memclk */
		//val =  200;	/* 200MHz(DDR400) */
		//val =  266;	/* 266MHz(DDR533) */
		//val =  333;	/* 333MHz(DDR667) */
		val =  400;	/* 400MHz(DDR800) */
		break;
	case NV_ECC_CAP:
#if SYSTEM_TYPE == SERVER
		val = 1;	/* memory bus ECC capable */
#else
		val = 0;	/* memory bus ECC not capable */
#endif
		break;
	case NV_4RANKType:
		/* Quad Rank DIMM slot type */
		val = 0;	/* normal */
		//val = 1;	/* R4 (registered DIMMs in AMD server configuration) */
		//val = 2;	/* S4 (Unbuffered SO-DIMMS) */
		break;
	case NV_BYPMAX:
#if   (UMA_SUPPORT == 0)
		val = 4;
#elif  (UMA_SUPPORT == 1)
		val = 7;
#endif
		break;
	case NV_RDWRQBYP:
#if  (UMA_SUPPORT == 0)
		val = 2;
#elif (UMA_SUPPORT == 1)
		val = 3;
#endif
		break;
	case NV_MCTUSRTMGMODE:
		val = 0;	/* Automatic (recommended) */
		//val = 1;	/* Limited */
		//val = 2;	/* Manual */
		break;
	case NV_MemCkVal:
		//val = 0;	/* 200MHz */
		//val = 1;	/* 266MHz */
		val = 2;	/* 333MHz */
		break;
	case NV_BankIntlv:
		/* Bank (chip select) interleaving */
		//val = 0;	/* disabled */
		val = 1;	/* enabled (recommended) */
		break;
	case NV_MemHole:
		//val = 0;	/* Disabled */
		val = 1;	/* Enabled (recommended) */
		break;
	case NV_AllMemClks:
		val = 0;	/* Normal (only to slots that have enabled DIMMs) */
		//val = 1;	/* Enable all memclocks */
		break;
	case NV_SPDCHK_RESTRT:
		val = 0;	/* Exit current node initialization if any DIMM has SPD checksum error */
		//val = 1;	/* Ignore faulty SPD checksum (DIMM will still be disabled), continue current node intialization */
		break;
	case NV_DQSTrainCTL:
		//val = 0;	/*Skip dqs training */
		val = 1;	/* Perform dqs training */
		break;
	case NV_NodeIntlv:
		val = 0;	/* Disabled (recommended) */
		//val = 1;	/* Enable */
		break;
	case NV_BurstLen32:
#if (UMA_SUPPORT == 0)
		val = 0;	/* 64 byte mode */
#elif (UMA_SUPPORT == 1)
		val = 1;	/* 32 byte mode */
#endif
		break;
	case NV_CKE_PDEN:
		//val = 0;	/* Disable */
		val = 1;	/* Enable */
		break;
	case NV_CKE_CTL:
		val = 0;	/* per channel control */
		//val = 1;	/* per chip select control */
		break;
	case NV_CLKHZAltVidC3:
		val = 0;	/* disable */
		//val = 1;	/* enable */
		break;
	case NV_BottomIO:
		val = 0xE0;	/* address bits [31:24] */
		break;
	case NV_BottomUMA:
#if (UMA_SUPPORT == 0)
		val = 0xE0;	/* address bits [31:24] */
#elif (UMA_SUPPORT == 1)
		val = 0xB0;	/* address bits [31:24] */
#endif
		break;
	case NV_ECC:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif
		break;
	case NV_NBECC:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif
		break;
	case NV_ChipKill:
#if (SYSTEM_TYPE == SERVER)
		val = 1;	/* Enable */
#else
		val = 0;	/* Disable */
#endif
		break;
	case NV_ECCRedir:
		val = 0;	/* Disable */
		//val = 1;	/* Enable */
		break;
	case NV_DramBKScrub:
		val = 0x00;	/* Disabled */
		//val = 0x01;	/* 40ns */
		//val = 0x02;	/* 80ns */
		//val = 0x03;	/* 160ns */
		//val = 0x04;	/* 320ns */
		//val = 0x05;	/* 640ns */
		//val = 0x06;	/* 1.28us */
		//val = 0x07;	/* 2.56us */
		//val = 0x08;	/* 5.12us */
		//val = 0x09;	/* 10.2us */
		//val = 0x0a;	/* 20.5us */
		//val = 0x0b;	/* 41us */
		//val = 0x0c;	/* 81.9us */
		//val = 0x0d;	/* 163.8us */
		//val = 0x0e;	/* 327.7us */
		//val = 0x0f;	/* 655.4us */
		//val = 0x10;	/* 1.31ms */
		//val = 0x11;	/* 2.62ms */
		//val = 0x12;	/* 5.24ms */
		//val = 0x13;	/* 10.49ms */
		//val = 0x14;	/* 20.97sms */
		//val = 0x15;	/* 42ms */
		//val = 0x16;	/* 84ms */
		break;
	case NV_L2BKScrub:
		val = 0;	/* Disabled - See L2Scrub in BKDG */
		break;
	case NV_DCBKScrub:
		val = 0;	/* Disabled - See DcacheScrub in BKDG */
		break;
	case NV_CS_SpareCTL:
		val = 0;	/* Disabled */
		//val = 1;	/* Enabled */
		break;
	case NV_SyncOnUnEccEn:
		val = 0;	/* Disabled */
		//val = 1;	/* Enabled */
		break;
	case NV_Unganged:
		/* channel interleave is better performance than ganged mode at this time */
		val = 1;		/* Enabled */
		//val = 0;	/* Disabled */
		break;
	case NV_ChannelIntlv:
		val = 5;	/* Not currently checked in mctchi_d.c */
	/* Bit 0 =     0 - Disable
	 *             1 - Enable
	 * Bits[2:1] = 00b - Address bits 6
	 *             01b - Address bits 1
	 *             10b - Hash*, XOR of address bits [20:16, 6]
	 *             11b - Hash*, XOR of address bits [20:16, 9]
	 */
		break;
	}

	return val;
}


void mctHookAfterDIMMpre(void)
{
}


void mctGet_MaxLoadFreq(struct DCTStatStruc *pDCTstat)
{
	pDCTstat->PresetmaxFreq = 400;
}


void mctAdjustAutoCycTmg(void)
{
}

void mctAdjustAutoCycTmg_D(void)
{
}


void mctHookAfterAutoCycTmg(void)
{
}


void mctGetCS_ExcludeMap(void)
{
}


void mctHookAfterAutoCfg(void)
{
}


void mctHookAfterPSCfg(void)
{
}


void mctHookAfterHTMap(void)
{
}


void mctHookAfterCPU(void)
{
}


void mctSaveDQSSigTmg_D(void)
{
}


void mctGetDQSSigTmg_D(void)
{
}


void mctHookBeforeECC(void)
{
}


void mctHookAfterECC(void)
{
}


void mctInitMemGPIOs_A(void)
{
}


void mctInitMemGPIOs_A_D(void)
{
}


void mctNodeIDDebugPort_D(void)
{
}


void mctWarmReset(void)
{
}

void mctWarmReset_D(void)
{
}


void mctHookBeforeDramInit(void)
{
}


void mctHookAfterDramInit(void)
{
}

static void coreDelay (void);


/* Erratum 350 */
void vErrata350(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	u8 u8Channel;
	u8 u8Receiver;
	u32 u32Addr;
	u8 u8Valid;
	u32 u32DctDev;

	// 1. dummy read for each installed DIMM */
	for (u8Channel = 0; u8Channel < 2; u8Channel++) {
		// This will be 0 for vaild DIMMS, eles 8
		u8Receiver = mct_InitReceiver_D(pDCTstat, u8Channel);

		for (; u8Receiver < 8; u8Receiver += 2) {
			u32Addr = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, u8Channel, u8Receiver, &u8Valid);

			if(!u8Valid) {	/* Address not supported on current CS */
				print_t("vErrata350: Address not supported on current CS\n");
				continue;
			}
			print_t("vErrata350: dummy read \n");
			read32_fs(u32Addr);
		}
	}

	print_t("vErrata350: step 2a\n");

	/* 2. Write 0000_8000h to register F2x[1, 0]9C_xD080F0C. */
	u32DctDev = pDCTstat->dev_dct;
	Set_NB32_index_wait(u32DctDev, 0x098, 0xD080F0C, 0x00008000);
	/*                                                ^--- value
	                                        ^---F2x[1, 0]9C_x0D080F0C, No description in BKDG.
	                                 ^----F2x[1, 0]98 DRAM Controller Additional Data Offset Register */

	if(!pDCTstat->GangedMode) {
		print_t("vErrata350: step 2b\n");
		Set_NB32_index_wait(u32DctDev, 0x198, 0xD080F0C, 0x00008000);
		/*                                                ^--- value
		                                        ^---F2x[1, 0]9C_x0D080F0C, No description in BKDG
		                                ^----F2x[1, 0]98 DRAM Controller Additional Data Offset Register */
	}

	print_t("vErrata350: step 3\n");
	/* 3. Wait at least 300 nanoseconds. */
	coreDelay();

	print_t("vErrata350: step 4\n");
	/* 4. Write 0000_0000h to register F2x[1, 0]9C_xD080F0C. */
	Set_NB32_index_wait(u32DctDev, 0x098, 0xD080F0C, 0x00000000);

	if(!pDCTstat->GangedMode) {
		print_t("vErrata350: step 4b\n");
		Set_NB32_index_wait(u32DctDev, 0x198, 0xD080F0C, 0x00000000);
	}

	print_t("vErrata350: step 5\n");
	/* 5. Wait at least 2 microseconds. */
	coreDelay();

}


void mctHookBeforeAnyTraining(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA)
{
	if (pDCTstatA->LogicalCPUID & AMD_RB_C2) {
		vErrata350(pMCTstat, pDCTstatA);
	}
}

void mctHookAfterAnyTraining(void)
{
}

u32 mctGetLogicalCPUID_D(u8 node)
{
	return mctGetLogicalCPUID(node);
}

u8 mctSetNodeBoundary_D(void)
{
	return 0;
}
