/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __DDRPHY_MD32_REGS_H__
#define __DDRPHY_MD32_REGS_H__

#define Channel_A_DDRPHY_DPM_BASE_ADDRESS    0x10940000

#define DDRPHY_MD32_BASE_ADDRESS     Channel_A_DDRPHY_DPM_BASE_VIRTUAL

#define DDRPHY_MD32_REG_SSPM_CFGREG_SW_RSTN                    (DDRPHY_MD32_BASE_ADDRESS + 0x0000)
    #define SSPM_CFGREG_SW_RSTN_SW_RSTN                        Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_RESOURCE_CTL               (DDRPHY_MD32_BASE_ADDRESS + 0x0004)
    #define SSPM_CFGREG_RESOURCE_CTL_APSRC_REQ                 Fld(1, 0) //[0:0]
    #define SSPM_CFGREG_RESOURCE_CTL_APSRC_ACK                 Fld(1, 4) //[4:4]
    #define SSPM_CFGREG_RESOURCE_CTL_INFRA_REQ                 Fld(1, 8) //[8:8]
    #define SSPM_CFGREG_RESOURCE_CTL_INFRA_ACK                 Fld(1, 12) //[12:12]
    #define SSPM_CFGREG_RESOURCE_CTL_SRCLKENA_REQ              Fld(1, 16) //[16:16]
    #define SSPM_CFGREG_RESOURCE_CTL_SRCLKENA_ACK              Fld(1, 20) //[20:20]
    #define SSPM_CFGREG_RESOURCE_CTL_DCS_ULTRA_REQ             Fld(1, 24) //[24:24]
    #define SSPM_CFGREG_RESOURCE_CTL_DCS_ULTRA_REQ_ACK         Fld(1, 28) //[28:28]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SYS_RMAP                   (DDRPHY_MD32_BASE_ADDRESS + 0x0008)
    #define SSPM_CFGREG_SYS_RMAP_SYS_RMAP                      Fld(4, 0) //[3:0]
    #define SSPM_CFGREG_SYS_RMAP_DRAM_RMAP0                    Fld(6, 4) //[9:4]
    #define SSPM_CFGREG_SYS_RMAP_DRAM_RMAP1                    Fld(6, 10) //[15:10]
    #define SSPM_CFGREG_SYS_RMAP_DRAM_RMAP2                    Fld(6, 16) //[21:16]
    #define SSPM_CFGREG_SYS_RMAP_DRAM_RMAP3                    Fld(6, 22) //[27:22]
    #define SSPM_CFGREG_SYS_RMAP_BUS_ARB_POLICY                Fld(1, 28) //[28:28]
    #define SSPM_CFGREG_SYS_RMAP_H2H_POSTWRITE_DIS             Fld(1, 29) //[29:29]

#define DDRPHY_MD32_REG_SSPM_CFGREG_OCD_BYPASS                 (DDRPHY_MD32_BASE_ADDRESS + 0x000C)
    #define SSPM_CFGREG_OCD_BYPASS_OCD_BYPASS                  Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD2HOST_IPC                (DDRPHY_MD32_BASE_ADDRESS + 0x0010)
    #define SSPM_CFGREG_MD2HOST_IPC_MD2HOST_IPC                Fld(1, 0) //[0:0]
    #define SSPM_CFGREG_MD2HOST_IPC_MD2HOST_IPC1               Fld(1, 1) //[1:1]
    #define SSPM_CFGREG_MD2HOST_IPC_MD2HOST_IPC_INT            Fld(1, 8) //[8:8]
    #define SSPM_CFGREG_MD2HOST_IPC_MD2HOST_IPC_INT1           Fld(1, 9) //[9:9]
    #define SSPM_CFGREG_MD2HOST_IPC_WDT_INT                    Fld(1, 10) //[10:10]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD2SPM_IPC                 (DDRPHY_MD32_BASE_ADDRESS + 0x0014)
    #define SSPM_CFGREG_MD2SPM_IPC_MD2SPM_IPC                  Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_HOST2MD_IPC                (DDRPHY_MD32_BASE_ADDRESS + 0x0018)
    #define SSPM_CFGREG_HOST2MD_IPC_HOST2MD_IPC                Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SPM2MD_IPC                 (DDRPHY_MD32_BASE_ADDRESS + 0x001C)
    #define SSPM_CFGREG_SPM2MD_IPC_SPM2MD_IPC                  Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR0                       (DDRPHY_MD32_BASE_ADDRESS + 0x0020)
    #define SSPM_CFGREG_GPR0_GPR0                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR1                       (DDRPHY_MD32_BASE_ADDRESS + 0x0024)
    #define SSPM_CFGREG_GPR1_GPR1                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR2                       (DDRPHY_MD32_BASE_ADDRESS + 0x0028)
    #define SSPM_CFGREG_GPR2_GPR2                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR3                       (DDRPHY_MD32_BASE_ADDRESS + 0x002C)
    #define SSPM_CFGREG_GPR3_GPR3                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR4                       (DDRPHY_MD32_BASE_ADDRESS + 0x0030)
    #define SSPM_CFGREG_GPR4_GPR4                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR5                       (DDRPHY_MD32_BASE_ADDRESS + 0x0034)
    #define SSPM_CFGREG_GPR5_GPR5                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_DVFS_INFO                  (DDRPHY_MD32_BASE_ADDRESS + 0x0038)
    #define SSPM_CFGREG_DVFS_INFO_DVFS_INFO_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_TEMPERATURE                (DDRPHY_MD32_BASE_ADDRESS + 0x003C)
    #define SSPM_CFGREG_TEMPERATURE_TEMPERATURE                Fld(15, 0) //[14:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_WDT_CFG                    (DDRPHY_MD32_BASE_ADDRESS + 0x0040)
    #define SSPM_CFGREG_WDT_CFG_WDT_VAL                        Fld(20, 0) //[19:0]
    #define SSPM_CFGREG_WDT_CFG_WDT_EN                         Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_SSPM_CFGREG_WDT_KICK                   (DDRPHY_MD32_BASE_ADDRESS + 0x0044)
    #define SSPM_CFGREG_WDT_KICK_WDT_KICK                      Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SEMAPHORE                  (DDRPHY_MD32_BASE_ADDRESS + 0x0048)
    #define SSPM_CFGREG_SEMAPHORE_SEMA_0_M                     Fld(1, 0) //[0:0]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_0_H                     Fld(1, 1) //[1:1]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_1_M                     Fld(1, 2) //[2:2]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_1_H                     Fld(1, 3) //[3:3]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_2_M                     Fld(1, 4) //[4:4]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_2_H                     Fld(1, 5) //[5:5]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_3_M                     Fld(1, 6) //[6:6]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_3_H                     Fld(1, 7) //[7:7]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_4_M                     Fld(1, 8) //[8:8]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_4_H                     Fld(1, 9) //[9:9]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_5_M                     Fld(1, 10) //[10:10]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_5_H                     Fld(1, 11) //[11:11]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_6_M                     Fld(1, 12) //[12:12]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_6_H                     Fld(1, 13) //[13:13]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_7_M                     Fld(1, 14) //[14:14]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_7_H                     Fld(1, 15) //[15:15]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_8_M                     Fld(1, 16) //[16:16]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_8_H                     Fld(1, 17) //[17:17]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_9_M                     Fld(1, 18) //[18:18]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_9_H                     Fld(1, 19) //[19:19]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_10_M                    Fld(1, 20) //[20:20]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_10_H                    Fld(1, 21) //[21:21]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_11_M                    Fld(1, 22) //[22:22]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_11_H                    Fld(1, 23) //[23:23]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_12_M                    Fld(1, 24) //[24:24]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_12_H                    Fld(1, 25) //[25:25]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_13_M                    Fld(1, 26) //[26:26]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_13_H                    Fld(1, 27) //[27:27]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_14_M                    Fld(1, 28) //[28:28]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_14_H                    Fld(1, 29) //[29:29]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_15_M                    Fld(1, 30) //[30:30]
    #define SSPM_CFGREG_SEMAPHORE_SEMA_15_H                    Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_TBUF_WPTR             (DDRPHY_MD32_BASE_ADDRESS + 0x004C)
    #define SSPM_CFGREG_MD32_TBUF_WPTR_MON_TBUF_WPTR           Fld(4, 0) //[3:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_CCNT                  (DDRPHY_MD32_BASE_ADDRESS + 0x0050)
    #define SSPM_CFGREG_MD32_CCNT_MON_CCNT                     Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_PCNT0                 (DDRPHY_MD32_BASE_ADDRESS + 0x0054)
    #define SSPM_CFGREG_MD32_PCNT0_MON_PCNT0                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_PCNT1                 (DDRPHY_MD32_BASE_ADDRESS + 0x0058)
    #define SSPM_CFGREG_MD32_PCNT1_MON_PCNT1                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_PCNT2                 (DDRPHY_MD32_BASE_ADDRESS + 0x005C)
    #define SSPM_CFGREG_MD32_PCNT2_MON_PCNT2                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_CONTID                (DDRPHY_MD32_BASE_ADDRESS + 0x0060)
    #define SSPM_CFGREG_MD32_CONTID_MON_CONTID                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_PC                    (DDRPHY_MD32_BASE_ADDRESS + 0x0064)
    #define SSPM_CFGREG_MD32_PC_MON_PC                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_PC_MON                (DDRPHY_MD32_BASE_ADDRESS + 0x0068)
    #define SSPM_CFGREG_MD32_PC_MON_MON_PC                     Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_BUS_STATUS            (DDRPHY_MD32_BASE_ADDRESS + 0x006C)
    #define SSPM_CFGREG_MD32_BUS_STATUS_BUS_STATUS             Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_AHB_M0_ADDR           (DDRPHY_MD32_BASE_ADDRESS + 0x0070)
    #define SSPM_CFGREG_MD32_AHB_M0_ADDR_AHB_M0_ADDR           Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_AHB_M1_ADDR           (DDRPHY_MD32_BASE_ADDRESS + 0x0074)
    #define SSPM_CFGREG_MD32_AHB_M1_ADDR_AHB_M1_ADDR           Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_ONE_TIME_LOCK              (DDRPHY_MD32_BASE_ADDRESS + 0x0078)
    #define SSPM_CFGREG_ONE_TIME_LOCK_ONE_TIME_LOCK            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SECURE_CTRL                (DDRPHY_MD32_BASE_ADDRESS + 0x007C)
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_0              Fld(1, 0) //[0:0]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_1              Fld(1, 1) //[1:1]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_2              Fld(1, 2) //[2:2]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_3              Fld(1, 3) //[3:3]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_4              Fld(1, 4) //[4:4]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_5              Fld(1, 5) //[5:5]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_6              Fld(1, 6) //[6:6]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_7              Fld(1, 7) //[7:7]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_8              Fld(1, 8) //[8:8]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_9              Fld(1, 9) //[9:9]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_10             Fld(1, 10) //[10:10]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_11             Fld(1, 11) //[11:11]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_12             Fld(1, 12) //[12:12]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_13             Fld(1, 13) //[13:13]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_14             Fld(1, 14) //[14:14]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_24             Fld(1, 24) //[24:24]
    #define SSPM_CFGREG_SECURE_CTRL_SECURE_CTRL_28             Fld(1, 28) //[28:28]

#define DDRPHY_MD32_REG_SSPM_CFGREG_TCM_LOCK_CNT               (DDRPHY_MD32_BASE_ADDRESS + 0x0080)
    #define SSPM_CFGREG_TCM_LOCK_CNT_TCM_LOCK_CNT              Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SLPP_S_EN                  (DDRPHY_MD32_BASE_ADDRESS + 0x0088)
    #define SSPM_CFGREG_SLPP_S_EN_SLPP_EN                      Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_P2P_TOEN                   (DDRPHY_MD32_BASE_ADDRESS + 0x008C)
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_0                    Fld(1, 0) //[0:0]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_1                    Fld(1, 1) //[1:1]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_2                    Fld(1, 2) //[2:2]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_3                    Fld(1, 3) //[3:3]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_4                    Fld(1, 4) //[4:4]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_5                    Fld(1, 5) //[5:5]
    #define SSPM_CFGREG_P2P_TOEN_P2P_TOEN_6                    Fld(1, 6) //[6:6]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX0_IN_IRQ               (DDRPHY_MD32_BASE_ADDRESS + 0x00A0)
    #define SSPM_CFGREG_MBOX0_IN_IRQ_MBOX0_IN_IRQ              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX1_IN_IRQ               (DDRPHY_MD32_BASE_ADDRESS + 0x00A4)
    #define SSPM_CFGREG_MBOX1_IN_IRQ_MBOX1_IN_IRQ              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX2_IN_IRQ               (DDRPHY_MD32_BASE_ADDRESS + 0x00A8)
    #define SSPM_CFGREG_MBOX2_IN_IRQ_MBOX2_IN_IRQ              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX3_IN_IRQ               (DDRPHY_MD32_BASE_ADDRESS + 0x00AC)
    #define SSPM_CFGREG_MBOX3_IN_IRQ_MBOX3_IN_IRQ              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX4_IN_IRQ               (DDRPHY_MD32_BASE_ADDRESS + 0x00B0)
    #define SSPM_CFGREG_MBOX4_IN_IRQ_MBOX4_IN_IRQ              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX0_OUT_IRQ              (DDRPHY_MD32_BASE_ADDRESS + 0x00C0)
    #define SSPM_CFGREG_MBOX0_OUT_IRQ_MBOX0_OUT_IRQ            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX1_OUT_IRQ              (DDRPHY_MD32_BASE_ADDRESS + 0x00C4)
    #define SSPM_CFGREG_MBOX1_OUT_IRQ_MBOX1_OUT_IRQ            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX2_OUT_IRQ              (DDRPHY_MD32_BASE_ADDRESS + 0x00C8)
    #define SSPM_CFGREG_MBOX2_OUT_IRQ_MBOX2_OUT_IRQ            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX3_OUT_IRQ              (DDRPHY_MD32_BASE_ADDRESS + 0x00CC)
    #define SSPM_CFGREG_MBOX3_OUT_IRQ_MBOX3_OUT_IRQ            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX4_OUT_IRQ              (DDRPHY_MD32_BASE_ADDRESS + 0x00D0)
    #define SSPM_CFGREG_MBOX4_OUT_IRQ_MBOX4_OUT_IRQ            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_ACAO_INT_SET               (DDRPHY_MD32_BASE_ADDRESS + 0x00D8)
    #define SSPM_CFGREG_ACAO_INT_SET_ACAO_INT_SET              Fld(17, 0) //[16:0]
    #define SSPM_CFGREG_ACAO_INT_SET_RSV0                      Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_CFGREG_ACAO_INT_CLR               (DDRPHY_MD32_BASE_ADDRESS + 0x00DC)
    #define SSPM_CFGREG_ACAO_INT_CLR_ACAO_INT_CLR              Fld(17, 0) //[16:0]
    #define SSPM_CFGREG_ACAO_INT_CLR_RSV0                      Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX0_BASE                 (DDRPHY_MD32_BASE_ADDRESS + 0x00E0)
    #define SSPM_CFGREG_MBOX0_BASE_MBOX0_BASE                  Fld(13, 0) //[12:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX1_BASE                 (DDRPHY_MD32_BASE_ADDRESS + 0x00E4)
    #define SSPM_CFGREG_MBOX1_BASE_MBOX1_BASE                  Fld(13, 0) //[12:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX2_BASE                 (DDRPHY_MD32_BASE_ADDRESS + 0x00E8)
    #define SSPM_CFGREG_MBOX2_BASE_MBOX2_BASE                  Fld(13, 0) //[12:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX3_BASE                 (DDRPHY_MD32_BASE_ADDRESS + 0x00EC)
    #define SSPM_CFGREG_MBOX3_BASE_MBOX3_BASE                  Fld(13, 0) //[12:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX4_BASE                 (DDRPHY_MD32_BASE_ADDRESS + 0x00F0)
    #define SSPM_CFGREG_MBOX4_BASE_MBOX4_BASE                  Fld(13, 0) //[12:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBOX8_7B_BASE              (DDRPHY_MD32_BASE_ADDRESS + 0x00F4)
    #define SSPM_CFGREG_MBOX8_7B_BASE_MBOX5_7B_4               Fld(5, 0) //[4:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_DVFS_INFO_1                (DDRPHY_MD32_BASE_ADDRESS + 0x00F8)
    #define SSPM_CFGREG_DVFS_INFO_1_DVFS_INFO_1                Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_DVFS_INFO_2                (DDRPHY_MD32_BASE_ADDRESS + 0x00FC)
    #define SSPM_CFGREG_DVFS_INFO_2_DVFS_INFO_2                Fld(5, 0) //[4:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_RSV_RW_REG0                (DDRPHY_MD32_BASE_ADDRESS + 0x0100)
    #define SSPM_CFGREG_RSV_RW_REG0_GICNIRQOUT_IRQ             Fld(8, 0) //[7:0]
    #define SSPM_CFGREG_RSV_RW_REG0_STANDBYWFI_INQ             Fld(8, 8) //[15:8]
    #define SSPM_CFGREG_RSV_RW_REG0_SPM_WAKEUP_IRQ             Fld(1, 16) //[16:16]
    #define SSPM_CFGREG_RSV_RW_REG0_RSV0                       Fld(1, 17) //[17:17]
    #define SSPM_CFGREG_RSV_RW_REG0_RSV_RW_REG0                Fld(14, 18) //[31:18]

#define DDRPHY_MD32_REG_SSPM_CFGREG_RSV_RW_REG1                (DDRPHY_MD32_BASE_ADDRESS + 0x0104)
    #define SSPM_CFGREG_RSV_RW_REG1_RSV_RW_REG1                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_RSV_RO_REG0                (DDRPHY_MD32_BASE_ADDRESS + 0x0108)
    #define SSPM_CFGREG_RSV_RO_REG0_RSV_RO_REG0                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_RSV_RO_REG1                (DDRPHY_MD32_BASE_ADDRESS + 0x010C)
    #define SSPM_CFGREG_RSV_RO_REG1_RSV_RO_REG1                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR6                       (DDRPHY_MD32_BASE_ADDRESS + 0x0110)
    #define SSPM_CFGREG_GPR6_GPR6                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR7                       (DDRPHY_MD32_BASE_ADDRESS + 0x0114)
    #define SSPM_CFGREG_GPR7_GPR7                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR8                       (DDRPHY_MD32_BASE_ADDRESS + 0x0118)
    #define SSPM_CFGREG_GPR8_GPR8                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR9                       (DDRPHY_MD32_BASE_ADDRESS + 0x011C)
    #define SSPM_CFGREG_GPR9_GPR9                              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR10                      (DDRPHY_MD32_BASE_ADDRESS + 0x0120)
    #define SSPM_CFGREG_GPR10_GPR10                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR11                      (DDRPHY_MD32_BASE_ADDRESS + 0x0124)
    #define SSPM_CFGREG_GPR11_GPR11                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR12                      (DDRPHY_MD32_BASE_ADDRESS + 0x0128)
    #define SSPM_CFGREG_GPR12_GPR12                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR13                      (DDRPHY_MD32_BASE_ADDRESS + 0x012C)
    #define SSPM_CFGREG_GPR13_GPR13                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR14                      (DDRPHY_MD32_BASE_ADDRESS + 0x0130)
    #define SSPM_CFGREG_GPR14_GPR14                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_GPR15                      (DDRPHY_MD32_BASE_ADDRESS + 0x0134)
    #define SSPM_CFGREG_GPR15_GPR15                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_PC_DRAM_CHA                (DDRPHY_MD32_BASE_ADDRESS + 0x013C)
    #define SSPM_CFGREG_PC_DRAM_CHA_PC_DRAM_CHA                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_PC_DRAM_CHB                (DDRPHY_MD32_BASE_ADDRESS + 0x0140)
    #define SSPM_CFGREG_PC_DRAM_CHB_PC_DRAM_CHB                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_PC_DRAM_CHC                (DDRPHY_MD32_BASE_ADDRESS + 0x0144)
    #define SSPM_CFGREG_PC_DRAM_CHC_PC_DRAM_CHC                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_PC_DRAM_CHD                (DDRPHY_MD32_BASE_ADDRESS + 0x0148)
    #define SSPM_CFGREG_PC_DRAM_CHD_PC_DRAM_CHD                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_SP                    (DDRPHY_MD32_BASE_ADDRESS + 0x014C)
    #define SSPM_CFGREG_MD32_SP_MON_SP                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MD32_LR                    (DDRPHY_MD32_BASE_ADDRESS + 0x0150)
    #define SSPM_CFGREG_MD32_LR_MON_LR                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_BUS_CTRL0                  (DDRPHY_MD32_BASE_ADDRESS + 0x0168)
    #define SSPM_CFGREG_BUS_CTRL0_BUS_CTRL0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_BUS_CTRL1                  (DDRPHY_MD32_BASE_ADDRESS + 0x016C)
    #define SSPM_CFGREG_BUS_CTRL1_BUS_CTRL1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_BUS_CTRL2                  (DDRPHY_MD32_BASE_ADDRESS + 0x0170)
    #define SSPM_CFGREG_BUS_CTRL2_BUS_CTRL2                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_DR_APBP2P_CTRL             (DDRPHY_MD32_BASE_ADDRESS + 0x0174)
    #define SSPM_CFGREG_DR_APBP2P_CTRL_R_APB_BROADCAST_EN      Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_TBUF_MON_SEL               (DDRPHY_MD32_BASE_ADDRESS + 0x0178)
    #define SSPM_CFGREG_TBUF_MON_SEL_TBU_MON_SEL               Fld(4, 0) //[3:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_TBUFL                      (DDRPHY_MD32_BASE_ADDRESS + 0x017C)
    #define SSPM_CFGREG_TBUFL_TBUFL                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_TBUFH                      (DDRPHY_MD32_BASE_ADDRESS + 0x0180)
    #define SSPM_CFGREG_TBUFH_TBUFH                            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_MBIST_CFG                  (DDRPHY_MD32_BASE_ADDRESS + 0x0190)
    #define SSPM_CFGREG_MBIST_CFG_DM_MD32_MBIST_CFG            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CFGREG_SRAM_DELSEL                (DDRPHY_MD32_BASE_ADDRESS + 0x0194)
    #define SSPM_CFGREG_SRAM_DELSEL_DM_MD32_SRAM_DELSEL_0      Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER0_CON                        (DDRPHY_MD32_BASE_ADDRESS + 0x1000)
    #define SSPM_TIMER0_CON_TIMER0_EN                          Fld(1, 0) //[0:0]
    #define SSPM_TIMER0_CON_TIMER0_RTC                         Fld(2, 4) //[5:4]

#define DDRPHY_MD32_REG_SSPM_TIMER0_RESET_VAL                  (DDRPHY_MD32_BASE_ADDRESS + 0x1004)
    #define SSPM_TIMER0_RESET_VAL_TIMER0_RST_VAL               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER0_CUR_VAL                    (DDRPHY_MD32_BASE_ADDRESS + 0x1008)
    #define SSPM_TIMER0_CUR_VAL_TIMER0_CUR_VAL                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER0_IRQ_ACK                    (DDRPHY_MD32_BASE_ADDRESS + 0x100C)
    #define SSPM_TIMER0_IRQ_ACK_TIMER0_IRQ_EN                  Fld(1, 0) //[0:0]
    #define SSPM_TIMER0_IRQ_ACK_TIMER0_IRQ_STATUS              Fld(1, 4) //[4:4]
    #define SSPM_TIMER0_IRQ_ACK_TIMER0_IRQ_CLR                 Fld(1, 5) //[5:5]

#define DDRPHY_MD32_REG_SSPM_TIMER1_CON                        (DDRPHY_MD32_BASE_ADDRESS + 0x1010)
    #define SSPM_TIMER1_CON_TIMER1_EN                          Fld(1, 0) //[0:0]
    #define SSPM_TIMER1_CON_TIMER1_RTC                         Fld(2, 4) //[5:4]

#define DDRPHY_MD32_REG_SSPM_TIMER1_RESET_VAL                  (DDRPHY_MD32_BASE_ADDRESS + 0x1014)
    #define SSPM_TIMER1_RESET_VAL_TIMER1_RST_VAL               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER1_CUR_VAL                    (DDRPHY_MD32_BASE_ADDRESS + 0x1018)
    #define SSPM_TIMER1_CUR_VAL_TIMER1_CUR_VAL                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER1_IRQ_ACK                    (DDRPHY_MD32_BASE_ADDRESS + 0x101C)
    #define SSPM_TIMER1_IRQ_ACK_TIMER1_IRQ_EN                  Fld(1, 0) //[0:0]
    #define SSPM_TIMER1_IRQ_ACK_TIMER1_IRQ_STATUS              Fld(1, 4) //[4:4]
    #define SSPM_TIMER1_IRQ_ACK_TIMER2_IRQ_CLR                 Fld(1, 5) //[5:5]

#define DDRPHY_MD32_REG_SSPM_TIMER2_CON                        (DDRPHY_MD32_BASE_ADDRESS + 0x1020)
    #define SSPM_TIMER2_CON_TIMER2_EN                          Fld(1, 0) //[0:0]
    #define SSPM_TIMER2_CON_TIMER2_RTC                         Fld(2, 4) //[5:4]

#define DDRPHY_MD32_REG_SSPM_TIMER2_RESET_VAL                  (DDRPHY_MD32_BASE_ADDRESS + 0x1024)
    #define SSPM_TIMER2_RESET_VAL_TIMER2_RST_VAL               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER2_CUR_VAL                    (DDRPHY_MD32_BASE_ADDRESS + 0x1028)
    #define SSPM_TIMER2_CUR_VAL_TIMER2_CUR_VAL                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER2_IRQ_ACK                    (DDRPHY_MD32_BASE_ADDRESS + 0x102C)
    #define SSPM_TIMER2_IRQ_ACK_TIMER2_IRQ_EN                  Fld(1, 0) //[0:0]
    #define SSPM_TIMER2_IRQ_ACK_TIMER2_IRQ_STATUS              Fld(1, 4) //[4:4]
    #define SSPM_TIMER2_IRQ_ACK_TIMER2_IRQ_CLR                 Fld(1, 5) //[5:5]

#define DDRPHY_MD32_REG_SSPM_TIMER3_CON                        (DDRPHY_MD32_BASE_ADDRESS + 0x1030)
    #define SSPM_TIMER3_CON_TIMER3_EN                          Fld(1, 0) //[0:0]
    #define SSPM_TIMER3_CON_TIMER3_RTC                         Fld(2, 4) //[5:4]

#define DDRPHY_MD32_REG_SSPM_TIMER3_RESET_VAL                  (DDRPHY_MD32_BASE_ADDRESS + 0x1034)
    #define SSPM_TIMER3_RESET_VAL_TIMER3_RST_VAL               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER3_CUR_VAL                    (DDRPHY_MD32_BASE_ADDRESS + 0x1038)
    #define SSPM_TIMER3_CUR_VAL_TIMER3_CUR_VAL                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TIMER3_IRQ_ACK                    (DDRPHY_MD32_BASE_ADDRESS + 0x103C)
    #define SSPM_TIMER3_IRQ_ACK_TIMER3_IRQ_EN                  Fld(1, 0) //[0:0]
    #define SSPM_TIMER3_IRQ_ACK_TIMER3_IRQ_STATUS              Fld(1, 4) //[4:4]
    #define SSPM_TIMER3_IRQ_ACK_TIMER3_IRQ_CLR                 Fld(1, 5) //[5:5]

#define DDRPHY_MD32_REG_SSPM_OS_TIMER_CON                      (DDRPHY_MD32_BASE_ADDRESS + 0x1080)
    #define SSPM_OS_TIMER_CON_OS_TIMER_EN                      Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_OS_TIMER_CNT_L                    (DDRPHY_MD32_BASE_ADDRESS + 0x108C)
    #define SSPM_OS_TIMER_CNT_L_OS_TIMER_CNT_L                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_OS_TIMER_CNT_H                    (DDRPHY_MD32_BASE_ADDRESS + 0x1090)
    #define SSPM_OS_TIMER_CNT_H_OS_TIMER_CNT_H                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_OS_TIMER_TVAL                     (DDRPHY_MD32_BASE_ADDRESS + 0x1094)
    #define SSPM_OS_TIMER_TVAL_OS_TIMER_TVAL                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_OS_TIMER_IRQ_ACK                  (DDRPHY_MD32_BASE_ADDRESS + 0x1098)
    #define SSPM_OS_TIMER_IRQ_ACK_OS_TIMER_IRQ_EN              Fld(1, 0) //[0:0]
    #define SSPM_OS_TIMER_IRQ_ACK_OS_TIMER_IRQ_STATUS          Fld(1, 4) //[4:4]
    #define SSPM_OS_TIMER_IRQ_ACK_OS_TIMER_IRQ_CLR             Fld(1, 5) //[5:5]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_RAW_STA0                 (DDRPHY_MD32_BASE_ADDRESS + 0x2000)
    #define SSPM_INTC_IRQ_RAW_STA0_IRQ_RAW_STA0                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_RAW_STA1                 (DDRPHY_MD32_BASE_ADDRESS + 0x2004)
    #define SSPM_INTC_IRQ_RAW_STA1_IRQ_RAW_STA1                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_STA0                     (DDRPHY_MD32_BASE_ADDRESS + 0x2010)
    #define SSPM_INTC_IRQ_STA0_IRQ_STA0                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_STA1                     (DDRPHY_MD32_BASE_ADDRESS + 0x2014)
    #define SSPM_INTC_IRQ_STA1_IRQ_STA1                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_EN0                      (DDRPHY_MD32_BASE_ADDRESS + 0x2020)
    #define SSPM_INTC_IRQ_EN0_IRQ_EN0                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_EN1                      (DDRPHY_MD32_BASE_ADDRESS + 0x2024)
    #define SSPM_INTC_IRQ_EN1_IRQ_EN1                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_WAKE_EN0                 (DDRPHY_MD32_BASE_ADDRESS + 0x2030)
    #define SSPM_INTC_IRQ_WAKE_EN0_IRQ_WAKE_EN0                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_WAKE_EN1                 (DDRPHY_MD32_BASE_ADDRESS + 0x2034)
    #define SSPM_INTC_IRQ_WAKE_EN1_IRQ_WAKE_EN1                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_POL0                     (DDRPHY_MD32_BASE_ADDRESS + 0x2040)
    #define SSPM_INTC_IRQ_POL0_IRQ_POL0                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_POL1                     (DDRPHY_MD32_BASE_ADDRESS + 0x2044)
    #define SSPM_INTC_IRQ_POL1_IRQ_POL1                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP0_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x2050)
    #define SSPM_INTC_IRQ_GRP0_0_IRQ_GRP0_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP0_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x2054)
    #define SSPM_INTC_IRQ_GRP0_1_IRQ_GRP0_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP1_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x2060)
    #define SSPM_INTC_IRQ_GRP1_0_IRQ_GRP1_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP1_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x2064)
    #define SSPM_INTC_IRQ_GRP1_1_IRQ_GRP1_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP2_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x2070)
    #define SSPM_INTC_IRQ_GRP2_0_IRQ_GRP2_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP2_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x2074)
    #define SSPM_INTC_IRQ_GRP2_1_IRQ_GRP2_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP3_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x2080)
    #define SSPM_INTC_IRQ_GRP3_0_IRQ_GRP3_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP3_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x2084)
    #define SSPM_INTC_IRQ_GRP3_1_IRQ_GRP3_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP4_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x2090)
    #define SSPM_INTC_IRQ_GRP4_0_IRQ_GRP4_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP4_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x2094)
    #define SSPM_INTC_IRQ_GRP4_1_IRQ_GRP4_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP5_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x20A0)
    #define SSPM_INTC_IRQ_GRP5_0_IRQ_GRP5_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP5_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x20A4)
    #define SSPM_INTC_IRQ_GRP5_1_IRQ_GRP5_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP6_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x20B0)
    #define SSPM_INTC_IRQ_GRP6_0_IRQ_GRP6_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP6_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x20B4)
    #define SSPM_INTC_IRQ_GRP6_1_IRQ_GRP6_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP7_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x20C0)
    #define SSPM_INTC_IRQ_GRP7_0_IRQ_GRP7_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP7_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x20C4)
    #define SSPM_INTC_IRQ_GRP7_1_IRQ_GRP7_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP8_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x20D0)
    #define SSPM_INTC_IRQ_GRP8_0_IRQ_GRP8_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP8_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x20D4)
    #define SSPM_INTC_IRQ_GRP8_1_IRQ_GRP8_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP9_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x20E0)
    #define SSPM_INTC_IRQ_GRP9_0_IRQ_GRP9_0                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP9_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x20E4)
    #define SSPM_INTC_IRQ_GRP9_1_IRQ_GRP9_1                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP10_0                  (DDRPHY_MD32_BASE_ADDRESS + 0x20F0)
    #define SSPM_INTC_IRQ_GRP10_0_IRQ_GRP10_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP10_1                  (DDRPHY_MD32_BASE_ADDRESS + 0x20F4)
    #define SSPM_INTC_IRQ_GRP10_1_IRQ_GRP10_1                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP11_0                  (DDRPHY_MD32_BASE_ADDRESS + 0x2100)
    #define SSPM_INTC_IRQ_GRP11_0_IRQ_GRP11_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP11_1                  (DDRPHY_MD32_BASE_ADDRESS + 0x2104)
    #define SSPM_INTC_IRQ_GRP11_1_IRQ_GRP11_1                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP12_0                  (DDRPHY_MD32_BASE_ADDRESS + 0x2110)
    #define SSPM_INTC_IRQ_GRP12_0_IRQ_GRP12_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP12_1                  (DDRPHY_MD32_BASE_ADDRESS + 0x2114)
    #define SSPM_INTC_IRQ_GRP12_1_IRQ_GRP12_1                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP13_0                  (DDRPHY_MD32_BASE_ADDRESS + 0x2120)
    #define SSPM_INTC_IRQ_GRP13_0_IRQ_GRP13_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP13_1                  (DDRPHY_MD32_BASE_ADDRESS + 0x2124)
    #define SSPM_INTC_IRQ_GRP13_1_IRQ_GRP13_1                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP14_0                  (DDRPHY_MD32_BASE_ADDRESS + 0x2130)
    #define SSPM_INTC_IRQ_GRP14_0_IRQ_GRP14_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP14_1                  (DDRPHY_MD32_BASE_ADDRESS + 0x2134)
    #define SSPM_INTC_IRQ_GRP14_1_IRQ_GRP14_1                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP0_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x2150)
    #define SSPM_INTC_IRQ_GRP0_STA0_IRQ_GRP0_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP0_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x2154)
    #define SSPM_INTC_IRQ_GRP0_STA1_IRQ_GRP0_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP1_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x2160)
    #define SSPM_INTC_IRQ_GRP1_STA0_IRQ_GRP1_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP1_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x2164)
    #define SSPM_INTC_IRQ_GRP1_STA1_IRQ_GRP1_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP2_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x2170)
    #define SSPM_INTC_IRQ_GRP2_STA0_IRQ_GRP2_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP2_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x2174)
    #define SSPM_INTC_IRQ_GRP2_STA1_IRQ_GRP2_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP3_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x2180)
    #define SSPM_INTC_IRQ_GRP3_STA0_IRQ_GRP3_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP3_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x2184)
    #define SSPM_INTC_IRQ_GRP3_STA1_IRQ_GRP3_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP4_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x2190)
    #define SSPM_INTC_IRQ_GRP4_STA0_IRQ_GRP4_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP4_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x2194)
    #define SSPM_INTC_IRQ_GRP4_STA1_IRQ_GRP4_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP5_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x21A0)
    #define SSPM_INTC_IRQ_GRP5_STA0_IRQ_GRP5_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP5_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x21A4)
    #define SSPM_INTC_IRQ_GRP5_STA1_IRQ_GRP5_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP6_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x21B0)
    #define SSPM_INTC_IRQ_GRP6_STA0_IRQ_GRP6_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP6_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x21B4)
    #define SSPM_INTC_IRQ_GRP6_STA1_IRQ_GRP6_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP7_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x21C0)
    #define SSPM_INTC_IRQ_GRP7_STA0_IRQ_GRP7_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP7_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x21C4)
    #define SSPM_INTC_IRQ_GRP7_STA1_IRQ_GRP7_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP8_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x21D0)
    #define SSPM_INTC_IRQ_GRP8_STA0_IRQ_GRP8_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP8_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x21D4)
    #define SSPM_INTC_IRQ_GRP8_STA1_IRQ_GRP8_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP9_STA0                (DDRPHY_MD32_BASE_ADDRESS + 0x21E0)
    #define SSPM_INTC_IRQ_GRP9_STA0_IRQ_GRP9_STA0              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP9_STA1                (DDRPHY_MD32_BASE_ADDRESS + 0x21E4)
    #define SSPM_INTC_IRQ_GRP9_STA1_IRQ_GRP9_STA1              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP10_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x21F0)
    #define SSPM_INTC_IRQ_GRP10_STA0_IRQ_GRP10_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP10_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x21F4)
    #define SSPM_INTC_IRQ_GRP10_STA1_IRQ_GRP10_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP11_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x2200)
    #define SSPM_INTC_IRQ_GRP11_STA0_IRQ_GRP11_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP11_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x2204)
    #define SSPM_INTC_IRQ_GRP11_STA1_IRQ_GRP11_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP12_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x2210)
    #define SSPM_INTC_IRQ_GRP12_STA0_IRQ_GRP12_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP12_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x2214)
    #define SSPM_INTC_IRQ_GRP12_STA1_IRQ_GRP12_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP13_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x2220)
    #define SSPM_INTC_IRQ_GRP13_STA0_IRQ_GRP13_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP13_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x2224)
    #define SSPM_INTC_IRQ_GRP13_STA1_IRQ_GRP13_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP14_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x2230)
    #define SSPM_INTC_IRQ_GRP14_STA0_IRQ_GRP14_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP14_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x2234)
    #define SSPM_INTC_IRQ_GRP14_STA1_IRQ_GRP14_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP15_STA0               (DDRPHY_MD32_BASE_ADDRESS + 0x2240)
    #define SSPM_INTC_IRQ_GRP15_STA0_IRQ_GRP15_STA0            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_GRP15_STA1               (DDRPHY_MD32_BASE_ADDRESS + 0x2244)
    #define SSPM_INTC_IRQ_GRP15_STA1_IRQ_GRP15_STA1            Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_OUT                      (DDRPHY_MD32_BASE_ADDRESS + 0x2250)
    #define SSPM_INTC_IRQ_OUT_IRQ_OUT                          Fld(15, 0) //[14:0]

#define DDRPHY_MD32_REG_SSPM_INTC_IRQ_CLR_TRG                  (DDRPHY_MD32_BASE_ADDRESS + 0x2254)
    #define SSPM_INTC_IRQ_CLR_TRG_IRQ_CLR_TRG                  Fld(15, 0) //[14:0]

#define DDRPHY_MD32_REG_SSPM_INTC_UART_RX_IRQ                  (DDRPHY_MD32_BASE_ADDRESS + 0x2258)
    #define SSPM_INTC_UART_RX_IRQ_UART_RX_IRQ                  Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CK_EN                             (DDRPHY_MD32_BASE_ADDRESS + 0x3000)
    #define SSPM_CK_EN_R_CLK_EN                                Fld(12, 0) //[11:0]
    #define SSPM_CK_EN_R_LPIF_CLK_FR                           Fld(1, 12) //[12:12]
    #define SSPM_CK_EN_R_DCM_MCLK_DIV                          Fld(2, 13) //[14:13]
    #define SSPM_CK_EN_R_LPIF_CLK_26M                          Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_MCLK_DIV                          (DDRPHY_MD32_BASE_ADDRESS + 0x3004)
    #define SSPM_MCLK_DIV_MCLK_DIV                             Fld(2, 0) //[1:0]
    #define SSPM_MCLK_DIV_RSV0                                 Fld(2, 2) //[3:2]
    #define SSPM_MCLK_DIV_MCLK_SRC                             Fld(2, 4) //[5:4]
    #define SSPM_MCLK_DIV_RSV1                                 Fld(2, 6) //[7:6]
    #define SSPM_MCLK_DIV_MCLK_DCM_EN                          Fld(1, 8) //[8:8]
    #define SSPM_MCLK_DIV_RSV2                                 Fld(7, 9) //[15:9]
    #define SSPM_MCLK_DIV_DIVSW_SEL_O                          Fld(4, 16) //[19:16]
    #define SSPM_MCLK_DIV_CKSRC_SEL_O                          Fld(4, 20) //[23:20]
    #define SSPM_MCLK_DIV_RSV3                                 Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_SSPM_DCM_CTRL                          (DDRPHY_MD32_BASE_ADDRESS + 0x3008)
    #define SSPM_DCM_CTRL_R_DCM_EN                             Fld(12, 0) //[11:0]
    #define SSPM_DCM_CTRL_WAKEUP_TYPE                          Fld(1, 28) //[28:28]
    #define SSPM_DCM_CTRL_MD32_GATED                           Fld(1, 30) //[30:30]
    #define SSPM_DCM_CTRL_CLK_OFF                              Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_SSPM_WAKE_INT                          (DDRPHY_MD32_BASE_ADDRESS + 0x300C)
    #define SSPM_WAKE_INT_WAKEUP_INT                           Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_CTRL                         (DDRPHY_MD32_BASE_ADDRESS + 0x3010)
    #define SSPM_UART_CTRL_UART_BCLK_CG                        Fld(1, 0) //[0:0]
    #define SSPM_UART_CTRL_UART_CLK_SEL                        Fld(2, 1) //[2:1]
    #define SSPM_UART_CTRL_UART_RST_N                          Fld(1, 3) //[3:3]

#define DDRPHY_MD32_REG_SSPM_DMA_GLBSTA                        (DDRPHY_MD32_BASE_ADDRESS + 0x4000)
    #define SSPM_DMA_GLBSTA_RUN_1                              Fld(1, 0) //[0:0]
    #define SSPM_DMA_GLBSTA_INTSTA_1                           Fld(1, 1) //[1:1]
    #define SSPM_DMA_GLBSTA_RUN_2                              Fld(1, 2) //[2:2]
    #define SSPM_DMA_GLBSTA_INTSTA_2                           Fld(1, 3) //[3:3]
    #define SSPM_DMA_GLBSTA_RUN_3                              Fld(1, 4) //[4:4]
    #define SSPM_DMA_GLBSTA_INTSTA_3                           Fld(1, 5) //[5:5]
    #define SSPM_DMA_GLBSTA_RUN_4                              Fld(1, 6) //[6:6]
    #define SSPM_DMA_GLBSTA_INTSTA_4                           Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_DMA_GLBSTA2                       (DDRPHY_MD32_BASE_ADDRESS + 0x4004)
    #define SSPM_DMA_GLBSTA2_RUN_1                             Fld(1, 0) //[0:0]
    #define SSPM_DMA_GLBSTA2_INTSTA_1                          Fld(1, 1) //[1:1]
    #define SSPM_DMA_GLBSTA2_RUN_2                             Fld(1, 2) //[2:2]
    #define SSPM_DMA_GLBSTA2_INTSTA_2                          Fld(1, 3) //[3:3]
    #define SSPM_DMA_GLBSTA2_RUN_3                             Fld(1, 4) //[4:4]
    #define SSPM_DMA_GLBSTA2_INTSTA_3                          Fld(1, 5) //[5:5]
    #define SSPM_DMA_GLBSTA2_RUN_4                             Fld(1, 6) //[6:6]
    #define SSPM_DMA_GLBSTA2_INTSTA_4                          Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_DMA_GLBLIMITER                    (DDRPHY_MD32_BASE_ADDRESS + 0x4028)
    #define SSPM_DMA_GLBLIMITER_GLBLIMITER                     Fld(4, 0) //[3:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_SRC                          (DDRPHY_MD32_BASE_ADDRESS + 0x4100)
    #define SSPM_DMA1_SRC_SRC                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_DST                          (DDRPHY_MD32_BASE_ADDRESS + 0x4104)
    #define SSPM_DMA1_DST_DST                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_WPPT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4108)
    #define SSPM_DMA1_WPPT_WPPT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_WPTO                         (DDRPHY_MD32_BASE_ADDRESS + 0x410C)
    #define SSPM_DMA1_WPTO_WPTO                                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_COUNT                        (DDRPHY_MD32_BASE_ADDRESS + 0x4110)
    #define SSPM_DMA1_COUNT_COUNT                              Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_CON                          (DDRPHY_MD32_BASE_ADDRESS + 0x4114)
    #define SSPM_DMA1_CON_SIZE                                 Fld(2, 0) //[1:0]
    #define SSPM_DMA1_CON_SRC_BEN                              Fld(1, 2) //[2:2]
    #define SSPM_DMA1_CON_DST_BEN                              Fld(1, 3) //[3:3]
    #define SSPM_DMA1_CON_DRQ                                  Fld(1, 4) //[4:4]
    #define SSPM_DMA1_CON_BRUST_TYPE                           Fld(2, 8) //[9:8]
    #define SSPM_DMA1_CON_INTEN                                Fld(1, 15) //[15:15]
    #define SSPM_DMA1_CON_WPSD                                 Fld(1, 16) //[16:16]
    #define SSPM_DMA1_CON_WPEN                                 Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_DMA1_START                        (DDRPHY_MD32_BASE_ADDRESS + 0x4118)
    #define SSPM_DMA1_START_START                              Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA1_INTSTA                       (DDRPHY_MD32_BASE_ADDRESS + 0x411C)
    #define SSPM_DMA1_INTSTA_INTSTA                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA1_ACKINT                       (DDRPHY_MD32_BASE_ADDRESS + 0x4120)
    #define SSPM_DMA1_ACKINT_ACKINT                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA1_RLCT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4124)
    #define SSPM_DMA1_RLCT_RLCT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA1_LIMITER                      (DDRPHY_MD32_BASE_ADDRESS + 0x4128)
    #define SSPM_DMA1_LIMITER_LIMITER                          Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_SRC                          (DDRPHY_MD32_BASE_ADDRESS + 0x4200)
    #define SSPM_DMA2_SRC_SRC                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_DST                          (DDRPHY_MD32_BASE_ADDRESS + 0x4204)
    #define SSPM_DMA2_DST_DST                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_WPPT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4208)
    #define SSPM_DMA2_WPPT_WPPT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_WPTO                         (DDRPHY_MD32_BASE_ADDRESS + 0x420C)
    #define SSPM_DMA2_WPTO_WPTO                                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_COUNT                        (DDRPHY_MD32_BASE_ADDRESS + 0x4210)
    #define SSPM_DMA2_COUNT_COUNT                              Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_CON                          (DDRPHY_MD32_BASE_ADDRESS + 0x4214)
    #define SSPM_DMA2_CON_SIZE                                 Fld(2, 0) //[1:0]
    #define SSPM_DMA2_CON_SRC_BEN                              Fld(1, 2) //[2:2]
    #define SSPM_DMA2_CON_DST_BEN                              Fld(1, 3) //[3:3]
    #define SSPM_DMA2_CON_DRQ                                  Fld(1, 4) //[4:4]
    #define SSPM_DMA2_CON_BRUST_TYPE                           Fld(2, 8) //[9:8]
    #define SSPM_DMA2_CON_INTEN                                Fld(1, 15) //[15:15]
    #define SSPM_DMA2_CON_WPSD                                 Fld(1, 16) //[16:16]
    #define SSPM_DMA2_CON_WPEN                                 Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_DMA2_START                        (DDRPHY_MD32_BASE_ADDRESS + 0x4218)
    #define SSPM_DMA2_START_START                              Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA2_INTSTA                       (DDRPHY_MD32_BASE_ADDRESS + 0x421C)
    #define SSPM_DMA2_INTSTA_INTSTA                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA2_ACKINT                       (DDRPHY_MD32_BASE_ADDRESS + 0x4220)
    #define SSPM_DMA2_ACKINT_ACKINT                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA2_RLCT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4224)
    #define SSPM_DMA2_RLCT_RLCT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA2_LIMITER                      (DDRPHY_MD32_BASE_ADDRESS + 0x4228)
    #define SSPM_DMA2_LIMITER_LIMITER                          Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_SRC                          (DDRPHY_MD32_BASE_ADDRESS + 0x4300)
    #define SSPM_DMA3_SRC_SRC                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_DST                          (DDRPHY_MD32_BASE_ADDRESS + 0x4304)
    #define SSPM_DMA3_DST_DST                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_WPPT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4308)
    #define SSPM_DMA3_WPPT_WPPT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_WPTO                         (DDRPHY_MD32_BASE_ADDRESS + 0x430C)
    #define SSPM_DMA3_WPTO_WPTO                                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_COUNT                        (DDRPHY_MD32_BASE_ADDRESS + 0x4310)
    #define SSPM_DMA3_COUNT_COUNT                              Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_CON                          (DDRPHY_MD32_BASE_ADDRESS + 0x4314)
    #define SSPM_DMA3_CON_SIZE                                 Fld(2, 0) //[1:0]
    #define SSPM_DMA3_CON_SRC_BEN                              Fld(1, 2) //[2:2]
    #define SSPM_DMA3_CON_DST_BEN                              Fld(1, 3) //[3:3]
    #define SSPM_DMA3_CON_DRQ                                  Fld(1, 4) //[4:4]
    #define SSPM_DMA3_CON_BRUST_TYPE                           Fld(2, 8) //[9:8]
    #define SSPM_DMA3_CON_INTEN                                Fld(1, 15) //[15:15]
    #define SSPM_DMA3_CON_WPSD                                 Fld(1, 16) //[16:16]
    #define SSPM_DMA3_CON_WPEN                                 Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_DMA3_START                        (DDRPHY_MD32_BASE_ADDRESS + 0x4318)
    #define SSPM_DMA3_START_START                              Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA3_INTSTA                       (DDRPHY_MD32_BASE_ADDRESS + 0x431C)
    #define SSPM_DMA3_INTSTA_INTSTA                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA3_ACKINT                       (DDRPHY_MD32_BASE_ADDRESS + 0x4320)
    #define SSPM_DMA3_ACKINT_ACKINT                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA3_RLCT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4324)
    #define SSPM_DMA3_RLCT_RLCT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA3_LIMITER                      (DDRPHY_MD32_BASE_ADDRESS + 0x4328)
    #define SSPM_DMA3_LIMITER_LIMITER                          Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_SRC                          (DDRPHY_MD32_BASE_ADDRESS + 0x4400)
    #define SSPM_DMA4_SRC_SRC                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_DST                          (DDRPHY_MD32_BASE_ADDRESS + 0x4404)
    #define SSPM_DMA4_DST_DST                                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_WPPT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4408)
    #define SSPM_DMA4_WPPT_WPPT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_WPTO                         (DDRPHY_MD32_BASE_ADDRESS + 0x440C)
    #define SSPM_DMA4_WPTO_WPTO                                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_COUNT                        (DDRPHY_MD32_BASE_ADDRESS + 0x4410)
    #define SSPM_DMA4_COUNT_COUNT                              Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_CON                          (DDRPHY_MD32_BASE_ADDRESS + 0x4414)
    #define SSPM_DMA4_CON_SIZE                                 Fld(2, 0) //[1:0]
    #define SSPM_DMA4_CON_SRC_BEN                              Fld(1, 2) //[2:2]
    #define SSPM_DMA4_CON_DST_BEN                              Fld(1, 3) //[3:3]
    #define SSPM_DMA4_CON_DRQ                                  Fld(1, 4) //[4:4]
    #define SSPM_DMA4_CON_BRUST_TYPE                           Fld(2, 8) //[9:8]
    #define SSPM_DMA4_CON_INTEN                                Fld(1, 15) //[15:15]
    #define SSPM_DMA4_CON_WPSD                                 Fld(1, 16) //[16:16]
    #define SSPM_DMA4_CON_WPEN                                 Fld(1, 17) //[17:17]

#define DDRPHY_MD32_REG_SSPM_DMA4_START                        (DDRPHY_MD32_BASE_ADDRESS + 0x4418)
    #define SSPM_DMA4_START_START                              Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA4_INTSTA                       (DDRPHY_MD32_BASE_ADDRESS + 0x441C)
    #define SSPM_DMA4_INTSTA_INTSTA                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA4_ACKINT                       (DDRPHY_MD32_BASE_ADDRESS + 0x4420)
    #define SSPM_DMA4_ACKINT_ACKINT                            Fld(1, 15) //[15:15]

#define DDRPHY_MD32_REG_SSPM_DMA4_RLCT                         (DDRPHY_MD32_BASE_ADDRESS + 0x4424)
    #define SSPM_DMA4_RLCT_RLCT                                Fld(16, 0) //[15:0]

#define DDRPHY_MD32_REG_SSPM_DMA4_LIMITER                      (DDRPHY_MD32_BASE_ADDRESS + 0x4428)
    #define SSPM_DMA4_LIMITER_LIMITER                          Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_RBR_THR_DLL_ADDR             (DDRPHY_MD32_BASE_ADDRESS + 0x5000)
    #define SSPM_UART_RBR_THR_DLL_ADDR_RBR                     Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_IER_DLM_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5004)
    #define SSPM_UART_IER_DLM_ADDR_RHRI                        Fld(1, 0) //[0:0]
    #define SSPM_UART_IER_DLM_ADDR_THRI                        Fld(1, 1) //[1:1]
    #define SSPM_UART_IER_DLM_ADDR_LINT_STSI                   Fld(1, 2) //[2:2]
    #define SSPM_UART_IER_DLM_ADDR_MODEM_STSI                  Fld(1, 3) //[3:3]
    #define SSPM_UART_IER_DLM_ADDR_RESERVED                    Fld(1, 4) //[4:4]
    #define SSPM_UART_IER_DLM_ADDR_XOFFI                       Fld(1, 5) //[5:5]
    #define SSPM_UART_IER_DLM_ADDR_RTSI                        Fld(1, 6) //[6:6]
    #define SSPM_UART_IER_DLM_ADDR_CTSI                        Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_IIR_FCR_EFR_ADDR             (DDRPHY_MD32_BASE_ADDRESS + 0x5008)
    #define SSPM_UART_IIR_FCR_EFR_ADDR_ID                      Fld(6, 0) //[5:0]
    #define SSPM_UART_IIR_FCR_EFR_ADDR_FIFOE                   Fld(2, 6) //[7:6]

#define DDRPHY_MD32_REG_SSPM_UART_LCR_ADDR                     (DDRPHY_MD32_BASE_ADDRESS + 0x500C)
    #define SSPM_UART_LCR_ADDR_CHAR_LENGTH                     Fld(2, 0) //[1:0]
    #define SSPM_UART_LCR_ADDR_STB                             Fld(1, 2) //[2:2]
    #define SSPM_UART_LCR_ADDR_PEN                             Fld(1, 3) //[3:3]
    #define SSPM_UART_LCR_ADDR_EPS                             Fld(1, 4) //[4:4]
    #define SSPM_UART_LCR_ADDR_SP                              Fld(1, 5) //[5:5]
    #define SSPM_UART_LCR_ADDR_SB                              Fld(1, 6) //[6:6]
    #define SSPM_UART_LCR_ADDR_DLAB                            Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_MCR_XON1_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x5010)
    #define SSPM_UART_MCR_XON1_ADDR_RTS                        Fld(1, 1) //[1:1]
    #define SSPM_UART_MCR_XON1_ADDR_LOOPBACK_EN                Fld(1, 4) //[4:4]
    #define SSPM_UART_MCR_XON1_ADDR_XOFF_STATUS                Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_LSR_XON2_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x5014)
    #define SSPM_UART_LSR_XON2_ADDR_DR                         Fld(1, 0) //[0:0]
    #define SSPM_UART_LSR_XON2_ADDR_OE                         Fld(1, 1) //[1:1]
    #define SSPM_UART_LSR_XON2_ADDR_PE                         Fld(1, 2) //[2:2]
    #define SSPM_UART_LSR_XON2_ADDR_FE                         Fld(1, 3) //[3:3]
    #define SSPM_UART_LSR_XON2_ADDR_BI                         Fld(1, 4) //[4:4]
    #define SSPM_UART_LSR_XON2_ADDR_THRE                       Fld(1, 5) //[5:5]
    #define SSPM_UART_LSR_XON2_ADDR_TEMT                       Fld(1, 6) //[6:6]
    #define SSPM_UART_LSR_XON2_ADDR_FIFOERR                    Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_MSR_XOFF1_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x5018)
    #define SSPM_UART_MSR_XOFF1_ADDR_DCTS                      Fld(1, 0) //[0:0]
    #define SSPM_UART_MSR_XOFF1_ADDR_CTS                       Fld(1, 4) //[4:4]

#define DDRPHY_MD32_REG_SSPM_UART_SCR_XOFF2_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x501C)
    #define SSPM_UART_SCR_XOFF2_ADDR_SCR                       Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_AUTOBAUD_EN_ADDR             (DDRPHY_MD32_BASE_ADDRESS + 0x5020)
    #define SSPM_UART_AUTOBAUD_EN_ADDR_AUTOBAUD_EN             Fld(1, 0) //[0:0]
    #define SSPM_UART_AUTOBAUD_EN_ADDR_AUTOBAUD_SEL            Fld(1, 1) //[1:1]
    #define SSPM_UART_AUTOBAUD_EN_ADDR_SLEEP_ACK_SEL           Fld(1, 2) //[2:2]

#define DDRPHY_MD32_REG_SSPM_UART_RATE_STEP_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x5024)
    #define SSPM_UART_RATE_STEP_ADDR_SPEED                     Fld(2, 0) //[1:0]

#define DDRPHY_MD32_REG_SSPM_UART_STEP_COUNT_ADDR              (DDRPHY_MD32_BASE_ADDRESS + 0x5028)
    #define SSPM_UART_STEP_COUNT_ADDR_SAMPLECOUNT              Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_SAMPLE_COUNT_ADDR            (DDRPHY_MD32_BASE_ADDRESS + 0x502C)
    #define SSPM_UART_SAMPLE_COUNT_ADDR_SAMPLEPOINT            Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_AUTOBAUD_DATA_ADDR           (DDRPHY_MD32_BASE_ADDRESS + 0x5030)
    #define SSPM_UART_AUTOBAUD_DATA_ADDR_BAUD_RATE             Fld(4, 0) //[3:0]
    #define SSPM_UART_AUTOBAUD_DATA_ADDR_BAUD_STAT             Fld(4, 4) //[7:4]

#define DDRPHY_MD32_REG_SSPM_UART_RATE_FIX_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x5034)
    #define SSPM_UART_RATE_FIX_ADDR_RATE_FIX                   Fld(1, 0) //[0:0]
    #define SSPM_UART_RATE_FIX_ADDR_AUTOBAUD_RATE_FIX          Fld(1, 1) //[1:1]
    #define SSPM_UART_RATE_FIX_ADDR_FREQ_SEL                   Fld(1, 2) //[2:2]

#define DDRPHY_MD32_REG_SSPM_UART_AUTOBAUD_RATE_ADDR           (DDRPHY_MD32_BASE_ADDRESS + 0x5038)
    #define SSPM_UART_AUTOBAUD_RATE_ADDR_AUTOBAUDSAMPLE        Fld(6, 0) //[5:0]

#define DDRPHY_MD32_REG_SSPM_UART_GUARD_ADDR                   (DDRPHY_MD32_BASE_ADDRESS + 0x503C)
    #define SSPM_UART_GUARD_ADDR_GUARD_CNT                     Fld(4, 0) //[3:0]
    #define SSPM_UART_GUARD_ADDR_GUARD_EN                      Fld(1, 4) //[4:4]

#define DDRPHY_MD32_REG_SSPM_UART_ESC_CHAR_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x5040)
    #define SSPM_UART_ESC_CHAR_ADDR_ESC_CHAR                   Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_ESC_EN_ADDR                  (DDRPHY_MD32_BASE_ADDRESS + 0x5044)
    #define SSPM_UART_ESC_EN_ADDR_ESC_EN                       Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_SLEEP_EN_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x5048)
    #define SSPM_UART_SLEEP_EN_ADDR_SLEEP_EN                   Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_RXDMA_EN_ADDR                (DDRPHY_MD32_BASE_ADDRESS + 0x504C)
    #define SSPM_UART_RXDMA_EN_ADDR_RX_DMA_EN                  Fld(1, 0) //[0:0]
    #define SSPM_UART_RXDMA_EN_ADDR_TX_DMA_EN                  Fld(1, 1) //[1:1]
    #define SSPM_UART_RXDMA_EN_ADDR_TO_CNT_AUTORST             Fld(1, 2) //[2:2]
    #define SSPM_UART_RXDMA_EN_ADDR_FIFO_LSR_SEL               Fld(1, 3) //[3:3]

#define DDRPHY_MD32_REG_SSPM_UART_RXTRIG_ADDR                  (DDRPHY_MD32_BASE_ADDRESS + 0x5050)
    #define SSPM_UART_RXTRIG_ADDR_RXTRIG                       Fld(4, 0) //[3:0]

#define DDRPHY_MD32_REG_SSPM_UART_FRACDIV_L_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x5054)
    #define SSPM_UART_FRACDIV_L_ADDR_FRACDIV_L                 Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_FRACDIV_M_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x5058)
    #define SSPM_UART_FRACDIV_M_ADDR_FRACDIV_M                 Fld(2, 0) //[1:0]

#define DDRPHY_MD32_REG_SSPM_UART_FCR_ADDR                     (DDRPHY_MD32_BASE_ADDRESS + 0x505C)
    #define SSPM_UART_FCR_ADDR_FIFO_EN                         Fld(1, 0) //[0:0]
    #define SSPM_UART_FCR_ADDR_RXFIFO_CLR                      Fld(1, 1) //[1:1]
    #define SSPM_UART_FCR_ADDR_TXFIFO_CLR                      Fld(1, 2) //[2:2]
    #define SSPM_UART_FCR_ADDR_RESERVED                        Fld(1, 3) //[3:3]
    #define SSPM_UART_FCR_ADDR_TFTL1_TFTL0                     Fld(2, 4) //[5:4]
    #define SSPM_UART_FCR_ADDR_RFTL1_RFTL0                     Fld(2, 6) //[7:6]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_ADDR                   (DDRPHY_MD32_BASE_ADDRESS + 0x5060)
    #define SSPM_UART_DEBUG_ADDR_ACC_SEL                       Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_1_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5064)
    #define SSPM_UART_DEBUG_1_ADDR_TXSTATE                     Fld(5, 0) //[4:0]
    #define SSPM_UART_DEBUG_1_ADDR_XCSTATE                     Fld(3, 5) //[7:5]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_2_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5068)
    #define SSPM_UART_DEBUG_2_ADDR_RXSTATE                     Fld(4, 0) //[3:0]
    #define SSPM_UART_DEBUG_2_ADDR_IP_TX_DMA0                  Fld(4, 4) //[7:4]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_3_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x506C)
    #define SSPM_UART_DEBUG_3_ADDR_IP_TX_DMA1                  Fld(2, 0) //[1:0]
    #define SSPM_UART_DEBUG_3_ADDR_TOFFSET_TX_DMA              Fld(6, 2) //[7:2]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_4_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5070)
    #define SSPM_UART_DEBUG_4_ADDR_TX_WOFFSET                  Fld(6, 0) //[5:0]
    #define SSPM_UART_DEBUG_4_ADDR_TX_ROFFSET0                 Fld(2, 6) //[7:6]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_5_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5074)
    #define SSPM_UART_DEBUG_5_ADDR_TX_ROFFSET1                 Fld(4, 0) //[3:0]
    #define SSPM_UART_DEBUG_5_ADDR_OP_RX_REQ0                  Fld(4, 4) //[7:4]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_6_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5078)
    #define SSPM_UART_DEBUG_6_ADDR_OP_RX_REQ1                  Fld(2, 0) //[1:0]
    #define SSPM_UART_DEBUG_6_ADDR_ROFFSET_RXDMA               Fld(6, 2) //[7:2]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_7_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x507C)
    #define SSPM_UART_DEBUG_7_ADDR_RX_WOFFSET                  Fld(6, 0) //[5:0]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_8_ADDR                 (DDRPHY_MD32_BASE_ADDRESS + 0x5080)
    #define SSPM_UART_DEBUG_8_ADDR_XON_DET                     Fld(1, 0) //[0:0]
    #define SSPM_UART_DEBUG_8_ADDR_XOFF_DET                    Fld(1, 1) //[1:1]
    #define SSPM_UART_DEBUG_8_ADDR_SUPPLOAD                    Fld(1, 2) //[2:2]
    #define SSPM_UART_DEBUG_8_ADDR_SW_TX_DIS                   Fld(1, 3) //[3:3]
    #define SSPM_UART_DEBUG_8_ADDR_HW_TX_DIS                   Fld(1, 4) //[4:4]
    #define SSPM_UART_DEBUG_8_ADDR_SLEEPING                    Fld(1, 5) //[5:5]
    #define SSPM_UART_DEBUG_8_ADDR_VFIFO_LIMIT                 Fld(1, 6) //[6:6]
    #define SSPM_UART_DEBUG_8_ADDR_HWFIFO_LIMIT                Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_DEBUG_SEL                    (DDRPHY_MD32_BASE_ADDRESS + 0x5084)
    #define SSPM_UART_DEBUG_SEL_UART_DBG_SEL                   Fld(3, 0) //[2:0]

#define DDRPHY_MD32_REG_SSPM_UART_DLL_ADDR                     (DDRPHY_MD32_BASE_ADDRESS + 0x5090)
    #define SSPM_UART_DLL_ADDR_DLL                             Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_DLM_ADDR                     (DDRPHY_MD32_BASE_ADDRESS + 0x5094)
    #define SSPM_UART_DLM_ADDR_DLM                             Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_EFR_ADDR                     (DDRPHY_MD32_BASE_ADDRESS + 0x5098)
    #define SSPM_UART_EFR_ADDR_SW_FLOW_CONTROL                 Fld(4, 0) //[3:0]
    #define SSPM_UART_EFR_ADDR_ENHANCED_EN                     Fld(1, 4) //[4:4]
    #define SSPM_UART_EFR_ADDR_AUTO_RTS_EN                     Fld(1, 6) //[6:6]
    #define SSPM_UART_EFR_ADDR_AUTO_CTS_EN                     Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_SSPM_UART_FEATURE_SEL                  (DDRPHY_MD32_BASE_ADDRESS + 0x509C)
    #define SSPM_UART_FEATURE_SEL_FEATURE_SEL                  Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_XON1_ADDR                    (DDRPHY_MD32_BASE_ADDRESS + 0x50A0)
    #define SSPM_UART_XON1_ADDR_XON1                           Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_XON2_ADDR                    (DDRPHY_MD32_BASE_ADDRESS + 0x50A4)
    #define SSPM_UART_XON2_ADDR_XON2                           Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_XOFF1_ADDR                   (DDRPHY_MD32_BASE_ADDRESS + 0x50A8)
    #define SSPM_UART_XOFF1_ADDR_XOFF1                         Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_XOFF2_ADDR                   (DDRPHY_MD32_BASE_ADDRESS + 0x50AC)
    #define SSPM_UART_XOFF2_ADDR_XOFF2                         Fld(8, 0) //[7:0]

#define DDRPHY_MD32_REG_SSPM_UART_UART_RX_SEL_ADDR             (DDRPHY_MD32_BASE_ADDRESS + 0x50B0)
    #define SSPM_UART_UART_RX_SEL_ADDR_USB_RX_SEL              Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_SLEEP_REQ_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x50B4)
    #define SSPM_UART_SLEEP_REQ_ADDR_SLEEP_REQ                 Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_SLEEP_ACK_ADDR               (DDRPHY_MD32_BASE_ADDRESS + 0x50B8)
    #define SSPM_UART_SLEEP_ACK_ADDR_SLEEP_ACK                 Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_UART_SPM_SEL                      (DDRPHY_MD32_BASE_ADDRESS + 0x50BC)
    #define SSPM_UART_SPM_SEL_SPM_SEL                          Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_TWAM_CTRL                         (DDRPHY_MD32_BASE_ADDRESS + 0x6000)
    #define SSPM_TWAM_CTRL_TWAM_SW_RST                         Fld(1, 0) //[0:0]
    #define SSPM_TWAM_CTRL_TWAM_EN                             Fld(1, 1) //[1:1]
    #define SSPM_TWAM_CTRL_SPEED_MODE_EN                       Fld(1, 2) //[2:2]

#define DDRPHY_MD32_REG_SSPM_TWAM_WINDOW_LEN                   (DDRPHY_MD32_BASE_ADDRESS + 0x6004)
    #define SSPM_TWAM_WINDOW_LEN_TWAM_WINDOW_LEN               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_TWAM_MON_TYPE                     (DDRPHY_MD32_BASE_ADDRESS + 0x6008)
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE0                  Fld(2, 0) //[1:0]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE1                  Fld(2, 2) //[3:2]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE2                  Fld(2, 4) //[5:4]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE3                  Fld(2, 6) //[7:6]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE4                  Fld(2, 8) //[9:8]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE5                  Fld(2, 10) //[11:10]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE6                  Fld(2, 12) //[13:12]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE7                  Fld(2, 14) //[15:14]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE8                  Fld(2, 16) //[17:16]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE9                  Fld(2, 18) //[19:18]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE10                 Fld(2, 20) //[21:20]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE11                 Fld(2, 22) //[23:22]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE12                 Fld(2, 24) //[25:24]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE13                 Fld(2, 26) //[27:26]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE14                 Fld(2, 28) //[29:28]
    #define SSPM_TWAM_MON_TYPE_TWAM_MON_TYPE15                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_SSPM_TWAM_SIG_SEL0                     (DDRPHY_MD32_BASE_ADDRESS + 0x600C)
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH0_SIG_SEL                Fld(5, 0) //[4:0]
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH1_SIG_SEL                Fld(5, 5) //[9:5]
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH2_SIG_SEL                Fld(5, 10) //[14:10]
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH3_SIG_SEL                Fld(5, 15) //[19:15]
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH4_SIG_SEL                Fld(5, 20) //[24:20]
    #define SSPM_TWAM_SIG_SEL0_TWAM_CH5_SIG_SEL                Fld(5, 25) //[29:25]

#define DDRPHY_MD32_REG_SSPM_TWAM_SIG_SEL1                     (DDRPHY_MD32_BASE_ADDRESS + 0x6010)
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH6_SIG_SEL                Fld(5, 0) //[4:0]
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH7_SIG_SEL                Fld(5, 5) //[9:5]
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH8_SIG_SEL                Fld(5, 10) //[14:10]
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH9_SIG_SEL                Fld(5, 15) //[19:15]
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH10_SIG_SEL               Fld(5, 20) //[24:20]
    #define SSPM_TWAM_SIG_SEL1_TWAM_CH11_SIG_SEL               Fld(5, 25) //[29:25]

#define DDRPHY_MD32_REG_SSPM_TWAM_SIG_SEL2                     (DDRPHY_MD32_BASE_ADDRESS + 0x6014)
    #define SSPM_TWAM_SIG_SEL2_TWAM_CH12_SIG_SEL               Fld(5, 0) //[4:0]
    #define SSPM_TWAM_SIG_SEL2_TWAM_CH13_SIG_SEL               Fld(5, 5) //[9:5]
    #define SSPM_TWAM_SIG_SEL2_TWAM_CH14_SIG_SEL               Fld(5, 10) //[14:10]
    #define SSPM_TWAM_SIG_SEL2_TWAM_CH15_SIG_SEL               Fld(5, 15) //[19:15]

#define DDRPHY_MD32_REG_SSPM_TWAM_IRQ                          (DDRPHY_MD32_BASE_ADDRESS + 0x6018)
    #define SSPM_TWAM_IRQ_TIRQ                                 Fld(1, 0) //[0:0]
    #define SSPM_TWAM_IRQ_IRQ_CLR                              Fld(1, 7) //[7:7]
    #define SSPM_TWAM_IRQ_IRQ_CLR_FLAG                         Fld(1, 8) //[8:8]

#define DDRPHY_MD32_REG_SSPM_TWAM_TIMER                        (DDRPHY_MD32_BASE_ADDRESS + 0x601C)
    #define SSPM_TWAM_TIMER_TWAM_TIMER                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_WFI_WFE                           (DDRPHY_MD32_BASE_ADDRESS + 0x6020)
    #define SSPM_WFI_WFE_WFI_WFE                               Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT0                     (DDRPHY_MD32_BASE_ADDRESS + 0x6030)
    #define SSPM_CUR_IDLE_CNT0_CUR_IDLE_CNT0                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT1                     (DDRPHY_MD32_BASE_ADDRESS + 0x6034)
    #define SSPM_CUR_IDLE_CNT1_CUR_IDLE_CNT1                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT2                     (DDRPHY_MD32_BASE_ADDRESS + 0x6038)
    #define SSPM_CUR_IDLE_CNT2_CUR_IDLE_CNT2                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT3                     (DDRPHY_MD32_BASE_ADDRESS + 0x603C)
    #define SSPM_CUR_IDLE_CNT3_CUR_IDLE_CNT3                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT4                     (DDRPHY_MD32_BASE_ADDRESS + 0x6040)
    #define SSPM_CUR_IDLE_CNT4_CUR_IDLE_CNT4                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT5                     (DDRPHY_MD32_BASE_ADDRESS + 0x6044)
    #define SSPM_CUR_IDLE_CNT5_CUR_IDLE_CNT5                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT6                     (DDRPHY_MD32_BASE_ADDRESS + 0x6048)
    #define SSPM_CUR_IDLE_CNT6_CUR_IDLE_CNT6                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT7                     (DDRPHY_MD32_BASE_ADDRESS + 0x604C)
    #define SSPM_CUR_IDLE_CNT7_CUR_IDLE_CNT7                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT8                     (DDRPHY_MD32_BASE_ADDRESS + 0x6050)
    #define SSPM_CUR_IDLE_CNT8_CUR_IDLE_CNT8                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT9                     (DDRPHY_MD32_BASE_ADDRESS + 0x6054)
    #define SSPM_CUR_IDLE_CNT9_CUR_IDLE_CNT9                   Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT10                    (DDRPHY_MD32_BASE_ADDRESS + 0x6058)
    #define SSPM_CUR_IDLE_CNT10_CUR_IDLE_CNT10                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT11                    (DDRPHY_MD32_BASE_ADDRESS + 0x605C)
    #define SSPM_CUR_IDLE_CNT11_CUR_IDLE_CNT11                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT12                    (DDRPHY_MD32_BASE_ADDRESS + 0x6060)
    #define SSPM_CUR_IDLE_CNT12_CUR_IDLE_CNT12                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT13                    (DDRPHY_MD32_BASE_ADDRESS + 0x6064)
    #define SSPM_CUR_IDLE_CNT13_CUR_IDLE_CNT13                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT14                    (DDRPHY_MD32_BASE_ADDRESS + 0x6068)
    #define SSPM_CUR_IDLE_CNT14_CUR_IDLE_CNT14                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_CUR_IDLE_CNT15                    (DDRPHY_MD32_BASE_ADDRESS + 0x606C)
    #define SSPM_CUR_IDLE_CNT15_CUR_IDLE_CNT15                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT0                    (DDRPHY_MD32_BASE_ADDRESS + 0x6080)
    #define SSPM_LAST_IDLE_CNT0_LAST_IDEL_CNT0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT1                    (DDRPHY_MD32_BASE_ADDRESS + 0x6084)
    #define SSPM_LAST_IDLE_CNT1_LAST_IDEL_CNT1                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT2                    (DDRPHY_MD32_BASE_ADDRESS + 0x6088)
    #define SSPM_LAST_IDLE_CNT2_LAST_IDEL_CNT2                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT3                    (DDRPHY_MD32_BASE_ADDRESS + 0x608C)
    #define SSPM_LAST_IDLE_CNT3_LAST_IDEL_CNT3                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT4                    (DDRPHY_MD32_BASE_ADDRESS + 0x6090)
    #define SSPM_LAST_IDLE_CNT4_LAST_IDEL_CNT4                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT5                    (DDRPHY_MD32_BASE_ADDRESS + 0x6094)
    #define SSPM_LAST_IDLE_CNT5_LAST_IDEL_CNT5                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT6                    (DDRPHY_MD32_BASE_ADDRESS + 0x6098)
    #define SSPM_LAST_IDLE_CNT6_LAST_IDEL_CNT6                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT7                    (DDRPHY_MD32_BASE_ADDRESS + 0x609C)
    #define SSPM_LAST_IDLE_CNT7_LAST_IDEL_CNT7                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT8                    (DDRPHY_MD32_BASE_ADDRESS + 0x60A0)
    #define SSPM_LAST_IDLE_CNT8_LAST_IDEL_CNT8                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT9                    (DDRPHY_MD32_BASE_ADDRESS + 0x60A4)
    #define SSPM_LAST_IDLE_CNT9_LAST_IDEL_CNT9                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT10                   (DDRPHY_MD32_BASE_ADDRESS + 0x60A8)
    #define SSPM_LAST_IDLE_CNT10_LAST_IDEL_CNT10               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT11                   (DDRPHY_MD32_BASE_ADDRESS + 0x60AC)
    #define SSPM_LAST_IDLE_CNT11_LAST_IDEL_CNT11               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT12                   (DDRPHY_MD32_BASE_ADDRESS + 0x60B0)
    #define SSPM_LAST_IDLE_CNT12_LAST_IDEL_CNT12               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT13                   (DDRPHY_MD32_BASE_ADDRESS + 0x60B4)
    #define SSPM_LAST_IDLE_CNT13_LAST_IDEL_CNT13               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT14                   (DDRPHY_MD32_BASE_ADDRESS + 0x60B8)
    #define SSPM_LAST_IDLE_CNT14_LAST_IDEL_CNT14               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_SSPM_LAST_IDLE_CNT15                   (DDRPHY_MD32_BASE_ADDRESS + 0x60BC)
    #define SSPM_LAST_IDLE_CNT15_LAST_IDEL_CNT15               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_FSM_CFG                           (DDRPHY_MD32_BASE_ADDRESS + 0x7000)
    #define LPIF_FSM_CFG_LPIF_FMS_SW_RSTB                      Fld(1, 0) //[0:0]
    #define LPIF_FSM_CFG_LPIF_INTERNAL_TEST                    Fld(1, 1) //[1:1]
    #define LPIF_FSM_CFG_LPIF_DFS_RUNTIME_MRW_EN               Fld(1, 2) //[2:2]
    #define LPIF_FSM_CFG_LPIF_FSM_VAL_LOAD_FROM_CFG            Fld(1, 3) //[3:3]
    #define LPIF_FSM_CFG_LPIF_FSM_CTRL_SINGLE_CH               Fld(1, 4) //[4:4]
    #define LPIF_FSM_CFG_LPIF_SPM_IN_SYNC_BYPASS               Fld(1, 5) //[5:5]
    #define LPIF_FSM_CFG_LPIF_FSM_CONTROL_SINGLE_CH_HYBRID_S1  Fld(1, 6) //[6:6]
    #define LPIF_FSM_CFG_LPIF_PLL_CONTROL_SINGLE_CHANNEL       Fld(1, 7) //[7:7]
    #define LPIF_FSM_CFG_LPIF_LP_NEW_8X                        Fld(1, 8) //[8:8]
    #define LPIF_FSM_CFG_LPIF_SHU_SRAM_BASED                   Fld(1, 9) //[9:9]
    #define LPIF_FSM_CFG_LPIF_SHU_INDEX                        Fld(1, 10) //[10:10]
    #define LPIF_FSM_CFG_DBG_LATENCY_CNT_EN                    Fld(1, 11) //[11:11]
    #define LPIF_FSM_CFG_LPIF_FSM                              Fld(10, 12) //[21:12]
    #define LPIF_FSM_CFG_SR_DEBON_EN                           Fld(1, 22) //[22:22]
    #define LPIF_FSM_CFG_SR_MIN_PLS_DEBON_EN                   Fld(1, 23) //[23:23]
    #define LPIF_FSM_CFG_DELAY_PST_ACK_OUTPUT_SEL              Fld(4, 24) //[27:24]
    #define LPIF_FSM_CFG_DELAY_PST_ABOUT_OUTPUT_SEL            Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0                   (DDRPHY_MD32_BASE_ADDRESS + 0x7004)
    #define LPIF_LOW_POWER_CFG_0_DMSUS_OFF                     Fld(2, 0) //[1:0]
    #define LPIF_LOW_POWER_CFG_0_PHYPLL_EN                     Fld(2, 2) //[3:2]
    #define LPIF_LOW_POWER_CFG_0_DPY_DLL_EN                    Fld(2, 4) //[5:4]
    #define LPIF_LOW_POWER_CFG_0_DPY_2ND_DLL_EN                Fld(2, 6) //[7:6]
    #define LPIF_LOW_POWER_CFG_0_DPY_DLL_CK_EN                 Fld(2, 8) //[9:8]
    #define LPIF_LOW_POWER_CFG_0_DPY_VREF_EN                   Fld(2, 10) //[11:10]
    #define LPIF_LOW_POWER_CFG_0_EMI_CLK_OFF_REQ               Fld(2, 12) //[13:12]
    #define LPIF_LOW_POWER_CFG_0_MEM_CK_OFF                    Fld(2, 14) //[15:14]
    #define LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN               Fld(2, 16) //[17:16]
    #define LPIF_LOW_POWER_CFG_0_DR_GATE_RETRY_EN              Fld(2, 18) //[19:18]
    #define LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN                 Fld(2, 20) //[21:20]
    #define LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW                Fld(2, 22) //[23:22]
    #define LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN                Fld(2, 24) //[25:24]
    #define LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW               Fld(2, 26) //[27:26]
    #define LPIF_LOW_POWER_CFG_0_DR_SHU_EN                     Fld(2, 28) //[29:28]
    #define LPIF_LOW_POWER_CFG_0_DR_SHORT_QUEUE                Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1                   (DDRPHY_MD32_BASE_ADDRESS + 0x7008)
    #define LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL                  Fld(4, 0) //[3:0]
    #define LPIF_LOW_POWER_CFG_1_DPY_BCLK_ENABLE               Fld(2, 4) //[5:4]
    #define LPIF_LOW_POWER_CFG_1_SHU_RESTORE                   Fld(2, 6) //[7:6]
    #define LPIF_LOW_POWER_CFG_1_DPHY_PRECAL_UP                Fld(2, 8) //[9:8]
    #define LPIF_LOW_POWER_CFG_1_DPHY_RXDLY_TRACK_EN           Fld(2, 10) //[11:10]
    #define LPIF_LOW_POWER_CFG_1_DMY_EN_MOD_SEL                Fld(2, 12) //[13:12]
    #define LPIF_LOW_POWER_CFG_1_DMYRD_INTV_SEL                Fld(2, 14) //[15:14]
    #define LPIF_LOW_POWER_CFG_1_DMYRD_EN                      Fld(2, 16) //[17:16]
    #define LPIF_LOW_POWER_CFG_1_TX_TRACKING_DIS               Fld(2, 18) //[19:18]
    #define LPIF_LOW_POWER_CFG_1_TX_TRACKING_RETRY_EN          Fld(2, 20) //[21:20]
    #define LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL             Fld(8, 22) //[29:22]
    #define LPIF_LOW_POWER_CFG_1_DR_SRAM_LOAD                  Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2                   (DDRPHY_MD32_BASE_ADDRESS + 0x700C)
    #define LPIF_LOW_POWER_CFG_2_DR_SRAM_RESTORE               Fld(2, 0) //[1:0]
    #define LPIF_LOW_POWER_CFG_2_DR_SHU_LEVEL_SRAM_LATCH       Fld(2, 2) //[3:2]
    #define LPIF_LOW_POWER_CFG_2_DPY_MODE_SW                   Fld(2, 4) //[5:4]
    #define LPIF_LOW_POWER_CFG_2_EMI_SLEEP_PROT_EN             Fld(1, 6) //[6:6]
    #define LPIF_LOW_POWER_CFG_2_MPLLOUT_OFF                   Fld(1, 7) //[7:7]
    #define LPIF_LOW_POWER_CFG_2_DPY_RESERVED                  Fld(8, 8) //[15:8]
    #define LPIF_LOW_POWER_CFG_2_DRAMC_DFS_STA                 Fld(13, 16) //[28:16]
    #define LPIF_LOW_POWER_CFG_2_MPLL_S_OFF                    Fld(1, 29) //[29:29]
    #define LPIF_LOW_POWER_CFG_2_FHC_PAUSE_MPLL                Fld(1, 30) //[30:30]
    #define LPIF_LOW_POWER_CFG_2_FHC_PAUSE_MEM                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_0                    (DDRPHY_MD32_BASE_ADDRESS + 0x7010)
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DMSUS_OFF              Fld(1, 0) //[0:0]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_EN              Fld(1, 1) //[1:1]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_EN             Fld(1, 2) //[2:2]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_2ND_DLL_EN         Fld(1, 3) //[3:3]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_CK_EN          Fld(1, 4) //[4:4]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_VREF_EN            Fld(1, 5) //[5:5]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_EMI_CLK_OFF_REQ        Fld(1, 6) //[6:6]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_MEM_CK_OFF             Fld(1, 7) //[7:7]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DDRPHY_FB_CK_EN        Fld(1, 8) //[8:8]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_GATE_RETRY_EN       Fld(1, 9) //[9:9]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_SHU_EN          Fld(1, 10) //[10:10]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_MODE_SW         Fld(1, 11) //[11:11]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL2_SHU_EN         Fld(1, 12) //[12:12]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL2_MODE_SW        Fld(1, 13) //[13:13]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SHU_EN              Fld(1, 14) //[14:14]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SHORT_QUEUE         Fld(1, 15) //[15:15]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SHU_LEVEL           Fld(1, 16) //[16:16]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_BCLK_ENABLE        Fld(1, 17) //[17:17]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_SHU_RESTORE            Fld(1, 18) //[18:18]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPHY_PRECAL_UP         Fld(1, 19) //[19:19]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPHY_RXDLY_TRACK_EN    Fld(1, 20) //[20:20]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DMY_EN_MOD_SEL         Fld(1, 21) //[21:21]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DMYRD_INTV_SEL         Fld(1, 22) //[22:22]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DMYRD_EN               Fld(1, 23) //[23:23]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_TX_TRACKING_DIS        Fld(1, 24) //[24:24]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_TX_TRACKING_RETRY_EN   Fld(1, 25) //[25:25]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SHU_SRAM_LEVEL      Fld(1, 26) //[26:26]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SRAM_LOAD           Fld(1, 27) //[27:27]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SRAM_RESTORE        Fld(1, 28) //[28:28]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DR_SHU_LEVEL_SRAM_LATCH Fld(1, 29) //[29:29]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_RESERVED           Fld(1, 30) //[30:30]
    #define LPIF_FSM_OUT_CTRL_0_LOG_OPT_DRAMC_DFS_STA          Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_1                    (DDRPHY_MD32_BASE_ADDRESS + 0x7014)
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_MODE_SW            Fld(1, 0) //[0:0]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_MPLL_S_OFF             Fld(1, 1) //[1:1]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_FHC_PAUSE_MPLL         Fld(1, 2) //[2:2]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_FHC_PAUSE_MEM          Fld(1, 3) //[3:3]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_MCK8X_EN           Fld(1, 4) //[4:4]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_MIDPI_EN           Fld(1, 5) //[5:5]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_PI_RESETB_EN       Fld(1, 6) //[6:6]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DVFS_MEM_CK_MUX_UPDATE Fld(1, 7) //[7:7]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DVFS_MEM_CK_MUX_SEL    Fld(1, 8) //[8:8]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_DSM_EN             Fld(1, 9) //[9:9]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_PICG_FREE          Fld(1, 10) //[10:10]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_FASTK_RDDQS_EN     Fld(1, 11) //[11:11]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_CS_PULL_UP_EN      Fld(1, 12) //[12:12]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_CS_PULL_DN_EN      Fld(1, 13) //[13:13]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_CA_PULL_UP_EN      Fld(1, 14) //[14:14]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_DPY_CA_PULL_DN_EN      Fld(1, 15) //[15:15]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_MPLLOUT_OFF            Fld(1, 16) //[16:16]
    #define LPIF_FSM_OUT_CTRL_1_LOG_OPT_EMI_S1_MODE_ASYNC      Fld(1, 17) //[17:17]
    #define LPIF_FSM_OUT_CTRL_1_RESERVED_X14_31_16             Fld(14, 18) //[31:18]

#define DDRPHY_MD32_REG_LPIF_IPC_MASK_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x7018)
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0               Fld(1, 0) //[0:0]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_01              Fld(1, 1) //[1:1]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_02              Fld(1, 2) //[2:2]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_03              Fld(1, 3) //[3:3]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_04              Fld(1, 4) //[4:4]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_05              Fld(1, 5) //[5:5]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_06              Fld(1, 6) //[6:6]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_07              Fld(1, 7) //[7:7]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_08              Fld(1, 8) //[8:8]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_09              Fld(1, 9) //[9:9]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0A              Fld(1, 10) //[10:10]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0B              Fld(1, 11) //[11:11]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0C              Fld(1, 12) //[12:12]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0D              Fld(1, 13) //[13:13]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0E              Fld(1, 14) //[14:14]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_0F              Fld(1, 15) //[15:15]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_10              Fld(1, 16) //[16:16]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_11              Fld(1, 17) //[17:17]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_12              Fld(1, 18) //[18:18]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_13              Fld(1, 19) //[19:19]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_14              Fld(1, 20) //[20:20]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_15              Fld(1, 21) //[21:21]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_16              Fld(1, 22) //[22:22]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_17              Fld(1, 23) //[23:23]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_18              Fld(1, 24) //[24:24]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_19              Fld(1, 25) //[25:25]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1A              Fld(1, 26) //[26:26]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1B              Fld(1, 27) //[27:27]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1C              Fld(1, 28) //[28:28]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1D              Fld(1, 29) //[29:29]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1E              Fld(1, 30) //[30:30]
    #define LPIF_IPC_MASK_0_PWR_STATE_IPC_MASK_1F              Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_IPC_MASK_1                        (DDRPHY_MD32_BASE_ADDRESS + 0x701C)
    #define LPIF_IPC_MASK_1_PWR_STATE_IPC_MASK_RESERVED        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x7020)
    #define LPIF_FSM_CTRL_0_LPIF_SW_DDR_PST_ABORT_ACK          Fld(1, 0) //[0:0]
    #define LPIF_FSM_CTRL_0_LPIF_SW_DDR_PST_ACK                Fld(1, 1) //[1:1]
    #define LPIF_FSM_CTRL_0_RELEASE_LPIF_IRQ                   Fld(1, 2) //[2:2]
    #define LPIF_FSM_CTRL_0_RELEASE_DDR_PST_ACK                Fld(1, 3) //[3:3]
    #define LPIF_FSM_CTRL_0_DFS_STATUS_RECORD                  Fld(4, 4) //[7:4]
    #define LPIF_FSM_CTRL_0_DVS_STATUS_RECORD                  Fld(1, 8) //[8:8]
    #define LPIF_FSM_CTRL_0_RUN_TIME_STATUS_RECORD             Fld(1, 9) //[9:9]
    #define LPIF_FSM_CTRL_0_RESERVED_X20_11_10                 Fld(2, 10) //[11:10]
    #define LPIF_FSM_CTRL_0_DFS_STATUS_RECORD_UPDATE           Fld(1, 12) //[12:12]
    #define LPIF_FSM_CTRL_0_DVS_STATUS_RECORD_UPDATE           Fld(1, 13) //[13:13]
    #define LPIF_FSM_CTRL_0_RUN_TIME_STATUS_RECORD_UPDATE      Fld(1, 14) //[14:14]
    #define LPIF_FSM_CTRL_0_RESERVED_X20_15_15                 Fld(1, 15) //[15:15]
    #define LPIF_FSM_CTRL_0_JUMP_TO_SIDLE                      Fld(1, 16) //[16:16]
    #define LPIF_FSM_CTRL_0_JUMP_TO_SR                         Fld(1, 17) //[17:17]
    #define LPIF_FSM_CTRL_0_JUMP_TO_S1                         Fld(1, 18) //[18:18]
    #define LPIF_FSM_CTRL_0_JUMP_TO_S0                         Fld(1, 19) //[19:19]
    #define LPIF_FSM_CTRL_0_JUMP_TO_HYBRID_S1                  Fld(1, 20) //[20:20]
    #define LPIF_FSM_CTRL_0_JUMP_TO_DFS                        Fld(1, 21) //[21:21]
    #define LPIF_FSM_CTRL_0_JUMP_TO_DVS_ENTR                   Fld(1, 22) //[22:22]
    #define LPIF_FSM_CTRL_0_JUMP_TO_DVS_EXIT                   Fld(1, 23) //[23:23]
    #define LPIF_FSM_CTRL_0_JUMP_TO_EN_RUNTIME                 Fld(1, 24) //[24:24]
    #define LPIF_FSM_CTRL_0_JUMP_TO_DIS_RUNTIME                Fld(1, 25) //[25:25]
    #define LPIF_FSM_CTRL_0_DRAMC_S0_STATUS                    Fld(2, 26) //[27:26]
    #define LPIF_FSM_CTRL_0_RESERVED_X20_31_28                 Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_1                        (DDRPHY_MD32_BASE_ADDRESS + 0x7024)
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_0                   Fld(1, 0) //[0:0]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_1                   Fld(1, 1) //[1:1]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_2                   Fld(1, 2) //[2:2]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_3                   Fld(1, 3) //[3:3]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_4                   Fld(1, 4) //[4:4]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_5                   Fld(1, 5) //[5:5]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_6                   Fld(1, 6) //[6:6]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_7                   Fld(1, 7) //[7:7]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_8                   Fld(1, 8) //[8:8]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_9                   Fld(1, 9) //[9:9]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_10                  Fld(1, 10) //[10:10]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_11                  Fld(1, 11) //[11:11]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_12                  Fld(1, 12) //[12:12]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_13                  Fld(1, 13) //[13:13]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_14                  Fld(1, 14) //[14:14]
    #define LPIF_FSM_CTRL_1_RXDLY_TRACK_EN_15                  Fld(1, 15) //[15:15]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_0                         Fld(1, 16) //[16:16]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_1                         Fld(1, 17) //[17:17]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_2                         Fld(1, 18) //[18:18]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_3                         Fld(1, 19) //[19:19]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_4                         Fld(1, 20) //[20:20]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_5                         Fld(1, 21) //[21:21]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_6                         Fld(1, 22) //[22:22]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_7                         Fld(1, 23) //[23:23]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_8                         Fld(1, 24) //[24:24]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_9                         Fld(1, 25) //[25:25]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_10                        Fld(1, 26) //[26:26]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_11                        Fld(1, 27) //[27:27]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_12                        Fld(1, 28) //[28:28]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_13                        Fld(1, 29) //[29:29]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_14                        Fld(1, 30) //[30:30]
    #define LPIF_FSM_CTRL_1_DMYRD_EN_15                        Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_2                        (DDRPHY_MD32_BASE_ADDRESS + 0x7028)
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_0                  Fld(1, 0) //[0:0]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_1                  Fld(1, 1) //[1:1]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_2                  Fld(1, 2) //[2:2]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_3                  Fld(1, 3) //[3:3]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_4                  Fld(1, 4) //[4:4]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_5                  Fld(1, 5) //[5:5]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_6                  Fld(1, 6) //[6:6]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_7                  Fld(1, 7) //[7:7]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_8                  Fld(1, 8) //[8:8]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_9                  Fld(1, 9) //[9:9]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_10                 Fld(1, 10) //[10:10]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_11                 Fld(1, 11) //[11:11]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_12                 Fld(1, 12) //[12:12]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_13                 Fld(1, 13) //[13:13]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_14                 Fld(1, 14) //[14:14]
    #define LPIF_FSM_CTRL_2_TX_TRACKING_DIS_15                 Fld(1, 15) //[15:15]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_0                  Fld(1, 16) //[16:16]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_1                  Fld(1, 17) //[17:17]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_2                  Fld(1, 18) //[18:18]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_3                  Fld(1, 19) //[19:19]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_4                  Fld(1, 20) //[20:20]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_5                  Fld(1, 21) //[21:21]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_6                  Fld(1, 22) //[22:22]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_7                  Fld(1, 23) //[23:23]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_8                  Fld(1, 24) //[24:24]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_9                  Fld(1, 25) //[25:25]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_10                 Fld(1, 26) //[26:26]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_11                 Fld(1, 27) //[27:27]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_12                 Fld(1, 28) //[28:28]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_13                 Fld(1, 29) //[29:29]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_14                 Fld(1, 30) //[30:30]
    #define LPIF_FSM_CTRL_2_DR_SRAM_RESTORE_15                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_3                        (DDRPHY_MD32_BASE_ADDRESS + 0x702C)
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_0                Fld(1, 0) //[0:0]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_1                Fld(1, 1) //[1:1]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_2                Fld(1, 2) //[2:2]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_3                Fld(1, 3) //[3:3]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_4                Fld(1, 4) //[4:4]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_5                Fld(1, 5) //[5:5]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_6                Fld(1, 6) //[6:6]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_7                Fld(1, 7) //[7:7]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_8                Fld(1, 8) //[8:8]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_9                Fld(1, 9) //[9:9]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_10               Fld(1, 10) //[10:10]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_11               Fld(1, 11) //[11:11]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_12               Fld(1, 12) //[12:12]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_13               Fld(1, 13) //[13:13]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_14               Fld(1, 14) //[14:14]
    #define LPIF_FSM_CTRL_3_TX_TRACK_RETRY_EN_15               Fld(1, 15) //[15:15]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_0               Fld(1, 16) //[16:16]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_1               Fld(1, 17) //[17:17]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_2               Fld(1, 18) //[18:18]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_3               Fld(1, 19) //[19:19]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_4               Fld(1, 20) //[20:20]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_5               Fld(1, 21) //[21:21]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_6               Fld(1, 22) //[22:22]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_7               Fld(1, 23) //[23:23]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_8               Fld(1, 24) //[24:24]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_9               Fld(1, 25) //[25:25]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_10              Fld(1, 26) //[26:26]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_11              Fld(1, 27) //[27:27]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_12              Fld(1, 28) //[28:28]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_13              Fld(1, 29) //[29:29]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_14              Fld(1, 30) //[30:30]
    #define LPIF_FSM_CTRL_3_RX_GATING_RETRY_EN_15              Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_4                        (DDRPHY_MD32_BASE_ADDRESS + 0x7030)
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_0                 Fld(1, 0) //[0:0]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_1                 Fld(1, 1) //[1:1]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_2                 Fld(1, 2) //[2:2]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_3                 Fld(1, 3) //[3:3]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_4                 Fld(1, 4) //[4:4]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_5                 Fld(1, 5) //[5:5]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_6                 Fld(1, 6) //[6:6]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_7                 Fld(1, 7) //[7:7]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_8                 Fld(1, 8) //[8:8]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_9                 Fld(1, 9) //[9:9]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_10                Fld(1, 10) //[10:10]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_11                Fld(1, 11) //[11:11]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_12                Fld(1, 12) //[12:12]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_13                Fld(1, 13) //[13:13]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_14                Fld(1, 14) //[14:14]
    #define LPIF_FSM_CTRL_4_DLL_ALL_SLAVE_EN_15                Fld(1, 15) //[15:15]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_0            Fld(1, 16) //[16:16]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_1            Fld(1, 17) //[17:17]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_2            Fld(1, 18) //[18:18]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_3            Fld(1, 19) //[19:19]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_4            Fld(1, 20) //[20:20]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_5            Fld(1, 21) //[21:21]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_6            Fld(1, 22) //[22:22]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_7            Fld(1, 23) //[23:23]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_8            Fld(1, 24) //[24:24]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_9            Fld(1, 25) //[25:25]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_10           Fld(1, 26) //[26:26]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_11           Fld(1, 27) //[27:27]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_12           Fld(1, 28) //[28:28]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_13           Fld(1, 29) //[29:29]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_14           Fld(1, 30) //[30:30]
    #define LPIF_FSM_CTRL_4_IMPEDANCE_TRACKING_EN_15           Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_FSM_CFG_1                         (DDRPHY_MD32_BASE_ADDRESS + 0x7034)
    #define LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL                 Fld(1, 0) //[0:0]
    #define LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND             Fld(1, 1) //[1:1]
    #define LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR         Fld(1, 2) //[2:2]
    #define LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND     Fld(1, 3) //[3:3]
    #define LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW            Fld(1, 4) //[4:4]
    #define LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW_2ND        Fld(1, 5) //[5:5]
    #define LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL              Fld(1, 6) //[6:6]
    #define LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL_2ND          Fld(1, 7) //[7:7]

#define DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3                   (DDRPHY_MD32_BASE_ADDRESS + 0x7038)
    #define LPIF_LOW_POWER_CFG_3_DPY_MCK8X_EN                  Fld(2, 0) //[1:0]
    #define LPIF_LOW_POWER_CFG_3_DPY_MIDPI_EN                  Fld(2, 2) //[3:2]
    #define LPIF_LOW_POWER_CFG_3_DPY_PI_RESETB_EN              Fld(2, 4) //[5:4]
    #define LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE        Fld(2, 6) //[7:6]
    #define LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL           Fld(4, 8) //[11:8]
    #define LPIF_LOW_POWER_CFG_3_DPY_DSM_EN                    Fld(2, 12) //[13:12]
    #define LPIF_LOW_POWER_CFG_3_DPY_FASTK_RDDQS_EN            Fld(2, 14) //[15:14]
    #define LPIF_LOW_POWER_CFG_3_DPY_CS_PULL_UP_EN             Fld(2, 16) //[17:16]
    #define LPIF_LOW_POWER_CFG_3_DPY_CS_PULL_DN_EN             Fld(2, 18) //[19:18]
    #define LPIF_LOW_POWER_CFG_3_DPY_CA_PULL_UP_EN             Fld(2, 20) //[21:20]
    #define LPIF_LOW_POWER_CFG_3_DPY_CA_PULL_DN_EN             Fld(2, 22) //[23:22]
    #define LPIF_LOW_POWER_CFG_3_EMI_S1_MODE_ASYNC             Fld(1, 24) //[24:24]
    #define LPIF_LOW_POWER_CFG_3_RESERVED_X38_25_25            Fld(1, 25) //[25:25]
    #define LPIF_LOW_POWER_CFG_3_DPY_PICG_FREE                 Fld(2, 26) //[27:26]
    #define LPIF_LOW_POWER_CFG_3_RESERVED_X38_31_28            Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_DFD_DBUG_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x703C)
    #define LPIF_DFD_DBUG_0_LPIF_DFD_DEBUG_ISO_EN              Fld(1, 0) //[0:0]
    #define LPIF_DFD_DBUG_0_MD32_DRAMC_CKGEN_MCK_CG_EN         Fld(1, 1) //[1:1]
    #define LPIF_DFD_DBUG_0_RESERVED_X3C_31_2                  Fld(30, 2) //[31:2]

#define DDRPHY_MD32_REG_LPIF_RESERVED_3                        (DDRPHY_MD32_BASE_ADDRESS + 0x7040)
    #define LPIF_RESERVED_3_RESERVED_X40_31_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_0             (DDRPHY_MD32_BASE_ADDRESS + 0x7044)
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_TIME_OUT_CLR    Fld(1, 0) //[0:0]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_20US_CLR        Fld(1, 1) //[1:1]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_12US_CLR        Fld(1, 2) //[2:2]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_DLL_1ST_LOCKING_CLR Fld(1, 3) //[3:3]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_DLL_2ND_LOCKING_CLR Fld(1, 4) //[4:4]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_CK_OFF_TO_DMSUS_CLR Fld(1, 5) //[5:5]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_50NS_CLR        Fld(1, 6) //[6:6]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_4US_CLR         Fld(1, 7) //[7:7]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_SR_DEBON_CLR    Fld(1, 8) //[8:8]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_SR_MIN_PLS_DEBON_CLR Fld(1, 9) //[9:9]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_HW_S1_STEP_CLR  Fld(1, 10) //[10:10]
    #define LPIF_TIMING_COUNTER_CTRL_0_COUNTER_256MCK_CLR      Fld(1, 11) //[11:11]
    #define LPIF_TIMING_COUNTER_CTRL_0_RESERVED_X44_31_12      Fld(20, 12) //[31:12]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_1             (DDRPHY_MD32_BASE_ADDRESS + 0x7048)
    #define LPIF_TIMING_COUNTER_CTRL_1_FSM_COUNTER_CLK2        Fld(4, 0) //[3:0]
    #define LPIF_TIMING_COUNTER_CTRL_1_FSM_COUNTER_CLK1        Fld(4, 4) //[7:4]
    #define LPIF_TIMING_COUNTER_CTRL_1_FSM_COUNTER_CLK0        Fld(4, 8) //[11:8]
    #define LPIF_TIMING_COUNTER_CTRL_1_COUNTER_SR_MIN_PLS_DEBON Fld(8, 12) //[19:12]
    #define LPIF_TIMING_COUNTER_CTRL_1_COUNTER_SR_DEBON        Fld(11, 20) //[30:20]
    #define LPIF_TIMING_COUNTER_CTRL_1_RESERVED_X44_31_31      Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_2             (DDRPHY_MD32_BASE_ADDRESS + 0x704C)
    #define LPIF_TIMING_COUNTER_CTRL_2_COUNTER_DLL_1ST_LOCKING_CLK2 Fld(8, 0) //[7:0]
    #define LPIF_TIMING_COUNTER_CTRL_2_COUNTER_DLL_2ND_LOCKING_CLK2 Fld(8, 8) //[15:8]
    #define LPIF_TIMING_COUNTER_CTRL_2_COUNTER_CK_OFF_TO_DMSUS_CLK2 Fld(8, 16) //[23:16]
    #define LPIF_TIMING_COUNTER_CTRL_2_COUNTER_50NS_CLK2       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_3             (DDRPHY_MD32_BASE_ADDRESS + 0x7050)
    #define LPIF_TIMING_COUNTER_CTRL_3_COUNTER_DLL_1ST_LOCKING_CLK1 Fld(8, 0) //[7:0]
    #define LPIF_TIMING_COUNTER_CTRL_3_COUNTER_DLL_2ND_LOCKING_CLK1 Fld(8, 8) //[15:8]
    #define LPIF_TIMING_COUNTER_CTRL_3_COUNTER_CK_OFF_TO_DMSUS_CLK1 Fld(8, 16) //[23:16]
    #define LPIF_TIMING_COUNTER_CTRL_3_COUNTER_50NS_CLK1       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_4             (DDRPHY_MD32_BASE_ADDRESS + 0x7054)
    #define LPIF_TIMING_COUNTER_CTRL_4_COUNTER_DLL_1ST_LOCKING_CLK0 Fld(8, 0) //[7:0]
    #define LPIF_TIMING_COUNTER_CTRL_4_COUNTER_DLL_2ND_LOCKING_CLK0 Fld(8, 8) //[15:8]
    #define LPIF_TIMING_COUNTER_CTRL_4_COUNTER_CK_OFF_TO_DMSUS_CLK0 Fld(8, 16) //[23:16]
    #define LPIF_TIMING_COUNTER_CTRL_4_COUNTER_50NS_CLK0       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_5             (DDRPHY_MD32_BASE_ADDRESS + 0x7058)
    #define LPIF_TIMING_COUNTER_CTRL_5_COUNTER_20US_26M        Fld(10, 0) //[9:0]
    #define LPIF_TIMING_COUNTER_CTRL_5_RESERVED_X58_15_10      Fld(6, 10) //[15:10]
    #define LPIF_TIMING_COUNTER_CTRL_5_COUNTER_TIME_OUT_26M    Fld(16, 16) //[31:16]

#define DDRPHY_MD32_REG_LPIF_TIMING_COUNTER_CTRL_6             (DDRPHY_MD32_BASE_ADDRESS + 0x705C)
    #define LPIF_TIMING_COUNTER_CTRL_6_COUNTER_12US_26M        Fld(10, 0) //[9:0]
    #define LPIF_TIMING_COUNTER_CTRL_6_RESERVED_X5C_15_11      Fld(2, 10) //[11:10]
    #define LPIF_TIMING_COUNTER_CTRL_6_COUNTER_4US_26M         Fld(10, 12) //[21:12]
    #define LPIF_TIMING_COUNTER_CTRL_6_RESERVED_X5C_31_22      Fld(10, 22) //[31:22]

#define DDRPHY_MD32_REG_LPIF_FSM_CTRL_5                        (DDRPHY_MD32_BASE_ADDRESS + 0x7060)
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_0              Fld(2, 0) //[1:0]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_1              Fld(2, 2) //[3:2]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_2              Fld(2, 4) //[5:4]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_3              Fld(2, 6) //[7:6]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_4              Fld(2, 8) //[9:8]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_5              Fld(2, 10) //[11:10]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_6              Fld(2, 12) //[13:12]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_7              Fld(2, 14) //[15:14]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_8              Fld(2, 16) //[17:16]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_9              Fld(2, 18) //[19:18]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_10             Fld(2, 20) //[21:20]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_11             Fld(2, 22) //[23:22]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_12             Fld(2, 24) //[25:24]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_13             Fld(2, 26) //[27:26]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_14             Fld(2, 28) //[29:28]
    #define LPIF_FSM_CTRL_5_DRAM_PAR_CLOCK_MODE_15             Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_RESERVED_5                        (DDRPHY_MD32_BASE_ADDRESS + 0x7064)
    #define LPIF_RESERVED_5_COUNTER_HW_S1_STEP                 Fld(4, 0) //[3:0]
    #define LPIF_RESERVED_5_COUNTER_256MCK                     Fld(9, 4) //[12:4]
    #define LPIF_RESERVED_5_RESERVED_X64_31_13                 Fld(19, 13) //[31:13]

#define DDRPHY_MD32_REG_LPIF_RESERVED_6                        (DDRPHY_MD32_BASE_ADDRESS + 0x7068)
    #define LPIF_RESERVED_6_MAX_CNT_SREF_REQ_HIGH_TO_SREF_ACK  Fld(8, 0) //[7:0]
    #define LPIF_RESERVED_6_MAX_CNT_SREF_REQ_LOW_TO_SREF_ACK   Fld(8, 8) //[15:8]
    #define LPIF_RESERVED_6_MAX_CNT_SHU_EN_HIGH_TO_ACK         Fld(8, 16) //[23:16]
    #define LPIF_RESERVED_6_MAX_CNT_HW_S1_REQ_LOW_TO_SREF_ACK_LOW Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_RESERVED_7                        (DDRPHY_MD32_BASE_ADDRESS + 0x706C)
    #define LPIF_RESERVED_7_RESERVED_X6C_31_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_8                        (DDRPHY_MD32_BASE_ADDRESS + 0x7070)
    #define LPIF_RESERVED_8_RESERVED_X70_31_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_9                        (DDRPHY_MD32_BASE_ADDRESS + 0x7074)
    #define LPIF_RESERVED_9_RESERVED_X74_31_0                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_10                       (DDRPHY_MD32_BASE_ADDRESS + 0x7078)
    #define LPIF_RESERVED_10_RESERVED_X78_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_11                       (DDRPHY_MD32_BASE_ADDRESS + 0x707C)
    #define LPIF_RESERVED_11_RESERVED_X7C_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_SEMA_0                            (DDRPHY_MD32_BASE_ADDRESS + 0x7080)
    #define LPIF_SEMA_0_SEMA_0_M0                              Fld(1, 0) //[0:0]
    #define LPIF_SEMA_0_SEMA_1_M0                              Fld(1, 1) //[1:1]
    #define LPIF_SEMA_0_SEMA_2_M0                              Fld(1, 2) //[2:2]
    #define LPIF_SEMA_0_SEMA_3_M0                              Fld(1, 3) //[3:3]
    #define LPIF_SEMA_0_SEMA_4_M0                              Fld(1, 4) //[4:4]
    #define LPIF_SEMA_0_SEMA_5_M0                              Fld(1, 5) //[5:5]
    #define LPIF_SEMA_0_SEMA_6_M0                              Fld(1, 6) //[6:6]
    #define LPIF_SEMA_0_SEMA_7_M0                              Fld(1, 7) //[7:7]
    #define LPIF_SEMA_0_RESERVED_X80_31_8                      Fld(24, 8) //[31:8]

#define DDRPHY_MD32_REG_LPIF_SEMA_1                            (DDRPHY_MD32_BASE_ADDRESS + 0x7084)
    #define LPIF_SEMA_1_SEMA_0_M1                              Fld(1, 0) //[0:0]
    #define LPIF_SEMA_1_SEMA_1_M1                              Fld(1, 1) //[1:1]
    #define LPIF_SEMA_1_SEMA_2_M1                              Fld(1, 2) //[2:2]
    #define LPIF_SEMA_1_SEMA_3_M1                              Fld(1, 3) //[3:3]
    #define LPIF_SEMA_1_SEMA_4_M1                              Fld(1, 4) //[4:4]
    #define LPIF_SEMA_1_SEMA_5_M1                              Fld(1, 5) //[5:5]
    #define LPIF_SEMA_1_SEMA_6_M1                              Fld(1, 6) //[6:6]
    #define LPIF_SEMA_1_SEMA_7_M1                              Fld(1, 7) //[7:7]
    #define LPIF_SEMA_1_RESERVED_X84_31_8                      Fld(24, 8) //[31:8]

#define DDRPHY_MD32_REG_LPIF_SEMA_2                            (DDRPHY_MD32_BASE_ADDRESS + 0x7088)
    #define LPIF_SEMA_2_SEMA_0_M2                              Fld(1, 0) //[0:0]
    #define LPIF_SEMA_2_SEMA_1_M2                              Fld(1, 1) //[1:1]
    #define LPIF_SEMA_2_SEMA_2_M2                              Fld(1, 2) //[2:2]
    #define LPIF_SEMA_2_SEMA_3_M2                              Fld(1, 3) //[3:3]
    #define LPIF_SEMA_2_SEMA_4_M2                              Fld(1, 4) //[4:4]
    #define LPIF_SEMA_2_SEMA_5_M2                              Fld(1, 5) //[5:5]
    #define LPIF_SEMA_2_SEMA_6_M2                              Fld(1, 6) //[6:6]
    #define LPIF_SEMA_2_SEMA_7_M2                              Fld(1, 7) //[7:7]
    #define LPIF_SEMA_2_RESERVED_X88_31_8                      Fld(24, 8) //[31:8]

#define DDRPHY_MD32_REG_LPIF_SEMA_3                            (DDRPHY_MD32_BASE_ADDRESS + 0x708C)
    #define LPIF_SEMA_3_SEMA_0_M3                              Fld(1, 0) //[0:0]
    #define LPIF_SEMA_3_SEMA_1_M3                              Fld(1, 1) //[1:1]
    #define LPIF_SEMA_3_SEMA_2_M3                              Fld(1, 2) //[2:2]
    #define LPIF_SEMA_3_SEMA_3_M3                              Fld(1, 3) //[3:3]
    #define LPIF_SEMA_3_SEMA_4_M3                              Fld(1, 4) //[4:4]
    #define LPIF_SEMA_3_SEMA_5_M3                              Fld(1, 5) //[5:5]
    #define LPIF_SEMA_3_SEMA_6_M3                              Fld(1, 6) //[6:6]
    #define LPIF_SEMA_3_SEMA_7_M3                              Fld(1, 7) //[7:7]
    #define LPIF_SEMA_3_RESERVED_X8C_31_8                      Fld(24, 8) //[31:8]

#define DDRPHY_MD32_REG_LPIF_RESERVED_16                       (DDRPHY_MD32_BASE_ADDRESS + 0x7090)
    #define LPIF_RESERVED_16_RESERVED_X90_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_17                       (DDRPHY_MD32_BASE_ADDRESS + 0x7094)
    #define LPIF_RESERVED_17_RESERVED_X94_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_18                       (DDRPHY_MD32_BASE_ADDRESS + 0x7098)
    #define LPIF_RESERVED_18_RESERVED_X98_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DUMMY_REG                         (DDRPHY_MD32_BASE_ADDRESS + 0x709C)
    #define LPIF_DUMMY_REG_DUMMY_REG                           Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_0                          (DDRPHY_MD32_BASE_ADDRESS + 0x70A0)
    #define LPIF_STATUS_0_LPIF_DDR_PST                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_1                          (DDRPHY_MD32_BASE_ADDRESS + 0x70A4)
    #define LPIF_STATUS_1_DDR_PST_REQ                          Fld(1, 0) //[0:0]
    #define LPIF_STATUS_1_DDR_PST_ABORT_REQ                    Fld(1, 1) //[1:1]
    #define LPIF_STATUS_1_DDR_PST_ABORT_REQ_LATCH              Fld(1, 2) //[2:2]
    #define LPIF_STATUS_1_LPC_INTERNAL_COUNTER_ABORT_FLAG      Fld(1, 3) //[3:3]
    #define LPIF_STATUS_1_RESERVED_XA4_31_4                    Fld(28, 4) //[31:4]

#define DDRPHY_MD32_REG_LPIF_STATUS_2                          (DDRPHY_MD32_BASE_ADDRESS + 0x70A8)
    #define LPIF_STATUS_2_DESTINATION_DDR_PST                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_3                          (DDRPHY_MD32_BASE_ADDRESS + 0x70AC)
    #define LPIF_STATUS_3_CUR_DDR_PST_STA                      Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_4                          (DDRPHY_MD32_BASE_ADDRESS + 0x70B0)
    #define LPIF_STATUS_4_EMI_CLK_OFF_REQ_ACK                  Fld(2, 0) //[1:0]
    #define LPIF_STATUS_4_RESERVED_XB0_3_2                     Fld(2, 2) //[3:2]
    #define LPIF_STATUS_4_DRAMC_DFS_STA                        Fld(4, 4) //[7:4]
    #define LPIF_STATUS_4_DQSSOC_REQ                           Fld(2, 8) //[9:8]
    #define LPIF_STATUS_4_RESERVED_XB0_11_10                   Fld(2, 10) //[11:10]
    #define LPIF_STATUS_4_DR_SHORT_QUEUE_ACK                   Fld(2, 12) //[13:12]
    #define LPIF_STATUS_4_DR_SHU_EN_ACK                        Fld(2, 14) //[15:14]
    #define LPIF_STATUS_4_DR_SRAM_PLL_LOAD_ACK                 Fld(2, 16) //[17:16]
    #define LPIF_STATUS_4_DR_SRAM_LOAD_ACK                     Fld(2, 18) //[19:18]
    #define LPIF_STATUS_4_DR_SRAM_RESTORE_ACK                  Fld(2, 20) //[21:20]
    #define LPIF_STATUS_4_TX_TRACKING_DIS_ACK                  Fld(2, 22) //[23:22]
    #define LPIF_STATUS_4_RESERVED_XB0_27_24                   Fld(4, 24) //[27:24]
    #define LPIF_STATUS_4_DDR_PST_ACK                          Fld(1, 28) //[28:28]
    #define LPIF_STATUS_4_DDR_PST_ABORT_ACK                    Fld(1, 29) //[29:29]
    #define LPIF_STATUS_4_RESERVED_XB0_31_30                   Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_5                          (DDRPHY_MD32_BASE_ADDRESS + 0x70B4)
    #define LPIF_STATUS_5_DDR_PST_STA_D0                       Fld(6, 0) //[5:0]
    #define LPIF_STATUS_5_DDR_PST_ACK_D0                       Fld(1, 6) //[6:6]
    #define LPIF_STATUS_5_DDR_PST_ABORT_ACK_D0                 Fld(1, 7) //[7:7]
    #define LPIF_STATUS_5_DDR_PST_STA_D1                       Fld(6, 8) //[13:8]
    #define LPIF_STATUS_5_DDR_PST_ACK_D1                       Fld(1, 14) //[14:14]
    #define LPIF_STATUS_5_DDR_PST_ABORT_ACK_D1                 Fld(1, 15) //[15:15]
    #define LPIF_STATUS_5_DDR_PST_STA_D2                       Fld(6, 16) //[21:16]
    #define LPIF_STATUS_5_DDR_PST_ACK_D2                       Fld(1, 22) //[22:22]
    #define LPIF_STATUS_5_DDR_PST_ABORT_ACK_D2                 Fld(1, 23) //[23:23]
    #define LPIF_STATUS_5_DDR_PST_STA_D3                       Fld(6, 24) //[29:24]
    #define LPIF_STATUS_5_DDR_PST_ACK_D3                       Fld(1, 30) //[30:30]
    #define LPIF_STATUS_5_DDR_PST_ABORT_ACK_D3                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_STATUS_6                          (DDRPHY_MD32_BASE_ADDRESS + 0x70B8)
    #define LPIF_STATUS_6_DDR_PST_STA_D4                       Fld(6, 0) //[5:0]
    #define LPIF_STATUS_6_DDR_PST_ACK_D4                       Fld(1, 6) //[6:6]
    #define LPIF_STATUS_6_DDR_PST_ABORT_ACK_D4                 Fld(1, 7) //[7:7]
    #define LPIF_STATUS_6_DDR_PST_STA_D5                       Fld(6, 8) //[13:8]
    #define LPIF_STATUS_6_DDR_PST_ACK_D5                       Fld(1, 14) //[14:14]
    #define LPIF_STATUS_6_DDR_PST_ABORT_ACK_D5                 Fld(1, 15) //[15:15]
    #define LPIF_STATUS_6_DDR_PST_STA_D6                       Fld(6, 16) //[21:16]
    #define LPIF_STATUS_6_DDR_PST_ACK_D6                       Fld(1, 22) //[22:22]
    #define LPIF_STATUS_6_DDR_PST_ABORT_ACK_D6                 Fld(1, 23) //[23:23]
    #define LPIF_STATUS_6_DDR_PST_STA_D7                       Fld(6, 24) //[29:24]
    #define LPIF_STATUS_6_DDR_PST_ACK_D7                       Fld(1, 30) //[30:30]
    #define LPIF_STATUS_6_DDR_PST_ABORT_ACK_D7                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_STATUS_7                          (DDRPHY_MD32_BASE_ADDRESS + 0x70BC)
    #define LPIF_STATUS_7_DDR_PST_STA_D8                       Fld(6, 0) //[5:0]
    #define LPIF_STATUS_7_DDR_PST_ACK_D8                       Fld(1, 6) //[6:6]
    #define LPIF_STATUS_7_DDR_PST_ABORT_ACK_D8                 Fld(1, 7) //[7:7]
    #define LPIF_STATUS_7_DDR_PST_STA_D9                       Fld(6, 8) //[13:8]
    #define LPIF_STATUS_7_DDR_PST_ACK_D9                       Fld(1, 14) //[14:14]
    #define LPIF_STATUS_7_DDR_PST_ABORT_ACK_D9                 Fld(1, 15) //[15:15]
    #define LPIF_STATUS_7_DDR_PST_STA_DA                       Fld(6, 16) //[21:16]
    #define LPIF_STATUS_7_DDR_PST_ACK_DA                       Fld(1, 22) //[22:22]
    #define LPIF_STATUS_7_DDR_PST_ABORT_ACK_DA                 Fld(1, 23) //[23:23]
    #define LPIF_STATUS_7_DDR_PST_STA_DB                       Fld(6, 24) //[29:24]
    #define LPIF_STATUS_7_DDR_PST_ACK_DB                       Fld(1, 30) //[30:30]
    #define LPIF_STATUS_7_DDR_PST_ABORT_ACK_DB                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_STATUS_8                          (DDRPHY_MD32_BASE_ADDRESS + 0x70C0)
    #define LPIF_STATUS_8_DDR_PST_STA_DC                       Fld(6, 0) //[5:0]
    #define LPIF_STATUS_8_DDR_PST_ACK_DC                       Fld(1, 6) //[6:6]
    #define LPIF_STATUS_8_DDR_PST_ABORT_ACK_DC                 Fld(1, 7) //[7:7]
    #define LPIF_STATUS_8_DDR_PST_STA_DD                       Fld(6, 8) //[13:8]
    #define LPIF_STATUS_8_DDR_PST_ACK_DD                       Fld(1, 14) //[14:14]
    #define LPIF_STATUS_8_DDR_PST_ABORT_ACK_DD                 Fld(1, 15) //[15:15]
    #define LPIF_STATUS_8_DDR_PST_STA_DE                       Fld(6, 16) //[21:16]
    #define LPIF_STATUS_8_DDR_PST_ACK_DE                       Fld(1, 22) //[22:22]
    #define LPIF_STATUS_8_DDR_PST_ABORT_ACK_DE                 Fld(1, 23) //[23:23]
    #define LPIF_STATUS_8_DDR_PST_STA_DF                       Fld(6, 24) //[29:24]
    #define LPIF_STATUS_8_DDR_PST_ACK_DF                       Fld(1, 30) //[30:30]
    #define LPIF_STATUS_8_DDR_PST_ABORT_ACK_DF                 Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_STATUS_9                          (DDRPHY_MD32_BASE_ADDRESS + 0x70C4)
    #define LPIF_STATUS_9_DRAMC_DMSUS_OFF                      Fld(2, 0) //[1:0]
    #define LPIF_STATUS_9_DRAMC_PHYPLL_EN                      Fld(2, 2) //[3:2]
    #define LPIF_STATUS_9_DRAMC_DPY_DLL_EN                     Fld(2, 4) //[5:4]
    #define LPIF_STATUS_9_DRAMC_DPY_2ND_DLL_EN                 Fld(2, 6) //[7:6]
    #define LPIF_STATUS_9_DRAMC_DPY_DLL_CK_EN                  Fld(2, 8) //[9:8]
    #define LPIF_STATUS_9_DRAMC_DPY_VREF_EN                    Fld(2, 10) //[11:10]
    #define LPIF_STATUS_9_DRAMC_EMI_CLK_OFF_REQ                Fld(2, 12) //[13:12]
    #define LPIF_STATUS_9_DRAMC_MEM_CK_OFF                     Fld(2, 14) //[15:14]
    #define LPIF_STATUS_9_DRAMC_DDRPHY_FB_CK_EN                Fld(2, 16) //[17:16]
    #define LPIF_STATUS_9_DRAMC_DR_GATE_RETRY_EN               Fld(2, 18) //[19:18]
    #define LPIF_STATUS_9_DRAMC_PHYPLL_SHU_EN                  Fld(2, 20) //[21:20]
    #define LPIF_STATUS_9_DRAMC_PHYPLL_MODE_SW                 Fld(2, 22) //[23:22]
    #define LPIF_STATUS_9_DRAMC_PHYPLL2_SHU_EN                 Fld(2, 24) //[25:24]
    #define LPIF_STATUS_9_DRAMC_PHYPLL2_MODE_SW                Fld(2, 26) //[27:26]
    #define LPIF_STATUS_9_DRAMC_DR_SHU_EN                      Fld(2, 28) //[29:28]
    #define LPIF_STATUS_9_DRAMC_DR_SHORT_QUEUE                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_10                         (DDRPHY_MD32_BASE_ADDRESS + 0x70C8)
    #define LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL                  Fld(4, 0) //[3:0]
    #define LPIF_STATUS_10_DRAMC_DPY_BCLK_ENABLE               Fld(2, 4) //[5:4]
    #define LPIF_STATUS_10_DRAMC_SHU_RESTORE                   Fld(2, 6) //[7:6]
    #define LPIF_STATUS_10_DRAMC_DPHY_PRECAL_UP                Fld(2, 8) //[9:8]
    #define LPIF_STATUS_10_DRAMC_DPHY_RXDLY_TRACK_EN           Fld(2, 10) //[11:10]
    #define LPIF_STATUS_10_DRAMC_DMY_EN_MOD_SEL                Fld(2, 12) //[13:12]
    #define LPIF_STATUS_10_DRAMC_DMYRD_INTV_SEL                Fld(2, 14) //[15:14]
    #define LPIF_STATUS_10_DRAMC_DMYRD_EN                      Fld(2, 16) //[17:16]
    #define LPIF_STATUS_10_DRAMC_TX_TRACKING_DIS               Fld(2, 18) //[19:18]
    #define LPIF_STATUS_10_DRAMC_TX_TRACKING_RETRY_EN          Fld(2, 20) //[21:20]
    #define LPIF_STATUS_10_DRAMC_DR_SHU_SRAM_LEVEL             Fld(8, 22) //[29:22]
    #define LPIF_STATUS_10_DRAMC_DR_SRAM_LOAD                  Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_11                         (DDRPHY_MD32_BASE_ADDRESS + 0x70CC)
    #define LPIF_STATUS_11_DRAMC_DR_SRAM_RESTORE               Fld(2, 0) //[1:0]
    #define LPIF_STATUS_11_DRAMC_DR_SHU_LEVEL_SRAM_LATCH       Fld(2, 2) //[3:2]
    #define LPIF_STATUS_11_DRAMC_DPY_MODE_SW                   Fld(2, 4) //[5:4]
    #define LPIF_STATUS_11_RESERVED_XCC_7_6                    Fld(2, 6) //[7:6]
    #define LPIF_STATUS_11_DRAMC_DPY_RESERVED                  Fld(8, 8) //[15:8]
    #define LPIF_STATUS_11_DRAMC_DRAMC_DFS_CON                 Fld(13, 16) //[28:16]
    #define LPIF_STATUS_11_RESERVED_XCC_31_27                  Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_STATUS_12                         (DDRPHY_MD32_BASE_ADDRESS + 0x70D0)
    #define LPIF_STATUS_12_FSM_TIME_OUT_FLAG                   Fld(1, 0) //[0:0]
    #define LPIF_STATUS_12_EXP_FSM_JUMP                        Fld(1, 1) //[1:1]
    #define LPIF_STATUS_12_IRQ_LPIF_LOW_POWER                  Fld(1, 2) //[2:2]
    #define LPIF_STATUS_12_IRQ_LPIF_OTHERS_STATE               Fld(1, 3) //[3:3]
    #define LPIF_STATUS_12_DFS_STATUS_RECORD                   Fld(4, 4) //[7:4]
    #define LPIF_STATUS_12_DVS_STATUS_RECORD                   Fld(1, 8) //[8:8]
    #define LPIF_STATUS_12_RUNTIME_STATUS_RECORD               Fld(1, 9) //[9:9]
    #define LPIF_STATUS_12_RESERVED_XD0_11_10                  Fld(2, 10) //[11:10]
    #define LPIF_STATUS_12_MUX_LPIF_DPHY_RXDLY_TRACK_EN        Fld(1, 12) //[12:12]
    #define LPIF_STATUS_12_MUX_LPIF_DMYRD_EN                   Fld(1, 13) //[13:13]
    #define LPIF_STATUS_12_MUX_LPIF_TX_TRACKING_DIS            Fld(1, 14) //[14:14]
    #define LPIF_STATUS_12_MUX_LPIF_DR_SRAM_RESTORE            Fld(1, 15) //[15:15]
    #define LPIF_STATUS_12_MUX_LPIF_TX_TRACK_RETRY_EN          Fld(1, 16) //[16:16]
    #define LPIF_STATUS_12_MUX_LPIF_RX_GATING_RETRY_EN         Fld(1, 17) //[17:17]
    #define LPIF_STATUS_12_MUX_LPIF_DLL_ALL_SLAVE_EN           Fld(1, 18) //[18:18]
    #define LPIF_STATUS_12_MUX_LPIF_IMPEDANCE_TRACKING_EN      Fld(1, 19) //[19:19]
    #define LPIF_STATUS_12_MUX_LPIF_DPHY_RXDLY_TRACK_EN_PREV   Fld(1, 20) //[20:20]
    #define LPIF_STATUS_12_MUX_LPIF_DMYRD_EN_PREV              Fld(1, 21) //[21:21]
    #define LPIF_STATUS_12_MUX_LPIF_TX_TRACKING_DIS_PREV       Fld(1, 22) //[22:22]
    #define LPIF_STATUS_12_MUX_LPIF_DR_SRAM_RESTORE_PREV       Fld(1, 23) //[23:23]
    #define LPIF_STATUS_12_MUX_LPIF_TX_TRACK_RETRY_EN_PREV     Fld(1, 24) //[24:24]
    #define LPIF_STATUS_12_MUX_LPIF_RX_GATING_RETRY_EN_PREV    Fld(1, 25) //[25:25]
    #define LPIF_STATUS_12_MUX_LPIF_DLL_ALL_SLAVE_EN_PREV      Fld(1, 26) //[26:26]
    #define LPIF_STATUS_12_MUX_LPIF_IMPEDANCE_TRACKING_EN_PREV Fld(1, 27) //[27:27]
    #define LPIF_STATUS_12_SHU_INDEX                           Fld(1, 28) //[28:28]
    #define LPIF_STATUS_12_RESERVED_XD0_31_29                  Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_STATUS_13                         (DDRPHY_MD32_BASE_ADDRESS + 0x70D4)
    #define LPIF_STATUS_13_COUNTER_TIME_OUT_FLAG               Fld(1, 0) //[0:0]
    #define LPIF_STATUS_13_COUNTER_20US_DONE                   Fld(1, 1) //[1:1]
    #define LPIF_STATUS_13_COUNTER_12US_DONE                   Fld(1, 2) //[2:2]
    #define LPIF_STATUS_13_COUNTER_DLL_1ST_LOCKING_DONE        Fld(1, 3) //[3:3]
    #define LPIF_STATUS_13_COUNTER_DLL_2ND_LOCKING_DONE        Fld(1, 4) //[4:4]
    #define LPIF_STATUS_13_COUNTER_CK_OFF_TO_DMSUS_DONE        Fld(1, 5) //[5:5]
    #define LPIF_STATUS_13_COUNTER_50NS_DONE                   Fld(1, 6) //[6:6]
    #define LPIF_STATUS_13_COUNTER_4US_DONE                    Fld(1, 7) //[7:7]
    #define LPIF_STATUS_13_LPIF_FSM                            Fld(10, 8) //[17:8]
    #define LPIF_STATUS_13_COUNTER_SR_DEBON_DONE               Fld(2, 18) //[19:18]
    #define LPIF_STATUS_13_COUNTER_SR_MIN_PLS_DEBON_DONE       Fld(2, 20) //[21:20]
    #define LPIF_STATUS_13_HW_S1_STEP_COUNTER_DONE             Fld(1, 22) //[22:22]
    #define LPIF_STATUS_13_COUNTER_256MCK_DONE                 Fld(1, 23) //[23:23]
    #define LPIF_STATUS_13_MUX_LPIF_DRAM_PAR_CLOCK_MODE        Fld(2, 24) //[25:24]
    #define LPIF_STATUS_13_MUX_LPIF_DRAM_PAR_CLOCK_MODE_PREV   Fld(2, 26) //[27:26]
    #define LPIF_STATUS_13_RESERVED_XD4_31_28                  Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_STATUS_14                         (DDRPHY_MD32_BASE_ADDRESS + 0x70D8)
    #define LPIF_STATUS_14_DRAMC_DPY_MCK8X_EN                  Fld(2, 0) //[1:0]
    #define LPIF_STATUS_14_DRAMC_DPY_MIDPI_EN                  Fld(2, 2) //[3:2]
    #define LPIF_STATUS_14_DRAMC_DPY_PI_RESETB_EN              Fld(2, 4) //[5:4]
    #define LPIF_STATUS_14_DRAMC_DVFS_MEM_CK_MUX_UPDATE        Fld(2, 6) //[7:6]
    #define LPIF_STATUS_14_DRAMC_DVFS_MEM_CK_MUX_SEL           Fld(4, 8) //[11:8]
    #define LPIF_STATUS_14_DRAMC_DPY_DSM_EN                    Fld(2, 12) //[13:12]
    #define LPIF_STATUS_14_DRAMC_DPY_FASTK_RDDQS_EN            Fld(2, 14) //[15:14]
    #define LPIF_STATUS_14_DRAMC_DPY_CS_PULL_UP_EN             Fld(2, 16) //[17:16]
    #define LPIF_STATUS_14_DRAMC_DPY_CS_PULL_DN_EN             Fld(2, 18) //[19:18]
    #define LPIF_STATUS_14_DRAMC_DPY_CA_PULL_UP_EN             Fld(2, 20) //[21:20]
    #define LPIF_STATUS_14_DRAMC_DPY_CA_PULL_DN_EN             Fld(2, 22) //[23:22]
    #define LPIF_STATUS_14_DRAMC_FHC_PAUSE_MEM                 Fld(1, 24) //[24:24]
    #define LPIF_STATUS_14_DRAMC_FHC_PAUSE_MPLL                Fld(1, 25) //[25:25]
    #define LPIF_STATUS_14_DRAMC_MPLL_S_OFF                    Fld(1, 26) //[26:26]
    #define LPIF_STATUS_14_DRAMC_MPLLOUT_OFF                   Fld(1, 27) //[27:27]
    #define LPIF_STATUS_14_DRAMC_EMI_S1_MODE_ASYNC             Fld(1, 28) //[28:28]
    #define LPIF_STATUS_14_RESERVED_XD8_29_29                  Fld(1, 29) //[29:29]
    #define LPIF_STATUS_14_DRAMC_DPY_PICG_FREE                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_RESERVED_20                       (DDRPHY_MD32_BASE_ADDRESS + 0x70DC)
    #define LPIF_RESERVED_20_RESERVED_XDC_31_0                 Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_15                         (DDRPHY_MD32_BASE_ADDRESS + 0x70E0)
    #define LPIF_STATUS_15_DRAMC_PWR_RST_B                     Fld(2, 0) //[1:0]
    #define LPIF_STATUS_15_DRAMC_PWR_ISO                       Fld(2, 2) //[3:2]
    #define LPIF_STATUS_15_DRAMC_PWR_ON                        Fld(2, 4) //[5:4]
    #define LPIF_STATUS_15_DRAMC_PWR_ON_2ND                    Fld(2, 6) //[7:6]
    #define LPIF_STATUS_15_DRAMC_PWR_CLK_DIS                   Fld(2, 8) //[9:8]
    #define LPIF_STATUS_15_DRAMC_MPLL_OFF                      Fld(1, 12) //[12:12]
    #define LPIF_STATUS_15_DRAMC_PWR_SRAM_PDN                  Fld(4, 16) //[19:16]
    #define LPIF_STATUS_15_DRAMC_PWR_SC_SRAM_PDN_ACK           Fld(1, 20) //[20:20]
    #define LPIF_STATUS_15_DRAMC_SHU_SRAM_SLEEP_B              Fld(2, 24) //[25:24]
    #define LPIF_STATUS_15_DRAMC_SHU_SRAM_CKISO                Fld(2, 26) //[27:26]
    #define LPIF_STATUS_15_DRAMC_SHU_SRAM_ISOINT_B             Fld(2, 28) //[29:28]
    #define LPIF_STATUS_15_DRAMC_SHU_SRAM_PDN                  Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_16                         (DDRPHY_MD32_BASE_ADDRESS + 0x70E4)
    #define LPIF_STATUS_16_SC_PWR_RST_B                        Fld(2, 0) //[1:0]
    #define LPIF_STATUS_16_SC_PWR_ISO                          Fld(2, 2) //[3:2]
    #define LPIF_STATUS_16_SC_PWR_ON                           Fld(2, 4) //[5:4]
    #define LPIF_STATUS_16_SC_PWR_ON_2ND                       Fld(2, 6) //[7:6]
    #define LPIF_STATUS_16_SC_PWR_CLK_DIS                      Fld(2, 8) //[9:8]
    #define LPIF_STATUS_16_RESERVED_XE4_11_10                  Fld(2, 10) //[11:10]
    #define LPIF_STATUS_16_SC_MPLL_OFF                         Fld(1, 12) //[12:12]
    #define LPIF_STATUS_16_RESERVED_XE4_15_13                  Fld(3, 13) //[15:13]
    #define LPIF_STATUS_16_SC_PWR_SRAM_PDN                     Fld(4, 16) //[19:16]
    #define LPIF_STATUS_16_RESERVED_XE4_23_20                  Fld(4, 20) //[23:20]
    #define LPIF_STATUS_16_SC_SHU_SRAM_SLEEP_B                 Fld(2, 24) //[25:24]
    #define LPIF_STATUS_16_RESERVED_XE4_26_26                  Fld(1, 26) //[26:26]
    #define LPIF_STATUS_16_SC_SHU_SRAM_CKISO                   Fld(1, 27) //[27:27]
    #define LPIF_STATUS_16_RESERVED_XE4_28_28                  Fld(1, 28) //[28:28]
    #define LPIF_STATUS_16_SC_SHU_SRAM_ISOINT_B                Fld(1, 29) //[29:29]
    #define LPIF_STATUS_16_SC_SHU_SRAM_PDN                     Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_17                         (DDRPHY_MD32_BASE_ADDRESS + 0x70E8)
    #define LPIF_STATUS_17_LPIF_PWR_RST_B                      Fld(2, 0) //[1:0]
    #define LPIF_STATUS_17_LPIF_PWR_ISO                        Fld(2, 2) //[3:2]
    #define LPIF_STATUS_17_LPIF_PWR_ON                         Fld(2, 4) //[5:4]
    #define LPIF_STATUS_17_LPIF_PWR_ON_2ND                     Fld(2, 6) //[7:6]
    #define LPIF_STATUS_17_LPIF_PWR_CLK_DIS                    Fld(2, 8) //[9:8]
    #define LPIF_STATUS_17_LPIF_MPLL_OFF                       Fld(1, 12) //[12:12]
    #define LPIF_STATUS_17_LPIF_PWR_SRAM_PDN                   Fld(8, 16) //[23:16]
    #define LPIF_STATUS_17_LPIF_SHU_SRAM_SLEEP_B               Fld(2, 24) //[25:24]
    #define LPIF_STATUS_17_LPIF_SHU_SRAM_CKISO                 Fld(2, 26) //[27:26]
    #define LPIF_STATUS_17_LPIF_SHU_SRAM_ISOINT_B              Fld(2, 28) //[29:28]
    #define LPIF_STATUS_17_LPIF_SHU_SRAM_PDN                   Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_STATUS_18                         (DDRPHY_MD32_BASE_ADDRESS + 0x70EC)
    #define LPIF_STATUS_18_DRAMC_LPIF_COM                      Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_STATUS_19                         (DDRPHY_MD32_BASE_ADDRESS + 0x70F0)
    #define LPIF_STATUS_19_DRAMC_LPIF_STA                      Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_MISC_CTL_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x70F4)
    #define LPIF_MISC_CTL_0_EMI_SLEEP_IDLE                     Fld(1, 0) //[0:0]

#define DDRPHY_MD32_REG_LPIF_MISC_CTL_1                        (DDRPHY_MD32_BASE_ADDRESS + 0x70F8)
    #define LPIF_MISC_CTL_1_PWR_RST_B                          Fld(2, 0) //[1:0]
    #define LPIF_MISC_CTL_1_PWR_ISO                            Fld(2, 2) //[3:2]
    #define LPIF_MISC_CTL_1_PWR_ON                             Fld(2, 4) //[5:4]
    #define LPIF_MISC_CTL_1_PWR_ON_2ND                         Fld(2, 6) //[7:6]
    #define LPIF_MISC_CTL_1_PWR_CLK_DIS                        Fld(2, 8) //[9:8]
    #define LPIF_MISC_CTL_1_MPLL_OFF                           Fld(1, 12) //[12:12]
    #define LPIF_MISC_CTL_1_PWR_SRAM_PDN                       Fld(8, 16) //[23:16]
    #define LPIF_MISC_CTL_1_SHU_SRAM_SLEEP_B                   Fld(2, 24) //[25:24]
    #define LPIF_MISC_CTL_1_SHU_SRAM_CKISO                     Fld(2, 26) //[27:26]
    #define LPIF_MISC_CTL_1_SHU_SRAM_ISOINT_B                  Fld(2, 28) //[29:28]
    #define LPIF_MISC_CTL_1_SHU_SRAM_PDN                       Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_MISC_STATUS_0                     (DDRPHY_MD32_BASE_ADDRESS + 0x70FC)
    #define LPIF_MISC_STATUS_0_PWR_ON_ACK                      Fld(2, 0) //[1:0]
    #define LPIF_MISC_STATUS_0_PWR_ON_2ND_ACK                  Fld(2, 2) //[3:2]
    #define LPIF_MISC_STATUS_0_SRAM_PDN_ACK                    Fld(4, 8) //[11:8]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x7100)
    #define LPIF_DVFS_CMD_0_DVFS_CMD0                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_1                        (DDRPHY_MD32_BASE_ADDRESS + 0x7104)
    #define LPIF_DVFS_CMD_1_DVFS_CMD1                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_2                        (DDRPHY_MD32_BASE_ADDRESS + 0x7108)
    #define LPIF_DVFS_CMD_2_DVFS_CMD2                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_3                        (DDRPHY_MD32_BASE_ADDRESS + 0x710C)
    #define LPIF_DVFS_CMD_3_DVFS_CMD3                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_4                        (DDRPHY_MD32_BASE_ADDRESS + 0x7110)
    #define LPIF_DVFS_CMD_4_DVFS_CMD4                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_5                        (DDRPHY_MD32_BASE_ADDRESS + 0x7114)
    #define LPIF_DVFS_CMD_5_DVFS_CMD5                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_6                        (DDRPHY_MD32_BASE_ADDRESS + 0x7118)
    #define LPIF_DVFS_CMD_6_DVFS_CMD6                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_7                        (DDRPHY_MD32_BASE_ADDRESS + 0x711C)
    #define LPIF_DVFS_CMD_7_DVFS_CMD7                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_8                        (DDRPHY_MD32_BASE_ADDRESS + 0x7120)
    #define LPIF_DVFS_CMD_8_DVFS_CMD8                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_9                        (DDRPHY_MD32_BASE_ADDRESS + 0x7124)
    #define LPIF_DVFS_CMD_9_DVFS_CMD9                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_10                       (DDRPHY_MD32_BASE_ADDRESS + 0x7128)
    #define LPIF_DVFS_CMD_10_DVFS_CMD10                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_11                       (DDRPHY_MD32_BASE_ADDRESS + 0x712C)
    #define LPIF_DVFS_CMD_11_DVFS_CMD11                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_12                       (DDRPHY_MD32_BASE_ADDRESS + 0x7130)
    #define LPIF_DVFS_CMD_12_DVFS_CMD12                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_13                       (DDRPHY_MD32_BASE_ADDRESS + 0x7134)
    #define LPIF_DVFS_CMD_13_DVFS_CMD13                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_14                       (DDRPHY_MD32_BASE_ADDRESS + 0x7138)
    #define LPIF_DVFS_CMD_14_DVFS_CMD14                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_15                       (DDRPHY_MD32_BASE_ADDRESS + 0x713C)
    #define LPIF_DVFS_CMD_15_DVFS_CMD15                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_16                       (DDRPHY_MD32_BASE_ADDRESS + 0x7140)
    #define LPIF_DVFS_CMD_16_DVFS_CMD16                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_17                       (DDRPHY_MD32_BASE_ADDRESS + 0x7144)
    #define LPIF_DVFS_CMD_17_DVFS_CMD17                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_18                       (DDRPHY_MD32_BASE_ADDRESS + 0x7148)
    #define LPIF_DVFS_CMD_18_DVFS_CMD18                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_19                       (DDRPHY_MD32_BASE_ADDRESS + 0x714C)
    #define LPIF_DVFS_CMD_19_DVFS_CMD19                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_20                       (DDRPHY_MD32_BASE_ADDRESS + 0x7150)
    #define LPIF_DVFS_CMD_20_DVFS_CMD20                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_21                       (DDRPHY_MD32_BASE_ADDRESS + 0x7154)
    #define LPIF_DVFS_CMD_21_DVFS_CMD21                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_22                       (DDRPHY_MD32_BASE_ADDRESS + 0x7158)
    #define LPIF_DVFS_CMD_22_DVFS_CMD22                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CMD_23                       (DDRPHY_MD32_BASE_ADDRESS + 0x715C)
    #define LPIF_DVFS_CMD_23_DVFS_CMD23                        Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_CONFIG                       (DDRPHY_MD32_BASE_ADDRESS + 0x7160)
    #define LPIF_DVFS_CONFIG_DVFS_CON                          Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_STATUS_0                     (DDRPHY_MD32_BASE_ADDRESS + 0x7164)
    #define LPIF_DVFS_STATUS_0_DVFS_CMD_ACK                    Fld(1, 0) //[0:0]
    #define LPIF_DVFS_STATUS_0_DVFS_CMD_REQ                    Fld(1, 1) //[1:1]

#define DDRPHY_MD32_REG_LPIF_DVFS_STATUS_1                     (DDRPHY_MD32_BASE_ADDRESS + 0x7168)
    #define LPIF_DVFS_STATUS_1_DVFS_CMD_DAT                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DVFS_STATUS_2                     (DDRPHY_MD32_BASE_ADDRESS + 0x716C)
    #define LPIF_DVFS_STATUS_2_DVFS_CON_STA                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_170                      (DDRPHY_MD32_BASE_ADDRESS + 0x7170)
    #define LPIF_RESERVED_170_RESERVED_X170_31_0               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_174                      (DDRPHY_MD32_BASE_ADDRESS + 0x7174)
    #define LPIF_RESERVED_174_RESERVED_X174_31_0               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_178                      (DDRPHY_MD32_BASE_ADDRESS + 0x7178)
    #define LPIF_RESERVED_178_RESERVED_X178_31_0               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_RESERVED_17C                      (DDRPHY_MD32_BASE_ADDRESS + 0x717C)
    #define LPIF_RESERVED_17C_RESERVED_X17C_31_0               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0               (DDRPHY_MD32_BASE_ADDRESS + 0x7180)
    #define LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1               (DDRPHY_MD32_BASE_ADDRESS + 0x7184)
    #define LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_2               (DDRPHY_MD32_BASE_ADDRESS + 0x7188)
    #define LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_3               (DDRPHY_MD32_BASE_ADDRESS + 0x718C)
    #define LPIF_MR_OP_STORE_SHU_0_3_MR_OP_SET_SHU_0_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_0_3_MR_OP_SET_SHU_0_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_0_3_MR_OP_SET_SHU_0_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_0_3_MR_OP_SET_SHU_0_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_1_0               (DDRPHY_MD32_BASE_ADDRESS + 0x7190)
    #define LPIF_MR_OP_STORE_SHU_1_0_MR_OP_SET_SHU_1_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_1_0_MR_OP_SET_SHU_1_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_1_0_MR_OP_SET_SHU_1_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_1_0_MR_OP_SET_SHU_1_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_1_1               (DDRPHY_MD32_BASE_ADDRESS + 0x7194)
    #define LPIF_MR_OP_STORE_SHU_1_1_MR_OP_SET_SHU_1_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_1_1_MR_OP_SET_SHU_1_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_1_1_MR_OP_SET_SHU_1_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_1_1_MR_OP_SET_SHU_1_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_1_2               (DDRPHY_MD32_BASE_ADDRESS + 0x7198)
    #define LPIF_MR_OP_STORE_SHU_1_2_MR_OP_SET_SHU_1_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_1_2_MR_OP_SET_SHU_1_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_1_2_MR_OP_SET_SHU_1_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_1_2_MR_OP_SET_SHU_1_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_1_3               (DDRPHY_MD32_BASE_ADDRESS + 0x719C)
    #define LPIF_MR_OP_STORE_SHU_1_3_MR_OP_SET_SHU_1_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_1_3_MR_OP_SET_SHU_1_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_1_3_MR_OP_SET_SHU_1_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_1_3_MR_OP_SET_SHU_1_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_2_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71A0)
    #define LPIF_MR_OP_STORE_SHU_2_0_MR_OP_SET_SHU_2_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_2_0_MR_OP_SET_SHU_2_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_2_0_MR_OP_SET_SHU_2_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_2_0_MR_OP_SET_SHU_2_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_2_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71A4)
    #define LPIF_MR_OP_STORE_SHU_2_1_MR_OP_SET_SHU_2_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_2_1_MR_OP_SET_SHU_2_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_2_1_MR_OP_SET_SHU_2_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_2_1_MR_OP_SET_SHU_2_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_2_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71A8)
    #define LPIF_MR_OP_STORE_SHU_2_2_MR_OP_SET_SHU_2_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_2_2_MR_OP_SET_SHU_2_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_2_2_MR_OP_SET_SHU_2_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_2_2_MR_OP_SET_SHU_2_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_2_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71AC)
    #define LPIF_MR_OP_STORE_SHU_2_3_MR_OP_SET_SHU_2_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_2_3_MR_OP_SET_SHU_2_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_2_3_MR_OP_SET_SHU_2_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_2_3_MR_OP_SET_SHU_2_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_3_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71B0)
    #define LPIF_MR_OP_STORE_SHU_3_0_MR_OP_SET_SHU_3_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_3_0_MR_OP_SET_SHU_3_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_3_0_MR_OP_SET_SHU_3_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_3_0_MR_OP_SET_SHU_3_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_3_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71B4)
    #define LPIF_MR_OP_STORE_SHU_3_1_MR_OP_SET_SHU_3_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_3_1_MR_OP_SET_SHU_3_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_3_1_MR_OP_SET_SHU_3_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_3_1_MR_OP_SET_SHU_3_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_3_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71B8)
    #define LPIF_MR_OP_STORE_SHU_3_2_MR_OP_SET_SHU_3_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_3_2_MR_OP_SET_SHU_3_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_3_2_MR_OP_SET_SHU_3_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_3_2_MR_OP_SET_SHU_3_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_3_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71BC)
    #define LPIF_MR_OP_STORE_SHU_3_3_MR_OP_SET_SHU_3_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_3_3_MR_OP_SET_SHU_3_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_3_3_MR_OP_SET_SHU_3_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_3_3_MR_OP_SET_SHU_3_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_4_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71C0)
    #define LPIF_MR_OP_STORE_SHU_4_0_MR_OP_SET_SHU_4_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_4_0_MR_OP_SET_SHU_4_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_4_0_MR_OP_SET_SHU_4_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_4_0_MR_OP_SET_SHU_4_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_4_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71C4)
    #define LPIF_MR_OP_STORE_SHU_4_1_MR_OP_SET_SHU_4_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_4_1_MR_OP_SET_SHU_4_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_4_1_MR_OP_SET_SHU_4_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_4_1_MR_OP_SET_SHU_4_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_4_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71C8)
    #define LPIF_MR_OP_STORE_SHU_4_2_MR_OP_SET_SHU_4_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_4_2_MR_OP_SET_SHU_4_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_4_2_MR_OP_SET_SHU_4_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_4_2_MR_OP_SET_SHU_4_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_4_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71CC)
    #define LPIF_MR_OP_STORE_SHU_4_3_MR_OP_SET_SHU_4_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_4_3_MR_OP_SET_SHU_4_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_4_3_MR_OP_SET_SHU_4_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_4_3_MR_OP_SET_SHU_4_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_5_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71D0)
    #define LPIF_MR_OP_STORE_SHU_5_0_MR_OP_SET_SHU_5_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_5_0_MR_OP_SET_SHU_5_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_5_0_MR_OP_SET_SHU_5_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_5_0_MR_OP_SET_SHU_5_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_5_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71D4)
    #define LPIF_MR_OP_STORE_SHU_5_1_MR_OP_SET_SHU_5_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_5_1_MR_OP_SET_SHU_5_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_5_1_MR_OP_SET_SHU_5_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_5_1_MR_OP_SET_SHU_5_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_5_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71D8)
    #define LPIF_MR_OP_STORE_SHU_5_2_MR_OP_SET_SHU_5_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_5_2_MR_OP_SET_SHU_5_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_5_2_MR_OP_SET_SHU_5_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_5_2_MR_OP_SET_SHU_5_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_5_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71DC)
    #define LPIF_MR_OP_STORE_SHU_5_3_MR_OP_SET_SHU_5_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_5_3_MR_OP_SET_SHU_5_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_5_3_MR_OP_SET_SHU_5_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_5_3_MR_OP_SET_SHU_5_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_6_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71E0)
    #define LPIF_MR_OP_STORE_SHU_6_0_MR_OP_SET_SHU_6_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_6_0_MR_OP_SET_SHU_6_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_6_0_MR_OP_SET_SHU_6_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_6_0_MR_OP_SET_SHU_6_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_6_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71E4)
    #define LPIF_MR_OP_STORE_SHU_6_1_MR_OP_SET_SHU_6_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_6_1_MR_OP_SET_SHU_6_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_6_1_MR_OP_SET_SHU_6_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_6_1_MR_OP_SET_SHU_6_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_6_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71E8)
    #define LPIF_MR_OP_STORE_SHU_6_2_MR_OP_SET_SHU_6_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_6_2_MR_OP_SET_SHU_6_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_6_2_MR_OP_SET_SHU_6_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_6_2_MR_OP_SET_SHU_6_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_6_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71EC)
    #define LPIF_MR_OP_STORE_SHU_6_3_MR_OP_SET_SHU_6_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_6_3_MR_OP_SET_SHU_6_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_6_3_MR_OP_SET_SHU_6_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_6_3_MR_OP_SET_SHU_6_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_0               (DDRPHY_MD32_BASE_ADDRESS + 0x71F0)
    #define LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_1               (DDRPHY_MD32_BASE_ADDRESS + 0x71F4)
    #define LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_2               (DDRPHY_MD32_BASE_ADDRESS + 0x71F8)
    #define LPIF_MR_OP_STORE_SHU_7_2_MR_OP_SET_SHU_7_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_7_2_MR_OP_SET_SHU_7_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_7_2_MR_OP_SET_SHU_7_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_7_2_MR_OP_SET_SHU_7_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_3               (DDRPHY_MD32_BASE_ADDRESS + 0x71FC)
    #define LPIF_MR_OP_STORE_SHU_7_3_MR_OP_SET_SHU_7_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_7_3_MR_OP_SET_SHU_7_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_7_3_MR_OP_SET_SHU_7_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_7_3_MR_OP_SET_SHU_7_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_8_0               (DDRPHY_MD32_BASE_ADDRESS + 0x7200)
    #define LPIF_MR_OP_STORE_SHU_8_0_MR_OP_SET_SHU_8_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_8_0_MR_OP_SET_SHU_8_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_8_0_MR_OP_SET_SHU_8_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_8_0_MR_OP_SET_SHU_8_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_8_1               (DDRPHY_MD32_BASE_ADDRESS + 0x7204)
    #define LPIF_MR_OP_STORE_SHU_8_1_MR_OP_SET_SHU_8_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_8_1_MR_OP_SET_SHU_8_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_8_1_MR_OP_SET_SHU_8_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_8_1_MR_OP_SET_SHU_8_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_8_2               (DDRPHY_MD32_BASE_ADDRESS + 0x7208)
    #define LPIF_MR_OP_STORE_SHU_8_2_MR_OP_SET_SHU_8_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_8_2_MR_OP_SET_SHU_8_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_8_2_MR_OP_SET_SHU_8_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_8_2_MR_OP_SET_SHU_8_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_8_3               (DDRPHY_MD32_BASE_ADDRESS + 0x720C)
    #define LPIF_MR_OP_STORE_SHU_8_3_MR_OP_SET_SHU_8_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_8_3_MR_OP_SET_SHU_8_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_8_3_MR_OP_SET_SHU_8_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_8_3_MR_OP_SET_SHU_8_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_9_0               (DDRPHY_MD32_BASE_ADDRESS + 0x7210)
    #define LPIF_MR_OP_STORE_SHU_9_0_MR_OP_SET_SHU_9_0         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_9_0_MR_OP_SET_SHU_9_1         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_9_0_MR_OP_SET_SHU_9_2         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_9_0_MR_OP_SET_SHU_9_3         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_9_1               (DDRPHY_MD32_BASE_ADDRESS + 0x7214)
    #define LPIF_MR_OP_STORE_SHU_9_1_MR_OP_SET_SHU_9_4         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_9_1_MR_OP_SET_SHU_9_5         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_9_1_MR_OP_SET_SHU_9_6         Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_9_1_MR_OP_SET_SHU_9_7         Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_9_2               (DDRPHY_MD32_BASE_ADDRESS + 0x7218)
    #define LPIF_MR_OP_STORE_SHU_9_2_MR_OP_SET_SHU_9_8         Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_9_2_MR_OP_SET_SHU_9_9         Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_9_2_MR_OP_SET_SHU_9_10        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_9_2_MR_OP_SET_SHU_9_11        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_9_3               (DDRPHY_MD32_BASE_ADDRESS + 0x721C)
    #define LPIF_MR_OP_STORE_SHU_9_3_MR_OP_SET_SHU_9_12        Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_9_3_MR_OP_SET_SHU_9_13        Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_9_3_MR_OP_SET_SHU_9_14        Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_9_3_MR_OP_SET_SHU_9_15        Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_10_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7220)
    #define LPIF_MR_OP_STORE_SHU_10_0_MR_OP_SET_SHU_10_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_10_0_MR_OP_SET_SHU_10_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_10_0_MR_OP_SET_SHU_10_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_10_0_MR_OP_SET_SHU_10_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_10_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7224)
    #define LPIF_MR_OP_STORE_SHU_10_1_MR_OP_SET_SHU_10_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_10_1_MR_OP_SET_SHU_10_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_10_1_MR_OP_SET_SHU_10_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_10_1_MR_OP_SET_SHU_10_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_10_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7228)
    #define LPIF_MR_OP_STORE_SHU_10_2_MR_OP_SET_SHU_10_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_10_2_MR_OP_SET_SHU_10_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_10_2_MR_OP_SET_SHU_10_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_10_2_MR_OP_SET_SHU_10_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_10_3              (DDRPHY_MD32_BASE_ADDRESS + 0x722C)
    #define LPIF_MR_OP_STORE_SHU_10_3_MR_OP_SET_SHU_10_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_10_3_MR_OP_SET_SHU_10_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_10_3_MR_OP_SET_SHU_10_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_10_3_MR_OP_SET_SHU_10_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_11_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7230)
    #define LPIF_MR_OP_STORE_SHU_11_0_MR_OP_SET_SHU_11_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_11_0_MR_OP_SET_SHU_11_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_11_0_MR_OP_SET_SHU_11_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_11_0_MR_OP_SET_SHU_11_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_11_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7234)
    #define LPIF_MR_OP_STORE_SHU_11_1_MR_OP_SET_SHU_11_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_11_1_MR_OP_SET_SHU_11_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_11_1_MR_OP_SET_SHU_11_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_11_1_MR_OP_SET_SHU_11_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_11_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7238)
    #define LPIF_MR_OP_STORE_SHU_11_2_MR_OP_SET_SHU_11_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_11_2_MR_OP_SET_SHU_11_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_11_2_MR_OP_SET_SHU_11_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_11_2_MR_OP_SET_SHU_11_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_11_3              (DDRPHY_MD32_BASE_ADDRESS + 0x723C)
    #define LPIF_MR_OP_STORE_SHU_11_3_MR_OP_SET_SHU_11_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_11_3_MR_OP_SET_SHU_11_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_11_3_MR_OP_SET_SHU_11_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_11_3_MR_OP_SET_SHU_11_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_12_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7240)
    #define LPIF_MR_OP_STORE_SHU_12_0_MR_OP_SET_SHU_12_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_12_0_MR_OP_SET_SHU_12_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_12_0_MR_OP_SET_SHU_12_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_12_0_MR_OP_SET_SHU_12_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_12_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7244)
    #define LPIF_MR_OP_STORE_SHU_12_1_MR_OP_SET_SHU_12_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_12_1_MR_OP_SET_SHU_12_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_12_1_MR_OP_SET_SHU_12_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_12_1_MR_OP_SET_SHU_12_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_12_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7248)
    #define LPIF_MR_OP_STORE_SHU_12_2_MR_OP_SET_SHU_12_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_12_2_MR_OP_SET_SHU_12_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_12_2_MR_OP_SET_SHU_12_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_12_2_MR_OP_SET_SHU_12_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_12_3              (DDRPHY_MD32_BASE_ADDRESS + 0x724C)
    #define LPIF_MR_OP_STORE_SHU_12_3_MR_OP_SET_SHU_12_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_12_3_MR_OP_SET_SHU_12_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_12_3_MR_OP_SET_SHU_12_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_12_3_MR_OP_SET_SHU_12_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_13_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7250)
    #define LPIF_MR_OP_STORE_SHU_13_0_MR_OP_SET_SHU_13_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_13_0_MR_OP_SET_SHU_13_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_13_0_MR_OP_SET_SHU_13_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_13_0_MR_OP_SET_SHU_13_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_13_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7254)
    #define LPIF_MR_OP_STORE_SHU_13_1_MR_OP_SET_SHU_13_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_13_1_MR_OP_SET_SHU_13_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_13_1_MR_OP_SET_SHU_13_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_13_1_MR_OP_SET_SHU_13_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_13_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7258)
    #define LPIF_MR_OP_STORE_SHU_13_2_MR_OP_SET_SHU_13_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_13_2_MR_OP_SET_SHU_13_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_13_2_MR_OP_SET_SHU_13_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_13_2_MR_OP_SET_SHU_13_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_13_3              (DDRPHY_MD32_BASE_ADDRESS + 0x725C)
    #define LPIF_MR_OP_STORE_SHU_13_3_MR_OP_SET_SHU_13_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_13_3_MR_OP_SET_SHU_13_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_13_3_MR_OP_SET_SHU_13_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_13_3_MR_OP_SET_SHU_13_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_14_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7260)
    #define LPIF_MR_OP_STORE_SHU_14_0_MR_OP_SET_SHU_14_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_14_0_MR_OP_SET_SHU_14_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_14_0_MR_OP_SET_SHU_14_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_14_0_MR_OP_SET_SHU_14_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_14_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7264)
    #define LPIF_MR_OP_STORE_SHU_14_1_MR_OP_SET_SHU_14_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_14_1_MR_OP_SET_SHU_14_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_14_1_MR_OP_SET_SHU_14_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_14_1_MR_OP_SET_SHU_14_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_14_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7268)
    #define LPIF_MR_OP_STORE_SHU_14_2_MR_OP_SET_SHU_14_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_14_2_MR_OP_SET_SHU_14_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_14_2_MR_OP_SET_SHU_14_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_14_2_MR_OP_SET_SHU_14_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_14_3              (DDRPHY_MD32_BASE_ADDRESS + 0x726C)
    #define LPIF_MR_OP_STORE_SHU_14_3_MR_OP_SET_SHU_14_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_14_3_MR_OP_SET_SHU_14_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_14_3_MR_OP_SET_SHU_14_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_14_3_MR_OP_SET_SHU_14_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_0              (DDRPHY_MD32_BASE_ADDRESS + 0x7270)
    #define LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_0       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_1       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_2       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_3       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_1              (DDRPHY_MD32_BASE_ADDRESS + 0x7274)
    #define LPIF_MR_OP_STORE_SHU_15_1_MR_OP_SET_SHU_15_4       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_15_1_MR_OP_SET_SHU_15_5       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_15_1_MR_OP_SET_SHU_15_6       Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_15_1_MR_OP_SET_SHU_15_7       Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_2              (DDRPHY_MD32_BASE_ADDRESS + 0x7278)
    #define LPIF_MR_OP_STORE_SHU_15_2_MR_OP_SET_SHU_15_8       Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_15_2_MR_OP_SET_SHU_15_9       Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_15_2_MR_OP_SET_SHU_15_10      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_15_2_MR_OP_SET_SHU_15_11      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_3              (DDRPHY_MD32_BASE_ADDRESS + 0x727C)
    #define LPIF_MR_OP_STORE_SHU_15_3_MR_OP_SET_SHU_15_12      Fld(8, 0) //[7:0]
    #define LPIF_MR_OP_STORE_SHU_15_3_MR_OP_SET_SHU_15_13      Fld(8, 8) //[15:8]
    #define LPIF_MR_OP_STORE_SHU_15_3_MR_OP_SET_SHU_15_14      Fld(8, 16) //[23:16]
    #define LPIF_MR_OP_STORE_SHU_15_3_MR_OP_SET_SHU_15_15      Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_0                        (DDRPHY_MD32_BASE_ADDRESS + 0x7280)
    #define LPIF_INT_PSTA_0_PSTA_CLK_26M_EN                    Fld(1, 0) //[0:0]
    #define LPIF_INT_PSTA_0_PSTA_IN_SYNC_BYPASS                Fld(1, 1) //[1:1]
    #define LPIF_INT_PSTA_0_PSTA_TEST_TRIGGER_EN               Fld(1, 2) //[2:2]
    #define LPIF_INT_PSTA_0_PTA_ABORT_CASE_EN                  Fld(1, 3) //[3:3]
    #define LPIF_INT_PSTA_0_PSTA_ABORT_TIME_RAND_EN            Fld(1, 4) //[4:4]
    #define LPIF_INT_PSTA_0_PSTA_PST_TO_REQ_TIME_RAND_EN       Fld(1, 5) //[5:5]
    #define LPIF_INT_PSTA_0_PSTA_REQ_TO_NXT_PST_TIME_RAND_EN   Fld(1, 6) //[6:6]
    #define LPIF_INT_PSTA_0_PSTA_ACK_TO_PST_REQ_LOW_TIME_RAND_EN Fld(1, 7) //[7:7]
    #define LPIF_INT_PSTA_0_PSTA_LOOP_MODE_ENABLE              Fld(1, 8) //[8:8]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CLR                      Fld(1, 9) //[9:9]
    #define LPIF_INT_PSTA_0_RESERVED_XA0_11_10                 Fld(2, 10) //[11:10]
    #define LPIF_INT_PSTA_0_PSTA_HW_S1_TEST_EN                 Fld(1, 12) //[12:12]
    #define LPIF_INT_PSTA_0_PSTA_HW_S1_HIGH_PERIOD_TIME_RAND_EN Fld(1, 13) //[13:13]
    #define LPIF_INT_PSTA_0_PSTA_HW_S1_REQ_INTV_TIME_RAND_EN   Fld(1, 14) //[14:14]
    #define LPIF_INT_PSTA_0_PSTA_HW_S1_BYPASS_LOW_ACK_CHK      Fld(1, 15) //[15:15]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_0_EN                 Fld(1, 16) //[16:16]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_1_EN                 Fld(1, 17) //[17:17]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_2_EN                 Fld(1, 18) //[18:18]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_3_EN                 Fld(1, 19) //[19:19]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_4_EN                 Fld(1, 20) //[20:20]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_5_EN                 Fld(1, 21) //[21:21]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_6_EN                 Fld(1, 22) //[22:22]
    #define LPIF_INT_PSTA_0_PSTA_TEST_CND_7_EN                 Fld(1, 23) //[23:23]
    #define LPIF_INT_PSTA_0_RESERVED_XA0_31_24                 Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_1                        (DDRPHY_MD32_BASE_ADDRESS + 0x7284)
    #define LPIF_INT_PSTA_1_PSTA_LOOP_MODE_TIME                Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_2                        (DDRPHY_MD32_BASE_ADDRESS + 0x7288)
    #define LPIF_INT_PSTA_2_PSTA_ABORT_TIME                    Fld(10, 0) //[9:0]
    #define LPIF_INT_PSTA_2_PSTA_ABORT_TIME_MIN                Fld(10, 10) //[19:10]
    #define LPIF_INT_PSTA_2_PSTA_ABORT_TIME_MAX                Fld(10, 20) //[29:20]
    #define LPIF_INT_PSTA_2_RESERVED_XA2_31_30                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_3                        (DDRPHY_MD32_BASE_ADDRESS + 0x728C)
    #define LPIF_INT_PSTA_3_PSTA_PST_TO_REQ_TIME               Fld(4, 0) //[3:0]
    #define LPIF_INT_PSTA_3_PSTA_PST_TO_REQ_TIME_MIN           Fld(4, 4) //[7:4]
    #define LPIF_INT_PSTA_3_PSTA_PST_TO_REQ_TIME_MAX           Fld(4, 8) //[11:8]
    #define LPIF_INT_PSTA_3_RESERVED_XA3_15_12                 Fld(4, 12) //[15:12]
    #define LPIF_INT_PSTA_3_PSTA_ACK_TO_PST_REQ_LOW_TIME       Fld(4, 16) //[19:16]
    #define LPIF_INT_PSTA_3_PSTA_ACK_TO_PST_REQ_LOW_TIME_MIN   Fld(4, 20) //[23:20]
    #define LPIF_INT_PSTA_3_PSTA_ACK_TO_PST_REQ_LOW_TIME_MAX   Fld(4, 24) //[27:24]
    #define LPIF_INT_PSTA_3_RESERVED_XA3_31_28                 Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_4                        (DDRPHY_MD32_BASE_ADDRESS + 0x7290)
    #define LPIF_INT_PSTA_4_PSTA_REQ_TO_NXT_PST_TIME           Fld(8, 0) //[7:0]
    #define LPIF_INT_PSTA_4_PSTA_REQ_TO_NXT_PST_TIME_MIN       Fld(8, 8) //[15:8]
    #define LPIF_INT_PSTA_4_PSTA_REQ_TO_NXT_PST_TIME_MAX       Fld(8, 16) //[23:16]
    #define LPIF_INT_PSTA_4_RESERVED_XA4_31_24                 Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_5                        (DDRPHY_MD32_BASE_ADDRESS + 0x7294)
    #define LPIF_INT_PSTA_5_PSTA_CMD_PST_0                     Fld(5, 0) //[4:0]
    #define LPIF_INT_PSTA_5_RESERVED_XA5_7_5                   Fld(3, 5) //[7:5]
    #define LPIF_INT_PSTA_5_PSTA_CMD_PST_1                     Fld(5, 8) //[12:8]
    #define LPIF_INT_PSTA_5_RESERVED_XA5_15_13                 Fld(3, 13) //[15:13]
    #define LPIF_INT_PSTA_5_PSTA_CMD_PST_2                     Fld(5, 16) //[20:16]
    #define LPIF_INT_PSTA_5_RESERVED_XA5_23_21                 Fld(3, 21) //[23:21]
    #define LPIF_INT_PSTA_5_PSTA_CMD_PST_3                     Fld(5, 24) //[28:24]
    #define LPIF_INT_PSTA_5_RESERVED_XA5_31_29                 Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_6                        (DDRPHY_MD32_BASE_ADDRESS + 0x7298)
    #define LPIF_INT_PSTA_6_PSTA_CMD_PST_4                     Fld(5, 0) //[4:0]
    #define LPIF_INT_PSTA_6_RESERVED_XA6_7_5                   Fld(3, 5) //[7:5]
    #define LPIF_INT_PSTA_6_PSTA_CMD_PST_5                     Fld(5, 8) //[12:8]
    #define LPIF_INT_PSTA_6_RESERVED_XA6_15_13                 Fld(3, 13) //[15:13]
    #define LPIF_INT_PSTA_6_PSTA_CMD_PST_6                     Fld(5, 16) //[20:16]
    #define LPIF_INT_PSTA_6_RESERVED_XA6_23_21                 Fld(3, 21) //[23:21]
    #define LPIF_INT_PSTA_6_PSTA_CMD_PST_7                     Fld(5, 24) //[28:24]
    #define LPIF_INT_PSTA_6_RESERVED_XA6_31_29                 Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_7                        (DDRPHY_MD32_BASE_ADDRESS + 0x729C)
    #define LPIF_INT_PSTA_7_PSTA_ABORT_TIME_LSFR_POL           Fld(20, 0) //[19:0]
    #define LPIF_INT_PSTA_7_RESERVED_XA7_23_20                 Fld(4, 20) //[23:20]
    #define LPIF_INT_PSTA_7_PSTA_ACK_TO_PST_REQ_LOW_TIME_LSFR_POL Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_8                        (DDRPHY_MD32_BASE_ADDRESS + 0x72A0)
    #define LPIF_INT_PSTA_8_PSTA_REQ_TO_NXT_PST_TIME_LSFR_POL  Fld(16, 0) //[15:0]
    #define LPIF_INT_PSTA_8_PSTA_PST_TO_REQ_TIME_LSFR_POL      Fld(8, 16) //[23:16]
    #define LPIF_INT_PSTA_8_RESERVED_XA8_31_24                 Fld(8, 24) //[31:24]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_9                        (DDRPHY_MD32_BASE_ADDRESS + 0x72A4)
    #define LPIF_INT_PSTA_9_PSTA_DDR_PST                       Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_10                       (DDRPHY_MD32_BASE_ADDRESS + 0x72A8)
    #define LPIF_INT_PSTA_10_PSTA_DDR_PST_REQ                  Fld(1, 0) //[0:0]
    #define LPIF_INT_PSTA_10_PSTA_DDR_PST_ABORT_REQ            Fld(1, 1) //[1:1]
    #define LPIF_INT_PSTA_10_RESERVED_XAA_3_2                  Fld(2, 2) //[3:2]
    #define LPIF_INT_PSTA_10_PSTA_DDR_FSM_DONE                 Fld(1, 4) //[4:4]
    #define LPIF_INT_PSTA_10_PSTA_DDR_HW_S1_ACK                Fld(1, 5) //[5:5]
    #define LPIF_INT_PSTA_10_PSTA_FSM                          Fld(5, 8) //[12:8]
    #define LPIF_INT_PSTA_10_CNT_PSTA_FSM                      Fld(12, 16) //[27:16]
    #define LPIF_INT_PSTA_10_RESERVED_XAA_31_28                Fld(4, 28) //[31:28]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_11                       (DDRPHY_MD32_BASE_ADDRESS + 0x72AC)
    #define LPIF_INT_PSTA_11_PSTA_HW_S1_REQ_INTV_TIME          Fld(10, 0) //[9:0]
    #define LPIF_INT_PSTA_11_PSTA_HW_S1_REQ_INTV_TIME_MIN      Fld(10, 10) //[19:10]
    #define LPIF_INT_PSTA_11_PSTA_HW_S1_REQ_INTV_TIME_MAX      Fld(10, 20) //[29:20]
    #define LPIF_INT_PSTA_11_RESERVED_XAB_31_30                Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_12                       (DDRPHY_MD32_BASE_ADDRESS + 0x72B0)
    #define LPIF_INT_PSTA_12_PSTA_HW_S1_HIGH_PERIOD_TIME       Fld(10, 0) //[9:0]
    #define LPIF_INT_PSTA_12_PSTA_HW_S1_HIGH_PERIOD_TIME_MIN   Fld(10, 10) //[19:10]
    #define LPIF_INT_PSTA_12_PSTA_HW_S1_HIGH_PERIOD_TIME_MAX   Fld(10, 20) //[29:20]
    #define LPIF_INT_PSTA_12_RESERVED_XAC_31_30                Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_13                       (DDRPHY_MD32_BASE_ADDRESS + 0x72B4)
    #define LPIF_INT_PSTA_13_PSTA_HW_S1_REQ_INTV_TIME_LSFR_POL Fld(20, 0) //[19:0]
    #define LPIF_INT_PSTA_13_RESERVED_XAD_31_30                Fld(12, 20) //[31:20]

#define DDRPHY_MD32_REG_LPIF_INT_PSTA_14                       (DDRPHY_MD32_BASE_ADDRESS + 0x72B8)
    #define LPIF_INT_PSTA_14_PSTA_HW_S1_HIGH_PERIOD_TIME_LSFR_POL Fld(20, 0) //[19:0]
    #define LPIF_INT_PSTA_14_RESERVED_XAE_31_30                Fld(12, 20) //[31:20]

#define DDRPHY_MD32_REG_LPIF_HW_S1_0                           (DDRPHY_MD32_BASE_ADDRESS + 0x72C0)
    #define LPIF_HW_S1_0_HW_S1_PS_CHK_RESULT_CLR               Fld(1, 0) //[0:0]
    #define LPIF_HW_S1_0_HW_S1_LATCH_CLR                       Fld(1, 1) //[1:1]
    #define LPIF_HW_S1_0_HW_S1_TRIG_BY_FSM_EN                  Fld(1, 2) //[2:2]
    #define LPIF_HW_S1_0_HW_S1_TRIG_BY_IRQ_EN                  Fld(1, 3) //[3:3]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DMSUS_OFF                Fld(1, 4) //[4:4]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_MODE_SW              Fld(1, 5) //[5:5]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_DLL_EN               Fld(1, 6) //[6:6]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_2ND_DLL_EN           Fld(1, 7) //[7:7]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_DLL_CK_EN            Fld(1, 8) //[8:8]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_VREF_EN              Fld(1, 9) //[9:9]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_EMI_CLK_OFF_REQ          Fld(1, 10) //[10:10]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_MEM_CK_OFF               Fld(1, 11) //[11:11]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DDRPHY_FB_CK_EN          Fld(1, 12) //[12:12]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_TX_TRACKING_DIS          Fld(1, 13) //[13:13]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_FHC_PAUSE_MPLL           Fld(1, 14) //[14:14]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_FHC_PAUSE_MEM            Fld(1, 15) //[15:15]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_MCK8X_EN             Fld(1, 16) //[16:16]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_MIDPI_EN             Fld(1, 17) //[17:17]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_PI_RESETB_EN         Fld(1, 18) //[18:18]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DR_GATE_RETRY_EN         Fld(1, 19) //[19:19]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPHY_PRECAL_UP           Fld(1, 20) //[20:20]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_TX_TRACKING_RETRY_EN     Fld(1, 21) //[21:21]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_CS_PULL_DN_EN        Fld(1, 22) //[22:22]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_EMI_S1_MODE_ASYNC    Fld(1, 23) //[23:23]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_CA_PULL_DN_EN        Fld(1, 24) //[24:24]
    #define LPIF_HW_S1_0_HW_S1_BYPASS_DPY_PICG_FREE            Fld(1, 25) //[25:25]
    #define LPIF_HW_S1_0_RESERVED_XB0_31_26                    Fld(6, 26) //[31:26]

#define DDRPHY_MD32_REG_LPIF_HW_S1_1                           (DDRPHY_MD32_BASE_ADDRESS + 0x72C4)
    #define LPIF_HW_S1_1_HW_S1_PST_CHK                         Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_HW_S1_2                           (DDRPHY_MD32_BASE_ADDRESS + 0x72C8)
    #define LPIF_HW_S1_2_HW_S1_PST_CHK_RESULT                  Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_HW_S1_3                           (DDRPHY_MD32_BASE_ADDRESS + 0x72CC)
    #define LPIF_HW_S1_3_HW_S1_REQ                             Fld(1, 0) //[0:0]
    #define LPIF_HW_S1_3_HW_S1_ACK                             Fld(1, 1) //[1:1]
    #define LPIF_HW_S1_3_HW_S1_PST_REQ_CHK_RESULT              Fld(1, 2) //[2:2]
    #define LPIF_HW_S1_3_RESERVED_XB0_3_3                      Fld(1, 3) //[3:3]
    #define LPIF_HW_S1_3_RISING_HW_S1_REQ_LATCH                Fld(1, 4) //[4:4]
    #define LPIF_HW_S1_3_FALLING_HW_S1_REQ_LATCH               Fld(1, 5) //[5:5]
    #define LPIF_HW_S1_3_HW_S1_FSM                             Fld(6, 6) //[11:6]
    #define LPIF_HW_S1_3_RESERVED_XB3_31_12                    Fld(20, 12) //[31:12]

#define DDRPHY_MD32_REG_LPIF_HW_S1_4                           (DDRPHY_MD32_BASE_ADDRESS + 0x72D0)
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_PI_RESETB_EN          Fld(2, 0) //[1:0]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_MIDPI_EN              Fld(2, 2) //[3:2]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_MCK8X_EN              Fld(2, 4) //[5:4]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_TX_TRACKING_RETRY_EN      Fld(2, 6) //[7:6]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_TX_TRACKING_DIS           Fld(2, 8) //[9:8]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPHY_PRECAL_UP            Fld(2, 10) //[11:10]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DR_GATE_RETRY_EN          Fld(2, 12) //[13:12]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DDRPHY_FB_CK_EN           Fld(2, 14) //[15:14]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_MEM_CK_OFF                Fld(2, 16) //[17:16]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_EMI_CLK_OFF_REQ           Fld(2, 18) //[19:18]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_VREF_EN               Fld(2, 20) //[21:20]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_DLL_CK_EN             Fld(2, 22) //[23:22]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_2ND_DLL_EN            Fld(2, 24) //[25:24]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_DLL_EN                Fld(2, 26) //[27:26]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DPY_MODE_SW               Fld(2, 28) //[29:28]
    #define LPIF_HW_S1_4_HW_S1_DRAMC_DMSUS_OFF                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_HW_S1_5                           (DDRPHY_MD32_BASE_ADDRESS + 0x72D4)
    #define LPIF_HW_S1_5_HW_S1_DRAMC_FHC_PAUSE_MPLL            Fld(1, 0) //[0:0]
    #define LPIF_HW_S1_5_HW_S1_DRAMC_FHC_PAUSE_MEM             Fld(1, 1) //[1:1]
    #define LPIF_HW_S1_5_HW_S1_DRAMC_DPY_CS_PULL_DN_EN         Fld(2, 2) //[3:2]
    #define LPIF_HW_S1_5_HW_S1_DRAMC_EMI_S1_MODE_ASYNC         Fld(1, 4) //[4:4]
    #define LPIF_HW_S1_5_HW_S1_DRAMC_DPY_CA_PULL_DN_EN         Fld(2, 5) //[6:5]
    #define LPIF_HW_S1_5_HW_S1_DRAMC_DPY_PICG_FREE             Fld(2, 8) //[9:8]
    #define LPIF_HW_S1_5_RESERVED_XB5_31_10                    Fld(22, 10) //[31:10]

#define DDRPHY_MD32_REG_LPIF_HW_S1_6                           (DDRPHY_MD32_BASE_ADDRESS + 0x72D8)
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DMSUS_OFF                Fld(1, 0) //[0:0]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_MODE_SW              Fld(1, 1) //[1:1]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_DLL_EN               Fld(1, 2) //[2:2]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_2ND_DLL_EN           Fld(1, 3) //[3:3]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_DLL_CK_EN            Fld(1, 4) //[4:4]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_VREF_EN              Fld(1, 5) //[5:5]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_EMI_CLK_OFF_REQ          Fld(1, 6) //[6:6]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_MEM_CK_OFF               Fld(1, 7) //[7:7]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DDRPHY_FB_CK_EN          Fld(1, 8) //[8:8]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_TX_TRACKING_DIS          Fld(1, 9) //[9:9]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_FHC_PAUSE_MPLL           Fld(1, 10) //[10:10]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_FHC_PAUSE_MEM            Fld(1, 11) //[11:11]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_MCK8X_EN             Fld(1, 12) //[12:12]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_MIDPI_EN             Fld(1, 13) //[13:13]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_PI_RESETB_EN         Fld(1, 14) //[14:14]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DR_GATE_RETRY_EN         Fld(1, 15) //[15:15]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPHY_PRECAL_UP           Fld(1, 16) //[16:16]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_TX_TRACKING_RETRY_EN     Fld(1, 17) //[17:17]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_CS_PULL_DN_EN        Fld(1, 18) //[18:18]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_CA_PULL_DN_EN        Fld(1, 19) //[19:19]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_EMI_S1_MODE_ASYNC        Fld(1, 20) //[20:20]
    #define LPIF_HW_S1_6_LOG_OPT_HWS1_DPY_PICG_FREE            Fld(1, 21) //[21:21]
    #define LPIF_HW_S1_6_RESERVED_XB6_31_22                    Fld(10, 22) //[31:22]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH0                        (DDRPHY_MD32_BASE_ADDRESS + 0x7380)
    #define LPIF_DBG_LATCH0_LPIF_DDR_PST                       Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH1                        (DDRPHY_MD32_BASE_ADDRESS + 0x7384)
    #define LPIF_DBG_LATCH1_CUR_LPIF_DDR_PST_STA               Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH2                        (DDRPHY_MD32_BASE_ADDRESS + 0x7388)
    #define LPIF_DBG_LATCH2_EMI_CLK_OFF_REQ_ACK                Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH2_RESERVED_XE2_3_2                   Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH2_DRAMC_DFS_STA                      Fld(4, 4) //[7:4]
    #define LPIF_DBG_LATCH2_DQSSOC_REQ                         Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH2_RESERVED_XE2_11_10                 Fld(2, 10) //[11:10]
    #define LPIF_DBG_LATCH2_DR_SHORT_QUEUE_ACK                 Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH2_DR_SHU_EN_ACK                      Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH2_DR_SRAM_PLL_LOAD_ACK               Fld(2, 16) //[17:16]
    #define LPIF_DBG_LATCH2_DR_SRAM_LOAD_ACK                   Fld(2, 18) //[19:18]
    #define LPIF_DBG_LATCH2_DR_SRAM_RESTORE_ACK                Fld(2, 20) //[21:20]
    #define LPIF_DBG_LATCH2_TX_TRACKING_DIS_ACK                Fld(2, 22) //[23:22]
    #define LPIF_DBG_LATCH2_DDR_PST_ACK                        Fld(1, 24) //[24:24]
    #define LPIF_DBG_LATCH2_DDR_PST_ABORT_ACK                  Fld(1, 25) //[25:25]
    #define LPIF_DBG_LATCH2_EMI_SLEEP_IDLE                     Fld(1, 26) //[26:26]
    #define LPIF_DBG_LATCH2_EMI_SLEEP_PROT_EN                  Fld(1, 27) //[27:27]
    #define LPIF_DBG_LATCH2_DDR_PST_REQ                        Fld(1, 28) //[28:28]
    #define LPIF_DBG_LATCH2_DDR_PST_ABORT_REQ                  Fld(1, 29) //[29:29]
    #define LPIF_DBG_LATCH2_DDR_PST_ABORT_REQ_LATCH            Fld(1, 30) //[30:30]
    #define LPIF_DBG_LATCH2_LPC_INTERNAL_COUNTER_ABORT_FLAG    Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH3                        (DDRPHY_MD32_BASE_ADDRESS + 0x738C)
    #define LPIF_DBG_LATCH3_DDR_PST_STA_D0                     Fld(6, 0) //[5:0]
    #define LPIF_DBG_LATCH3_DDR_PST_ACK_D0                     Fld(1, 6) //[6:6]
    #define LPIF_DBG_LATCH3_DDR_PST_ABORT_ACK_D0               Fld(1, 7) //[7:7]
    #define LPIF_DBG_LATCH3_DDR_PST_STA_D1                     Fld(6, 8) //[13:8]
    #define LPIF_DBG_LATCH3_DDR_PST_ACK_D1                     Fld(1, 14) //[14:14]
    #define LPIF_DBG_LATCH3_DDR_PST_ABORT_ACK_D1               Fld(1, 15) //[15:15]
    #define LPIF_DBG_LATCH3_DDR_PST_STA_D2                     Fld(6, 16) //[21:16]
    #define LPIF_DBG_LATCH3_DDR_PST_ACK_D2                     Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH3_DDR_PST_ABORT_ACK_D2               Fld(1, 23) //[23:23]
    #define LPIF_DBG_LATCH3_DDR_PST_STA_D3                     Fld(6, 24) //[29:24]
    #define LPIF_DBG_LATCH3_DDR_PST_ACK_D3                     Fld(1, 30) //[30:30]
    #define LPIF_DBG_LATCH3_DDR_PST_ABORT_ACK_D3               Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH4                        (DDRPHY_MD32_BASE_ADDRESS + 0x7390)
    #define LPIF_DBG_LATCH4_DDR_PST_STA_D4                     Fld(6, 0) //[5:0]
    #define LPIF_DBG_LATCH4_DDR_PST_ACK_D4                     Fld(1, 6) //[6:6]
    #define LPIF_DBG_LATCH4_DDR_PST_ABORT_ACK_D4               Fld(1, 7) //[7:7]
    #define LPIF_DBG_LATCH4_DDR_PST_STA_D5                     Fld(6, 8) //[13:8]
    #define LPIF_DBG_LATCH4_DDR_PST_ACK_D5                     Fld(1, 14) //[14:14]
    #define LPIF_DBG_LATCH4_DDR_PST_ABORT_ACK_D5               Fld(1, 15) //[15:15]
    #define LPIF_DBG_LATCH4_DDR_PST_STA_D6                     Fld(6, 16) //[21:16]
    #define LPIF_DBG_LATCH4_DDR_PST_ACK_D6                     Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH4_DDR_PST_ABORT_ACK_D6               Fld(1, 23) //[23:23]
    #define LPIF_DBG_LATCH4_DDR_PST_STA_D7                     Fld(6, 24) //[29:24]
    #define LPIF_DBG_LATCH4_DDR_PST_ACK_D7                     Fld(1, 30) //[30:30]
    #define LPIF_DBG_LATCH4_DDR_PST_ABORT_ACK_D7               Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH5                        (DDRPHY_MD32_BASE_ADDRESS + 0x7394)
    #define LPIF_DBG_LATCH5_DDR_PST_STA_D8                     Fld(6, 0) //[5:0]
    #define LPIF_DBG_LATCH5_DDR_PST_ACK_D8                     Fld(1, 6) //[6:6]
    #define LPIF_DBG_LATCH5_DDR_PST_ABORT_ACK_D8               Fld(1, 7) //[7:7]
    #define LPIF_DBG_LATCH5_DDR_PST_STA_D9                     Fld(6, 8) //[13:8]
    #define LPIF_DBG_LATCH5_DDR_PST_ACK_D9                     Fld(1, 14) //[14:14]
    #define LPIF_DBG_LATCH5_DDR_PST_ABORT_ACK_D9               Fld(1, 15) //[15:15]
    #define LPIF_DBG_LATCH5_DDR_PST_STA_DA                     Fld(6, 16) //[21:16]
    #define LPIF_DBG_LATCH5_DDR_PST_ACK_DA                     Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH5_DDR_PST_ABORT_ACK_DA               Fld(1, 23) //[23:23]
    #define LPIF_DBG_LATCH5_DDR_PST_STA_DB                     Fld(6, 24) //[29:24]
    #define LPIF_DBG_LATCH5_DDR_PST_ACK_DB                     Fld(1, 30) //[30:30]
    #define LPIF_DBG_LATCH5_DDR_PST_ABORT_ACK_DB               Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH6                        (DDRPHY_MD32_BASE_ADDRESS + 0x7398)
    #define LPIF_DBG_LATCH6_DDR_PST_STA_DC                     Fld(6, 0) //[5:0]
    #define LPIF_DBG_LATCH6_DDR_PST_ACK_DC                     Fld(1, 6) //[6:6]
    #define LPIF_DBG_LATCH6_DDR_PST_ABORT_ACK_DC               Fld(1, 7) //[7:7]
    #define LPIF_DBG_LATCH6_DDR_PST_STA_DD                     Fld(6, 8) //[13:8]
    #define LPIF_DBG_LATCH6_DDR_PST_ACK_DD                     Fld(1, 14) //[14:14]
    #define LPIF_DBG_LATCH6_DDR_PST_ABORT_ACK_DD               Fld(1, 15) //[15:15]
    #define LPIF_DBG_LATCH6_DDR_PST_STA_DE                     Fld(6, 16) //[21:16]
    #define LPIF_DBG_LATCH6_DDR_PST_ACK_DE                     Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH6_DDR_PST_ABORT_ACK_DE               Fld(1, 23) //[23:23]
    #define LPIF_DBG_LATCH6_DDR_PST_STA_DF                     Fld(6, 24) //[29:24]
    #define LPIF_DBG_LATCH6_DDR_PST_ACK_DF                     Fld(1, 30) //[30:30]
    #define LPIF_DBG_LATCH6_DDR_PST_ABORT_ACK_DF               Fld(1, 31) //[31:31]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH7                        (DDRPHY_MD32_BASE_ADDRESS + 0x739C)
    #define LPIF_DBG_LATCH7_DRAMC_DMSUS_OFF                    Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH7_DRAMC_PHYPLL_EN                    Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH7_DRAMC_DPY_DLL_EN                   Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH7_DRAMC_DPY_2ND_DLL_EN               Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH7_DRAMC_DPY_DLL_CK_EN                Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH7_DRAMC_DPY_VREF_EN                  Fld(2, 10) //[11:10]
    #define LPIF_DBG_LATCH7_DRAMC_EMI_CLK_OFF_REQ              Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH7_DRAMC_MEM_CK_OFF                   Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH7_DRAMC_DDRPHY_FB_CK_EN              Fld(2, 16) //[17:16]
    #define LPIF_DBG_LATCH7_DRAMC_DR_GATE_RETRY_EN             Fld(2, 18) //[19:18]
    #define LPIF_DBG_LATCH7_DRAMC_PHYPLL_SHU_EN                Fld(2, 20) //[21:20]
    #define LPIF_DBG_LATCH7_DRAMC_PHYPLL_MODE_SW               Fld(2, 22) //[23:22]
    #define LPIF_DBG_LATCH7_DRAMC_PHYPLL2_SHU_EN               Fld(2, 24) //[25:24]
    #define LPIF_DBG_LATCH7_DRAMC_PHYPLL2_MODE_SW              Fld(2, 26) //[27:26]
    #define LPIF_DBG_LATCH7_DRAMC_DR_SHU_EN                    Fld(2, 28) //[29:28]
    #define LPIF_DBG_LATCH7_DRAMC_DR_SHORT_QUEUE               Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH8                        (DDRPHY_MD32_BASE_ADDRESS + 0x73A0)
    #define LPIF_DBG_LATCH8_DRAMC_DR_SHU_LEVEL                 Fld(4, 0) //[3:0]
    #define LPIF_DBG_LATCH8_DRAMC_DPY_BCLK_ENABLE              Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH8_DRAMC_SHU_RESTORE                  Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH8_DRAMC_DPHY_PRECAL_UP               Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH8_DRAMC_DPHY_RXDLY_TRACK_EN          Fld(2, 10) //[11:10]
    #define LPIF_DBG_LATCH8_DRAMC_DMY_EN_MOD_SEL               Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH8_DRAMC_DMYRD_INTV_SEL               Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH8_DRAMC_DMYRD_EN                     Fld(2, 16) //[17:16]
    #define LPIF_DBG_LATCH8_DRAMC_TX_TRACKING_DIS              Fld(2, 18) //[19:18]
    #define LPIF_DBG_LATCH8_DRAMC_TX_TRACKING_RETRY_EN         Fld(2, 20) //[21:20]
    #define LPIF_DBG_LATCH8_DRAMC_DR_SHU_SRAM_LEVEL            Fld(8, 22) //[29:22]
    #define LPIF_DBG_LATCH8_DRAMC_DR_SRAM_LOAD                 Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH9                        (DDRPHY_MD32_BASE_ADDRESS + 0x73A4)
    #define LPIF_DBG_LATCH9_DRAMC_DPY_MCK8X_EN                 Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_MIDPI_EN                 Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_PI_RESETB_EN             Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH9_DRAMC_DVFS_MEM_CK_MUX_UPDATE       Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH9_DRAMC_DVFS_MEM_CK_MUX_SEL          Fld(4, 8) //[11:8]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_DSM_EN                   Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_FASTK_RDDQS_EN           Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_CS_PULL_UP_EN            Fld(2, 16) //[17:16]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_CS_PULL_DN_EN            Fld(2, 18) //[19:18]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_CA_PULL_UP_EN            Fld(2, 20) //[21:20]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_CA_PULL_DN_EN            Fld(2, 22) //[23:22]
    #define LPIF_DBG_LATCH9_DRAMC_FHC_PAUSE_MEM                Fld(1, 24) //[24:24]
    #define LPIF_DBG_LATCH9_DRAMC_FHC_PAUSE_MPLL               Fld(1, 25) //[25:25]
    #define LPIF_DBG_LATCH9_DRAMC_MPLL_S_OFF                   Fld(1, 26) //[26:26]
    #define LPIF_DBG_LATCH9_DRAMC_MPLLOUT_OFF                  Fld(1, 27) //[27:27]
    #define LPIF_DBG_LATCH9_DRAMC_EMI_S1_MODE_ASYNC            Fld(1, 28) //[28:28]
    #define LPIF_DBG_LATCH9_RESERVED_XE9_29_29                 Fld(1, 29) //[29:29]
    #define LPIF_DBG_LATCH9_DRAMC_DPY_PICG_FREE                Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH10                       (DDRPHY_MD32_BASE_ADDRESS + 0x73A8)
    #define LPIF_DBG_LATCH10_DRAMC_DR_SRAM_RESTORE             Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH10_DRAMC_DR_SHU_LEVEL_SRAM_LATCH     Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH10_DRAMC_DPY_MODE_SW                 Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH10_RESERVED_XEA_7_6                  Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH10_DRAMC_DPY_RESERVED                Fld(8, 8) //[15:8]
    #define LPIF_DBG_LATCH10_DRAMC_DRAMC_DFS_CON               Fld(13, 16) //[28:16]
    #define LPIF_DBG_LATCH10_RESERVED_XEA_31_29                Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH11                       (DDRPHY_MD32_BASE_ADDRESS + 0x73AC)
    #define LPIF_DBG_LATCH11_FSM_TIME_OUT_FLAG                 Fld(1, 0) //[0:0]
    #define LPIF_DBG_LATCH11_EXP_FSM_JUMP                      Fld(1, 1) //[1:1]
    #define LPIF_DBG_LATCH11_IRQ_LPIF_LOW_POWER                Fld(1, 2) //[2:2]
    #define LPIF_DBG_LATCH11_IRQ_LPIF_OTHERS_STATE             Fld(1, 3) //[3:3]
    #define LPIF_DBG_LATCH11_DFS_STATUS_RECORD                 Fld(4, 4) //[7:4]
    #define LPIF_DBG_LATCH11_DVS_STATUS_RECORD                 Fld(1, 8) //[8:8]
    #define LPIF_DBG_LATCH11_RUNTIME_STATUS_RECORD             Fld(1, 9) //[9:9]
    #define LPIF_DBG_LATCH11_RESERVED_XEB_11_10                Fld(2, 10) //[11:10]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DPHY_RXDLY_TRACK_EN      Fld(1, 12) //[12:12]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DMYRD_EN                 Fld(1, 13) //[13:13]
    #define LPIF_DBG_LATCH11_MUX_LPIF_TX_TRACKING_DIS          Fld(1, 14) //[14:14]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DR_SRAM_RESTORE          Fld(1, 15) //[15:15]
    #define LPIF_DBG_LATCH11_MUX_LPIF_TX_TRACK_RETRY_EN        Fld(1, 16) //[16:16]
    #define LPIF_DBG_LATCH11_MUX_LPIF_RX_GATING_RETRY_EN       Fld(1, 17) //[17:17]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DLL_ALL_SLAVE_EN         Fld(1, 18) //[18:18]
    #define LPIF_DBG_LATCH11_MUX_LPIF_IMPEDANCE_TRACKING_EN    Fld(1, 19) //[19:19]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DPHY_RXDLY_TRACK_EN_PREV Fld(1, 20) //[20:20]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DMYRD_EN_PREV            Fld(1, 21) //[21:21]
    #define LPIF_DBG_LATCH11_MUX_LPIF_TX_TRACKING_DIS_PREV     Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DR_SRAM_RESTORE_PREV     Fld(1, 23) //[23:23]
    #define LPIF_DBG_LATCH11_MUX_LPIF_TX_TRACK_RETRY_EN_PREV   Fld(1, 24) //[24:24]
    #define LPIF_DBG_LATCH11_MUX_LPIF_RX_GATING_RETRY_EN_PREV  Fld(1, 25) //[25:25]
    #define LPIF_DBG_LATCH11_MUX_LPIF_DLL_ALL_SLAVE_EN_PREV    Fld(1, 26) //[26:26]
    #define LPIF_DBG_LATCH11_MUX_LPIF_IMPEDANCE_TRACKING_EN_PREV Fld(1, 27) //[27:27]
    #define LPIF_DBG_LATCH11_SHU_INDEX                         Fld(1, 28) //[28:28]
    #define LPIF_DBG_LATCH11_RESERVED_XEB_31_29                Fld(3, 29) //[31:29]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH12                       (DDRPHY_MD32_BASE_ADDRESS + 0x73B0)
    #define LPIF_DBG_LATCH12_DRAMC_PWR_RST_B                   Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_ISO                     Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_ON                      Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_ON_2ND                  Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_CLK_DIS                 Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH12_DRAMC_MPLL_OFF                    Fld(1, 12) //[12:12]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_SRAM_PDN                Fld(4, 16) //[19:16]
    #define LPIF_DBG_LATCH12_DRAMC_PWR_SC_SRAM_PDN_ACK         Fld(1, 20) //[20:20]
    #define LPIF_DBG_LATCH12_DRAMC_SHU_SRAM_SLEEP_B            Fld(2, 24) //[25:24]
    #define LPIF_DBG_LATCH12_DRAMC_SHU_SRAM_CKISO              Fld(2, 26) //[27:26]
    #define LPIF_DBG_LATCH12_DRAMC_SHU_SRAM_ISOINT_B           Fld(2, 28) //[29:28]
    #define LPIF_DBG_LATCH12_DRAMC_SHU_SRAM_PDN                Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH13                       (DDRPHY_MD32_BASE_ADDRESS + 0x73B4)
    #define LPIF_DBG_LATCH13_DRAMC_LPIF_COM                    Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH14                       (DDRPHY_MD32_BASE_ADDRESS + 0x73B8)
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_PI_RESETB_EN      Fld(2, 0) //[1:0]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_MIDPI_EN          Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_MCK8X_EN          Fld(2, 4) //[5:4]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_TX_TRACKING_RETRY_EN  Fld(2, 6) //[7:6]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_TX_TRACKING_DIS       Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPHY_PRECAL_UP        Fld(2, 10) //[11:10]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DR_GATE_RETRY_EN      Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DDRPHY_FB_CK_EN       Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_MEM_CK_OFF            Fld(2, 16) //[17:16]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_EMI_CLK_OFF_REQ       Fld(2, 18) //[19:18]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_VREF_EN           Fld(2, 20) //[21:20]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_DLL_CK_EN         Fld(2, 22) //[23:22]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_2ND_DLL_EN        Fld(2, 24) //[25:24]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_DLL_EN            Fld(2, 26) //[27:26]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DPY_MODE_SW           Fld(2, 28) //[29:28]
    #define LPIF_DBG_LATCH14_HW_S1_DRAMC_DMSUS_OFF             Fld(2, 30) //[31:30]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH15                       (DDRPHY_MD32_BASE_ADDRESS + 0x73BC)
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_FHC_PAUSE_MPLL        Fld(1, 0) //[0:0]
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_FHC_PAUSE_MEM         Fld(1, 1) //[1:1]
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_DPY_CS_PULL_DN_EN     Fld(2, 2) //[3:2]
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_EMI_S1_MODE_ASYNC     Fld(1, 4) //[4:4]
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_DPY_CA_PULL_DN_EN     Fld(2, 5) //[6:5]
    #define LPIF_DBG_LATCH15_HW_S1_DRAMC_DPY_PICG_FREE         Fld(2, 8) //[9:8]
    #define LPIF_DBG_LATCH15_PWR_ON_ACK                        Fld(2, 12) //[13:12]
    #define LPIF_DBG_LATCH15_PWR_ON_2ND_ACK                    Fld(2, 14) //[15:14]
    #define LPIF_DBG_LATCH15_SRAM_PDN_ACK                      Fld(4, 16) //[19:16]
    #define LPIF_DBG_LATCH15_HW_S1_REQ                         Fld(1, 20) //[20:20]
    #define LPIF_DBG_LATCH15_HW_S1_ACK                         Fld(1, 21) //[21:21]
    #define LPIF_DBG_LATCH15_HW_S1_PST_REQ_CHK_RESULT          Fld(1, 22) //[22:22]
    #define LPIF_DBG_LATCH15_RESERVED_XEF_31_23                Fld(9, 23) //[31:23]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH16                       (DDRPHY_MD32_BASE_ADDRESS + 0x73C0)
    #define LPIF_DBG_LATCH16_HW_S1_PST_CHK_RESULT              Fld(32, 0) //[31:0]

#define DDRPHY_MD32_REG_LPIF_DBG_LATCH17                       (DDRPHY_MD32_BASE_ADDRESS + 0x73C4)
    #define LPIF_DBG_LATCH17_MAX_CNT_SREF_REQ_HIGH_TO_SREF_ACK Fld(8, 0) //[7:0]
    #define LPIF_DBG_LATCH17_MAX_CNT_SREF_REQ_LOW_TO_SREF_ACK  Fld(8, 8) //[15:8]
    #define LPIF_DBG_LATCH17_MAX_CNT_SHU_EN_HIGH_TO_ACK        Fld(8, 16) //[23:16]
    #define LPIF_DBG_LATCH17_MAX_CNT_HW_S1_REQ_LOW_TO_SREF_ACK_LOW Fld(8, 24) //[31:24]

#endif // __DDRPHY_MD32_REGS_H__
