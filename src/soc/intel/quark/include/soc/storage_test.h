/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __STORAGE_TEST_H__
#define __STORAGE_TEST_H__

#include <commonlib/sd_mmc_ctrlr.h>
#include <device/device.h>
#include <device/pci.h>
#include <timer.h>

#ifdef __SIMPLE_DEVICE__
uint32_t storage_test_init(pci_devfn_t dev, uint32_t *previous_bar,
	uint16_t *previous_command);
void storage_test(uint32_t bar, int full_initialization);
void storage_test_complete(pci_devfn_t dev, uint32_t previous_bar,
	uint16_t previous_command);
#else
uint32_t storage_test_init(struct device *dev, uint32_t *previous_bar,
	uint16_t *previous_command);
void storage_test(uint32_t bar, int full_initialization);
void storage_test_complete(struct device *dev, uint32_t previous_bar,
	uint16_t previous_command);
#endif

/* Logging support */
struct log_entry {
	struct mono_time time;
	struct mmc_command cmd;
	int cmd_issued;
	int ret;
	uint32_t response_entries;
	uint32_t response[4];
};

#define LOG_ENTRIES	256

#endif /* __STORAGE_TEST_H__ */
