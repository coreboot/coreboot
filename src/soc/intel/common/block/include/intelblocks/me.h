/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_COMMON_ME_SPEC_H_
#define _SOC_INTEL_COMMON_ME_SPEC_H_

#include <stdint.h>
#if CONFIG(SOC_INTEL_CSE_HAVE_SPEC_SUPPORT)

#if CONFIG_ME_SPEC == 12
#include "me_12.h"
#elif CONFIG_ME_SPEC == 13
#include "me_13.h"
#elif CONFIG_ME_SPEC == 15
#include "me_15.h"
#elif CONFIG_ME_SPEC == 16
#include "me_16.h"
#elif CONFIG_ME_SPEC == 18
#include "me_18.h"
#endif

#else
#include <soc/me.h>
#endif /* SOC_INTEL_CSE_HAVE_SPEC_SUPPORT */

#endif /* _SOC_INTEL_COMMON_ME_SPEC_H_ */
