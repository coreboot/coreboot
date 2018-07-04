/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_ARM64_TRANSITION_H__
#define __ARCH_ARM64_TRANSITION_H__

/* ======================== Transition Library =================================
 * Transition library provides two main functionalities:
 * 1) It allows any program X to be executed at EL Y using the state Z. It
 * provides struct exc_state which holds the state of the EL to which we want to
 * execute X at. Before performing an eret to the entry point of the program X,
 * it initializes required registers using this exc_state structure. Here, X0 =
 * args to the program X. IMP!!! : We do not initialize SP_EL0 for the program
 * X, the program will have to handle that on its own. This is because while
 * performing an eret to X, we could make SP_EL0 point to regs structure which
 * then follows common exception exit path.
 * 2) It serves as a common mechanism for handling exception entry and exit at
 * any given EL. On entry to an exception, SP_ELx is selected by default. The
 * exc entry routine stores all xregs and jumps to exc_entry which
 * saves ELR, SPSR, EL, Mode and other information about the state from which
 * exception was generated. On exit, xregs are restored by unwinding of SP_ELx.
 * =============================================================================
 */

/* Macros for EL mode in SPSR */
#define STACK_POP_BYTES	 16
#define STACK_PUSH_BYTES -16

#define EXC_VID_CUR_SP_EL0_SYNC		0
#define EXC_VID_CUR_SP_EL0_IRQ		1
#define EXC_VID_CUR_SP_EL0_FIRQ		2
#define EXC_VID_CUR_SP_EL0_SERR		3
#define EXC_VID_CUR_SP_ELX_SYNC		4
#define EXC_VID_CUR_SP_ELX_IRQ		5
#define EXC_VID_CUR_SP_ELX_FIQ		6
#define EXC_VID_CUR_SP_ELX_SERR		7
#define EXC_VID_LOW64_SYNC		8
#define EXC_VID_LOW64_IRQ		9
#define EXC_VID_LOW64_FIQ		10
#define EXC_VID_LOW64_SERR		11
#define EXC_VID_LOW32_SYNC		12
#define EXC_VID_LOW32_IRQ		13
#define EXC_VID_LOW32_FIQ		14
#define EXC_VID_LOW32_SERR		15
#define NUM_EXC_VIDS			16

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <arch/lib_helpers.h>

#define XI_INDEX(i)    X##i##_INDEX = i

enum {
	XI_INDEX(0),
	XI_INDEX(1),
	XI_INDEX(2),
	XI_INDEX(3),
	XI_INDEX(4),
	XI_INDEX(5),
	XI_INDEX(6),
	XI_INDEX(7),
	XI_INDEX(8),
	XI_INDEX(9),
	XI_INDEX(10),
	XI_INDEX(11),
	XI_INDEX(12),
	XI_INDEX(13),
	XI_INDEX(14),
	XI_INDEX(15),
	XI_INDEX(16),
	XI_INDEX(17),
	XI_INDEX(18),
	XI_INDEX(19),
	XI_INDEX(20),
	XI_INDEX(21),
	XI_INDEX(22),
	XI_INDEX(23),
	XI_INDEX(24),
	XI_INDEX(25),
	XI_INDEX(26),
	XI_INDEX(27),
	XI_INDEX(28),
	XI_INDEX(29),
	XI_INDEX(30),
	XMAX_INDEX,
};

/*
 * Important: Any changes made to the two structures below should reflect in the
 *  exc_prologue and exc_exit routines in transition_asm.S
 */
struct regs {
	uint64_t sp;
	uint64_t x[31];
};

struct elx_state {
	uint64_t spsr;
	uint64_t sp_el0;
	uint64_t sp_elx;
	uint64_t elr;
};

struct exc_state {
	struct elx_state elx;
	struct regs regs;
};

/*
 * get_eret_EL returns the value of the exception state to which we will be
 * returning. This value is saved in SPSR before performing an eret.
 *
 * Exception mode is defined by M[3:0] bits in SPSR:
 * ( M[3:2] = EL, M[1] = unused, M[0] = t/h mode for stack
 *
 * 0b0000 EL0t
 * 0b0100 EL1t
 * 0b0101 EL1h
 * 0b1000 EL2t
 * 0b1001 EL2h
 * 0b1100 EL3t
 * 0b1101 EL3h
 */

static inline uint8_t get_eret_el(uint8_t el, uint8_t l_or_h)
{
	uint8_t el_mode = el << CURRENT_EL_SHIFT;

	el_mode |= l_or_h;

	return el_mode;
}

static inline uint8_t get_el_from_spsr(uint64_t spsr)
{
	return ((spsr >> CURRENT_EL_SHIFT) & CURRENT_EL_MASK);
}

static inline uint8_t get_mode_from_spsr(uint64_t spsr)
{
	return (spsr & SPSR_L_H_MASK);
}

/* Transitions supported are:
 * 1. elx --> elx - 1
 * 2. Transitions to aarch64 state
 *
 * Other than this, if any transition needs to be supported, relevant changes
 * need to be done to hcr/scr registers.
 */

/*
 * User of transition library can make a call to transition_with_entry and pass
 * the entry point and its argument which are put into elr and x0 by this
 * function. After that it makes a call to transition.
 */
void transition_with_entry(void *entry, void *arg, struct exc_state *exc_state);
/*
 * transition function sets up all the registers as per the struct elx_state
 * before jumping to trans_switch.
 */
void transition(struct exc_state *exc_state);

/*
 * exc_exit it called while returning from an exception. It expects pointer to
 * the regs structure on stack so that it can unwind the used stack.
 */
void exc_exit(struct regs *regs);
/*
 * trans_switch is called by the non-exception path i.e. transition C code
 * while making a transition to lower EL. It select L mode so that SP_EL0 is
 * used during the unwinding in exc_exit.
 */
void trans_switch(struct regs *regs);
/* exc_set_vbar sets up the vbar for exception vectors. */
void exc_set_vbar(void);

/* exc_dispatch is the user-defined exception handler. */
void exc_dispatch(struct exc_state *exc_state, uint64_t id);
/*
 * exc_entry is the C based component of the exception entry before we
 * jump to user-defined handler. This initializes all the regs in elx_state and
 * also sets the sp value in regs structure.
 */
void exc_entry(struct exc_state *exc_state, uint64_t id);

#endif /* __ASSEMBLER__ */

#endif /* __ARCH_ARM64_TRANSITION_H__ */
