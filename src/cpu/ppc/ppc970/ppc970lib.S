
#include "ppc970.h"

/*----------------------------------------------------------------------------+
| PpcMflr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMflr)
	mflr	r3
	blr
	function_epilog(ppcMflr)

/*----------------------------------------------------------------------------+
| Inbyte
+----------------------------------------------------------------------------*/
	function_prolog(inbyte)
	lbz	r3,0x0000(r3)
	sync
	blr
	function_epilog(inbyte)

/*----------------------------------------------------------------------------+
| Outbyte
+----------------------------------------------------------------------------*/
	function_prolog(outbyte)
	stb	r4,0x0000(r3)
	sync
	blr
	function_epilog(outbyte)

/*----------------------------------------------------------------------------+
| Inhalf
+----------------------------------------------------------------------------*/
	function_prolog(inhalf)
	lhz	r3,0x0000(r3)
	sync
	blr
	function_epilog(inhalf)

/*----------------------------------------------------------------------------+
| Outhalf
+----------------------------------------------------------------------------*/
	function_prolog(outhalf)
	sth	r4,0x0000(r3)
	sync
	blr
	function_epilog(outhalf)

/*----------------------------------------------------------------------------+
| Inhalf_brx (Load halfword byte-reverse indexed)
+----------------------------------------------------------------------------*/
        function_prolog(inhalf_brx)
        lhbrx   r3,r0,r3
        sync
        blr
        function_epilog(inhalf_brx)

/*----------------------------------------------------------------------------+
| Outhalf_brx (Store halfword byte-reverse indexed)
+----------------------------------------------------------------------------*/
	function_prolog(outhalf_brx)
	sthbrx  r4,r0,r3
	sync
	blr
	function_epilog(outhalf_brx)

/*----------------------------------------------------------------------------+
| Inword
+----------------------------------------------------------------------------*/
	function_prolog(inword)
	ld	r3,0x0000(r3)
	sync
	blr
	function_epilog(inword)

/*----------------------------------------------------------------------------+
| Outword
+----------------------------------------------------------------------------*/
	function_prolog(outword)
	std	r4,0x0000(r3)
	sync
	blr
	function_epilog(outword)

/*----------------------------------------------------------------------------+
| Inint
+----------------------------------------------------------------------------*/
	function_prolog(inint)
	lwz	r3,0x0000(r3)
	sync
	blr
	function_epilog(inint)

/*----------------------------------------------------------------------------+
| Outint
+----------------------------------------------------------------------------*/
	function_prolog(outint)
	stw	r4,0x0000(r3)
	sync
	blr
	function_epilog(outint)

/*----------------------------------------------------------------------------+
| Inint_brx  (Load word byte-reverse indexed)
+----------------------------------------------------------------------------*/
	function_prolog(inint_brx)
	lwbrx   r3,r0,r3
	sync
	blr
	function_epilog(inint_brx)

/*----------------------------------------------------------------------------+
| Outint_brx (Store word byte-reverse indexed)
+----------------------------------------------------------------------------*/
	function_prolog(outint_brx)
	stwbrx  r4,r0,r3
	sync
	blr
	function_epilog(outint_brx)

/*----------------------------------------------------------------------------+
| PpcDflush.  Assume addresses 0-2MB are cachable.  Do a series of loads to 
| fill the L2 using a memory range twice as large as the L2 in case a line
| between 0-2MB is dirty in the L2 to start.
+----------------------------------------------------------------------------*/
	function_prolog(ppcDflush)
	mfmsr   r9
	rlwinm  r8,r9,0,17,15
	mtmsrd  r8,1
	isync
        /*--------------------------------------------------------------------+
        | Back to the initial start address.
        +--------------------------------------------------------------------*/
        addi    r3,r0,0x0000
        addis   r4,r0,0x0000
        /*--------------------------------------------------------------------+
        | 2x number of blocks in 512KB L2 cache.
        +--------------------------------------------------------------------*/
        ori     r4,r4,0x2000
	mtctr	r4
..fl:   lwz     r6,0x0(r3)
	addi    r3,r3,128
	bdnz	..fl
	sync
	/*--------------------------------------------------------------------+
	| Now flush the last lines.
	+--------------------------------------------------------------------*/
        addis   r3,r0,0x0008
        ori     r4,r4,0x1000
	mtctr	r4
..fl1:	dcbf	r0,r3
	addi	r3,r3,123
	bdnz	..fl1
	/*--------------------------------------------------------------------+
	| No dirty lines should exist in the L2 at this point.
	+--------------------------------------------------------------------*/
	mtmsrd	r9,1
	isync
	blr
	function_epilog(ppcDflush)

/*----------------------------------------------------------------------------+
| PpcDcbz_area (dcbz: data cache block set to zero).  Although the cache line
| in L2 is 128 bytes the dcbz instruction will only zero 32 bytes when HID5
| bit 56 is set to 0.  This function will work with HID5 bit 56 set to 0 or 1.
+----------------------------------------------------------------------------*/
        function_prolog(ppcDcbz_area)
        rlwinm. r5,r4,0,27,31
        rlwinm  r5,r4,27,5,31
        beq     ..d_ran
        addi    r5,r5,0x0001
..d_ran:mtctr   r5
..d_ag: dcbz    r0,r3
        addi    r3,r3,32
        bdnz    ..d_ag
        blr
        function_epilog(ppcDcbz_area)

/*----------------------------------------------------------------------------+
| PpcTlbsync (tlbsync: TLB Synchronize)
+----------------------------------------------------------------------------*/
	function_prolog(ppcTlbsync)
	tlbsync
	blr
	function_epilog(ppcTlbsync)

/*----------------------------------------------------------------------------+
| PpcTlbie (tlbie: TLB Invalidate Entry)
+----------------------------------------------------------------------------*/
	function_prolog(ppcTlbie)
	cmpi	cr0,1,r4,0x0000
	bne	..tlp
	tlbie   r3,0
	blr
..tlp:  tlbie   r3,1
	blr
	function_epilog(ppcTlbie)

/*----------------------------------------------------------------------------+
| PpcAbend
+----------------------------------------------------------------------------*/
	function_prolog(ppcAbend)
	.long  0x00000000
	function_epilog(ppcAbend)

/*----------------------------------------------------------------------------+
| PpcAndMsr
+----------------------------------------------------------------------------*/
	function_prolog(ppcAndMsr)
	mfmsr   r6
	and     r7,r6,r3
	mtmsrd  r7,0
	isync
	ori     r3,r6,0x000
	blr
	function_epilog(ppcAndMsr)

/*----------------------------------------------------------------------------+
| PpcCntlzw  (cntlzw: count leading zeros word)
+----------------------------------------------------------------------------*/
	function_prolog(ppcCntlzw)
	cntlzw  r3,r3
	blr
	function_epilog(ppcCntlzw)

/*----------------------------------------------------------------------------+
| PpcCntlzd  (cntlzd: count leading zeros double word)
+----------------------------------------------------------------------------*/
	function_prolog(ppcCntlzd)
	cntlzd  r3,r3
	blr
	function_epilog(ppcCntlzd)

/*----------------------------------------------------------------------------+
| PpcDcbf (dcbf: data cache block flush)
+----------------------------------------------------------------------------*/
	function_prolog(ppcDcbf)
	dcbf  	r0,r3
	blr
	function_epilog(ppcDcbf)

/*----------------------------------------------------------------------------+
| PpcDcbst  (dcbst: data cache block touch for store)
+----------------------------------------------------------------------------*/
	function_prolog(ppcDcbst)
	dcbst  	r0,r3
	blr
	function_epilog(ppcDcbst)

/*----------------------------------------------------------------------------+
| PpcDcbz  (dcbz: data cache block set to zero)
+----------------------------------------------------------------------------*/
	function_prolog(ppcDcbz)
	dcbz  	r0,r3
	blr
	function_epilog(ppcDcbz)

/*----------------------------------------------------------------------------+
| PpcHalt  (3 nop instructions + branch)
+----------------------------------------------------------------------------*/
	function_prolog(ppcHalt)
	ori	r0,r0,0x0000  
	b	.ppcHalt
	function_epilog(ppcHalt)

/*----------------------------------------------------------------------------+
| PpcIcbi (icbi: instruction cache block invalidate)
+----------------------------------------------------------------------------*/
	function_prolog(ppcIcbi)
	icbi  	r0,r3
	blr
	function_epilog(ppcIcbi)

/*----------------------------------------------------------------------------+
| PpcIsync  (isync: instruction synchronize)
+----------------------------------------------------------------------------*/
	function_prolog(ppcIsync)
	isync
	blr
	function_epilog(ppcIsync)

/*----------------------------------------------------------------------------+
| PpcMfgpr1
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfgpr1)
        addi    r3,r1,0x0000
	blr
	function_epilog(ppcMfgpr1)

/*----------------------------------------------------------------------------+
| PpcMfgpr2
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfgpr2)
        addi    r3,r2,0x0000
	blr
	function_epilog(ppcMfgpr2)

/*----------------------------------------------------------------------------+
| PpcMtmsr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMtmsr)
        mtmsrd	r3,0
	isync
	blr
	function_epilog(ppcMtmsr)

/*----------------------------------------------------------------------------+
| PpcMfmsr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfmsr)
        mfmsr   r3
	blr
	function_epilog(ppcMfmsr)

/*----------------------------------------------------------------------------+
| PpcOrMsr
+----------------------------------------------------------------------------*/
	function_prolog(ppcOrMsr)
	mfmsr   r6
	or      r7,r6,r3
	mtmsrd  r7,0
	isync
	ori     r3,r6,0x000
	blr
	function_epilog(ppcOrMsr)

/*----------------------------------------------------------------------------+
| PpcSync
+----------------------------------------------------------------------------*/
	function_prolog(ppcSync)
	sync
	blr
	function_epilog(ppcSync)

/*----------------------------------------------------------------------------+
| PpcEieio
+----------------------------------------------------------------------------*/
	function_prolog(ppcEieio)
	eieio
	blr
	function_epilog(ppcEieio)

/*----------------------------------------------------------------------------+
| PpcMthid0
+----------------------------------------------------------------------------*/
	function_prolog(ppcMthid0)
	sync
	mtspr	SPR_HID0,r3
	mfspr	r3,SPR_HID0
	mfspr	r3,SPR_HID0
	mfspr	r3,SPR_HID0
	mfspr	r3,SPR_HID0
	mfspr	r3,SPR_HID0
	mfspr	r3,SPR_HID0
	blr
	function_epilog(ppcMthid0)

/*----------------------------------------------------------------------------+
| PpcMthid1
+----------------------------------------------------------------------------*/
	function_prolog(ppcMthid1)
	mtspr	SPR_HID1,r3
	mtspr	SPR_HID1,r3
	isync
	blr
	function_epilog(ppcMthid1)

/*----------------------------------------------------------------------------+
| PpcMthid4
+----------------------------------------------------------------------------*/
	function_prolog(ppcMthid4)
	sync
	mtspr	SPR_HID4,r3
	isync
	blr
	function_epilog(ppcMthid4)

/*----------------------------------------------------------------------------+
| PpcMthid5
+----------------------------------------------------------------------------*/
	function_prolog(ppcMthid5)
	mtspr	SPR_HID5,r3
	blr
	function_epilog(ppcMthid5)

/*----------------------------------------------------------------------------+
| PpcMftb
+----------------------------------------------------------------------------*/
	function_prolog(ppcMftb)
	mfspr	r6,tblr
	std	r6,0x0000(r3)
	blr
	function_epilog(ppcMftb)

/*----------------------------------------------------------------------------+
| PpcMttb
+----------------------------------------------------------------------------*/
	function_prolog(ppcMttb)
	mfmsr 	r7
	lwz 	r6,0x0000(r3)
	rlwinm	r8,r7,0,17,15
	mtmsrd	r8,1
	ori	r5,r6,0x0000
	sradi	r6,r6,32
	addi	r4,r0,0x0000
	rldicl	r6,r6,0,32
	mtspr	SPR_TBL_WRITE,r4
	mtspr	SPR_TBU_WRITE,r6
	mtspr	SPR_TBL_WRITE,r5
	mtmsrd	r7,1
	blr
	function_epilog(ppcMttb)

/*----------------------------------------------------------------------------+
| PpcMtspr_any
+----------------------------------------------------------------------------*/
	function_prolog(ppcMtspr_any)
        rlwinm	r3,r3,3,19,29
	addi	r3,r3,0x0010
	mflr	r6
	bl	..sp_get_lr
..sp_get_lr:
	mflr	r5
	add 	r5,r5,r3
	mtlr	r5
	blr
	mtspr   0x000,r4
	b       ..ppcMtspr_any_end
	mtspr   0x001,r4
	b       ..ppcMtspr_any_end
	mtspr   0x002,r4
	b       ..ppcMtspr_any_end
	mtspr   0x003,r4
	b       ..ppcMtspr_any_end
	mtspr   0x004,r4
	b       ..ppcMtspr_any_end
	mtspr   0x005,r4
	b       ..ppcMtspr_any_end
	mtspr   0x006,r4
	b       ..ppcMtspr_any_end
	mtspr   0x007,r4
	b       ..ppcMtspr_any_end
	mtspr   0x008,r4
	b       ..ppcMtspr_any_end
	mtspr   0x009,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x00f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x010,r4
	b       ..ppcMtspr_any_end
	mtspr   0x011,r4
	b       ..ppcMtspr_any_end
	mtspr   0x012,r4
	b       ..ppcMtspr_any_end
	mtspr   0x013,r4
	b       ..ppcMtspr_any_end
	mtspr   0x014,r4
	b       ..ppcMtspr_any_end
	mtspr   0x015,r4
	b       ..ppcMtspr_any_end
	mtspr   0x016,r4
	b       ..ppcMtspr_any_end
	mtspr   0x017,r4
	b       ..ppcMtspr_any_end
	mtspr   0x018,r4
	b       ..ppcMtspr_any_end
	mtspr   0x019,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x01f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x020,r4
	b       ..ppcMtspr_any_end
	mtspr   0x021,r4
	b       ..ppcMtspr_any_end
	mtspr   0x022,r4
	b       ..ppcMtspr_any_end
	mtspr   0x023,r4
	b       ..ppcMtspr_any_end
	mtspr   0x024,r4
	b       ..ppcMtspr_any_end
	mtspr   0x025,r4
	b       ..ppcMtspr_any_end
	mtspr   0x026,r4
	b       ..ppcMtspr_any_end
	mtspr   0x027,r4
	b       ..ppcMtspr_any_end
	mtspr   0x028,r4
	b       ..ppcMtspr_any_end
	mtspr   0x029,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x02f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x030,r4
	b       ..ppcMtspr_any_end
	mtspr   0x031,r4
	b       ..ppcMtspr_any_end
	mtspr   0x032,r4
	b       ..ppcMtspr_any_end
	mtspr   0x033,r4
	b       ..ppcMtspr_any_end
	mtspr   0x034,r4
	b       ..ppcMtspr_any_end
	mtspr   0x035,r4
	b       ..ppcMtspr_any_end
	mtspr   0x036,r4
	b       ..ppcMtspr_any_end
	mtspr   0x037,r4
	b       ..ppcMtspr_any_end
	mtspr   0x038,r4
	b       ..ppcMtspr_any_end
	mtspr   0x039,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x03f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x040,r4
	b       ..ppcMtspr_any_end
	mtspr   0x041,r4
	b       ..ppcMtspr_any_end
	mtspr   0x042,r4
	b       ..ppcMtspr_any_end
	mtspr   0x043,r4
	b       ..ppcMtspr_any_end
	mtspr   0x044,r4
	b       ..ppcMtspr_any_end
	mtspr   0x045,r4
	b       ..ppcMtspr_any_end
	mtspr   0x046,r4
	b       ..ppcMtspr_any_end
	mtspr   0x047,r4
	b       ..ppcMtspr_any_end
	mtspr   0x048,r4
	b       ..ppcMtspr_any_end
	mtspr   0x049,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x04f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x050,r4
	b       ..ppcMtspr_any_end
	mtspr   0x051,r4
	b       ..ppcMtspr_any_end
	mtspr   0x052,r4
	b       ..ppcMtspr_any_end
	mtspr   0x053,r4
	b       ..ppcMtspr_any_end
	mtspr   0x054,r4
	b       ..ppcMtspr_any_end
	mtspr   0x055,r4
	b       ..ppcMtspr_any_end
	mtspr   0x056,r4
	b       ..ppcMtspr_any_end
	mtspr   0x057,r4
	b       ..ppcMtspr_any_end
	mtspr   0x058,r4
	b       ..ppcMtspr_any_end
	mtspr   0x059,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x05f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x060,r4
	b       ..ppcMtspr_any_end
	mtspr   0x061,r4
	b       ..ppcMtspr_any_end
	mtspr   0x062,r4
	b       ..ppcMtspr_any_end
	mtspr   0x063,r4
	b       ..ppcMtspr_any_end
	mtspr   0x064,r4
	b       ..ppcMtspr_any_end
	mtspr   0x065,r4
	b       ..ppcMtspr_any_end
	mtspr   0x066,r4
	b       ..ppcMtspr_any_end
	mtspr   0x067,r4
	b       ..ppcMtspr_any_end
	mtspr   0x068,r4
	b       ..ppcMtspr_any_end
	mtspr   0x069,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x06f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x070,r4
	b       ..ppcMtspr_any_end
	mtspr   0x071,r4
	b       ..ppcMtspr_any_end
	mtspr   0x072,r4
	b       ..ppcMtspr_any_end
	mtspr   0x073,r4
	b       ..ppcMtspr_any_end
	mtspr   0x074,r4
	b       ..ppcMtspr_any_end
	mtspr   0x075,r4
	b       ..ppcMtspr_any_end
	mtspr   0x076,r4
	b       ..ppcMtspr_any_end
	mtspr   0x077,r4
	b       ..ppcMtspr_any_end
	mtspr   0x078,r4
	b       ..ppcMtspr_any_end
	mtspr   0x079,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x07f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x080,r4
	b       ..ppcMtspr_any_end
	mtspr   0x081,r4
	b       ..ppcMtspr_any_end
	mtspr   0x082,r4
	b       ..ppcMtspr_any_end
	mtspr   0x083,r4
	b       ..ppcMtspr_any_end
	mtspr   0x084,r4
	b       ..ppcMtspr_any_end
	mtspr   0x085,r4
	b       ..ppcMtspr_any_end
	mtspr   0x086,r4
	b       ..ppcMtspr_any_end
	mtspr   0x087,r4
	b       ..ppcMtspr_any_end
	mtspr   0x088,r4
	b       ..ppcMtspr_any_end
	mtspr   0x089,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x08f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x090,r4
	b       ..ppcMtspr_any_end
	mtspr   0x091,r4
	b       ..ppcMtspr_any_end
	mtspr   0x092,r4
	b       ..ppcMtspr_any_end
	mtspr   0x093,r4
	b       ..ppcMtspr_any_end
	mtspr   0x094,r4
	b       ..ppcMtspr_any_end
	mtspr   0x095,r4
	b       ..ppcMtspr_any_end
	mtspr   0x096,r4
	b       ..ppcMtspr_any_end
	mtspr   0x097,r4
	b       ..ppcMtspr_any_end
	mtspr   0x098,r4
	b       ..ppcMtspr_any_end
	mtspr   0x099,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x09f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0a9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0aa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ab,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ac,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ad,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ae,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0af,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0b9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ba,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0bb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0bc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0bd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0be,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0bf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0c9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ca,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0cb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0cc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0cd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ce,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0cf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0d9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0da,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0db,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0dc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0dd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0de,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0df,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0e9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ea,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0eb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ec,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ed,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ee,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ef,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0f9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0fa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0fb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0fc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0fd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0fe,r4
	b       ..ppcMtspr_any_end
	mtspr   0x0ff,r4
	b       ..ppcMtspr_any_end
	mtspr   0x100,r4
	b       ..ppcMtspr_any_end
	mtspr   0x101,r4
	b       ..ppcMtspr_any_end
	mtspr   0x102,r4
	b       ..ppcMtspr_any_end
	mtspr   0x103,r4
	b       ..ppcMtspr_any_end
	mtspr   0x104,r4
	b       ..ppcMtspr_any_end
	mtspr   0x105,r4
	b       ..ppcMtspr_any_end
	mtspr   0x106,r4
	b       ..ppcMtspr_any_end
	mtspr   0x107,r4
	b       ..ppcMtspr_any_end
	mtspr   0x108,r4
	b       ..ppcMtspr_any_end
	mtspr   0x109,r4
	b       ..ppcMtspr_any_end
	mtspr   0x10a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x10b,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	mtspr   0x10e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x10f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x110,r4
	b       ..ppcMtspr_any_end
	mtspr   0x111,r4
	b       ..ppcMtspr_any_end
	mtspr   0x112,r4
	b       ..ppcMtspr_any_end
	mtspr   0x113,r4
	b       ..ppcMtspr_any_end
	mtspr   0x114,r4
	b       ..ppcMtspr_any_end
	mtspr   0x115,r4
	b       ..ppcMtspr_any_end
	mtspr   0x116,r4
	b       ..ppcMtspr_any_end
	mtspr   0x117,r4
	b       ..ppcMtspr_any_end
	mtspr   0x118,r4
	b       ..ppcMtspr_any_end
	mtspr   0x119,r4
	b       ..ppcMtspr_any_end
	mtspr   0x11a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x11b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x11c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x11d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x11e,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	mtspr   0x120,r4
	b       ..ppcMtspr_any_end
	mtspr   0x121,r4
	b       ..ppcMtspr_any_end
	mtspr   0x122,r4
	b       ..ppcMtspr_any_end
	mtspr   0x123,r4
	b       ..ppcMtspr_any_end
	mtspr   0x124,r4
	b       ..ppcMtspr_any_end
	mtspr   0x125,r4
	b       ..ppcMtspr_any_end
	mtspr   0x126,r4
	b       ..ppcMtspr_any_end
	mtspr   0x127,r4
	b       ..ppcMtspr_any_end
	mtspr   0x128,r4
	b       ..ppcMtspr_any_end
	mtspr   0x129,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x12f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x130,r4
	b       ..ppcMtspr_any_end
	mtspr   0x131,r4
	b       ..ppcMtspr_any_end
	mtspr   0x132,r4
	b       ..ppcMtspr_any_end
	mtspr   0x133,r4
	b       ..ppcMtspr_any_end
	mtspr   0x134,r4
	b       ..ppcMtspr_any_end
	mtspr   0x135,r4
	b       ..ppcMtspr_any_end
	mtspr   0x136,r4
	b       ..ppcMtspr_any_end
	mtspr   0x137,r4
	b       ..ppcMtspr_any_end
	mtspr   0x138,r4
	b       ..ppcMtspr_any_end
	mtspr   0x139,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x13f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x140,r4
	b       ..ppcMtspr_any_end
	mtspr   0x141,r4
	b       ..ppcMtspr_any_end
	mtspr   0x142,r4
	b       ..ppcMtspr_any_end
	mtspr   0x143,r4
	b       ..ppcMtspr_any_end
	mtspr   0x144,r4
	b       ..ppcMtspr_any_end
	mtspr   0x145,r4
	b       ..ppcMtspr_any_end
	mtspr   0x146,r4
	b       ..ppcMtspr_any_end
	mtspr   0x147,r4
	b       ..ppcMtspr_any_end
	mtspr   0x148,r4
	b       ..ppcMtspr_any_end
	mtspr   0x149,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x14f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x150,r4
	b       ..ppcMtspr_any_end
	mtspr   0x151,r4
	b       ..ppcMtspr_any_end
	mtspr   0x152,r4
	b       ..ppcMtspr_any_end
	mtspr   0x153,r4
	b       ..ppcMtspr_any_end
	mtspr   0x154,r4
	b       ..ppcMtspr_any_end
	mtspr   0x155,r4
	b       ..ppcMtspr_any_end
	mtspr   0x156,r4
	b       ..ppcMtspr_any_end
	mtspr   0x157,r4
	b       ..ppcMtspr_any_end
	mtspr   0x158,r4
	b       ..ppcMtspr_any_end
	mtspr   0x159,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x15f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x160,r4
	b       ..ppcMtspr_any_end
	mtspr   0x161,r4
	b       ..ppcMtspr_any_end
	mtspr   0x162,r4
	b       ..ppcMtspr_any_end
	mtspr   0x163,r4
	b       ..ppcMtspr_any_end
	mtspr   0x164,r4
	b       ..ppcMtspr_any_end
	mtspr   0x165,r4
	b       ..ppcMtspr_any_end
	mtspr   0x166,r4
	b       ..ppcMtspr_any_end
	mtspr   0x167,r4
	b       ..ppcMtspr_any_end
	mtspr   0x168,r4
	b       ..ppcMtspr_any_end
	mtspr   0x169,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x16f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x170,r4
	b       ..ppcMtspr_any_end
	mtspr   0x171,r4
	b       ..ppcMtspr_any_end
	mtspr   0x172,r4
	b       ..ppcMtspr_any_end
	mtspr   0x173,r4
	b       ..ppcMtspr_any_end
	mtspr   0x174,r4
	b       ..ppcMtspr_any_end
	mtspr   0x175,r4
	b       ..ppcMtspr_any_end
	mtspr   0x176,r4
	b       ..ppcMtspr_any_end
	mtspr   0x177,r4
	b       ..ppcMtspr_any_end
	mtspr   0x178,r4
	b       ..ppcMtspr_any_end
	mtspr   0x179,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x17f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x180,r4
	b       ..ppcMtspr_any_end
	mtspr   0x181,r4
	b       ..ppcMtspr_any_end
	mtspr   0x182,r4
	b       ..ppcMtspr_any_end
	mtspr   0x183,r4
	b       ..ppcMtspr_any_end
	mtspr   0x184,r4
	b       ..ppcMtspr_any_end
	mtspr   0x185,r4
	b       ..ppcMtspr_any_end
	mtspr   0x186,r4
	b       ..ppcMtspr_any_end
	mtspr   0x187,r4
	b       ..ppcMtspr_any_end
	mtspr   0x188,r4
	b       ..ppcMtspr_any_end
	mtspr   0x189,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x18f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x190,r4
	b       ..ppcMtspr_any_end
	mtspr   0x191,r4
	b       ..ppcMtspr_any_end
	mtspr   0x192,r4
	b       ..ppcMtspr_any_end
	mtspr   0x193,r4
	b       ..ppcMtspr_any_end
	mtspr   0x194,r4
	b       ..ppcMtspr_any_end
	mtspr   0x195,r4
	b       ..ppcMtspr_any_end
	mtspr   0x196,r4
	b       ..ppcMtspr_any_end
	mtspr   0x197,r4
	b       ..ppcMtspr_any_end
	mtspr   0x198,r4
	b       ..ppcMtspr_any_end
	mtspr   0x199,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x19f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1a9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1aa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ab,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ac,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ad,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ae,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1af,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1b9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ba,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1bb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1bc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1bd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1be,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1bf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1c9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ca,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1cb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1cc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1cd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ce,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1cf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1d9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1da,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1db,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1dc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1dd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1de,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1df,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1e9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ea,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1eb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ec,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ed,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ee,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ef,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1f9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1fa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1fb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1fc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1fd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1fe,r4
	b       ..ppcMtspr_any_end
	mtspr   0x1ff,r4
	b       ..ppcMtspr_any_end
	mtspr   0x200,r4
	b       ..ppcMtspr_any_end
	mtspr   0x201,r4
	b       ..ppcMtspr_any_end
	mtspr   0x202,r4
	b       ..ppcMtspr_any_end
	mtspr   0x203,r4
	b       ..ppcMtspr_any_end
	mtspr   0x204,r4
	b       ..ppcMtspr_any_end
	mtspr   0x205,r4
	b       ..ppcMtspr_any_end
	mtspr   0x206,r4
	b       ..ppcMtspr_any_end
	mtspr   0x207,r4
	b       ..ppcMtspr_any_end
	mtspr   0x208,r4
	b       ..ppcMtspr_any_end
	mtspr   0x209,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x20f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x210,r4
	b       ..ppcMtspr_any_end
	mtspr   0x211,r4
	b       ..ppcMtspr_any_end
	mtspr   0x212,r4
	b       ..ppcMtspr_any_end
	mtspr   0x213,r4
	b       ..ppcMtspr_any_end
	mtspr   0x214,r4
	b       ..ppcMtspr_any_end
	mtspr   0x215,r4
	b       ..ppcMtspr_any_end
	mtspr   0x216,r4
	b       ..ppcMtspr_any_end
	mtspr   0x217,r4
	b       ..ppcMtspr_any_end
	mtspr   0x218,r4
	b       ..ppcMtspr_any_end
	mtspr   0x219,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x21f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x220,r4
	b       ..ppcMtspr_any_end
	mtspr   0x221,r4
	b       ..ppcMtspr_any_end
	mtspr   0x222,r4
	b       ..ppcMtspr_any_end
	mtspr   0x223,r4
	b       ..ppcMtspr_any_end
	mtspr   0x224,r4
	b       ..ppcMtspr_any_end
	mtspr   0x225,r4
	b       ..ppcMtspr_any_end
	mtspr   0x226,r4
	b       ..ppcMtspr_any_end
	mtspr   0x227,r4
	b       ..ppcMtspr_any_end
	mtspr   0x228,r4
	b       ..ppcMtspr_any_end
	mtspr   0x229,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x22f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x230,r4
	b       ..ppcMtspr_any_end
	mtspr   0x231,r4
	b       ..ppcMtspr_any_end
	mtspr   0x232,r4
	b       ..ppcMtspr_any_end
	mtspr   0x233,r4
	b       ..ppcMtspr_any_end
	mtspr   0x234,r4
	b       ..ppcMtspr_any_end
	mtspr   0x235,r4
	b       ..ppcMtspr_any_end
	mtspr   0x236,r4
	b       ..ppcMtspr_any_end
	mtspr   0x237,r4
	b       ..ppcMtspr_any_end
	mtspr   0x238,r4
	b       ..ppcMtspr_any_end
	mtspr   0x239,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x23f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x240,r4
	b       ..ppcMtspr_any_end
	mtspr   0x241,r4
	b       ..ppcMtspr_any_end
	mtspr   0x242,r4
	b       ..ppcMtspr_any_end
	mtspr   0x243,r4
	b       ..ppcMtspr_any_end
	mtspr   0x244,r4
	b       ..ppcMtspr_any_end
	mtspr   0x245,r4
	b       ..ppcMtspr_any_end
	mtspr   0x246,r4
	b       ..ppcMtspr_any_end
	mtspr   0x247,r4
	b       ..ppcMtspr_any_end
	mtspr   0x248,r4
	b       ..ppcMtspr_any_end
	mtspr   0x249,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x24f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x250,r4
	b       ..ppcMtspr_any_end
	mtspr   0x251,r4
	b       ..ppcMtspr_any_end
	mtspr   0x252,r4
	b       ..ppcMtspr_any_end
	mtspr   0x253,r4
	b       ..ppcMtspr_any_end
	mtspr   0x254,r4
	b       ..ppcMtspr_any_end
	mtspr   0x255,r4
	b       ..ppcMtspr_any_end
	mtspr   0x256,r4
	b       ..ppcMtspr_any_end
	mtspr   0x257,r4
	b       ..ppcMtspr_any_end
	mtspr   0x258,r4
	b       ..ppcMtspr_any_end
	mtspr   0x259,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x25f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x260,r4
	b       ..ppcMtspr_any_end
	mtspr   0x261,r4
	b       ..ppcMtspr_any_end
	mtspr   0x262,r4
	b       ..ppcMtspr_any_end
	mtspr   0x263,r4
	b       ..ppcMtspr_any_end
	mtspr   0x264,r4
	b       ..ppcMtspr_any_end
	mtspr   0x265,r4
	b       ..ppcMtspr_any_end
	mtspr   0x266,r4
	b       ..ppcMtspr_any_end
	mtspr   0x267,r4
	b       ..ppcMtspr_any_end
	mtspr   0x268,r4
	b       ..ppcMtspr_any_end
	mtspr   0x269,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x26f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x270,r4
	b       ..ppcMtspr_any_end
	mtspr   0x271,r4
	b       ..ppcMtspr_any_end
	mtspr   0x272,r4
	b       ..ppcMtspr_any_end
	mtspr   0x273,r4
	b       ..ppcMtspr_any_end
	mtspr   0x274,r4
	b       ..ppcMtspr_any_end
	mtspr   0x275,r4
	b       ..ppcMtspr_any_end
	mtspr   0x276,r4
	b       ..ppcMtspr_any_end
	mtspr   0x277,r4
	b       ..ppcMtspr_any_end
	mtspr   0x278,r4
	b       ..ppcMtspr_any_end
	mtspr   0x279,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x27f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x280,r4
	b       ..ppcMtspr_any_end
	mtspr   0x281,r4
	b       ..ppcMtspr_any_end
	mtspr   0x282,r4
	b       ..ppcMtspr_any_end
	mtspr   0x283,r4
	b       ..ppcMtspr_any_end
	mtspr   0x284,r4
	b       ..ppcMtspr_any_end
	mtspr   0x285,r4
	b       ..ppcMtspr_any_end
	mtspr   0x286,r4
	b       ..ppcMtspr_any_end
	mtspr   0x287,r4
	b       ..ppcMtspr_any_end
	mtspr   0x288,r4
	b       ..ppcMtspr_any_end
	mtspr   0x289,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x28f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x290,r4
	b       ..ppcMtspr_any_end
	mtspr   0x291,r4
	b       ..ppcMtspr_any_end
	mtspr   0x292,r4
	b       ..ppcMtspr_any_end
	mtspr   0x293,r4
	b       ..ppcMtspr_any_end
	mtspr   0x294,r4
	b       ..ppcMtspr_any_end
	mtspr   0x295,r4
	b       ..ppcMtspr_any_end
	mtspr   0x296,r4
	b       ..ppcMtspr_any_end
	mtspr   0x297,r4
	b       ..ppcMtspr_any_end
	mtspr   0x298,r4
	b       ..ppcMtspr_any_end
	mtspr   0x299,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x29f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2a9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2aa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ab,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ac,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ad,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ae,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2af,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2b9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ba,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2bb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2bc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2bd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2be,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2bf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2c9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ca,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2cb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2cc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2cd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ce,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2cf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2d9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2da,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2db,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2dc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2dd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2de,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2df,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2e9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ea,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2eb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ec,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ed,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ee,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ef,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2f9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2fa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2fb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2fc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2fd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2fe,r4
	b       ..ppcMtspr_any_end
	mtspr   0x2ff,r4
	b       ..ppcMtspr_any_end
	mtspr   0x300,r4
	b       ..ppcMtspr_any_end
	mtspr   0x301,r4
	b       ..ppcMtspr_any_end
	mtspr   0x302,r4
	b       ..ppcMtspr_any_end
	mtspr   0x303,r4
	b       ..ppcMtspr_any_end
	mtspr   0x304,r4
	b       ..ppcMtspr_any_end
	mtspr   0x305,r4
	b       ..ppcMtspr_any_end
	mtspr   0x306,r4
	b       ..ppcMtspr_any_end
	mtspr   0x307,r4
	b       ..ppcMtspr_any_end
	mtspr   0x308,r4
	b       ..ppcMtspr_any_end
	mtspr   0x309,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x30f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x310,r4
	b       ..ppcMtspr_any_end
	mtspr   0x311,r4
	b       ..ppcMtspr_any_end
	mtspr   0x312,r4
	b       ..ppcMtspr_any_end
	mtspr   0x313,r4
	b       ..ppcMtspr_any_end
	mtspr   0x314,r4
	b       ..ppcMtspr_any_end
	mtspr   0x315,r4
	b       ..ppcMtspr_any_end
	mtspr   0x316,r4
	b       ..ppcMtspr_any_end
	mtspr   0x317,r4
	b       ..ppcMtspr_any_end
	mtspr   0x318,r4
	b       ..ppcMtspr_any_end
	mtspr   0x319,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x31f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x320,r4
	b       ..ppcMtspr_any_end
	mtspr   0x321,r4
	b       ..ppcMtspr_any_end
	mtspr   0x322,r4
	b       ..ppcMtspr_any_end
	mtspr   0x323,r4
	b       ..ppcMtspr_any_end
	mtspr   0x324,r4
	b       ..ppcMtspr_any_end
	mtspr   0x325,r4
	b       ..ppcMtspr_any_end
	mtspr   0x326,r4
	b       ..ppcMtspr_any_end
	mtspr   0x327,r4
	b       ..ppcMtspr_any_end
	mtspr   0x328,r4
	b       ..ppcMtspr_any_end
	mtspr   0x329,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x32f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x330,r4
	b       ..ppcMtspr_any_end
	mtspr   0x331,r4
	b       ..ppcMtspr_any_end
	mtspr   0x332,r4
	b       ..ppcMtspr_any_end
	mtspr   0x333,r4
	b       ..ppcMtspr_any_end
	mtspr   0x334,r4
	b       ..ppcMtspr_any_end
	mtspr   0x335,r4
	b       ..ppcMtspr_any_end
	mtspr   0x336,r4
	b       ..ppcMtspr_any_end
	mtspr   0x337,r4
	b       ..ppcMtspr_any_end
	mtspr   0x338,r4
	b       ..ppcMtspr_any_end
	mtspr   0x339,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x33f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x340,r4
	b       ..ppcMtspr_any_end
	mtspr   0x341,r4
	b       ..ppcMtspr_any_end
	mtspr   0x342,r4
	b       ..ppcMtspr_any_end
	mtspr   0x343,r4
	b       ..ppcMtspr_any_end
	mtspr   0x344,r4
	b       ..ppcMtspr_any_end
	mtspr   0x345,r4
	b       ..ppcMtspr_any_end
	mtspr   0x346,r4
	b       ..ppcMtspr_any_end
	mtspr   0x347,r4
	b       ..ppcMtspr_any_end
	mtspr   0x348,r4
	b       ..ppcMtspr_any_end
	mtspr   0x349,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x34f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x350,r4
	b       ..ppcMtspr_any_end
	mtspr   0x351,r4
	b       ..ppcMtspr_any_end
	mtspr   0x352,r4
	b       ..ppcMtspr_any_end
	mtspr   0x353,r4
	b       ..ppcMtspr_any_end
	mtspr   0x354,r4
	b       ..ppcMtspr_any_end
	mtspr   0x355,r4
	b       ..ppcMtspr_any_end
	mtspr   0x356,r4
	b       ..ppcMtspr_any_end
	mtspr   0x357,r4
	b       ..ppcMtspr_any_end
	mtspr   0x358,r4
	b       ..ppcMtspr_any_end
	mtspr   0x359,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x35f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x360,r4
	b       ..ppcMtspr_any_end
	mtspr   0x361,r4
	b       ..ppcMtspr_any_end
	mtspr   0x362,r4
	b       ..ppcMtspr_any_end
	mtspr   0x363,r4
	b       ..ppcMtspr_any_end
	mtspr   0x364,r4
	b       ..ppcMtspr_any_end
	mtspr   0x365,r4
	b       ..ppcMtspr_any_end
	mtspr   0x366,r4
	b       ..ppcMtspr_any_end
	mtspr   0x367,r4
	b       ..ppcMtspr_any_end
	mtspr   0x368,r4
	b       ..ppcMtspr_any_end
	mtspr   0x369,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x36f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x370,r4
	b       ..ppcMtspr_any_end
	mtspr   0x371,r4
	b       ..ppcMtspr_any_end
	mtspr   0x372,r4
	b       ..ppcMtspr_any_end
	mtspr   0x373,r4
	b       ..ppcMtspr_any_end
	mtspr   0x374,r4
	b       ..ppcMtspr_any_end
	mtspr   0x375,r4
	b       ..ppcMtspr_any_end
	mtspr   0x376,r4
	b       ..ppcMtspr_any_end
	mtspr   0x377,r4
	b       ..ppcMtspr_any_end
	mtspr   0x378,r4
	b       ..ppcMtspr_any_end
	mtspr   0x379,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x37f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x380,r4
	b       ..ppcMtspr_any_end
	mtspr   0x381,r4
	b       ..ppcMtspr_any_end
	mtspr   0x382,r4
	b       ..ppcMtspr_any_end
	mtspr   0x383,r4
	b       ..ppcMtspr_any_end
	mtspr   0x384,r4
	b       ..ppcMtspr_any_end
	mtspr   0x385,r4
	b       ..ppcMtspr_any_end
	mtspr   0x386,r4
	b       ..ppcMtspr_any_end
	mtspr   0x387,r4
	b       ..ppcMtspr_any_end
	mtspr   0x388,r4
	b       ..ppcMtspr_any_end
	mtspr   0x389,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x38f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x390,r4
	b       ..ppcMtspr_any_end
	mtspr   0x391,r4
	b       ..ppcMtspr_any_end
	mtspr   0x392,r4
	b       ..ppcMtspr_any_end
	mtspr   0x393,r4
	b       ..ppcMtspr_any_end
	mtspr   0x394,r4
	b       ..ppcMtspr_any_end
	mtspr   0x395,r4
	b       ..ppcMtspr_any_end
	mtspr   0x396,r4
	b       ..ppcMtspr_any_end
	mtspr   0x397,r4
	b       ..ppcMtspr_any_end
	mtspr   0x398,r4
	b       ..ppcMtspr_any_end
	mtspr   0x399,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39a,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39b,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39c,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39d,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39e,r4
	b       ..ppcMtspr_any_end
	mtspr   0x39f,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3a9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3aa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ab,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ac,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ad,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ae,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3af,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3b9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ba,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3bb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3bc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3bd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3be,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3bf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3c9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ca,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3cb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3cc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3cd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ce,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3cf,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3d9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3da,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3db,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3dc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3dd,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3de,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3df,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e0,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e1,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e3,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e4,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e5,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e6,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3e9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ea,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3eb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ec,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ed,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ee,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3ef,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	mtspr   0x3f2,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3f3,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	mtspr   0x3f5,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	mtspr   0x3f7,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3f8,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3f9,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3fa,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3fb,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3fc,r4
	b       ..ppcMtspr_any_end
	mtspr   0x3fd,r4
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMtspr_any_end
..ppcMtspr_any_end:
	mtlr	r6
	blr
	function_epilog(ppcMtspr_any)

/*----------------------------------------------------------------------------+
| PpcMfspr_any
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfspr_any)
        rlwinm	r3,r3,3,19,29
	addi	r3,r3,0x0010
	mflr	r6
	bl	..sp_gett_lr
..sp_gett_lr:
	mflr	r5
	add 	r5,r5,r3
	mtlr	r5
	blr
	mfspr   r3,0x000
	b       ..ppcMfspr_any_end
	mfspr   r3,0x001
	b       ..ppcMfspr_any_end
	mfspr   r3,0x002
	b       ..ppcMfspr_any_end
	mfspr   r3,0x003
	b       ..ppcMfspr_any_end
	mfspr   r3,0x004
	b       ..ppcMfspr_any_end
	mfspr   r3,0x005
	b       ..ppcMfspr_any_end
	mfspr   r3,0x006
	b       ..ppcMfspr_any_end
	mfspr   r3,0x007
	b       ..ppcMfspr_any_end
	mfspr   r3,0x008
	b       ..ppcMfspr_any_end
	mfspr   r3,0x009
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x00f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x010
	b       ..ppcMfspr_any_end
	mfspr   r3,0x011
	b       ..ppcMfspr_any_end
	mfspr   r3,0x012
	b       ..ppcMfspr_any_end
	mfspr   r3,0x013
	b       ..ppcMfspr_any_end
	mfspr   r3,0x014
	b       ..ppcMfspr_any_end
	mfspr   r3,0x015
	b       ..ppcMfspr_any_end
	mfspr   r3,0x016
	b       ..ppcMfspr_any_end
	mfspr   r3,0x017
	b       ..ppcMfspr_any_end
	mfspr   r3,0x018
	b       ..ppcMfspr_any_end
	mfspr   r3,0x019
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x01f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x020
	b       ..ppcMfspr_any_end
	mfspr   r3,0x021
	b       ..ppcMfspr_any_end
	mfspr   r3,0x022
	b       ..ppcMfspr_any_end
	mfspr   r3,0x023
	b       ..ppcMfspr_any_end
	mfspr   r3,0x024
	b       ..ppcMfspr_any_end
	mfspr   r3,0x025
	b       ..ppcMfspr_any_end
	mfspr   r3,0x026
	b       ..ppcMfspr_any_end
	mfspr   r3,0x027
	b       ..ppcMfspr_any_end
	mfspr   r3,0x028
	b       ..ppcMfspr_any_end
	mfspr   r3,0x029
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x02f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x030
	b       ..ppcMfspr_any_end
	mfspr   r3,0x031
	b       ..ppcMfspr_any_end
	mfspr   r3,0x032
	b       ..ppcMfspr_any_end
	mfspr   r3,0x033
	b       ..ppcMfspr_any_end
	mfspr   r3,0x034
	b       ..ppcMfspr_any_end
	mfspr   r3,0x035
	b       ..ppcMfspr_any_end
	mfspr   r3,0x036
	b       ..ppcMfspr_any_end
	mfspr   r3,0x037
	b       ..ppcMfspr_any_end
	mfspr   r3,0x038
	b       ..ppcMfspr_any_end
	mfspr   r3,0x039
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x03f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x040
	b       ..ppcMfspr_any_end
	mfspr   r3,0x041
	b       ..ppcMfspr_any_end
	mfspr   r3,0x042
	b       ..ppcMfspr_any_end
	mfspr   r3,0x043
	b       ..ppcMfspr_any_end
	mfspr   r3,0x044
	b       ..ppcMfspr_any_end
	mfspr   r3,0x045
	b       ..ppcMfspr_any_end
	mfspr   r3,0x046
	b       ..ppcMfspr_any_end
	mfspr   r3,0x047
	b       ..ppcMfspr_any_end
	mfspr   r3,0x048
	b       ..ppcMfspr_any_end
	mfspr   r3,0x049
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x04f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x050
	b       ..ppcMfspr_any_end
	mfspr   r3,0x051
	b       ..ppcMfspr_any_end
	mfspr   r3,0x052
	b       ..ppcMfspr_any_end
	mfspr   r3,0x053
	b       ..ppcMfspr_any_end
	mfspr   r3,0x054
	b       ..ppcMfspr_any_end
	mfspr   r3,0x055
	b       ..ppcMfspr_any_end
	mfspr   r3,0x056
	b       ..ppcMfspr_any_end
	mfspr   r3,0x057
	b       ..ppcMfspr_any_end
	mfspr   r3,0x058
	b       ..ppcMfspr_any_end
	mfspr   r3,0x059
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x05f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x060
	b       ..ppcMfspr_any_end
	mfspr   r3,0x061
	b       ..ppcMfspr_any_end
	mfspr   r3,0x062
	b       ..ppcMfspr_any_end
	mfspr   r3,0x063
	b       ..ppcMfspr_any_end
	mfspr   r3,0x064
	b       ..ppcMfspr_any_end
	mfspr   r3,0x065
	b       ..ppcMfspr_any_end
	mfspr   r3,0x066
	b       ..ppcMfspr_any_end
	mfspr   r3,0x067
	b       ..ppcMfspr_any_end
	mfspr   r3,0x068
	b       ..ppcMfspr_any_end
	mfspr   r3,0x069
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x06f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x070
	b       ..ppcMfspr_any_end
	mfspr   r3,0x071
	b       ..ppcMfspr_any_end
	mfspr   r3,0x072
	b       ..ppcMfspr_any_end
	mfspr   r3,0x073
	b       ..ppcMfspr_any_end
	mfspr   r3,0x074
	b       ..ppcMfspr_any_end
	mfspr   r3,0x075
	b       ..ppcMfspr_any_end
	mfspr   r3,0x076
	b       ..ppcMfspr_any_end
	mfspr   r3,0x077
	b       ..ppcMfspr_any_end
	mfspr   r3,0x078
	b       ..ppcMfspr_any_end
	mfspr   r3,0x079
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x07f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x080
	b       ..ppcMfspr_any_end
	mfspr   r3,0x081
	b       ..ppcMfspr_any_end
	mfspr   r3,0x082
	b       ..ppcMfspr_any_end
	mfspr   r3,0x083
	b       ..ppcMfspr_any_end
	mfspr   r3,0x084
	b       ..ppcMfspr_any_end
	mfspr   r3,0x085
	b       ..ppcMfspr_any_end
	mfspr   r3,0x086
	b       ..ppcMfspr_any_end
	mfspr   r3,0x087
	b       ..ppcMfspr_any_end
	mfspr   r3,0x088
	b       ..ppcMfspr_any_end
	mfspr   r3,0x089
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x08f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x090
	b       ..ppcMfspr_any_end
	mfspr   r3,0x091
	b       ..ppcMfspr_any_end
	mfspr   r3,0x092
	b       ..ppcMfspr_any_end
	mfspr   r3,0x093
	b       ..ppcMfspr_any_end
	mfspr   r3,0x094
	b       ..ppcMfspr_any_end
	mfspr   r3,0x095
	b       ..ppcMfspr_any_end
	mfspr   r3,0x096
	b       ..ppcMfspr_any_end
	mfspr   r3,0x097
	b       ..ppcMfspr_any_end
	mfspr   r3,0x098
	b       ..ppcMfspr_any_end
	mfspr   r3,0x099
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x09f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0a9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0aa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ab
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ac
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ad
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ae
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0af
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0b9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ba
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0bb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0bc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0bd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0be
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0bf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0c9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ca
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0cb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0cc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0cd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ce
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0cf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0d9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0da
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0db
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0dc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0dd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0de
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0df
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0e9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ea
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0eb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ec
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ed
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ee
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ef
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0f9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0fa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0fb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0fc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0fd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0fe
	b       ..ppcMfspr_any_end
	mfspr   r3,0x0ff
	b       ..ppcMfspr_any_end
	mfspr   r3,0x100
	b       ..ppcMfspr_any_end
	mfspr   r3,0x101
	b       ..ppcMfspr_any_end
	mfspr   r3,0x102
	b       ..ppcMfspr_any_end
	mfspr   r3,0x103
	b       ..ppcMfspr_any_end
	mfspr   r3,0x104
	b       ..ppcMfspr_any_end
	mfspr   r3,0x105
	b       ..ppcMfspr_any_end
	mfspr   r3,0x106
	b       ..ppcMfspr_any_end
	mfspr   r3,0x107
	b       ..ppcMfspr_any_end
	mfspr   r3,0x108
	b       ..ppcMfspr_any_end
	mfspr   r3,0x109
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x10f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x110
	b       ..ppcMfspr_any_end
	mfspr   r3,0x111
	b       ..ppcMfspr_any_end
	mfspr   r3,0x112
	b       ..ppcMfspr_any_end
	mfspr   r3,0x113
	b       ..ppcMfspr_any_end
	mfspr   r3,0x114
	b       ..ppcMfspr_any_end
	mfspr   r3,0x115
	b       ..ppcMfspr_any_end
	mfspr   r3,0x116
	b       ..ppcMfspr_any_end
	mfspr   r3,0x117
	b       ..ppcMfspr_any_end
	mfspr   r3,0x118
	b       ..ppcMfspr_any_end
	mfspr   r3,0x119
	b       ..ppcMfspr_any_end
	mfspr   r3,0x11a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x11b
	b       ..ppcMfspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMfspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMfspr_any_end
	mfspr   r3,0x11e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x11f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x120
	b       ..ppcMfspr_any_end
	mfspr   r3,0x121
	b       ..ppcMfspr_any_end
	mfspr   r3,0x122
	b       ..ppcMfspr_any_end
	mfspr   r3,0x123
	b       ..ppcMfspr_any_end
	mfspr   r3,0x124
	b       ..ppcMfspr_any_end
	mfspr   r3,0x125
	b       ..ppcMfspr_any_end
	mfspr   r3,0x126
	b       ..ppcMfspr_any_end
	mfspr   r3,0x127
	b       ..ppcMfspr_any_end
	mfspr   r3,0x128
	b       ..ppcMfspr_any_end
	mfspr   r3,0x129
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x12f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x130
	b       ..ppcMfspr_any_end
	mfspr   r3,0x131
	b       ..ppcMfspr_any_end
	mfspr   r3,0x132
	b       ..ppcMfspr_any_end
	mfspr   r3,0x133
	b       ..ppcMfspr_any_end
	mfspr   r3,0x134
	b       ..ppcMfspr_any_end
	mfspr   r3,0x135
	b       ..ppcMfspr_any_end
	mfspr   r3,0x136
	b       ..ppcMfspr_any_end
	mfspr   r3,0x137
	b       ..ppcMfspr_any_end
	mfspr   r3,0x138
	b       ..ppcMfspr_any_end
	mfspr   r3,0x139
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x13f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x140
	b       ..ppcMfspr_any_end
	mfspr   r3,0x141
	b       ..ppcMfspr_any_end
	mfspr   r3,0x142
	b       ..ppcMfspr_any_end
	mfspr   r3,0x143
	b       ..ppcMfspr_any_end
	mfspr   r3,0x144
	b       ..ppcMfspr_any_end
	mfspr   r3,0x145
	b       ..ppcMfspr_any_end
	mfspr   r3,0x146
	b       ..ppcMfspr_any_end
	mfspr   r3,0x147
	b       ..ppcMfspr_any_end
	mfspr   r3,0x148
	b       ..ppcMfspr_any_end
	mfspr   r3,0x149
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x14f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x150
	b       ..ppcMfspr_any_end
	mfspr   r3,0x151
	b       ..ppcMfspr_any_end
	mfspr   r3,0x152
	b       ..ppcMfspr_any_end
	mfspr   r3,0x153
	b       ..ppcMfspr_any_end
	mfspr   r3,0x154
	b       ..ppcMfspr_any_end
	mfspr   r3,0x155
	b       ..ppcMfspr_any_end
	mfspr   r3,0x156
	b       ..ppcMfspr_any_end
	mfspr   r3,0x157
	b       ..ppcMfspr_any_end
	mfspr   r3,0x158
	b       ..ppcMfspr_any_end
	mfspr   r3,0x159
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x15f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x160
	b       ..ppcMfspr_any_end
	mfspr   r3,0x161
	b       ..ppcMfspr_any_end
	mfspr   r3,0x162
	b       ..ppcMfspr_any_end
	mfspr   r3,0x163
	b       ..ppcMfspr_any_end
	mfspr   r3,0x164
	b       ..ppcMfspr_any_end
	mfspr   r3,0x165
	b       ..ppcMfspr_any_end
	mfspr   r3,0x166
	b       ..ppcMfspr_any_end
	mfspr   r3,0x167
	b       ..ppcMfspr_any_end
	mfspr   r3,0x168
	b       ..ppcMfspr_any_end
	mfspr   r3,0x169
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x16f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x170
	b       ..ppcMfspr_any_end
	mfspr   r3,0x171
	b       ..ppcMfspr_any_end
	mfspr   r3,0x172
	b       ..ppcMfspr_any_end
	mfspr   r3,0x173
	b       ..ppcMfspr_any_end
	mfspr   r3,0x174
	b       ..ppcMfspr_any_end
	mfspr   r3,0x175
	b       ..ppcMfspr_any_end
	mfspr   r3,0x176
	b       ..ppcMfspr_any_end
	mfspr   r3,0x177
	b       ..ppcMfspr_any_end
	mfspr   r3,0x178
	b       ..ppcMfspr_any_end
	mfspr   r3,0x179
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x17f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x180
	b       ..ppcMfspr_any_end
	mfspr   r3,0x181
	b       ..ppcMfspr_any_end
	mfspr   r3,0x182
	b       ..ppcMfspr_any_end
	mfspr   r3,0x183
	b       ..ppcMfspr_any_end
	mfspr   r3,0x184
	b       ..ppcMfspr_any_end
	mfspr   r3,0x185
	b       ..ppcMfspr_any_end
	mfspr   r3,0x186
	b       ..ppcMfspr_any_end
	mfspr   r3,0x187
	b       ..ppcMfspr_any_end
	mfspr   r3,0x188
	b       ..ppcMfspr_any_end
	mfspr   r3,0x189
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x18f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x190
	b       ..ppcMfspr_any_end
	mfspr   r3,0x191
	b       ..ppcMfspr_any_end
	mfspr   r3,0x192
	b       ..ppcMfspr_any_end
	mfspr   r3,0x193
	b       ..ppcMfspr_any_end
	mfspr   r3,0x194
	b       ..ppcMfspr_any_end
	mfspr   r3,0x195
	b       ..ppcMfspr_any_end
	mfspr   r3,0x196
	b       ..ppcMfspr_any_end
	mfspr   r3,0x197
	b       ..ppcMfspr_any_end
	mfspr   r3,0x198
	b       ..ppcMfspr_any_end
	mfspr   r3,0x199
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x19f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1a9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1aa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ab
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ac
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ad
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ae
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1af
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1b9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ba
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1bb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1bc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1bd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1be
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1bf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1c9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ca
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1cb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1cc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1cd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ce
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1cf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1d9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1da
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1db
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1dc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1dd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1de
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1df
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1e9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ea
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1eb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ec
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ed
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ee
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ef
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1f9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1fa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1fb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1fc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1fd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1fe
	b       ..ppcMfspr_any_end
	mfspr   r3,0x1ff
	b       ..ppcMfspr_any_end
	mfspr   r3,0x200
	b       ..ppcMfspr_any_end
	mfspr   r3,0x201
	b       ..ppcMfspr_any_end
	mfspr   r3,0x202
	b       ..ppcMfspr_any_end
	mfspr   r3,0x203
	b       ..ppcMfspr_any_end
	mfspr   r3,0x204
	b       ..ppcMfspr_any_end
	mfspr   r3,0x205
	b       ..ppcMfspr_any_end
	mfspr   r3,0x206
	b       ..ppcMfspr_any_end
	mfspr   r3,0x207
	b       ..ppcMfspr_any_end
	mfspr   r3,0x208
	b       ..ppcMfspr_any_end
	mfspr   r3,0x209
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x20f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x210
	b       ..ppcMfspr_any_end
	mfspr   r3,0x211
	b       ..ppcMfspr_any_end
	mfspr   r3,0x212
	b       ..ppcMfspr_any_end
	mfspr   r3,0x213
	b       ..ppcMfspr_any_end
	mfspr   r3,0x214
	b       ..ppcMfspr_any_end
	mfspr   r3,0x215
	b       ..ppcMfspr_any_end
	mfspr   r3,0x216
	b       ..ppcMfspr_any_end
	mfspr   r3,0x217
	b       ..ppcMfspr_any_end
	mfspr   r3,0x218
	b       ..ppcMfspr_any_end
	mfspr   r3,0x219
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x21f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x220
	b       ..ppcMfspr_any_end
	mfspr   r3,0x221
	b       ..ppcMfspr_any_end
	mfspr   r3,0x222
	b       ..ppcMfspr_any_end
	mfspr   r3,0x223
	b       ..ppcMfspr_any_end
	mfspr   r3,0x224
	b       ..ppcMfspr_any_end
	mfspr   r3,0x225
	b       ..ppcMfspr_any_end
	mfspr   r3,0x226
	b       ..ppcMfspr_any_end
	mfspr   r3,0x227
	b       ..ppcMfspr_any_end
	mfspr   r3,0x228
	b       ..ppcMfspr_any_end
	mfspr   r3,0x229
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x22f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x230
	b       ..ppcMfspr_any_end
	mfspr   r3,0x231
	b       ..ppcMfspr_any_end
	mfspr   r3,0x232
	b       ..ppcMfspr_any_end
	mfspr   r3,0x233
	b       ..ppcMfspr_any_end
	mfspr   r3,0x234
	b       ..ppcMfspr_any_end
	mfspr   r3,0x235
	b       ..ppcMfspr_any_end
	mfspr   r3,0x236
	b       ..ppcMfspr_any_end
	mfspr   r3,0x237
	b       ..ppcMfspr_any_end
	mfspr   r3,0x238
	b       ..ppcMfspr_any_end
	mfspr   r3,0x239
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x23f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x240
	b       ..ppcMfspr_any_end
	mfspr   r3,0x241
	b       ..ppcMfspr_any_end
	mfspr   r3,0x242
	b       ..ppcMfspr_any_end
	mfspr   r3,0x243
	b       ..ppcMfspr_any_end
	mfspr   r3,0x244
	b       ..ppcMfspr_any_end
	mfspr   r3,0x245
	b       ..ppcMfspr_any_end
	mfspr   r3,0x246
	b       ..ppcMfspr_any_end
	mfspr   r3,0x247
	b       ..ppcMfspr_any_end
	mfspr   r3,0x248
	b       ..ppcMfspr_any_end
	mfspr   r3,0x249
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x24f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x250
	b       ..ppcMfspr_any_end
	mfspr   r3,0x251
	b       ..ppcMfspr_any_end
	mfspr   r3,0x252
	b       ..ppcMfspr_any_end
	mfspr   r3,0x253
	b       ..ppcMfspr_any_end
	mfspr   r3,0x254
	b       ..ppcMfspr_any_end
	mfspr   r3,0x255
	b       ..ppcMfspr_any_end
	mfspr   r3,0x256
	b       ..ppcMfspr_any_end
	mfspr   r3,0x257
	b       ..ppcMfspr_any_end
	mfspr   r3,0x258
	b       ..ppcMfspr_any_end
	mfspr   r3,0x259
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x25f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x260
	b       ..ppcMfspr_any_end
	mfspr   r3,0x261
	b       ..ppcMfspr_any_end
	mfspr   r3,0x262
	b       ..ppcMfspr_any_end
	mfspr   r3,0x263
	b       ..ppcMfspr_any_end
	mfspr   r3,0x264
	b       ..ppcMfspr_any_end
	mfspr   r3,0x265
	b       ..ppcMfspr_any_end
	mfspr   r3,0x266
	b       ..ppcMfspr_any_end
	mfspr   r3,0x267
	b       ..ppcMfspr_any_end
	mfspr   r3,0x268
	b       ..ppcMfspr_any_end
	mfspr   r3,0x269
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x26f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x270
	b       ..ppcMfspr_any_end
	mfspr   r3,0x271
	b       ..ppcMfspr_any_end
	mfspr   r3,0x272
	b       ..ppcMfspr_any_end
	mfspr   r3,0x273
	b       ..ppcMfspr_any_end
	mfspr   r3,0x274
	b       ..ppcMfspr_any_end
	mfspr   r3,0x275
	b       ..ppcMfspr_any_end
	mfspr   r3,0x276
	b       ..ppcMfspr_any_end
	mfspr   r3,0x277
	b       ..ppcMfspr_any_end
	mfspr   r3,0x278
	b       ..ppcMfspr_any_end
	mfspr   r3,0x279
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x27f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x280
	b       ..ppcMfspr_any_end
	mfspr   r3,0x281
	b       ..ppcMfspr_any_end
	mfspr   r3,0x282
	b       ..ppcMfspr_any_end
	mfspr   r3,0x283
	b       ..ppcMfspr_any_end
	mfspr   r3,0x284
	b       ..ppcMfspr_any_end
	mfspr   r3,0x285
	b       ..ppcMfspr_any_end
	mfspr   r3,0x286
	b       ..ppcMfspr_any_end
	mfspr   r3,0x287
	b       ..ppcMfspr_any_end
	mfspr   r3,0x288
	b       ..ppcMfspr_any_end
	mfspr   r3,0x289
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x28f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x290
	b       ..ppcMfspr_any_end
	mfspr   r3,0x291
	b       ..ppcMfspr_any_end
	mfspr   r3,0x292
	b       ..ppcMfspr_any_end
	mfspr   r3,0x293
	b       ..ppcMfspr_any_end
	mfspr   r3,0x294
	b       ..ppcMfspr_any_end
	mfspr   r3,0x295
	b       ..ppcMfspr_any_end
	mfspr   r3,0x296
	b       ..ppcMfspr_any_end
	mfspr   r3,0x297
	b       ..ppcMfspr_any_end
	mfspr   r3,0x298
	b       ..ppcMfspr_any_end
	mfspr   r3,0x299
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x29f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2a9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2aa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ab
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ac
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ad
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ae
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2af
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2b9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ba
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2bb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2bc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2bd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2be
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2bf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2c9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ca
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2cb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2cc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2cd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ce
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2cf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2d9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2da
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2db
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2dc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2dd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2de
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2df
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2e9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ea
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2eb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ec
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ed
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ee
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ef
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2f9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2fa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2fb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2fc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2fd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2fe
	b       ..ppcMfspr_any_end
	mfspr   r3,0x2ff
	b       ..ppcMfspr_any_end
	mfspr   r3,0x300
	b       ..ppcMfspr_any_end
	mfspr   r3,0x301
	b       ..ppcMfspr_any_end
	mfspr   r3,0x302
	b       ..ppcMfspr_any_end
	mfspr   r3,0x303
	b       ..ppcMfspr_any_end
	mfspr   r3,0x304
	b       ..ppcMfspr_any_end
	mfspr   r3,0x305
	b       ..ppcMfspr_any_end
	mfspr   r3,0x306
	b       ..ppcMfspr_any_end
	mfspr   r3,0x307
	b       ..ppcMfspr_any_end
	mfspr   r3,0x308
	b       ..ppcMfspr_any_end
	mfspr   r3,0x309
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x30f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x310
	b       ..ppcMfspr_any_end
	mfspr   r3,0x311
	b       ..ppcMfspr_any_end
	mfspr   r3,0x312
	b       ..ppcMfspr_any_end
	mfspr   r3,0x313
	b       ..ppcMfspr_any_end
	mfspr   r3,0x314
	b       ..ppcMfspr_any_end
	mfspr   r3,0x315
	b       ..ppcMfspr_any_end
	mfspr   r3,0x316
	b       ..ppcMfspr_any_end
	mfspr   r3,0x317
	b       ..ppcMfspr_any_end
	mfspr   r3,0x318
	b       ..ppcMfspr_any_end
	mfspr   r3,0x319
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x31f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x320
	b       ..ppcMfspr_any_end
	mfspr   r3,0x321
	b       ..ppcMfspr_any_end
	mfspr   r3,0x322
	b       ..ppcMfspr_any_end
	mfspr   r3,0x323
	b       ..ppcMfspr_any_end
	mfspr   r3,0x324
	b       ..ppcMfspr_any_end
	mfspr   r3,0x325
	b       ..ppcMfspr_any_end
	mfspr   r3,0x326
	b       ..ppcMfspr_any_end
	mfspr   r3,0x327
	b       ..ppcMfspr_any_end
	mfspr   r3,0x328
	b       ..ppcMfspr_any_end
	mfspr   r3,0x329
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x32f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x330
	b       ..ppcMfspr_any_end
	mfspr   r3,0x331
	b       ..ppcMfspr_any_end
	mfspr   r3,0x332
	b       ..ppcMfspr_any_end
	mfspr   r3,0x333
	b       ..ppcMfspr_any_end
	mfspr   r3,0x334
	b       ..ppcMfspr_any_end
	mfspr   r3,0x335
	b       ..ppcMfspr_any_end
	mfspr   r3,0x336
	b       ..ppcMfspr_any_end
	mfspr   r3,0x337
	b       ..ppcMfspr_any_end
	mfspr   r3,0x338
	b       ..ppcMfspr_any_end
	mfspr   r3,0x339
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x33f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x340
	b       ..ppcMfspr_any_end
	mfspr   r3,0x341
	b       ..ppcMfspr_any_end
	mfspr   r3,0x342
	b       ..ppcMfspr_any_end
	mfspr   r3,0x343
	b       ..ppcMfspr_any_end
	mfspr   r3,0x344
	b       ..ppcMfspr_any_end
	mfspr   r3,0x345
	b       ..ppcMfspr_any_end
	mfspr   r3,0x346
	b       ..ppcMfspr_any_end
	mfspr   r3,0x347
	b       ..ppcMfspr_any_end
	mfspr   r3,0x348
	b       ..ppcMfspr_any_end
	mfspr   r3,0x349
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x34f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x350
	b       ..ppcMfspr_any_end
	mfspr   r3,0x351
	b       ..ppcMfspr_any_end
	mfspr   r3,0x352
	b       ..ppcMfspr_any_end
	mfspr   r3,0x353
	b       ..ppcMfspr_any_end
	mfspr   r3,0x354
	b       ..ppcMfspr_any_end
	mfspr   r3,0x355
	b       ..ppcMfspr_any_end
	mfspr   r3,0x356
	b       ..ppcMfspr_any_end
	mfspr   r3,0x357
	b       ..ppcMfspr_any_end
	mfspr   r3,0x358
	b       ..ppcMfspr_any_end
	mfspr   r3,0x359
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x35f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x360
	b       ..ppcMfspr_any_end
	mfspr   r3,0x361
	b       ..ppcMfspr_any_end
	mfspr   r3,0x362
	b       ..ppcMfspr_any_end
	mfspr   r3,0x363
	b       ..ppcMfspr_any_end
	mfspr   r3,0x364
	b       ..ppcMfspr_any_end
	mfspr   r3,0x365
	b       ..ppcMfspr_any_end
	mfspr   r3,0x366
	b       ..ppcMfspr_any_end
	mfspr   r3,0x367
	b       ..ppcMfspr_any_end
	mfspr   r3,0x368
	b       ..ppcMfspr_any_end
	mfspr   r3,0x369
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x36f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x370
	b       ..ppcMfspr_any_end
	mfspr   r3,0x371
	b       ..ppcMfspr_any_end
	mfspr   r3,0x372
	b       ..ppcMfspr_any_end
	mfspr   r3,0x373
	b       ..ppcMfspr_any_end
	mfspr   r3,0x374
	b       ..ppcMfspr_any_end
	mfspr   r3,0x375
	b       ..ppcMfspr_any_end
	mfspr   r3,0x376
	b       ..ppcMfspr_any_end
	mfspr   r3,0x377
	b       ..ppcMfspr_any_end
	mfspr   r3,0x378
	b       ..ppcMfspr_any_end
	mfspr   r3,0x379
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x37f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x380
	b       ..ppcMfspr_any_end
	mfspr   r3,0x381
	b       ..ppcMfspr_any_end
	mfspr   r3,0x382
	b       ..ppcMfspr_any_end
	mfspr   r3,0x383
	b       ..ppcMfspr_any_end
	mfspr   r3,0x384
	b       ..ppcMfspr_any_end
	mfspr   r3,0x385
	b       ..ppcMfspr_any_end
	mfspr   r3,0x386
	b       ..ppcMfspr_any_end
	mfspr   r3,0x387
	b       ..ppcMfspr_any_end
	mfspr   r3,0x388
	b       ..ppcMfspr_any_end
	mfspr   r3,0x389
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x38f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x390
	b       ..ppcMfspr_any_end
	mfspr   r3,0x391
	b       ..ppcMfspr_any_end
	mfspr   r3,0x392
	b       ..ppcMfspr_any_end
	mfspr   r3,0x393
	b       ..ppcMfspr_any_end
	mfspr   r3,0x394
	b       ..ppcMfspr_any_end
	mfspr   r3,0x395
	b       ..ppcMfspr_any_end
	mfspr   r3,0x396
	b       ..ppcMfspr_any_end
	mfspr   r3,0x397
	b       ..ppcMfspr_any_end
	mfspr   r3,0x398
	b       ..ppcMfspr_any_end
	mfspr   r3,0x399
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39a
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39b
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39c
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39d
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39e
	b       ..ppcMfspr_any_end
	mfspr   r3,0x39f
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3a9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3aa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ab
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ac
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ad
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ae
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3af
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3b9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ba
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3bb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3bc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3bd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3be
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3bf
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3c9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ca
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3cb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3cc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3cd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ce
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3cf
	b       ..ppcMfspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMfspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMfspr_any_end
	addi	r3,r0,0x0000
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3d9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3da
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3db
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3dc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3dd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3de
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3df
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3e9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ea
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3eb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ec
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ed
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ee
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ef
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f0
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f1
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f2
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f3
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f4
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f5
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f6
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f7
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f8
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3f9
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3fa
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3fb
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3fc
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3fd
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3fe
	b       ..ppcMfspr_any_end
	mfspr   r3,0x3ff
	b       ..ppcMfspr_any_end
..ppcMfspr_any_end:
	mtlr	r6
	blr
	function_epilog(ppcMfspr_any)

/*----------------------------------------------------------------------------+
| PpcCachelinesize
+----------------------------------------------------------------------------*/
	function_prolog(ppcCachelinesize)
	addi	r3,r0,0x0080
	blr
	function_epilog(ppcCachelinesize)

/*----------------------------------------------------------------------------+
| PpcProcid
+----------------------------------------------------------------------------*/
	function_prolog(ppcProcid)
	addi	r3,r0,0x03CA
	blr
	function_epilog(ppcProcid)

/*----------------------------------------------------------------------------+
| PpcMtmmucr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMtmmucr)
	blr
	function_epilog(ppcMtmmucr)

/*----------------------------------------------------------------------------+
| PpcMttlb1
+----------------------------------------------------------------------------*/
	function_prolog(ppcMttlb1)
	blr
	function_epilog(ppcMttlb1)

/*----------------------------------------------------------------------------+
| PpcMttlb2
+----------------------------------------------------------------------------*/
	function_prolog(ppcMttlb2)
	blr
	function_epilog(ppcMttlb2)

/*----------------------------------------------------------------------------+
| PpcMttlb3
+----------------------------------------------------------------------------*/
	function_prolog(ppcMttlb3)
	blr
	function_epilog(ppcMttlb3)

/*----------------------------------------------------------------------------+
| PpcMftlb1
+----------------------------------------------------------------------------*/
	function_prolog(ppcMftlb1)
	addis	r3,r0,0xdead
	ori	r3,r3,0xbeef
	blr
	function_epilog(ppcMftlb1)

/*----------------------------------------------------------------------------+
| PpcMftlb2
+----------------------------------------------------------------------------*/
	function_prolog(ppcMftlb2)
	addis	r3,r0,0xdead
	ori	r3,r3,0xbeef
	blr
	function_epilog(ppcMftlb2)

/*----------------------------------------------------------------------------+
| PpcMftlb3
+----------------------------------------------------------------------------*/
	function_prolog(ppcMftlb3)
	addis	r3,r0,0xdead
	ori	r3,r3,0xbeef
	blr
	function_epilog(ppcMftlb3)

/*----------------------------------------------------------------------------+
| PpcMfmmucr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfmmucr)
	addis	r3,r0,0xdead
	ori	r3,r3,0xbeef
	blr
	function_epilog(ppcMfmmucr)

/*----------------------------------------------------------------------------+
| PpcMfdcr_any
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfdcr_any)
	addis	r3,r0,0xdead
	ori	r3,r3,0xbeef
	blr
	function_epilog(ppcMfdcr_any)

/*----------------------------------------------------------------------------+
| PpcMtdcr_any
+----------------------------------------------------------------------------*/
	function_prolog(ppcMtdcr_any)
	blr
	function_epilog(ppcMtdcr_any)

/*----------------------------------------------------------------------------+
| PpcIstrap.
+----------------------------------------------------------------------------*/
	function_prolog(ppcIstrap)
	mfspr	r3,srr1
	rlwinm.	r3,r3,0,13,15
	bne	..is_trap
	addi	r3,r0,0x0000
	b	..is_return
..is_trap:
	addi	r3,r0,0x0001
..is_return:
	blr
	function_epilog(ppcIstrap)

/*----------------------------------------------------------------------------+
| P_ptegg.  r3=large page inidicator, r4=ea, r5 = sdr1, r6 = vsid.
+----------------------------------------------------------------------------*/
	function_prolog(p_ptegg)
	cmpi	cr0,1,r3,0x0000
	bne	..lp
	addi	r3,r0,12
	addi	r8,r0,0x0001
	rlwinm	r8,r8,16,0,31
	addi	r8,r8,-1
	b	..ppast
..lp:   addi	r3,r0,24
	addi	r8,r0,0x0001
	rlwinm	r8,r8,4,0,31
	addi	r8,r8,-1
        /*--------------------------------------------------------------------+
        | Only lower 39 bits of VSID are used in hash function.
        +--------------------------------------------------------------------*/
..ppast:rldicl	r6,r6,0,25
        /*--------------------------------------------------------------------+
        | Discard page offset in effective address, only bits specified in 
        | the mask (r8) are used.
        +--------------------------------------------------------------------*/
	srd	r4,r4,r3
	and	r4,r4,r8
        /*--------------------------------------------------------------------+
        | Perform hash function.
        +--------------------------------------------------------------------*/
	xor	r6,r6,r4
        /*--------------------------------------------------------------------+
        | 11 lowest bits from hash function are used directly.
        +--------------------------------------------------------------------*/
	rlwinm	r3,r6,7,14,24
        /*--------------------------------------------------------------------+
        | Shift output of the hash function by 11 bits.
        +--------------------------------------------------------------------*/
	sradi	r4,r6,11
        /*--------------------------------------------------------------------+
        | Calculate mask from sdr1.htabsize and AND it with upper 28 bits 
	| of the hash function result.
        +--------------------------------------------------------------------*/
	rldicl	r7,r5,0,59
	addi	r8,r0,0x0001
	slw	r8,r8,r7
	addi	r8,r8,-1
	and	r8,r8,r4
        /*--------------------------------------------------------------------+
        | Or in the 28+ 16 bits of the sdr1.htaborg.
        +--------------------------------------------------------------------*/
	rldicl	r5,r5,0,2
	addi	r4,r0,0x0012
	srd	r5,r5,r4
	or	r4,r5,r8
	rldicr	r4,r4,18,46
	or	r3,r3,r4
	blr
	function_epilog(p_ptegg)

/*----------------------------------------------------------------------------+
| S_ptegg.  r3=large page inidicator, r4=ea, r5 = sdr1, r6 = vsid.
+----------------------------------------------------------------------------*/
	function_prolog(s_ptegg)
	cmpi	cr0,1,r3,0x0000
	bne	..lps
	addi	r3,r0,12
	addi	r8,r0,0x0001
	rlwinm	r8,r8,16,0,31
	addi	r8,r8,-1
	b	..spast
..lps:  addi	r3,r0,24
	addi	r8,r0,0x0001
	rlwinm	r8,r8,4,0,31
	addi	r8,r8,-1
        /*--------------------------------------------------------------------+
        | Only lower 39 bits of VSID are used in hash function.
        +--------------------------------------------------------------------*/
..spast:rldicl	r6,r6,0,25
        /*--------------------------------------------------------------------+
        | Discard page offset in effective address, only bits specified in 
        | the mask (r8) are used.
        +--------------------------------------------------------------------*/
	srd	r4,r4,r3
	and	r4,r4,r8
        /*--------------------------------------------------------------------+
        | Perform hash function.
        +--------------------------------------------------------------------*/
	xor	r6,r6,r4
	nand    r6,r6,r6
        /*--------------------------------------------------------------------+
        | 11 lowest bits from hash function are used directly.
        +--------------------------------------------------------------------*/
	rlwinm	r3,r6,7,14,24
        /*--------------------------------------------------------------------+
        | Shift output of the hash function by 11 bits.
        +--------------------------------------------------------------------*/
	sradi	r4,r6,11
        /*--------------------------------------------------------------------+
        | Calculate mask from sdr1.htabsize and AND it with upper 28 bits 
	| of the hash function result.
        +--------------------------------------------------------------------*/
	rldicl	r7,r5,0,59
	addi	r8,r0,0x0001
	slw	r8,r8,r7
	addi	r8,r8,-1
	and	r8,r8,r4
        /*--------------------------------------------------------------------+
        | Or in the 28+ 16 bits of the sdr1.htaborg.
        +--------------------------------------------------------------------*/
	rldicl	r5,r5,0,2
	addi	r4,r0,0x0012
	srd	r5,r5,r4
	or	r4,r5,r8
	rldicr	r4,r4,18,46
	or	r3,r3,r4
	blr
	function_epilog(s_ptegg)

/*----------------------------------------------------------------------------+
| ppcLwsync.
+----------------------------------------------------------------------------*/
	function_prolog(ppcLwsync)
	sync	1
	blr
	function_epilog(ppcLwsync)

/*----------------------------------------------------------------------------+
| ppcPtesync.
+----------------------------------------------------------------------------*/
	function_prolog(ppcPtesync)
	sync	2
	blr
	function_epilog(ppcPtesync)

/*----------------------------------------------------------------------------+
| ppcTestandset.  r3=address, r4=value.
+----------------------------------------------------------------------------*/
	function_prolog(ppcTestandset)
..again:lwarx	r5,r0,r3
	cmpwi	cr0,r5,0x0000
	bne	..again
	stwcx.	r4,r0,r3
	bne	..again
	blr
	function_epilog(ppcTestandset)

/*----------------------------------------------------------------------------+
| ppcSlbmte.
+----------------------------------------------------------------------------*/
	function_prolog(ppcSlbmte)
	slbmte	r3,r4
	blr
	function_epilog(ppcSlbmte)

/*----------------------------------------------------------------------------+
| ppcSlbie.
+----------------------------------------------------------------------------*/
	function_prolog(ppcSlbie)
	slbie	r3
	blr
	function_epilog(ppcSlbie)

/*----------------------------------------------------------------------------+
| ppcSlbia.
+----------------------------------------------------------------------------*/
	function_prolog(ppcSlbia)
	slbia
	blr
	function_epilog(ppcSlbia)

/*----------------------------------------------------------------------------+
| ppcSlbmfev.
+----------------------------------------------------------------------------*/
	function_prolog(ppcSlbmfev)
	slbmfev	r3,r3
	blr
	function_epilog(ppcSlbmfev)

/*----------------------------------------------------------------------------+
| ppcSlbmfee.
+----------------------------------------------------------------------------*/
	function_prolog(ppcSlbmfee)
	slbmfee	r3,r3
	blr
	function_epilog(ppcSlbmfee)

/*----------------------------------------------------------------------------+
| ppcTlbiel.
+----------------------------------------------------------------------------*/
	function_prolog(ppcTlbiel)
	TLBIEL(r3)
	blr
	function_epilog(ppcTlbiel)

/*----------------------------------------------------------------------------+
| PpcStvx
+----------------------------------------------------------------------------*/
	function_prolog(ppcStvx)
        rlwinm	r3,r3,3,19,29
	addi	r3,r3,0x0010
	mflr	r6
	bl	..vr_sett_lr
..vr_sett_lr:
	mflr	r5
	add 	r5,r5,r3
	mtlr	r5
	blr
	stvx	0,r0,r4
	b       ..ppcStvx_any_end
	stvx    1,r0,r4
	b       ..ppcStvx_any_end
	stvx    2,r0,r4
	b       ..ppcStvx_any_end
	stvx    3,r0,r4
	b       ..ppcStvx_any_end
	stvx    4,r0,r4
	b       ..ppcStvx_any_end
	stvx    5,r0,r4
	b       ..ppcStvx_any_end
	stvx    6,r0,r4
	b       ..ppcStvx_any_end
	stvx    7,r0,r4
	b       ..ppcStvx_any_end
	stvx    8,r0,r4
	b       ..ppcStvx_any_end
	stvx    9,r0,r4
	b       ..ppcStvx_any_end
	stvx    10,r0,r4
	b       ..ppcStvx_any_end
	stvx    11,r0,r4
	b       ..ppcStvx_any_end
	stvx    12,r0,r4
	b       ..ppcStvx_any_end
	stvx    13,r0,r4
	b       ..ppcStvx_any_end
	stvx    14,r0,r4
	b       ..ppcStvx_any_end
	stvx    15,r0,r4
	b       ..ppcStvx_any_end
	stvx    16,r0,r4
	b       ..ppcStvx_any_end
	stvx    17,r0,r4
	b       ..ppcStvx_any_end
	stvx    18,r0,r4
	b       ..ppcStvx_any_end
	stvx    19,r0,r4
	b       ..ppcStvx_any_end
	stvx    20,r0,r4
	b       ..ppcStvx_any_end
	stvx    21,r0,r4
	b       ..ppcStvx_any_end
	stvx    22,r0,r4
	b       ..ppcStvx_any_end
	stvx    23,r0,r4
	b       ..ppcStvx_any_end
	stvx    24,r0,r4
	b       ..ppcStvx_any_end
	stvx    25,r0,r4
	b       ..ppcStvx_any_end
	stvx    26,r0,r4
	b       ..ppcStvx_any_end
	stvx    27,r0,r4
	b       ..ppcStvx_any_end
	stvx    28,r0,r4
	b       ..ppcStvx_any_end
	stvx    29,r0,r4
	b       ..ppcStvx_any_end
	stvx    30,r0,r4
	b       ..ppcStvx_any_end
	stvx    31,r0,r4
	b       ..ppcStvx_any_end
..ppcStvx_any_end:
	mtlr	r6
	blr
	function_epilog(ppcStvx)

/*----------------------------------------------------------------------------+
| PpcLvxl
+----------------------------------------------------------------------------*/
	function_prolog(ppcLvxl)
        rlwinm	r3,r3,3,19,29
	addi	r3,r3,0x0010
	mflr	r6
	bl	..vr_gett_lr
..vr_gett_lr:
	mflr	r5
	add 	r5,r5,r3
	mtlr	r5
	blr
	lvxl	0,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    1,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    2,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    3,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    4,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    5,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    6,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    7,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    8,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    9,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    10,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    11,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    12,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    13,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    14,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    15,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    16,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    17,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    18,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    19,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    20,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    21,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    22,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    23,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    24,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    25,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    26,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    27,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    28,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    29,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    30,r0,r4
	b       ..ppcLvxl_any_end
	lvxl    31,r0,r4
	b       ..ppcLvxl_any_end
..ppcLvxl_any_end:
	mtlr	r6
	blr
	function_epilog(ppcLvxl)

/*----------------------------------------------------------------------------+
| PpcMfvscr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMfvscr)
	mfvscr	0
	blr
	function_epilog(ppcMfvscr)

/*----------------------------------------------------------------------------+
| PpcMtvscr
+----------------------------------------------------------------------------*/
	function_prolog(ppcMtvscr)
	mtvscr	0
	blr
	function_epilog(ppcMtvscr)
