/*bsp_970fx/bootlib/init_core.s, pibs_970, pibs_970_1.0 1/14/05 14:58:41*/
/*----------------------------------------------------------------------------+
|       COPYRIGHT   I B M   CORPORATION 2002, 2004
|       LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
|	US Government Users Restricted Rights - Use, duplication or
|       disclosure restricted by GSA ADP Schedule Contract with
|	IBM Corp.
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| PPC970FX BSP for EPOS
| Author: Maciej P. Tyrlik
| Component: Boot library.
| File: init_core.s
| Purpose: Basic PPC405 core initialization.
| Changes:
| Date:         Comment:
| -----         --------
| 29-Jan-02     Created                                                     MPT
| 30-Jan-02     Completed                                                   MPT
| 19-Apr-02     Changed some instructions to macros so that new GCC AS worksMPT
| 23-Apr-02     Removed critical interrupt enabling after rfi               MPT
| 31-Jul-02     Fixed data cache invalidate code                            MPT
| 01-Feb-03     Ported to Argan 7XXFX                                       CRB
| 07-Aug-03     Ported to PPC7XXGX                                          CRB
| 12-Sep-03     Removed PVR definitions, now in board include file          MCG
| 16-Sep-03     Do not enable HID0[MUM] or L2CR[L2CE] if 7XXGX DD1.0        MCG
| 31-Oct-03     Enable cache for MV64460 integrated SRAM                    MCG
| 07-Jan-04     Initialize FPRs to avoid errata.                            MCG
| 10-Feb-04     Port to PPC970FX					    MPT
+----------------------------------------------------------------------------*/

#include <ppc970.h>

/*----------------------------------------------------------------------------+
| Local defines.
+----------------------------------------------------------------------------*/
#define INITIAL_SLB_VSID_VAL		0x0000000000000C00
#define INITIAL_SLB_ESID_VAL		0x0000000008000000
#define INITIAL_SLB_INVA_VAL		0x0000000000000000

/*----------------------------------------------------------------------------+
| Init_core.  Assumption: hypervisor on, 64-bit on, HID1[10]=0, HID4[23]=0.
| Data cahability must be turned on.  Instruction cahability must be off.
+----------------------------------------------------------------------------*/
        /*--------------------------------------------------------------------+
        | Set time base to 0.
        +--------------------------------------------------------------------*/
	addi	r4,r0,0x0000
	mtspr	SPR_TBU_WRITE,r4
	mtspr	SPR_TBL_WRITE,r4
        /*--------------------------------------------------------------------+
        | Set HID1[10] to 0 (instruction cache off) and set HID4[23] to 0 (data
        | cache on), set HID4[DC_SET1] and HID4[DC_SET2] to 0.
        +--------------------------------------------------------------------*/
        LOAD_64BIT_VAL(r4,HID1_EN_IC)
	nor	r4,r4,r4
	mfspr	r5,SPR_HID1
	isync
	and	r5,r5,r4
	mtspr	SPR_HID1,r5	
	mtspr	SPR_HID1,r5	
	isync
        LOAD_64BIT_VAL(r4,HID4_RM_CI|HID4_DC_SET1|HID4_DC_SET2)
	nor	r4,r4,r4
	mfspr	r5,SPR_HID4
        LOAD_64BIT_VAL(r6,HID4_L1DC_FLSH)
	isync
	and	r5,r5,r4
	or	r5,r5,r6
	sync
	mtspr	SPR_HID4,r5	
	isync
        /*--------------------------------------------------------------------+
        | Clear the flash invalidate L1 data cache bit in HID4.
        +--------------------------------------------------------------------*/
	nor	r6,r6,r6
	and	r5,r5,r6
	sync
	mtspr	SPR_HID4,r5	
	isync
        /*--------------------------------------------------------------------+
        | Clear and set up some registers.
        +--------------------------------------------------------------------*/
	addi    r4,r0,0x0000
        mtxer   r4
        /*--------------------------------------------------------------------+
        | Invalidate SLB.  First load SLB with known values then perform 
        | invalidate.  Invalidate will clear the D-ERAT and I-ERAT.  The SLB
	| is 64 entry fully associative.  On power on D-ERAT and I-ERAT are all
        | set to invalid values.
        +--------------------------------------------------------------------*/
	addi	r5,r0,SLB_SIZE
	mtctr	r5
	LOAD_64BIT_VAL(r6,INITIAL_SLB_VSID_VAL)
	LOAD_64BIT_VAL(r7,INITIAL_SLB_ESID_VAL)
	addis	r8,r0,0x1000
..slbl: slbmte	r6,r7
	addi	r6,r6,0x1000
	add  	r7,r7,r8
	addi	r7,r7,0x0001
	bdnz	..slbl
	mtctr	r5
	LOAD_64BIT_VAL(r6,INITIAL_SLB_INVA_VAL)
..slbi: slbie	r6
	add  	r6,r6,r8
	bdnz	..slbi
        /*--------------------------------------------------------------------+
        | Load SLB.  Following is the initial memory map.
	| Entry(6) ESID(36)    VSID
	| 0x0	   0x000000000 0x0000000000000 (large page cachable)
	| 0x1      0x00000000F 0x000000000000F (small non-cachable, G)
	| at 0x00000000 there will be 48MB mapped (SDRAM)
	| at 0xF8000000 there will be 16MB mapped (NB)
	| at 0xF4000000 there will be 64KB mapped (I/O space)
        | at 0xFF000000 there will be 16MB or 1MB mapped (FLASH)
        +--------------------------------------------------------------------*/
	addi	r6,r0,0x0100
	addis	r7,r0,0x0800
	slbmte	r6,r7
	addi	r6,r0,0x0000
	ori  	r6,r6,0xF000
	addi 	r7,r0,0x0001
	oris	r7,r7,0xF800
	slbmte	r6,r7
        /*--------------------------------------------------------------------+
        | Invalidate all 1024 instruction and data TLBs (4 way)
        +--------------------------------------------------------------------*/
        addi    r8,r0,0x0100
        mtspr   ctr,r8
        addi    r8,r0,0x0000
..ivt:  TLBIEL(r8)
        addi    r8,r8,0x1000
        bdnz    ..ivt
        ptesync
        /*--------------------------------------------------------------------+
        | Dcbz the page table space.   Calculate SDR1 address.  Store SDR1
        | address in r30.
        +--------------------------------------------------------------------*/
	mfspr	r3,SPR_PIR
	cmpi	cr0,1,r3,0x0000
	bne	..cpu1
        addis   r3,r0,INITIAL_PAGE_TABLE_ADDR_CPU0@h
        ori     r3,r3,INITIAL_PAGE_TABLE_ADDR_CPU0@l
	b	..skcpu
..cpu1: addis   r3,r0,INITIAL_PAGE_TABLE_ADDR_CPU1@h
        ori     r3,r3,INITIAL_PAGE_TABLE_ADDR_CPU1@l
..skcpu:addis   r4,r0,INITIAL_PAGE_TABLE_SIZE@h
        ori     r4,r4,INITIAL_PAGE_TABLE_SIZE@l
	rlwinm	r5,r4,14,14,31 
	cntlzw	r5,r5
	subfic	r5,r5,31
	or	r30,r3,r5
        bl      .ppcDcbz_area
        /*--------------------------------------------------------------------+
        | Setup 0x00000000FFFFFFFF mask in r29. 
        +--------------------------------------------------------------------*/
	addi	r29,r0,0x0001
	rldicl	r29,r29,32,31
	addi	r29,r29,-1
        /*--------------------------------------------------------------------+
        | Setup 48MB of addresses in DRAM in page table (3 large PTE).  The 
        | parameters to p_ptegg are: r3 = lp, r4 = ea, r5 = sdr1, r6 = vsid.
        +--------------------------------------------------------------------*/
	addi	r3,r0,0x0001
	addi	r4,r0,0x0000
        ori     r5,r30,0x0000
	addi	r6,r0,0x0000
	bl	.p_ptegg
	addi	r4,r0,0x0001
	stw	r4,0x0004(r3)
	addi	r4,r0,0x0180
	stw	r4,0x000C(r3)
        /*--------------------------------------------------------------------+
        | Second 16MB is mapped here.
        +--------------------------------------------------------------------*/
	addi	r3,r0,0x0001
	addis	r4,r0,0x0100
        ori     r5,r30,0x0000
	addi	r6,r0,0x0000
	bl	.p_ptegg
	addi	r4,r0,0x0101
	stw	r4,0x0004(r3)
	addis	r4,r0,0x0100
	ori     r4,r4,0x0180
	stw	r4,0x000C(r3)
        /*--------------------------------------------------------------------+
        | Third 16MB is mapped here.
        +--------------------------------------------------------------------*/
	addi	r3,r0,0x0001
	addis	r4,r0,0x0200
        ori     r5,r30,0x0000
	addi	r6,r0,0x0000
	bl	.p_ptegg
	addi	r4,r0,0x0201
	stw	r4,0x0004(r3)
	addis	r4,r0,0x0200
	ori     r4,r4,0x0180
	stw	r4,0x000C(r3)
        /*--------------------------------------------------------------------+
        | Setup 64KB of addresses in I/O space (0xF4000000).
        +--------------------------------------------------------------------*/
	addi	r3,r0,0x0010
	mtctr	r3
	addis	r31,r0,0xF400
	and	r31,r31,r29
..aF4:	addi	r3,r0,0x0000
	ori	r4,r31,0x0000
	ori	r5,r30,0x0000
	addi	r6,r0,0x000F
	bl	.p_ptegg
	addi	r6,r3,0x0080
..aF4a:	lwz	r4,0x0004(r3)
	cmpli	cr0,1,r4,0x0000
	beq	..aF4s
	addi	r3,r3,0x0010
	cmp	cr0,1,r3,r6
	blt	..aF4a
..aF4h:	b	..aF4h
..aF4s: rlwinm  r4,r31,16,4,24
	ori	r4,r4,0x0001
	stw	r4,0x0004(r3)
	ori	r4,r31,0x01AC
	stw	r4,0x000C(r3)
	addi	r31,r31,0x1000
	bdnz	..aF4
        /*--------------------------------------------------------------------+
        | Setup 16MB of addresses in NB register space (0xF8000000).
        +--------------------------------------------------------------------*/
	addi	r3,r0,0x1000
	mtctr	r3
	addis	r31,r0,0xF800
	and	r31,r31,r29
..aF8:	addi	r3,r0,0x0000
	ori	r4,r31,0x0000
	ori	r5,r30,0x0000
	addi	r6,r0,0x000F
	bl	.p_ptegg
	addi	r6,r3,0x0080
..aF8a:	lwz	r4,0x0004(r3)
	cmpli	cr0,1,r4,0x0000
	beq	..aF8s
	addi	r3,r3,0x0010
	cmp	cr0,1,r3,r6
	blt	..aF8a
..aF8h:	b	..aF8h
..aF8s: rlwinm	r4,r31,16,4,24
	ori	r4,r4,0x0001
	stw	r4,0x0004(r3)
	ori	r4,r31,0x01AC
	stw	r4,0x000C(r3)
	addi	r31,r31,0x1000
	bdnz	..aF8
        /*--------------------------------------------------------------------+
        | Setup 16MB or 1MB of addresses in ROM (at 0xFF000000 or 0xFFF00000).
        +--------------------------------------------------------------------*/
	mfspr	r3,SPR_HIOR
	LOAD_64BIT_VAL(r4,BOOT_BASE_AS)
	cmpd	cr0,r3,r4
	beq	..big
	addi	r3,r0,0x0100
	mtctr	r3
	addis	r31,r0,0xFFF0
	b	..done
..big:  addi	r3,r0,0x1000
	mtctr	r3
	addis	r31,r0,0xFF00
..done:	and	r31,r31,r29
..aFF:	addi	r3,r0,0x0000
	ori	r4,r31,0x0000
	ori	r5,r30,0x0000
	addi	r6,r0,0x000F
	bl	.p_ptegg
	addi	r6,r3,0x0080
..aFFa:	lwz	r4,0x0004(r3)
	cmpli	cr0,1,r4,0x0000
	beq	..aFFs
	addi	r3,r3,0x0010
	cmp	cr0,1,r3,r6
	blt	..aFFa
..aFFh:	b	..aFFh
..aFFs: rlwinm	r4,r31,16,4,24
	ori	r4,r4,0x0001
	stw	r4,0x0004(r3)
	ori	r4,r31,0x01A3
	stw	r4,0x000C(r3)
	addi	r31,r31,0x1000
	bdnz	..aFF
        /*--------------------------------------------------------------------+
        | Synchronize after setting up page table.
        +--------------------------------------------------------------------*/
	ptesync
        /*--------------------------------------------------------------------+
        | Set the SDR1 register.
        +--------------------------------------------------------------------*/
	mtspr	SPR_SDR1,r30
        /*--------------------------------------------------------------------+
        | Clear SRR0, SRR1.
        +--------------------------------------------------------------------*/
        addi    r0,r0,0x0000
        mtspr   SPR_SRR0,r0
        mtspr   SPR_SRR1,r0
        /*--------------------------------------------------------------------+
        | Setup for subsequent MSR[ME] initialization to enable machine checks
        | and translation.
        +--------------------------------------------------------------------*/
        mfmsr   r3
        ori     r3,r3,(MSR_ME|MSR_IS|MSR_DS|MSR_FP)
        mtsrr1  r3
	mtmsrd	r3,0
	isync
        /*--------------------------------------------------------------------+
        | Setup HID registers (HID0, HID1, HID4, HID5).  When HIOR is set to
        | 0 HID0 external time base bit is inherited from current HID0. When
	| HIOR is set to FLASH_BASE_INTEL_AS then HID0 external time base bit
        | is set to 1 in order to indicate that the tiembase is driven by
        | external source. When HIOR is greater than FLASH_BASE_INTEL_AS then
        | HID0 external time base bit is set to 0 in order to indicate that the
        | tiembase is driven from internal clock.
        +--------------------------------------------------------------------*/
	LOAD_64BIT_VAL(r6,HID0_EXT_TB_EN)
	LOAD_64BIT_VAL(r7,FLASH_BASE_INTEL_AS)
	mfspr	r5,SPR_HIOR
	cmpdi	cr0,r5,0x0000
	beq	..hior0
	cmpd	cr0,r5,r7
	beq	..hiorl
	addi	r8,r0,0x0000	
	b	..hiors
..hiorl:ori	r8,r6,0x0000
	b	..hiors
..hior0:mfspr	r5,SPR_HID0
	and 	r8,r5,r6
..hiors:LOAD_64BIT_VAL(r4,HID0_PREFEAR)
       	andc	r4,r4,r6
	or	r4,r4,r8 
	sync
	mtspr	SPR_HID0,r4
	mfspr	r4,SPR_HID0
	mfspr	r4,SPR_HID0
	mfspr	r4,SPR_HID0
	mfspr	r4,SPR_HID0
	mfspr	r4,SPR_HID0
	mfspr	r4,SPR_HID0
	LOAD_64BIT_VAL(r4,HID1_PREFEAR)
	mtspr	SPR_HID1,r4
	mtspr	SPR_HID1,r4
	isync
	LOAD_64BIT_VAL(r4,HID4_PREFEAR)
	sync	
	mtspr	SPR_HID4,r4
	isync
	sync
	LOAD_64BIT_VAL(r4,HID5_PREFEAR)
	mtspr   SPR_HID5,r4
	isync
	/*--------------------------------------------------------------------+
        | Synchronize memory accesses (sync).
        +--------------------------------------------------------------------*/
        sync
	LOAD_64BIT_VAL(r0,.init_chip)
	mfspr	r1,SPR_HIOR
	or	r0,r0,r1
        eieio
        mtspr   SPR_SRR0,r0
        rfid
