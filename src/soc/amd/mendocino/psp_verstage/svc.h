/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#ifndef PSP_VERSTAGE_SVC_H
#define PSP_VERSTAGE_SVC_H

#include <bl_uapp/bl_syscall_public.h>
#include <types.h>

#define SVC_CALL4(SVC_ID, R0, R1, R2, R3, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"mov r2, %[reg2]\n\t" \
	"mov r3, %[reg3]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1), [reg2] "r" (R2), \
		[reg3] "r" (R3) /* input(s) */ \
	: "r0", "r1", "r2", "r3", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL3(SVC_ID, R0,  R1, R2, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"mov r2, %[reg2]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1), [reg2] "r" (R2) \
	: "r0", "r1", "r2", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL2(SVC_ID, R0, R1, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1)/* input(s) */ \
	: "r0", "r1", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL1(SVC_ID, R0, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0) /* input(s) */ \
	: "r0", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL0(SVC_ID, Ret) \
	__asm__ __volatile__ ( \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "I" (SVC_ID) /* input(s) */ \
	: "memory", "cc" /* list of clobbered registers */)

struct cmd_param_sha {
	struct sha_generic_data *ptr_sha_op;
};

struct cmd_param_debug {
	char *debug_buffer;
	uint32_t debug_buffer_len;
};

struct cmd_param_debug_ex {
	uint32_t word0;
	uint32_t word1;
	uint32_t word2;
	uint32_t word3;
};

struct cmd_param_modexp {
	struct mod_exp_params *ptr_modexp;
};

struct cmd_param_psp_update {
	unsigned int *ptr_psp_dir_addr;
};

struct cmd_param_spirom_info {
	struct spirom_info *ptr_spirom_info;
};

struct cmd_param_map_spirom {
	unsigned int spirom_addr;
	unsigned int size;
	void **ppspirom_addr_axi;
};

struct cmd_param_unmap_spirom {
	void *ptr_spirom_addr_axi;
};

struct cmd_param_read_timer_val {
	enum psp_timer_type timer_type;
	uint64_t *ptr_counter_value;
};

struct cmd_param_delay_in_micro_second {
	uint32_t delay;
};

struct cmd_param_reset_system {
	uint32_t reset_type;
};

struct cmd_param_get_boot_mode {
	unsigned int *ptr_boot_mode;
};

struct cmd_param_copy_data_from_uapp {
	unsigned int address;
	unsigned int size;
};

struct cmd_param_map_fch_io_device {
	enum fch_io_device io_device;
	unsigned int arg1;
	unsigned int arg2;
	void **pptr_io_device_addr_axi;
};

struct cmd_param_unmap_fch_io_device {
	enum fch_io_device io_device;
	void *ptr_io_device_addr_axi;
};

struct cmd_param_ccp_dma {
	uint32_t spi_offset;
	uint32_t dst_addr;
	uint32_t size;
};

struct cmd_param_set_platform_boot_mode {
	uint32_t boot_mode;
};

#endif /* PSP_VERSTAGE_SVC_H */
