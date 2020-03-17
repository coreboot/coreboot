/*
 * This file is part of the coreboot project.
 *
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

#ifndef _QUARK_SD_H_
#define _QUARK_SD_H_

#define SD_PFA       (0x14 << 11)        /* B0:D20:F0 - SDIO controller */
#define SD_CFG_BASE  (PCI_CFG | SD_PFA)  /* SD cntrl base in PCI config space */
#define SD_CFG_CMD   (SD_CFG_BASE+0x04)  /* Command reg in PCI config space */
#define SD_CFG_ADDR  (SD_CFG_BASE+0x10)  /* Base address in PCI config space */
#define SD_BASE_ADDR (0xA0018000)        /* SD controller's base address */
#define SD_HOST_CTRL (SD_BASE_ADDR+0x28) /* HOST_CTRL register */

#endif /* _QUARK_SD_H_ */
