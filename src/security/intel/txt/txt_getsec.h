/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_INTEL_TXT_GETSEC_H_
#define SECURITY_INTEL_TXT_GETSEC_H_

#include <types.h>

void enable_getsec_or_reset(void);

bool getsec_parameter(uint32_t *version_mask,
		      uint32_t *version_numbers_supported,
		      uint32_t *max_size_acm_area,
		      uint32_t *memory_type_mask,
		      uint32_t *senter_function_disable,
		      uint32_t *txt_feature_flags);

bool getsec_capabilities(uint32_t *eax);

void getsec_enteraccs(const uint32_t esi,
		      const uint32_t acm_base,
		      const uint32_t acm_size);

void getsec_sclean(const uint32_t acm_base,
		   const uint32_t acm_size);

#endif /* SECURITY_INTEL_TXT_REGISTER_H_ */
