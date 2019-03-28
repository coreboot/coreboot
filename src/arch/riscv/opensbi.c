/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 9elements Agency GmbH <patrick.rudolph@9elements.com>
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

#include <sbi/fw_dynamic.h>
#include <arch/boot.h>
/* DO NOT INLCUDE COREBOOT HEADERS HERE */

void run_opensbi(const int hart_id,
		 const void *fdt,
		 const void *opensbi,
		 const void *payload,
		 const int payload_mode)
{
	struct fw_dynamic_info info = {
		.magic = FW_DYNAMIC_INFO_MAGIC_VALUE,
		.version = FW_DYNAMIC_INFO_VERSION_MAX,
		.next_mode = payload_mode,
		.next_addr = (uintptr_t)payload,
	};

	csr_write(mepc, opensbi);
	asm volatile (
			"mv	a0, %0\n\t"
			"mv	a1, %1\n\t"
			"mv	a2, %2\n\t"
			"mret" :
			: "r"(hart_id), "r"(fdt), "r"(&info)
			: "a0", "a1", "a2");
}
