/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SA)
#include <intelblocks/systemagent.h>
#endif

#include <arch/mmio.h>
#include <string.h>

#include "txt.h"
#include "txt_register.h"

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

		printk(BIOS_ERR, " Type: ");

		switch (txt_error & TXT_ERROR_MASK) {
		case 0:
			printk(BIOS_ERR, "Legacy Shutdown\n");
			break;
		case 5:
			printk(BIOS_ERR, "Load memory type error in ACM area\n");
			break;
		case 6:
			printk(BIOS_ERR, "Unrecognized ACM format\n");
			break;
		case 7:
			printk(BIOS_ERR, "Failure to authenticate\n");
			break;
		case 8:
			printk(BIOS_ERR, "Invalid ACM format\n");
			break;
		case 9:
			printk(BIOS_ERR, "Unexpected Snoop hit\n");
			break;
		case 10:
			printk(BIOS_ERR, "Invalid event\n");
			break;
		case 11:
			printk(BIOS_ERR, "Invalid MLE\n");
			break;
		case 12:
			printk(BIOS_ERR, "Machine check event\n");
			break;
		case 13:
			printk(BIOS_ERR, "VMXAbort\n");
			break;
		case 14:
			printk(BIOS_ERR, "AC memory corruption\n");
			break;
		case 15:
			printk(BIOS_ERR, "Illegal voltage/bus ratio\n");
			break;
		default:
			printk(BIOS_ERR, "unknown\n");
			break;
		}
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
		intel_txt_log_acm_error(read32((void *)TXT_BIOSACM_ERRORCODE));
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
	       (read64((void *)TXT_VER_FSBIF) & TXT_VER_PRODUCTION_FUSED) ? "true" : "false");
}

void txt_dump_regions(void)
{
	struct txt_biosdataregion *bdr = NULL;
	uintptr_t tseg = 0;
	uint64_t reg64;

	reg64 = read64((void *)TXT_HEAP_BASE);
	if ((reg64 != 0 && reg64 != ~0UL) &&
	    (read64((void *)(uintptr_t)reg64) >= (sizeof(*bdr) + sizeof(uint64_t))))
		bdr = (void *)((uintptr_t)reg64 + sizeof(uint64_t));

	printk(BIOS_DEBUG, "TEE-TXT: TSEG 0x%lx\n", tseg * MiB);
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
