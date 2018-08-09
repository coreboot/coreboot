/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This file define some function used to swap value between memory
 * and float register. This will be used in misaligned access exception
 * handling.
 */

#if defined(__riscv_flen)
#if __riscv_flen >= 32

	.text

/* void read_f32(int regnum, uint32_t* v)
 *    regnum : which register want to read
 *    v : address to hold bits info of reading
 */
	.align	1
	.globl read_f32
read_f32:
	la	a2, .Lr32_t
	andi	a0, a0, 31
	slli	a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr	a0
	.align	2
.Lr32_t:
	.half	.Lr32_f0  - .Lr32_t
	.half	.Lr32_f1  - .Lr32_t
	.half	.Lr32_f2  - .Lr32_t
	.half	.Lr32_f3  - .Lr32_t
	.half	.Lr32_f4  - .Lr32_t
	.half	.Lr32_f5  - .Lr32_t
	.half	.Lr32_f6  - .Lr32_t
	.half	.Lr32_f7  - .Lr32_t
	.half	.Lr32_f8  - .Lr32_t
	.half	.Lr32_f9  - .Lr32_t
	.half	.Lr32_f10 - .Lr32_t
	.half	.Lr32_f11 - .Lr32_t
	.half	.Lr32_f12 - .Lr32_t
	.half	.Lr32_f13 - .Lr32_t
	.half	.Lr32_f14 - .Lr32_t
	.half	.Lr32_f15 - .Lr32_t
	.half	.Lr32_f16 - .Lr32_t
	.half	.Lr32_f17 - .Lr32_t
	.half	.Lr32_f18 - .Lr32_t
	.half	.Lr32_f19 - .Lr32_t
	.half	.Lr32_f20 - .Lr32_t
	.half	.Lr32_f21 - .Lr32_t
	.half	.Lr32_f22 - .Lr32_t
	.half	.Lr32_f23 - .Lr32_t
	.half	.Lr32_f24 - .Lr32_t
	.half	.Lr32_f25 - .Lr32_t
	.half	.Lr32_f26 - .Lr32_t
	.half	.Lr32_f27 - .Lr32_t
	.half	.Lr32_f28 - .Lr32_t
	.half	.Lr32_f29 - .Lr32_t
	.half	.Lr32_f30 - .Lr32_t
	.half	.Lr32_f31 - .Lr32_t
#define  read32(which)	.Lr32_##which: fsw which, 0(a1); ret
	read32(f0)
	read32(f1)
	read32(f2)
	read32(f3)
	read32(f4)
	read32(f5)
	read32(f6)
	read32(f7)
	read32(f8)
	read32(f9)
	read32(f10)
	read32(f11)
	read32(f12)
	read32(f13)
	read32(f14)
	read32(f15)
	read32(f16)
	read32(f17)
	read32(f18)
	read32(f19)
	read32(f20)
	read32(f21)
	read32(f22)
	read32(f23)
	read32(f24)
	read32(f25)
	read32(f26)
	read32(f27)
	read32(f28)
	read32(f29)
	read32(f30)
	read32(f31)

/* void write_f32(int regnum, uint32_t* v)
 *    regnum: which register want to write
 *    v : address to hold bits info of writing
 */
	.align	1
	.globl write_f32
write_f32:
	la	a2, .Lw32_t
	andi	a0, a0, 31
	slli	a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr	a0
	.align	2
.Lw32_t:
	.half	.Lw32_f0  - .Lw32_t
	.half	.Lw32_f1  - .Lw32_t
	.half	.Lw32_f2  - .Lw32_t
	.half	.Lw32_f3  - .Lw32_t
	.half	.Lw32_f4  - .Lw32_t
	.half	.Lw32_f5  - .Lw32_t
	.half	.Lw32_f6  - .Lw32_t
	.half	.Lw32_f7  - .Lw32_t
	.half	.Lw32_f8  - .Lw32_t
	.half	.Lw32_f9  - .Lw32_t
	.half	.Lw32_f10 - .Lw32_t
	.half	.Lw32_f11 - .Lw32_t
	.half	.Lw32_f12 - .Lw32_t
	.half	.Lw32_f13 - .Lw32_t
	.half	.Lw32_f14 - .Lw32_t
	.half	.Lw32_f15 - .Lw32_t
	.half	.Lw32_f16 - .Lw32_t
	.half	.Lw32_f17 - .Lw32_t
	.half	.Lw32_f18 - .Lw32_t
	.half	.Lw32_f19 - .Lw32_t
	.half	.Lw32_f20 - .Lw32_t
	.half	.Lw32_f21 - .Lw32_t
	.half	.Lw32_f22 - .Lw32_t
	.half	.Lw32_f23 - .Lw32_t
	.half	.Lw32_f24 - .Lw32_t
	.half	.Lw32_f25 - .Lw32_t
	.half	.Lw32_f26 - .Lw32_t
	.half	.Lw32_f27 - .Lw32_t
	.half	.Lw32_f28 - .Lw32_t
	.half	.Lw32_f29 - .Lw32_t
	.half	.Lw32_f30 - .Lw32_t
	.half	.Lw32_f31 - .Lw32_t
#define write32(which)	.Lw32_##which: flw which, 0(a1); ret
	write32(f0)
	write32(f1)
	write32(f2)
	write32(f3)
	write32(f4)
	write32(f5)
	write32(f6)
	write32(f7)
	write32(f8)
	write32(f9)
	write32(f10)
	write32(f11)
	write32(f12)
	write32(f13)
	write32(f14)
	write32(f15)
	write32(f16)
	write32(f17)
	write32(f18)
	write32(f19)
	write32(f20)
	write32(f21)
	write32(f22)
	write32(f23)
	write32(f24)
	write32(f25)
	write32(f26)
	write32(f27)
	write32(f28)
	write32(f29)
	write32(f30)
	write32(f31)
#endif // __riscv_flen >= 32

#if __riscv_flen >= 64

	.text

/* void read_f64(int regnum, uint64_t* v)
 *    regnum : which register want to read
 *    v : address to hold bits info of reading
 */
	.align	1
	.globl read_f64
read_f64:
	la	a2, .Lr64_t
	andi	a0, a0, 31
	slli	a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr	a0
	.align	2
.Lr64_t:
	.half	.Lr64_f0  - .Lr64_t
	.half	.Lr64_f1  - .Lr64_t
	.half	.Lr64_f2  - .Lr64_t
	.half	.Lr64_f3  - .Lr64_t
	.half	.Lr64_f4  - .Lr64_t
	.half	.Lr64_f5  - .Lr64_t
	.half	.Lr64_f6  - .Lr64_t
	.half	.Lr64_f7  - .Lr64_t
	.half	.Lr64_f8  - .Lr64_t
	.half	.Lr64_f9  - .Lr64_t
	.half	.Lr64_f10 - .Lr64_t
	.half	.Lr64_f11 - .Lr64_t
	.half	.Lr64_f12 - .Lr64_t
	.half	.Lr64_f13 - .Lr64_t
	.half	.Lr64_f14 - .Lr64_t
	.half	.Lr64_f15 - .Lr64_t
	.half	.Lr64_f16 - .Lr64_t
	.half	.Lr64_f17 - .Lr64_t
	.half	.Lr64_f18 - .Lr64_t
	.half	.Lr64_f19 - .Lr64_t
	.half	.Lr64_f20 - .Lr64_t
	.half	.Lr64_f21 - .Lr64_t
	.half	.Lr64_f22 - .Lr64_t
	.half	.Lr64_f23 - .Lr64_t
	.half	.Lr64_f24 - .Lr64_t
	.half	.Lr64_f25 - .Lr64_t
	.half	.Lr64_f26 - .Lr64_t
	.half	.Lr64_f27 - .Lr64_t
	.half	.Lr64_f28 - .Lr64_t
	.half	.Lr64_f29 - .Lr64_t
	.half	.Lr64_f30 - .Lr64_t
	.half	.Lr64_f31 - .Lr64_t
#define  read64(which)	.Lr64_##which: fsd which, 0(a1); ret
	read64(f0)
	read64(f1)
	read64(f2)
	read64(f3)
	read64(f4)
	read64(f5)
	read64(f6)
	read64(f7)
	read64(f8)
	read64(f9)
	read64(f10)
	read64(f11)
	read64(f12)
	read64(f13)
	read64(f14)
	read64(f15)
	read64(f16)
	read64(f17)
	read64(f18)
	read64(f19)
	read64(f20)
	read64(f21)
	read64(f22)
	read64(f23)
	read64(f24)
	read64(f25)
	read64(f26)
	read64(f27)
	read64(f28)
	read64(f29)
	read64(f30)
	read64(f31)

/* void write_f64(int regnum, uint64_t* v)
 *    regnum: which register want to write
 *    v : address to hold bits info of writing
 */
	.align	1
	.globl write_f64
write_f64:
	la	a2, .Lw64_t
	andi	a0, a0, 31
	slli	a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr	a0
	.align	2
.Lw64_t:
	.half	.Lw64_f0  - .Lw64_t
	.half	.Lw64_f1  - .Lw64_t
	.half	.Lw64_f2  - .Lw64_t
	.half	.Lw64_f3  - .Lw64_t
	.half	.Lw64_f4  - .Lw64_t
	.half	.Lw64_f5  - .Lw64_t
	.half	.Lw64_f6  - .Lw64_t
	.half	.Lw64_f7  - .Lw64_t
	.half	.Lw64_f8  - .Lw64_t
	.half	.Lw64_f9  - .Lw64_t
	.half	.Lw64_f10 - .Lw64_t
	.half	.Lw64_f11 - .Lw64_t
	.half	.Lw64_f12 - .Lw64_t
	.half	.Lw64_f13 - .Lw64_t
	.half	.Lw64_f14 - .Lw64_t
	.half	.Lw64_f15 - .Lw64_t
	.half	.Lw64_f16 - .Lw64_t
	.half	.Lw64_f17 - .Lw64_t
	.half	.Lw64_f18 - .Lw64_t
	.half	.Lw64_f19 - .Lw64_t
	.half	.Lw64_f20 - .Lw64_t
	.half	.Lw64_f21 - .Lw64_t
	.half	.Lw64_f22 - .Lw64_t
	.half	.Lw64_f23 - .Lw64_t
	.half	.Lw64_f24 - .Lw64_t
	.half	.Lw64_f25 - .Lw64_t
	.half	.Lw64_f26 - .Lw64_t
	.half	.Lw64_f27 - .Lw64_t
	.half	.Lw64_f28 - .Lw64_t
	.half	.Lw64_f29 - .Lw64_t
	.half	.Lw64_f30 - .Lw64_t
	.half	.Lw64_f31 - .Lw64_t
#define write64(which)	.Lw64_##which: fld which, 0(a1); ret
	write64(f0)
	write64(f1)
	write64(f2)
	write64(f3)
	write64(f4)
	write64(f5)
	write64(f6)
	write64(f7)
	write64(f8)
	write64(f9)
	write64(f10)
	write64(f11)
	write64(f12)
	write64(f13)
	write64(f14)
	write64(f15)
	write64(f16)
	write64(f17)
	write64(f18)
	write64(f19)
	write64(f20)
	write64(f21)
	write64(f22)
	write64(f23)
	write64(f24)
	write64(f25)
	write64(f26)
	write64(f27)
	write64(f28)
	write64(f29)
	write64(f30)
	write64(f31)

#endif // __riscv_flen >= 64

#endif // defined(__riscv_flen)
