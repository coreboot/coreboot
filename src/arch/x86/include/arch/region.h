/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_REGION_H__
#define __ARCH_REGION_H__

#include <commonlib/region.h>

/*
 * Preload region device initialization. This function initializes a preload region device
 * that encapsulates the original region device. The readat function call is replaced with
 * a preloading version that uses the prefetchnta instruction to preload data in the background,
 * when the read size is large enough and the thread is a preload thread.
 *
 * Architectual requirements:
 * - The SPIROM region must be memory mapped and marked as cachable.
 * - The CPU must support the prefetchnta instruction.
 * - The CPU must support the rdtsc instruction.
 *
 * The mmap and munmap functions are redireted to the original region device without doing
 * any preloading. The preload region device is read-only and does not support write or erase operations.
 *
 * @param orig_rdev The original region device to encapsulate.
 * @return A pointer to the initialized preload region device.
 */
const struct region_device *
mmio_region_device_preload_ro_init(const struct region_device *orig_rdev);

#endif /* __ARCH_REGION_H__ */
