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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Call-backs */
#include <delay.h>

static u16 mctGet_NVbits(u8 index)
{
	u16 val = 0;

	switch (index) {
	case NV_PACK_TYPE:
#if CONFIG_CPU_SOCKET_TYPE == 0x10	/* Socket F */
		val = 0;
#elif CONFIG_CPU_SOCKET_TYPE == 0x11	/* AM3 */
		val = 1;
#elif CONFIG_CPU_SOCKET_TYPE == 0x13	/* ASB2 */
		val = 4;
//#elif SYSTEM_TYPE == MOBILE
//		val = 2;
#endif
		break;
	case NV_MAX_NODES:
		val = MAX_NODES_SUPPORTED;
		break;
	case NV_MAX_DIMMS:
		val = MAX_DIMMS_SUPPORTED;
		//val = 8;
		break;
	case NV_MAX_MEMCLK:
		/* Maximum platform supported memclk */
		//val =  200;	/* 200MHz(DDR400) */
		//val =  266;	/* 266MHz(DDR533) */
		//val =  333;	/* 333MHz(DDR667) */
		val =  MEM_MAX_LOAD_FREQ;;	/* 400MHz(DDR800) */
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
#if !CONFIG_GFXUMA
		val = 4;
#elif  CONFIG_GFXUMA
		val = 7;
#endif
		break;
	case NV_RDWRQBYP:
#if !CONFIG_GFXUMA
		val = 2;
#elif CONFIG_GFXUMA
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
#if !CONFIG_GFXUMA
		val = 0;	/* 64 byte mode */
#elif CONFIG_GFXUMA
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
#if !CONFIG_GFXUMA
		val = 0xE0;	/* address bits [31:24] */
#elif CONFIG_GFXUMA
		val = 0xC0;	/* address bits [31:24] */
#endif
		break;
	case NV_BottomUMA:
#if !CONFIG_GFXUMA
		val = 0xE0;	/* address bits [31:24] */
#elif CONFIG_GFXUMA
		val = 0xC0;	/* address bits [31:24] */
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


static void mctHookAfterDIMMpre(void)
{
}


static void mctGet_MaxLoadFreq(struct DCTStatStruc *pDCTstat)
{
	pDCTstat->PresetmaxFreq = MEM_MAX_LOAD_FREQ;
}

#ifdef UNUSED_CODE
static void mctAdjustAutoCycTmg(void)
{
}
#endif


static void mctAdjustAutoCycTmg_D(void)
{
}


static void mctHookAfterAutoCycTmg(void)
{
}


static void mctGetCS_ExcludeMap(void)
{
}


static void mctHookAfterAutoCfg(void)
{
}


static void mctHookAfterPSCfg(void)
{
}


static void mctHookAfterHTMap(void)
{
}


static void mctHookAfterCPU(void)
{
}


static void mctSaveDQSSigTmg_D(void)
{
}


static void mctGetDQSSigTmg_D(void)
{
}


static void mctHookBeforeECC(void)
{
}


static void mctHookAfterECC(void)
{
}

#ifdef UNUSED_CODE
static void mctInitMemGPIOs_A(void)
{
}
#endif


static void mctInitMemGPIOs_A_D(void)
{
}


static void mctNodeIDDebugPort_D(void)
{
}


#ifdef UNUSED_CODE
static void mctWarmReset(void)
{
}
#endif


static void mctWarmReset_D(void)
{
}


static void mctHookBeforeDramInit(void)
{
}


static void mctHookAfterDramInit(void)
{
}

#if (CONFIG_DIMM_SUPPORT & 0x000F)==0x0005 /* AMD_FAM10_DDR3 */
static void coreDelay(u32 microseconds)
{
	msr_t now;
	msr_t end;
	u32 cycles;

	/* delay ~40us
	   This seems like a hack to me...
	   It would be nice to have a central delay function. */

	cycles = (microseconds * 100) << 3;  /* x8 (number of 1.25ns ticks) */

        if (!(rdmsr(HWCR).lo & TSC_FREQ_SEL_MASK)) {
            msr_t pstate_msr = rdmsr(CUR_PSTATE_MSR);
            if (!(rdmsr(0xC0010064+pstate_msr.lo).lo & NB_DID_M_ON)) {
	      cycles = cycles <<1; // half freq, double cycles
	    }
	} // else should we keep p0 freq at the time of setting TSC_FREQ_SEL_MASK somewhere and check it here ?

	now = rdmsr(TSC_MSR);
        // avoid overflow when called near 2^32 ticks ~ 5.3 s boundaries
	if (0xffffffff - cycles >= now.lo ) {
	  end.hi =  now.hi;
          end.lo = now.lo + cycles;
	} else {
          end.hi = now.hi +1; //
          end.lo = cycles - (1+(0xffffffff - now.lo));
	}
	do {
          now = rdmsr(TSC_MSR);
        } while ((now.hi < end.hi) || ((now.hi == end.hi) && (now.lo < end.lo)));
}

/* Erratum 350 */
static void vErrata350(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
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
	coreDelay(1);

	print_t("vErrata350: step 4\n");
	/* 4. Write 0000_0000h to register F2x[1, 0]9C_xD080F0C. */
	Set_NB32_index_wait(u32DctDev, 0x098, 0xD080F0C, 0x00000000);

	if(!pDCTstat->GangedMode) {
		print_t("vErrata350: step 4b\n");
		Set_NB32_index_wait(u32DctDev, 0x198, 0xD080F0C, 0x00000000);
	}

	print_t("vErrata350: step 5\n");
	/* 5. Wait at least 2 microseconds. */
	coreDelay(2);

}

static void vErratum372(struct DCTStatStruc *pDCTstat)
{
        msr_t msr = rdmsr(NB_CFG_MSR);

        int  nbPstate1supported = ! (msr.hi && (1 << (NB_GfxNbPstateDis -32))) ;

        // is this the right way to check for NB pstate 1 or DDR3-1333 ?
        if (((pDCTstat->PresetmaxFreq==1333)||(nbPstate1supported))
            &&(!pDCTstat->GangedMode)) {
           	/* DisableCf8ExtCfg */
        	msr.hi &= ~(3 << (51 - 32));
        	wrmsr(NB_CFG_MSR, msr);
        }
}

static void vErratum414(struct DCTStatStruc *pDCTstat)
{
     int dct=0;
    for(; dct < 2 ; dct++)
    {
        int dRAMConfigHi = Get_NB32(pDCTstat->dev_dct,0x94 + (0x100 * dct));
        int powerDown =  dRAMConfigHi && (1 << PowerDownEn ) ;
        int ddr3 = dRAMConfigHi && (1 << Ddr3Mode ) ;
        int dRAMMRS = Get_NB32(pDCTstat->dev_dct,0x84 + (0x100 * dct));
        int pchgPDModeSel = dRAMMRS && (1 << PchgPDModeSel ) ;
	if (powerDown && ddr3 && pchgPDModeSel )
	{
	  Set_NB32(pDCTstat->dev_dct,0x84 + (0x100 * dct), dRAMMRS & ~(1 << PchgPDModeSel) );
	}
    }
}
#endif


static void mctHookBeforeAnyTraining(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA)
{
#if (CONFIG_DIMM_SUPPORT & 0x000F)==0x0005 /* AMD_FAM10_DDR3 */
  /* FIXME :  as of 25.6.2010 errata 350 and 372 should apply to  ((RB|BL|DA)-C[23])|(HY-D[01])|(PH-E0) but I don't find constants for all of them */
	if (pDCTstatA->LogicalCPUID & AMD_DRBH_Cx) {
		vErrata350(pMCTstat, pDCTstatA);
		vErratum372(pDCTstatA);
		vErratum414(pDCTstatA);
	}
#endif
}

#if (CONFIG_DIMM_SUPPORT & 0x000F)==0x0005 /* AMD_FAM10_DDR3 */
static u32 mct_AdjustSPDTimings(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA, u32 val)
{
	if (pDCTstatA->LogicalCPUID & AMD_DR_Bx) {
		if (pDCTstatA->Status & (1 << SB_Registered)) {
			val ++;
		}
	}
	return val;
}
#endif

static void mctHookAfterAnyTraining(void)
{
}

static u32 mctGetLogicalCPUID_D(u8 node)
{
	return mctGetLogicalCPUID(node);
}

#if (CONFIG_DIMM_SUPPORT & 0x000F)!=0x0005 /* not needed for AMD_FAM10_DDR3 */
static u8 mctSetNodeBoundary_D(void)
{
	return 0;
}
#endif
