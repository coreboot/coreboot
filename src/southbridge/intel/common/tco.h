/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_TCO_H
#define SOUTHBRIDGE_INTEL_COMMON_TCO_H

#if CONFIG(TCO_SPACE_NOT_YET_SPLIT)
/* Could get conflicting values. */
#undef TCO1_STS
#undef TCO2_STS
#undef TCO1_CNT
#endif

#include <soc/intel/common/tco.h>

#define PMBASE_TCO_OFFSET	0x60

#endif /* SOUTHBRIDGE_INTEL_COMMON_TCO_H */
