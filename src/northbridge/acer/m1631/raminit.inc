.type raminit_start, @function

/***
 *** sungeun
 ***
 ***/

#define MAX_MEM_BANK $0x78
#define LOAD_CURRENT_MCR mov (%esi),%al

	/*** sungeun jump to start ***/
	jmp	raminit_start

/***
 *** BEGIN TABLES
 ***/
mem_bank_reg_table :
	.byte 0x6c
	.byte 0x70
	.byte 0x74
	.byte 0x78

ma_table:
	.long 0x04000000
	.byte 0b0100
	.long 0x00002000
	.byte 0b0011
	.long 0x00001000
	.byte 0b0010
	.long 0x00000800
	.byte 0b0001
	.long 0x00000200	
	.byte 0b0000

vc_ma_table:
	.long 0x00000200
	.byte 0b0010
	.long 0x00000100
	.byte 0b0001
	.long 0x00000200
	.byte 0b0010
	.long 0x00000100
	.byte 0b0001
	.long 0x00000080
	.byte 0b0000

sizing_table:
	.long 0x02000000,0x00200000
	.byte 4
	.byte 5
	.long 0x04000000,0x00200000
	.byte 5
	.byte 6
	.long 0x08000000,0x00200000
	.byte 5
	.byte 0
	.long 0x08000000,0x00800000
	.byte 3
	.byte 0
	.long 0x08000000,0x04000000
	.byte 1
	.byte 0

memory_size:
	.byte 0b0000
	.byte 0b0001
	.byte 0b0010
	.byte 0b0011
	.byte 0b0100
	.byte 0b0101
	.byte 0b0111

internal_bank_tbl:
	.long 0x1000
	.long 0x2000
	.long 0x4000
/***
 *** END TABLES
 ***/



/***
 *** BEGIN MACROS
 ***/


#define cmos_data_in \
	outb	%al,$0x70	; \
	inb	$0x71,%al


#define pci_write_byte \
	mov	$0x80000000,%edx ; \
	or	%ax,%dx		; \
	mov	%edx,%eax	; \
	shl	$16,%edx	; \
	mov	$0x0cf8,%dx	; \
	and	$0x0fc,%al	; \
	outl	%eax,%dx	; \
	shr	$16,%edx	; \
	mov	%dl,%al		; \
	and	$3,%al		; \
	mov	$0x0cfc,%dx	; \
	add	%al,%dl		; \
	mov	%cl,%al		; \
	outb	%al,%dx


#define pci_read_byte \
	mov	$0x80000000,%edx ; \
	or	%ax,%dx		; \
	mov	%edx,%eax	; \
	shl	$16,%edx	; \
	mov	$0x0cf8,%dx	; \
	and	$0x0fc,%al	; \
	outl	%eax,%dx	; \
	shr	$16,%edx	; \
	mov	%dl,%al		; \
	and	$3,%al		; \
	mov	$0x0cfc,%dx	; \
	add	%al,%dl		; \
	inb	%dx,%al


#define pci_write_dword \
	mov	$0,%ah		; \
	mov	$0x80000000,%edx ; \
	or	%ax,%dx		; \
	mov	%edx,%eax	; \
	mov	$0x0cf8,%dx	; \
	outl	%eax,%dx	; \
	mov	$0x0fc,%dl	; \
	mov	%edi,%eax	; \
	outl	%eax,%dx


#define pci_read_dword \
	mov	$0,%ah		; \
	mov	$0x80000000,%edx ; \
	or	%ax,%dx		; \
	mov	%edx,%eax	; \
	mov	$0x0cf8,%dx	; \
	outl	%eax,%dx	; \
	mov	$0x0fc,%dl	; \
	inl	%dx,%eax


#define save_dram_type \
	mov	%sp,%di		; \
	mov	%esi,%eax		; \
	subl	$mem_bank_reg_table, %eax ; \
	mov	$2,%dl		; \
	mul	%dl		; \
	addl	$0x94,%eax	; \
	test	$0x10,%ch	; \
	jnz 9f ; \
	add	%ch,%al		; \
	pci_write_byte		; \
	jmp	8f ; \
9:		; \
	and	$0x0f,%ch	; \
	add	$0xc,%al		; \
	pci_read_byte		; \
8:		; \
	mov	%di,%sp


#define set_no_dram \
	mov	%sp,%di		; \
	mov	$0x00,%cx	; \
	rol	$16,%edi	; \
	save_dram_type ; \
	mov	$0x0100,%cx	; \
	save_dram_type ; \
	ror	$16,%edi	; \
	LOAD_CURRENT_MCR	; \
	add	$3,%al		; \
	pci_read_byte		; \
	and	$0b11100111,%al	; \
	mov	%al,%cl		; \
	LOAD_CURRENT_MCR	; \
	add	$3,%al		; \
	pci_write_byte		; \
	mov	%di,%sp


#define memory_error \
	mov	%sp,%di		; \
	mov	$0,%ch		; \
	test	$0x10000,%ebx	; \
	jnz	7f ; \
	mov	$0x01,%ch	; \
7:		; \
	mov	$0x00,%cl	; \
	rol	$16,%edi	; \
	save_dram_type		; \
	ror	$16,%edi	; \
	mov	%di,%sp


#define pci_read_write_byte \
	mov	%al,%bh		; \
	mov	%sp,%di		; \
	pci_read_byte		; \
	and	%ch,%al		; \
	or	%cl,%al		; \
	mov	%al,%cl		; \
	mov	%bh,%al		; \
	pci_write_byte		; \
	mov	%di,%sp


#define DISABLE_BLOCK \
	LOAD_CURRENT_MCR	; \
	pci_write_dword		; \
	and	$0x0f87fffff,%eax ; \
	xor	%di,%di		; \
	mov	%eax,%edi	; \
	LOAD_CURRENT_MCR	; \
	pci_read_dword


/***
 *** END MACROS
 ***/

#if 0
/***
 *** START REAL STUFF
 ***/
raminit_start:
	mov	$0x81,%ax
	mov	$0x7f00,%cx	/*** disable SMA mode ***/
	pci_read_write_byte
	mov	$0x82,%ax
	mov	$0xff10,%cx	/*** set 3 DIMMs, no ECC ***/
	pci_read_write_byte
	mov	$0x84,%ax
	mov	$0xfe00,%cx	/*** set 3 DIMMs, no ECC ***/
	pci_read_write_byte

	/*** initialize, load table, zero %ecx, %ebx ***/
	movl	$mem_bank_reg_table,%esi
	xor	%ecx,%ecx
	xor	%ebx,%ebx

	/*** main loop ***/
detect_block_again:

	or	$0x10000,%ebx	/*** for even bank ***/
	xor	%bx,%bx
	LOAD_CURRENT_MCR	/*** current memory control register ***/
	add	$3,%al		/*** point to upper byte of MCR ***/
	mov	%cx,%di
	mov	$0x0ee,%cl	/*** EDO DRAM ***/
	pci_write_byte
	LOAD_CURRENT_MCR
	add	$2,%al
	mov	$0x064,%cl	/*** 256M col=12 bits ***/
	pci_write_byte
	mov	%di,%cx
	jmp	detect_ma_size
odd_bank_detect:
	shl	$3,%bx
	and	$(~0x10000),%ebx
	LOAD_CURRENT_MCR
	add	$3,%al
	mov	$0x0e9,%cl
	pci_write_byte
	LOAD_CURRENT_MCR
	add	$2,%al
	mov	$0x0e4,%cl
	pci_write_byte
detect_ma_size:
	/***
	 *** sungeun *** BEGIN "inlined" macro DETECT_DRAM_TYPE
	 ***/
	/*** standard EDO dram test ***/
	movl	$0x055555555,%es:(0x0)
	movl	$0x0aaaaaaaa,%es:(0x8)
	cmpl	$0x55555555,%es:(0)
	jnz	detect_sdram
	cmpl	$0x0aaaaaaaa,%es:(8)
	jnz	detect_sdram
	mov	$0x01,%cx
	test $0x10000,%ebx
	jnz  1f
	mov	$0x01,%ch
1:
	save_dram_type
	jmp	 dram_type_detect_end
detect_sdram:
	mov $0x7e,%ax
	mov $0xf700, %cx	/*** turn off EDO detect ***/
	pci_read_write_byte
	LOAD_CURRENT_MCR
	mov %al,%ah
	mov $0x42,%al
	cmos_data_in
	test	$0b00010000,%al
	mov %ah,%al
	/*** jz cas_2 *** always slow ***/
	LOAD_CURRENT_MCR
	pci_read_dword	/*** changed from pci_write_dword ***/
	mov	%eax,%edi
	and	$0x00000fffc,%edi
	test	$0x010000,%ebx
	jz	1f
	or	$0x0f6640000,%edi
	jmp	configure_sdram_type
1:
	or	$0x0f1e40000,%edi
	jmp	configure_sdram_type
cas_2:
	LOAD_CURRENT_MCR
	pci_read_dword	/*** changed from pci_write_dword ***/
	mov	%eax,%edi
	and	$0x00000fffc,%edi
	test	$0x010000,%ebx
	jz	1f
	or	$0x0d6640000,%edi
	jmp	configure_sdram_type
1:
	or	$0x0d1e40000,%edi
configure_sdram_type:
	LOAD_CURRENT_MCR
	pci_write_dword	/*** changed from pci_read_dword ***/
	mov	$0xFFFF,%ax	/*** mov	$1000,%ax ***/
1:	/*** timing loop *** wait for dram to finish configuration ***/
	dec	%ax
	jnz	1b
	movl	$0x055555555,%es:(0x0)
	movl	$0x0aaaaaaaa,%es:(0x8)
	cmpl	$0x055555555,%es:(0x0)
	jnz	chk_vc_sdram_type
	cmpl	$0x0aaaaaaaa,%es:(0x8)
	jnz	chk_vc_sdram_type
	mov	$0x02,%cx
	test	$0x10000,%ebx
	jnz 	1f
	mov	$01,%ch
1:
	save_dram_type
	jmp	 dram_type_detect_end
chk_vc_sdram_type:
	LOAD_CURRENT_MCR
	pci_write_dword
	mov	%eax,%edi
	and	$0x0e7ffffff,%edi
	LOAD_CURRENT_MCR
	pci_read_dword
	mov	$0x7e,%ax
	mov	$0xf700,%cx
	pci_read_write_byte
	LOAD_CURRENT_MCR
	pci_write_dword
	mov	%eax,%edi
	LOAD_CURRENT_MCR
	test	$0x010000,%ebx
	jz	1f
	and	$0x00000d7f0,%edi
	or	$0x0fe620000,%edi
	jmp	configure_vc_sdram_type
1:
	and	$0x00000d7f0,%edi
	or	$0x0f9e20000,%edi
configure_vc_sdram_type:
	LOAD_CURRENT_MCR
	pci_read_dword
	mov	$1000,%ax
1:
	dec	%ax
	jnz	1b
	movl	$0x055555555,%es:(0x0)
	movl	$0x0aaaaaaaa,%es:(0x8)
	cmpl	$0x055555555,%es:(0x0)
	jnz	no_dram
	cmpl	$0x0aaaaaaaa,%es:(0x8)
	jnz	no_dram
	mov	$0x03,%cx
	test	$0x10000,%ebx
	jnz 	1f
	mov	$0x01,%ch
1:
	save_dram_type
	jmp	 dram_type_detect_end
no_dram:
	memory_error
	test	$0x10000,%ebx
	jnz	prepare_next_detect
dram_type_detect_end:
	/***
	 *** sungeun *** END "inlined" macro DETECT_DRAM_TYPE
	 ***/
	test	$0x10000,%ebx
	jnz	 1f
	/***
	 *** sungeun *** BEGIN "inlined" macro A_SUB
	 ***/
	mov	$0x10,%ch
	save_dram_type
	mov	%al,%cl
	mov	$0x11,%ch
	save_dram_type
	mov	%al,%ch
	mov	%cx,%ax
	cmp	$0x00,%al
	jz	no_sdram
	cmp	%ah,%al
	jz	a_sub_end
	cmp	$0x00,%ah
	jnz	no_sdram
	LOAD_CURRENT_MCR
	pci_write_dword
	mov	%eax,%edi
	and	$0x00000ffff,%edi
	mov	$0x042,%al
	cmos_data_in
	test	$0b00010000,%al
	jz	cas_l_2
	or	$0x0e6000000,%edi
	jmp	set_cas_end
cas_l_2:
	or	$0x0c6000000,%edi
set_cas_end:
	ror	$27,%edi
	and	$0x0ff,%cx
	or	%cx,%di
	rol	$7,%edi
	shr	$3,%bx
	and	$0x07,%bx
	or	%bx,%di
	rol	$20,%edi
	and	$0x0000f0000,%ecx
	or	%ecx,%edi
	test	$0x80000,%ebx
	jz	set_single
	or	$1,%edi
set_single:
	LOAD_CURRENT_MCR
	pci_read_dword
	jmp     detect_block_again_end
no_sdram:
	set_no_dram
	jmp	 detect_block_again_end
a_sub_end:
	/***
	 *** sungeun *** END "inlined" macro A_SUB
	 ***/
1:	
	mov	$0x10,%ch
	save_dram_type
	mov	%si,%di
	and	$0x03,%ax
	test	$0x02,%ax
	jnz	vcm_or_sdram
	mov	$0,%ax
	jmp	detect_ma_table
vcm_or_sdram:
	test	$0x01,%ax
	jnz	vcm_ma_type_set
	mov	$10,%ax
	jmp	detect_ma_table
vcm_ma_type_set:
	mov	$10,%ax
	or	$0x40000,%ebx
detect_ma_table:
	/***
	 *** sungeun *** BEGIN "inlined" macro MA_DETECT
	 ***/
	test	$0x40000,%ebx
	jnz	vc_sdram_table
	movl	$ma_table, %esi
	jmp	detect_ma
vc_sdram_table:
	movl	$vc_ma_table,%esi
detect_ma:
	add	%ax,%si
	mov	(%esi),%edx
	mov	%edx,%esi
	movl	$0x055555555,(%esi)
	movl	$0x0aaaaaaaa,(%esi)
	cmpl	$0x055555555,%esi
	jz	1f
	jmp    ma_detect_end
1:
	movl	$ma_table,%esi
	add	%ax,%si
	mov	4(%esi),%dl
	test	$0x10000,%ebx
	jz	1f
	xor	%ecx,%ecx
	mov	%dl,%cl
	shl	$16,%ecx
	jmp	memory_sizing_sub
1:
	ror	$20,%ecx
	mov	%dl,%cl
	rol	$20,%ecx
	jmp	memory_sizing_sub
ma_detect_end:
	/***
	 *** sungeun *** END "inlined" macro MA_DETECT
	 ***/
	add	$5,%ax
	cmp	$25,%ax
	jz	 1f
	jmp	detect_ma_table
1:	
	and	$(~0x40000),%ebx
	mov	%di,%si
	mov	$0x10,%ch
	test	$0x10000,%ebx
	jnz 	1f
	mov	$0x11,%ch
1:	
	save_dram_type
memory_sizing_sub:
	mov	%di,%si
	test	$0x10000,%ebx
	jz	1f
	mov	%ecx,%eax
	shr	$16,%eax
	mov	%al,%cl
	jmp	configure_ma
1:	
	mov	%ecx,%eax
	shr	$20,%eax
	mov	%al,%cl
configure_ma:
	LOAD_CURRENT_MCR
	add	$2,%al
	pci_read_byte 
	and	$0x0f0,%al
	and	$0x0f,%cl
	or	%cl,%al
	mov	%al,%cl
	LOAD_CURRENT_MCR
	add	$2,%al
	pci_write_byte

	/***
	 *** sungeun *** BEGIN "inlined" macro MEMORY_SIZING
	 ***/
	mov	%si,%di	
	mov	$10,%eax
	mov	%cl,%dl	
	and	$0x0f,%dl
	mul	%dl	
	movl $sizing_table,%esi
	addw	%ax,%si	
	mov	8(%esi),%dl
	mov	8(%esi),%dh
	ror	$16,%edx
	mov	9(%esi),%dl
	rol	$16,%edx
	mov	%esi,%eax
	mov	%eax,%esi
memory_sizing_loop:	
	cmp	%dl,%dh	
	jz	1f
	cmpl	$0x0aaaaaaaa,0(%esi)
	jz	1f
	memory_error
	mov	%di,%si
	jmp	 prepare_next_detect
1:	
	movl	$0x55555555,0(%esi)
	movl	$0x0aaaaaaaa,0(%esi)
	cmpl	$0x55555555,0(%esi)
	jz	determine_size
	cmp	$0,%dl
	jnz	1f
	memory_error
	mov	%di,%si
	jmp	 prepare_next_detect
1:
	dec	%dl	
	ror	$16,%edx
	mov	%dl,%al	
	rol	$16,%edx
	cmp	%al,%dl	
	jnz	1f	
	shr	$1,%esi	
1:
	shr	$1,%esi	
	jmp	memory_sizing_loop
determine_size:		
	sub	%dl,%dh	
	cmp	$2,%dh	
	jae	1f
	or	$0x20000,%ebx
1:
	xor	%dx,%dx	
find_memory_size_offset:
	cmp	$0x00200000,%esi
	jz	1f
	shr	$1,%esi	
	inc	%dx	
	jmp	find_memory_size_offset
1:	
	movl    $memory_size,%esi
	addw	%dx,%si	
	mov	$0,%dx	
	orb	0(%esi),%dl
	or	%dx,%bx	
	mov	%di,%si	
	test	$0x10000,%ebx
	jz	second_bank_number
	mov	$0x10,%ch
	jmp	read_dram_type
second_bank_number:
	mov	$0x11,%ch
read_dram_type:	
	save_dram_type	
	cmp	$0x01,%al
	jz	EDO_T	
	cmp	$0x00,%al
	jz	EDO_T	
	cmp	$0x02,%al
	jz	determine_bank_number
	or	$0x40000,%ebx
determine_bank_number:
	/***
	 *** sungeun *** BEGIN "inlined" macro bank_number
	 ***/
	and	$(~0x80000),%ebx
	test	$0x40000,%ebx
	jnz	configure_internal_bank_end 
	mov	%cx,%di
	LOAD_CURRENT_MCR
	add	$1,%al
	pci_read_byte
	and	$0x0ef,%al
	mov	%al,%cl
	LOAD_CURRENT_MCR
	add	$1,%al
	pci_write_byte
	LOAD_CURRENT_MCR
	pci_read_byte
	and	$0x0fc,%al
	or	$1,%al
	mov	%al,%cl
	LOAD_CURRENT_MCR
	pci_write_byte
	mov	%di,%cx
	test	$0x10000,%ebx
	jz	1f 
	ror	$16,%ecx
	jmp	what_memory_ma
1:
	ror	$20,%ecx
what_memory_ma:	
	movl	$0x055555555,%es:(0x0) 
	mov	%cx,%ax
	test	$0x10000,%ebx
	rol	$16,%ecx
	jnz	1f 
	rol	$4,%ecx
1:
	and	$0x0f,%ax
	mov	$4,%dl
	mul	%dl
	mov	%si,%di
	movl	$internal_bank_tbl, %esi 
	add	%ax,%si
	movl	(%esi),%edx
	mov	%edx,%esi
	movl	$0x0aaaaaaaa,(%esi) 
	cmpl	$0x055555555,%es:(0x0) 
	mov	%di,%si
	jz	i_4_bank
	mov	%cx,%di
	LOAD_CURRENT_MCR
	pci_read_byte
	and	$0x0fc,%al
	mov	%al,%cl
	LOAD_CURRENT_MCR
	pci_write_byte
	mov	%di,%cx
i_4_bank:	
	or	$0x80000,%ebx
configure_internal_bank_end:
	/***
	 *** sungeun *** END "inlined" macro bank_number
	 ***/
EDO_T:
	and	$(~0x20000),%ebx
	and	$(~0x40000),%ebx
	/***
	 *** sungeun *** END "inlined" macro MEMORY_SIZING
	 ***/

	LOAD_CURRENT_MCR
	add	$2,%al
	mov	%bx,%di
	pci_read_byte
	and	$0x6,%di
	shl	$4,%di
	and	$0x8f,%al
	or	%di,%ax
	mov	%al,%cl
	LOAD_CURRENT_MCR
	add	$2,%al
	pci_write_byte
	/***
	 *** sungeun *** BEGIN "inlined" macro CHECK_SUB
	 ***/
	test	$0x10000,%ebx
	jnz	1f
	mov	%bx,%ax
	and	$0x03f,%ax
	mov	%al,%ah
	and	$0x07,%al
	shr	$3,%ah
	cmp	%al,%ah
	jz	2f
	set_no_dram
	jmp	1f
2:
	shr	$16,%ecx
	mov	%cx,%ax
	and	$0x0ff,%ax
	mov	%al,%ah
	and	$0x0f,%al
	shr	$4,%ah
	cmp	%al,%ah
	jz	1f
	set_no_dram
1:
	/***
	 *** sungeun *** END "inlined" macro CHECK_SUB
	 ***/
prepare_next_detect:
	test	$0x10000,%ebx
	jnz	 odd_bank_detect
detect_block_again_end:
	DISABLE_BLOCK
	inc	%si
	cmpb	$0x078,(%esi)
	jz	configure_memory_block
	jmp	 detect_block_again
configure_memory_block:
	/***
	 *** sungeun *** BEGIN "inlined" macro CONFIGURE_MEMORY
	 ***/
	movl	$mem_bank_reg_table,%esi 
	mov     (%esi),%dl
	mov	$0x094,%bl
configure_next:
	xor	%edi,%edi
	mov	%bl,%al
	pci_read_byte
	cmp	$0x00,%al
	jz	1f 
	or	$0x3,%di
1:
	shl	$2,%di
	inc	%bl
	mov	%bl,%al
	pci_read_byte
	inc	%bl
	cmp	$0x00,%al
	jz	1f 
	or	$0x03,%di
1:
	LOAD_CURRENT_MCR
	pci_write_dword
	and	$0x0f87fffff,%eax
	shl	$23,%edi
	or	%edi,%eax
	mov	%eax,%edi
	LOAD_CURRENT_MCR
	pci_read_dword
	inc	%si
	cmpb	MAX_MEM_BANK,(%esi)
	jnz	configure_next
	/***
	 *** sungeun *** END "inlined" macro CONFIGURE_MEMORY
	 ***/
	mov	$0x7e,%ax
	mov	$0xf700,%cx
	pci_read_write_byte
	mov	$1000,%ax
1:	
	dec	%ax
	jnz	1b
	mov	$0x7e,%ax
	mov	$0xff10,%cx
	pci_read_write_byte

/***
 *** END REAL STUFF
 ****/
#endif
#include <asm.h>
#include <cpu/p5/macros.h>

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */
#define INIT_MCR $0xf6621000
#define WRITE_MCR0 movl %ecx, %eax ;  outl %eax, %dx
/*#include <cpu/p5/start32.inc>*/
raminit_start:
	mov	$0x81,%ax
	mov	$0x7f00,%cx	/*** disable SMA mode ***/
	pci_read_write_byte
	mov	$0x82,%ax
	mov	$0xff10,%cx	/*** set 3 DIMMs, no ECC ***/
	pci_read_write_byte
	mov	$0x84,%ax
	mov	$0xfe00,%cx	/*** set 3 DIMMs, no ECC ***/
	pci_read_write_byte

	/*** initialize, load table, zero %ecx, %ebx ***/
	movl	$mem_bank_reg_table,%esi
	xor	%ecx,%ecx
	xor	%ebx,%ebx

/* make the flash writeable */
/* select the south bridge, register 44 (base) */
/* south bridge is device 7. 7 << 3 is 0x38 */
	movl	$0x80003844, %eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
/* but 0x40 makes flash writeable. You need this for DoC */
	movb	$0x40, %al
/* 0xff selects register 0x47 */
	movb	$0xff, %dl
	outb	%al,%dx

	xorl	%edi, %edi
	movl	$0x8000006c, %eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
/*	movb	$0x8000006c, %al
	CALLSP(pci_read_dword)*/
	movb	$0xfc, %dl
	inl	%dx, %eax
	movl	%eax, %ecx
	andl    $0xfffc, %ecx
	orl	INIT_MCR,  %ecx
	WRITE_MCR0
	/*
	movb	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
	rorl	$16, %ecx
	/* clear out the CAS, since we assume start at zero
	 * and increment 
	 */
	andb	$0xf0, %cl
	/* test 8 bit CAS */
	movb	$0, 0
	movb	$1, 0x800
	cmpb	$0,0
	jnz	sizeram
	inc	%cl
	/* change the value we store each time. It makes debugging easier */
	movb	$2, 0x1000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	movb	$3, 0x2000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	
sizeram:		
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	andw	$0xefff, %cx
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 4M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	movb	$0, 0
	ror	$20, %ecx
	/* clear the size out to 4 MB */
	andb	$0xf8, %cl
	/* 4 MB */
	movl	$0x400000, %esi
1:

	/* move a 4 to the next power-of-two address. 
	 * if there is no memory there, it will wrap to zero 
	 */
 	movb	$4, %es:(%esi)

	cmpb	$0, 0
	/* if there is not a 0 at 0, the last write wrapped. Hop out */
	jne	1f
	inc	%cl
	rol	$1, %esi
	jmp	1b
/* size is now in esi */
/* %ecx has setting for register */
1:
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
 */
	movb	$0, 0
	movb	$5, 0x1000
	movb	$6, 0x2000
	movb	$7, 0x4000
	cmpb	$0, 0
	jz	1f /* only one bank */
	orl	$0x80000, %ecx
1: 
	/* clear 4 banks */
	andb	$0xfe, %cl
	WRITE_MCR0
	movl	$0x8000007c, %eax
	movb	$0xf8, %dl
	outl	%eax, %dx
	movl	$0x2423c411, %eax
	movb	$0xfc, %dl
	outl	%eax, %dx

