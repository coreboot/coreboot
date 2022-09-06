/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _SOC_BL31_H_
#define _SOC_BL31_H_

#include <stdbool.h>

void register_reset_to_bl31(int gpio_index, bool active_high);

#endif  /* _SOC_BL31_H_ */
