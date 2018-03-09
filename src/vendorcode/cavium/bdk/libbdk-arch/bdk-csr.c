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
#include "libbdk-arch/bdk-csrs-pccpf.h"
#include "libbdk-arch/bdk-csrs-pem.h"
#include "libbdk-arch/bdk-csrs-rst.h"
#include "libbdk-hal/bdk-pcie.h"

#ifndef BDK_BUILD_HOST

/**
 * Read a slow CSR, not RSL or NCB.
 *
 * @param type    Bus type the CSR is on
 * @param busnum  Bus number the CSR is on
 * @param size    Width of the CSR in bytes
 * @param address The address of the CSR
 *
 * @return The value of the CSR
 */
uint64_t __bdk_csr_read_slow(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address)
{
    switch (type)
    {
        case BDK_CSR_TYPE_DAB:
        case BDK_CSR_TYPE_DAB32b:
        case BDK_CSR_TYPE_NCB:
        case BDK_CSR_TYPE_NCB32b:
        case BDK_CSR_TYPE_PEXP_NCB:
        case BDK_CSR_TYPE_RSL:
        case BDK_CSR_TYPE_RSL32b:
        case BDK_CSR_TYPE_RVU_PF_BAR0:
        case BDK_CSR_TYPE_RVU_PF_BAR2:
        case BDK_CSR_TYPE_RVU_PFVF_BAR2:
        case BDK_CSR_TYPE_RVU_VF_BAR2:
            /* Handled by inline code, we should never get here */
            bdk_error("%s: Passed type that should be handled inline\n", __FUNCTION__);
            break;

        case BDK_CSR_TYPE_PCCBR:
        case BDK_CSR_TYPE_PCCPF:
        case BDK_CSR_TYPE_PCCVF:
        case BDK_CSR_TYPE_PEXP:
        case BDK_CSR_TYPE_MDSB:
        case BDK_CSR_TYPE_PCICONFIGEP_SHADOW:
        case BDK_CSR_TYPE_PCICONFIGEPVF:
            bdk_error("%s: Register not supported\n", __FUNCTION__);
            break;

        case BDK_CSR_TYPE_SYSREG:
            return bdk_sysreg_read(node, bdk_get_core_num(), address);

        case BDK_CSR_TYPE_PCICONFIGRC:
        {
            union bdk_pcc_dev_con_s dev_con;
            switch (busnum)
            {
                case 0:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 1:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 2:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 3:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC3_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC3_CN83XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 4:
                    dev_con.u = BDK_PCC_DEV_CON_E_PCIERC4;
                    break;
                case 5:
                    dev_con.u = BDK_PCC_DEV_CON_E_PCIERC5;
                    break;
                default:
                    bdk_error("%s: Illegal PCIe bus number\n", __FUNCTION__);
                    return -1;
            }
            return bdk_pcie_config_read32(node, 100 + dev_con.cn8.ecam, dev_con.s.bus, dev_con.s.func >> 3, dev_con.s.func & 7, address);
        }
        case BDK_CSR_TYPE_PCICONFIGEP:
        {
            BDK_CSR_DEFINE(cfg_rd, BDK_PEMX_CFG_RD(busnum));
            cfg_rd.u = 0;
            cfg_rd.s.addr = address;
            BDK_CSR_WRITE(node, BDK_PEMX_CFG_RD(busnum), cfg_rd.u);
            cfg_rd.u = BDK_CSR_READ(node, BDK_PEMX_CFG_RD(busnum));
            return cfg_rd.s.data;
        }
    }
    return -1; /* Return -1 as this looks invalid in register dumps. Zero is too common as a good value */
}


/**
 * Write a value to a slow CSR, not RSL or NCB.
 *
 * @param type    Bus type the CSR is on
 * @param busnum  Bus number the CSR is on
 * @param size    Width of the CSR in bytes
 * @param address The address of the CSR
 * @param value   Value to write to the CSR
 */
void __bdk_csr_write_slow(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value)
{
    switch (type)
    {
        case BDK_CSR_TYPE_DAB:
        case BDK_CSR_TYPE_DAB32b:
        case BDK_CSR_TYPE_NCB:
        case BDK_CSR_TYPE_NCB32b:
        case BDK_CSR_TYPE_PEXP_NCB:
        case BDK_CSR_TYPE_RSL:
        case BDK_CSR_TYPE_RSL32b:
        case BDK_CSR_TYPE_RVU_PF_BAR0:
        case BDK_CSR_TYPE_RVU_PF_BAR2:
        case BDK_CSR_TYPE_RVU_PFVF_BAR2:
        case BDK_CSR_TYPE_RVU_VF_BAR2:
            /* Handled by inline code, we should never get here */
            bdk_error("%s: Passed type that should be handled inline\n", __FUNCTION__);
            break;

        case BDK_CSR_TYPE_PCCBR:
        case BDK_CSR_TYPE_PCCPF:
        case BDK_CSR_TYPE_PCCVF:
        case BDK_CSR_TYPE_PEXP:
        case BDK_CSR_TYPE_MDSB:
        case BDK_CSR_TYPE_PCICONFIGEP_SHADOW:
        case BDK_CSR_TYPE_PCICONFIGEPVF:
            bdk_error("%s: Register not supported\n", __FUNCTION__);
            break;

        case BDK_CSR_TYPE_SYSREG:
            bdk_sysreg_write(node, bdk_get_core_num(), address, value);
            break;

        case BDK_CSR_TYPE_PCICONFIGRC:
        {
            union bdk_pcc_dev_con_s dev_con;
            switch (busnum)
            {
                case 0:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC0_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 1:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC1_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 2:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN83XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC2_CN81XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 3:
                    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC3_CN88XX;
                    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
                        dev_con.u = BDK_PCC_DEV_CON_E_PCIERC3_CN83XX;
                    else
                        bdk_fatal("Update PCICONFIG in %s\n", __FUNCTION__);
                    break;
                case 4:
                    dev_con.u = BDK_PCC_DEV_CON_E_PCIERC4;
                    break;
                case 5:
                    dev_con.u = BDK_PCC_DEV_CON_E_PCIERC5;
                    break;
                default:
                    bdk_error("%s: Illegal PCIe bus number\n", __FUNCTION__);
                    return;
            }
            bdk_pcie_config_write32(node, 100 + dev_con.cn8.ecam, dev_con.s.bus, dev_con.s.func >> 3, dev_con.s.func & 7, address, value);
            break;
        }
        case BDK_CSR_TYPE_PCICONFIGEP:
        {
            BDK_CSR_DEFINE(cfg_wr, BDK_PEMX_CFG_WR(busnum));
            cfg_wr.u = 0;
            cfg_wr.s.addr = address;
            cfg_wr.s.data = value;
            BDK_CSR_WRITE(node, BDK_PEMX_CFG_WR(busnum), cfg_wr.u);
            break;
        }
    }
}

#endif

void __bdk_csr_fatal(const char *name, int num_args, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4)
{
    switch (num_args)
    {
        case 0:
            bdk_fatal("%s is invalid on this chip\n", name);
        case 1:
            bdk_fatal("%s(%lu) is invalid on this chip\n", name, arg1);
        case 2:
            bdk_fatal("%s(%lu,%lu) is invalid on this chip\n", name, arg1, arg2);
        case 3:
            bdk_fatal("%s(%lu,%lu,%lu) is invalid on this chip\n", name, arg1, arg2, arg3);
        default:
            bdk_fatal("%s(%lu,%lu,%lu,%lu) is invalid on this chip\n", name, arg1, arg2, arg3, arg4);
    }
}

/**
 * Read a core system register from a different node or core
 *
 * @param node   Node to read from
 * @param core   Core to read
 * @param regnum Register to read in MRS encoding
 *
 * @return Register value
 */
uint64_t bdk_sysreg_read(int node, int core, uint64_t regnum)
{
    BDK_CSR_INIT(pp_reset, node, BDK_RST_PP_RESET);
    if (pp_reset.u & (1ull<<core))
    {
        bdk_error("Attempt to read system register for core in reset\n");
        return -1;
    }

    /* Addresses indicate selects as follows:
        select 3,4,14,2,3
     == 0x03040e020300
           | | | | |^--- 1 if is E2H duplicated register
           | | | |^^-- fifth select
           | | |^^-- fourth select
           | |^^-- third select
           |^^-- second select
          ^^-- first select */
    uint64_t first = (regnum >> 40) & 0xff;
    uint64_t second = (regnum >> 32) & 0xff;
    uint64_t third = (regnum >> 24) & 0xff;
    uint64_t fourth = (regnum >> 16) & 0xff;
    uint64_t fifth = (regnum >> 8) & 0xff;
    uint64_t regid = ((first & 3) << 14) | (second << 11) | (third << 7) | (fourth << 3) | fifth;

    /* Note this requires DAP_IMP_DAR[caben] = 1 */
    uint64_t address = 1ull<<47;
    address |= 0x7Bull << 36;
    address |= core << 19;
    address |= regid << 3;
    address = bdk_numa_get_address(node, address);
    return bdk_read64_uint64(address);
}

/**
 * Write a system register for a different node or core
 *
 * @param node   Node to write too
 * @param core   Core to write
 * @param regnum Register to write in MSR encoding
 * @param value  Value to write
 */
void bdk_sysreg_write(int node, int core, uint64_t regnum, uint64_t value)
{
    BDK_CSR_INIT(pp_reset, node, BDK_RST_PP_RESET);
    if (pp_reset.u & (1ull<<core))
    {
        bdk_error("Attempt to write system register for core in reset\n");
        return;
    }

    /* Addresses indicate selects as follows:
        select 3,4,14,2,3
     == 0x03040e020300
           | | | | |^--- 1 if is E2H duplicated register
           | | | |^^-- fifth select
           | | |^^-- fourth select
           | |^^-- third select
           |^^-- second select
          ^^-- first select */
    uint64_t first = (regnum >> 40) & 0xff;
    uint64_t second = (regnum >> 32) & 0xff;
    uint64_t third = (regnum >> 24) & 0xff;
    uint64_t fourth = (regnum >> 16) & 0xff;
    uint64_t fifth = (regnum >> 8) & 0xff;
    uint64_t regid = ((first & 3) << 14) | (second << 11) | (third << 7) | (fourth << 3) | fifth;

    /* Note this requires DAP_IMP_DAR[caben] = 1 */
    uint64_t address = 1ull<<47;
    address |= 0x7Bull << 36;
    address |= core << 19;
    address |= regid << 3;
    address = bdk_numa_get_address(node, address);
    bdk_write64_uint64(address, value);
}

