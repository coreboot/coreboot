/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <cbfs.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <lib.h>
#include <smp/node.h>
#include <string.h>
#include <types.h>

#if CONFIG(SOC_INTEL_COMMON_BLOCK_SA)
#include <soc/intel/common/reset.h>
#else
#if CONFIG(SOUTHBRIDGE_INTEL_COMMON_ME)
#include <southbridge/intel/common/me.h>
#endif
#include <cf9_reset.h>
#endif

#include "txt.h"
#include "txt_register.h"
#include "txt_getsec.h"

/* Usual security practice: if an unexpected error happens, reboot */
void __noreturn txt_reset_platform(void)
{
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SA)
	global_reset();
#else
#if CONFIG(SOUTHBRIDGE_INTEL_COMMON_ME)
	set_global_reset(1);
#endif
	full_reset();
#endif
}

/**
 * Dump the ACM error status bits.
 *
 * @param  acm_error The status register to dump
 * @return -1 on error (register is not valid)
 *	  0 on error (Class > 0 and Major > 0)
 *	  1 on success (Class == 0 and Major == 0 and progress > 0)
 */
int intel_txt_log_acm_error(const uint32_t acm_error)
{
	if (!(acm_error & ACMERROR_TXT_VALID))
		return -1;

	const uint8_t type = (acm_error & ACMERROR_TXT_TYPE_CODE)
			      >> ACMERROR_TXT_TYPE_SHIFT;

	switch (type) {
	case ACMERROR_TXT_AC_MODULE_TYPE_BIOS:
		printk(BIOS_ERR, "BIOSACM");
		break;
	case ACMERROR_TXT_AC_MODULE_TYPE_SINIT:
		printk(BIOS_ERR, "SINIT");
		break;
	default:
		printk(BIOS_ERR, "ACM");
		break;
	}
	printk(BIOS_ERR, ": Error code valid\n");

	if (acm_error & ACMERROR_TXT_EXTERNAL)
		printk(BIOS_ERR, " Caused by: External\n");
	else
		printk(BIOS_ERR, " Caused by: Processor\n");

	const uint32_t class = (acm_error & ACMERROR_TXT_CLASS_CODE)
							>> ACMERROR_TXT_CLASS_SHIFT;
	const uint32_t major = (acm_error & ACMERROR_TXT_MAJOR_CODE)
							>> ACMERROR_TXT_MAJOR_SHIFT;
	const uint32_t minor = (acm_error & ACMERROR_TXT_MINOR_CODE)
							>> ACMERROR_TXT_MINOR_SHIFT;
	const uint32_t progress = (acm_error & ACMERROR_TXT_PROGRESS_CODE)
							>> ACMERROR_TXT_PROGRESS_SHIFT;

	if (!minor) {
		if (class == 0 && major == 0 && progress > 0) {
			printk(BIOS_ERR, " Execution successful\n");
			printk(BIOS_ERR, " Progress code 0x%x\n", progress);
		} else {
			printk(BIOS_ERR, " Error Class: %x\n", class);
			printk(BIOS_ERR, " Error: %x.%x\n", major, progress);
		}
	} else {
		printk(BIOS_ERR, " ACM didn't start\n");
		printk(BIOS_ERR, " Error Type: 0x%x\n", acm_error & 0xffffff);
		return -1;
	}

	return (acm_error & ACMERROR_TXT_EXTERNAL) && class == 0 && major == 0 && progress > 0;
}

void intel_txt_log_spad(void)
{
	const uint64_t acm_status = read64((void *)TXT_SPAD);

	printk(BIOS_INFO, "TXT-STS: ACM verification ");

	if (acm_status & ACMSTS_VERIFICATION_ERROR)
		printk(BIOS_INFO, "error\n");
	else
		printk(BIOS_INFO, "successful\n");

	printk(BIOS_INFO, "TXT-STS: IBB ");

	if (acm_status & ACMSTS_IBB_MEASURED)
		printk(BIOS_INFO, "measured\n");
	else
		printk(BIOS_INFO, "not measured\n");

	printk(BIOS_INFO, "TXT-STS: TXT is ");

	if (acm_status & ACMSTS_TXT_DISABLED)
		printk(BIOS_INFO, "disabled\n");
	else
		printk(BIOS_INFO, "not disabled\n");

	printk(BIOS_INFO, "TXT-STS: BIOS is ");

	if (acm_status & ACMSTS_BIOS_TRUSTED)
		printk(BIOS_INFO, "trusted\n");
	else
		printk(BIOS_INFO, "not trusted\n");
}

/* Returns true if secrets might be in memory */
bool intel_txt_memory_has_secrets(void)
{
	bool ret;
	if (!CONFIG(INTEL_TXT))
		return false;

	ret = (read8((void *)TXT_ESTS) & TXT_ESTS_WAKE_ERROR_STS) ||
	      (read64((void *)TXT_E2STS) & TXT_E2STS_SECRET_STS);

	if (ret)
		printk(BIOS_CRIT, "TXT-STS: Secrets in memory!\n");
	return ret;
}

bool intel_txt_chipset_is_production_fused(void)
{
	/*
	 * Certain chipsets report production fused information in either
	 * TXT.VER.FSBIF or TXT.VER.EMIF/TXT.VER.QPIIF.
	 * Chapter B.1.7 and B.1.9
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	uint32_t reg = read32((void *)TXT_VER_FSBIF);

	if (reg == 0 || reg == UINT32_MAX)
		reg = read32((void *)TXT_VER_QPIIF);

	return (reg & TXT_VER_PRODUCTION_FUSED) ? true : false;
}

static struct acm_info_table *find_info_table(const void *ptr)
{
	const struct acm_header_v0 *acm_header = (struct acm_header_v0 *)ptr;

	return (struct acm_info_table *)(ptr +
		(acm_header->header_len + acm_header->scratch_size) * sizeof(uint32_t));
}

/**
 * Validate that the provided ACM is usable on this platform.
 */
static int validate_acm(const void *ptr)
{
	const struct acm_header_v0 *acm_header = (struct acm_header_v0 *)ptr;
	uint32_t max_size_acm_area = 0;

	if (acm_header->module_type != CHIPSET_ACM)
		return ACM_E_TYPE_NOT_MATCH;

	/* Seems inconsistent across generations. */
	if (acm_header->module_sub_type != 0 && acm_header->module_sub_type != 1)
		return ACM_E_MODULE_SUB_TYPE_WRONG;

	if (acm_header->module_vendor != INTEL_ACM_VENDOR)
		return ACM_E_MODULE_VENDOR_NOT_INTEL;

	if (acm_header->size == 0)
		return ACM_E_SIZE_INCORRECT;

	if (((acm_header->header_len + acm_header->scratch_size) * sizeof(uint32_t) +
	    sizeof(struct acm_info_table)) > (acm_header->size & 0xffffff) * sizeof(uint32_t)) {
		return ACM_E_SIZE_INCORRECT;
	}

	if (!getsec_parameter(NULL, NULL, &max_size_acm_area, NULL, NULL, NULL))
		return ACM_E_CANT_CALL_GETSEC;

	/*
	 * Causes #GP if acm_header->size > processor internal authenticated
	 * code area capacity.
	 * SAFER MODE EXTENSIONS REFERENCE.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 */
	const size_t acm_len = 1UL << log2_ceil((acm_header->size & 0xffffff) << 2);
	if (max_size_acm_area < acm_len) {
		printk(BIOS_ERR, "TEE-TXT: BIOS ACM doesn't fit into AC execution region\n");
		return ACM_E_NOT_FIT_INTO_CPU_ACM_MEM;
	}

	struct acm_info_table *info = find_info_table(ptr);
	if (!info)
		return ACM_E_NO_INFO_TABLE;
	if (info->chipset_acm_type != BIOS)
		return ACM_E_NOT_BIOS_ACM;

	static const u8 acm_uuid[] = {
		0xaa, 0x3a, 0xc0, 0x7f, 0xa7, 0x46, 0xdb, 0x18,
		0x2e, 0xac, 0x69, 0x8f, 0x8d, 0x41, 0x7f, 0x5a,
	};
	if (memcmp(acm_uuid, info->uuid, sizeof(acm_uuid)) != 0)
		return ACM_E_UUID_NOT_MATCH;

	const bool production_acm = !(acm_header->flags & ACM_FORMAT_FLAGS_DEBUG);
	if (production_acm != intel_txt_chipset_is_production_fused())
		return ACM_E_PLATFORM_IS_NOT_PROD;

	return 0;
}

/*
 * Prepare to run the BIOS ACM: mmap it from the CBFS and verify that it
 * can be launched. Returns pointer to ACM on success, NULL on failure.
 */
static void *intel_txt_prepare_bios_acm(size_t *acm_len)
{
	void *acm_data = NULL;

	if (!acm_len)
		return NULL;

	acm_data = cbfs_map(CONFIG_INTEL_TXT_CBFS_BIOS_ACM, acm_len);
	if (!acm_data) {
		printk(BIOS_ERR, "TEE-TXT: Couldn't locate BIOS ACM in CBFS.\n");
		return NULL;
	}

	/*
	 * CPU enforces only 4KiB alignment.
	 * Chapter A.1.1
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	if (!IS_ALIGNED((uintptr_t)acm_data, 4096)) {
		printk(BIOS_ERR, "TEE-TXT: BIOS ACM isn't mapped at page boundary.\n");
		cbfs_unmap(acm_data);
		return NULL;
	}

	/*
	 * Causes #GP if not multiple of 64.
	 * SAFER MODE EXTENSIONS REFERENCE.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 */
	if (!IS_ALIGNED(*acm_len, 64)) {
		printk(BIOS_ERR, "TEE-TXT: BIOS ACM size isn't multiple of 64.\n");
		cbfs_unmap(acm_data);
		return NULL;
	}

	/*
	 * The ACM should be aligned to it's size, but that's not possible, as
	 * some ACMs are not power of two. Use the next power of two for verification.
	 */
	if (!IS_ALIGNED((uintptr_t)acm_data, (1UL << log2_ceil(*acm_len)))) {
		printk(BIOS_ERR, "TEE-TXT: BIOS ACM isn't aligned to its size.\n");
		cbfs_unmap(acm_data);
		return NULL;
	}

	/*
	 * When setting up the MTRRs to cache the BIOS ACM, one must cache less than
	 * a page (4 KiB) of unused memory after the BIOS ACM. On Haswell, failure
	 * to do so will cause a TXT reset with Class Code 5, Major Error Code 2.
	 */
	if (popcnt(ALIGN_UP(*acm_len, 4096)) > get_var_mtrr_count()) {
		printk(BIOS_ERR, "TEE-TXT: Not enough MTRRs to cache this BIOS ACM's size.\n");
		cbfs_unmap(acm_data);
		return NULL;
	}

	if (CONFIG(INTEL_TXT_LOGGING))
		txt_dump_acm_info(acm_data);

	const int ret = validate_acm(acm_data);
	if (ret < 0) {
		printk(BIOS_ERR, "TEE-TXT: Validation of ACM failed with: %d\n", ret);
		cbfs_unmap(acm_data);
		return NULL;
	}

	return acm_data;
}

#define MCU_BASE_ADDR	(TXT_BASE + 0x278)
#define BIOACM_ADDR	(TXT_BASE + 0x27c)
#define APINIT_ADDR	(TXT_BASE + 0x290)
#define SEMAPHORE	(TXT_BASE + 0x294)

/* Returns on failure, resets the computer on success */
void intel_txt_run_sclean(void)
{
	size_t acm_len;

	void *acm_data = intel_txt_prepare_bios_acm(&acm_len);

	if (!acm_data)
		return;

	/* FIXME: Do we need to program these two? */
	//write32((void *)MCU_BASE_ADDR, 0xffe1a990);
	//write32((void *)APINIT_ADDR, 0xfffffff0);

	write32((void *)BIOACM_ADDR, (uintptr_t)acm_data);
	write32((void *)SEMAPHORE, 0);

	/*
	 * The time SCLEAN will take depends on the installed RAM size.
	 * On Haswell with 8 GiB of DDR3, it takes five or ten minutes. (rough estimate)
	 */
	printk(BIOS_ALERT, "TEE-TXT: Invoking SCLEAN. This can take several minutes.\n");

	/*
	 * Invoke the BIOS ACM. If successful, the system will reset with memory unlocked.
	 */
	getsec_sclean((uintptr_t)acm_data, acm_len);

	/*
	 * However, if this function returns, the BIOS ACM could not be invoked. This is bad.
	 */
	printk(BIOS_CRIT, "TEE-TXT: getsec_sclean could not launch the BIOS ACM.\n");

	cbfs_unmap(acm_data);
}

/*
 * Test all bits for TXT execution.
 *
 * @return 0 on success
 */
int intel_txt_run_bios_acm(const u8 input_params)
{
	size_t acm_len;

	void *acm_data = intel_txt_prepare_bios_acm(&acm_len);

	if (!acm_data)
		return -1;

	/* Call into assembly which invokes the referenced ACM */
	getsec_enteraccs(input_params, (uintptr_t)acm_data, acm_len);

	cbfs_unmap(acm_data);

	const uint64_t acm_status = read64((void *)TXT_SPAD);
	if (acm_status & ACMERROR_TXT_VALID) {
		printk(BIOS_ERR, "TEE-TXT: FATAL ACM launch error !\n");
		/*
		 * WARNING !
		 * To clear TXT.BIOSACM.ERRORCODE you must issue a cold reboot!
		 */
		intel_txt_log_acm_error(read32((void *)TXT_BIOSACM_ERRORCODE));
		return -1;
	}

	return 0;
}

 /* Returns true if cond is not met */
static bool check_precondition(const int cond)
{
	printk(BIOS_DEBUG, "%s\n", cond ? "true" : "false");
	return !cond;
}

/*
 * Test all bits that are required for Intel TXT.
 * Enable SMX if available.
 *
 * @return 0 on success
 */
bool intel_txt_prepare_txt_env(void)
{
	bool failure = false;
	uint32_t txt_feature_flags = 0;

	unsigned int ecx = cpuid_ecx(1);

	printk(BIOS_DEBUG, "TEE-TXT: CPU supports SMX: ");
	failure |= check_precondition(ecx & CPUID_SMX);

	printk(BIOS_DEBUG, "TEE-TXT: CPU supports VMX: ");
	failure |= check_precondition(ecx & CPUID_VMX);

	msr_t msr = rdmsr(IA32_FEATURE_CONTROL);
	if (!(msr.lo & BIT(0))) {
		printk(BIOS_ERR, "TEE-TXT: IA32_FEATURE_CONTROL is not locked\n");
		txt_reset_platform();
	}

	printk(BIOS_DEBUG, "TEE-TXT: IA32_FEATURE_CONTROL\n");
	printk(BIOS_DEBUG, " VMXON in SMX enable: ");
	failure |= check_precondition(msr.lo & BIT(1));

	printk(BIOS_DEBUG, " VMXON outside SMX enable: ");
	failure |= check_precondition(msr.lo & FEATURE_ENABLE_VMX);

	printk(BIOS_DEBUG, " register is locked: ");
	failure |= check_precondition(msr.lo & BIT(0));

	/* IA32_FEATURE_CONTROL enables getsec instructions */
	printk(BIOS_DEBUG, " GETSEC (all instructions) is enabled: ");
	failure |= check_precondition((msr.lo & 0xff00) == 0xff00);

	/* Prevent crash and opt out early */
	if (failure)
		return true;

	uint32_t eax = 0;
	/*
	 * GetSec[CAPABILITIES]
	 * SAFER MODE EXTENSIONS REFERENCE.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 * Must check BIT0 of TXT chipset has been detected by CPU.
	 */
	if (!getsec_capabilities(&eax))
		return true;

	printk(BIOS_DEBUG, "TEE-TXT: GETSEC[CAPABILITIES] returned:\n");
	printk(BIOS_DEBUG, " TXT capable chipset:  %s\n", (eax & BIT(0)) ? "true" : "false");

	printk(BIOS_DEBUG, " ENTERACCS available:  %s\n", (eax & BIT(2)) ? "true" : "false");
	printk(BIOS_DEBUG, " EXITAC available:     %s\n", (eax & BIT(3)) ? "true" : "false");
	printk(BIOS_DEBUG, " SENTER available:     %s\n", (eax & BIT(4)) ? "true" : "false");
	printk(BIOS_DEBUG, " SEXIT available:      %s\n", (eax & BIT(5)) ? "true" : "false");
	printk(BIOS_DEBUG, " PARAMETERS available: %s\n", (eax & BIT(6)) ? "true" : "false");
	printk(BIOS_DEBUG, " SMCTRL available:     %s\n", (eax & BIT(7)) ? "true" : "false");
	printk(BIOS_DEBUG, " WAKEUP available:     %s\n", (eax & BIT(8)) ? "true" : "false");

	txt_dump_getsec_parameters();

	/*
	 * Causes #GP if function is not supported by getsec.
	 * SAFER MODE EXTENSIONS REFERENCE.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 * Order Number:  325383-060US
	 */
	if ((eax & 0x7d) != 0x7d)
		failure = true;

	const uint64_t status = read64((void *)TXT_SPAD);

	if (status & ACMSTS_TXT_DISABLED) {
		printk(BIOS_INFO, "TEE-TXT: TXT disabled by BIOS policy in FIT.\n");
		failure = true;
	}

	/*
	 * Only the BSP must call getsec[ENTERACCS].
	 * SAFER MODE EXTENSIONS REFERENCE.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 * Order Number:  325383-060US
	 */
	if (!boot_cpu()) {
		printk(BIOS_ERR, "TEE-TXT: BSP flag not set in APICBASE_MSR.\n");
		failure = true;
	}

	/*
	 * There must be no MCEs pending.
	 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
	 * Order Number:  325383-060US
	 */
	msr = rdmsr(IA32_MCG_STATUS);
	if (msr.lo & 0x4) {
		printk(BIOS_ERR, "TEE-TXT: IA32_MCG_STATUS.MCIP is set.\n");
		failure = true;
	}

	if (!getsec_parameter(NULL, NULL, NULL, NULL, NULL, &txt_feature_flags)) {
		return true;
	} else {
		printk(BIOS_DEBUG, "TEE-TXT: Machine Check Register: ");
		if (txt_feature_flags & GETSEC_PARAMS_TXT_EXT_MACHINE_CHECK)
			printk(BIOS_DEBUG, "preserved\n");
		else
			printk(BIOS_DEBUG, "must be clear\n");
	}

	if (!(txt_feature_flags & GETSEC_PARAMS_TXT_EXT_MACHINE_CHECK)) {
		/*
		* Make sure there are no uncorrectable MCE errors.
		* Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
		*/
		size_t max_mc_msr = mca_get_bank_count();
		for (size_t i = 0; i < max_mc_msr; i++) {
			msr = rdmsr(IA32_MC_STATUS(i));
			if (!(msr.hi & MCA_STATUS_HI_UC))
				continue;

			printk(BIOS_ERR, "TEE-TXT: IA32_MC%zd_STATUS.UC is set.\n", i);
			failure = true;
			break;
		}
	}

	/* Need to park all APs. */
	if (CONFIG(PARALLEL_MP_AP_WORK))
		mp_park_aps();

	return failure;
}
