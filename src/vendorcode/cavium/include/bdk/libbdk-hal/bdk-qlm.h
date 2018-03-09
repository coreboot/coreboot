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

/**
 * @file
 *
 * Function and structure definitions for QLM manipulation
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

typedef enum
{
    BDK_QLM_MODE_DISABLED,  /* QLM is disabled (all chips) */
    BDK_QLM_MODE_PCIE_1X1,  /* 1 PCIe, 1 lane. Other lanes unused */
    BDK_QLM_MODE_PCIE_2X1,  /* 2 PCIe, 1 lane each */
    BDK_QLM_MODE_PCIE_1X2,  /* 1 PCIe, 2 lanes */
    BDK_QLM_MODE_PCIE_1X4,  /* 1 PCIe, 4 lanes */
    BDK_QLM_MODE_PCIE_1X8,  /* 1 PCIe, 8 lanes */
    BDK_QLM_MODE_PCIE_1X16,  /* 1 PCIe, 16 lanes (CN93XX) */

    BDK_QLM_MODE_SATA_4X1,  /* SATA, each lane independent (cn88xx) */
    BDK_QLM_MODE_SATA_2X1,  /* SATA, each lane independent (cn83xx) */

    BDK_QLM_MODE_ILK,           /* ILK 4 lanes (cn78xx) */
    BDK_QLM_MODE_SGMII_4X1,     /* SGMII, each lane independent (cn88xx) */
    BDK_QLM_MODE_SGMII_2X1,     /* SGMII, each lane independent (cn83xx) */
    BDK_QLM_MODE_SGMII_1X1,     /* SGMII, single lane (cn80xx) */
    BDK_QLM_MODE_XAUI_1X4,      /* 1 XAUI or DXAUI, 4 lanes (cn88xx), use gbaud to tell difference */
    BDK_QLM_MODE_RXAUI_2X2,     /* 2 RXAUI, 2 lanes each (cn88xx) */
    BDK_QLM_MODE_RXAUI_1X2,     /* 1 RXAUI, 2 lanes each (cn83xx) */
    BDK_QLM_MODE_OCI,           /* OCI Multichip interconnect (cn88xx) */
    BDK_QLM_MODE_XFI_4X1,       /* 4 XFI, 1 lane each (cn88xx) */
    BDK_QLM_MODE_XFI_2X1,       /* 2 XFI, 1 lane each (cn83xx) */
    BDK_QLM_MODE_XFI_1X1,       /* 1 XFI, single lane (cn80xx) */
    BDK_QLM_MODE_XLAUI_1X4,     /* 1 XLAUI, 4 lanes each (cn88xx) */
    BDK_QLM_MODE_10G_KR_4X1,    /* 4 10GBASE-KR, 1 lane each (cn88xx) */
    BDK_QLM_MODE_10G_KR_2X1,    /* 2 10GBASE-KR, 1 lane each (cn83xx) */
    BDK_QLM_MODE_10G_KR_1X1,    /* 1 10GBASE-KR, single lane (cn80xx) */
    BDK_QLM_MODE_40G_KR4_1X4,   /* 1 40GBASE-KR4, 4 lanes each (cn88xx) */
    BDK_QLM_MODE_QSGMII_4X1,    /* QSGMII is 4 SGMII on one lane (cn81xx, cn83xx) */
    BDK_QLM_MODE_25G_4X1,       /* 25G, 1 lane each (CN93XX QLMs) */
    BDK_QLM_MODE_25G_2X1,       /* 25G, 1 lane each (CN93XX DLMs) */
    BDK_QLM_MODE_50G_2X2,       /* 50G, 2 lanes each (CN93XX QLMs) */
    BDK_QLM_MODE_50G_1X2,       /* 50G, 2 lanes each (CN93XX DLMs) */
    BDK_QLM_MODE_100G_1X4,      /* 100G, 4 lanes each (CN93XX) */
    BDK_QLM_MODE_25G_KR_4X1,    /* 25G-KR, 1 lane each (CN93XX QLMs) */
    BDK_QLM_MODE_25G_KR_2X1,    /* 25G-KR, 1 lane each (CN93XX DLMs) */
    BDK_QLM_MODE_50G_KR_2X2,    /* 50G-KR, 2 lanes each (CN93XX QLMs) */
    BDK_QLM_MODE_50G_KR_1X2,    /* 50G-KR, 2 lanes each (CN93XX DLMs) */
    BDK_QLM_MODE_100G_KR4_1X4,  /* 100G-KR4, 4 lanes each (CN93XX) */
    BDK_QLM_MODE_USXGMII_4X1,   /* USXGMII, 1 lane each, 10M, 100M, 1G, 2.5G, 5G, 10G, 20G (CN93XX QLMs) */
    BDK_QLM_MODE_USXGMII_2X1,   /* USXGMII, 1 lane each, 10M, 100M, 1G, 2.5G, 5G, 10G, 20G (CN93XX QLMs) */
    BDK_QLM_MODE_LAST,
} bdk_qlm_modes_t;

typedef enum
{
    BDK_QLM_CLK_COMMON_0,
    BDK_QLM_CLK_COMMON_1,
    BDK_QLM_CLK_EXTERNAL,
    BDK_QLM_CLK_COMMON_2, /* Must be after EXTERNAL as device trees have hard coded values */
    BDK_QLM_CLK_LAST,
} bdk_qlm_clock_t;

typedef enum
{
    BDK_QLM_MODE_FLAG_ENDPOINT = 1, /* PCIe in EP instead of RC */
} bdk_qlm_mode_flags_t;

typedef enum
{
    BDK_QLM_LOOP_DISABLED,  /* No shallow loopback */
} bdk_qlm_loop_t;

typedef enum
{
    BDK_QLM_DIRECTION_TX = 1,
    BDK_QLM_DIRECTION_RX = 2,
    BDK_QLM_DIRECTION_BOTH = 3,
} bdk_qlm_direction_t;

/**
 * Types of QLM margining supported
 */
typedef enum
{
    BDK_QLM_MARGIN_VERTICAL,
    BDK_QLM_MARGIN_HORIZONTAL,
} bdk_qlm_margin_t;

/**
 * Eye diagram captures are stored in the following structure
 */
typedef struct
{
    int width;              /* Width in the x direction (time) */
    int height;             /* Height in the y direction (voltage) */
    uint32_t data[64][128]; /* Error count at location, saturates as max */
} bdk_qlm_eye_t;


/**
 * How to do the various QLM operations changes greatly
 * between chips. Each chip has its specific operations
 * stored in the structure below. The correct structure
 * is chosen based on the chip we're running on.
 */
typedef struct
{
    uint32_t chip_model;
    void (*init)(bdk_node_t node);
    int (*get_num)(bdk_node_t node);
    int (*get_lanes)(bdk_node_t node, int qlm);
    bdk_qlm_modes_t (*get_mode)(bdk_node_t node, int qlm);
    int (*set_mode)(bdk_node_t node, int qlm, bdk_qlm_modes_t mode, int baud_mhz, bdk_qlm_mode_flags_t flags);
    int (*get_gbaud_mhz)(bdk_node_t node, int qlm);
    int (*measure_refclock)(bdk_node_t node, int qlm);
    int (*get_qlm_num)(bdk_node_t node, bdk_if_t iftype, int interface, int index);
    int (*reset)(bdk_node_t node, int qlm);
    int (*enable_prbs)(bdk_node_t node, int qlm, int prbs, bdk_qlm_direction_t dir);
    int (*disable_prbs)(bdk_node_t node, int qlm);
    uint64_t (*get_prbs_errors)(bdk_node_t node, int qlm, int lane, int clear);
    void (*inject_prbs_error)(bdk_node_t node, int qlm, int lane);
    int (*enable_loop)(bdk_node_t node, int qlm, bdk_qlm_loop_t loop);
    int (*auto_config)(bdk_node_t node);
    int (*dip_auto_config)(bdk_node_t node);    
    int (*tune_lane_tx)(bdk_node_t node, int qlm, int lane, int tx_swing, int tx_pre, int tx_post, int tx_gain, int tx_vboost);
    int (*rx_equalization)(bdk_node_t node, int qlm, int lane);
    int (*eye_capture)(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_eye_t *eye);
} bdk_qlm_ops_t;

/**
 * Initialize the QLM layer
 */
extern void bdk_qlm_init(bdk_node_t node) BDK_WEAK;

/**
 * Return the number of QLMs supported for the chip
 *
 * @return Number of QLMs
 */
extern int bdk_qlm_get_num(bdk_node_t node);

/**
 * Return the number of lanes in a QLM. QLMs normally contain
 * 4 lanes, except for chips which only have half of a QLM.
 *
 * @param node   Node to use in a Numa setup. Can be an exact ID or a special
 *               value.
 * @param qlm    QLM to get lanes number for
 *
 * @return Number of lanes on the QLM
 */
extern int bdk_qlm_get_lanes(bdk_node_t node, int qlm);

/**
 * Convert a mode into a configuration variable string value
 *
 * @param mode   Mode to convert
 *
 * @return configuration value string
 */
extern const char *bdk_qlm_mode_to_cfg_str(bdk_qlm_modes_t mode);

/**
 * Convert a mode into a human understandable string
 *
 * @param mode   Mode to convert
 *
 * @return Easy to read string
 */
extern const char *bdk_qlm_mode_tostring(bdk_qlm_modes_t mode);

/**
 * Convert a configuration variable value string into a mode
 *
 * @param val  Configuration variable value
 *
 * @return mode
 */
extern bdk_qlm_modes_t bdk_qlm_cfg_string_to_mode(const char *val);

/**
 * Get the mode of a QLM as a human readable string
 *
 * @param node   Node to use in a Numa setup. Can be an exact ID or a special
 *               value.
 * @param qlm    QLM to examine
 *
 * @return String mode
 */
extern bdk_qlm_modes_t bdk_qlm_get_mode(bdk_node_t node, int qlm);

/**
 * For chips that don't use pin strapping, this function programs
 * the QLM to the specified mode
 *
 * @param node     Node to use in a Numa setup
 * @param qlm      QLM to configure
 * @param mode     Desired mode
 * @param baud_mhz Desired speed
 * @param flags    Flags to specify mode specific options
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_set_mode(bdk_node_t node, int qlm, bdk_qlm_modes_t mode, int baud_mhz, bdk_qlm_mode_flags_t flags);

/**
 * Set the QLM's clock source.
 *
 * @param node     Node to use in a Numa setup
 * @param qlm      QLM to configure
 * @param clk      Clock source for QLM
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_set_clock(bdk_node_t node, int qlm, bdk_qlm_clock_t clk);

/**
 * Get the speed (Gbaud) of the QLM in Mhz.
 *
 * @param node   Node to use in a Numa setup. Can be an exact ID or a special
 *               value.
 * @param qlm    QLM to examine
 *
 * @return Speed in Mhz
 */
extern int bdk_qlm_get_gbaud_mhz(bdk_node_t node, int qlm);

/**
 * Measure the reference clock of a QLM
 *
 * @param node   Node to use in a Numa setup. Can be an exact ID or a special
 *               value.
 * @param qlm    QLM to measure
 *
 * @return Clock rate in Hz
 */
extern int bdk_qlm_measure_clock(bdk_node_t node, int qlm);

/**
 * Lookup the hardware QLM number for a given interface type and
 * index. If the associated interface doesn't map to a QLM,
 * returns -1.
 *
 * @param node      Node to use in a Numa setup
 * @param iftype    Interface type
 * @param interface Interface index number
 * @param index     Port on the interface. Most chips use the
 *                  same mode for all ports, but there are
 *                  exceptions. For example, BGX2 on CN83XX
 *                  spans two DLMs.
 *
 * @return QLM number or -1 on failure
 */
extern int bdk_qlm_get(bdk_node_t node, bdk_if_t iftype, int interface, int index);

/**
 * Reset a QLM to its initial state
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_reset(bdk_node_t node, int qlm);

/**
 * Enable PRBS on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 * @param prbs   PRBS mode (31, etc)
 * @param dir    Directions to enable. This is so you can enable TX and later
 *               enable RX after TX has run for a time
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_enable_prbs(bdk_node_t node, int qlm, int prbs, bdk_qlm_direction_t dir);

/**
 * Disable PRBS on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_disable_prbs(bdk_node_t node, int qlm);

/**
 * Return the number of PRBS errors since PRBS started running
 *
 * @param node   Node to use in numa setup
 * @param qlm    QLM to use
 * @param lane   Which lane
 * @param clear  Clear the counter after returning its value
 *
 * @return Number of errors
 */
extern uint64_t bdk_qlm_get_prbs_errors(bdk_node_t node, int qlm, int lane, int clear);

/**
 * Inject an error into PRBS
 *
 * @param node   Node to use in numa setup
 * @param qlm    QLM to use
 * @param lane   Which lane
 */
extern void bdk_qlm_inject_prbs_error(bdk_node_t node, int qlm, int lane);

/**
 * Enable shallow loopback on a QLM
 *
 * @param node   Node to use in a numa setup
 * @param qlm    QLM to use
 * @param loop   Type of loopback. Not all QLMs support all modes
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_enable_loop(bdk_node_t node, int qlm, bdk_qlm_loop_t loop);

/**
 * Configure the TX tuning parameters for a QLM lane. The tuning parameters can
 * be specified as -1 to maintain their current value
 *
 * @param node      Node to configure
 * @param qlm       QLM to configure
 * @param lane      Lane to configure
 * @param tx_swing  Transmit swing (coef 0) Range 0-31
 * @param tx_pre    Pre cursor emphasis (Coef -1). Range 0-15
 * @param tx_post   Post cursor emphasis (Coef +1). Range 0-31
 * @param tx_gain   Transmit gain. Range 0-7
 * @param tx_vboost Transmit voltage boost. Range 0-1
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_tune_lane_tx(bdk_node_t node, int qlm, int lane, int tx_swing, int tx_pre, int tx_post, int tx_gain, int tx_vboost);

/**
 * Perform RX equalization on a QLM
 *
 * @param node   Node the QLM is on
 * @param qlm    QLM to perform RX equalization on
 * @param lane   Lane to use, or -1 for all lanes
 *
 * @return Zero on success, negative if any lane failed RX equalization
 */
extern int bdk_qlm_rx_equalization(bdk_node_t node, int qlm, int lane);

/**
 * Capture an eye diagram for the given QLM lane. The output data is written
 * to "eye".
 *
 * @param node     Node to use in numa setup
 * @param qlm      QLM to use
 * @param qlm_lane Which lane
 * @param eye      Output eye data
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_eye_capture(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_eye_t *eye);

/**
 * Display an eye diagram for the given QLM lane. The eye data can be in "eye", or
 * captured during the call if "eye" is NULL.
 *
 * @param node     Node to use in numa setup
 * @param qlm      QLM to use
 * @param qlm_lane Which lane
 * @param format   Display format. 0 = raw, 1 = Color ASCII
 * @param eye      Eye data to display, or NULL if the data should be captured.
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_eye_display(bdk_node_t node, int qlm, int qlm_lane, int format, const bdk_qlm_eye_t *eye);

/**
 * Call the board specific method of determining the required QLM configuration
 * and automatically settign up the QLMs to match. For example, on the EBB8800
 * this function queries the MCU for the current setup.
 *
 * @param node   Node to configure
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_auto_config(bdk_node_t node);

/**
 * Get the current RX margining parameter
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter value
 */
extern int64_t bdk_qlm_margin_rx_get(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type);

/**
 * Get the current RX margining parameter minimum value
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter minimum value
 */
extern int64_t bdk_qlm_margin_rx_get_min(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type);

/**
 * Get the current RX margining parameter maximum value
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter maximum value
 */
extern int64_t bdk_qlm_margin_rx_get_max(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type);

/**
 * Set the current RX margining parameter value
 *
 * @param node     Node to set margin value on
 * @param qlm      QLM to set
 * @param qlm_lane Lane to set
 * @param margin_type
 *                 Type of margining parameter to set
 * @param value    Value of margining parameter
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_margin_rx_set(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type, int value);

/**
 * Restore the supplied RX margining parameter value as if it was never set. This
 * disables any overrides in the SERDES need to perform margining
 *
 * @param node     Node to restore margin value on
 * @param qlm      QLM to restore
 * @param qlm_lane Lane to restore
 * @param margin_type
 *                 Type of margining parameter to restore
 * @param value    Value of margining parameter
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_qlm_margin_rx_restore(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type, int value);

/**
 * For Cavium SFF  query the dip switches to determine the QLM setup. Applying
 * any configuration found.
 *
 * @param node   Node to configure
 *
 * @return Zero on success, negative on failure
 */

extern int bdk_qlm_dip_auto_config(bdk_node_t node);

/** @} */
