/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SUPERIO_COMMON_SSDT_H__
#define __SUPERIO_COMMON_SSDT_H__

#include <device/device.h>

const char *superio_common_ldn_acpi_name(const struct device *dev);
void superio_common_fill_ssdt_generator(const struct device *dev);

#endif /* __SUPERIO_COMMON_SSDT_H__ */
