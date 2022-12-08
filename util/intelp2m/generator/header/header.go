package header

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
)

const fileHeader string = `/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CFG_GPIO_H
#define CFG_GPIO_H

#include <gpio.h>

/* Pad configuration was generated automatically using intelp2m %s */
static const struct pad_config gpio_table[] = {`

const completion string = `};

#endif /* CFG_GPIO_H */
`

func Add(lines []string) []string {
	wrapper := make([]string, 0)
	wrapper = append(wrapper, fmt.Sprintf(fileHeader, p2m.Config.Version))
	wrapper = append(wrapper, lines...)
	wrapper = append(wrapper, completion)
	return wrapper
}
