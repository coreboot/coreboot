/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _QUARK_SD_H_
#define _QUARK_SD_H_

#define SD_PFA       (0x14 << 11)        /* B0:D20:F0 - SDIO controller */
#define SD_CFG_BASE  (PCI_CFG | SD_PFA)  /* SD cntrl base in PCI config space */
#define SD_CFG_CMD   (SD_CFG_BASE+0x04)  /* Command reg in PCI config space */
#define SD_CFG_ADDR  (SD_CFG_BASE+0x10)  /* Base address in PCI config space */
#define SD_BASE_ADDR (0xA0018000)        /* SD controller's base address */
#define SD_HOST_CTRL (SD_BASE_ADDR+0x28) /* HOST_CTRL register */

#endif /* _QUARK_SD_H_ */
