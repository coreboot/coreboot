/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Eric Biederman <ebiederm@xmission.com>
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

	/*
	 * Put the processor back into a reset state
	 * with respect to the XMM registers.
	 */
	xorps %xmm0, %xmm0
	xorps %xmm1, %xmm1
	xorps %xmm2, %xmm2
	xorps %xmm3, %xmm3
	xorps %xmm4, %xmm4
	xorps %xmm5, %xmm5
	xorps %xmm6, %xmm6
	xorps %xmm7, %xmm7

	/*
	 * Disable SSE instructions.
	 *
	 * Clear CR4[9] (OSFXSR) and CR4[10] (OSXMMEXCPT) so that the
	 * processor can no longer execute SSE instructions, and unmasked
	 * SIMD floating point exceptions will generate an invalid opcode
	 * exception (#UD).
	 */
	movl	%cr4, %eax
	andl	$~(3 << 9), %eax
	movl	%eax, %cr4

