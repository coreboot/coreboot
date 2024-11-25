/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PCU_H_
#define _SOC_PCU_H_

#if !defined(__SIMPLE_DEVICE__)
#define _PCU_DEV(bus, func)		pcidev_path_on_bus(bus, PCI_DEVFN(PCU_DEV, func))
#else
#define _PCU_DEV(bus, func)		PCI_DEV(bus, PCU_DEV, func)
#endif

#define PCU_DEV                         30

#define PCU_CR0_FUN                     0
#define PCU_DEV_CR0(bus)                _PCU_DEV(bus, PCU_CR0_FUN)
#define PCU_CR0_PLATFORM_INFO           0xa8
#define PCU_CR0_TURBO_ACTIVATION_RATIO  0xb0
#define   TURBO_ACTIVATION_RATIO_LOCK   BIT(31)
#define PCU_CR0_P_STATE_LIMITS          0xd8
#define P_STATE_LIMITS_LOCK_SHIFT       31
#define P_STATE_LIMITS_LOCK             (1 << P_STATE_LIMITS_LOCK_SHIFT)
#define PCU_CR0_TEMPERATURE_TARGET      0xe4
#define PCU_CR0_PACKAGE_RAPL_LIMIT_LWR  0xe8
#define PCU_CR0_PACKAGE_RAPL_LIMIT_UPR   (PCU_CR0_PACKAGE_RAPL_LIMIT_LWR + 4)
#define   PKG_PWR_LIM_LOCK_UPR          BIT(31)
#define PCU_CR0_CURRENT_CONFIG          0xf8
#define MAX_NON_TURBO_LIM_RATIO_SHIFT   8 /* 8:15 */
#define MAX_NON_TURBO_LIM_RATIO_MASK    (0xff << MAX_NON_TURBO_LIM_RATIO_SHIFT)
#define PCU_CR0_PMAX                    0xf0
#define   PMAX_LOCK                     BIT(31)
#define PCU_CR0_VR_CURRENT_CONFIG_CFG   0xf8
#define VR_CURRENT_CONFIG_LOCK          BIT(31)

#define PCU_CR1_FUN                     1

#define PCU_CR1_C2C3TT_REG                                 0xdc
#define PCU_CR1_PCIE_ILTR_OVRD                             0xfc
#define PCU_CR1_SAPMCTL                                    0xb0

#define PCU_CR1_BIOS_MB_DATA_REG                           0x8c

#define PCU_CR1_BIOS_MB_INTERFACE_REG                      0x90
#define   BIOS_MB_RUN_BUSY_MASK                            BIT(31)
#define   BIOS_MB_CMD_MASK                                 0xff
#define   BIOS_CMD_READ_PCU_MISC_CFG                       0x5
#define   BIOS_CMD_WRITE_PCU_MISC_CFG                      0x6
#define   BIOS_ERR_INVALID_CMD                             0x01

#define PCU_CR1_BIOS_RESET_CPL_REG                         0x94
#define   RST_CPL1_MASK                                    BIT(1)
#define   RST_CPL2_MASK                                    BIT(2)
#define   RST_CPL3_MASK                                    BIT(3)
#define   RST_CPL4_MASK                                    BIT(4)
#define   PCODE_INIT_DONE1_MASK                            BIT(9)
#define   PCODE_INIT_DONE2_MASK                            BIT(10)
#define   PCODE_INIT_DONE3_MASK                            BIT(11)
#define   PCODE_INIT_DONE4_MASK                            BIT(12)

#define PCU_CR1_MC_BIOS_REQ                                0x98

#if (CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP))
#define PCU_CR1_DESIRED_CORES_CFG2_REG                     0xa0
#else
#define PCU_CR1_DESIRED_CORES_CFG2_REG                     0xbc
#endif
#define PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK           BIT(31)

#define PCU_CR1_SAPMCTL                                    0xb0
#define SAPMCTL_LOCK_MASK                                   BIT(31)

#define PCU_CR2_FUN                     2
#define PCU_DEV_CR2(bus)                                   _PCU_DEV(bus, PCU_CR2_FUN)
#define PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK                0x8c
#define PCIE_IN_PKGCSTATE_L1_MASK                          0xFFFFFF /* 23:0 bits */
#define PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK2               0x90
#define KTI_IN_PKGCSTATE_L1_MASK                           0x7 /* 2:0 bits */
#define PCU_CR2_DRAM_POWER_INFO_LWR                        0xa8
#define PCU_CR2_DRAM_POWER_INFO_UPR                        (PCU_CR2_DRAM_POWER_INFO_LWR + 4)
#define   DRAM_POWER_INFO_LOCK_UPR                         BIT(31)
#define PCU_CR2_DYNAMIC_PERF_POWER_CTL                     0xdc
#define UNCORE_PLIMIT_OVERRIDE_BIT                         20
#define UNOCRE_PLIMIT_OVERRIDE_SHIFT                       (1 << UNCORE_PLIMIT_OVERRIDE_BIT)
#define PCU_CR2_PROCHOT_RESPONSE_RATIO_REG                 0xb0
#define PROCHOT_RATIO                                      0xa /* bits 0:7 */
#define PCU_CR2_DRAM_PLANE_POWER_LIMIT_LWR                 0xf0
#define PP_PWR_LIM_LOCK                                    BIT(31)
#define PCU_CR2_DRAM_PLANE_POWER_LIMIT_UPR                 (PCU_CR2_DRAM_PLANE_POWER_LIMIT_LWR + 4)
#define PP_PWR_LIM_LOCK_UPR                                BIT(31)

#define PCU_CR3_FUN                     3
#define PCU_DEV_CR3(bus)                                   _PCU_DEV(bus, PCU_CR3_FUN)
#define PCU_CR3_CAPID4                                     0x94
#define ERR_SPOOFING_DIS                                   1
#if (CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP))
#define PCU_CR3_CONFIG_TDP_CONTROL                         0x60
#else
#define PCU_CR3_CONFIG_TDP_CONTROL                         0xd8
#endif
#define   TDP_LOCK                                         BIT(31)
#define PCU_CR3_FLEX_RATIO                                 0xa0
#define   OC_LOCK                                          BIT(20)

#define PCU_CR4_FUN 4
#define PCU_VIRAL_CONTROL                                  0x84
#define PCU_FW_ERR_EN                                      (1 << 10)
#define PCU_UC_ERR_EN                                      (1 << 9)
#define PCU_HW_ERR_EN                                      (1 << 8)
#define PCU_EMCA_MODE                                      (1 << 2)

#define PCU_CR6_FUN 6
#define PCU_DEV_CR6(bus) _PCU_DEV(bus, PCU_CR6_FUN)
#define PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_LWR                0xa8
#define PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_UPR                (PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_LWR + 4)
#define PLT_PWR_LIM_LOCK_UPR                               BIT(31)
#define PCU_CR6_PLATFORM_POWER_INFO_CFG_LWR                0xb0
#define PCU_CR6_PLATFORM_POWER_INFO_CFG_UPR                (PCU_CR6_PLATFORM_POWER_INFO_CFG_LWR + 4)
#define PLT_PWR_INFO_LOCK_UPR                              BIT(31)

#endif /* _SOC_PCU_H_ */
