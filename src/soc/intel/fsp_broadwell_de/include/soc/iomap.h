/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

/*
 * Memory Mapped IO bases.
 */

/* PCI Configuration Space */
#define MCFG_BASE_ADDRESS        CONFIG_MMCONF_BASE_ADDRESS
#define MCFG_BASE_SIZE           0x10000000

/* Transactions in this range will abort */
#define ABORT_BASE_ADDRESS       0xfeb00000
#define ABORT_BASE_SIZE          0x00010000

/* PSEG */
#define PSEG_BASE_ADDRESS        0xfeb80000
#define PSEG_BASE_SIZE           0x00080000

/* IOxAPIC */
#define IOXAPIC1_BASE_ADDRESS    0xfec00000
#define IOXAPIC1_BASE_SIZE       0x00100000
#define IOXAPIC2_BASE_ADDRESS    0xfec01000
#define IOXAPIC2_BASE_SIZE       0x00100000

/* PCH (HPET/LT/TPM/Others) */
#define PCH_BASE_ADDRESS         0xfed00000
#define PCH_BASE_SIZE            0x00100000

/* Local XAPIC */
#define LXAPIC_BASE_ADDRESS      0xfee00000
#define LXAPIC_BASE_SIZE         0x00100000

/* High Performance Event Timer */
#define HPET_BASE_ADDRESS        0xfed00000
#define HPET_BASE_SIZE           0x400

/* Firmware */
#define FIRMWARE_BASE_ADDRESS    0xff000000
#define FIRMWARE_BASE_SIZE       0x01000000

/*
 * IO Port bases.
 */

/* ACPI Base Address */
#define ACPI_BASE_ADDRESS        0x400
#define ACPI_BASE_SIZE           0x80

/* GPIO Base Address */
#define GPIO_BASE_ADDRESS        0x500
#define GPIO_BASE_SIZE           0x80

#endif /* _SOC_IOMAP_H_ */
