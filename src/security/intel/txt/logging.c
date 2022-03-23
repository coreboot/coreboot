/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <types.h>

#include "txt.h"
#include "txt_getsec.h"
#include "txt_register.h"

const char *intel_txt_processor_error_type(uint8_t type)
{
	static const char *const names[] = {
		[0]  = "Legacy Shutdown",
		[5]  = "Load memory type error in ACM area",
		[6]  = "Unrecognized ACM format",
		[7]  = "Failure to authenticate",
		[8]  = "Invalid ACM format",
		[9]  = "Unexpected Snoop hit",
		[10] = "Invalid event",
		[11] = "Invalid MLE",
		[12] = "Machine check event",
		[13] = "VMXAbort",
		[14] = "AC memory corruption",
		[15] = "Illegal voltage/bus ratio",
	};

	return type < ARRAY_SIZE(names) && names[type] ? names[type] : "Unknown";
}

/**
 * Logs microcode or SINIT ACM errors.
 * Does not log SBIOS ACM errors.
 */
static void log_txt_error(const char *phase)
{
	const uint64_t txt_error = read64((void *)TXT_ERROR);

	if (txt_error & ACMERROR_TXT_VALID) {
		printk(BIOS_ERR, "%s: Error occurred\n", phase);

		if (txt_error & ACMERROR_TXT_EXTERNAL)
			printk(BIOS_ERR, " Caused by: External\n");
		else
			printk(BIOS_ERR, " Caused by: Processor\n");

		printk(BIOS_ERR, " Type: %s\n",
		       intel_txt_processor_error_type(txt_error & TXT_ERROR_MASK));
	}
}

/**
 * Dump useful informaation about the BIOS ACM state.
 * Should run right after console_init() in romstage.
 * Resets the platform if TXT reset is active and MLE cannot be established.
 **/
void intel_txt_log_bios_acm_error(void)
{
	uint32_t bios_acm_error;
	uint64_t acm_status;
	uint64_t txt_error;

	printk(BIOS_INFO, "TEE-TXT: State of ACM and ucode update:\n");

	bios_acm_error = read32((void *)TXT_BIOSACM_ERRORCODE);
	acm_status = read64((void *)TXT_SPAD);
	txt_error = read64((void *)TXT_ERROR);

	/* Errors by BIOS ACM or FIT */
	if ((txt_error & ACMERROR_TXT_VALID) &&
	    (acm_status & ACMERROR_TXT_VALID)) {
		intel_txt_log_acm_error(bios_acm_error);
		log_txt_error("FIT MICROCODE");
	}
	/* Errors by SINIT */
	if ((txt_error & ACMERROR_TXT_VALID) &&
	    !(acm_status & ACMERROR_TXT_VALID)) {
		intel_txt_log_acm_error(txt_error);
		log_txt_error("SINIT");
	}

	/* Check for fatal ACM error and TXT reset */
	uint8_t error = read8((void *)TXT_ESTS);
	if (error & TXT_ESTS_TXT_RESET_STS) {
		printk(BIOS_CRIT, "TXT-STS: Intel TXT reset detected\n");
		intel_txt_log_acm_error(read32((void *)TXT_ERROR));
	}
}

/**
 * Dump information about the provided ACM.
 */
void txt_dump_acm_info(const struct acm_header_v0 *acm_header)
{
	const struct acm_info_table *info = NULL;
	if (!acm_header)
		return;

	printk(BIOS_INFO, "ACM @ %p\n", acm_header);

	const size_t acm_size = (acm_header->size & 0xffffff) << 2;
	const size_t info_off = (acm_header->header_len + acm_header->scratch_size) * 4;

	if (acm_size > (info_off + sizeof(struct acm_info_table)))
		info = (const struct acm_info_table *)
			((const unsigned char *)acm_header + info_off);

	printk(BIOS_INFO, " ACM:      Binary Info\n");
	if (acm_header->module_type == CHIPSET_ACM)
		printk(BIOS_INFO, " Type:     Chipset ACM\n");

	if (acm_header->module_sub_type == 0)
		printk(BIOS_INFO, " Subtype:  undefined\n");
	else if (acm_header->module_sub_type == 1)
		printk(BIOS_INFO, " Subtype:  Run at reset\n");

	printk(BIOS_INFO, " Header:   v%u.%u\n", acm_header->header_version[0],
	       acm_header->header_version[1]);

	printk(BIOS_INFO, " Chipset:  %x\n", acm_header->chipset_id);
	printk(BIOS_INFO, " Size:     %zu\n", acm_size);

	switch (acm_header->flags) {
	case ACM_FORMAT_FLAGS_PW:
		printk(BIOS_INFO, " Flags:    PW signed (Production Worthy)\n");
		break;
	case ACM_FORMAT_FLAGS_NPW:
		printk(BIOS_INFO, " Flags:    NPW signed (Non Production Worthy)\n");
		break;
	case ACM_FORMAT_FLAGS_DEBUG:
		printk(BIOS_INFO, " Flags:    Debug signed\n");
		break;
	}

	if (acm_header->module_vendor == INTEL_ACM_VENDOR)
		printk(BIOS_INFO, " Vendor:   Intel Corporation\n");

	printk(BIOS_INFO, " Date:     %x\n", acm_header->date);

	switch (acm_header->size) {
	case ACM_FORMAT_SIZE_64KB:
			printk(BIOS_INFO, " Size:     64KB\n");
			printk(BIOS_INFO, " CBnT:     no\n");
		break;
	case ACM_FORMAT_SIZE_128KB:
			printk(BIOS_INFO, " Size:     128KB\n");
			printk(BIOS_INFO, " CBnT:     no\n");
		break;
	case ACM_FORMAT_SIZE_256KB:
			printk(BIOS_INFO, " Size:     256KB\n");
			printk(BIOS_INFO, " CBnT:     yes\n");
		break;
	default:
			printk(BIOS_INFO, " Size:     0x%08x\n", acm_header->size);

		break;
	}

	printk(BIOS_INFO, " TXT SVN:  %u\n", acm_header->txt_svn);
	printk(BIOS_INFO, " SE SVN:   %u\n", acm_header->se_svn);

	if (!info)
		return;
	printk(BIOS_INFO, " Table info:\n");
	printk(BIOS_INFO, "  UUID: ");
	for (size_t i = 0; i < sizeof(info->uuid); i++)
		printk(BIOS_INFO, "%02X ", info->uuid[i]);
	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "  Chipset acm type: 0x%x\n", info->chipset_acm_type);
	printk(BIOS_INFO, "  Capabilities: 0x%x\n", info->capabilities);
}

/**
 * Dump information about the chipset's TXT capabilities.
 */
void txt_dump_chipset_info(void)
{
	printk(BIOS_INFO, "TEE-TXT: Chipset Key Hash 0x");
	for (int i = 0; i < TXT_ACM_KEY_HASH_LEN; i++) {
		printk(BIOS_INFO, "%llx", read64((void *)TXT_ACM_KEY_HASH +
		       (i * sizeof(uint64_t))));
	}
	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "TEE-TXT: DIDVID 0x%x\n", read32((void *)TXT_DIDVID));
	printk(BIOS_INFO, "TEE-TXT: production fused chipset: %s\n",
	       intel_txt_chipset_is_production_fused() ? "true" : "false");
}

void txt_dump_regions(void)
{
	struct txt_biosdataregion *bdr = NULL;

	uintptr_t tseg_base;
	size_t tseg_size;

	smm_region(&tseg_base, &tseg_size);

	uint64_t reg64;

	reg64 = read64((void *)TXT_HEAP_BASE);
	if ((reg64 != 0 && reg64 != ~0UL) &&
	    (read64((void *)(uintptr_t)reg64) >= (sizeof(*bdr) + sizeof(uint64_t))))
		bdr = (void *)((uintptr_t)reg64 + sizeof(uint64_t));

	printk(BIOS_DEBUG, "TEE-TXT: TSEG 0x%lx, size %zu MiB\n", tseg_base, tseg_size / MiB);
	printk(BIOS_DEBUG, "TEE-TXT: TXT.HEAP.BASE  0x%llx\n", read64((void *)TXT_HEAP_BASE));
	printk(BIOS_DEBUG, "TEE-TXT: TXT.HEAP.SIZE  0x%llx\n", read64((void *)TXT_HEAP_SIZE));
	printk(BIOS_DEBUG, "TEE-TXT: TXT.SINIT.BASE 0x%llx\n", read64((void *)TXT_SINIT_BASE));
	printk(BIOS_DEBUG, "TEE-TXT: TXT.SINIT.SIZE 0x%llx\n", read64((void *)TXT_SINIT_SIZE));
	printk(BIOS_DEBUG, "TEE-TXT: TXT.MSEG.BASE  0x%llx\n", read64((void *)TXT_MSEG_BASE));
	printk(BIOS_DEBUG, "TEE-TXT: TXT.MSEG.SIZE  0x%llx\n", read64((void *)TXT_MSEG_SIZE));

	if (bdr) {
		printk(BIOS_DEBUG, "TEE-TXT: BiosDataRegion.bios_sinit_size 0x%x\n",
		       bdr->bios_sinit_size);
		printk(BIOS_DEBUG, "TEE-TXT: BiosDataRegion.lcp_pd_size 0x%llx\n",
		       bdr->lcp_pd_size);
		printk(BIOS_DEBUG, "TEE-TXT: BiosDataRegion.lcp_pd_base 0x%llx\n",
		       bdr->lcp_pd_base);
	}
}

void txt_dump_getsec_parameters(void)
{
	uint32_t version_mask;
	uint32_t version_numbers_supported;
	uint32_t max_size_acm_area;
	uint32_t memory_type_mask;
	uint32_t senter_function_disable;
	uint32_t txt_feature_flags;

	if (!getsec_parameter(&version_mask, &version_numbers_supported,
			      &max_size_acm_area, &memory_type_mask,
			      &senter_function_disable, &txt_feature_flags)) {
		printk(BIOS_WARNING, "Could not obtain GETSEC parameters\n");
		return;
	}
	printk(BIOS_DEBUG, "TEE-TXT: GETSEC[PARAMETERS] returned:\n");
	printk(BIOS_DEBUG, " ACM Version comparison mask: %08x\n", version_mask);
	printk(BIOS_DEBUG, " ACM Version numbers supported: %08x\n",
		version_numbers_supported);
	printk(BIOS_DEBUG, " Max size of authenticated code execution area: %08x\n",
		max_size_acm_area);
	printk(BIOS_DEBUG, " External memory types supported during AC mode: %08x\n",
		memory_type_mask);
	printk(BIOS_DEBUG, " Selective SENTER functionality control: %02x\n",
		(senter_function_disable >> 8) & 0x7f);
	printk(BIOS_DEBUG, " Feature Extensions Flags: %08x\n", txt_feature_flags);
	printk(BIOS_DEBUG, "\tS-CRTM Capability rooted in: ");
	if (txt_feature_flags & GETSEC_PARAMS_TXT_EXT_CRTM_SUPPORT) {
		printk(BIOS_DEBUG, "processor\n");
	} else {
		printk(BIOS_DEBUG, "BIOS\n");
	}
	printk(BIOS_DEBUG, "\tMachine Check Register: ");
	if (txt_feature_flags & GETSEC_PARAMS_TXT_EXT_MACHINE_CHECK) {
		printk(BIOS_DEBUG, "preserved\n");
	} else {
		printk(BIOS_DEBUG, "must be clear\n");
	}
}
