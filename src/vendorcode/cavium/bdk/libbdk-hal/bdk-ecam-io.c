/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/
#include <bdk.h>
#include "libbdk-arch/bdk-csrs-ecam.h"
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-pccpf.h"
#include "libbdk-arch/bdk-csrs-pem.h"
#include "libbdk-hal/device/bdk-device.h"
#include "libbdk-hal/bdk-ecam.h"

#if 1 /* Support CN88XX pass 1.0 */
/*******************************************************************
 *******************************************************************
    These functions are related to CN88XX pass 1.0 errata and do not
    apply to any other chip
 *******************************************************************
 *******************************************************************/

/**
 * Errata (ECAM-22630) ECAM function accesses can fault
 * For some errata workaround we need a check to tell if a ECAM access is to a
 * valid intenral device. This function decodes a pcc_dev_con_e enumeration and
 * checks if the supplied arguments match it. This should only
 * ever be called on CN88XX pass 1.0.
 *
 * @param ecam    ECAM to check
 * @param bus     ECAM bus number
 * @param dev     Device to check
 * @param fn      sub function of device
 * @param dev_con Enumeration to match against
 *
 * @return Non zero if the device matches
 */
static int is_internal_cn88xxp1_0(const bdk_device_t *device, int dev_con)
{
    union bdk_pcc_dev_con_s d = { .u = dev_con };
    return (d.cn8.ecam == device->ecam) && (d.s.bus == device->bus) && (d.s.func == ((device->dev<<3)|device->func));
}

/**
 * Errata (ECAM-22630) ECAM function accesses can fault
 * This is a companion to the function above to determine if the ECAM device is
 * any of the valid internal devices. This should only ever be
 * called on CN88XX pass 1.0.
 *
 * @param ecam   ECAM to check
 * @param bus    ECAM bus number
 * @param dev    Device to check
 * @param fn     sub function of device
 *
 * @return Non zero if the device matches
 */
static int is_any_internal_cn88xxp1_0(const bdk_device_t *device)
{
    /* Errata (ECAM-22630) ECAM function accesses can fault
        CN88XXP1.0: The ECAM has a bug where accessing a non-existent
        device causes an exception. This is a list of all valid devices
        for CN88XX pass 1.0 */
    static const uint32_t INTERNAL_DEVICES_CN88XXP1_0[] = {
        BDK_PCC_DEV_CON_E_BGXX(0),
        BDK_PCC_DEV_CON_E_BGXX(1),
        BDK_PCC_DEV_CON_E_DAP,
        BDK_PCC_DEV_CON_E_DFA,
        BDK_PCC_DEV_CON_E_FUSF,
        BDK_PCC_DEV_CON_E_GIC_CN8,
        BDK_PCC_DEV_CON_E_GPIO_CN8,
        BDK_PCC_DEV_CON_E_GSERX(0),
        BDK_PCC_DEV_CON_E_GSERX(1),
        BDK_PCC_DEV_CON_E_GSERX(10),
        BDK_PCC_DEV_CON_E_GSERX(11),
        BDK_PCC_DEV_CON_E_GSERX(12),
        BDK_PCC_DEV_CON_E_GSERX(13),
        BDK_PCC_DEV_CON_E_GSERX(2),
        BDK_PCC_DEV_CON_E_GSERX(3),
        BDK_PCC_DEV_CON_E_GSERX(4),
        BDK_PCC_DEV_CON_E_GSERX(5),
        BDK_PCC_DEV_CON_E_GSERX(6),
        BDK_PCC_DEV_CON_E_GSERX(7),
        BDK_PCC_DEV_CON_E_GSERX(8),
        BDK_PCC_DEV_CON_E_GSERX(9),
        BDK_PCC_DEV_CON_E_GTI_CN8,
        BDK_PCC_DEV_CON_E_IOBNX(0),
        BDK_PCC_DEV_CON_E_IOBNX(1),
        BDK_PCC_DEV_CON_E_KEY,
        BDK_PCC_DEV_CON_E_L2C,
        BDK_PCC_DEV_CON_E_L2C_CBCX(0),
        BDK_PCC_DEV_CON_E_L2C_CBCX(1),
        BDK_PCC_DEV_CON_E_L2C_CBCX(2),
        BDK_PCC_DEV_CON_E_L2C_CBCX(3),
        BDK_PCC_DEV_CON_E_L2C_MCIX(0),
        BDK_PCC_DEV_CON_E_L2C_MCIX(1),
        BDK_PCC_DEV_CON_E_L2C_MCIX(2),
        BDK_PCC_DEV_CON_E_L2C_MCIX(3),
        BDK_PCC_DEV_CON_E_L2C_TADX(0),
        BDK_PCC_DEV_CON_E_L2C_TADX(1),
        BDK_PCC_DEV_CON_E_L2C_TADX(2),
        BDK_PCC_DEV_CON_E_L2C_TADX(3),
        BDK_PCC_DEV_CON_E_L2C_TADX(4),
        BDK_PCC_DEV_CON_E_L2C_TADX(5),
        BDK_PCC_DEV_CON_E_L2C_TADX(6),
        BDK_PCC_DEV_CON_E_L2C_TADX(7),
        BDK_PCC_DEV_CON_E_LMCX(0),
        BDK_PCC_DEV_CON_E_LMCX(1),
        BDK_PCC_DEV_CON_E_LMCX(2),
        BDK_PCC_DEV_CON_E_LMCX(3),
        BDK_PCC_DEV_CON_E_MIO_BOOT,
        BDK_PCC_DEV_CON_E_MIO_EMM,
        BDK_PCC_DEV_CON_E_MIO_FUS,
        BDK_PCC_DEV_CON_E_MIO_PTP,
        BDK_PCC_DEV_CON_E_MIO_TWSX(0),
        BDK_PCC_DEV_CON_E_MIO_TWSX(1),
        BDK_PCC_DEV_CON_E_MIO_TWSX(2),
        BDK_PCC_DEV_CON_E_MIO_TWSX(3),
        BDK_PCC_DEV_CON_E_MIO_TWSX(4),
        BDK_PCC_DEV_CON_E_MIO_TWSX(5),
        BDK_PCC_DEV_CON_E_MPI,
        BDK_PCC_DEV_CON_E_MRML,
        BDK_PCC_DEV_CON_E_NCSI,
        BDK_PCC_DEV_CON_E_NIC_CN88XX,
        BDK_PCC_DEV_CON_E_OCLAX_CN8(0),
        BDK_PCC_DEV_CON_E_OCLAX_CN8(1),
        BDK_PCC_DEV_CON_E_OCLAX_CN8(2),
        BDK_PCC_DEV_CON_E_OCLAX_CN8(3),
        BDK_PCC_DEV_CON_E_OCLAX_CN8(4),
        BDK_PCC_DEV_CON_E_OCX,
        BDK_PCC_DEV_CON_E_PCCBR_DFA,
        BDK_PCC_DEV_CON_E_PCCBR_MRML,
        BDK_PCC_DEV_CON_E_PCCBR_NIC_CN88XX,
        BDK_PCC_DEV_CON_E_PCCBR_RAD_CN88XX,
        BDK_PCC_DEV_CON_E_PCCBR_ZIP_CN88XX,
        BDK_PCC_DEV_CON_E_PCIERC0_CN88XX,
        BDK_PCC_DEV_CON_E_PCIERC1_CN88XX,
        BDK_PCC_DEV_CON_E_PCIERC2_CN88XX,
        BDK_PCC_DEV_CON_E_PCIERC3_CN88XX,
        BDK_PCC_DEV_CON_E_PCIERC4,
        BDK_PCC_DEV_CON_E_PCIERC5,
        BDK_PCC_DEV_CON_E_PEMX(0),
        BDK_PCC_DEV_CON_E_PEMX(1),
        BDK_PCC_DEV_CON_E_PEMX(2),
        BDK_PCC_DEV_CON_E_PEMX(3),
        BDK_PCC_DEV_CON_E_PEMX(4),
        BDK_PCC_DEV_CON_E_PEMX(5),
        BDK_PCC_DEV_CON_E_RAD_CN88XX,
        BDK_PCC_DEV_CON_E_RNM_CN88XX,
        BDK_PCC_DEV_CON_E_RST,
        BDK_PCC_DEV_CON_E_SATA0_CN88XX,
        BDK_PCC_DEV_CON_E_SATA1_CN88XX,
        BDK_PCC_DEV_CON_E_SATA10,
        BDK_PCC_DEV_CON_E_SATA11,
        BDK_PCC_DEV_CON_E_SATA12,
        BDK_PCC_DEV_CON_E_SATA13,
        BDK_PCC_DEV_CON_E_SATA14,
        BDK_PCC_DEV_CON_E_SATA15,
        BDK_PCC_DEV_CON_E_SATA2,
        BDK_PCC_DEV_CON_E_SATA3,
        BDK_PCC_DEV_CON_E_SATA4,
        BDK_PCC_DEV_CON_E_SATA5,
        BDK_PCC_DEV_CON_E_SATA6,
        BDK_PCC_DEV_CON_E_SATA7,
        BDK_PCC_DEV_CON_E_SATA8,
        BDK_PCC_DEV_CON_E_SATA9,
        BDK_PCC_DEV_CON_E_SGP,
        BDK_PCC_DEV_CON_E_SLI0_CN88XX,
        BDK_PCC_DEV_CON_E_SLI1,
        BDK_PCC_DEV_CON_E_SMI,
        BDK_PCC_DEV_CON_E_SMMU0_CN8,
        BDK_PCC_DEV_CON_E_SMMU1,
        BDK_PCC_DEV_CON_E_SMMU2,
        BDK_PCC_DEV_CON_E_SMMU3,
        BDK_PCC_DEV_CON_E_TNS,
        BDK_PCC_DEV_CON_E_UAAX_CN8(0),
        BDK_PCC_DEV_CON_E_UAAX_CN8(1),
        BDK_PCC_DEV_CON_E_USBHX(0),
        BDK_PCC_DEV_CON_E_USBHX(1),
        BDK_PCC_DEV_CON_E_VRMX(0),
        BDK_PCC_DEV_CON_E_VRMX(1),
        BDK_PCC_DEV_CON_E_ZIP_CN88XX,
        0,
    };

    int loc = 0;
    while (INTERNAL_DEVICES_CN88XXP1_0[loc])
    {
        if (is_internal_cn88xxp1_0(device, INTERNAL_DEVICES_CN88XXP1_0[loc]))
            return 1;
        loc++;
    }
    return 0;
}

static int is_accessable_cn88xxp1_0(const bdk_device_t *device)
{
    /* Errata (ECAM-22630) ECAM function accesses can fault */
    /* Skip internal devices that don't exists */
    if (!is_any_internal_cn88xxp1_0(device))
        return 0;

    /* Errata (ECAM-23020) PCIERC transactions fault unless PEM is
       out of reset. The PCIe ports don't work until the PEM is
       turned on. Check for one of the PCIe ports */
    int pem = -1;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC0_CN88XX))
        pem = 0;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC1_CN88XX))
        pem = 1;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC2_CN88XX))
        pem = 2;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC3_CN88XX))
        pem = 3;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC4))
        pem = 4;
    if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_PCIERC5))
        pem = 5;
    if (pem != -1)
    {
        BDK_CSR_INIT(pem_on, device->node, BDK_PEMX_ON(pem));
        if (!pem_on.s.pemon || !pem_on.s.pemoor)
            return 0;
    }

    {
        /* SATA ports should be hidden if they aren't configured at the QLM */
        int qlm = -1;
        if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA0_CN88XX) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA1_CN88XX) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA2) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA3))
            qlm = 2;
        if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA4) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA5) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA6) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA7))
            qlm = 3;
        if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA8) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA9) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA10) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA11))
            qlm = 6;
        if (is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA12) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA13) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA14) ||
            is_internal_cn88xxp1_0(device, BDK_PCC_DEV_CON_E_SATA15))
            qlm = 7;
        if (qlm != -1)
        {
            BDK_CSR_INIT(cfg, device->node, BDK_GSERX_CFG(qlm));
            if (!cfg.s.sata)
                return 0;
        }
    }
    return 1;
}

#endif /* Support CN88XX pass 1.0 */

/**
 * Build an ECAM config space request address for a device
 *
 * @param device Device being accessed
 * @param reg    Register to access
 *
 * @return 64bit IO address
 */
uint64_t __bdk_ecam_build_address(const bdk_device_t *device, int reg)
{
    /* CN88XX pass 1.0 had a plethora of errata related to ECAM access. This
       checks to make sure we're allowed to access this location based on
       the various errata */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_0) && !is_accessable_cn88xxp1_0(device))
        return 0;

    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
    {
        /* Build the address */
        union bdk_ecam_cfg_addr_s address;
        address.u = BDK_ECAM_BAR_E_ECAMX_PF_BAR2(device->ecam);
        address.s.node = device->node;
        address.s.bus = device->bus;
        address.s.func = device->dev << 3 | device->func;
        address.s.addr = reg;
        return address.u;
    }
    else
    {
        /* Build the address. The architects decided to make it different
           from CN8XXX for no obvious reason */
        union bdk_ecam_cfg_addr_s address;
        address.u = BDK_ECAM_BAR_E_ECAMX_PF_BAR2(0);
        address.s.node = device->node;
        address.s.dmn = device->ecam;
        address.s.bus = device->bus;
        address.s.func = device->dev << 3 | device->func;
        address.s.addr = reg;
        return address.u;
    }
}

/**
 * Read from an ECAM
 *
 * @param device Device to read from
 * @param reg    Register to read
 *
 * @return Result of the read of -1 on failure
 */
uint32_t bdk_ecam_read32(const bdk_device_t *device, int reg)
{
    uint64_t address = __bdk_ecam_build_address(device, reg);
    uint32_t result;
    if (address)
        result = bdk_le32_to_cpu(bdk_read64_uint32(address));
    else
        result = 0xffffffff;

    /* Errata ECAM-22630: CN88XX pass 1.x, except pass 1.0, will return zero
       for non-existent devices instead of ones. We look for this special case
       for 32bit reads for reg=0 so we can scan device properly */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (reg == 0) && (result == 0))
        result = 0xffffffff;

    return result;
}

/**
 * Write to an ECAM register
 *
 * @param device Device to write to
 * @param reg    Register to write
 * @param value  Value to write
 */
void bdk_ecam_write32(const bdk_device_t *device, int reg, uint32_t value)
{
    uint64_t address = __bdk_ecam_build_address(device, reg);
    if (address)
        bdk_write64_uint32(address, bdk_cpu_to_le32(value));
}

