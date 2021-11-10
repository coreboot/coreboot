/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 coresystems GmbH
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

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>

/*
 * Some of this is x86 specific, and the rest of it is generic. Right now,
 * since we only support x86, we'll avoid trying to make lots of infrastructure
 * we don't need. If in the future, we want to use coreboot on some other
 * architecture, then take out the generic parsing code and move it elsewhere.
 */

/* === Parsing code === */
/* This is the generic parsing code. */

static void cb_parse_x86_rom_var_mtrr(void *ptr, struct sysinfo_t *info)
{
	struct cb_x86_rom_mtrr *rom_mtrr = ptr;
	info->x86_rom_var_mtrr_index = rom_mtrr->index;
}

int cb_parse_arch_specific(struct cb_record *rec, struct sysinfo_t *info)
{
	switch(rec->tag) {
	case CB_TAG_X86_ROM_MTRR:
		cb_parse_x86_rom_var_mtrr(rec, info);
		break;
	default:
		return 0;
	}
	return 1;
}

int get_coreboot_info(struct sysinfo_t *info)
{
	int ret;

	/* Ensure the variable range MTRR index covering the ROM is set to
	 * an invalid value. */
	info->x86_rom_var_mtrr_index = -1;

	ret = cb_parse_header(phys_to_virt(0x00000000), 0x1000, info);

	if (ret)
		ret = cb_parse_header(phys_to_virt(0x000f0000), 0x1000, info);

	return ret;
}
