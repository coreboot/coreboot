/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_TCO_H
#define SOUTHBRIDGE_INTEL_COMMON_TCO_H

#if CONFIG(TCO_SPACE_NOT_YET_SPLIT)
/* Could get conflicting values. */
#undef TCO1_STS
#undef TCO2_STS
#undef TCO1_CNT
#endif

#define PMBASE_TCO_OFFSET	0x60

#define TCO1_STS		0x04
#define  TCO1_TIMEOUT		(1 << 3)
#define TCO2_STS		0x06
#define  TCO2_STS_SECOND_TO	(1 << 1)
#define TCO1_CNT		0x08
#define  TCO_TMR_HLT		(1 << 11)

#endif /* SOUTHBRIDGE_INTEL_COMMON_TCO_H */
