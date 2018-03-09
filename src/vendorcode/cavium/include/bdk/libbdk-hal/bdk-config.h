#ifndef __CB_BDK_CONFIG_H__
#define __CB_BDK_CONFIG_H__
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

#include <inttypes.h>
#include <bdk-devicetree.h>

/**
 * @file
 *
 * Functions for controlling the system configuration.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

#define BDK_CONFIG_MANUFACTURING_ADDRESS 0xff0000 /* 16MB - 64KB */

typedef enum
{
    /* Board manufacturing data */
    BDK_CONFIG_BOARD_MODEL,         /* No parameters */
    BDK_CONFIG_BOARD_REVISION,      /* No parameters */
    BDK_CONFIG_BOARD_SERIAL,        /* No parameters */
    BDK_CONFIG_MAC_ADDRESS,         /* No parameters */
    BDK_CONFIG_MAC_ADDRESS_NUM,     /* No parameters */
    BDK_CONFIG_MAC_ADDRESS_NUM_OVERRIDE, /* No parameters */

    /* Board generic */
    BDK_CONFIG_BMC_TWSI,            /* No parameters */
    BDK_CONFIG_WATCHDOG_TIMEOUT,    /* No parameters */
    BDK_CONFIG_TWSI_WRITE,          /* Parameters: Write number */
    BDK_CONFIG_MDIO_WRITE,          /* Parameters: Write number */

    /* Board wiring of network ports and PHYs */
    BDK_CONFIG_PHY_ADDRESS,         /* Parameters: Node, Interface, Port */
    BDK_CONFIG_BGX_ENABLE,          /* Parameters: Node, BGX, Port */
    /* Non-EBB specific SFF8104 board and alike */
    BDK_CONFIG_AQUANTIA_PHY,        /* Parameters: Node, BGX, Port */

    /* BDK Configuration params */
    BDK_CONFIG_VERSION,
    BDK_CONFIG_NUM_PACKET_BUFFERS,
    BDK_CONFIG_PACKET_BUFFER_SIZE,
    BDK_CONFIG_SHOW_LINK_STATUS,
    BDK_CONFIG_COREMASK,
    BDK_CONFIG_BOOT_MENU_TIMEOUT,
    BDK_CONFIG_BOOT_PATH_OPTION,
    BDK_CONFIG_BOOT_NEXT_STAGE,
    BDK_CONFIG_TRACE,

    /* Chip feature items */
    BDK_CONFIG_MULTI_NODE,          /* No parameters */
    BDK_CONFIG_PCIE_EA,             /* No parameters */
    BDK_CONFIG_PCIE_ORDERING,       /* No parameters */
    BDK_CONFIG_PCIE_PRESET_REQUEST_VECTOR, /* Parameters: Node, Port */
    BDK_CONFIG_PCIE_WIDTH,          /* Parameters: Node, Port */
    BDK_CONFIG_PCIE_PHYSICAL_SLOT,  /* Parameters: Node, Port */
    BDK_CONFIG_PCIE_SKIP_LINK_TRAIN, /* Parameters: Node, Port */
    BDK_CONFIG_PCIE_FLASH,          /* Parameters: Node, Port */
    BDK_CONFIG_CCPI_LANE_REVERSE,   /* No parameters */
    BDK_CONFIG_CHIP_SKU,            /* Parameter: Node */
    BDK_CONFIG_CHIP_SERIAL,         /* Parameter: Node */
    BDK_CONFIG_CHIP_UNIQUE_ID,      /* Parameter: Node */

    /* QLM related config */
    BDK_CONFIG_QLM_AUTO_CONFIG,     /* Parameters: Node */
    /* SFF8104 related QLM config */
    BDK_CONFIG_QLM_DIP_AUTO_CONFIG, /* Parameters: Node */
    BDK_CONFIG_QLM_MODE,            /* Parameters: Node, QLM */
    BDK_CONFIG_QLM_FREQ,            /* Parameters: Node, QLM */
    BDK_CONFIG_QLM_CLK,             /* Parameters: Node, QLM */
    BDK_CONFIG_QLM_TUNING_TX_SWING, /* Parameters: Node, QLM, Lane */
    BDK_CONFIG_QLM_TUNING_TX_PREMPTAP, /* Parameters: Node, QLM, Lane */
    BDK_CONFIG_QLM_TUNING_TX_GAIN,  /* Parameters: Node, QLM, Lane */
    BDK_CONFIG_QLM_TUNING_TX_VBOOST, /* Parameters: Node, QLM, Lane */
    BDK_CONFIG_QLM_CHANNEL_LOSS,    /* Parameters: Node, QLM */

    /* DRAM configuration options */
    BDK_CONFIG_DDR_SPEED,                                           /* Parameters: Node */
    BDK_CONFIG_DDR_ALT_REFCLK,                                      /* Parameters: Node */
    BDK_CONFIG_DDR_SPD_ADDR,                                        /* Parameters: DIMM, LMC, Node */
    BDK_CONFIG_DDR_SPD_DATA,                                        /* Parameters: DIMM, LMC, Node */
    BDK_CONFIG_DDR_RANKS_DQX_CTL,                                   /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_RANKS_WODT_MASK,                                 /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_PASR,                                /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_ASR,                                 /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_SRT,                                 /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,                              /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_DIC,                                 /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM,                             /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE1_DB_OUTPUT_IMPEDANCE,                 /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK,                            /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE,                          /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE,                          /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
    BDK_CONFIG_DDR_RANKS_MODE2_VREFDQ_TRAIN_EN,                     /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_RANKS_RODT_CTL,                                  /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_RANKS_RODT_MASK,                                 /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MIN_RTT_NOM_IDX,                          /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MAX_RTT_NOM_IDX,                          /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MIN_RODT_CTL,                             /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MAX_RODT_CTL,                             /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_CK_CTL,                                   /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_CMD_CTL,                                  /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_CTL_CTL,                                  /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MIN_CAS_LATENCY,                          /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_OFFSET_EN,                                /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_OFFSET,                                   /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMPUTE,                           /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMP_OFFSET,                       /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_DDR2T,                                    /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_DISABLE_SEQUENTIAL_DELAY_CHECK,           /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MAXIMUM_ADJACENT_RLEVEL_DELAY_INCREMENT,  /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_PARITY,                                   /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_FPRCH2,                                   /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MODE32B,                                  /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_MEASURED_VREF,                            /* Parameters: LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_DLL_WRITE_OFFSET,                         /* Parameters: Byte, LMC, Node */
    BDK_CONFIG_DDR_CUSTOM_DLL_READ_OFFSET,                          /* Parameters: Byte, LMC, Node */

    /* High level DRAM options */
    BDK_CONFIG_DRAM_VERBOSE,        /* Parameters: Node */
    BDK_CONFIG_DRAM_BOOT_TEST,      /* Parameters: Node */
    BDK_CONFIG_DRAM_CONFIG_GPIO,    /* No parameters */
    BDK_CONFIG_DRAM_SCRAMBLE,       /* No parameters */

    /* USB */
    BDK_CONFIG_USB_PWR_GPIO,        /* Parameters: Node, Port */
    BDK_CONFIG_USB_PWR_GPIO_POLARITY, /* Parameters: Node, Port */
    BDK_CONFIG_USB_REFCLK_SRC, /* Parameters: Node, Port */

    /* Nitrox reset - For CN88XX SC and SNT part. High drives Nitrox DC_OK high */
    BDK_CONFIG_NITROX_GPIO,         /* Parameters: Node */

    /* How EYE diagrams are captured from a QLM */
    BDK_CONFIG_EYE_ZEROS,           /* No parameters */
    BDK_CONFIG_EYE_SAMPLE_TIME,     /* No parameters */
    BDK_CONFIG_EYE_SETTLE_TIME,     /* No parameters */

    /* SGPIO */
    BDK_CONFIG_SGPIO_SCLOCK_FREQ,   /* Parameters: Node */
    BDK_CONFIG_SGPIO_PIN_POWER,     /* Parameters: Node */
    BDK_CONFIG_SGPIO_PIN_SCLOCK,    /* Parameters: Node */
    BDK_CONFIG_SGPIO_PIN_SLOAD,     /* Parameters: Node */
    BDK_CONFIG_SGPIO_PIN_SDATAOUT,  /* Parameters: Node, Dataline */

    /* VRM temperature throttling */
    BDK_CONFIG_VRM_TEMP_TRIP,       /* Parameters: Node */
    BDK_CONFIG_VRM_TEMP_HIGH,       /* Parameters: Node */
    BDK_CONFIG_VRM_TEMP_LOW,        /* Parameters: Node */
    BDK_CONFIG_VRM_THROTTLE_NORMAL, /* Parameters: Node */
    BDK_CONFIG_VRM_THROTTLE_THERM,  /* Parameters: Node */

    /* Generic GPIO, unrelated to a specific block */
    BDK_CONFIG_GPIO_PIN_SELECT,     /* Parameters: GPIO, Node */
    BDK_CONFIG_GPIO_POLARITY,       /* Parameters: GPIO, Node */

    /* PBUS */
    BDK_CONFIG_PBUS_CFG,            /* Parameters: Region, Node */
    BDK_CONFIG_PBUS_TIM,            /* Parameters: Region, Node */

    /* Trusted boot information */
    BDK_CONFIG_TRUST_CSIB,          /* No parameters */
    BDK_CONFIG_TRUST_ROT_ADDR,      /* No parameters */
    BDK_CONFIG_TRUST_BSSK_ADDR,     /* No parameters */

    __BDK_CONFIG_END
} bdk_config_t;

/**
 * Get an integer configuration item
 *
 * @param cfg_item  Config item to get. If the item takes parameters (see bdk_config_t), then the
 *                  parameters are listed following cfg_item.
 *
 * @return The value of the configuration item, or def_value if the item is not set
 */
int64_t bdk_config_get_int(bdk_config_t cfg_item, ...);

/**
 * Set an integer configuration item. Note this only sets the item in memory,
 * persistent storage is not updated.
 *
 * @param value    Configuration item value
 * @param cfg_item Config item to set. If the item takes parameters (see bdk_config_t), then the
 *                 parameters are listed following cfg_item.
 */
void bdk_config_set_int(int64_t value, bdk_config_t cfg_item, ...);

/**
 * Get a string configuration item
 *
 * @param cfg_item  Config item to get. If the item takes parameters (see bdk_config_t), then the
 *                  parameters are listed following cfg_item.
 *
 * @return The value of the configuration item, or def_value if the item is not set
 */
const char *bdk_config_get_str(bdk_config_t cfg_item, ...);

/**
 * Set the device tree used for configuration
 *
 * @param fdt    Device tree to use. Memory is assumed to be from malloc() and bdk_config takes
 *               over ownership on success
 *
 * @return Zero on success, negative on failure
 */
int bdk_config_set_fdt(const struct bdk_devicetree_key_value *fdt);

typedef enum
{
    BDK_CONFIG_TYPE_INT,
    BDK_CONFIG_TYPE_STR,
    BDK_CONFIG_TYPE_STR_LIST,
    BDK_CONFIG_TYPE_BINARY,
} bdk_config_type_t;

typedef struct
{
    const char *format;     /* Printf style format string to create the item name */
    const bdk_config_type_t ctype;/* Type of this item */
    int64_t default_value;  /* Default value when no present. String defaults are cast to pointers from this */
    const int64_t min_value;/* Minimum valid value for INT parameters. Unused for Strings */
    const int64_t max_value;/* Maximum valid value for INT parameters. Unused for Strings */
} bdk_config_info_t;

extern bdk_config_info_t config_info[];

/** @} */
#endif	/* !__CB_BDK_CONFIG_H__ */
