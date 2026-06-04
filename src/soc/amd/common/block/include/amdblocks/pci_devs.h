/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PCI_DEVS_H
#define AMD_BLOCK_PCI_DEVS_H


#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _SOC_DEV(slot, func)	pcidev_on_root(slot, func)
#else
#define _SOC_DEV(slot, func)	PCI_DEV(0, slot, func)
#endif

#endif /* AMD_BLOCK_PCI_DEVS_H */
