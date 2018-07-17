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
#include <libbdk-arch/bdk-csrs-mio_fus.h>
#include <libbdk-dram/bdk-dram-config.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-gpio.h>
#include <libbdk-hal/bdk-l2c.h>
#include <libbdk-hal/bdk-utils.h>
#include <libbdk-os/bdk-init.h>
#include <libdram/libdram-config.h>
#include "dram-internal.h"
#include <stddef.h>                /* for NULL */
#include <lame_string.h>        /* for strtol() and friends */


/* This global variable is accessed through dram_is_verbose() to determine
   the verbosity level. Use that function instead of setting it directly */
dram_verbosity_t dram_verbosity = VBL_OFF; /* init this here so we could set a non-zero default */

static uint32_t measured_ddr_hertz[BDK_NUMA_MAX_NODES];

/* The various DRAM configs in the libdram/configs directory need space
   to store the DRAM config. Since only one config is ever in active use
   at a time, store the configs in __libdram_global_cfg. In a multi-node
   setup, independent calls to get the DRAM config will load first node 0's
   config, then node 1's */
dram_config_t __libdram_global_cfg;

static void bdk_dram_clear_mem(bdk_node_t node)
{
     uint64_t mbytes = bdk_dram_get_size_mbytes(node);
     uint64_t skip = (node == bdk_numa_master()) ? bdk_dram_get_top_of_bdk() : 0;
     uint64_t len =  (mbytes << 20) - skip;

     BDK_TRACE(DRAM, "N%d: Clearing DRAM\n", node);
     if (skip)
     {
         /* All memory below skip may contain valid data, so we can't clear
            it. We still need to make sure all cache lines in this area are
            fully dirty so that ECC bits will be updated on store. A single
            write to the cache line isn't good enough because partial LMC
            writes may be enabled */
         ddr_print("N%d: Rewriting DRAM: start 0 length 0x%llx\n", node, skip);
         volatile uint64_t *ptr = bdk_phys_to_ptr(bdk_numa_get_address(node, 8));
         /* The above pointer got address 8 to avoid NULL pointer checking
            in bdk_phys_to_ptr(). Correct it here */
         ptr--;
         uint64_t *end = bdk_phys_to_ptr(bdk_numa_get_address(node, skip));
         while (ptr < end)
         {
             *ptr = *ptr;
             ptr++;
         }
     }
     ddr_print("N%d: Clearing DRAM: start 0x%llx length 0x%llx\n", node, skip, len);
     bdk_zero_memory(bdk_phys_to_ptr(bdk_numa_get_address(node, skip)), len);
     BDK_TRACE(DRAM, "N%d: DRAM clear complete\n", node);
}

static void bdk_dram_clear_ecc(bdk_node_t node)
{
    /* Clear any DRAM errors set during init */
    BDK_TRACE(DRAM, "N%d: Clearing LMC ECC errors\n", node);
    int num_lmc = __bdk_dram_get_num_lmc(node);
    for (int lmc = 0; lmc < num_lmc; lmc++) {
        DRAM_CSR_WRITE(node, BDK_LMCX_INT(lmc), BDK_CSR_READ(node, BDK_LMCX_INT(lmc)));
    }
}

static void bdk_dram_enable_ecc_reporting(bdk_node_t node)
{
    /* Enable LMC ECC error HW reporting */
    int num_lmc = __bdk_dram_get_num_lmc(node);

    BDK_TRACE(DRAM, "N%d: Enable LMC ECC error reporting\n", node);

    for (int lmc = 0; lmc < num_lmc; lmc++) {

        // NOTE: this must be done for pass 2.x
        // enable ECC interrupts to allow ECC error info in LMCX_INT
        if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
            DRAM_CSR_WRITE(node, BDK_LMCX_INT_ENA_W1S(lmc), -1ULL);
            BDK_CSR_INIT(lmc_int_ena_w1s, node, BDK_LMCX_INT_ENA_W1S(lmc));
            ddr_print("N%d.LMC%d: %-36s : 0x%08llx\n",
                      node, lmc, "LMC_INT_ENA_W1S", lmc_int_ena_w1s.u);
        }
    }
}

static void bdk_dram_disable_ecc_reporting(bdk_node_t node)
{
    /* Disable LMC ECC error HW reporting */
    int num_lmc = __bdk_dram_get_num_lmc(node);

    BDK_TRACE(DRAM, "N%d: Disable LMC ECC error reporting\n", node);

    for (int lmc = 0; lmc < num_lmc; lmc++) {

        // NOTE: this must be done for pass 2.x
        // disable ECC interrupts to prevent ECC error info in LMCX_INT
        if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
            DRAM_CSR_WRITE(node, BDK_LMCX_INT_ENA_W1C(lmc), -1ULL);
            BDK_CSR_INIT(lmc_int_ena_w1c, node, BDK_LMCX_INT_ENA_W1C(lmc));
            ddr_print("N%d.LMC%d: %-36s : 0x%08llx\n",
                      node, lmc, "LMC_INT_ENA_W1C", lmc_int_ena_w1c.u);
        }
    }
}

// this routine simply makes the calls to the tuning routines and returns any errors
static int bdk_libdram_tune_node(int node)
{
    int errs, tot_errs;
    int do_dllro_hw = IS_ENABLED(CONFIG_CAVIUM_BDK_DDR_TUNE_HW_OFFSETS);
    int do_dllwo = IS_ENABLED(CONFIG_CAVIUM_BDK_DDR_TUNE_WRITE_OFFSETS);
    int do_eccdll = IS_ENABLED(CONFIG_CAVIUM_BDK_DDR_TUNE_ECC_ENABLE);
    BDK_CSR_INIT(lmc_config, node, BDK_LMCX_CONFIG(0)); // FIXME: probe LMC0
    do_eccdll = (lmc_config.s.ecc_ena != 0); // change to ON if ECC enabled

    // Automatically tune the data byte DLL read offsets
    // always done by default, but allow use of HW-assist
    // NOTE: HW-assist will also tune the ECC byte 

    BDK_TRACE(DRAM, "N%d: Starting DLL Read Offset Tuning for LMCs\n", node);
    if (!do_dllro_hw || (lmc_config.s.mode32b != 0)) {
        errs = perform_dll_offset_tuning(node, 2, /* tune */1); 
    } else {
        errs = perform_HW_dll_offset_tuning(node, /* read */2, 0x0A/* all bytelanes */); 
    }
    BDK_TRACE(DRAM, "N%d: Finished DLL Read Offset Tuning for LMCs, %d errors)\n",
              node, errs);
    tot_errs = errs;

    // disabled by default for now, does not seem to be needed?
    // Automatically tune the data byte DLL write offsets
    // allow override of default setting
    if (do_dllwo) {
        BDK_TRACE(DRAM, "N%d: Starting DLL Write Offset Tuning for LMCs\n", node);
        errs = perform_dll_offset_tuning(node, /* write */1, /* tune */1);
        BDK_TRACE(DRAM, "N%d: Finished DLL Write Offset Tuning for LMCs, %d errors)\n",
               node, errs);
        tot_errs += errs;
    }

    // disabled by default for now, does not seem to be needed much?
    // Automatically tune the ECC byte DLL read offsets
    // FIXME? allow override of the filtering
    // FIXME? allow programmatic override, not via envvar? 
    if (do_eccdll && !do_dllro_hw && (lmc_config.s.mode32b == 0)) { // do not do HW-assist twice for ECC
        BDK_TRACE(DRAM, "N%d: Starting ECC DLL Read Offset Tuning for LMCs\n", node);
        errs = perform_HW_dll_offset_tuning(node, 2, 8/* ECC bytelane */); 
        BDK_TRACE(DRAM, "N%d: Finished ECC DLL Read Offset Tuning for LMCs, %d errors\n",
                  node, errs);
        tot_errs += errs;
    }

    return tot_errs;
}

// this routine makes the calls to the tuning routines when criteria are met
// intended to be called for automated tuning, to apply filtering...

#define IS_DDR4  1
#define IS_DDR3  0
#define IS_RDIMM 1
#define IS_UDIMM 0
#define IS_1SLOT 1
#define IS_2SLOT 0

// FIXME: DDR3 is not tuned
static const uint32_t ddr_speed_filter[2][2][2] = {
    [IS_DDR4] = { 
        [IS_RDIMM] = {
            [IS_1SLOT] =  940,
            [IS_2SLOT] =  800
        },
        [IS_UDIMM] = {
            [IS_1SLOT] = 1050,
            [IS_2SLOT] =  940
        },
    },
    [IS_DDR3] = {
        [IS_RDIMM] = {
            [IS_1SLOT] =    0, // disabled
            [IS_2SLOT] =    0  // disabled
        },
        [IS_UDIMM] = {
            [IS_1SLOT] =    0, // disabled
            [IS_2SLOT] =    0  // disabled
        }
    }
};

static int bdk_libdram_maybe_tune_node(int node)
{
    const char *str;

    // FIXME: allow an override here so that all configs can be tuned or none
    // If the envvar is defined, always either force it or avoid it accordingly
    if ((str = getenv("ddr_tune_all_configs")) != NULL) {
        int tune_it = !!strtoul(str, NULL, 0);
        printf("N%d: DRAM auto-tuning %s.\n", node, (tune_it) ? "forced" : "avoided");
        return (tune_it) ? bdk_libdram_tune_node(node) : 0;
    }

    // filter the tuning calls here...
    // determine if we should/can run automatically for this configuration
    //
    // FIXME: tune only when the configuration indicates it will help:
    //    DDR type, RDIMM or UDIMM, 1-slot or 2-slot, and speed
    //
    uint32_t ddr_speed = divide_nint(libdram_get_freq_from_pll(node, 0), 1000000); // sample LMC0
    BDK_CSR_INIT(lmc_config, node, BDK_LMCX_CONFIG(0)); // sample LMC0

    int is_ddr4  = !!__bdk_dram_is_ddr4(node, 0);
    int is_rdimm = !!__bdk_dram_is_rdimm(node, 0);
    int is_1slot = !!(lmc_config.s.init_status < 4); // HACK, should do better
    int do_tune = 0;

    uint32_t ddr_min_speed = ddr_speed_filter[is_ddr4][is_rdimm][is_1slot];
    do_tune = (ddr_min_speed && (ddr_speed > ddr_min_speed));

    ddr_print("N%d: DDR%d %cDIMM %d-slot at %d MHz %s eligible for auto-tuning.\n",
              node, (is_ddr4)?4:3, (is_rdimm)?'R':'U', (is_1slot)?1:2,
              ddr_speed, (do_tune)?"is":"is not");

    // call the tuning routines, done filtering...
    return ((do_tune) ? bdk_libdram_tune_node(node) : 0);
}

/**
 * This is the main DRAM init function. Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to initialize. This may not be the same node as the one running the code
 * @param dram_config
 *               DRAM configuration to use
 * @param ddr_clock_override
 *               If non-zeo, this overrides the DRAM clock speed in the config structure. This
 *               allows quickly testing of different DRAM speeds without modifying the basic
 *               config. If zero, the DRAM speed in the config is used.
 *
 * @return Amount of memory in MB. Zero or negative is a failure.
 */
int libdram_config(int node, const dram_config_t *dram_config, int ddr_clock_override)
{
    /* Boards may need to mux the TWSI connection between THUNDERX and the BMC.
       This allows the BMC to monitor DIMM temeratures and health */
    int gpio_select = bdk_config_get_int(BDK_CONFIG_DRAM_CONFIG_GPIO);
    if (gpio_select != -1)
        bdk_gpio_initialize(bdk_numa_master(), gpio_select, 1, 1);

    /* Read all the SPDs and store them in the device tree. They are needed by
       later software to populate SMBIOS information */
    for (int lmc = 0; lmc < 4; lmc++)
        for (int dimm = 0; dimm < DDR_CFG_T_MAX_DIMMS; dimm++)
            read_entire_spd(node, (dram_config_t *)dram_config, lmc, dimm);

    const ddr_configuration_t *ddr_config = dram_config->config;
    int ddr_clock_hertz = (ddr_clock_override) ? ddr_clock_override : dram_config->ddr_clock_hertz;
    if (ddr_clock_hertz == 0)  // 0 == AUTO
    {
        ddr_clock_hertz = dram_get_default_spd_speed(node, ddr_config);
        if (ddr_clock_hertz < 0) {
            printf("N%d: DRAM init: AUTO clock ILLEGAL configuration\n", node);
            return -1;
        }
    }
    int errs;

    // At this point, we only know the desired clock rate (ddr_clock_hertz).
    // We do not know whether we are configuring RDIMMs.
    // We also do not yet know if 100MHz alternate refclk is actually available.
    // so, if we are being asked for 2133MT/s or better, we still need to do:
    // 1. probe for RDIMMs (if not, 50MHz refclk is good enough)
    // 2. determine if 100MHz refclk is there, and switch to it before starting any configuration
    //
    // NOTES:
    // 1. dclk_alt_refclk_sel need only be set on LMC0 (see above disabled code)
    // 2. I think we need to first probe to see if we need it, and configure it then if dictated use
    // 3. then go on to configure at the selected refclk
    int ddr_refclk_hertz = bdk_clock_get_rate(node, BDK_CLOCK_MAIN_REF);
    int alt_refclk = bdk_config_get_int(BDK_CONFIG_DDR_ALT_REFCLK, node);

    char *str = getenv("ddr_100mhz_refclk");
    if (str) { // if the envvar was found, force it to that setting
        int do_100mhz = !!strtoul(str, NULL, 0);
        alt_refclk = (do_100mhz) ? 100 : 50;
    }

    dram_verbosity = bdk_config_get_int(BDK_CONFIG_DRAM_VERBOSE);

    // Here we check for fuses that limit the number of LMCs we can configure,
    //  but only on 83XX and 88XX...
    int lmc_limit = 4;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) || CAVIUM_IS_MODEL(CAVIUM_CN83XX)) {
        BDK_CSR_INIT(mio_fus_dat2, node, BDK_MIO_FUS_DAT2);
        if (mio_fus_dat2.s.lmc_half) {
            lmc_limit = (CAVIUM_IS_MODEL(CAVIUM_CN88XX)) ? 2 : 1; // limit LMCs to half present
            error_print("Only %d LMC(s)s supported for this Thunder model\n", lmc_limit);
        }
    }

    /* We need to calculate the interface mask based on the provided SPD
       addresses/contents */
    uint32_t interface_mask = 0;
    for (int i = 0; i < lmc_limit; i++)
    {
        // We need to check only DIMM 0 of each LMC for possible presence of the LMC.
        // This trusts that the board database is correctly configured.
        // Empty DIMM slots in present LMCs will be detected later.
        if (ddr_config[i].dimm_config_table[0].spd_addr ||
            ddr_config[i].dimm_config_table[0].spd_ptr)
            interface_mask |= 1 << i;

        // we know whether alternate refclk is always wanted
        // we also know already if we want 2133 MT/s
        // if alt refclk not always wanted, then probe DDR and DIMM type
        // if DDR4 and RDIMMs, then set desired refclk to 100MHz, otherwise to default (50MHz)
        // depend on ddr_initialize() to do the refclk selection and validation
        if (i == 0) { // only check for LMC0
            if (alt_refclk) { // if alternate refclk was specified, let it override everything
                ddr_refclk_hertz = alt_refclk * 1000000;
                ddr_print("N%d: DRAM init: %d MHz refclk is REQUESTED ALWAYS\n", node, alt_refclk);
            } else if (ddr_clock_hertz > 1000000000) { // if more than 2000 MT/s
                int ddr_type = get_ddr_type(node, &ddr_config[0].dimm_config_table[0]);
                int spd_dimm_type = get_dimm_module_type(node, &ddr_config[0].dimm_config_table[0], ddr_type);
                // is DDR4 and RDIMM just to be sure
                if ((ddr_type == DDR4_DRAM) &&
                    ((spd_dimm_type == 1) || (spd_dimm_type == 5) || (spd_dimm_type == 8))) {
                    ddr_refclk_hertz = 100000000; // yes, we require 100MHz refclk, so set it
                    ddr_print("N%d: DRAM init: 100 MHz refclk is REQUIRED\n", node);
                }
            } // if (ddr_clock_hertz > 1000000000)
        } // if (i == 0)
    }

    BDK_TRACE(DRAM, "N%d: DRAM init started (hertz=%d, refclk=%d, config=%p)\n",
              node, ddr_clock_hertz, ddr_refclk_hertz, dram_config);
    debug_print("N%d: DRAM init started (hertz=%d, refclk=%d, config=%p)\n",
                node, ddr_clock_hertz, ddr_refclk_hertz, dram_config);

    BDK_TRACE(DRAM, "N%d: Calling DRAM init\n", node);
    measured_ddr_hertz[node] = 0;
    int mbytes = octeon_ddr_initialize(node,
        bdk_clock_get_rate(node, BDK_CLOCK_RCLK),
        ddr_clock_hertz,
        ddr_refclk_hertz,
        interface_mask,
        ddr_config,
        &measured_ddr_hertz[node],
        0,
        0,
        0);
    BDK_TRACE(DRAM, "N%d: DRAM init returned %d, measured %u Hz\n",
              node, mbytes, measured_ddr_hertz[node]);

    // do not tune or mess with memory if there was an init problem...
    if (mbytes > 0) {

        bdk_dram_disable_ecc_reporting(node);

        // call the tuning routines, with filtering...
        BDK_TRACE(DRAM, "N%d: Calling DRAM tuning\n", node);
        errs = bdk_libdram_maybe_tune_node(node);
        BDK_TRACE(DRAM, "N%d: DRAM tuning returned %d errors\n",
                  node, errs);

        // finally, clear memory and any left-over ECC errors
        bdk_dram_clear_mem(node);
        bdk_dram_clear_ecc(node);

        bdk_dram_enable_ecc_reporting(node);
    }

    /* Boards may need to mux the TWSI connection between THUNDERX and the BMC.
       This allows the BMC to monitor DIMM temeratures and health */
    if (gpio_select != -1)
        bdk_gpio_initialize(bdk_numa_master(), gpio_select, 1, 0);

    return mbytes;
}

/**
 * This is the main DRAM tuning function. Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to tune. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
int libdram_tune(int node)
{
    int tot_errs;
    int l2c_is_locked = bdk_l2c_is_locked(node);

    dram_verbosity = bdk_config_get_int(BDK_CONFIG_DRAM_VERBOSE);

    // the only way this entry point should be called is from a MENU item,
    // so, enable any non-running cores on this node, and leave them
    // running at the end...
    ddr_print("N%d: %s: Starting cores (mask was 0x%llx)\n",
              node, __FUNCTION__, bdk_get_running_coremask(node));
    bdk_init_cores(node, ~0ULL);

    // must test for L2C locked here, cannot go on with it unlocked
    // FIXME: but we only need to worry about Node 0???
    if (node == 0) {
        if (!l2c_is_locked) { // is unlocked, must lock it now
            ddr_print("N%d: %s: L2C was unlocked - locking it now\n", node, __FUNCTION__);
            // FIXME: this should be common-ized; it currently matches bdk_init()...
            bdk_l2c_lock_mem_region(node, 0, bdk_l2c_get_cache_size_bytes(node) * 3 / 4);
        } else {
            ddr_print("N%d: %s: L2C was already locked - continuing\n", node, __FUNCTION__);
        }
    } else {
        ddr_print("N%d: %s: non-zero node, not worrying about L2C lock status\n", node, __FUNCTION__);
    }

    // call the tuning routines, no filtering...
    tot_errs = bdk_libdram_tune_node(node);

    // FIXME: only for node 0, unlock L2C if it was unlocked before...
    if (node == 0) {
        if (!l2c_is_locked) { // it was Node 0 and unlocked, must re-unlock it now
            ddr_print("N%d: Node 0 L2C was unlocked before - unlocking it now\n", node);
            // FIXME: this should be common-ized; it currently matches bdk_init()...
            bdk_l2c_unlock_mem_region(node, 0, bdk_l2c_get_cache_size_bytes(node) * 3 / 4);
        } else {
            ddr_print("N%d: %s: L2C was already locked - leaving it locked\n", node, __FUNCTION__);
        }
    } else {
        ddr_print("N%d: %s: non-zero node, not worrying about L2C lock status\n", node, __FUNCTION__);
    }

    // make sure to clear memory and any ECC errs when done... 
    bdk_dram_clear_mem(node);
    bdk_dram_clear_ecc(node);

    return tot_errs;
}

/**
 * This is the main function for DRAM margining of Write Voltage.
 * Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to test. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
static
int libdram_margin_write_voltage(int node)
{
    int tot_errs;

    // call the margining routine
    tot_errs = perform_margin_write_voltage(node);

    // make sure to clear memory and any ECC errs when done... 
    bdk_dram_clear_mem(node);
    bdk_dram_clear_ecc(node);

    return tot_errs;
}

/**
 * This is the main function for DRAM margining of Read Voltage.
 * Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to test. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
static
int libdram_margin_read_voltage(int node)
{
    int tot_errs;

    // call the margining routine
    tot_errs = perform_margin_read_voltage(node);

    // make sure to clear memory and any ECC errs when done... 
    bdk_dram_clear_mem(node);
    bdk_dram_clear_ecc(node);

    return tot_errs;
}

/**
 * This is the main function for DRAM margining of Write Timing.
 * Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to test. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
static
int libdram_margin_write_timing(int node)
{
    int tot_errs;

    // call the tuning routine, tell it we are margining not tuning...
    tot_errs = perform_dll_offset_tuning(node, /* write offsets */1, /* margin */0);

    // make sure to clear memory and any ECC errs when done... 
    bdk_dram_clear_mem(node);
    bdk_dram_clear_ecc(node);

    return tot_errs;
}

/**
 * This is the main function for DRAM margining of Read Timing.
 * Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to test. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
static
int libdram_margin_read_timing(int node)
{
    int tot_errs;

    // call the tuning routine, tell it we are margining not tuning...
    tot_errs = perform_dll_offset_tuning(node, /* read offsets */2, /* margin */0);

    // make sure to clear memory and any ECC errs when done... 
    bdk_dram_clear_mem(node);
    bdk_dram_clear_ecc(node);

    return tot_errs;
}

/**
 * This is the main function for all DRAM margining.
 * Users of libdram should call this function,
 * avoiding the other internal function. As a rule, functions starting with
 * "libdram_*" are part of the external API and should be used.
 *
 * @param node   Node to test. This may not be the same node as the one running the code
 *
 * @return Success or Fail
 */
int libdram_margin(int node)
{
    int ret_rt, ret_wt, ret_rv, ret_wv;
    const char *risk[2] = { "Low Risk", "Needs Review" };
    int l2c_is_locked = bdk_l2c_is_locked(node);

    // for now, no margining on 81xx, until we can reduce the dynamic runtime size... 
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX)) {
        printf("Sorry, margining is not available on 81xx yet...\n");
        return 0;
    }

    dram_verbosity = bdk_config_get_int(BDK_CONFIG_DRAM_VERBOSE);

    // the only way this entry point should be called is from a MENU item,
    // so, enable any non-running cores on this node, and leave them
    // running at the end...
    ddr_print("N%d: %s: Starting cores (mask was 0x%llx)\n",
              node, __FUNCTION__, bdk_get_running_coremask(node));
    bdk_init_cores(node, ~0ULL);

    // must test for L2C locked here, cannot go on with it unlocked
    // FIXME: but we only need to worry about Node 0???
    if (node == 0) {
        if (!l2c_is_locked) { // is unlocked, must lock it now
            ddr_print("N%d: %s: L2C was unlocked - locking it now\n", node, __FUNCTION__);
            // FIXME: this should be common-ized; it currently matches bdk_init()...
            bdk_l2c_lock_mem_region(node, 0, bdk_l2c_get_cache_size_bytes(node) * 3 / 4);
        } else {
            ddr_print("N%d: %s: L2C was already locked - continuing\n", node, __FUNCTION__);
        }
    } else {
        ddr_print("N%d: %s: non-zero node, not worrying about L2C lock status\n", node, __FUNCTION__);
    }

    debug_print("N%d: Starting DRAM Margin ALL\n", node);
    ret_rt = libdram_margin_read_timing(node);
    ret_wt = libdram_margin_write_timing(node);
    ret_rv = libdram_margin_read_voltage(node);
    ret_wv = libdram_margin_write_voltage(node);
    debug_print("N%d: DRAM Margin ALL finished\n", node);

    /*
      >>> Summary from DDR Margining tool:
      >>> N0: Read Timing Margin   : Low Risk
      >>> N0: Write Timing Margin  : Low Risk
      >>> N0: Read Voltage Margin  : Low Risk
      >>> N0: Write Voltage Margin : Low Risk  
     */
    printf("  \n");
    printf("-------------------------------------\n");
    printf("  \n");
    printf("Summary from DDR Margining tool\n");
    printf("N%d: Read Timing Margin   : %s\n", node, risk[!!ret_rt]);
    printf("N%d: Write Timing Margin  : %s\n", node, risk[!!ret_wt]);

    // these may not have been done due to DDR3 and/or THUNDER pass 1.x
    // FIXME? would it be better to print an appropriate message here? 
    if (ret_rv != -1) printf("N%d: Read Voltage Margin  : %s\n", node, risk[!!ret_rv]);
    if (ret_wv != -1) printf("N%d: Write Voltage Margin : %s\n", node, risk[!!ret_wv]);

    printf("  \n");
    printf("-------------------------------------\n");
    printf("  \n");

    // FIXME: only for node 0, unlock L2C if it was unlocked before...
    if (node == 0) {
        if (!l2c_is_locked) { // it was Node 0 and unlocked, must re-unlock it now
            ddr_print("N%d: Node 0 L2C was unlocked before - unlocking it now\n", node);
            // FIXME: this should be common-ized; it currently matches bdk_init()...
            bdk_l2c_unlock_mem_region(node, 0, bdk_l2c_get_cache_size_bytes(node) * 3 / 4);
        } else {
            ddr_print("N%d: %s: L2C was already locked - leaving it locked\n", node, __FUNCTION__);
        }
    } else {
        ddr_print("N%d: %s: non-zero node, not worrying about L2C lock status\n", node, __FUNCTION__);
    }

    return 0;
}

/**
 * Get the measured DRAM frequency after a call to libdram_config
 *
 * @param node   Node to get frequency for
 *
 * @return Frequency in Hz
 */
uint32_t libdram_get_freq(int node)
{
    return measured_ddr_hertz[node];
}

/**
 * Get the measured DRAM frequency from the DDR_PLL_CTL CSR
 *
 * @param node   Node to get frequency for
 *
 * @return Frequency in Hz
 */
uint32_t libdram_get_freq_from_pll(int node, int lmc)
{
    static const uint8_t _en[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12};
    BDK_CSR_INIT(c, node, BDK_LMCX_DDR_PLL_CTL(0));
    // we check the alternate refclk select bit in LMC0 to indicate 100MHz use
    // assumption: the alternate refclk is setup for 100MHz
    uint64_t ddr_ref_hertz = (c.s.dclk_alt_refclk_sel) ? 100000000 : bdk_clock_get_rate(node, BDK_CLOCK_MAIN_REF);
    uint64_t en = _en[c.cn83xx.ddr_ps_en];
    uint64_t calculated_ddr_hertz = ddr_ref_hertz * (c.cn83xx.clkf + 1) / ((c.cn83xx.clkr + 1) * en);
    return calculated_ddr_hertz;
}

#ifndef DRAM_CSR_WRITE_INLINE
void dram_csr_write(bdk_node_t node, const char *csr_name, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value)
{
    VB_PRT(VBL_CSRS, "N%d: DDR Config %s[%016llx] => %016llx\n", node, csr_name, address, value);
    bdk_csr_write(node, type, busnum, size, address, value);
}
#endif
