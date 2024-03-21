/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __VBOOT_SECDATA_TPM_PRIVATE_H__
#define __VBOOT_SECDATA_TPM_PRIVATE_H__

#include <console/console.h>
#include <security/tpm/tis.h>
#include <vb2_api.h>

#define VBDEBUG(format, args...) \
	printk(BIOS_INFO, "%s():%d: " format, __func__, __LINE__, ## args)

#define RETURN_ON_FAILURE(tpm_cmd) do {                                          \
		tpm_result_t rc_;                                                \
		if ((rc_ = (tpm_cmd)) != TPM_SUCCESS) {                          \
			VBDEBUG("Antirollback: %08x returned by " #tpm_cmd "\n", \
				(tpm_result_t)rc_);                              \
			return rc_;                                              \
		}                                                                \
	} while (0)

tpm_result_t safe_write(uint32_t index, const void *data, uint32_t length);

tpm_result_t factory_initialize_tpm1(struct vb2_context *ctx);

tpm_result_t factory_initialize_tpm2(struct vb2_context *ctx);

#endif /* __VBOOT_SECDATA_TPM_PRIVATE_H__ */
