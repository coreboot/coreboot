// SPDX-License-Identifier: BSD-3-Clause
/*
 * Libpayload NVMe device driver
 * Copyright (C) 2019 secunet Security Networks AG
 */

#ifndef _STORAGE_NVME_H
#define _STORAGE_NVME_H

#include "storage.h"

void nvme_initialize(struct pci_dev *dev);

#endif /* _STORAGE_NVME_H */
