#include <arch/cpu.h>
#include <arch/pal.h>
#include <arch/asm.h>
.set noat
.set noreorder
.text

#define PAL_EXCEPT(label,location) . = location ;  label:
#define PAL_EXCEPT_DUMMY(location) . = location ; mb ; mb ; mb; hw_mfpr p7, EV6__EXC_ADDR ; bsr p0, __fatal_error_pal; mb ; mb ; mb
#define PAL_CALL_DUMMY(location) . = location ; mb ; mb ; mb ; mb ; bsr p0, __fatal_error_pal ;  mb ; mb ; mb
	
#define va_48 0
#define mchk_en 1
#define tb_mb_en 0

#define EV6__I_STAT__W1C ((1 << EV6__I_STAT__TPE__S) | (1 << EV6__I_STAT__DPE__S))
#define EV6__DC_STAT__W1C ( \
	(1 << EV6__DC_STAT__TPERR_P0__S) | \
	(1 << EV6__DC_STAT__TPERR_P1__S) | \
	(1 << EV6__DC_STAT__ECC_ERR_ST__S) | \
	(1 << EV6__DC_STAT__ECC_ERR_LD__S) | \
	(1 << EV6__DC_STAT__SEO__S))

#define EV6__I_CTL__INIT (\
	(3 << EV6__I_CTL__IC_EN__S) | \
	(2 << (EV6__I_CTL__SPE__S + va_48)) | \
	(2 << EV6__I_CTL__SDE__S) | \
	(3 << EV6__I_CTL__SBE__S) | \
	(va_48 << EV6__I_CTL__VA_48__S) | \
	(1 << EV6__I_CTL__CALL_PAL_R23__S) | \
	(mchk_en << EV6__I_CTL__MCHK_EN__S) | \
	(tb_mb_en << EV6__I_CTL__TB_MB_EN__S))

#define EV6__PCTX__INIT (1 << EV6__PCTX__FPE__S)
	
#define EV6__M_CTL__INIT (2 << (EV6__M_CTL__SPE__S+va_48))

#define EV6__IER__INIT 0

#define EV6__HW_INT_CLR__INIT ( \
	(1 << (EV6__HW_INT_CLR__MCHK_D__S - EV6__HW_INT_CLR__MCHK_D__S)) | \
	(3 << (EV6__HW_INT_CLR__PC__S - EV6__HW_INT_CLR__MCHK_D__S)) | \
	(1 << (EV6__HW_INT_CLR__CR__S - EV6__HW_INT_CLR__MCHK_D__S)) | \
	(1 << (EV6__HW_INT_CLR__SL__S - EV6__HW_INT_CLR__MCHK_D__S)))

#define EV6__VA_CTL__INIT (va_48 << EV6__VA_CTL__VA_48__S)
	
EntryPoint:
	/*	On startup
	 *      R19 - Signature (0xDEC?????) likely (0xDECB001)
	 *	R23 - Memory size in bytes.
	 *	r22 - CPU speed in picoseconds.
	 *	R25 - Chip revision.
	 *	excAddr - Address of ICache flush code to execute in PALmode.
	 */

	/* Palcode switching entry point */
	mb
	bis	a3, a3, a0		/* Signature */
	bis	t9, t9, a1		/* Memory size in bytes */
	bis	t8, t8, a2		/* CPU speed in picoseconds */
	br	zero, pal_mode_start
	mb
	mb
	mb

PAL_EXCEPT_DUMMY(EV6__DTBM_DOUBLE_3_ENTRY)
PAL_EXCEPT_DUMMY(EV6__DTBM_DOUBLE_4_ENTRY)
PAL_EXCEPT_DUMMY(EV6__FEN_ENTRY)
PAL_EXCEPT_DUMMY(EV6__UNALIGN_ENTRY)

	/* DTBM */
PAL_EXCEPT(DTBM_SINGLE,	EV6__DTBM_SINGLE_ENTRY)
	hw_mfpr	p7, EV6__EXC_ADDR		/* (0L,1) get exception address	*/
	hw_mfpr	p2, EV6__VA			/* (4-7,1L,3) get original va	*/

	lda	p1, 0x3301(zero)		/* all r/w enable	     */
	srl	p2, 13, p0			/* shift out the byte offset	*/
	sll	p0, EV6__DTB_PTE0__PFN__S, p0	/* get pfn into position	*/
	bis	p0, p1, p0			/* produce the pte           */

	FETCH_BLOCK_ALIGN
	hw_mtpr	p2, EV6__DTB_TAG0		/* (2&6,0L) write tag0		*/
	hw_mtpr	p2, EV6__DTB_TAG1		/* (1&5,1L) write tag1		*/
	hw_mtpr	p0, EV6__DTB_PTE0		/* (0,4,2,6) (0L) write pte0	*/
	hw_mtpr	p0, EV6__DTB_PTE1		/* (3,7,1,5) (1L) write pte1	*/

	hw_mtpr	zero, EV6__MM_STAT		/* (4-7,0L) start outside IQ    */
	FETCH_BLOCK_ALIGN
	hw_ret	(p7)				/* return with jmp		*/
	br	zero, .-4			/* stop predictor */

PAL_EXCEPT_DUMMY(EV6__DFAULT_ENTRY)
PAL_EXCEPT_DUMMY(EV6__OPCDEC_ENTRY)
PAL_EXCEPT_DUMMY(EV6__IACV_ENTRY)
PAL_EXCEPT_DUMMY(EV6__MCHK_ENTRY)

	/* *ITBM* */
PAL_EXCEPT(ITB_MISS,	EV6__ITB_MISS_ENTRY)
	hw_mfpr	p7, EV6__EXC_ADDR		/* (0L,1) get exception address	*/
	srl	p7, 13, p2			/* shift out the byte offset	*/
	lda	p1, 0x301(zero)			/* all read enable		*/
	sll	p2, EV6__ITB_PTE__PFN__S, p2	/* get pfn into position	*/
	bis	p2, p1, p2			/* produce the pte		*/

	FETCH_BLOCK_ALIGN
	hw_mtpr	p7, EV6__ITB_TAG		/* (2&6,0L) write tag0		*/
	hw_mtpr	p2, EV6__ITB_PTE		/* (0&4,0L) write pte0		*/
	FETCH_BLOCK_ALIGN
	hw_ret/stall	(p7)			/* (0L) return			*/
	br	zero, .-4

PAL_EXCEPT_DUMMY(EV6__ARITH_ENTRY)
PAL_EXCEPT_DUMMY(EV6__INTERRUPT_ENTRY)
PAL_EXCEPT_DUMMY(EV6__MT_FPCR_ENTRY)
PAL_EXCEPT_DUMMY(EV6__RESET_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_00_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_01_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_02_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_03_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_04_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_05_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_06_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_07_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_08_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_09_ENTRY)

#if 0
PAL_CALL_DUMMY(EV6__CALL_PAL_0A_ENTRY)
#else
	/* swppal */
PAL_EXCEPT(SWPPAL, EV6__CALL_PAL_0A_ENTRY)
#define OSF_P_MISC__SWITCH__S 62
	/* new pal base is in a0 */
	bis	a0, a0, p1			/* base in p1 */
	lda	p0, 0x7fff(zero)		/* check for pal base */
	and	p1, p0, p0			/* get low 15 bits */
	cmpeq	p0, 0, v0			/* check for non-zero bits */

	blbc	v0, call_pal__swppal_fail	/* if not clear say unknown */

	bis	zero, 1, p0			/* get a '1' */
	sll	p0, OSF_P_MISC__SWITCH__S, p0	/* switch bit into position */
	bis	zero, p0, p6			/* mark switch */

	bis	zero, zero, v0			/* status success */
	bis	p1, 1, p1			/* set pal mode bit */
	hw_ret (p1)				/* go to it */
	bis	zero, zero, zero		/* nop */
	
call_pal__swppal_fail:
	bis	zero, 1, v0			/* failure */
	hw_ret (p7)
	bis	zero, zero, zero		/* nop */
	bis	zero, zero, zero		/* nop */
#endif

PAL_CALL_DUMMY(EV6__CALL_PAL_0B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_0C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_0D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_0E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_0F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_10_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_11_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_12_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_13_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_14_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_15_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_16_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_17_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_18_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_19_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1A_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_1F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_20_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_21_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_22_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_23_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_24_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_25_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_26_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_27_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_28_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_29_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2A_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_2F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_30_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_31_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_32_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_33_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_34_ENTRY)

#if 0
PAL_CALL_DUMMY(EV6__CALL_PAL_35_ENTRY)
#else
	/* swpipl */
PAL_EXCEPT(SWPIPL, EV6__CALL_PAL_35_ENTRY)
	/* Fake the swpipl call just return the maximum ipl
	 * I'm not going to enable interrupts anyway.
	 */
	bis zero, 7, v0
	hw_ret/stall(p7)
#endif

PAL_CALL_DUMMY(EV6__CALL_PAL_36_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_37_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_38_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_39_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3A_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_3F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_80_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_81_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_82_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_83_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_84_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_85_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_86_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_87_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_88_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_89_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8A_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_8F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_90_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_91_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_92_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_93_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_94_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_95_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_96_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_97_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_98_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_99_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9A_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9B_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9C_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9D_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9E_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_9F_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_A0_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A1_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A2_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A3_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A4_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A5_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A6_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A7_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A8_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_A9_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AA_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AB_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AC_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AD_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AE_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_AF_ENTRY)

PAL_CALL_DUMMY(EV6__CALL_PAL_B0_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B1_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B2_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B3_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B4_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B5_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B6_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B7_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B8_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_B9_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BA_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BB_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BC_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BD_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BE_ENTRY)
PAL_CALL_DUMMY(EV6__CALL_PAL_BF_ENTRY)

PAL_EXCEPT(dummy,  EV6__CALL_PAL_BF_ENTRY + 0x80)

/* Data for pal_mode_start
 * I can't use the magic linker gp magic here so roll my own.
 * Note: The linker script ensures that EntryPoint _edata & _end
 * are all 8 byte aligned.
 */
EntryPoint_addr:
	.quad EntryPoint
_edata_addr:
	.quad	_edata
_end_addr:
	.quad	_end
kernel_mode_start_addr:
	.quad	kernel_mode_start
__fatal_error_non_pal_addr:
	.quad	__fatal_error_non_pal
	
pal_mode_start:
	/* relocate to address zero */
	/* The following move routine is not totally
	 * general.  In particular it does not handle
	 * overlapping memory locations or moving to a higher
	 * address in memory.
	 */
	br pv, relocate
relocate:
	lda	t0,(EntryPoint - relocate)(pv)
	ldq_p	t1,(EntryPoint_addr - relocate)(pv)
	ldq_p	t2,(_edata_addr - relocate)(pv)
	subq	t2, t1, t2
	br	Mstart
	
	/* Move the program */
Move:	ldq_p	t3, 0(t0)
	subq	t2, 8, t2
	addq	t0, 8, t0
	stq_p	t3, 0(t1)
	addq	t1, 8, t1
Mstart:	bne	t2, Move

	/* set up pal_base register */
	FETCH_BLOCK_ALIGN
	ldq_p	t0, (EntryPoint_addr - relocate)(pv)
	hw_mtpr	t0, EV6__PAL_BASE

	/* flush ITB & DTB */
	FETCH_BLOCK_ALIGN
	hw_mtpr	zero, EV6__ITB_IA
	hw_mtpr	zero, EV6__DTB_IA

	/* clear i_stat & dc_stat */
	LOAD_CONSTANT32(t0, EV6__I_STAT__W1C)
	LOAD_CONSTANT16(t2, EV6__DC_STAT__W1C)
	FETCH_BLOCK_ALIGN
	hw_mtpr t0, EV6__I_STAT
	hw_mtpr t2, EV6__DC_STAT

	/* Initialize I_CTL */
	LOAD_CONSTANT32(t0, EV6__I_CTL__INIT)
	FETCH_BLOCK_ALIGN
	hw_mtpr	t0, EV6__I_CTL

	/* Initialize pctx & m_ctl */
	FETCH_BLOCK_ALIGN
	LOAD_CONSTANT16(t0, EV6__PCTX__INIT)
	LOAD_CONSTANT16(t2, EV6__M_CTL__INIT)
	hw_mtpr	t0, EV6__PCTX
	hw_mtpr t2, EV6__M_CTL

	/* Initialize ier_cm */
	LOAD_CONSTANT32(t0, EV6__IER__INIT)
	hw_mtpr	t0, EV6__IER_CM
	hw_mtpr	zero, EV6__CC

	/* Initialize SIRR */
	bis	zero, 1, t0
	sll	t0, 32, t0
	hw_mtpr zero, EV6__SIRR
	hw_mtpr $1, EV6__CC_CTL

	/* Initialize int_clr */
	LOAD_CONSTANT16(t0, EV6__HW_INT_CLR__INIT)
	sll	t0, EV6__HW_INT_CLR__MCHK_D__S, t0
	hw_mtpr	t0, EV6__HW_INT_CLR
	hw_mtpr	zero, EV6__DTB_ALT_MODE

	/* set pctr_ctl & va_ctl */
	hw_mtpr	zero, EV6__PCTR_CTL
	LOAD_CONSTANT16(t0, EV6__VA_CTL__INIT)
	hw_mtpr	t0, EV6__VA_CTL
	FETCH_BLOCK_ALIGN
		
	/* Clear the asn */
	hw_mtpr	zero, EV6__DTB_ASN0
	hw_mtpr	zero, EV6__DTB_ASN1
	FETCH_BLOCK_ALIGN

	/* write pctrl_ctl again to clear the 2nd stage overflow flag.
	 * and force other mtpr to retwir while we are at it.
	 */
	hw_mtpr zero, EV6__PCTR_CTL | 0xF0
	FETCH_BLOCK_ALIGN

	/* We need to write HW_INT_CLR to avoid a interrup that can occur
	 * the counters come up in an unpredictable state near overflow.
	 */
	
	lda	t0, 3(zero)
	sll	t0, EV6__HW_INT_CLR__PC__S, t0
	hw_mtpr	t0, EV6__HW_INT_CLR
	FETCH_BLOCK_ALIGN
	hw_mtpr t0, EV6__HW_INT_CLR
	FETCH_BLOCK_ALIGN
	
	/* Clear the fpcr */
	mt_fpcr $f31

#if 0
	/* Setup 48 bit address space */
	FETCH_BLOCK_ALIGN
	bis	zero, 2, t0
	hw_mtpr	t0, EV6__VA_CTL
	bis	zero, 8, t0
	hw_mtpr	t0, EV6__M_CTL
#else
	/* Setup 40 bit address space */
	FETCH_BLOCK_ALIGN
	bis	zero, 0, t0
	hw_mtpr	t0, EV6__VA_CTL
	bis	zero, 4, t0
	hw_mtpr	t0, EV6__M_CTL
#endif

	FETCH_BLOCK_ALIGN
	/* Enable special pal instructions in kernel mode */
	hw_mfpr t0, EV6__I_CTL
	lda	t1, 1(zero)
	sll	t1, EV6__I_CTL__HWE__S, t1
	bis	t0, t1, t0
	hw_mtpr	t0, EV6__I_CTL

	/* Now get out of palmode */
	FETCH_BLOCK_ALIGN
	ldq_p	pv, (kernel_mode_start_addr - relocate)(pv)
	hw_jmp	(pv)

GLOBL(start)
	br	pv, __start
__start:
	/* Some dummy parameters for debugging */
	bis	zero, zero, t8    /* CPU speed in picoseconds  */
	lda	t9, 128*1024*1024 /* Memory size in bytes 128MB */
	bis	zero, zero, t11   /* Chip revision */
	
	lda	a0, (EntryPoint - __start)(pv)
	call_pal PAL_swppal
	br	zero, .-4	/* spin in place  */

	/* __fatal_error_pal Called with:
	 * p0 - Exception handler that caught the exception
	 * p7 - Address where the exception occured.
	 * ra - Potentially valid return address from the code
	 *      that took the exception.
	 */
	FETCH_BLOCK_ALIGN
__fatal_error_pal:
	subq	p0, 0x14, a0
	bis	p7, p7, a1
	bis	ra, ra, a2
	bsr	pv, __fatal_error_pal2
__fatal_error_pal2:
	lda	pv, (__fatal_error_non_pal - __fatal_error_pal2)(pv)
	hw_ret	(pv)
	
