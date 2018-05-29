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
#include "libbdk-arch/bdk-csrs-fusf.h"
#include "libbdk-arch/bdk-csrs-rom.h"

/* The define BDK_TRUST_HARD_BLOW_NV controls whether the BDK will
   hard blow the secure NV counter on boot. This is needed for a
   production system, but can be dangerous in a development
   environment. The default value of 0 is to prevent bricking of
   chips due to CSIB[NVCOUNT] mistakes. BDK_TRUST_HARD_BLOW_NV must
   be changed to a 1 for production. The code below will display a
   warning if BDK_TRUST_HARD_BLOW_NV=0 in a trusted boot to remind
   you */
#define BDK_TRUST_HARD_BLOW_NV 0

/* The CSIB used to boot will be stored here by bsk-start.S */
union bdk_rom_csib_s __bdk_trust_csib __attribute__((section("init")));
static bdk_trust_level_t __bdk_trust_level = BDK_TRUST_LEVEL_BROKEN;

/**
 * Update the fused secure NV counter to reflect the CSIB[NVCOUNT] value. In
 * production systems, be sure to set BDK_TRUST_HARD_BLOW_NV=1.
 */
static void __bdk_program_nv_counter(void)
{
    int hw_nv = bdk_trust_get_nv_counter();
    int csib_nv = __bdk_trust_csib.s.nvcnt;

    if (!BDK_TRUST_HARD_BLOW_NV)
    {
        printf("\33[1m"); /* Bold */
        bdk_warn("\n");
        bdk_warn("********************************************************\n");
        bdk_warn("* Configured for soft blow of secure NV counter. This\n");
        bdk_warn("* build is not suitable for production trusted boot.\n");
        bdk_warn("********************************************************\n");
        bdk_warn("\n");
        printf("\33[0m"); /* Normal */
    }

    /* Check if the CSIB NV counter is less than the HW fused values.
       This means the image is an old rollback. Refuse to run */
    if (csib_nv < hw_nv)
        bdk_fatal("CSIB[NVCOUNT] is less than FUSF_CTL[ROM_T_CNT]. Image rollback not allowed\n");
    /* If the CSIB NV counter matches the HW fuses, everything is
       good */
    if (csib_nv == hw_nv)
        return;
    /* CSIB NV counter is larger than the HW fuses. We must blow
       fuses to move the hardware counter forward, protecting from
       image rollback */
    if (BDK_TRUST_HARD_BLOW_NV)
    {
        BDK_TRACE(INIT, "Trust: Hard blow secure NV counter to %d\n", csib_nv);
        uint64_t v = 1ull << BDK_FUSF_FUSE_NUM_E_ROM_T_CNTX(csib_nv - 1);
        bdk_fuse_field_hard_blow(bdk_numa_master(), BDK_FUSF_FUSE_NUM_E_FUSF_LCK, v, 0);
    }
    else
    {
        BDK_TRACE(INIT, "Trust: Soft blow secure NV counter to %d\n", csib_nv);
        bdk_fuse_field_soft_blow(bdk_numa_master(), BDK_FUSF_FUSE_NUM_E_ROM_T_CNTX(csib_nv - 1));
    }
}

/**
 * Called by boot stub (TBL1FW) to initialize the state of trust
 */
void __bdk_trust_init(void)
{
    extern uint64_t __bdk_init_reg_pc; /* The contents of PC when this image started */
    const bdk_node_t node = bdk_numa_local();
    volatile uint64_t *huk = bdk_phys_to_ptr(bdk_numa_get_address(node, BDK_FUSF_HUKX(0)));

    /* Non-trusted boot address */
    if (__bdk_init_reg_pc == 0x120000)
    {
        __bdk_trust_level = BDK_TRUST_LEVEL_NONE;
        if (huk[0] | huk[1])
        {
            BDK_TRACE(INIT, "Trust: Initial image, Non-trusted boot with HUK\n");
            goto fail_trust;
        }
        else
        {
            BDK_TRACE(INIT, "Trust: Initial image, Non-trusted boot without HUK\n");
            goto skip_trust;
        }
    }

    if (__bdk_init_reg_pc != 0x150000)
    {
        /* Not the first image */
        BDK_CSR_INIT(rst_boot, node, BDK_RST_BOOT);
        if (!rst_boot.s.trusted_mode)
        {
            __bdk_trust_level = BDK_TRUST_LEVEL_NONE;
            BDK_TRACE(INIT, "Trust: Secondary image, non-trusted boot\n");
            goto skip_trust;
        }
        int csibsize = 0;
        const union bdk_rom_csib_s *csib = bdk_config_get_blob(&csibsize, BDK_CONFIG_TRUST_CSIB);
        if (!csib)
        {
            __bdk_trust_level = BDK_TRUST_LEVEL_NONE;
            BDK_TRACE(INIT, "Trust: Secondary image, non-trusted boot\n");
            goto skip_trust;
        }
        if (csibsize != sizeof(__bdk_trust_csib))
        {
            BDK_TRACE(INIT, "Trust: Secondary image, Trusted boot with corrupt CSIB, trust broken\n");
            goto fail_trust;
        }
        /* Record our trust level */
        switch (csib->s.crypt)
        {
            case 0:
                __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED;
                BDK_TRACE(INIT, "Trust: Secondary image, Trused boot, no encryption\n");
                goto success_trust;
            case 1:
                __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED_SSK;
                BDK_TRACE(INIT, "Trust: Secondary image, Trused boot, SSK encryption\n");
                goto success_trust;
            case 2:
                __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED_BSSK;
                BDK_TRACE(INIT, "Trust: Secondary image, Trused boot, BSSK encryption\n");
                goto success_trust;
            default:
                __bdk_trust_level = BDK_TRUST_LEVEL_BROKEN;
                BDK_TRACE(INIT, "Trust: Secondary image, Trusted boot, Corrupt CSIB[crypt], trust broken\n");
                goto fail_trust;
        }
    }

    /* Copy the Root of Trust public key out of the CSIB */
    volatile uint64_t *rot_pub_key = bdk_key_alloc(node, 64);
    if (!rot_pub_key)
    {
        __bdk_trust_level = BDK_TRUST_LEVEL_BROKEN;
        BDK_TRACE(INIT, "Trust: Failed to allocate ROT memory, trust broken\n");
        goto fail_trust;
    }
    rot_pub_key[0] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk0);
    rot_pub_key[1] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk1);
    rot_pub_key[2] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk2);
    rot_pub_key[3] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk3);
    rot_pub_key[4] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk4);
    rot_pub_key[5] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk5);
    rot_pub_key[6] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk6);
    rot_pub_key[7] = bdk_le64_to_cpu(__bdk_trust_csib.s.rotpk7);
    bdk_config_set_int(bdk_ptr_to_phys((void*)rot_pub_key), BDK_CONFIG_TRUST_ROT_ADDR);
    BDK_TRACE(INIT, "Trust: ROT %016lx %016lx %016lx %016lx %016lx %016lx %016lx %016lx\n",
        bdk_cpu_to_be64(rot_pub_key[0]), bdk_cpu_to_be64(rot_pub_key[1]),
        bdk_cpu_to_be64(rot_pub_key[2]), bdk_cpu_to_be64(rot_pub_key[3]),
        bdk_cpu_to_be64(rot_pub_key[4]), bdk_cpu_to_be64(rot_pub_key[5]),
        bdk_cpu_to_be64(rot_pub_key[6]), bdk_cpu_to_be64(rot_pub_key[7]));

    /* Update the secure NV counter with the value in the CSIB */
    __bdk_program_nv_counter();

    /* Create the BSSK */
    if (huk[0] | huk[1])
    {
        uint64_t iv[2] = {0, 0};
        volatile uint64_t *bssk = bdk_key_alloc(node, 16);
        if (!bssk)
        {
            __bdk_trust_level = BDK_TRUST_LEVEL_BROKEN;
            BDK_TRACE(INIT, "Trust: Failed to allocate BSSK memory, trust broken\n");
            goto fail_trust;
        }
        BDK_TRACE(INIT, "Trust: Calculating BSSK\n");
        uint64_t tmp_bssk[2];
        tmp_bssk[0] = __bdk_trust_csib.s.fs0;
        tmp_bssk[1] = __bdk_trust_csib.s.fs1;
        bdk_aes128cbc_decrypt((void*)huk, (void*)tmp_bssk, 16, iv);
        bssk[0] = tmp_bssk[0];
        bssk[1] = tmp_bssk[1];
        tmp_bssk[0] = 0;
        tmp_bssk[1] = 0;
        bdk_config_set_int(bdk_ptr_to_phys((void*)bssk), BDK_CONFIG_TRUST_BSSK_ADDR);
        //BDK_TRACE(INIT, "Trust: BSSK %016lx %016lx\n", bdk_cpu_to_be64(bssk[0]), bdk_cpu_to_be64(bssk[1]));
    }

    /* Record our trust level */
    switch (__bdk_trust_csib.s.crypt)
    {
        case 0:
            __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED;
            BDK_TRACE(INIT, "Trust: Trused boot, no encryption\n");
            break;
        case 1:
            __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED_SSK;
            BDK_TRACE(INIT, "Trust: Trused boot, SSK encryption\n");
            break;
        case 2:
            __bdk_trust_level = BDK_TRUST_LEVEL_SIGNED_BSSK;
            BDK_TRACE(INIT, "Trust: Trused boot, BSSK encryption\n");
            break;
        default:
            __bdk_trust_level = BDK_TRUST_LEVEL_BROKEN;
            goto fail_trust;
    }

    /* We started at the trusted boot address, CSIB should be
       valid */
    bdk_config_set_blob(sizeof(__bdk_trust_csib), &__bdk_trust_csib, BDK_CONFIG_TRUST_CSIB);
success_trust:
    bdk_signed_load_public();
    return;

fail_trust:
    /* Hide secrets  */
    BDK_CSR_MODIFY(c, node, BDK_RST_BOOT,
        c.s.dis_huk = 1);
    BDK_TRACE(INIT, "Trust: Secrets Hidden\n");
skip_trust:
    /* Erase CSIB as it is invalid */
    memset(&__bdk_trust_csib, 0, sizeof(__bdk_trust_csib));
    bdk_config_set_blob(0, NULL, BDK_CONFIG_TRUST_CSIB);
}

/**
 * Returns the current level of trust. Must be called after
 * __bdk_trust_init()
 *
 * @return Enumerated trsut level, see bdk_trust_level_t
 */
bdk_trust_level_t bdk_trust_get_level(void)
{
    return __bdk_trust_level;
}

/**
 * Return the current secure NV counter stored in the fuses
 *
 * @return NV counter (0-31)
 */
int bdk_trust_get_nv_counter(void)
{
    /* Count leading zeros in FUSF_CTL[ROM_T_CNT] to dermine the
       hardware NV value */
    BDK_CSR_INIT(fusf_ctl, bdk_numa_master(), BDK_FUSF_CTL);
    int hw_nv = 0;
    if (fusf_ctl.s.rom_t_cnt)
        hw_nv = 32 - __builtin_clz(fusf_ctl.s.rom_t_cnt);
    return hw_nv;
}

