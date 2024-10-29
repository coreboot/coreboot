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

#if !defined(__SIMPLE_DEVICE__)
#define _PCU_DEV(bus, func) pcidev_path_on_bus(bus, PCI_DEVFN(PCU_DEV, func))
#else
#define _PCU_DEV(bus, func) PCI_DEV(bus, PCU_DEV, func)
#endif

/* PCU [B:31, D:30, F:0->F:6] */
#define PCU_IIO_STACK UNCORE_BUS_1
#define PCU_DEV 30

#define PCU_CR0_FUN 0
#define PCU_CR0_DEVID 0x3258
#define PCU_DEV_CR0(bus) _PCU_DEV(bus, PCU_CR0_FUN)
#define PCU_CR0_PLATFORM_INFO 0xa8
#define PCU_CR0_TURBO_ACTIVATION_RATIO 0xb0
#define TURBO_ACTIVATION_RATIO_LOCK BIT(31)
#define PCU_CR0_P_STATE_LIMITS 0xd8
#define P_STATE_LIMITS_LOCK BIT(31)
#define PCU_CR0_PACKAGE_RAPL_LIMIT_LWR 0xe8
#define PCU_CR0_PACKAGE_RAPL_LIMIT_UPR (PCU_CR0_PACKAGE_RAPL_LIMIT_LWR + 4)
#define PKG_PWR_LIM_LOCK_UPR BIT(31)
#define PCU_CR0_PMAX 0xf0
#define PMAX_LOCK BIT(31)
#define PCU_CR0_VR_CURRENT_CONFIG_CFG 0xf8
#define VR_CURRENT_CONFIG_LOCK BIT(31)

#define PCU_CR1_FUN 1
#define PCU_CR1_DEVID 0x3259
#define PCU_DEV_CR1(bus) _PCU_DEV(bus, PCU_CR1_FUN)
#define PCU_CR1_BIOS_MB_DATA_REG 0x8c

#define PCU_CR1_BIOS_MB_INTERFACE_REG 0x90
#define BIOS_MB_RUN_BUSY_MASK BIT(31)
#define BIOS_MB_CMD_MASK 0xff
#define BIOS_CMD_READ_PCU_MISC_CFG 0x5
#define BIOS_CMD_WRITE_PCU_MISC_CFG 0x6
#define BIOS_ERR_INVALID_CMD 0x01

#define PCU_CR1_BIOS_RESET_CPL_REG 0x94
#define RST_CPL1_MASK BIT(1)
#define RST_CPL2_MASK BIT(2)
#define RST_CPL3_MASK BIT(3)
#define RST_CPL4_MASK BIT(4)
#define PCODE_INIT_DONE1_MASK BIT(9)
#define PCODE_INIT_DONE2_MASK BIT(10)
#define PCODE_INIT_DONE3_MASK BIT(11)
#define PCODE_INIT_DONE4_MASK BIT(12)

#define PCU_CR1_DESIRED_CORES_CFG2_REG 0xbc
#define PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK BIT(31)

#define PCU_CR2_FUN 2
#define PCU_CR2_DEVID 0x325a
#define PCU_DEV_CR2(bus) _PCU_DEV(bus, PCU_CR2_FUN)
#define PCU_CR2_DRAM_POWER_INFO_LWR 0xa8
#define PCU_CR2_DRAM_POWER_INFO_UPR (PCU_CR2_DRAM_POWER_INFO_LWR + 4)
#define DRAM_POWER_INFO_LOCK_UPR BIT(31)

#define PCU_CR2_DRAM_PLANE_POWER_LIMIT_LWR 0xf0
#define PCU_CR2_DRAM_PLANE_POWER_LIMIT_UPR (PCU_CR2_DRAM_PLANE_POWER_LIMIT_LWR + 4)
#define PP_PWR_LIM_LOCK_UPR BIT(31)

#define PCU_CR3_FUN 3
#define PCU_CR3_DEVID 0x325b
#define PCU_CR3_CAPID4 0x94
#define ERR_SPOOFING_DIS 1
#define PCU_DEV_CR3(bus) _PCU_DEV(bus, PCU_CR3_FUN)
#define PCU_CR3_CONFIG_TDP_CONTROL 0xd8
#define TDP_LOCK BIT(31)
#define PCU_CR3_FLEX_RATIO 0xa0
#define OC_LOCK BIT(20)

#define PCU_CR4_FUN 4
#define PCU_CR4_DEVID 0x325c
#define PCU_VIRAL_CONTROL 0x84
#define PCU_FW_ERR_EN (1 << 10)
#define PCU_UC_ERR_EN (1 << 9)
#define PCU_HW_ERR_EN (1 << 8)
#define PCU_EMCA_MODE (1 << 2)

#define PCU_CR6_FUN 6
#define PCU_CR6_DEVID 0x325e
#define PCU_DEV_CR6(bus) _PCU_DEV(bus, PCU_CR6_FUN)
#define PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_LWR 0xa8
#define PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_UPR (PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_LWR + 4)
#define PLT_PWR_LIM_LOCK_UPR BIT(31)
#define PCU_CR6_PLATFORM_POWER_INFO_CFG_LWR 0xb0
#define PCU_CR6_PLATFORM_POWER_INFO_CFG_UPR (PCU_CR6_PLATFORM_POWER_INFO_CFG_LWR + 4)
#define PLT_PWR_INFO_LOCK_UPR BIT(31)

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
