/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include <fsp/soc_binding.h>

void variant_romstage_params(FSPM_UPD *);

const struct pad_config *variant_gpio_table(size_t *num);

#endif /* _VARIANT_H_ */
