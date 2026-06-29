/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_GLINDA_SOC_UTIL_H
#define AMD_GLINDA_SOC_UTIL_H

enum soc_type {
	SOC_GLINDA,
	SOC_FAEGAN,
	SOC_UNKNOWN,
};

enum soc_type get_soc_type(void);

#endif /* AMD_GLINDA_SOC_UTIL_H */
