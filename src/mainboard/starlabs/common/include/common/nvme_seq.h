/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_NVME_SEQ_H_
#define _STARLABS_CMN_NVME_SEQ_H_

#include <stddef.h>
#include <soc/gpio.h>

const struct pad_config *variant_nvme_power_sequence_pads(size_t *num);
const struct pad_config *variant_nvme_power_sequence_post_pads(size_t *num);

void variant_nvme_power_sequence_configure(void);
void variant_nvme_power_sequence_post_gpio_configure(void);

#endif /* _STARLABS_CMN_NVME_SEQ_H_ */
