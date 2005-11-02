
#include <ppc970.h>

/******** init_core.s ***************/
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
function_prolog(init_core)
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
	| at 0x00000000 there will be 32MB mapped (SDRAM)
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
	bne	..cpu1_init_core
        addis   r3,r0,INITIAL_PAGE_TABLE_ADDR_CPU0@h
        ori     r3,r3,INITIAL_PAGE_TABLE_ADDR_CPU0@l
	b	..skcpu
..cpu1_init_core: addis   r3,r0,INITIAL_PAGE_TABLE_ADDR_CPU1@h
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
        | Setup 32MB of addresses in DRAM in page table (2 large PTE).  The 
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
        | Second 32MB is mapped here.
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
        function_epilog(init_core)


/******** init_chip.s ***************/
/*----------------------------------------------------------------------------+
| Local defines.
+----------------------------------------------------------------------------*/
#define CPU1_DELAY			0x00010000

/*----------------------------------------------------------------------------+
| Init_chip.
+----------------------------------------------------------------------------*/
        function_prolog(init_chip)
        /*--------------------------------------------------------------------+
	| Skip if CPU1.
        +--------------------------------------------------------------------*/
	mfspr	r3,SPR_PIR
	cmpi	cr0,1,r3,0x0000
	bne	..cpu1
        /*--------------------------------------------------------------------+
        | Initialize the stack in the data cache for the "C" code that gets
        | called.
        +--------------------------------------------------------------------*/
        addis   r3,r0,BOOT_STACK_ADDR@h
        ori     r3,r3,BOOT_STACK_ADDR@l
        addis   r4,r0,BOOT_STACK_SIZE@h
        ori     r4,r4,BOOT_STACK_SIZE@l
        add     r1,r3,r4
        bl      .ppcDcbz_area
        addi    r1,r1,-stack_frame_min
        addi    r5,r0,0x0000
        std     r5,stack_frame_bc(r1)
        /*--------------------------------------------------------------------+
        | Load TOC.  Can't use ld since the TOC value might not be aligned on
	| double word boundary.
        +--------------------------------------------------------------------*/
	bl	..ot_init_chip
	.quad 	.TOC.@tocbase
..ot_init_chip:	mflr	r3
	lwz	r2,0x0000(r3)
	lwz	r3,0x0004(r3)
	rldicr	r2,r2,32,31
	or	r2,r2,r3
	mfspr	r3,SPR_HIOR
	or	r2,r2,r3
        /*--------------------------------------------------------------------+
        | Code for chip initialization code goes here.  Subtractive decoding
        | allows access to specified registers.
        +--------------------------------------------------------------------*/
	bl	.super_io_setup
        /*--------------------------------------------------------------------+
        | Setup default serial port using default baud rate.
        +--------------------------------------------------------------------*/
//	bl	.sinit_default_no_global
        /*--------------------------------------------------------------------+
        | Enable SDRAM only if running from FLASH.
        +--------------------------------------------------------------------*/
       	mflr	r3
	LOAD_64BIT_VAL(r4,BOOT_BASE_AS)
	cmpld	cr0,r3,r4
	blt	..skip
	bl	memory_init
        /*--------------------------------------------------------------------+
        | Check the memory where PIBS data section will be placed.
        +--------------------------------------------------------------------*/
..skip: bl      ..skip_data
        .string "\nMemory check failed at 0x%x, expected 0x%x, actual 0x%x"
	.align	2
..skip_data:
        addis   r3,r0,MEM_CHK_START_ADDR@h
        ori     r3,r3,MEM_CHK_START_ADDR@l
        addis   r4,r0,MEM_CHK_SIZE@h
        ori     r4,r4,MEM_CHK_SIZE@l
        mflr    r5
//        bl      mem_check
        /*--------------------------------------------------------------------+
        | Initialize RAM area that holds boot information for CPU1.
        +--------------------------------------------------------------------*/
	LOAD_64BIT_VAL(r31,CPU1_DATA_STRUCT_ADDR)
	addi	r3,r0,0x0000
	std	r3,CPU1_DATA_STRUCT_VALID_OFF(r31)
        /*--------------------------------------------------------------------+
        | DCBZ area stack is left in the cache since there is no way to 
        | invalidate data cache.  This area will be written to memory at some
 	| point.  Main memory should be functional at this point.
        +--------------------------------------------------------------------*/
        b       .init_data
        /*--------------------------------------------------------------------+
	| CPU1 will spin waiting for the CPU0 to initialize the system.  CPU1
        | then will check if the image for CPU1 has been loaded.  If the image
        | for CPU1 has been loaded CPU1 will jump to that image.  If the image
        | for CPU1 has not been loaded CPU1 will spin waiting for the image to
        | be loaded.
        +--------------------------------------------------------------------*/
..cpu1:	LOAD_64BIT_VAL(r31,NB_HW_INIT_STATE_ASM)
	lwz	r30,0x0000(r31)			
	cmpi	cr0,1,r30,0x0000
	beq	..cpu1
        /*--------------------------------------------------------------------+
        | Jump to SDRAM (cachable storage) and wait there.
        +--------------------------------------------------------------------*/
	sync
	ba	..loada
        /*--------------------------------------------------------------------+
        | Wait for image valid indicator.
        +--------------------------------------------------------------------*/
..loada:LOAD_64BIT_VAL(r31,CPU1_DATA_STRUCT_ADDR)
        ld 	r3,CPU1_DATA_STRUCT_VALID_OFF(r31)
	cmpi	cr0,1,r3,0x0000
	beq	..spin2
	ld 	r3,CPU1_DATA_STRUCT_SRR0_OFF(r31)
	mtspr	SPR_SRR0,r3
	ld 	r4,CPU1_DATA_STRUCT_SRR1_OFF(r31)
	mtspr	SPR_SRR1,r4
	ld 	r3,CPU1_DATA_STRUCT_R3_OFF(r31)
	isync
	rfid
..spin2:mfspr	r29,tblr
	LOAD_64BIT_VAL(r31,CPU1_DELAY)
..spin3:mfspr	r30,tblr
       	subf	r30,r29,r30
	cmp 	cr0,1,r30,r31
	blt	..spin3
	b	..loada
        function_epilog(init_chip)


/******** init_data.s ***************/
/*----------------------------------------------------------------------------+
| Init_data.
+----------------------------------------------------------------------------*/
        function_prolog(init_data)
	/*--------------------------------------------------------------------+
        | Check if we are running from FLASH.  If running from FLASH copy 1M
        | of FLASH to SDRAM.
        +--------------------------------------------------------------------*/
	bl	..next
..next:	mflr	r3
	LOAD_64BIT_VAL(r4,BOOT_BASE_AS)
	cmpld	cr0,r3,r4
	blt	..sk_c
        /*--------------------------------------------------------------------+
        | Perform the copy operation.  This copies data starting from SPR_HIOR
	| for number of bytes queal to __edata -  __stext.
        +--------------------------------------------------------------------*/
	LOAD_64BIT_VAL(r6,__stext)
	addi	r3,r6,-8
	mfspr	r4,SPR_HIOR
	addi 	r4,r4,-8
	LOAD_64BIT_VAL(r5,__edata);
	sub	r5,r5,r6
	rlwinm	r5,r5,29,3,31
	addi	r5,r5,0x0001
	mtctr	r5
..again1:ldu	r6,0x0008(r4)
	stdu	r6,0x0008(r3)
	bdnz	..again1
        /*--------------------------------------------------------------------+
        | Get the size of BSS into r6.
        +--------------------------------------------------------------------*/ 
..sk_c:	LOAD_64BIT_VAL(r4,__sbss)
        LOAD_64BIT_VAL(r5,__ebss)
        sub     r6,r5,r4
        /*--------------------------------------------------------------------+
        | Clear BSS.
        +--------------------------------------------------------------------*/
        addi    r8,r4,-1
        mtspr   ctr,r6
        addi    r9,r0,0x0000
..bag:  stbu    r9,0x0001(r8)
        bdnz    ..bag
        /*--------------------------------------------------------------------+
        | Synchronize.
        +--------------------------------------------------------------------*/
	sync
        ba      .init_cenv
        function_epilog(init_data)


/******** init_cenv.s ***************/
/*----------------------------------------------------------------------------+
| TOC entry for __initial_stack.
+----------------------------------------------------------------------------*/
TOC_ENTRY(.LC0,__initial_stack)

/*----------------------------------------------------------------------------+
| Initial stack.
+----------------------------------------------------------------------------*/
        data_prolog(__initial_stack)
        .space  MY_MAIN_STACK_SIZE
        data_epilog(__initial_stack)

/*----------------------------------------------------------------------------+
| Init_cenv.
+----------------------------------------------------------------------------*/
        function_prolog(init_cenv)
        /*--------------------------------------------------------------------+
        | Load TOC.  Can't use ld since the TOC value might not be aligned on
	| double word boundary.  R2 is loaded for the first time here when 
        | loaded by PIBS (second time when originally running from FLASH). 
        +--------------------------------------------------------------------*/
	bl	..ot
	.quad 	.TOC.@tocbase
..ot:	mflr	r3
	lwz	r2,0x0000(r3)
	lwz	r3,0x0004(r3)
	rldicr	r2,r2,32,31
	or	r2,r2,r3
        /*--------------------------------------------------------------------+
        | Get the address and size of the stack.
        +--------------------------------------------------------------------*/
	GETSYMADDR(r3,__initial_stack,.LC0)
        addis   r4,r0,MY_MAIN_STACK_SIZE@h
        ori     r4,r4,MY_MAIN_STACK_SIZE@l
        /*--------------------------------------------------------------------+
        | Setup the stack, stack bust be quadword (128-bit) aligned.
        +--------------------------------------------------------------------*/
        add     r1,r3,r4
        addi    r1,r1,-stack_frame_min
	rldicr	r1,r1,0,59
        addi    r5,r0,0x0000
        std     r5,stack_frame_bc(r1)
        std     r5,stack_frame_lr(r1)
        /*--------------------------------------------------------------------+
        | Call the "C" function.
        +--------------------------------------------------------------------*/
//        b       .my_main
        b       .ppc_main
..spin: b       ..spin
        function_epilog(init_cenv)

