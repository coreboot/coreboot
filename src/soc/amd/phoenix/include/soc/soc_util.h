/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PHOENIX_SOC_UTIL_H
#define AMD_PHOENIX_SOC_UTIL_H

enum soc_type {
	SOC_PHOENIX,
	SOC_PHOENIX2,
	SOC_UNKNOWN,
};

enum soc_type get_soc_type(void);

#endif /* AMD_PHOENIX_SOC_UTIL_H */
