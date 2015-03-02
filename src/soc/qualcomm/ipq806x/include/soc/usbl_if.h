/*
 * This file is part of the coreboot project.
 * Copyright (C) 2015 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __SOC_QUALCOMM_IPQ806X_INCLUDE_SOC_USBL_IF_H__
#define __SOC_QUALCOMM_IPQ806X_INCLUDE_SOC_USBL_IF_H__

#include <types.h>

typedef struct {
	u32	time_stamp;
	char	msg[27];
	u8	type;

} boot_log_entry;

typedef struct {
	u32		num_log_entries;
	boot_log_entry	*log;
} sbl_ro_info;

typedef struct {
	u32		start_magic;
	u32		num;
	char		*version;
	sbl_ro_info	info[2];
	/*
	 * The two addresses below can be used for communicating with the RPM
	 * (passing it the starting address of the program to execute and
	 * triggering the jump to the program). Presently these addresses are
	 * hardcodeded in firmware source code.
	 */
	u32		*rpm_jump_trigger;
	u32		*rpm_entry_addr_ptr;
	u32		end_magic;
} uber_sbl_shared_info;

#define UBER_SBL_SHARED_INFO_START_MAGIC	0x5552504d // URPM
#define UBER_SBL_SHARED_INFO_END_MAGIC		0x554b5254 // UKRT

extern uber_sbl_shared_info *maskrom_param;

#endif
