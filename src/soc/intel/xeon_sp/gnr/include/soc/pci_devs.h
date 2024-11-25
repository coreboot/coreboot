/* SPDX-License-Identifier: GPL-2.0-only */

/* TEMPORARY PLACE HOLDER! DO NOT USE! */
/* FORKED FROM src/soc/intel/xeon_sp/spr/include/soc/pci_devs.h */

#ifndef _SOC_PCI_DEVS_H_
#define _SOC_PCI_DEVS_H_

#include <device/pci_def.h>
#include <device/pci_type.h>
#include <soc/pch_pci_devs.h>
#include <types.h>

#define _SA_DEVFN(slot) PCI_DEVFN(SA_DEV_SLOT_##slot, 0)

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _SA_DEV(slot) pcidev_path_on_root_debug(_SA_DEVFN(slot), __func__)
#else
#define _SA_DEV(slot) PCI_DEV(0, SA_DEV_SLOT_##slot, 0)
#endif

#define UNCORE_BUS_0 0
#define UNCORE_BUS_1 1

/* UBOX Registers [U(1), D:0, F:1] */
#define SMM_FEATURE_CONTROL 0x8c
#define SMM_CODE_CHK_EN BIT(2)
#define SMM_FEATURE_CONTROL_LOCK BIT(0)
#define UBOX_DFX_DEVID 0x3251

/* CHA registers [B:31, D:29, F:0/F:1]
 * SAD is the previous xeon_sp register name. Keep defines for shared code.
 */
#define CHA_DEV 29

#define SAD_ALL_DEV CHA_DEV
#define SAD_ALL_FUNC 0
#define SAD_ALL_PAM0123_CSR 0x80
#define   PAM_LOCK BIT(0)
#define SAD_ALL_PAM456_CSR 0x84
#define SAD_ALL_DEVID 0x344f

/* PCU [B:31, D:30, F:0] */
#define PCU_IIO_STACK 1
#define PCU_CR0_DEVID 0x3258
/* PCU [B:31, D:30, F:1] */
#define PCU_CR1_DEVID 0x3259
/* PCU [B:31, D:30, F:2] */
#define PCU_CR2_DEVID 0x325a
/* PCU [B:31, D:30, F:3] */
#define PCU_CR3_DEVID 0x325b
/* PCU [B:31, D:30, F:6] */
#define PCU_CR6_DEVID 0x325e

/* Memory Map/VTD Device Functions
 * These are available in each IIO stack
 */
#define MMAP_VTD_DEV 0x0
#define MMAP_VTD_FUNC 0x0

#define VTD_TOLM_CSR 0xd0
#define VTD_TSEG_BASE_CSR 0xa8
#define VTD_TSEG_LIMIT_CSR 0xac
#define VTD_EXT_CAP_LOW 0x10
#define VTD_MMCFG_BASE_CSR 0x90
#define VTD_MMCFG_LIMIT_CSR 0x98
#define VTD_TOHM_CSR 0xc8
#define VTD_MMIOL_CSR 0xd8
#define VTD_NCMEM_BASE_CSR 0xe0
#define VTD_NCMEM_LIMIT_CSR 0xe8
#define VTD_BAR_CSR 0x180
#define VTD_LTDPR 0x290

#define VMD_DEV_NUM 0x00
#define VMD_FUNC_NUM 0x05

#define MMAP_VTD_CFG_REG_DEVID 0x09a2
#define MMAP_VTD_STACK_CFG_REG_DEVID 0x09a2
#define VTD_DEV_NUM 0x0
#define VTD_FUNC_NUM 0x0

#if !defined(__SIMPLE_DEVICE__)
#define VTD_DEV(bus) pcidev_path_on_bus((bus), PCI_DEVFN(VTD_DEV_NUM, VTD_FUNC_NUM))
#else
#define VTD_DEV(bus) PCI_DEV((bus), VTD_DEV_NUM, VTD_FUNC_NUM)
#endif

/* Root port Registers */

/* IEH */
#define IEH_EXT_CAP_ID 0x7 /* At 0x160 */
#define GSYSEVTCTL 0x104   /* Offset from IEH_EXT_CAP_ID */
#define CE_ERR_UNMSK 1
#define NON_FATAL_UNMSK (1 << 1)
#define FATAL_UNMSK (1 << 2)
#define GSYSEVTMAP 0x108 /* Offset from IEH_EXT_CAP_ID */
#define CE_SMI 1
#define NF_SMI (1 << 2)
#define FA_SMI (1 << 4)


#define DMIRCBAR 0x50
#define DMI3_DEVID 0x2020
#define PCIE_ROOTCTL 0x5c
#define ERRINJCON 0x198

/* IIO DFX Global D7F7 registers */
#define IIO_DFX_TSWCTL0 0x30c
#define IIO_DFX_LCK_CTL 0x504

/* XHCI register */
#define SYS_BUS_CFG2 0x44

/* MSM registers */
#define MSM_BUS 0xF2
#define MSM_DEV 3
#define MSM_FUN 0
#define MSM_FUN_PMON 1
#define CRASHLOG_CTL 0x1B8
#define BIOS_CRASHLOG_CTL 0x158
#define CRASHLOG_CTL_DIS BIT(2)

#endif /* _SOC_PCI_DEVS_H_ */
