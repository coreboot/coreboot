/* SPDX-License-Identifier: GPL-2.0-only */

#include <vm.h>
#include <arch/exception.h>
#include <commonlib/helpers.h>
#include <types.h>

/*  these functions are defined in src/arch/riscv/fp_asm.S */
#if defined(__riscv_flen)
#if __riscv_flen >= 32
extern void  read_f32(int regnum, uint32_t *v);
extern void write_f32(int regnum, uint32_t *v);
#endif // __riscv_flen >= 32
#if __riscv_flen >= 64
extern void  read_f64(int regnum, uint64_t *v);
extern void write_f64(int regnum, uint64_t *v);
#endif // __riscv_flen >= 64
#endif // defined(__riscv_flen)

/* This union makes it easy to read multibyte types by byte operations. */
union endian_buf {
	uint8_t   b[8];
	uint16_t  h[4];
	uint32_t  w[2];
	uint64_t  d[1];
	uintptr_t v;
};

/* This struct hold info of load/store instruction */
struct memory_instruction_info {
	/* opcode/mask used to identify instruction,
	 * (instruction_val) & mask == opcode */
	uint32_t opcode;
	uint32_t mask;
	/* reg_shift/reg_mask/reg_addition used to get register number
	 * ((instruction_val >> reg_shift) & reg_mask) + reg_addition */
	unsigned int reg_shift;
	unsigned int reg_mask;
	unsigned int reg_addition;
	unsigned int is_fp : 1;   /* mark as a float operation */
	unsigned int is_load : 1; /* mark as a load operation */
	unsigned int width : 8;   /* Record the memory width of the operation */
	unsigned int sign_extend : 1; /* mark need to be sign extended */
};

static struct memory_instruction_info insn_info[] = {
#if __riscv_xlen == 128
	{ 0x00002000, 0x0000e003,  2,  7, 8, 0, 1, 16, 1}, // C.LQ
#else
	{ 0x00002000, 0x0000e003,  2,  7, 8, 1, 1,  8, 0}, // C.FLD
#endif
	{ 0x00004000, 0x0000e003,  2,  7, 8, 0, 1,  4, 1}, // C.LW
#if __riscv_xlen == 32
	{ 0x00006000, 0x0000e003,  2,  7, 8, 1, 1,  4, 0}, // C.FLW
#else
	{ 0x00006000, 0x0000e003,  2,  7, 8, 0, 1,  8, 1}, // C.LD
#endif

#if __riscv_xlen == 128
	{ 0x0000a000, 0x0000e003,  2,  7, 8, 0, 0, 16, 0}, // C.SQ
#else
	{ 0x0000a000, 0x0000e003,  2,  7, 8, 1, 0,  8, 0}, // C.FSD
#endif
	{ 0x0000c000, 0x0000e003,  2,  7, 8, 0, 0,  4, 0}, // C.SW
#if __riscv_xlen == 32
	{ 0x0000e000, 0x0000e003,  2,  7, 8, 1, 0,  4, 0}, // C.FSW
#else
	{ 0x0000e000, 0x0000e003,  2,  7, 8, 0, 0,  8, 0}, // C.SD
#endif

#if __riscv_xlen == 128
	{ 0x00002002, 0x0000e003,  7, 15, 0, 0, 1, 16, 1}, // C.LQSP
#else
	{ 0x00002002, 0x0000e003,  7, 15, 0, 1, 1,  8, 0}, // C.FLDSP
#endif
	{ 0x00004002, 0x0000e003,  7, 15, 0, 0, 1,  4, 1}, // C.LWSP
#if __riscv_xlen == 32
	{ 0x00006002, 0x0000e003,  7, 15, 0, 1, 1,  4, 0}, // C.FLWSP
#else
	{ 0x00006002, 0x0000e003,  7, 15, 0, 0, 1,  8, 1}, // C.LDSP
#endif

#if __riscv_xlen == 128
	{ 0x0000a002, 0x0000e003,  2, 15, 0, 0, 0, 16, 0}, // C.SQSP
#else
	{ 0x0000a002, 0x0000e003,  2, 15, 0, 1, 0,  8, 0}, // C.FSDSP
#endif
	{ 0x0000c002, 0x0000e003,  2, 15, 0, 0, 0,  4, 0}, // C.SWSP
#if __riscv_xlen == 32
	{ 0x0000e002, 0x0000e003,  2, 15, 0, 1, 0,  4, 0}, // C.FSWSP
#else
	{ 0x0000e002, 0x0000e003,  2, 15, 0, 0, 0,  8, 0}, // C.SDSP
#endif

	{ 0x00000003, 0x0000707f,  7, 15, 0, 0, 1,  1, 1}, // LB
	{ 0x00001003, 0x0000707f,  7, 15, 0, 0, 1,  2, 1}, // LH
	{ 0x00002003, 0x0000707f,  7, 15, 0, 0, 1,  4, 1}, // LW
#if __riscv_xlen > 32
	{ 0x00003003, 0x0000707f,  7, 15, 0, 0, 1,  8, 1}, // LD
#endif
	{ 0x00004003, 0x0000707f,  7, 15, 0, 0, 1,  1, 0}, // LBU
	{ 0x00005003, 0x0000707f,  7, 15, 0, 0, 1,  2, 0}, // LHU
	{ 0x00006003, 0x0000707f,  7, 15, 0, 0, 1,  4, 0}, // LWU

	{ 0x00000023, 0x0000707f, 20, 15, 0, 0, 0,  1, 0}, // SB
	{ 0x00001023, 0x0000707f, 20, 15, 0, 0, 0,  2, 0}, // SH
	{ 0x00002023, 0x0000707f, 20, 15, 0, 0, 0,  4, 0}, // SW
#if __riscv_xlen > 32
	{ 0x00003023, 0x0000707f, 20, 15, 0, 0, 0,  8, 0}, // SD
#endif

#if defined(__riscv_flen)
#if __riscv_flen >= 32
	{ 0x00002007, 0x0000707f,  7, 15, 0, 1, 1,  4, 0}, // FLW
	{ 0x00003007, 0x0000707f,  7, 15, 0, 1, 1,  8, 0}, // FLD
#endif // __riscv_flen >= 32

#if __riscv_flen >= 64
	{ 0x00002027, 0x0000707f, 20, 15, 0, 1, 0,  4, 0}, // FSW
	{ 0x00003027, 0x0000707f, 20, 15, 0, 1, 0,  8, 0}, // FSD
#endif // __riscv_flen >= 64
#endif // defined(__riscv_flen)
};

static struct memory_instruction_info *match_instruction(uintptr_t insn)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(insn_info); i++)
		if ((insn_info[i].mask & insn) == insn_info[i].opcode)
			return &(insn_info[i]);
	return NULL;
}

static enum cb_err fetch_16bit_instruction(uintptr_t vaddr, uintptr_t *insn, int *size)
{
	uint16_t ins = mprv_read_mxr_u16((uint16_t *)vaddr);
	if (EXTRACT_FIELD(ins, 0x3) != 3) {
		*insn = ins;
		*size = 2;
		return CB_SUCCESS;
	}
	return CB_ERR;
}

static enum cb_err fetch_32bit_instruction(uintptr_t vaddr, uintptr_t *insn, int *size)
{
	uint32_t l = (uint32_t)mprv_read_mxr_u16((uint16_t *)vaddr + 0);
	uint32_t h = (uint32_t)mprv_read_mxr_u16((uint16_t *)vaddr + 1);
	uint32_t ins = (h << 16) | l;
	if ((EXTRACT_FIELD(ins, 0x3) == 3) &&
		(EXTRACT_FIELD(ins, 0x1c) != 0x7)) {
		*insn = ins;
		*size = 4;
		return CB_SUCCESS;
	}
	return CB_ERR;
}

void handle_misaligned(trapframe *tf)
{
	uintptr_t insn = 0;
	union endian_buf buff;
	int insn_size = 0;

	/* try to fetch 16/32 bits instruction */
	if (fetch_16bit_instruction(tf->epc, &insn, &insn_size) < 0) {
		if (fetch_32bit_instruction(tf->epc, &insn, &insn_size) < 0) {
			redirect_trap();
			return;
		}
	}

	/* matching instruction */
	struct memory_instruction_info *match = match_instruction(insn);

	if (!match) {
		redirect_trap();
		return;
	}

	int regnum;
	regnum = ((insn >> match->reg_shift) & match->reg_mask);
	regnum = regnum + match->reg_addition;
	buff.v = 0;
	if (match->is_load) {
		/* load operation */

		/* reading from memory by bytes prevents misaligned
		 * memory access */
		for (int i = 0; i < match->width; i++) {
			uint8_t *addr = (uint8_t *)(tf->badvaddr + i);
			buff.b[i] = mprv_read_u8(addr);
		}

		/* sign extend for signed integer loading */
		if (match->sign_extend)
			if (buff.v >> (8 * match->width - 1))
				buff.v |= -1 << (8 * match->width);

		/* write to register */
		if (match->is_fp) {
			int done = 0;
#if defined(__riscv_flen)
#if __riscv_flen >= 32
			/* single-precision floating-point */
			if (match->width == 4) {
				write_f32(regnum, buff.w);
				done = 1;
			}
#endif // __riscv_flen >= 32
#if __riscv_flen >= 64
			/* double-precision floating-point */
			if (match->width == 8) {
				write_f64(regnum, buff.d);
				done = 1;
			}
#endif // __riscv_flen >= 64
#endif // defined(__riscv_flen)
			if (!done)
				redirect_trap();
		} else {
			tf->gpr[regnum] = buff.v;
		}
	} else {
		/* store operation */

		/* reading from register */
		if (match->is_fp) {
			int done = 0;
#if defined(__riscv_flen)
#if __riscv_flen >= 32
			if (match->width == 4) {
				read_f32(regnum, buff.w);
				done = 1;
			}
#endif // __riscv_flen >= 32
#if __riscv_flen >= 64
			if (match->width == 8) {
				read_f64(regnum, buff.d);
				done = 1;
			}
#endif // __riscv_flen >= 64
#endif // defined(__riscv_flen)
			if (!done)
				redirect_trap();
		} else {
			buff.v = tf->gpr[regnum];
		}

		/* writing to memory by bytes prevents misaligned
		 * memory access */
		for (int i = 0; i < match->width; i++) {
			uint8_t *addr = (uint8_t *)(tf->badvaddr + i);
			mprv_write_u8(addr, buff.b[i]);
		}
	}

	/* return to where we came from */
	write_csr(mepc, read_csr(mepc) + insn_size);
}
