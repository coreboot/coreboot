/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_RISCV_INCLUDE_ARCH_BOOT_H
#define ARCH_RISCV_INCLUDE_ARCH_BOOT_H

#define RISCV_PAYLOAD_MODE_U 0
#define RISCV_PAYLOAD_MODE_S 1
#define RISCV_PAYLOAD_MODE_M 3

struct prog;
void run_payload(struct prog *prog, void *fdt, int payload_mode);
void run_payload_opensbi(struct prog *prog, void *fdt, struct prog *opensbi, int payload_mode);

void run_opensbi(const int hart_id,
		 const void *opensbi,
		 const void *fdt,
		 const void *payload,
		 const int payload_mode);
#endif
