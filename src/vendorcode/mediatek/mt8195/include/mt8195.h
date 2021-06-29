/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef MT8195_H
#define MT8195_H

/*=======================================================================*/
/* Constant Definitions                                                  */
/*=======================================================================*/

#define IO_PHYS             (0x10000000)
#define IO_SIZE             (0x02000000)

#define VER_BASE            (0x08000000)

/*=======================================================================*/
/* Register Bases                                                        */
/*=======================================================================*/
#define MCUCFG_BASE          (0x0C530000)
#define TOPCKGEN_BASE        (IO_PHYS)
#define INFRACFG_AO_BASE     (IO_PHYS + 0x00001000)
#define APMIXED_BASE         (IO_PHYS + 0x0000C000)
#define INFRA_AO_BCRM_BASE   (IO_PHYS + 0x00022000)
#define AUDIO_BASE           (IO_PHYS + 0x00890000)
#define AUDIO_SRC_BASE       (IO_PHYS + 0x008a0000)
#define CAMSYS_YUVB_BASE     (IO_PHYS + 0x060af000)
#define CAMSYS_MAIN_BASE     (IO_PHYS + 0x06000000)
#define CAMSYS_MRAW_BASE     (IO_PHYS + 0x06140000)
#define CAMSYS_RAWA_BASE     (IO_PHYS + 0x0604f000)
#define CAMSYS_RAWB_BASE     (IO_PHYS + 0x0608f000)
#define CAMSYS_YUVA_BASE     (IO_PHYS + 0x0606f000)
#define CCU_MAIN_BASE        (IO_PHYS + 0x07200000)
#define IMGSYS1_DIP_NR_BASE  (IO_PHYS + 0x05130000)
#define IMGSYS1_DIP_TOP_BASE (IO_PHYS + 0x05110000)
#define IMGSYS1_WPE_BASE     (IO_PHYS + 0x05220000)
#define IMGSYS_MAIN_BASE     (IO_PHYS + 0x05000000)
#define IPESYS_BASE          (IO_PHYS + 0x05330000)
#define MFGCFG_BASE          (IO_PHYS + 0x03fbf000)
#define PERICFG_AO_BASE      (IO_PHYS + 0x01003000)
#define SCP_PAR_TOP_BASE     (IO_PHYS + 0x00720000)
#define IPNNA_BASE           (IO_PHYS + 0x00211000)
#define VDEC_CORE1_GCON_BASE (IO_PHYS + 0x0803f000)
#define VDEC_GCON_BASE       (IO_PHYS + 0x0802f000)
#define VDEC_SOC_GCON_BASE   (IO_PHYS + 0x0800f000)
#define VDOSYS0_CONFIG_BASE  (IO_PHYS + 0x0c01a000)
#define VDOSYS1_CONFIG_BASE  (IO_PHYS + 0x0c100000)
#define VENC_CORE1_GCON_BASE (IO_PHYS + 0x0b000000)
#define VENC_GCON_BASE       (IO_PHYS + 0x0a000000)
#define VPP0_REG_BASE        (IO_PHYS + 0x04000000)
#define VPPSYS1_CONFIG_BASE  (IO_PHYS + 0x04f00000)
#define WPESYS_TOP_REG_BASE  (IO_PHYS + 0x04e00000)
#define WPE_VPP0_BASE        (IO_PHYS + 0x04e02000)
#define WPE_VPP1_BASE        (IO_PHYS + 0x04e03000)

#define IOCFG_BM_BASE 		(0x11D10000)
#define IOCFG_BL_BASE 		(0x11D30000)
#define IOCFG_BR_BASE 		(0x11D40000)
#define IOCFG_LM_BASE 		(0x11E20000)
#define IOCFG_RB_BASE 		(0x11EB0000)
#define IOCFG_TL_BASE 		(0x11F40000)

/*should be removed*/
#define PERICFG_BASE        (IO_PHYS + 0x00003000)

//#define PERI_CON_BASE       (IO_PHYS + 0x00003000)

#define GPIO_BASE           (IO_PHYS + 0x00005000)
#define SPM_BASE            (IO_PHYS + 0x00006000)
#define RGU_BASE            (IO_PHYS + 0x00007000)
#define GPT_BASE            (IO_PHYS + 0x00008000)
#define SYSTIMER_BASE       (IO_PHYS + 0x00017000)
#define TIA_BASE            (IO_PHYS + 0x0001C000)
#define PMIF_SPI_BASE       (IO_PHYS + 0x00024000)
#define PMICSPI_MST_BASE    (IO_PHYS + 0x00025000)
#define PMIF_SPMI_BASE      (IO_PHYS + 0x00027000)
#define SPMI_MST_BASE       (IO_PHYS + 0x00029000)
#define DDRPHY_BASE         (IO_PHYS + 0x00330000)
#define KPD_BASE            (IO_PHYS + 0x00010000)

#define DEM_BASE            (0x0D0A0000)

#define MCUSYS_CFGREG_BASE  (0x0C530000)
//#define CA7MCUCFG_BASE      (IO_PHYS + 0x00200100)
//#define CA15L_CONFIG_BASE   (IO_PHYS + 0x00200200)
#define SRAMROM_BASE        (IO_PHYS + 0x00214000)
#define GICD_BASE           (0x0c000000)
#define GICR_BASE           (0x0c040000)

#define AUXADC_BASE         (IO_PHYS + 0X01002000)
#define DEVINFO_BASE        (IO_PHYS + 0x01C10000)
#define UART0_BASE          (IO_PHYS + 0x01001100)
#define UART1_BASE          (IO_PHYS + 0x01001200)
#define UART2_BASE          (IO_PHYS + 0x01001300)
#define UART3_BASE          (IO_PHYS + 0x01001400)
#define SPI_BASE            (IO_PHYS + 0x010F0000)
#define NFI_BASE            (IO_PHYS + 0x0100D000) /* FIXME: not list in memory map */
#define NFIECC_BASE         (IO_PHYS + 0x0100E000) /* FIXME: not list in memory map */
#define MSDC0_TOP_BASE      (IO_PHYS + 0x01F50000)
#define MSDC1_TOP_BASE      (IO_PHYS + 0x01E10000)
#define MSDC0_BASE          (IO_PHYS + 0x01230000)
#define MSDC1_BASE          (IO_PHYS + 0x01240000)
#define MSDC2_BASE          (IO_PHYS + 0x01250000)
#define MSDC3_BASE          (IO_PHYS + 0x01260000) /* Reserved in Sylvia */
#define U3D_BASE			(IO_PHYS + 0x01200000) /* MAC: 0x1120_0000 */
#define USB_SIF_BASE		(IO_PHYS + 0x01E40000) /* PHY: 0x11E4_0000 */
#define USB1P_SIF_BASE      (IO_PHYS + 0x01C40000)
#define PCIE_BASE           (IO_PHYS + 0x093F0000)
#define PCIE_PHY_BASE       (IO_PHYS + 0x01E20000)

#define CPUXGPT_BASE        (IO_PHYS + 0x00200000)

#define SUB_INFRACFG_AO_BASE  (IO_PHYS + 0x0030E000)

/*=======================================================================*/
/* AP HW code offset                                                     */
/*=======================================================================*/
#define APHW_CODE           (VER_BASE)
#define APHW_SUBCODE        (VER_BASE + 0x04)
#define APHW_VER            (VER_BASE + 0x08)
#define APSW_VER            (VER_BASE + 0x0C)

// #define AMCONFG_BASE        (0xFFFFFFFF)

/*=======================================================================*/
/* USB register offset                                                   */
/*=======================================================================*/
#define SSUSB_DEV_BASE                      (U3D_BASE + 0x1000) /* FIXME: not list in memory map */
#define SSUSB_EPCTL_CSR_BASE                (U3D_BASE + 0x1800) /* FIXME: not list in memory map */
#define SSUSB_USB3_MAC_CSR_BASE             (U3D_BASE + 0x2400) /* FIXME: not list in memory map */
#define SSUSB_USB3_SYS_CSR_BASE             (U3D_BASE + 0x2400) /* FIXME: not list in memory map */
#define SSUSB_USB2_CSR_BASE                 (U3D_BASE + 0x3400) /* FIXME: not list in memory map */

#define SSUSB_SIFSLV_IPPC_BASE              (U3D_BASE + 0x3E00) /* FIXME: not list in memory map */

#define SSUSB_SIFSLV_SPLLC_BASE             (USB_SIF_BASE + 0x700) /* FIXME: not list in memory map */
#define SSUSB_SIFSLV_U2PHY_COM_BASE         (USB_SIF_BASE + 0x300) /* FIXME: not list in memory map */
#define SSUSB_SIFSLV_U3PHYD_BASE            (USB_SIF_BASE + 0x900) /* FIXME: not list in memory map */
#define SSUSB_SIFSLV_U2PHY_COM_SIV_B_BASE   (USB_SIF_BASE + 0x300) /* FIXME: not list in memory map */
#define SSUSB_USB30_PHYA_SIV_B2_BASE        (USB_SIF_BASE + 0xA00) /* FIXME: not list in memory map */
#define SSUSB_USB30_PHYA_SIV_B_BASE         (USB_SIF_BASE + 0xB00) /* FIXME: not list in memory map */
#define SSUSB_SIFSLV_U3PHYA_DA_BASE         (USB_SIF_BASE + 0xC00) /* FIXME: not list in memory map */


/*=======================================================================*/
/* USB download control                                                  */
/*=======================================================================*/
#define SECURITY_AO                 (0x1001A000)
#define BOOT_MISC0                  (SECURITY_AO + 0x0080)
#define MISC_LOCK_KEY               (SECURITY_AO + 0x0100)
#define RST_CON                     (SECURITY_AO + 0x0108)

#define MISC_LOCK_KEY_MAGIC    0xAD98
#define USBDL_FLAG    BOOT_MISC0


#define USBDL_BIT_EN        (0x00000001) /* 1: download bit enabled */
#define USBDL_BROM          (0x00000002) /* 0: usbdl by brom; 1: usbdl by bootloader */
#define USBDL_TIMEOUT_MASK  (0x0000FFFC) /* 14-bit timeout: 0x0000~0x3FFE: second; 0x3FFFF: no timeout */
#define USBDL_TIMEOUT_MAX   (USBDL_TIMEOUT_MASK >> 2) /* maximum timeout indicates no timeout */
#define USBDL_MAGIC         (0x444C0000) /* Brom will check this magic number */


#define SRAMROM_USBDL_TO_DIS (SRAMROM_BASE + 0x0054)
#define USBDL_TO_DIS         (0x00000001)

/*=======================================================================*/
/* NAND Control                                                          */
/*=======================================================================*/
#define NAND_PAGE_SIZE                  (2048)  // (Bytes)
#define NAND_BLOCK_BLKS                 (64)    // 64 nand pages = 128KB
#define NAND_PAGE_SHIFT                 (9)
#define NAND_LARGE_PAGE                 (11)    // large page
#define NAND_SMALL_PAGE                 (9)     // small page
#define NAND_BUS_WIDTH_8                (8)
#define NAND_BUS_WIDTH_16               (16)
#define NAND_FDM_SIZE                   (8)
#define NAND_ECC_SW                     (0)
#define NAND_ECC_HW                     (1)

#define NFI_MAX_FDM_SIZE                (8)
#define NFI_MAX_FDM_SEC_NUM             (8)
#define NFI_MAX_LOCK_CHANNEL            (16)

#define ECC_MAX_CORRECTABLE_BITS        (12)
#define ECC_MAX_PARITY_SIZE             (20)    /* in bytes */

#define ECC_ERR_LOCATION_MASK           (0x1FFF)
#define ECC_ERR_LOCATION_SHIFT          (16)

#define NAND_FFBUF_SIZE                 (2048 + 64)

/*=======================================================================*/
/* SW Reset Vector                                                       */
/*=======================================================================*/
/* setup the reset vector base address after warm reset to Aarch64 */
#define RVBADDRESS_CPU0     (MCUSYS_CFGREG_BASE + 0xC900)

/* IRQ */
#define SYS_TIMER_IRQ       (233 + 32)
#define GZ_SW_IRQ           (513 + 32)

#endif
