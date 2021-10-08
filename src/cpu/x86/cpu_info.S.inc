/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Pushes a 32-bit register onto the stack.
 *
 * There are two possible code sections where this code can be included:
 *   .code32 and .code64
 *
 * Doing a `push %eax` while in a .code64 section will result in a compiler
 * error. This macro manually pushes the 32-bit register onto the stack so we
 * can share the code between 32 and 64 bit builds.
 */
.macro pushr reg:req
#if ENV_X86_64
	movl	$0, -4(%esp)
	movl	\reg, -8(%esp)
	sub	$8, %esp
#else
	push	\reg
#endif
.endm

/* Push struct cpu_info */
.macro push_cpu_info index=$0
	pushr	\index	/* index (size_t) */
	pushr	$0	/* *cpu */
.endm

/* Push struct per_cpu_segment_data */
.macro push_per_cpu_segment_data cpu_info_pointer=%esp
	pushr	\cpu_info_pointer	/* *cpu_info */
.endm

/*
 * Sets the base address in the segment descriptor array.
 *
 * A segment descriptor has the following structure:
 *     struct {
 *         uint16_t segment_limit_0_15;
 *         uint16_t base_address_0_15;
 *         uint8_t base_address_16_23;
 *         uint8_t attrs[2];
 *         uint8_t base_address_24_31;
 *     };
 *
 * @desc_array: Address of the descriptor table
 * @base: Address to set in the descriptor
 * @desc_index: Index of the descriptor in the table. Defaults to 0. Must be a
 *              register if specified.
 *
 * Clobbers %eax, %ebx.
 */
.macro set_segment_descriptor_base desc_array:req, base:req, desc_index
	mov	\base, %eax

	mov	\desc_array, %ebx

.ifb \desc_index
	movw	%ax, 2(%ebx)
	shr	$16, %eax
	movb	%al, 4(%ebx)
	shr	$8, %eax
	movb	%al, 7(%ebx)
.else
	movw	%ax, 2(%ebx, \desc_index, 8)
	shr	$16, %eax
	movb	%al, 4(%ebx, \desc_index, 8)
	shr	$8, %eax
	movb	%al, 7(%ebx, \desc_index, 8)
.endif

.endm
