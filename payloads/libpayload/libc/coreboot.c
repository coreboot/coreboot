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
#include <commonlib/bsd/cbmem_id.h>
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

#if CONFIG(LP_MEMMAP_RAM_ONLY)
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
	info->cb_serial = virt_to_phys(ptr);
}

static void cb_parse_vbnv(unsigned char *ptr, struct sysinfo_t *info)
{
	struct lb_range *vbnv = (struct lb_range *)ptr;

	info->vbnv_start = vbnv->range_start;
	info->vbnv_size = vbnv->range_size;
}

static void cb_parse_mmc_info(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_mmc_info *mmc_info = (struct cb_mmc_info *)ptr;

	info->mmc_early_wake_status = mmc_info->early_cmd1_status;
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

static void cb_parse_board_config(unsigned char *ptr, struct sysinfo_t *info)
{
	struct cb_board_config *const config = (struct cb_board_config *)ptr;
	info->fw_config = cb_unpack64(config->fw_config);
	info->board_id = config->board_id;
	info->ram_code = config->ram_code;
	info->sku_id = config->sku_id;
}

#if CONFIG(LP_NVRAM)
static void cb_parse_optiontable(void *ptr, struct sysinfo_t *info)
{
	/* ptr is already virtual, but we want to keep physical addresses */
	info->cmos_option_table = virt_to_phys(ptr);
}

static void cb_parse_checksum(void *ptr, struct sysinfo_t *info)
{
	struct cb_cmos_checksum *cmos_cksum = ptr;
	info->cmos_range_start = cmos_cksum->range_start;
	info->cmos_range_end = cmos_cksum->range_end;
	info->cmos_checksum_location = cmos_cksum->location;
}
#endif

#if CONFIG(LP_COREBOOT_VIDEO_CONSOLE)
static void cb_parse_framebuffer(void *ptr, struct sysinfo_t *info)
{
	info->framebuffer = *(struct cb_framebuffer *)ptr;
}
#endif

static void cb_parse_string(const void *const ptr, uintptr_t *const info)
{
	/* ptr is already virtual (str->string just an offset to that),
	   but we want to keep physical addresses */
	const struct cb_string *const str = ptr;
	*info = virt_to_phys(str->string);
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

	if (flash->mmap_count == 0)
		return;

	info->spi_flash.mmap_window_count = MIN(flash->mmap_count, SYSINFO_MAX_MMAP_WINDOWS);
	memcpy(info->spi_flash.mmap_table, flash->mmap_table,
	       info->spi_flash.mmap_window_count * sizeof(struct flash_mmap_window));
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

#if CONFIG(LP_TIMER_RDTSC)
static void cb_parse_tsc_info(void *ptr, struct sysinfo_t *info)
{
	const struct cb_tsc_info *tsc_info = ptr;

	if (tsc_info->freq_khz == 0)
		return;

	/* Honor the TSC frequency passed to the payload. */
	info->cpu_khz = tsc_info->freq_khz;
}
#endif

static void cb_parse_cbmem_entry(void *ptr, struct sysinfo_t *info)
{
	const struct cb_cbmem_entry *cbmem_entry = ptr;

	if (cbmem_entry->size != sizeof(*cbmem_entry))
		return;

	switch (cbmem_entry->id) {
	case CBMEM_ID_ACPI_CNVS:
		info->acpi_cnvs = cbmem_entry->address;
		break;
	case CBMEM_ID_ACPI_GNVS:
		info->acpi_gnvs = cbmem_entry->address;
		break;
	case CBMEM_ID_CBFS_RO_MCACHE:
		info->cbfs_ro_mcache_offset = cbmem_entry->address;
		info->cbfs_ro_mcache_size = cbmem_entry->entry_size;
		break;
	case CBMEM_ID_CBFS_RW_MCACHE:
		info->cbfs_rw_mcache_offset = cbmem_entry->address;
		info->cbfs_rw_mcache_size = cbmem_entry->entry_size;
		break;
	case CBMEM_ID_CONSOLE:
		info->cbmem_cons = cbmem_entry->address;
		break;
	case CBMEM_ID_MRCDATA:
		info->mrc_cache = cbmem_entry->address;
		break;
	case CBMEM_ID_VBOOT_WORKBUF:
		info->vboot_workbuf = cbmem_entry->address;
		break;
	case CBMEM_ID_TIMESTAMP:
		info->tstamp_table = cbmem_entry->address;
		break;
	case CBMEM_ID_VPD:
		info->chromeos_vpd = cbmem_entry->address;
		break;
	case CBMEM_ID_FMAP:
		info->fmap_cache = cbmem_entry->address;
		break;
	case CBMEM_ID_WIFI_CALIBRATION:
		info->wifi_calibration = cbmem_entry->address;
		break;
	case CBMEM_ID_TYPE_C_INFO:
		info->type_c_info = cbmem_entry->address;
		break;
	case CBMEM_ID_MEM_CHIP_INFO:
		info->mem_chip_base = cbmem_entry->address;
		break;
	default:
		break;
	}
}

static void cb_parse_rsdp(void *ptr, struct sysinfo_t *info)
{
	const struct cb_acpi_rsdp *cb_acpi_rsdp = ptr;
	info->acpi_rsdp = cb_unpack64(cb_acpi_rsdp->rsdp_pointer);
}

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

	info->cb_header = virt_to_phys(header);

	/* Initialize IDs as undefined in case they don't show up in table. */
	info->board_id = UNDEFINED_STRAPPING_ID;
	info->ram_code = UNDEFINED_STRAPPING_ID;
	info->sku_id = UNDEFINED_STRAPPING_ID;
	info->fw_config = UNDEFINED_FW_CONFIG;

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
#if CONFIG(LP_NVRAM)
		case CB_TAG_CMOS_OPTION_TABLE:
			cb_parse_optiontable(ptr, info);
			break;
		case CB_TAG_OPTION_CHECKSUM:
			cb_parse_checksum(ptr, info);
			break;
#endif
#if CONFIG(LP_COREBOOT_VIDEO_CONSOLE)
		// FIXME we should warn on serial if coreboot set up a
		// framebuffer buf the payload does not know about it.
		case CB_TAG_FRAMEBUFFER:
			cb_parse_framebuffer(ptr, info);
			break;
#endif
		case CB_TAG_MAINBOARD:
			info->cb_mainboard = virt_to_phys(ptr);
			break;
		case CB_TAG_GPIO:
			cb_parse_gpios(ptr, info);
			break;
		case CB_TAG_VBNV:
			cb_parse_vbnv(ptr, info);
			break;
		case CB_TAG_MAC_ADDRS:
			cb_parse_mac_addresses(ptr, info);
			break;
		case CB_TAG_SERIALNO:
			cb_parse_string(ptr, &info->serialno);
			break;
		case CB_TAG_BOARD_CONFIG:
			cb_parse_board_config(ptr, info);
			break;
		case CB_TAG_RAM_OOPS:
			cb_parse_ramoops(ptr, info);
			break;
		case CB_TAG_SPI_FLASH:
			cb_parse_spi_flash(ptr, info);
			break;
		case CB_TAG_MMC_INFO:
			cb_parse_mmc_info(ptr, info);
			break;
		case CB_TAG_MTC:
			cb_parse_mtc(ptr, info);
			break;
		case CB_TAG_BOOT_MEDIA_PARAMS:
			cb_parse_boot_media_params(ptr, info);
			break;
		case CB_TAG_CBMEM_ENTRY:
			cb_parse_cbmem_entry(ptr, info);
			break;
#if CONFIG(LP_TIMER_RDTSC)
		case CB_TAG_TSC_INFO:
			cb_parse_tsc_info(ptr, info);
			break;
#endif
		case CB_TAG_ACPI_RSDP:
			cb_parse_rsdp(ptr, info);
			break;
		default:
			cb_parse_arch_specific(rec, info);
			break;
		}

		ptr += rec->size;
	}

	return 0;
}
