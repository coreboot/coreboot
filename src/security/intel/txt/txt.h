/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_INTEL_TXT_H_
#define SECURITY_INTEL_TXT_H_

#include <types.h>

/* Error codes */
#define ACM_E_TYPE_NOT_MATCH                0x01
#define ACM_E_MODULE_SUB_TYPE_WRONG         0x02
#define ACM_E_MODULE_VENDOR_NOT_INTEL       0x03
#define ACM_E_SIZE_INCORRECT                0x04
#define ACM_E_CANT_CALL_GETSEC              0x05
#define ACM_E_NOT_FIT_INTO_CPU_ACM_MEM      0x06
#define ACM_E_NO_INFO_TABLE                 0x07
#define ACM_E_NOT_BIOS_ACM                  0x08
#define ACM_E_UUID_NOT_MATCH                0x09
#define ACM_E_PLATFORM_IS_NOT_PROD          0x10

void intel_txt_romstage_init(void);
void __noreturn txt_reset_platform(void);
void intel_txt_log_bios_acm_error(void);
int intel_txt_log_acm_error(const uint32_t acm_error);
void intel_txt_log_spad(void);
bool intel_txt_memory_has_secrets(void);
bool intel_txt_chipset_is_production_fused(void);
void intel_txt_run_sclean(void);
int intel_txt_run_bios_acm(const u8 input_params);
bool intel_txt_prepare_txt_env(void);
/* Allow platform override to skip TXT lockdown, e.g. required for RAS error injection. */
bool skip_intel_txt_lockdown(void);
const char *intel_txt_processor_error_type(uint8_t type);
void disable_intel_txt(void);

#endif /* SECURITY_INTEL_TXT_H_ */
