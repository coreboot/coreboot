/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RISCV_SBI_H
#define RISCV_SBI_H

#define SBI_SET_TIMER              0
#define SBI_CONSOLE_PUTCHAR        1
#define SBI_CONSOLE_GETCHAR        2
#define SBI_CLEAR_IPI              3
#define SBI_SEND_IPI               4
#define SBI_REMOTE_FENCE_I         5
#define SBI_REMOTE_SFENCE_VMA      6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN               8

// Here begins the complex set of things added by
// later SBI. Unlike the original design, these
// require bits of state to be maintained in SBI.
// Disappointing!
#define SBI_BASE_EXTENSION 0x10
#  define SBI_GET_SBI_SPEC_VERSION    0
#  define SBI_GET_SBI_IMPL_ID         1
#  define SBI_GET_SBI_IMPL_VERSION    2
#  define SBI_PROBE_EXTENSION         3
#  define SBI_GET_MVENDORID           4
#  define SBI_GET_MARCHID             5
#  define SBI_GET_MIMPID              6

#define SBI_ENOSYS 38

#define IPI_SOFT            1
#define IPI_FENCE_I         2
#define IPI_SFENCE_VMA      4
#define IPI_SFENCE_VMA_ASID 8
#define IPI_SHUTDOWN        16

void handle_sbi(struct trapframe *tf);

#endif /* RISCV_SBI_H */
