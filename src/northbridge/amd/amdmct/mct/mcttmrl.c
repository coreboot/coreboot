/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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


/*
 * Description: Max Read Latency Training feature for DDR 2 MCT
 */

static u8 CompareMaxRdLatTestPattern_D(u32 pattern_buf, u32 addr);
static u32 GetMaxRdLatTestAddr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel,
				u8 *MaxRcvrEnDly, u8 *valid);
u8 mct_GetStartMaxRdLat_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 Channel,
				u8 DQSRcvEnDly, u32 *Margin);
static void maxRdLatencyTrain_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_setMaxRdLatTrnVal_D(struct DCTStatStruc *pDCTstat, u8 Channel,
					u16 MaxRdLatVal);

/*Warning:  These must be located so they do not cross a logical 16-bit
   segment boundary!*/
static const u32 TestMaxRdLAtPattern_D[] = {
	0x6E0E3FAC, 0x0C3CFF52,
	0x4A688181, 0x49C5B613,
	0x7C780BA6, 0x5C1650E3,
	0x0C4F9D76, 0x0C6753E6,
	0x205535A5, 0xBABFB6CA,
	0x610E6E5F, 0x0C5F1C87,
	0x488493CE, 0x14C9C383,
	0xF5B9A5CD, 0x9CE8F615,

	0xAAD714B5, 0xC38F1B4C,
	0x72ED647C, 0x669F7562,
	0x5233F802, 0x4A898B30,
	0x10A40617, 0x3326B465,
	0x55386E04, 0xC807E3D3,
	0xAB49E193, 0x14B4E63A,
	0x67DF2495, 0xEA517C45,
	0x7624CE51, 0xF8140C51,

	0x4824BD23, 0xB61DD0C9,
	0x072BCFBE, 0xE8F3807D,
	0x919EA373, 0x25E30C47,
	0xFEB12958, 0x4DA80A5A,
	0xE9A0DDF8, 0x792B0076,
	0xE81C73DC, 0xF025B496,
	0x1DB7E627, 0x808594FE,
	0x82668268, 0x655C7783,
};


static u32 SetupMaxRdPattern(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u32 *buffer)
{
	/* 1. Copy the alpha and Beta patterns from ROM to Cache,
	 *    aligning on 16 byte boundary
	 * 2. Set the ptr to Cacheable copy in DCTStatstruc.PtrPatternBufA
	 *    for Alpha
	 * 3. Set the ptr to Cacheable copy in DCTStatstruc.PtrPatternBufB
	 *    for Beta
	 */

	u32 *buf;
	u8 i;

	buf = (u32 *)(((u32)buffer + 0x10) & (0xfffffff0));

	for(i = 0; i < (16 * 3); i++) {
		buf[i] = TestMaxRdLAtPattern_D[i];
	}

	return (u32)buf;

}


void TrainMaxReadLatency_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;

	for(Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if(!pDCTstat->NodePresent)
			break;

		if(pDCTstat->DCTSysLimit)
			maxRdLatencyTrain_D(pMCTstat, pDCTstat);
	}
}


static void maxRdLatencyTrain_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 Channel;
	u32 TestAddr0;
	u8 _DisableDramECC = 0, _Wrap32Dis = 0, _SSE2 = 0;
	u16 MaxRdLatDly;
	u8 RcvrEnDly = 0;
	u32 PatternBuffer[60];	// FIXME: why not 48 + 4
	u32 Margin;
	u32 addr;
	u32 cr4;
	u32 lo, hi;

	u8 valid;
	u32 pattern_buf;

	cr4 = read_cr4();
	if(cr4 & (1<<9)) {		/* save the old value */
		_SSE2 = 1;
	}
	cr4 |= (1<<9);			/* OSFXSR enable SSE2 */
	write_cr4(cr4);

	addr = HWCR;
	_RDMSR(addr, &lo, &hi);
	if(lo & (1<<17)) {		/* save the old value */
		_Wrap32Dis = 1;
	}
	lo |= (1<<17);			/* HWCR.wrap32dis */
	lo &= ~(1<<15);			/* SSEDIS */
	/* Setting wrap32dis allows 64-bit memory references in
	   real mode */
	_WRMSR(addr, lo, hi);

	_DisableDramECC = mct_DisableDimmEccEn_D(pMCTstat, pDCTstat);

	pattern_buf = SetupMaxRdPattern(pMCTstat, pDCTstat, PatternBuffer);

	for (Channel = 0; Channel < 2; Channel++) {
		print_debug_dqs("\tMaxRdLatencyTrain51: Channel ",Channel, 1);
		pDCTstat->Channel = Channel;

		if( (pDCTstat->Status & (1 << SB_128bitmode)) && Channel)
			break;		/*if ganged mode, skip DCT 1 */

		TestAddr0 = GetMaxRdLatTestAddr_D(pMCTstat, pDCTstat, Channel, &RcvrEnDly,	 &valid);
		if(!valid)	/* Address not supported on current CS */
			continue;
		/* rank 1 of DIMM, testpattern 0 */
		WriteMaxRdLat1CLTestPattern_D(pattern_buf, TestAddr0);

		MaxRdLatDly = mct_GetStartMaxRdLat_D(pMCTstat, pDCTstat, Channel, RcvrEnDly, &Margin);
		print_debug_dqs("\tMaxRdLatencyTrain52:  MaxRdLatDly start ", MaxRdLatDly, 2);
		print_debug_dqs("\tMaxRdLatencyTrain52:  MaxRdLatDly Margin ", Margin, 2);
		while(MaxRdLatDly < MAX_RD_LAT) {	/* sweep Delay value here */
			mct_setMaxRdLatTrnVal_D(pDCTstat, Channel, MaxRdLatDly);
			ReadMaxRdLat1CLTestPattern_D(TestAddr0);
			if( CompareMaxRdLatTestPattern_D(pattern_buf, TestAddr0) == DQS_PASS)
				break;
			SetTargetWTIO_D(TestAddr0);
			FlushMaxRdLatTestPattern_D(TestAddr0);
			ResetTargetWTIO_D();
			MaxRdLatDly++;
		}
		print_debug_dqs("\tMaxRdLatencyTrain53:  MaxRdLatDly end ", MaxRdLatDly, 2);
		mct_setMaxRdLatTrnVal_D(pDCTstat, Channel, MaxRdLatDly + Margin);
	}

	if(_DisableDramECC) {
		mct_EnableDimmEccEn_D(pMCTstat, pDCTstat, _DisableDramECC);
	}

	if(!_Wrap32Dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);	/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
	if(!_SSE2){
		cr4 = read_cr4();
		cr4 &= ~(1<<9);	/* restore cr4.OSFXSR */
		write_cr4(cr4);
	}

#if DQS_TRAIN_DEBUG > 0
	{
		u8 Channel;
		print_debug("maxRdLatencyTrain: CH_MaxRdLat:\n");
		for(Channel = 0; Channel<2; Channel++) {
			print_debug("Channel:"); print_debug_hex8(Channel);
			print_debug(": ");
			print_debug_hex8( pDCTstat->CH_MaxRdLat[Channel] );
			print_debug("\n");
		}
	}
#endif

}

static void mct_setMaxRdLatTrnVal_D(struct DCTStatStruc *pDCTstat,
					u8 Channel, u16 MaxRdLatVal)
{
	u8 i;
	u32 reg;
	u32 dev;
	u32 val;

	if (pDCTstat->GangedMode) {
		Channel = 0; // for safe
		for (i=0; i<2; i++)
			pDCTstat->CH_MaxRdLat[i] = MaxRdLatVal;
	} else {
		pDCTstat->CH_MaxRdLat[Channel] = MaxRdLatVal;
	}

	dev = pDCTstat->dev_dct;
	reg = 0x78 + Channel * 0x100;
	val = Get_NB32(dev, reg);
	val &= ~(0x3ff<<22);
	val |= MaxRdLatVal<<22;
	/* program MaxRdLatency to correspond with current delay */
	Set_NB32(dev, reg, val);

}


static u8 CompareMaxRdLatTestPattern_D(u32 pattern_buf, u32 addr)
{
	/* Compare only the first beat of data.  Since target addrs are cache
	 * line aligned, the Channel parameter is used to determine which cache
	 * QW to compare.
	 */

	u32 *test_buf = (u32 *)pattern_buf;
	u32 addr_lo;
	u32 val, val_test;
	int i;
	u8 ret = DQS_PASS;

	SetUpperFSbase(addr);
	addr_lo = addr<<8;

	_EXECFENCE;
	for (i=0; i<(16*3); i++) {
		val = read32_fs(addr_lo);
		val_test = test_buf[i];

		print_debug_dqs_pair("\t\t\t\t\t\ttest_buf = ", (u32)test_buf, " value = ", val_test, 5);
		print_debug_dqs_pair("\t\t\t\t\t\ttaddr_lo = ", addr_lo, " value = ", val, 5);
		if(val != val_test) {
			ret = DQS_FAIL;
			break;
		}
		addr_lo += 4;
	}

	return ret;
}

static u32 GetMaxRdLatTestAddr_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat,
					u8 Channel, u8 *MaxRcvrEnDly,
					u8 *valid)
{
	u8 Max = 0;

	u8 Channel_Max = 0;
	u8 d;
	u8 d_Max = 0;

	u8 Byte;
	u32 TestAddr0 = 0;
	u8 ch, ch_start, ch_end;
	u8 bn;

	bn = 8;

	if(pDCTstat->Status & (1 << SB_128bitmode)) {
		ch_start = 0;
		ch_end = 2;
	} else {
		ch_start = Channel;
		ch_end = Channel + 1;
	}

	*valid = 0;

	for(ch = ch_start; ch < ch_end; ch++) {
		for(d=0; d<4; d++) {
			for(Byte = 0; Byte<bn; Byte++) {
				u8 tmp;
				tmp = pDCTstat->CH_D_B_RCVRDLY[ch][d][Byte];
				if(tmp>Max) {
					Max = tmp;
					Channel_Max = Channel;
					d_Max = d;
				}
			}
		}
	}

	if(mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, Channel_Max, d_Max << 1))  {
		TestAddr0 = mct_GetMCTSysAddr_D(pMCTstat, pDCTstat, Channel_Max, d_Max << 1, valid);
	}

	if(*valid)
		*MaxRcvrEnDly = Max;

	return TestAddr0;

}

u8 mct_GetStartMaxRdLat_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat,
				u8 Channel, u8 DQSRcvEnDly, u32 *Margin)
{
	u32 SubTotal;
	u32 val;
	u32 valx;
	u32 valxx;
	u32 index_reg;
	u32 reg_off;
	u32 dev;

	if(pDCTstat->GangedMode)
		Channel =  0;

	index_reg = 0x98 + 0x100 * Channel;

	reg_off = 0x100 * Channel;
	dev = pDCTstat->dev_dct;

	/* Multiply the CAS Latency by two to get a number of 1/2 MEMCLKs units.*/
	val = Get_NB32(dev, 0x88 + reg_off);
	SubTotal = ((val & 0x0f) + 1) << 1;	/* SubTotal is 1/2 Memclk unit */

	/* If registered DIMMs are being used then add 1 MEMCLK to the sub-total*/
	val = Get_NB32(dev, 0x90 + reg_off);
	if(!(val & (1 << UnBuffDimm)))
		SubTotal += 2;

	/*If the address prelaunch is setup for 1/2 MEMCLKs then add 1,
	 *  else add 2 to the sub-total. if (AddrCmdSetup || CsOdtSetup
	 *  || CkeSetup) then K := K + 2; */
	val = Get_NB32_index_wait(dev, index_reg, 0x04);
	if(!(val & 0x00202020))
		SubTotal += 1;
	else
		SubTotal += 2;

	/* If the F2x[1, 0]78[RdPtrInit] field is 4, 5, 6 or 7 MEMCLKs,
	 *  then add 4, 3, 2, or 1 MEMCLKs, respectively to the sub-total. */
	val = Get_NB32(dev, 0x78 + reg_off);
	SubTotal += 8 - (val & 0x0f);

	/* Convert bits 7-5 (also referred to as the course delay) of the current
	 * (or worst case) DQS receiver enable delay to 1/2 MEMCLKs units,
	 * rounding up, and add this to the sub-total. */
	SubTotal += DQSRcvEnDly >> 5;	/*BOZO-no rounding up */

	SubTotal <<= 1;			/*scale 1/2 MemClk to 1/4 MemClk */

	/* Convert the sub-total (in 1/2 MEMCLKs) to northbridge clocks (NCLKs)
	 * as follows (assuming DDR400 and assuming that no P-state or link speed
	 * changes have occurred). */

	/*New formula:
	SubTotal *= 3*(Fn2xD4[NBFid]+4)/(3+Fn2x94[MemClkFreq])/2 */
	val = Get_NB32(dev, 0x94 + reg_off);
	/* SubTotal div 4 to scale 1/4 MemClk back to MemClk */
	val &= 7;
	if (val == 4) {
		val++;		/* adjust for DDR2-1066 */
	}
	valx = (val + 3) << 2;	/* SubTotal div 4 to scale 1/4 MemClk back to MemClk */


	val = Get_NB32(pDCTstat->dev_nbmisc, 0xD4);
	val = ((val & 0x1f) + 4 ) * 3;

	/* Calculate 1 MemClk + 1 NCLK delay in NCLKs for margin */
	valxx = val << 2;
	valxx /= valx;
	if (valxx % valx)
		valxx++;	/* round up */
	valxx++;		/* add 1NCLK */
	*Margin = valxx;	/* one MemClk delay in NCLKs and one additional NCLK */

	val *= SubTotal;

	val /= valx;
	if (val % valx)
		val++;		/* round up */



	return val;
}


