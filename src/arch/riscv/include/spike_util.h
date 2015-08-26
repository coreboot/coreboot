/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors
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
 * Foundation, Inc.
 */

#ifndef _SPIKE_UTIL_H
#define _SPIKE_UTIL_H

#include <stdint.h>
//#include <string.h>
//#include <errno.h>
#include <arch/encoding.h>
#include <atomic.h>

#define LOG_REGBYTES 3
#define REGBYTES (1 << LOG_REGBYTES)
#define STORE    sd
#define HLS_SIZE 64
#define MENTRY_FRAME_SIZE HLS_SIZE

#define TOHOST_CMD(dev, cmd, payload) \
    (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

#define FROMHOST_DEV(fromhost_value) ((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

typedef struct {
  unsigned long base;
  unsigned long size;
  unsigned long node_id;
} memory_block_info;

typedef struct {
  unsigned long dev;
  unsigned long cmd;
  unsigned long data;
  unsigned long sbi_private_data;
} sbi_device_message;


typedef struct {
  sbi_device_message* device_request_queue_head;
  unsigned long device_request_queue_size;
  sbi_device_message* device_response_queue_head;
  sbi_device_message* device_response_queue_tail;

  int hart_id;
  int ipi_pending;
} hls_t;

#define MACHINE_STACK_TOP() ({ \
  register uintptr_t sp asm ("sp"); \
  (void*)((sp + RISCV_PGSIZE) & -RISCV_PGSIZE); })

// hart-local storage, at top of stack
#define HLS() ((hls_t*)(MACHINE_STACK_TOP() - HLS_SIZE))
#define OTHER_HLS(id) ((hls_t*)((void*)HLS() + RISCV_PGSIZE * ((id) - HLS()->hart_id)))

#define MACHINE_STACK_SIZE RISCV_PGSIZE

uintptr_t translate_address(uintptr_t vAddr);
uintptr_t mcall_query_memory(uintptr_t id, memory_block_info *p);
uintptr_t mcall_hart_id(void);
uintptr_t htif_interrupt(uintptr_t mcause, uintptr_t* regs);
uintptr_t mcall_console_putchar(uint8_t ch);
void testPrint(void);
uintptr_t mcall_dev_req(sbi_device_message *m);
uintptr_t mcall_dev_resp(void);
uintptr_t mcall_set_timer(unsigned long long when);
uintptr_t mcall_clear_ipi(void);
uintptr_t mcall_send_ipi(uintptr_t recipient);
uintptr_t mcall_shutdown(void);
void hls_init(uint32_t hart_id); // need to call this before launching linux

#endif
