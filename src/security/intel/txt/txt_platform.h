/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SECURITY_INTEL_TXT_PLATFORM_H__
#define __SECURITY_INTEL_TXT_PLATFORM_H__

#include <types.h>
#include "txt_register.h"

/* Prototypes to be defined in chipset code */
union dpr_register txt_get_chipset_dpr(void);

#endif /* __SECURITY_INTEL_TXT_PLATFORM_H__ */
