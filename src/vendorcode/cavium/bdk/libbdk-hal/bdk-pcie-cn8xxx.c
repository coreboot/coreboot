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
#include <string.h>
#include "libbdk-arch/bdk-csrs-dtx.h"
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-gic.h"
#include "libbdk-arch/bdk-csrs-pem.h"
#include "libbdk-arch/bdk-csrs-pcierc.h"
#include "libbdk-arch/bdk-csrs-sli.h"
#include "libbdk-arch/bdk-csrs-rst.h"
#include "libbdk-hal/bdk-pcie.h"
#include "libbdk-hal/bdk-config.h"
#include "libbdk-hal/bdk-utils.h"
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/device/bdk-device.h"
#include "libbdk-hal/bdk-ecam.h"

/**
 * Return the number of possible PCIe ports on a node. The actual number
 * of configured ports may be less and may also be disjoint.
 *
 * @param node   Node to query
 *
 * @return Number of PCIe ports that are possible
 */
int bdk_pcie_get_num_ports(bdk_node_t node)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 6;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 4;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 3;
    else
        return 0;
}


/**
 * Given a PCIe port, determine which SLI controls its memory regions
 *
 * @param node      Node for the PCIe port
 * @param pcie_port The PCIe port
 * @param sli       The SLI index is written to this integer pointer
 * @param sli_group The index of the PCIe port on the SLI is returned here. This is a sequencial
 *                  number for each PCIe on an SLI. Use this to index SLI regions.
 */
static void __bdk_pcie_get_sli(bdk_node_t node, int pcie_port, int *sli, int *sli_group)
{
    /* This mapping should be determined by find the SLI number on the
       same ECAM bus as the PCIERC bridge. That is fairly complex, so it is
       hardcoded for now */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        /* Ports 0-2 goto SLI0, ports 3-5 goto SLI1 */
        *sli = (pcie_port >= 3) ? 1 : 0;
        *sli_group = pcie_port - *sli * 3;
        return;
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) || CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        /* Only one SLI */
        *sli = 0;
        *sli_group = pcie_port;
        return;
    }
    else
        bdk_fatal("Unable to determine SLI for PCIe port. Update __bdk_pcie_get_sli()\n");
}

/**
 * Return the Core physical base address for PCIe MEM access. Memory is
 * read/written as an offset from this address.
 *
 * @param node      Node to use in a Numa setup
 * @param pcie_port PCIe port the memory is on
 * @param mem_type  Type of memory
 *
 * @return 64bit physical address for read/write
 */
uint64_t bdk_pcie_get_base_address(bdk_node_t node, int pcie_port, bdk_pcie_mem_t mem_type)
{
    /* See __bdk_pcie_sli_initialize() for a description about how SLI regions work */
    int sli;
    int sli_group;
    __bdk_pcie_get_sli(node, pcie_port, &sli, &sli_group);
    int region = (sli_group << 6) | (mem_type << 4);
    union bdk_sli_s2m_op_s s2m_op;
    s2m_op.u = 0;
    s2m_op.s.io = 1;
    s2m_op.s.node = node;
    s2m_op.s.did_hi = 0x8 + sli;
    s2m_op.s.region = region;
    return s2m_op.u;
}

/**
 * Size of the Mem address region returned at address
 * bdk_pcie_get_base_address()
 *
 * @param node      Node to use in a Numa setup
 * @param pcie_port PCIe port the IO is for
 * @param mem_type  Type of memory
 *
 * @return Size of the Mem window
 */
uint64_t bdk_pcie_get_base_size(bdk_node_t node, int pcie_port, bdk_pcie_mem_t mem_type)
{
    return 1ull << 36;
}

/**
 * @INTERNAL
 * Initialize the RC config space CSRs
 *
 * @param pcie_port PCIe port to initialize
 */
static void __bdk_pcie_rc_initialize_config_space(bdk_node_t node, int pcie_port)
{
    int sli;
    int sli_group;
    __bdk_pcie_get_sli(node, pcie_port, &sli, &sli_group);

    /* The reset default for config retries is too short. Set it to 48ms, which
       is what the Octeon SDK team is using. There is no documentation about
       where they got the 48ms number */
    int cfg_retry = 48 * 1000000 / (bdk_clock_get_rate(node, BDK_CLOCK_SCLK) >> 16);
    if (cfg_retry >= 0x10000)
        cfg_retry = 0xfffff;
    BDK_CSR_MODIFY(c, node, BDK_PEMX_CTL_STATUS(pcie_port),
        c.cn83xx.cfg_rtry = cfg_retry);


    /* Max Payload Size (PCIE*_CFG030[MPS]) */
    /* Max Read Request Size (PCIE*_CFG030[MRRS]) */
    /* Relaxed-order, no-snoop enables (PCIE*_CFG030[RO_EN,NS_EN] */
    /* Error Message Enables (PCIE*_CFG030[CE_EN,NFE_EN,FE_EN,UR_EN]) */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG030(pcie_port),
        c.s.mps = 1; /* Support 256 byte MPS */
        c.s.mrrs = 0x5; /* Support 4KB MRRS */
        c.s.ro_en = 1; /* Enable relaxed order processing. This will allow devices to affect read response ordering */
        c.s.ns_en = 1; /* Enable no snoop processing. Not used */
        c.s.ce_en = 1; /* Correctable error reporting enable. */
        c.s.nfe_en = 1; /* Non-fatal error reporting enable. */
        c.s.fe_en = 1; /* Fatal error reporting enable. */
        c.s.ur_en = 1); /* Unsupported request reporting enable. */

    /* Configure the PCIe slot number if specified */
    int slot_num = bdk_config_get_int(BDK_CONFIG_PCIE_PHYSICAL_SLOT, node, pcie_port);
    if (slot_num != -1)
    {
        BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG028(pcie_port),
            c.s.si = 1); /* Slot Implemented*/
        BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG033(pcie_port),
            c.s.ps_num = slot_num);
    }

    /* Disable ECRC Generation as not all card support it. The OS can enable it
       later if desired (PCIE*_CFG070[GE,CE]) */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG070(pcie_port),
        c.s.ge = 0; /* ECRC generation disable. */
        c.s.ce = 0); /* ECRC check disable. */

    /* Access Enables (PCIE*_CFG001[MSAE,ME]) */
        /* ME and MSAE should always be set. */
    /* Interrupt Disable (PCIE*_CFG001[I_DIS]) */
    /* System Error Message Enable (PCIE*_CFG001[SEE]) */
        BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG001(pcie_port),
        c.s.msae = 1; /* Memory space enable. */
        c.s.me = 1; /* Bus master enable. */
        c.s.i_dis = 1; /* INTx assertion disable. */
        c.s.see = 1); /* SERR# enable */

    /* Advanced Error Recovery Message Enables */
    /* (PCIE*_CFG066,PCIE*_CFG067,PCIE*_CFG069) */
    BDK_CSR_WRITE(node, BDK_PCIERCX_CFG066(pcie_port), 0);
    /* Use BDK_PCIERCX_CFG067 hardware default */
    BDK_CSR_WRITE(node, BDK_PCIERCX_CFG069(pcie_port), 0);


    /* Active State Power Management (PCIE*_CFG032[ASLPC]) */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG032(pcie_port),
        c.s.aslpc = 0); /* Active state Link PM control. */

    /* Link Width Mode (PCIERCn_CFG452[LME]) - Set during bdk_pcie_rc_initialize_link() */
    /* Primary Bus Number (PCIERCn_CFG006[PBNUM]) */
    /* Use bus numbers as follows:
        0 - 31: Reserved for internal ECAM
        32 - 87: First PCIe on SLI
        88 - 143: Second PCIe on SLI
        144 - 199: Third PCIe on SLI
        200 - 255: Fourth PCIe on SLI
        Start bus = 32 + pcie * 56 */
    const int BUSSES_PER_PCIE = 56;
    int bus = 32 + sli_group * BUSSES_PER_PCIE;
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG006(pcie_port),
        c.s.pbnum = 0;
        c.s.sbnum = bus;
        c.s.subbnum = bus + BUSSES_PER_PCIE - 1);

    /* Memory-mapped I/O BAR (PCIERCn_CFG008) */
    uint64_t mem_base = bdk_pcie_get_base_address(node, pcie_port, BDK_PCIE_MEM_NORMAL);
    uint64_t mem_limit = mem_base + bdk_pcie_get_base_size(node, pcie_port, BDK_PCIE_MEM_NORMAL) - 1;
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG008(pcie_port),
        c.s.mb_addr = mem_base >> 16;
        c.s.ml_addr = mem_limit >> 16);

    /* Prefetchable BAR (PCIERCn_CFG009,PCIERCn_CFG010,PCIERCn_CFG011) */
    uint64_t prefetch_base = bdk_pcie_get_base_address(node, pcie_port, BDK_PCIE_MEM_PREFETCH);
    uint64_t prefetch_limit = prefetch_base + bdk_pcie_get_base_size(node, pcie_port, BDK_PCIE_MEM_PREFETCH) - 1;
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG009(pcie_port),
        c.s.lmem_base = prefetch_base >> 16;
        c.s.lmem_limit = prefetch_limit >> 16);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG010(pcie_port),
        c.s.umem_base = prefetch_base >> 32);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG011(pcie_port),
        c.s.umem_limit = prefetch_limit >> 32);

    /* System Error Interrupt Enables (PCIERCn_CFG035[SECEE,SEFEE,SENFEE]) */
    /* PME Interrupt Enables (PCIERCn_CFG035[PMEIE]) */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG035(pcie_port),
        c.s.secee = 1; /* System error on correctable error enable. */
        c.s.sefee = 1; /* System error on fatal error enable. */
        c.s.senfee = 1; /* System error on non-fatal error enable. */
        c.s.pmeie = 1); /* PME interrupt enable. */

    /* Advanced Error Recovery Interrupt Enables */
    /* (PCIERCn_CFG075[CERE,NFERE,FERE]) */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG075(pcie_port),
        c.s.cere = 1; /* Correctable error reporting enable. */
        c.s.nfere = 1; /* Non-fatal error reporting enable. */
        c.s.fere = 1); /* Fatal error reporting enable. */

    /* Make sure the PEM agrees with GSERX about the speed its going to try */
    BDK_CSR_INIT(pem_cfg, node, BDK_PEMX_CFG(pcie_port));
    switch (pem_cfg.cn83xx.md)
    {
        case 0: /* Gen 1 */
            /* Set the target link speed */
            BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG040(pcie_port),
                c.s.tls = 1);
            break;
        case 1: /* Gen 2 */
            /* Set the target link speed */
            BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG040(pcie_port),
                c.s.tls = 2);
            break;
        case 2: /* Gen 3 */
            /* Set the target link speed */
            BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG040(pcie_port),
                c.s.tls = 3);
            break;
        default:
            bdk_error("N%d.PCIe%d: Unexpected rate of %d\n", node, pcie_port, pem_cfg.cn83xx.md);
            break;
    }

    BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(pcie_port));
    BDK_CSR_INIT(cfg452, node, BDK_PCIERCX_CFG452(pcie_port));
    BDK_CSR_INIT(cfg031, node, BDK_PCIERCX_CFG031(pcie_port));
    int lme = cfg452.s.lme;
    int mlw = cfg031.s.mlw;

    /* Link Width Mode (PCIERCn_CFG452[LME]) */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        lme = (pemx_cfg.cn88xx.lanes8) ? 0xf : 0x7;
        mlw = (pemx_cfg.cn88xx.lanes8) ? 8 : 4;
    }
    /* CN83XX can support 8 lanes on QLM0+1 or QLM2+3. 4 lanes on DLM5+6 */
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
    {
        switch (pcie_port)
        {
            case 0: /* PEM0 on QLM0-1 */
                lme = (pemx_cfg.cn83xx.lanes8) ? 0xf : 0x7;
                mlw = (pemx_cfg.cn83xx.lanes8) ? 8 : 4;
                break;
            case 1: /* PEM1 on QLM1 */
                lme = 0x7;
                mlw = 4;
                break;
            case 2: /* PEM2 on QLM2-3 or DLM4 */
            {
                BDK_CSR_INIT(pemx_qlm, node, BDK_PEMX_QLM(pcie_port));
                if (pemx_qlm.s.pem_bdlm) /* PEM2 is on DLM4 */
                {
                    lme = 0x3;
                    mlw = 2;
                }
                else /* PEM2 is on QLM2 */
                {
                    lme = (pemx_cfg.cn83xx.lanes8) ? 0xf : 0x7;
                    mlw = (pemx_cfg.cn83xx.lanes8) ? 8 : 4;
                }
                break;
            }
            case 3: /* PEM3 on QLM3 or DLM5-6 */
            {
                BDK_CSR_INIT(pemx_qlm, node, BDK_PEMX_QLM(pcie_port));
                if (pemx_qlm.s.pem_bdlm) /* PEM3 is on DLM5-6 */
                {
                    lme = (pemx_cfg.cn83xx.lanes8) ? 0x7 : 0x3;
                    mlw = (pemx_cfg.cn83xx.lanes8) ? 4 : 2;
                }
                else /* PEM3 is on QLM3 */
                {
                    lme = 0x7;
                    mlw = 4;
                }
                break;
            }
        }
    }
    /* CN80XX only supports 1 lane on PEM0 */
    if (cavium_is_altpkg(CAVIUM_CN81XX) && (pcie_port == 0))
    {
        lme = 1;
        mlw = 1;
    }

    /* Allow override of hardware max link width  */
    int max_width = bdk_config_get_int(BDK_CONFIG_PCIE_WIDTH, node, pcie_port);
    switch (max_width)
    {
        case 1:
            lme = 1;
            mlw = 1;
            break;
        case 2:
            lme = 3;
            mlw = 2;
            break;
        case 4:
            lme = 7;
            mlw = 4;
            break;
        case 8:
            lme = 0xf;
            mlw = 8;
            break;
        case 16:
            lme = 0x1f;
            mlw = 16;
            break;
        default:
            /* No change */
            break;
    }
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG452(pcie_port),
        c.s.lme = lme);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG031(pcie_port),
        c.s.mlw = mlw);

    /* Errata PEM-25990 - Disable ASLPMS */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG031(pcie_port),
        c.s.aslpms = 0);

    /* Errata PEM-26189 - PEM EQ Preset Removal */
    /* CFG554.PRV default changed from 16'h7ff to 16'h593. Should be
       safe to apply to CN88XX, CN81XX, and CN83XX */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG554(pcie_port),
        c.s.prv = bdk_config_get_int(BDK_CONFIG_PCIE_PRESET_REQUEST_VECTOR, node, pcie_port));

    /* Errata PEM-26189 - Disable the 2ms timer on all chips */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG554(pcie_port),
        c.s.p23td = 1);

    /* Errata PEM-21178 - Change the CFG[089-092] LxUTP and LxDTP defaults.
       Should be safe to apply to CN88XX, CN81XX, and CN83XX */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG089(pcie_port),
        c.s.l0dtp = 0x7;
        c.s.l0utp = 0x7;
        c.cn83xx.l1dtp = 0x7;
        c.s.l1utp = 0x7);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG090(pcie_port),
        c.s.l2dtp = 0x7;
        c.s.l2utp = 0x7;
        c.s.l3dtp = 0x7;
        c.s.l3utp = 0x7);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG091(pcie_port),
        c.s.l4dtp = 0x7;
        c.s.l4utp = 0x7;
        c.s.l5dtp = 0x7;
        c.s.l5utp = 0x7);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG092(pcie_port),
        c.s.l6dtp = 0x7;
        c.s.l6utp = 0x7;
        c.s.l7dtp = 0x7;
        c.s.l7utp = 0x7);

    /* (ECAM-27114) PCIERC has incorrect device code */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG002(pcie_port),
        c.s.sc = 0x4;
        c.s.bcc = 0x6);

    /* Errata PCIE-29440 - Atomic Egress ATOM_OP/ATOM_OP_EP not implemented
       correctly */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG038(pcie_port),
        c.s.atom_op =0x1;
        c.s.atom_op_eb=0);

    /* Errata PCIE-29566 PEM Link Hangs after going into L1 */
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG548(pcie_port),
            c.s.grizdnc = 0x0);
}

/**
 * Get the PCIe LTSSM state for the given port
 *
 * @param node      Node to query
 * @param pcie_port PEM to query
 *
 * @return LTSSM state
 */
static int __bdk_pcie_rc_get_ltssm_state(bdk_node_t node, int pcie_port)
{
    /* LTSSM state is in debug select 0 */
    BDK_CSR_WRITE(node, BDK_DTX_PEMX_SELX(pcie_port, 0), 0);
    BDK_CSR_WRITE(node, BDK_DTX_PEMX_ENAX(pcie_port, 0), 0xfffffffffull);
    /* Read the value */
    uint64_t debug = BDK_CSR_READ(node, BDK_DTX_PEMX_DATX(pcie_port, 0));
    /* Disable the PEM from driving OCLA signals */
    BDK_CSR_WRITE(node, BDK_DTX_PEMX_ENAX(pcie_port, 0), 0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return bdk_extract(debug, 0, 6); /* DBGSEL = 0x0, bits[5:0] */
    else
        return bdk_extract(debug, 3, 6); /* DBGSEL = 0x0, bits[8:3] */
}

/**
 * Get the PCIe LTSSM state for the given port
 *
 * @param node      Node to query
 * @param pcie_port PEM to query
 *
 * @return LTSSM state
 */
static const char *ltssm_string(int ltssm)
{
    switch (ltssm)
    {
        case 0x00: return "DETECT_QUIET";
        case 0x01: return "DETECT_ACT";
        case 0x02: return "POLL_ACTIVE";
        case 0x03: return "POLL_COMPLIANCE";
        case 0x04: return "POLL_CONFIG";
        case 0x05: return "PRE_DETECT_QUIET";
        case 0x06: return "DETECT_WAIT";
        case 0x07: return "CFG_LINKWD_START";
        case 0x08: return "CFG_LINKWD_ACEPT";
        case 0x09: return "CFG_LANENUM_WAIT";
        case 0x0A: return "CFG_LANENUM_ACEPT";
        case 0x0B: return "CFG_COMPLETE";
        case 0x0C: return "CFG_IDLE";
        case 0x0D: return "RCVRY_LOCK";
        case 0x0E: return "RCVRY_SPEED";
        case 0x0F: return "RCVRY_RCVRCFG";
        case 0x10: return "RCVRY_IDLE";
        case 0x11: return "L0";
        case 0x12: return "L0S";
        case 0x13: return "L123_SEND_EIDLE";
        case 0x14: return "L1_IDLE";
        case 0x15: return "L2_IDLE";
        case 0x16: return "L2_WAKE";
        case 0x17: return "DISABLED_ENTRY";
        case 0x18: return "DISABLED_IDLE";
        case 0x19: return "DISABLED";
        case 0x1A: return "LPBK_ENTRY";
        case 0x1B: return "LPBK_ACTIVE";
        case 0x1C: return "LPBK_EXIT";
        case 0x1D: return "LPBK_EXIT_TIMEOUT";
        case 0x1E: return "HOT_RESET_ENTRY";
        case 0x1F: return "HOT_RESET";
        case 0x20: return "RCVRY_EQ0";
        case 0x21: return "RCVRY_EQ1";
        case 0x22: return "RCVRY_EQ2";
        case 0x23: return "RCVRY_EQ3";
        default:   return "Unknown";
    }
}

/**
 * During PCIe link initialization we need to make config request to the attached
 * device to verify its speed and width. These config access happen very early
 * after the device is taken out of reset, so may fail for some amount of time.
 * This function automatically retries these config accesses. The normal builtin
 * hardware retry isn't enough for this very early access.
 *
 * @param node      Note to read from
 * @param pcie_port PCIe port to read from
 * @param bus       PCIe bus number
 * @param dev       PCIe device
 * @param func      PCIe function on the device
 * @param reg       Register to read
 *
 * @return Config register value, or all ones on failure
 */
static uint32_t cfg_read32_retry(bdk_node_t node, int pcie_port, int bus, int dev, int func, int reg)
{
    /* Read the PCI config register until we get a valid value. Some cards
       require time after link up to return data. Wait at most 3 seconds */
    uint64_t timeout = bdk_clock_get_count(BDK_CLOCK_TIME) + bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) * 3;
    uint32_t val;
    do
    {
        /* Read PCI capability pointer */
        val = bdk_pcie_config_read32(node, pcie_port, bus, dev, func, reg);
        /* Check the read succeeded */
        if (val != 0xffffffff)
            return val;
        /* Failed, wait a little and try again */
        bdk_wait_usec(10000);
        bdk_watchdog_poke();
    } while (bdk_clock_get_count(BDK_CLOCK_TIME) < timeout);

    BDK_TRACE(PCIE, "N%d.PCIe%d: Config read failed, can't communicate with device\n",
        node, pcie_port);
    return 0xffffffff;
}

/**
 * Initialize a host mode PCIe link. This function assumes the PEM has already
 * been taken out of reset and configure. It brings up the link and checks that
 * the negotiated speed and width is correct for the configured PEM and the
 * device plugged into it. Note that the return code will signal a retry needed
 * for some link failures. The caller is responsible for PEM reset and retry.
 *
 * @param node      Node the PEM is on
 * @param pcie_port PCIe port to initialize link on
 *
 * @return Zero on success
 *         Negative on failures where retries are not needed
 *         Positive if a retry is needed to fix a failure
 */
static int __bdk_pcie_rc_initialize_link(bdk_node_t node, int pcie_port)
{
    #define LTSSM_HISTORY_SIZE 64 /* Number of LTSSM transitions to record, must be a power of 2 */
    uint8_t ltssm_history[LTSSM_HISTORY_SIZE];
    int ltssm_history_loc;
    bool do_retry_speed = false;

    BDK_TRACE(PCIE, "N%d.PCIe%d: Checking the PEM is out of reset\n", node, pcie_port);
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_PEMX_ON(pcie_port), pemoor, ==, 1, 100000))
    {
        printf("N%d.PCIe%d: PEM in reset, skipping.\n", node, pcie_port);
        return -1;
    }

    /* Determine the maximum link speed and width */
    BDK_CSR_INIT(pciercx_cfg031, node, BDK_PCIERCX_CFG031(pcie_port));
    int max_gen = pciercx_cfg031.s.mls; /* Max speed of PEM from config (1-3) */
    int max_width = pciercx_cfg031.s.mlw; /* Max lane width of PEM (1-8) */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Link supports up to %d lanes, speed gen%d\n",
        node, pcie_port, max_width, max_gen);

    /* Record starting LTSSM state for debug */
    memset(ltssm_history, -1, sizeof(ltssm_history));
    ltssm_history[0] = __bdk_pcie_rc_get_ltssm_state(node, pcie_port);
    ltssm_history_loc = 0;

    /* Bring up the link */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Enabling the link\n", node, pcie_port);
    BDK_CSR_MODIFY(c, node, BDK_PEMX_CTL_STATUS(pcie_port), c.cn83xx.lnk_enb = 1);

    if (bdk_config_get_int(BDK_CONFIG_PCIE_SKIP_LINK_TRAIN, node, pcie_port)) {
       BDK_TRACE(PCIE, "N%d.PCIe%d: Skipping link configuration\n", node, pcie_port);
       return 0;
    }

retry_speed:
    /* Clear RC Correctable Error Status Register */
    BDK_CSR_WRITE(node, BDK_PCIERCX_CFG068(pcie_port), -1);

    /* Wait for the link to come up and link training to be complete */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Waiting for link\n", node, pcie_port);

    uint64_t clock_rate = bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME);
    uint64_t hold_time = clock_rate / 5; /* 200ms */
    uint64_t bounce_allow_time = clock_rate / 100; /* 10ms */
    uint64_t timeout = bdk_clock_get_count(BDK_CLOCK_TIME) + clock_rate; /* Timeout = 1s */
    uint64_t good_time = 0; /* Records when the link first went good */
    BDK_CSR_DEFINE(pciercx_cfg032, BDK_PCIERCX_CFG032(pcie_port));
    bool link_up;
    bool is_loop_done;
    do
    {
        /* Read link state */
        pciercx_cfg032.u = BDK_CSR_READ(node, BDK_PCIERCX_CFG032(pcie_port));

        /* Record LTSSM state for debug */
        int ltssm_state = __bdk_pcie_rc_get_ltssm_state(node, pcie_port);
        if (ltssm_history[ltssm_history_loc] != ltssm_state)
        {
            ltssm_history_loc = (ltssm_history_loc + 1) & (LTSSM_HISTORY_SIZE - 1);
            ltssm_history[ltssm_history_loc] = ltssm_state;
        }

        /* Check if the link is up */
        uint64_t current_time = bdk_clock_get_count(BDK_CLOCK_TIME);
        link_up = (pciercx_cfg032.s.dlla && !pciercx_cfg032.s.lt);
        if (link_up)
        {
            /* Is this the first link up? */
            if (!good_time)
            {
                /* Mark the time when the link transitioned to good */
                good_time = current_time;
            }
            else
            {
                /* Check for a link error */
                BDK_CSR_INIT(cfg068, node, BDK_PCIERCX_CFG068(pcie_port));
                if (cfg068.s.res)
                {
                    /* Ignore errors before we've been stable for bounce_allow_time */
                    if (good_time + bounce_allow_time <= current_time)
                    {
                        BDK_TRACE(PCIE, "N%d.PCIe%d: Link errors after link up\n", node, pcie_port);
                        return 1; /* Link error, signal a retry */
                    }
                    else
                    {
                        /* Clear RC Correctable Error Status Register */
                        BDK_CSR_WRITE(node, BDK_PCIERCX_CFG068(pcie_port), -1);
                        BDK_TRACE(PCIE, "N%d.PCIe%d: Ignored error during settling time\n", node, pcie_port);
                    }
                }
            }
        }
        else if (good_time)
        {
            if (good_time + bounce_allow_time <= current_time)
            {
                /* We allow bounces for bounce_allow_time after the link is good.
                   Once this time passes any bounce requires a retry */
                BDK_TRACE(PCIE, "N%d.PCIe%d: Link bounce detected\n", node, pcie_port);
                return 1; /* Link bounce, signal a retry */
            }
            else
            {
                BDK_TRACE(PCIE, "N%d.PCIe%d: Ignored bounce during settling time\n", node, pcie_port);
            }
        }

        /* Determine if we've hit the timeout */
        is_loop_done = (current_time >= timeout);
        /* Determine if we've had a good link for the required hold time */
        is_loop_done |= link_up && (good_time + hold_time <= current_time);
    } while (!is_loop_done);

    /* Trace the LTSSM state */
    BDK_TRACE(PCIE, "N%d.PCIe%d: LTSSM History\n", node, pcie_port);
    for (int i = 0; i < LTSSM_HISTORY_SIZE; i++)
    {
        ltssm_history_loc = (ltssm_history_loc + 1) & (LTSSM_HISTORY_SIZE - 1);
        if (ltssm_history[ltssm_history_loc] != 0xff)
            BDK_TRACE(PCIE, "N%d.PCIe%d:     %s\n",
                node, pcie_port, ltssm_string(ltssm_history[ltssm_history_loc]));
    }

    if (!link_up)
    {
        BDK_TRACE(PCIE, "N%d.PCIe%d: Link down, Data link layer %s(DLLA=%d), Link training %s(LT=%d), LTSSM %s\n",
            node, pcie_port,
            pciercx_cfg032.s.dlla ? "active" : "down", pciercx_cfg032.s.dlla,
            pciercx_cfg032.s.lt ? "active" : "complete", pciercx_cfg032.s.lt,
            ltssm_string(__bdk_pcie_rc_get_ltssm_state(node, pcie_port)));
        return 1; /* Link down, signal a retry */
    }

    /* Report the negotiated link speed and width */
    int neg_gen = pciercx_cfg032.s.ls; /* Current speed of PEM (1-3) */
    int neg_width = pciercx_cfg032.s.nlw; /* Current lane width of PEM (1-8) */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Link negotiated %d lanes, speed gen%d\n",
        node, pcie_port, neg_width, neg_gen);

    /* Determine PCIe bus number the directly attached device uses */
    BDK_CSR_INIT(pciercx_cfg006, node, BDK_PCIERCX_CFG006(pcie_port));
    int bus = pciercx_cfg006.s.sbnum;

    int dev_gen = 1; /* Device max speed (1-3) */
    int dev_width = 1; /* Device max lane width (1-16) */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Reading device max speed and width\n",
        node, pcie_port);

    /* Read PCI capability pointer */
    uint32_t cap = cfg_read32_retry(node, pcie_port, bus, 0, 0, 0x34);

    /* Check if we were able to read capabilities pointer */
    if (cap == 0xffffffff)
        return 1; /* Signal retry needed */

    /* Read device max speed and width */
    int cap_next = cap & 0xff;
    while (cap_next)
    {
        cap = cfg_read32_retry(node, pcie_port, bus, 0, 0, cap_next);
        if (cap == 0xffffffff)
            return 1; /* Signal retry needed */

        /* Is this a PCIe capability (0x10)? */
        if ((cap & 0xff) == 0x10)
        {
            BDK_TRACE(PCIE, "N%d.PCIe%d: Found PCIe capability at offset 0x%x\n",
                node, pcie_port, cap_next);
            /* Offset 0xc contains the max link info */
            cap = cfg_read32_retry(node, pcie_port, bus, 0, 0, cap_next + 0xc);
            if (cap == 0xffffffff)
                return 1; /* Signal retry needed */
            dev_gen = cap & 0xf; /* Max speed of PEM from config (1-3) */
            dev_width = (cap >> 4) & 0x3f; /* Max lane width of PEM (1-16) */
            BDK_TRACE(PCIE, "N%d.PCIe%d: Device supports %d lanes, speed gen%d\n",
                node, pcie_port, dev_width, dev_gen);
            break;
        }
        /* Move to next capability */
        cap_next = (cap >> 8) & 0xff;
    }

    /* Desired link speed and width is either limited by the device or our PEM
       configuration. Choose the most restrictive limit */
    int desired_gen = (dev_gen < max_gen) ? dev_gen : max_gen;
    int desired_width = (dev_width < max_width) ? dev_width : max_width;

    /* We need a change if we don't match the desired speed or width. Note that
       we allow better than expected in case the device lied about its
       capabilities */
    bool need_speed_change = (neg_gen < desired_gen);
    bool need_lane_change = (neg_width < desired_width);

    if (need_lane_change)
    {
        /* We didn't get the maximum number of lanes */
        BDK_TRACE(PCIE, "N%d.PCIe%d: Link width (%d) less that supported (%d)\n",
            node, pcie_port, neg_width, desired_width);
        return 2; /* Link wrong width, signal a retry */
    }
    else if (need_speed_change)
    {
        if (do_retry_speed)
        {
            BDK_TRACE(PCIE, "N%d.PCIe%d: Link speed (gen%d) less that supported (gen%d)\n",
                node, pcie_port, neg_gen, desired_gen);
            return 1; /* Link at width, but speed low. Request a retry */
        }
        else
        {
            /* We didn't get the maximum speed. Request a speed change */
            BDK_TRACE(PCIE, "N%d.PCIe%d: Link speed (gen%d) less that supported (gen%d), requesting a speed change\n",
                node, pcie_port, neg_gen, desired_gen);
            BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG515(pcie_port),
                c.s.dsc = 1);
            bdk_wait_usec(100000);
            do_retry_speed = true;
            goto retry_speed;
        }
    }
    else
    {
        BDK_TRACE(PCIE, "N%d.PCIe%d: Link at best speed and width\n", node, pcie_port);
        /* For gen3 links check if we are getting errors over the link */
        if (neg_gen == 3)
        {
            /* Read RC Correctable Error Status Register */
            BDK_CSR_INIT(cfg068, node, BDK_PCIERCX_CFG068(pcie_port));
            if (cfg068.s.res)
            {
                BDK_TRACE(PCIE, "N%d.PCIe%d: Link reporting error status\n", node, pcie_port);
                return 1; /* Getting receiver errors, request a retry */
            }
        }
        return 0; /* Link at correct speed and width */
    }
}

/**
 * Setup the SLI memory mapped regions to allow access to PCIe by the cores
 * using addresses returned by bdk_pcie_get_base_address().
 *
 * @param node      Node to configure
 * @param pcie_port PCIe port to configure
 */
static void __bdk_pcie_sli_initialize(bdk_node_t node, int pcie_port)
{
    int sli;
    int sli_group;
    __bdk_pcie_get_sli(node, pcie_port, &sli, &sli_group);

    /* Setup store merge timer */
    BDK_CSR_MODIFY(c, node, BDK_SLIX_S2M_CTL(sli),
        c.s.max_word = 0;     /* Allow 16 words to combine */
        c.s.timer = 50);      /* Wait up to 50 cycles for more data */

    /* There are 256 regions per SLI. We need four regions per PCIe port to
       support config, IO, normal, and prefetchable regions. The 256 regions
       are shared across PCIe, so we need three groups of these (one group
       for each PCIe). The setup is:
       SLI bit[7:6]: PCIe port, relative to SLI (max of 4)
       SLI bit[5:4]: Region. See bdk_pcie_mem_t enumeration
       SLI bit[3:0]: Address extension from 32 bits to 36 bits
       */
    for (bdk_pcie_mem_t mem_region = BDK_PCIE_MEM_CONFIG; mem_region <= BDK_PCIE_MEM_IO; mem_region++)
    {
        /* Use top two bits for PCIe port, next two bits for memory region */
        int sli_region = sli_group << 6;
        /* Use next two bits for mem region type */
        sli_region |= mem_region << 4;
        /* Figure out the hardware setting for each region */
        int ctype = 3;
        int nmerge = 1;
        int ordering = 0;
        switch (mem_region)
        {
            case BDK_PCIE_MEM_CONFIG: /* Config space */
                ctype = 1;      /* Config space */
                nmerge = 1;     /* No merging allowed */
                ordering = 0;   /* NO "relaxed ordering" or "no snoop" */
                break;
            case BDK_PCIE_MEM_NORMAL: /* Memory, not prefetchable */
                ctype = 0;      /* Memory space */
                nmerge = 1;     /* No merging allowed */
                ordering = 0;   /* NO "relaxed ordering" or "no snoop" */
                break;
            case BDK_PCIE_MEM_PREFETCH: /* Memory, prefetchable */
                ctype = 0;      /* Memory space */
                nmerge = 0;     /* Merging allowed */
                ordering = 1;   /* Yes "relaxed ordering" and "no snoop" */
                break;
            case BDK_PCIE_MEM_IO: /* IO */
                ctype = 2;      /* I/O space */
                nmerge = 1;     /* No merging allowed */
                ordering = 0;   /* NO "relaxed ordering" or "no snoop" */
                break;
        }
        /* Use the lower order bits to work as an address extension, allowing
           each PCIe port to map a total of 36 bits (32bit each region, 16
           regions) */
        int epf = sli_group;
        if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) || CAVIUM_IS_MODEL(CAVIUM_CN81XX)) {
            BDK_CSR_INIT(lmac_const0,node,BDK_SLIX_LMAC_CONST0X(sli,pcie_port));
            epf = lmac_const0.s.epf;
        }
        for (int r = sli_region; r < sli_region + 16; r++)
        {
            uint64_t address = 0;
            /* Address only applies to memory space */
            if (mem_region == BDK_PCIE_MEM_NORMAL)
            {
                /* Normal starts at bus address 0 */
                address = r - sli_region;
            } else if (mem_region == BDK_PCIE_MEM_PREFETCH)
            {
                /* Normal starts at bus address 0x10.0000.0000 */
                address = r - sli_region + 16;
            }
            BDK_CSR_MODIFY(c, node, BDK_SLIX_S2M_REGX_ACC(sli, r),
                c.s.ctype = ctype;
                c.s.zero = 0;
                c.cn83xx.epf = epf; /* Superimposed onto c.cn81xx.mac. EPF value works for both */
                c.s.nmerge = nmerge;
                c.s.wtype = ordering;
                c.s.rtype = ordering;
                c.s.ba = address);
        }
    }

    /* Setup MAC control */
    BDK_CSR_MODIFY(c, node, BDK_SLIX_M2S_MACX_CTL(sli, sli_group),
        c.s.waitl_com = 1; /* Improves ordering in Ali flash testing */
        c.s.ctlp_ro = 1;
        c.s.ptlp_ro = 1;
        c.s.wind_d = 1;
        c.s.bar0_d = 1;
        c.s.wait_com = (bdk_config_get_int(BDK_CONFIG_PCIE_ORDERING) == 1));
}


/**
 * Perform a complete PCIe RC reset. This is documented in the HRM as issuing a
 * fundamental reset
 *
 * @param node      Node to reset
 * @param pcie_port PCIe port to reset
 *
 * @return Zero on success, negative on failure
 */
static int __bdk_pcie_rc_reset(bdk_node_t node, int pcie_port)
{
    /* Find which QLM/DLM is associated with this PCIe port */
    int qlm = bdk_qlm_get_qlm_num(node, BDK_IF_PCIE, pcie_port, 0);
    if (qlm < 0)
        return -1;

    /* Check if this PCIe port combines two QLM/DLM */
    BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(pcie_port));
    int is_dual = CAVIUM_IS_MODEL(CAVIUM_CN81XX) ? pemx_cfg.cn81xx.lanes4 : pemx_cfg.cn83xx.lanes8;

    BDK_TRACE(PCIE, "N%d.PCIe%d: Performing PCIe fundamental reset\n", node, pcie_port);

    /* Host software may want to issue a fundamental reset to the PCIe bus.
       Software should perform the following steps:
       1.  Write PEM(0..1)_ON[PEMON] = 0. */
    BDK_CSR_MODIFY(c, node, BDK_PEMX_ON(pcie_port),
        c.s.pemon = 0);
    /* 2.  Write RST_SOFT_PRST(0..3)[SOFT_PRST] = 1.
        - This reassertion of [SOFT_PRST] causes the chip to drive PERSTn_L
            low (if RST_CTL(0..3)[RST_DRV] = 1). */
    BDK_CSR_MODIFY(c, node, BDK_RST_SOFT_PRSTX(pcie_port),
        c.s.soft_prst = 1);
    /* 3.  Read RST_SOFT_PRST(0..3). This ensures the PCIe bus is now in reset.
        - Note that PCIERCn_CFGn registers cannot be accessed when
        RST_SOFT_PRST(0..3)[SOFT_PRST] = 1. */
    BDK_CSR_READ(node, BDK_RST_SOFT_PRSTX(pcie_port));
    /* 4.  Write GSER(0..8)_PHY_CTL[PHY_RESET] = 1.
        - This puts the PHY in reset. */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_reset = 1);
    if (is_dual)
        BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm + 1),
            c.s.phy_reset = 1);
    /* Wait 10 us before proceeding to step 5. */
    bdk_wait_usec(10);
    /* 5.  Write GSERx_PHY_CTL[PHY_RESET] = 0 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_reset = 0);
    if (is_dual)
        BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm + 1),
            c.s.phy_reset = 0);

    /* Turn on PEM clocks */
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        BDK_CSR_MODIFY(c, node, BDK_PEMX_CLK_EN(pcie_port),
            c.cn83xx.pceclk_gate = 0;
            c.cn83xx.csclk_gate = 0);

    /* 6.  Wait 2 ms or more before taking the PCIe port out of reset. */
    bdk_wait_usec(2000);

    /* To take PCIe port out of reset, perform the following steps: */
    /* 1.  Write PEM(0..1)_ON[PEMON] = 1. */
    BDK_CSR_MODIFY(c, node, BDK_PEMX_ON(pcie_port),
        c.s.pemon = 1);
    /* 2.  Write RST_SOFT_PRST(0..3)[SOFT_PRST] = 0. */
    /* 3.  After RST_CTL(0..3)[RST_DONE], perform any configuration as the
       PCIe MAC has been reset. Set the PEM(0..1)_CTL_STATUS[LNK_ENB] = 1. */
    /* These steps are executed when we bring the link up. See
       bdk_pcie_rc_initialize() */
    return 0;
}

/**
 * Before PCIe link can be brought up a number of steps must be performed to
 * reset the PEM, take the PEM out of reset, initialize the PEM, initialize
 * RC config space, and initialize SLI. These steps must be performed every
 * time the PEM is reset, which may be repeated if the PCIe link doesn't come
 * up at the desired speed and width.
 *
 * @param node      Node to initialize
 * @param pcie_port PCIe port to initialize
 *
 * @return Zero on success, negative on failure
 */
static int __bdk_pcie_rc_pre_link_init(bdk_node_t node, int pcie_port)
{
    /* Make sure the PEM and GSER do a full reset before starting PCIe */
    if (__bdk_pcie_rc_reset(node, pcie_port))
    {
        bdk_error("N%d.PCIe%d: Reset failed.\n", node, pcie_port);
        return -1;
    }

    /* Bring the PCIe out of reset */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Taking port out of reset\n", node, pcie_port);
    BDK_CSR_WRITE(node, BDK_RST_SOFT_PRSTX(pcie_port), 0);

    /* Check and make sure PCIe came out of reset. If it doesn't the board
        probably hasn't wired the clocks up and the interface should be
        skipped */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Waiting for reset to complete\n", node, pcie_port);
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_RST_CTLX(pcie_port), rst_done, ==, 1, 10000))
    {
        printf("N%d.PCIe%d: Stuck in reset, skipping.\n", node, pcie_port);
        return -1;
    }

    /* Check BIST status */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Checking BIST\n", node, pcie_port);
    BDK_CSR_INIT(pemx_bist_status, node, BDK_PEMX_BIST_STATUS(pcie_port));
    if (pemx_bist_status.u)
        bdk_warn("N%d.PCIe%d: BIST FAILED (0x%016llx)\n", node, pcie_port, pemx_bist_status.u);

    /* Initialize the config space CSRs */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Setting up internal config space\n", node, pcie_port);
    __bdk_pcie_rc_initialize_config_space(node, pcie_port);

    /* Enable gen2 speed selection */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Enabling dynamic speed changes\n", node, pcie_port);
    BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG515(pcie_port),
        c.s.dsc = 1);

    /* Setup the SLI windows to allow access to this PCIe from the core */
    BDK_TRACE(PCIE, "N%d.PCIe%d: Initializing SLI\n", node, pcie_port);
    __bdk_pcie_sli_initialize(node, pcie_port);
    return 0;
}

/**
 * Initialize a PCIe port for use in host(RC) mode. It doesn't
 * enumerate the bus.
 *
 * @param pcie_port PCIe port to initialize
 *
 * @return Zero on success
 */
int bdk_pcie_rc_initialize(bdk_node_t node, int pcie_port)
{
    const int MAX_RETRIES = 2; /* Total of 3 attempts: First + 2 retries */
    int retry_count = 0;
    int result= -1,i;
    bdk_pemx_bar1_indexx_t bar1_idx;

    /* Make sure we aren't trying to setup a target mode interface in host
       mode. Sadly this bit is RAZ for CN88XX and CN81XX because the hardware
       team removed it. So much for backward compatibility */
    BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(pcie_port));
    int host_mode = CAVIUM_IS_MODEL(CAVIUM_CN83XX) ? pemx_cfg.cn83xx.hostmd : 1;
    if (!host_mode)
    {
        printf("N%d.PCIe%d: Port in endpoint mode.\n", node, pcie_port);
        return -1;
    }

    while (retry_count <= MAX_RETRIES)
    {
        if (retry_count)
            BDK_TRACE(PCIE, "N%d.PCIe%d: Starting link retry %d\n", node, pcie_port, retry_count);
        /* Perform init that must be done after PEM reset, but before link */
        if (__bdk_pcie_rc_pre_link_init(node, pcie_port))
            return -1;

        if (retry_count == MAX_RETRIES)
        {
            BDK_CSR_INIT(pciercx_cfg031, node, BDK_PCIERCX_CFG031(pcie_port));
           /* Drop speed to gen2 if link bouncing                  */
           /* Result =-1  PEM in reset                             */
           /* Result = 0: link speed and width ok no retry needed  */
           /* Result = 1: Link errors or speed change needed       */
           /* Result = 2: lane width error                         */
           if ((pciercx_cfg031.s.mls == 3) && (result != 2))
            {
                BDK_TRACE(PCIE, "N%d.PCIe%d: Dropping speed to gen2\n", node, pcie_port);
                pciercx_cfg031.s.mls = 2;
                BDK_CSR_WRITE(node, BDK_PCIERCX_CFG031(pcie_port), pciercx_cfg031.u);
                /* Set the target link speed */
                BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG040(pcie_port),
                    c.s.tls = 2);
            }
        }
        /* Bring the link up */
        result = __bdk_pcie_rc_initialize_link(node, pcie_port);
        if (result == 0)
        {
            BDK_TRACE(PCIE, "N%d.PCIe%d: Link does not need a retry\n", node, pcie_port);
            break;
        }
        else if (result > 0)
        {
            if (retry_count >= MAX_RETRIES)
            {
                BDK_TRACE(PCIE, "N%d.PCIe%d: Link requested a retry, but hit the max retries\n", node, pcie_port);
                /* If the link is down, report failure */
                BDK_CSR_INIT(pciercx_cfg032, node, BDK_PCIERCX_CFG032(pcie_port));
                bool link_up = (pciercx_cfg032.s.dlla && !pciercx_cfg032.s.lt);
                if (!link_up)
                    result = -1;
            }
            else
                BDK_TRACE(PCIE, "N%d.PCIe%d: Link requested a retry\n", node, pcie_port);
        }
        if (result < 0)
        {
            int ltssm_state = __bdk_pcie_rc_get_ltssm_state(node, pcie_port);
            printf("N%d.PCIe%d: Link timeout, probably the slot is empty (LTSSM %s)\n",
                node, pcie_port, ltssm_string(ltssm_state));
            return -1;
        }
        retry_count++;
        bdk_watchdog_poke();
    }

    /* Errata PCIE-28816: Link retrain initiated at GEN1 can cause PCIE
       link to hang. For Gen1 links we must disable equalization */
    BDK_CSR_INIT(pciercx_cfg032, node, BDK_PCIERCX_CFG032(pcie_port));
    if (pciercx_cfg032.s.ls == 1)
    {
        BDK_TRACE(PCIE, "N%d.PCIe%d: Disabling equalization for Gen1 link\n", node, pcie_port);
        BDK_CSR_MODIFY(c, node, BDK_PCIERCX_CFG548(pcie_port),
            c.s.ed = 1);
    }

    BDK_TRACE(PCIE, "N%d.PCIe%d: Setting up internal BARs\n", node, pcie_port);
    /* Disable BAR0 */
    BDK_CSR_WRITE(node, BDK_PEMX_P2N_BAR0_START(pcie_port), -1);
    /* BAR1 Starting at address 0 */
    BDK_CSR_WRITE(node, BDK_PEMX_P2N_BAR1_START(pcie_port), 0);
    /* Set BAR2 to cover all memory starting at address 0 */
    BDK_CSR_WRITE(node, BDK_PEMX_P2N_BAR2_START(pcie_port), 0);
    /* Setup BAR attributes */
    BDK_CSR_MODIFY(c, node, BDK_PEMX_BAR_CTL(pcie_port),
        c.cn83xx.bar1_siz = 1; /* 64MB BAR1 */
        c.s.bar2_enb = 1; /* BAR2 is enabled */
        c.s.bar2_cax = 0); /* Cache in L2 */

    /* Allow devices that truncate the bus address to 32-bits to reach the GITS_TRANSLATER */
    bar1_idx.u          = 0;
    bar1_idx.s.addr_idx = bdk_numa_get_address(node, BDK_GITS_TRANSLATER) >> 22;
    bar1_idx.s.addr_v   = 1;

    BDK_CSR_WRITE(node, BDK_PEMX_BAR1_INDEXX(pcie_port, 0), bar1_idx.u);

    /* The rest of the windows map linearly to match the BAR2 translation. */
    for (i = 1; i < 16; i++)
    {
        bar1_idx.s.addr_idx = i;
        BDK_CSR_WRITE(node, BDK_PEMX_BAR1_INDEXX(pcie_port, i), bar1_idx.u);
    }

    /* Display the link status */
    printf("N%d.PCIe%d: Link active, %d lanes, speed gen%d\n",
        node, pcie_port, pciercx_cfg032.s.nlw, pciercx_cfg032.s.ls);

    return 0;

}

/**
 * Return PCIe state
 *
 * @param pcie_port PCIe port to query
 *
 * @return True if port is up and running
 */
int bdk_pcie_is_running(bdk_node_t node, int pcie_port)
{
    BDK_CSR_INIT(pemx_on, node, BDK_PEMX_ON(pcie_port));
    BDK_CSR_INIT(rst_soft_prstx, node, BDK_RST_SOFT_PRSTX(pcie_port));
    BDK_CSR_INIT(pciercx_cfg032, node, BDK_PCIERCX_CFG032(pcie_port));

    if (!pemx_on.s.pemon || rst_soft_prstx.s.soft_prst)
        return 0;

    return bdk_config_get_int(BDK_CONFIG_PCIE_SKIP_LINK_TRAIN, node, pcie_port) ||
        (pciercx_cfg032.s.dlla && !pciercx_cfg032.s.lt);
}

/**
 * Shutdown a PCIe port and put it in reset
 *
 * @param pcie_port PCIe port to shutdown
 *
 * @return Zero on success
 */
int bdk_pcie_rc_shutdown(bdk_node_t node, int pcie_port)
{
    /* Check that the controller is out of reset */
    BDK_CSR_INIT(rst_ctlx, node, BDK_RST_CTLX(pcie_port));
    if (!rst_ctlx.s.rst_done)
        goto skip_idle_wait;

    /* Check if link is up */
    BDK_CSR_INIT(pciercx_cfg032, node, BDK_PCIERCX_CFG032(pcie_port));
    if ((pciercx_cfg032.s.dlla == 0) || (pciercx_cfg032.s.lt == 1))
        goto skip_idle_wait;
#if 0 // FIXME
    /* Wait for all pending operations to complete */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_PEMX_CPL_LUT_VALID(pcie_port), tag, ==, 0, 2000))
        printf("N%d.PCIe%d: Shutdown timeout\n", node, pcie_port);
#endif
skip_idle_wait:
    /* Bring down the link */
    BDK_CSR_MODIFY(c, node, BDK_PEMX_CTL_STATUS(pcie_port), c.cn83xx.lnk_enb = 0);
    /* Force reset */
    __bdk_pcie_rc_reset(node, pcie_port);
    return 0;
}

/**
 * @INTERNAL
 * Build a PCIe config space request address for a device
 *
 * @param pcie_port PCIe port to access
 * @param bus       Sub bus
 * @param dev       Device ID
 * @param fn        Device sub function
 * @param reg       Register to access
 *
 * @return 64bit IO address
 */
uint64_t pcie_build_config_addr(bdk_node_t node, int pcie_port, int bus, int dev, int fn, int reg)
{
    int num_pems = bdk_pcie_get_num_ports(node);
    if (pcie_port < num_pems)
    {
        /* Errata (SLI-22555) ECAM to off-chip PCI misroutes address. Use
           the SLI regions instead of ECAMs for config space access */
        uint64_t address = bdk_pcie_get_base_address(node, pcie_port, BDK_PCIE_MEM_CONFIG);
        /* Display the link status */
        address += (uint64_t)bus << 24;   /* Bus is bits 31:24 */
        address += dev << 19;   /* device+func is bits 23:16 */
        address += fn << 16;
        address += reg;         /* Offset is bits 11:0 */
        return address;
    }
    else if (pcie_port >= 100)
    {
        bdk_device_t device;
        memset(&device, 0, sizeof(device));
        device.node = node;
        device.ecam = pcie_port - 100;
        device.bus = bus;
        device.dev = dev;
        device.func = fn;
        return __bdk_ecam_build_address(&device, reg);
    }
    return 0;
}
