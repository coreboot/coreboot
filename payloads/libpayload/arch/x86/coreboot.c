/*
 * This file is part of the libpayload project.
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

static void cb_parse_memory(void *ptr, struct sysinfo_t *info)
{
	struct cb_memory *mem = ptr;
	int count = MEM_RANGE_COUNT(mem);
	int i;

	if (count > SYSINFO_MAX_MEM_RANGES)
		count = SYSINFO_MAX_MEM_RANGES;

	info->n_memranges = 0;

	for (i = 0; i < count; i++) {
		struct cb_memory_range *range = MEM_RANGE_PTR(mem, i);

#ifdef CONFIG_MEMMAP_RAM_ONLY
		if (range->type != CB_MEM_RAM)
			continue;
#endif

		info->memrange[info->n_memranges].base =
		    cb_unpack64(range->start);

		info->memrange[info->n_memranges].size =
		    cb_unpack64(range->size);

		info->memrange[info->n_memranges].type = range->type;

		info->n_memranges++;
	}
}

static void cb_parse_serial(void *ptr, struct sysinfo_t *info)
{
	info->serial = ((struct cb_serial *)ptr);
}

#ifdef CONFIG_CHROMEOS
static void cb_parse_vboot_handoff(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_vboot_handoff *vbho = (struct cb_vboot_handoff *)ptr;

	info->vboot_handoff = vbho->vboot_handoff_addr;
	info->vboot_handoff_size = vbho->vboot_handoff_size;
}

static void cb_parse_vbnv(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_vbnv *vbnv = (struct cb_vbnv *)ptr;

	info->vbnv_start = vbnv->vbnv_start;
	info->vbnv_size = vbnv->vbnv_size;
}

static void cb_parse_gpios(unsigned char *ptr, struct sysinfo_t *info)
{
	int i;
	struct cb_gpios *gpios = (struct cb_gpios *)ptr;

	info->num_gpios = (gpios->count < SYSINFO_MAX_GPIOS) ?
				(gpios->count) : SYSINFO_MAX_GPIOS;

	for (i = 0; i < info->num_gpios; i++)
		info->gpios[i] = gpios->gpios[i];
}

static void cb_parse_vdat(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_vdat *vdat = (struct cb_vdat *) ptr;

	info->vdat_addr = phys_to_virt(vdat->vdat_addr);
	info->vdat_size = vdat->vdat_size;
}
#endif

static void cb_parse_tstamp(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->tstamp_table = phys_to_virt(cbmem->cbmem_tab);
}

static void cb_parse_cbmem_cons(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->cbmem_cons = phys_to_virt(cbmem->cbmem_tab);
}

static void cb_parse_mrc_cache(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->mrc_cache = phys_to_virt(cbmem->cbmem_tab);
}

#ifdef CONFIG_NVRAM
static void cb_parse_optiontable(void *ptr, struct sysinfo_t *info)
{
	/* ptr points to a coreboot table entry and is already virtual */
	info->option_table = ptr;
}

static void cb_parse_checksum(void *ptr, struct sysinfo_t *info)
{
	struct cb_cmos_checksum *cmos_cksum = ptr;
	info->cmos_range_start = cmos_cksum->range_start;
	info->cmos_range_end = cmos_cksum->range_end;
	info->cmos_checksum_location = cmos_cksum->location;
}
#endif

#ifdef CONFIG_COREBOOT_VIDEO_CONSOLE
static void cb_parse_framebuffer(void *ptr, struct sysinfo_t *info)
{
	/* ptr points to a coreboot table entry and is already virtual */
	info->framebuffer = ptr;
}
#endif

static void cb_parse_x86_rom_var_mtrr(void *ptr, struct sysinfo_t *info)
{
	struct cb_x86_rom_mtrr *rom_mtrr = ptr;
	info->x86_rom_var_mtrr_index = rom_mtrr->index;
}

static void cb_parse_string(unsigned char *ptr, char **info)
{
	*info = (char *)((struct cb_string *)ptr)->string;
}

static int cb_parse_header(void *addr, int len, struct sysinfo_t *info)
{
	struct cb_header *header;
	unsigned char *ptr = addr;
	void *forward;
	int i;

	for (i = 0; i < len; i += 16, ptr += 16) {
		header = (struct cb_header *)ptr;
		if (!strncmp((const char *)header->signature, "LBIO", 4))
			break;
	}

	/* We walked the entire space and didn't find anything. */
	if (i >= len)
		return -1;

	if (!header->table_bytes)
		return 0;

	/* Make sure the checksums match. */
	if (ipchksum((u16 *) header, sizeof(*header)) != 0)
		return -1;

	if (ipchksum((u16 *) (ptr + sizeof(*header)),
		     header->table_bytes) != header->table_checksum)
		return -1;

	info->header = header;

	/* Now, walk the tables. */
	ptr += header->header_bytes;

	for (i = 0; i < header->table_entries; i++) {
		struct cb_record *rec = (struct cb_record *)ptr;

		/* We only care about a few tags here (maybe more later). */
		switch (rec->tag) {
		case CB_TAG_FORWARD:
			forward = phys_to_virt((void *)(unsigned long)((struct cb_forward *)rec)->forward);
			return cb_parse_header(forward, len, info);
			continue;
		case CB_TAG_MEMORY:
			cb_parse_memory(ptr, info);
			break;
		case CB_TAG_SERIAL:
			cb_parse_serial(ptr, info);
			break;
		case CB_TAG_VERSION:
			cb_parse_string(ptr, &info->cb_version);
			break;
		case CB_TAG_EXTRA_VERSION:
			cb_parse_string(ptr, &info->extra_version);
			break;
		case CB_TAG_BUILD:
			cb_parse_string(ptr, &info->build);
			break;
		case CB_TAG_COMPILE_TIME:
			cb_parse_string(ptr, &info->compile_time);
			break;
		case CB_TAG_COMPILE_BY:
			cb_parse_string(ptr, &info->compile_by);
			break;
		case CB_TAG_COMPILE_HOST:
			cb_parse_string(ptr, &info->compile_host);
			break;
		case CB_TAG_COMPILE_DOMAIN:
			cb_parse_string(ptr, &info->compile_domain);
			break;
		case CB_TAG_COMPILER:
			cb_parse_string(ptr, &info->compiler);
			break;
		case CB_TAG_LINKER:
			cb_parse_string(ptr, &info->linker);
			break;
		case CB_TAG_ASSEMBLER:
			cb_parse_string(ptr, &info->assembler);
			break;
#ifdef CONFIG_NVRAM
		case CB_TAG_CMOS_OPTION_TABLE:
			cb_parse_optiontable(ptr, info);
			break;
		case CB_TAG_OPTION_CHECKSUM:
			cb_parse_checksum(ptr, info);
			break;
#endif
#ifdef CONFIG_COREBOOT_VIDEO_CONSOLE
		// FIXME we should warn on serial if coreboot set up a
		// framebuffer buf the payload does not know about it.
		case CB_TAG_FRAMEBUFFER:
			cb_parse_framebuffer(ptr, info);
			break;
#endif
		case CB_TAG_MAINBOARD:
			info->mainboard = (struct cb_mainboard *)ptr;
#ifdef CONFIG_CHROMEOS
		case CB_TAG_GPIO:
			cb_parse_gpios(ptr, info);
			break;
		case CB_TAG_VDAT:
			cb_parse_vdat(ptr, info);
			break;
		case CB_TAG_VBNV:
			cb_parse_vbnv(ptr, info);
			break;
		case CB_TAG_VBOOT_HANDOFF:
			cb_parse_vboot_handoff(ptr, info);
			break;
#endif
		case CB_TAG_TIMESTAMPS:
			cb_parse_tstamp(ptr, info);
			break;
		case CB_TAG_CBMEM_CONSOLE:
			cb_parse_cbmem_cons(ptr, info);
			break;
		case CB_TAG_MRC_CACHE:
			cb_parse_mrc_cache(ptr, info);
			break;
		case CB_TAG_X86_ROM_MTRR:
			cb_parse_x86_rom_var_mtrr(ptr, info);
			break;
		}

		ptr += rec->size;
	}

	return 1;
}

/* == Architecture specific == */
/* This is the x86 specific stuff. */

int get_coreboot_info(struct sysinfo_t *info)
{
	int ret;

	/* Ensure the variable range MTRR index covering the ROM is set to
	 * an invalid value. */
	info->x86_rom_var_mtrr_index = -1;

	ret = cb_parse_header(phys_to_virt(0x00000000), 0x1000, info);

	if (ret != 1)
		ret = cb_parse_header(phys_to_virt(0x000f0000), 0x1000, info);

	return (ret == 1) ? 0 : -1;
}
