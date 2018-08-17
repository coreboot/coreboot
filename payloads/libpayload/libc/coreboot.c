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
#include <stdint.h>

/*
 * The code in this file applies to all coreboot architectures. Some coreboot
 * table tags are architecture specific, they are handled by their respective
 * cb_parse_arch_specific() functions.
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

#if IS_ENABLED(CONFIG_LP_MEMMAP_RAM_ONLY)
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

static void cb_parse_vboot_handoff(unsigned char *ptr, struct sysinfo_t *info)
{
	struct lb_range *vbho = (struct lb_range *)ptr;

	info->vboot_handoff = (void *)(uintptr_t)vbho->range_start;
	info->vboot_handoff_size = vbho->range_size;
}

static void cb_parse_vbnv(unsigned char *ptr, struct sysinfo_t *info)
{
	struct lb_range *vbnv = (struct lb_range *)ptr;

	info->vbnv_start = vbnv->range_start;
	info->vbnv_size = vbnv->range_size;
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

static void cb_parse_mac_addresses(unsigned char *ptr,
				   struct sysinfo_t *info)
{
	struct cb_macs *macs = (struct cb_macs *)ptr;
	int i;

	info->num_macs = (macs->count < ARRAY_SIZE(info->macs)) ?
		macs->count : ARRAY_SIZE(info->macs);

	for (i = 0; i < info->num_macs; i++)
		info->macs[i] = macs->mac_addrs[i];
}

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

static void cb_parse_acpi_gnvs(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->acpi_gnvs = phys_to_virt(cbmem->cbmem_tab);
}

static void cb_parse_board_id(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_strapping_id *const cbbid = (struct cb_strapping_id *)ptr;
	info->board_id = cbbid->id_code;
}

static void cb_parse_ram_code(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_strapping_id *const ram_code = (struct cb_strapping_id *)ptr;
	info->ram_code = ram_code->id_code;
}

static void cb_parse_sku_id(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_strapping_id *const sku_id = (struct cb_strapping_id *)ptr;
	info->sku_id = sku_id->id_code;
}

#if IS_ENABLED(CONFIG_LP_NVRAM)
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

#if IS_ENABLED(CONFIG_LP_COREBOOT_VIDEO_CONSOLE)
static void cb_parse_framebuffer(void *ptr, struct sysinfo_t *info)
{
	/* ptr points to a coreboot table entry and is already virtual */
	info->framebuffer = ptr;
}
#endif

static void cb_parse_string(unsigned char *ptr, char **info)
{
	*info = (char *)((struct cb_string *)ptr)->string;
}

static void cb_parse_wifi_calibration(void *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->wifi_calibration = phys_to_virt(cbmem->cbmem_tab);
}

static void cb_parse_ramoops(void *ptr, struct sysinfo_t *info)
{
	struct lb_range *ramoops = (struct lb_range *)ptr;

	info->ramoops_buffer = ramoops->range_start;
	info->ramoops_buffer_size = ramoops->range_size;
}

static void cb_parse_mtc(void *ptr, struct sysinfo_t *info)
{
	struct lb_range *mtc = (struct lb_range *)ptr;

	info->mtc_start = mtc->range_start;
	info->mtc_size = mtc->range_size;
}

static void cb_parse_spi_flash(void *ptr, struct sysinfo_t *info)
{
	struct cb_spi_flash *flash = (struct cb_spi_flash *)ptr;

	info->spi_flash.size = flash->flash_size;
	info->spi_flash.sector_size = flash->sector_size;
	info->spi_flash.erase_cmd = flash->erase_cmd;
}

static void cb_parse_boot_media_params(unsigned char *ptr,
				       struct sysinfo_t *info)
{
	struct cb_boot_media_params *const bmp =
			(struct cb_boot_media_params *)ptr;
	info->fmap_offset = bmp->fmap_offset;
	info->cbfs_offset = bmp->cbfs_offset;
	info->cbfs_size = bmp->cbfs_size;
	info->boot_media_size = bmp->boot_media_size;
}

static void cb_parse_vpd(void *ptr, struct sysinfo_t *info)
{
	struct cb_cbmem_tab *const cbmem = (struct cb_cbmem_tab *)ptr;
	info->chromeos_vpd = phys_to_virt(cbmem->cbmem_tab);
}

#if IS_ENABLED(CONFIG_LP_TIMER_RDTSC)
static void cb_parse_tsc_info(void *ptr, struct sysinfo_t *info)
{
	const struct cb_tsc_info *tsc_info = ptr;

	if (tsc_info->freq_khz == 0)
		return;

	/* Honor the TSC frequency passed to the payload. */
	info->cpu_khz = tsc_info->freq_khz;
}
#endif

int cb_parse_header(void *addr, int len, struct sysinfo_t *info)
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

	/* Make sure the checksums match. */
	if (ipchksum((u16 *) header, sizeof(*header)) != 0)
		return -1;

	if (!header->table_bytes)
		return 0;

	if (ipchksum((u16 *) (ptr + sizeof(*header)),
		     header->table_bytes) != header->table_checksum)
		return -1;

	info->header = header;

	/* Initialize IDs as undefined in case they don't show up in table. */
	info->board_id = UNDEFINED_STRAPPING_ID;
	info->ram_code = UNDEFINED_STRAPPING_ID;
	info->sku_id = UNDEFINED_STRAPPING_ID;

	/* Now, walk the tables. */
	ptr += header->header_bytes;

	for (i = 0; i < header->table_entries; i++) {
		struct cb_record *rec = (struct cb_record *)ptr;

		/* We only care about a few tags here (maybe more later). */
		switch (rec->tag) {
		case CB_TAG_FORWARD:
			forward = phys_to_virt((void *)(unsigned long)
					       ((struct cb_forward *)rec)->forward);
			return cb_parse_header(forward, len, info);
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
#if IS_ENABLED(CONFIG_LP_NVRAM)
		case CB_TAG_CMOS_OPTION_TABLE:
			cb_parse_optiontable(ptr, info);
			break;
		case CB_TAG_OPTION_CHECKSUM:
			cb_parse_checksum(ptr, info);
			break;
#endif
#if IS_ENABLED(CONFIG_LP_COREBOOT_VIDEO_CONSOLE)
		// FIXME we should warn on serial if coreboot set up a
		// framebuffer buf the payload does not know about it.
		case CB_TAG_FRAMEBUFFER:
			cb_parse_framebuffer(ptr, info);
			break;
#endif
		case CB_TAG_MAINBOARD:
			info->mainboard = (struct cb_mainboard *)ptr;
			break;
		case CB_TAG_GPIO:
			cb_parse_gpios(ptr, info);
			break;
		case CB_TAG_VBNV:
			cb_parse_vbnv(ptr, info);
			break;
		case CB_TAG_VBOOT_HANDOFF:
			cb_parse_vboot_handoff(ptr, info);
			break;
		case CB_TAG_MAC_ADDRS:
			cb_parse_mac_addresses(ptr, info);
			break;
		case CB_TAG_SERIALNO:
			cb_parse_string(ptr, &info->serialno);
			break;
		case CB_TAG_TIMESTAMPS:
			cb_parse_tstamp(ptr, info);
			break;
		case CB_TAG_CBMEM_CONSOLE:
			cb_parse_cbmem_cons(ptr, info);
			break;
		case CB_TAG_ACPI_GNVS:
			cb_parse_acpi_gnvs(ptr, info);
			break;
		case CB_TAG_BOARD_ID:
			cb_parse_board_id(ptr, info);
			break;
		case CB_TAG_RAM_CODE:
			cb_parse_ram_code(ptr, info);
			break;
		case CB_TAG_SKU_ID:
			cb_parse_sku_id(ptr, info);
			break;
		case CB_TAG_WIFI_CALIBRATION:
			cb_parse_wifi_calibration(ptr, info);
			break;
		case CB_TAG_RAM_OOPS:
			cb_parse_ramoops(ptr, info);
			break;
		case CB_TAG_SPI_FLASH:
			cb_parse_spi_flash(ptr, info);
			break;
		case CB_TAG_MTC:
			cb_parse_mtc(ptr, info);
			break;
		case CB_TAG_BOOT_MEDIA_PARAMS:
			cb_parse_boot_media_params(ptr, info);
			break;
#if IS_ENABLED(CONFIG_LP_TIMER_RDTSC)
		case CB_TAG_TSC_INFO:
			cb_parse_tsc_info(ptr, info);
			break;
#endif
		case CB_TAG_VPD:
			cb_parse_vpd(ptr, info);
			break;
		default:
			cb_parse_arch_specific(rec, info);
			break;
		}

		ptr += rec->size;
	}

	return 0;
}
