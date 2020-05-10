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

#define SBI_ENOSYS 38

#define IPI_SOFT            1
#define IPI_FENCE_I         2
#define IPI_SFENCE_VMA      4
#define IPI_SFENCE_VMA_ASID 8
#define IPI_SHUTDOWN        16

void handle_sbi(trapframe *tf);

#endif /* RISCV_SBI_H */
