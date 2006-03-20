#include <cpu/amd/model_gx2/gx2def.h>

void
cpubug(void){
	msr_t msr;
	int rev;

	msr = rdmsr(GLCP_CHIP_REVID);

	rev = msr.lo & 0xff;
	if (rev < 0x20) {
		printk_error("%s: rev < 0x20! bailing!\n");
		return;
	}
	
	switch(rev)
	{
		case 0x20:
			pcideadlock();
			eng1398();
			bug752();
			break;
		case 0x22:
			pcideadlock();
			eng1398();
			eng2900();
			bug 118339();
			break;
		case 0x22:
		case 0x30: 
			break;
		default:
			printk_error("unknown rev %x, bailing\n", rev);
			return;
	}
	bug784();
	bug118253();
	disablememoryreadorder();
}

#if 0
void
bug645(void){
	msr_t msr;
	rdmsr(CPU_ID_CONFIG);
	msr.whatever |= ID_CONFIG_SERIAL_SET;
	wrmsr(msr);
}

void
bug573(void){
	msr_t msr;

	msr = rdmsr(MC_GLD_MSR_PM);
	msr.eax &= 0xfff3;
	wrmsr(MC_GLD_MSR_PM);
}

static void
pcideadlock(void){
	msr_t msr;

	msr = rdmsr(CPU_DM_CONFIG0);
	msr.hi &= ~(7<<DM_CONFIG0_UPPER_WSREQ_SHIFT);
	msr.hi |= (2<<DM_CONFIG0_UPPER_WSREQ_SHIFT);
	msr.lo |= DM_CONFIG0_LOWER_MISSER_SET;
	wrmsr(CPU_DM_CONFIG0, msr);



	msr = rdmsr(CPU_IM_CONFIG);
	msr.lo |= IM_CONFIG_LOWER_QWT_SET;	/* interlock instruction fetches to WS regions with data accesses.
									  * This prevents in instruction fetch from going out to PCI if the 
									  * data side is about to make a request.
									  */
	wrmsr(CPU_IM_CONFIG, msr);
	/* write serialize memory hole to PCI. Need to to unWS when something is shadowed regardless of cachablility.*/

	msr.lo = 0x021212121;
	msr.hi = 0x021212121
	wrmsr( CPU_RCONF_A0_BF, msr);
	wrmsr( CPU_RCONF_C0_DF, msr);
	wrmsr( CPU_RCONF_E0_FF, msr);
}

;**************************************************************************
;*
;*	CPUbug784
;*
;*	Bugtool #784 + #792
;*
;*	Fix CPUID instructions for < 3.0 CPUs
;*
;*	Entry:
;*	Exit:
;*	Modified:
;*
;**************************************************************************

void cpubug784(void){
	static char *name = "Geode by NSC";

	/* we'll do this the stupid way, for now, but that's the string they want. NO ONE KNOWS why you
	  * would do this -- the OS can figure this type of stuff out!
	  */
	msr = rdmsr(0x3006);
	msr.hi = 0x646f6547;
	wrmsr(0x3006, msr);

	msr = rdmsr(0x3007);
	msr.hi = 0x79622065;
	msr.lo = 0x43534e20;
	wrmsr(0x3007, msr);

	msr = rdmsr(0x3002);
	wrmsr(*0x3008, msr);

; More CPUID to match AMD better. #792
	msr = rdmsr(0x3009);
	msr.hi = 0x0C0C0A13D;
	msr.lo = 0x00000000;
	wrmsr(0x3009, msr);
}

/* cpubug 1398: enable MC if we KNOW we have DDR*/
void
eng1398(void){
	msr_t msr;

	msr = rdmsr(MSR_GLCP+0x17);
	if ((msr.lo & 0xff) < CPU_REV_2_0) {
		msr = rdmsr(GLCP_SYS_RSTPLL);
	i	if (msr.lo & (1<<RSTPPL_LOWER_SDRMODE_SHIFT))
			return;
	}

	/* no bios to check, we just go for it? */
	msr = rdmsr(MC_GLD_MSR_PM);
	msr.lo |= 3; /* enable MC clock gating.*/
	wrmsr(MC_GLD_MSR_PM, msr);
}

void
eng2900{void){
	printk_err(" NOT DOING eng2900: only shown to be a windows problem\n");
#if 0

;**************************************************************************
;*
;*	CPUbugIAENG2900
;*
;*	Clear Quest IAENG00002900, VSS 118.150
;*
;*	BTB issue causes blue screen in windows.
;*
;*	Entry:
;*	Exit:
;*	Modified:
;*
;**************************************************************************
CPUbugIAENG2900	PROC NEAR PUBLIC
	pushad

; Clear bit 43, disables the sysenter/sysexit in CPUID3
	mov	ecx, 3003h
	RDMSR
	and	edx, 0FFFFF7FFh
	WRMSR

	mov	cx, TOKEN_BTB_2900_SWAPSIF_ENABLE
	NOSTACK	bx, GetNVRAMValueBX
	cmp	ax, TVALUE_ENABLE
	jne	bug2900exit


;Disable enable_actions in DIAGCTL while setting up GLCP
	mov	ecx, MSR_GLCP + 005fh
	xor	edx, edx
	xor	eax, eax
	WRMSR

;Changing DBGCLKCTL register to GeodeLink
	mov	ecx, MSR_GLCP + 0016h
	xor	edx, edx
	xor	eax, eax
	WRMSR

	mov	ecx, MSR_GLCP + 0016h
	xor	edx, edx
	mov	eax, 02h
	WRMSR

;The code below sets up the RedCloud to stall for 4 GeodeLink clocks when CPU is snooped.
;Because setting XSTATE to 0 overrides any other XSTATE action, the code will always
;stall for 4 GeodeLink clocks after a snoop request goes away even if it occured a clock or two 
;later than a different snoop; the stall signal will never 'glitch high' for 
;only one or two CPU clocks with this code.

;Send mb0 port 3 requests to upper GeodeLink diag bits [63:32]
	mov	ecx, MSR_GLIU0 + 2005h
	xor	edx, edx
	mov	eax, 80338041h
	WRMSR

;set5m watches request ready from mb0 to CPU (snoop)
	mov	ecx, MSR_GLCP + 0045h
	mov	edx, 5ad68000h
	xor	eax, eax
	WRMSR

;SET4M will be high when state is idle (XSTATE=11)
	mov	ecx, MSR_GLCP + 0044h
	xor	edx, edx
	mov	eax, 0140h
	WRMSR

;SET5n to watch for processor stalled state
	mov	ecx, MSR_GLCP + 004Dh
	mov	edx, 2000h
	xor	eax, eax
	WRMSR

;Writing action number 13: XSTATE=0 to occur when CPU is snooped unless we're stalled
	mov	ecx, MSR_GLCP + 0075h
	xor	edx, edx
	mov	eax, 00400000h
	WRMSR

;Writing action number 11: inc XSTATE every GeodeLink clock unless we're idle
	mov	ecx, MSR_GLCP + 0073h
	xor	edx, edx
	mov	eax, 30000h
	WRMSR


;Writing action number 5: STALL_CPU_PIPE when exitting idle state or not in idle state
	mov	ecx, MSR_GLCP + 006Dh
	xor	edx, edx
	mov	eax, 00430000h
	WRMSR

;Writing DIAGCTL Register to enable the stall action and to let set5m watch the upper GeodeLink diag bits.
	mov	ecx, MSR_GLCP + 005fh
	xor	edx, edx
	mov	eax, 80004000h
	WRMSR


bug2900exit:
	popad
	ret
CPUbugIAENG2900	ENDP
#endif
}

void eng118253(void){
	msr_t msr;

	msr = rdmsr(GLPCI_SPARE);
	msr.lo &= ~GLPCI_SPARE_LOWER_PPC_SET;
	wrmsr(GLPCI_SPARE, msr);
}
void
bug118339(void) {
	printk_err("This is OPTIONAL BIOS-ENABLED ... ignore for now\n");
#if 0
	PROC NEAR PUBLIC
	pushad

	mov	cx, TOKEN_VGTEAR_118339_SWAPSIF_ENABLE
	NOSTACK	bx, GetNVRAMValueBX
	cmp	ax, TVALUE_ENABLE
	jne	bug118339exit

;Disable enable_actions in DIAGCTL while setting up GLCP
	mov	ecx, MSR_GLCP + 005fh
	xor	edx, edx
	xor	eax, eax
	WRMSR

; SET2M fires if VG pri is odd (3, not 2) and Ystate=0
	mov	ecx, MSR_GLCP + 042h
;	mov	edx, 2d6b8000h
	mov	edx, 596b8000h
	mov	eax, 00000a00h
	WRMSR

; SET3M fires if MBUS changed and VG pri is odd
	mov	ecx, MSR_GLCP + 043h
	mov	edx, 596b8040h
	xor	eax, eax
	WRMSR

; Put VG request data on lower diag bus
	mov	ecx, MSR_GLIU0 + 2005h
	xor	edx, edx
	mov	eax, 80338041h
	WRMSR

; Increment Y state if SET3M if true
	mov	ecx, MSR_GLCP + 074h
	xor	edx, edx
	mov	eax, 0000c000h
	WRMSR

; Set up MBUS action to PRI=3 read of MBIU
	mov	ecx, MSR_GLCP + 020h
	mov	edx, 0000d863h
	mov	eax, 20002000h
	WRMSR

; Trigger MBUS action if VG=pri3 and Y=0, this blocks most PCI
	mov	ecx, MSR_GLCP + 071h
	xor	edx, edx
	mov	eax, 00000c00h
	WRMSR

;Writing DIAGCTL
	mov	ecx, MSR_GLCP + 005fh
	xor	edx, edx
	mov	eax, 80004000h
	WRMSR

	; Code to enable FS2 even when BTB and VGTEAR SWAPSiFs are enabled
	; As per Todd Roberts in PBz1094 and PBz1095
	; Moved from CPUREG to CPUBUG per Tom Sylla
	mov	ecx, 04C000042h	; GLCP SETMCTL Register
	rdmsr
	or	edx, 8			; Bit 35 = MCP_IN
	wrmsr
	
bug118339exit:
	popad
	ret
CPUbug118339	ENDP
#endif
}



/****************************************************************************/
/***/
/**	DisableMemoryReorder*/
/***/
/**	PBZ 3659:*/
/**	 The MC reordered transactions incorrectly and breaks coherency.*/
/**	 Disable reording and take a potential performance hit.*/
/**	 This is safe to do here and not in MC init since there is nothing*/
/**	 to maintain coherency with and the cache is not enabled yet.*/
/***/
/***/
/**	Entry:*/
/**	Exit:*/
/**	Modified:*/
/***/
/****************************************************************************/
void
DisableMemoryReorder(void) {	
	msr_t msr;
	msr = rdmsr(MC_CF8F_DATA);

	msr.hi |=  CF8F_UPPER_REORDER_DIS_SET);
	wrmsr(MC_CF8F_DATA, msr);
}
