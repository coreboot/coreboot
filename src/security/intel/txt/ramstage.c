/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/mmio.h>
#include <bootmem.h>
#include <bootstate.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <security/intel/cbnt/cbnt.h>
#include <types.h>

#include "txt.h"
#include "txt_platform.h"
#include "txt_register.h"
#include "txt_getsec.h"

/* FIXME: Seems to work only on some platforms */
static void log_ibb_measurements(void)
{
	const uint64_t mseg_size = read64((void *)TXT_MSEG_SIZE);
	uint64_t mseg_base = read64((void *)TXT_MSEG_BASE);

	if (!mseg_size || !mseg_base || mseg_size <= mseg_base)
		return;
	/*
	 * MSEG SIZE and MSEG BASE might contain random values.
	 * Assume below 4GiB and 8byte aligned.
	 */
	if (mseg_base & ~0xfffffff8ULL || mseg_size & ~0xfffffff8ULL)
		return;

	printk(BIOS_INFO, "TEE-TXT: IBB Hash 0x");
	for (; mseg_base < mseg_size; mseg_base++)
		printk(BIOS_INFO, "%02X", read8((void *)(uintptr_t)mseg_base));

	printk(BIOS_INFO, "\n");
}

void bootmem_platform_add_ranges(void)
{
	uint64_t status = read64((void *)TXT_SPAD);

	if (status & ACMSTS_TXT_DISABLED)
		return;

	/* Chapter 5.5.5 Intel TXT reserved memory */
	bootmem_add_range(TXT_RESERVED_SPACE,
			  TXT_RESERVED_SPACE_SIZE,
			  BM_MEM_RESERVED);

	/* Intel TPM decode memory */
	bootmem_add_range(TXT_TPM_DECODE_AREA,
			  TXT_RESERVED_SPACE - TXT_TPM_DECODE_AREA,
			  BM_MEM_RESERVED);

	/* Intel TXT public space memory */
	bootmem_add_range(TXT_PUBLIC_SPACE,
			  TXT_TPM_DECODE_AREA - TXT_PUBLIC_SPACE,
			  BM_MEM_RESERVED);

	/* Intel TXT private space memory */
	bootmem_add_range(TXT_PRIVATE_SPACE,
			  TXT_PUBLIC_SPACE - TXT_PRIVATE_SPACE,
			  BM_MEM_RESERVED);

	const union dpr_register dpr = {
		.raw = read32((void *)TXT_DPR),
	};

	const uint32_t dpr_base = dpr.top - dpr.size * MiB;

	/* Chapter 5.5.6 Intel TXT Device Memory */
	bootmem_add_range(dpr_base, dpr.size * MiB, BM_MEM_RESERVED);
}

static bool get_wake_error_status(void)
{
	const uint8_t error = read8((void *)TXT_ESTS);
	return !!(error & TXT_ESTS_WAKE_ERROR_STS);
}

static void check_secrets_txt(void *unused)
{
	uint64_t status = read64((void *)TXT_SPAD);

	if (status & ACMSTS_TXT_DISABLED)
		return;

	/*
	 * Check if secrets bit needs to be reset. Only platforms that support
	 * CONFIG(PLATFORM_HAS_DRAM_CLEAR) will be able to run this code.
	 * On some platforms FSP-M takes care of the DRAM clearing.
	 * Assume all memory really was cleared.
	 *
	 * TXT will issue a platform reset to come up sober.
	 */
	if (intel_txt_memory_has_secrets()) {
		printk(BIOS_INFO, "TEE-TXT: Wiping TEE...\n");
		intel_txt_run_bios_acm(ACMINPUT_CLEAR_SECRETS);

		/* Should never reach this point ... */
		intel_txt_log_acm_error(read32((void *)TXT_BIOSACM_ERRORCODE));
		die("Waiting for platform reset...\n");
	}
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, check_secrets_txt, NULL);

/**
 * Log TXT startup errors, check all bits for TXT, run BIOSACM using
 * GETSEC[ENTERACCS].
 *
 * If a "TXT reset" is detected or "memory had secrets" is set, then do nothing as
 * 1. Running ACMs will cause a TXT-RESET
 * 2. Memory will be scrubbed in BS_DEV_INIT
 * 3. TXT-RESET will be issued by code above later
 *
 */
static void init_intel_txt(void *unused)
{
	const uint64_t status = read64((void *)TXT_SPAD);

	if (status & ACMSTS_TXT_DISABLED)
		return;

	printk(BIOS_INFO, "TEE-TXT: Initializing TEE...\n");

	intel_txt_log_spad();

	if (CONFIG(INTEL_CBNT_LOGGING))
		intel_cbnt_log_registers();

	if (CONFIG(INTEL_TXT_LOGGING)) {
		intel_txt_log_bios_acm_error();
		txt_dump_chipset_info();
	}

	printk(BIOS_INFO, "TEE-TXT: Validate TEE...\n");

	if (intel_txt_prepare_txt_env()) {
		printk(BIOS_ERR, "TEE-TXT: Failed to prepare TXT environment\n");
		return;
	}

	/* Check for fatal ACM error and TXT reset */
	if (get_wake_error_status()) {
		/* Can't run ACMs with TXT_ESTS_WAKE_ERROR_STS set */
		printk(BIOS_ERR, "TEE-TXT: Fatal BIOS ACM error reported\n");
		return;
	}

	if (CONFIG(INTEL_TXT_TEST_BIOS_ACM_CALLING_CODE)) {
		printk(BIOS_INFO, "TEE-TXT: Testing BIOS ACM calling code...\n");

		/*
		 * Test BIOS ACM code.
		 * ACM should do nothing on reserved functions, and return an error code
		 * in TXT_BIOSACM_ERRORCODE. Tests showed that this is not true.
		 * Use special function "NOP" that does 'nothing'.
		 */
		if (intel_txt_run_bios_acm(ACMINPUT_NOP) < 0) {
			printk(BIOS_ERR,
				"TEE-TXT: Error calling BIOS ACM with NOP function.\n");
			return;
		}
	}

	if (status & (ACMSTS_BIOS_TRUSTED | ACMSTS_IBB_MEASURED)) {
		printk(BIOS_INFO, "TEE-TXT: Logging IBB measurements...\n");
		log_ibb_measurements();
	}

	int s3resume = acpi_is_wakeup_s3();
	if (!s3resume && !CONFIG(INTEL_CBNT_SUPPORT)) {
		printk(BIOS_INFO, "TEE-TXT: Scheck...\n");
		if (intel_txt_run_bios_acm(ACMINPUT_SCHECK) < 0) {
			printk(BIOS_ERR, "TEE-TXT: Error calling BIOS ACM.\n");
			return;
		}
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, init_intel_txt, NULL);

static void push_sinit_heap(u8 **heap_ptr, void *data, size_t data_length)
{
	/* Push size */
	const uint64_t tmp = data_length + 8;
	memcpy(*heap_ptr, &tmp, 8);
	*heap_ptr += 8;

	if (data_length) {
		/* Push data */
		memcpy(*heap_ptr, data, data_length);
		*heap_ptr += data_length;
	}
}

static void txt_heap_fill_common_bdr(struct txt_biosdataregion *bdr)
{
	/* TPM2.0 requires version 6 of BDT */
	bdr->version = CONFIG_INTEL_TXT_BDR_VERSION;

	bdr->no_logical_procs = dev_count_cpu();

	/* The following have been removed from BIOS Data Table in version 6 */
	size_t policy_len;
	void *policy_data = cbfs_map(CONFIG_INTEL_TXT_CBFS_BIOS_POLICY, &policy_len);
	if (policy_data) {
		/* Point to FIT Type 9 entry in flash */
		bdr->lcp_pd_base = (uintptr_t)policy_data;
		bdr->lcp_pd_size = (uint64_t)policy_len;
		cbfs_unmap(policy_data);
	} else {
		printk(BIOS_ERR, "TEE-TXT: Couldn't locate LCP PD Policy in CBFS.\n");
	}

	bdr->support_acpi_ppi = 0;
	bdr->platform_type = 0;
}

static void txt_heap_fill_bios_spec(struct txt_bios_spec_ver_element *spec)
{
	/* Fill in the version of the used TXT BIOS Specification */
	spec->header.type = HEAP_EXTDATA_TYPE_BIOS_SPEC_VER;
	spec->header.size = sizeof(*spec);
	spec->ver_major = 2;
	spec->ver_minor = 1;
	spec->ver_revision = 0;
}

static void txt_heap_push_bdr_for_two_acms(u8 **heap_struct)
{
	/*
	 * BIOS Data Format
	 * Chapter C.2
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	/* Structure format for two present ACMs */
	struct {
		struct txt_biosdataregion bdr;
		struct txt_bios_spec_ver_element spec;
		struct txt_heap_acm_element2 heap_acm;
		struct txt_extended_data_element_header end;
	} __packed data = {0};

	txt_heap_fill_common_bdr(&data.bdr);
	txt_heap_fill_bios_spec(&data.spec);

	void *sinit_base = (void *)(uintptr_t)read64((void *)TXT_SINIT_BASE);
	data.bdr.bios_sinit_size = cbfs_load(CONFIG_INTEL_TXT_CBFS_SINIT_ACM,
					     sinit_base,
					     read64((void *)TXT_SINIT_SIZE));

	/* Extended elements - ACM addresses */
	data.heap_acm.header.type = HEAP_EXTDATA_TYPE_ACM;
	data.heap_acm.num_acms = 2;
	data.heap_acm.acm_addrs[1] = (uintptr_t)sinit_base;

	printk(BIOS_INFO, "TEE-TXT: Placing SINIT ACM in memory.\n");
	if (CONFIG(INTEL_TXT_LOGGING))
		txt_dump_acm_info(sinit_base);

	data.heap_acm.acm_addrs[0] =
		(uintptr_t)cbfs_map(CONFIG_INTEL_TXT_CBFS_BIOS_ACM, NULL);

	data.heap_acm.header.size = sizeof(data.heap_acm);

	/* Extended elements - End marker */
	data.end.type = HEAP_EXTDATA_TYPE_END;
	data.end.size = sizeof(data.end);

	/* BiosData */
	push_sinit_heap(heap_struct, &data, sizeof(data));
}

static void txt_heap_push_bdr_for_one_acm(u8 **heap_struct)
{
	/*
	 * BIOS Data Format
	 * Chapter C.2
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	/* Structure format for one present ACM */
	struct {
		struct txt_biosdataregion bdr;
		struct txt_bios_spec_ver_element spec;
		struct txt_heap_acm_element1 heap_acm;
		struct txt_extended_data_element_header end;
	} __packed data = {0};

	txt_heap_fill_common_bdr(&data.bdr);
	txt_heap_fill_bios_spec(&data.spec);

	void *sinit_base = (void *)(uintptr_t)read64((void *)TXT_SINIT_BASE);
	/* Clear SINIT ACM memory */
	memset(sinit_base, 0, read64((void *)TXT_SINIT_SIZE));

	/* Extended elements - ACM addresses */
	data.heap_acm.header.type = HEAP_EXTDATA_TYPE_ACM;
	data.heap_acm.acm_addrs[0] =
		(uintptr_t)cbfs_map(CONFIG_INTEL_TXT_CBFS_BIOS_ACM, NULL);
	data.heap_acm.num_acms = 1;

	data.heap_acm.header.size = sizeof(data.heap_acm);

	/* Extended elements - End marker */
	data.end.type = HEAP_EXTDATA_TYPE_END;
	data.end.size = sizeof(data.end);

	/* BiosData */
	push_sinit_heap(heap_struct, &data, sizeof(data));
}

static void txt_initialize_heap(void)
{
	/* Fill TXT.HEAP.BASE with 4 subregions */
	u8 *heap_struct = (void *)((uintptr_t)read64((void *)TXT_HEAP_BASE));

	/*
	 * Since we may have either BIOS ACM or both BIOS and SINIT ACMs in
	 * CBFS, the size of txt_heap_acm_element will be different. We cannot
	 * always hardcode the number of ACM addresses for two ACMs. If we
	 * include BIOS ACM only, the BDR parsing will fail in TBoot due to
	 * invalid sizeof BDR. Check if SINIT ACM is present in CBFS and push
	 * properly formatted BDR region onto the TXT heap.
	 */
	if (cbfs_file_exists(CONFIG_INTEL_TXT_CBFS_SINIT_ACM))
		txt_heap_push_bdr_for_two_acms(&heap_struct);
	else
		txt_heap_push_bdr_for_one_acm(&heap_struct);

	/* OsMLEData */
	/* FIXME: Does firmware need to write this? */
	push_sinit_heap(&heap_struct, NULL, 0);

	/* OsSinitData */
	/* FIXME: Does firmware need to write this? */
	push_sinit_heap(&heap_struct, NULL, 0);

	/* SinitMLEData */
	/* FIXME: Does firmware need to write this? */
	push_sinit_heap(&heap_struct, NULL, 0);
}

__weak bool skip_intel_txt_lockdown(void)
{
	return false;
}

/**
 * Finalize the TXT device.
 *
 * - Lock TXT register.
 * - Protect TSEG using DMA protected regions.
 * - Setup TXT regions.
 * - Place SINIT ACM in TXT_SINIT memory segment.
 * - Fill TXT BIOSDATA region.
 */
static void lockdown_intel_txt(void *unused)
{
	if (skip_intel_txt_lockdown())
		return;

	const uint64_t status = read64((void *)TXT_SPAD);

	uint32_t txt_feature_flags = 0;
	uintptr_t tseg_base;
	size_t tseg_size;

	smm_region(&tseg_base, &tseg_size);

	if (status & ACMSTS_TXT_DISABLED)
		return;

	/*
	 * Document Number: 558294
	 * Chapter 5.4.3 Detection of Intel TXT Capability
	 */

	if (!getsec_parameter(NULL, NULL, NULL, NULL, NULL, &txt_feature_flags))
		return;

	/* LockConfig only exists on Intel TXT for Servers */
	if (txt_feature_flags & GETSEC_PARAMS_TXT_EXT_CRTM_SUPPORT) {
		printk(BIOS_INFO, "TEE-TXT: Locking TEE...\n");

		/* Lock TXT config, unlocks TXT_HEAP_BASE */
		if (intel_txt_run_bios_acm(ACMINPUT_LOCK_CONFIG) < 0) {
			printk(BIOS_ERR, "TEE-TXT: Failed to lock registers.\n");
			printk(BIOS_ERR, "TEE-TXT: SINIT won't be supported.\n");
			return;
		}
	}

	/*
	 * Document Number: 558294
	 * Chapter 5.5.6.1 DMA Protection Memory Region
	 */

	const u8 dpr_capable = !!(read64((void *)TXT_CAPABILITIES) &
				  TXT_CAPABILITIES_DPR);
	printk(BIOS_INFO, "TEE-TXT: DPR capable %x\n", dpr_capable);

	if (dpr_capable) {
		/* Verify the DPR settings on the MCH and mirror them to TXT public space */
		union dpr_register dpr = txt_get_chipset_dpr();

		printk(BIOS_DEBUG, "TEE-TXT: MCH DPR 0x%08x\n", dpr.raw);

		printk(BIOS_DEBUG, "TEE-TXT: MCH DPR base @ 0x%08x size %u MiB\n",
			(dpr.top - dpr.size) * MiB, dpr.size);

		// DPR TODO: implement SA_ENABLE_DPR in the intelblocks

		if (!dpr.lock) {
			printk(BIOS_ERR, "TEE-TXT: MCH DPR not locked.\n");
			return;
		}

		if (!dpr.epm || !dpr.prs) {
			printk(BIOS_ERR, "TEE-TXT: MCH DPR protection not active.\n");
			return;
		}

		_Static_assert(CONFIG_INTEL_TXT_HEAP_SIZE + CONFIG_INTEL_TXT_SINIT_SIZE
			       < CONFIG_INTEL_TXT_DPR_SIZE * MiB, "TXT Heap and Sinit must fit DPR");

		if (dpr.size < CONFIG_INTEL_TXT_DPR_SIZE) {
			printk(BIOS_ERR, "TEE-TXT: MCH DPR configured size is too small.\n");
			return;
		}

		if (dpr.top * MiB != tseg_base) {
			printk(BIOS_ERR, "TEE-TXT: MCH DPR top does not equal TSEG base.\n");
			return;
		}

		/* Clear reserved bits */
		dpr.prs = 0;
		dpr.epm = 0;

		write64((void *)TXT_DPR, dpr.raw);

		printk(BIOS_INFO, "TEE-TXT: TXT.DPR 0x%08x\n",
		       read32((void *)TXT_DPR));
	}

	/*
	 * Document Number: 558294
	 * Chapter 5.5.6.3 Intel TXT Heap Memory Region
	 */
	write64((void *)TXT_HEAP_SIZE, CONFIG_INTEL_TXT_HEAP_SIZE);
	write64((void *)TXT_HEAP_BASE,
		ALIGN_DOWN(tseg_base - read64((void *)TXT_HEAP_SIZE), 4096));

	/*
	 * Document Number: 558294
	 * Chapter 5.5.6.2 SINIT Memory Region
	 */
	write64((void *)TXT_SINIT_SIZE, CONFIG_INTEL_TXT_SINIT_SIZE);
	write64((void *)TXT_SINIT_BASE,
		ALIGN_DOWN(read64((void *)TXT_HEAP_BASE) -
			   read64((void *)TXT_SINIT_SIZE), 4096));

	/*
	 * FIXME: Server-TXT capable platforms need to install an STM in SMM and set up MSEG.
	 */

	/**
	 * Chapter 5.10.1 SMM in the Intel TXT for Servers Environment
	 * Disable MSEG.
	 */
	write64((void *)TXT_MSEG_SIZE, 0);
	write64((void *)TXT_MSEG_BASE, 0);

	/* Only initialize the heap on regular boots */
	if (!acpi_is_wakeup_s3())
		txt_initialize_heap();

	if (CONFIG(INTEL_TXT_LOGGING))
		txt_dump_regions();
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, lockdown_intel_txt, NULL);
