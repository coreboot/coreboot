/*
 * Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
 * reserved.
 * Copyright 2018-present Facebook, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This file consists of data imported from bdk-config.c
 */

#include <bdk.h>
#include <libbdk-hal/bdk-config.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lame_string.h>

static struct bdk_devicetree_key_value *config_fdt;

static struct bdk_devicetree_key_value *bdk_config_duplicate(
        const struct bdk_devicetree_key_value *old,
        size_t free_space)
{
    struct bdk_devicetree_key_value *new;
    size_t len = sizeof(struct bdk_devicetree_key_value) + free_space;
    const struct bdk_devicetree_key_value *iter = old;
    while (iter->key) {
       iter++;
       len += sizeof(struct bdk_devicetree_key_value);
    }
    new = malloc(len);
    if (!new)
       return NULL;

    memcpy(new, old, len);

    return new;
}

/**
 * Set the device tree used for configuration
 *
 * @param fdt    Device tree to use. Memory is assumed to be from malloc() and bdk_config takes
 *               over ownership on success
 *
 * @return Zero on success, negative on failure
 */
int bdk_config_set_fdt(const struct bdk_devicetree_key_value *fdt)
{
    if (ENV_HAS_HEAP_SECTION)
        config_fdt = bdk_config_duplicate(fdt, 0);
    else
        config_fdt = (void *)fdt;

    return 0;
}

/**
 * Look up a configuration item in the environment and replace it.
 *
 * @param name
 *
 * @return
 */
static void set_value(const char *name, const char *val)
{
    struct bdk_devicetree_key_value *iter;
    char n[64];

    strncpy(n, name, sizeof(n));
    n[sizeof(n)-1] = '\0';

    iter = config_fdt;
    while (iter->key) {
        if (strcmp(iter->key, n) == 0) {
            // we are leaking memory here...
            iter->value = (const char *)strdup(val);
            return;
        }
        iter++;
    }
    /* Not found: Create one */
    iter = bdk_config_duplicate(config_fdt,
       sizeof(struct bdk_devicetree_key_value));
    if (!iter)
        return;

    free(config_fdt);
    config_fdt = iter;
    while (iter->key) {
        iter++;
    }
    iter->key = (const char *)strdup(name);
    iter->value = (const char *)strdup(val);
    iter++;
    iter->key = 0;
    iter->value = 0;
}

/**
 * Look up a configuration item in the environment.
 *
 * @param name
 *
 * @return
 */
static const char *get_value(const char *name)
{
    const struct bdk_devicetree_key_value *iter;
    char n[64];

    strncpy(n, name, sizeof(n));
    n[sizeof(n)-1] = '\0';

    while (*n) {
        iter = config_fdt;
        while (iter->key) {
            if (strcmp(iter->key, n) == 0)
                return iter->value;
            iter++;
        }

        char *p = strrchr(n, '.');
        if (p)
            *p = '\0';
        else
            break;
    }
    return NULL;
}

/**
 * Get an integer configuration item
 *
 * @param cfg_item  Config item to get. If the item takes parameters (see bdk_config_t), then the
 *                  parameters are listed following cfg_item.
 *
 * @return The value of the configuration item, or def_value if the item is not set
 */
int64_t bdk_config_get_int(bdk_config_t cfg_item, ...)
{
    char name[64];
    size_t count;
    int64_t tmp;

    assert(cfg_item < __BDK_CONFIG_END);

    /* Make sure the correct access function was called */
    assert(config_info[cfg_item].ctype == BDK_CONFIG_TYPE_INT);

    if (!config_fdt)
        return config_info[cfg_item].default_value;

    va_list args;
    va_start(args, cfg_item);
    vsnprintf(name, sizeof(name)-1, config_info[cfg_item].format, args);
    va_end(args);

    const char *val = get_value(name);
    if (!val)
        return config_info[cfg_item].default_value;

#if 0
        if ((val[0] == '0') && (val[1] == 'x'))
            count = sscanf(val + 2, "%lx", &tmp);
        else
            count = sscanf(val, "%li", &tmp);
#endif

    if ((val[0] == '0') && (val[1] == 'x'))
        count = str_to_hex(val + 2, &tmp);
    else
        count = str_to_int(val, &tmp);
    if (count == 1) {
        if ((tmp < config_info[cfg_item].min_value) ||
            (tmp > config_info[cfg_item].max_value)) {
            printk(BIOS_WARNING, "Out of range for %s = %s, using "
                   "default\n", name, val);
            return config_info[cfg_item].default_value;
        }
        return tmp;
    }

    printk(BIOS_WARNING, "Failed to parse %s = %s, using default\n",
           name, val);
    return config_info[cfg_item].default_value;
}

/**
 * Set an integer configuration item. Note this only sets the item in memory,
 * persistent storage is not updated.
 *
 * @param value    Configuration item value
 * @param cfg_item Config item to set. If the item takes parameters (see bdk_config_t), then the
 *                 parameters are listed following cfg_item.
 */
void bdk_config_set_int(int64_t value, bdk_config_t cfg_item, ...)
{
    char name[64], val[32];

    assert(cfg_item < __BDK_CONFIG_END);

    /* Make sure the correct access function was called */
    assert(config_info[cfg_item].ctype == BDK_CONFIG_TYPE_INT);

    if (!ENV_HAS_HEAP_SECTION)
        return;

    if (!config_fdt)
        return;

    va_list args;
    va_start(args, cfg_item);
    vsnprintf(name, sizeof(name)-1, config_info[cfg_item].format, args);
    va_end(args);

    snprintf(val, sizeof(val), "0x%016llx", value);
    set_value(name, val);
}

/**
 * Get a string configuration item
 *
 * @param cfg_item  Config item to get. If the item takes parameters (see bdk_config_t), then the
 *                  parameters are listed following cfg_item.
 *
 * @return The value of the configuration item, or def_value if the item is not set
 */
const char *bdk_config_get_str(bdk_config_t cfg_item, ...)
{
    char name[64];

    /* Make sure the correct access function was called */
    assert(config_info[cfg_item].ctype == BDK_CONFIG_TYPE_STR);

    if (!config_fdt)
        return (const char *)config_info[cfg_item].default_value;

    va_list args;
    va_start(args, cfg_item);
    vsnprintf(name, sizeof(name)-1, config_info[cfg_item].format, args);

    if (BDK_CONFIG_QLM_MODE == cfg_item) {
        char name2[64];
        vsnprintf(name2, sizeof(name2)-1,"QLM-MODE.N%d.DLM%d" , args);
        const char *val = get_value(name2);
        if (val)
            printk(BIOS_WARNING, "%s: QLM-MODE.N%%d.DLM%%d format "
                   "depricated. Please use QLM-MODE.N%%d.QLM%%d "
                   "instead\n", name2);
    }
    va_end(args);

    const char *val = get_value(name);
    if (val)
        return val;
    else
        return (const char *)config_info[cfg_item].default_value;
}

bdk_config_info_t config_info[] = {
    /* Board manufacturing data */
    [BDK_CONFIG_BOARD_MODEL] = {
        .format = "BOARD-MODEL", /* String, No parameters */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"unknown",
    },
    [BDK_CONFIG_BOARD_REVISION] = {
        .format = "BOARD-REVISION", /* String, No parameters */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"unknown",
    },
    [BDK_CONFIG_BOARD_SERIAL] = {
        .format = "BOARD-SERIAL", /* String, No parameters */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"unknown",
    },
    [BDK_CONFIG_MAC_ADDRESS] = {
        .format = "BOARD-MAC-ADDRESS", /* Int64, No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Default updated at boot based on fuses */
        .min_value = 0,
        .max_value = 0xffffffffffffll,
    },
    [BDK_CONFIG_MAC_ADDRESS_NUM] = {
        .format = "BOARD-MAC-ADDRESS-NUM", /* Int, No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 256,
    },
    [BDK_CONFIG_MAC_ADDRESS_NUM_OVERRIDE] = {
        .format = "BOARD-MAC-ADDRESS-NUM-OVERRIDE", /* Int, No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1,
        .min_value = -1,
        .max_value = 256,
    },

    /* Board generic */
    [BDK_CONFIG_BMC_TWSI] = {
        .format = "BMC-TWSI", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* TWSI bus number, -1 = disabled */
        .min_value = -1,
        .max_value = 5,
    },
    [BDK_CONFIG_WATCHDOG_TIMEOUT] = {
        .format = "WATCHDOG-TIMEOUT", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = disabled */
        .min_value = 0,
        .max_value = 10000,
    },
    [BDK_CONFIG_TWSI_WRITE] = {
        .format = "TWSI-WRITE", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_STR_LIST,
    },
    [BDK_CONFIG_MDIO_WRITE] = {
        .format = "MDIO-WRITE", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_STR_LIST,
    },

    /* Board wiring of network ports and PHYs */
    [BDK_CONFIG_PHY_ADDRESS] = {
        .format = "PHY-ADDRESS.N%d.BGX%d.P%d", /* Parameters: Node, Interface, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default to no PHY */
        .min_value = -1,
        .max_value = 0xffffffffll,
    },
    [BDK_CONFIG_BGX_ENABLE] = {
        .format = "BGX-ENABLE.N%d.BGX%d.P%d", /* Parameters: Node, BGX, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1, /* 0 = disable, 1 = enable */
        .min_value = 0,
        .max_value = 1,
    },
    /* Non-EBB specific SFF8104 board and alike */
    [BDK_CONFIG_AQUANTIA_PHY] = {
        .format = "AQUANTIA-PHY.N%d.BGX%d.P%d", /*Parameters: Node, BGX, Port */
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffffll,
    },


    /* BDK Configuration params */
    [BDK_CONFIG_VERSION] = {
        .format = "BDK-VERSION",
        .ctype = BDK_CONFIG_TYPE_STR,
    },
    [BDK_CONFIG_NUM_PACKET_BUFFERS] = {
        .format = "BDK-NUM-PACKET-BUFFERS",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Default updated at boot */
        .min_value = 0,
        .max_value = 1000000,
    },
    [BDK_CONFIG_PACKET_BUFFER_SIZE] = {
        .format = "BDK-PACKET-BUFFER-SIZE",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1024, /* bytes */
        .min_value = 128,
        .max_value = 32768,
    },
    [BDK_CONFIG_SHOW_LINK_STATUS] = {
        .format = "BDK-SHOW-LINK-STATUS",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1, /* 0 = off, 1 = on */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_COREMASK] = {
        .format = "BDK-COREMASK",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Zero means all cores */
        .min_value = 0,
        .max_value = 0xffffffffffffll,
    },
    [BDK_CONFIG_BOOT_MENU_TIMEOUT] = {
        .format = "BDK-BOOT-MENU-TIMEOUT",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 10, /* seconds */
        .min_value = 0,
        .max_value = 300,
    },
    [BDK_CONFIG_BOOT_PATH_OPTION] = {
        .format = "BDK-BOOT-PATH-OPTION",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = normal, 1 = diagnostics */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_BOOT_NEXT_STAGE] = {
        .format = "BDK-CONFIG-BOOT-NEXT-STAGE-%s",
        .ctype = BDK_CONFIG_TYPE_STR,
    },
    [BDK_CONFIG_TRACE] = {
        .format = "BDK-CONFIG-TRACE",
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* bitmask */
        .min_value = 0,
        .max_value = 0x7fffffffffffffffull,
    },

    /* Chip feature items */
    [BDK_CONFIG_MULTI_NODE] = {
        .format = "MULTI-NODE", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 2, /* 2 = Auto */
        .min_value = 0,
        .max_value = 2,
    },
    [BDK_CONFIG_PCIE_EA] = {
        .format = "PCIE-ENHANCED-ALLOCATION", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1, /* 1 = EA supported, 0 = EA not supported */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_PCIE_ORDERING] = {
        .format = "PCIE-ORDERING", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 1 = Wait for commit, 0 = Don't wait for commit */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_PCIE_PRESET_REQUEST_VECTOR] = {
        .format = "PCIE-PRESET-REQUEST-VECTOR.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0x593, /* Value for PCIERCX_CFG554[PRV] */
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_PCIE_WIDTH] = {
    .format = "PCIE-WIDTH.N%d.PORT%d", /* Parameters: Node, Port */
    .ctype = BDK_CONFIG_TYPE_INT,
    .default_value = -1, /* Width override for PCIe links */
    .min_value = -1,
    .max_value = 16,
    },
    [BDK_CONFIG_PCIE_PHYSICAL_SLOT] = {
        .format = "PCIE-PHYSICAL-SLOT.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Define which physical slot we connect to on the board */
        .min_value = -1,
        .max_value = 8191,
    },
    [BDK_CONFIG_PCIE_SKIP_LINK_TRAIN] = {
            .format = "PCIE-SKIP-LINK-TRAIN.N%d.PORT%d", /* Parameters: Node, Port */
            .ctype = BDK_CONFIG_TYPE_INT,
            .default_value = 0, /* Define which physical slot we connect to on the board */
            .min_value = 0,
            .max_value = 1,
        },
    [BDK_CONFIG_PCIE_FLASH] = {
        .format = "PCIE-FLASH.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_STR_LIST,
    },
    [BDK_CONFIG_CCPI_LANE_REVERSE] = {
        .format = "CCPI-LANE-REVERSE", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = No forced lane reversal, 1 = forced lane reversal */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_CHIP_SKU] = {
        .format = "CHIP-SKU.NODE%d", /* Parameter: Node */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"TBD",
    },
    [BDK_CONFIG_CHIP_SERIAL] = {
        .format = "CHIP-SERIAL.NODE%d", /* Parameter: Node */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"TBD",
    },
    [BDK_CONFIG_CHIP_UNIQUE_ID] = {
        .format = "CHIP-UNIQUE-ID.NODE%d", /* Parameter: Node */
        .ctype = BDK_CONFIG_TYPE_STR,
        .default_value = (long)"TBD",
    },

    /* QLM related config */
    [BDK_CONFIG_QLM_AUTO_CONFIG] = {
        .format = "QLM-AUTO-CONFIG", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = off, 1 = on */
        .min_value = 0,
        .max_value = 1,
    },
    /* SFF8104 related QLM config */
    [BDK_CONFIG_QLM_DIP_AUTO_CONFIG] = {
        .format = "QLM-DIP-AUTO-CONFIG", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = off, 1 = on */
        .min_value = 0,
        .max_value = 1,
    },

    [BDK_CONFIG_QLM_MODE] = {
        .format = "QLM-MODE.N%d.QLM%d", /* Parameters: Node, QLM */
        .ctype = BDK_CONFIG_TYPE_STR,
    },
    [BDK_CONFIG_QLM_FREQ] = {
        .format = "QLM-FREQ.N%d.QLM%d", /* Parameters: Node, QLM */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Mhz */
        .min_value = 0,
        .max_value = 10312,
    },
    [BDK_CONFIG_QLM_CLK] = {
        .format = "QLM-CLK.N%d.QLM%d", /* Parameters: Node, QLM */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 2, /* 2 = External */
        .min_value = 0,
        .max_value = 2,
    },
    [BDK_CONFIG_QLM_TUNING_TX_SWING] = {
        .format = "QLM-TUNING-TX-SWING.N%d.QLM%d.LANE%d", /* Parameters: Node, QLM, Lane */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default of no tuning */
        .min_value = -1,
        .max_value = 31,
    },
    [BDK_CONFIG_QLM_TUNING_TX_PREMPTAP] = {
        .format = "QLM-TUNING-TX-PREMPTAP.N%d.QLM%d.LANE%d", /* Parameters: Node, QLM, Lane */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default of no tuning */
        .min_value = -1,
        .max_value = 511,
    },
    [BDK_CONFIG_QLM_TUNING_TX_GAIN] = {
        .format = "QLM-TUNING-TX-GAIN.N%d.QLM%d.LANE%d", /* Parameters: Node, QLM, Lane */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default of no tuning */
        .min_value = -1,
        .max_value = 7,
    },
    [BDK_CONFIG_QLM_TUNING_TX_VBOOST] = {
        .format = "QLM-TUNING-TX-VBOOST.N%d.QLM%d.LANE%d", /* Parameters: Node, QLM, Lane */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default of no tuning */
        .min_value = -1,
        .max_value = 1,
    },
    [BDK_CONFIG_QLM_CHANNEL_LOSS] = {
        .format = "QLM-CHANNEL-LOSS.N%d.QLM%d", /* Parameters: Node, QLM */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Default will use Cavium defaults */
        .min_value = -1,
        .max_value = 40,
    },
    /* DRAM configuration options */
    [BDK_CONFIG_DDR_SPEED] = {
        .format = "DDR-SPEED.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* In MT/s */
        .min_value = 0,
        .max_value = 2400,
    },
    [BDK_CONFIG_DDR_ALT_REFCLK] = {
        .format = "DDR-ALT-REFCLK.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Mhz */
        .min_value = 0,
        .max_value = 100,
    },
    [BDK_CONFIG_DDR_SPD_ADDR] = {
        .format = "DDR-CONFIG-SPD-ADDR.DIMM%d.LMC%d.N%d", /* Parameters: DIMM, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_SPD_DATA] = {
        .format = "DDR-CONFIG-SPD-DATA.DIMM%d.LMC%d.N%d", /* Parameters: DIMM, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_BINARY,
    },
    [BDK_CONFIG_DDR_RANKS_DQX_CTL] = {
        .format = "DDR-CONFIG-DQX-CTL.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xf,
    },
    [BDK_CONFIG_DDR_RANKS_WODT_MASK] = {
        .format = "DDR-CONFIG-WODT-MASK.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xfffffff,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_PASR] = {
        .format = "DDR-CONFIG-MODE1-PASR.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x7,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_ASR] = {
        .format = "DDR-CONFIG-MODE1-ASR.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_SRT] = {
        .format = "DDR-CONFIG-MODE1-SRT.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR] = {
        .format = "DDR-CONFIG-MODE1-RTT-WR.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // Split for extension bit
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x7,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_DIC] = {
        .format = "DDR-CONFIG-MODE1-DIC.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x3,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM] = {
        .format = "DDR-CONFIG-MODE1-RTT-NOM.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x7,
    },
    [BDK_CONFIG_DDR_RANKS_MODE1_DB_OUTPUT_IMPEDANCE] = {
        .format = "DDR-CONFIG-MODE1-DB-OUTPUT-IMPEDANCE.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // Not per RANK, only one
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x7,
    },
    [BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK] = {
        .format = "DDR-CONFIG-MODE2-RTT-PARK.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x7,
    },
    [BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE] = {
        .format = "DDR-CONFIG-MODE2-VREF-VALUE.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0x3f,
    },
    [BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE] = {
        .format = "DDR-CONFIG-MODE2-VREF-RANGE.RANKS%d.DIMMS%d.RANK%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, Rank, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_RANKS_MODE2_VREFDQ_TRAIN_EN] = {
        .format = "DDR-CONFIG-MODE2-VREFDQ-TRAIN-EN.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // Not per RANK, only one
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },

    [BDK_CONFIG_DDR_RANKS_RODT_CTL] = {
        .format = "DDR-CONFIG-RODT-CTL.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xf,
    },
    [BDK_CONFIG_DDR_RANKS_RODT_MASK] = {
        .format = "DDR-CONFIG-RODT-MASK.RANKS%d.DIMMS%d.LMC%d.N%d", /* Parameters: Num Ranks, Num DIMMs, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xfffffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_MIN_RTT_NOM_IDX] = {
        .format = "DDR-CONFIG-CUSTOM-MIN-RTT-NOM-IDX.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1,
        .min_value = 0,
        .max_value = 7,
    },
    [BDK_CONFIG_DDR_CUSTOM_MAX_RTT_NOM_IDX] = {
        .format = "DDR-CONFIG-CUSTOM-MAX-RTT-NOM-IDX.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 5,
        .min_value = 0,
        .max_value = 7,
    },
    [BDK_CONFIG_DDR_CUSTOM_MIN_RODT_CTL] = {
        .format = "DDR-CONFIG-CUSTOM-MIN-RODT-CTL.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1,
        .min_value = 0,
        .max_value = 7,
    },
    [BDK_CONFIG_DDR_CUSTOM_MAX_RODT_CTL] = {
        .format = "DDR-CONFIG-CUSTOM-MAX-RODT-CTL.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 5,
        .min_value = 0,
        .max_value = 7,
    },
    [BDK_CONFIG_DDR_CUSTOM_CK_CTL] = {
        .format = "DDR-CONFIG-CUSTOM-CK-CTL.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_CMD_CTL] = {
        .format = "DDR-CONFIG-CUSTOM-CMD-CTL.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_CTL_CTL] = {
        .format = "DDR-CONFIG-CUSTOM-CTL-CTL.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xf,
    },
    [BDK_CONFIG_DDR_CUSTOM_MIN_CAS_LATENCY] = {
        .format = "DDR-CONFIG-CUSTOM-MIN-CAS-LATENCY.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_OFFSET_EN] = {
        .format = "DDR-CONFIG-CUSTOM-OFFSET-EN.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_OFFSET] = {
        .format = "DDR-CONFIG-CUSTOM-OFFSET.%s.LMC%d.N%d", /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // UDIMM or RDIMM
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xf,
    },
    [BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMPUTE] = {
        .format = "DDR-CONFIG-CUSTOM-RLEVEL-COMPUTE.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMP_OFFSET] = {
        .format = "DDR-CONFIG-CUSTOM-RLEVEL-COMP-OFFSET.%s.LMC%d.N%d", /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // UDIMM or RDIMM
        .default_value = 2,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_DDR2T] = {
        .format = "DDR-CONFIG-CUSTOM-DDR2T.%s.LMC%d.N%d", /* Parameters: Type(UDIMM,RDIMM), LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT, // UDIMM or RDIMM
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_DISABLE_SEQUENTIAL_DELAY_CHECK] = {
        .format = "DDR-CONFIG-CUSTOM-DISABLE-SEQUENTIAL-DELAY-CHECK.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_MAXIMUM_ADJACENT_RLEVEL_DELAY_INCREMENT] = {
        .format = "DDR-CONFIG-CUSTOM-MAXIMUM-ADJACENT-RLEVEL-DELAY-INCREMENT.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xffff,
    },
    [BDK_CONFIG_DDR_CUSTOM_PARITY] = {
        .format = "DDR-CONFIG-CUSTOM-PARITY.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_FPRCH2] = {
        .format = "DDR-CONFIG-CUSTOM-FPRCH2.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 0xf,
    },
    [BDK_CONFIG_DDR_CUSTOM_MODE32B] = {
        .format = "DDR-CONFIG-CUSTOM-MODE32B.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_MEASURED_VREF] = {
        .format = "DDR-CONFIG-CUSTOM-MEASURED-VREF.LMC%d.N%d", /* Parameters: LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DDR_CUSTOM_DLL_WRITE_OFFSET] = {
        .format = "DDR-CONFIG-CUSTOM-DLL-WRITE-OFFSET.BYTE%d.LMC%d.N%d", /* Parameters: Byte, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = -63,
        .max_value = 63,
    },
    [BDK_CONFIG_DDR_CUSTOM_DLL_READ_OFFSET] = {
        .format = "DDR-CONFIG-CUSTOM-DLL-READ-OFFSET.BYTE%d.LMC%d.N%d", /* Parameters: Byte, LMC, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0,
        .min_value = -63,
        .max_value = 63,
    },

    /* High level DRAM options */
    [BDK_CONFIG_DRAM_VERBOSE] = {
        .format = "DDR-VERBOSE", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = off */
        .min_value = 0,
        .max_value = 255,
    },
    [BDK_CONFIG_DRAM_BOOT_TEST] = {
        .format = "DDR-TEST-BOOT", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* 0 = off, 1 = on */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_DRAM_CONFIG_GPIO] = {
        .format = "DDR-CONFIG-GPIO", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* -1 = disabled, otherwise GPIO number */
        .min_value = -1,
        .max_value = 63,
    },
    [BDK_CONFIG_DRAM_SCRAMBLE] = {
        .format = "DDR-CONFIG-SCRAMBLE", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 2, /* 0=off, 1=on, 2=trust on, non-trust off */
        .min_value = 0,
        .max_value = 2,
    },

    /* USB */
    [BDK_CONFIG_USB_PWR_GPIO] = {
        .format = "USB-PWR-GPIO.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 49,
    },
    [BDK_CONFIG_USB_PWR_GPIO_POLARITY] = {
        .format = "USB-PWR-GPIO-POLARITY.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 1, /* GPIO polarity: 1=high, 0=low */
        .min_value = 0,
        .max_value = 1,
    },
    [BDK_CONFIG_USB_REFCLK_SRC] = {
        .format = "USB-REFCLK-SRC.N%d.PORT%d", /* Parameters: Node, Port */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Clock Source (SS:HS)
                            ** 0 - SS(USB_REF_CLK)   HS(USB_REF_CLK)
                            ** 1 - SS(DLMC_REF_CLK0) HS(DLMC_REF_CLK0)
                            ** 2 - SS(DLMC_REF_CLK1) HS(DLMC_REF_CLK1)
                            ** 3 - SS(USB_REF_CLK)   HS(PLL_REF_CLK)
                            ** 4 - SS(DLMC_REF_CLK0) HS(PLL_REF_CLK)
                            ** 5 - SS(DLMC_REF_CLK1) HS(PLL_REF_CLK)
                            */
        .min_value = 0,
        .max_value = 5,
    },

    /* Nitrox reset - For CN88XX SC and SNT part. High drives Nitrox DC_OK high */
    [BDK_CONFIG_NITROX_GPIO] = {
        .format = "NITROX-GPIO.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 49,
    },

    /* How EYE diagrams are captured from a QLM */
    [BDK_CONFIG_EYE_ZEROS] = {
        .format = "QLM-EYE-NUM-ZEROS", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 2,
        .min_value = 1,
        .max_value = 63,
    },
    [BDK_CONFIG_EYE_SAMPLE_TIME] = {
        .format = "QLM-EYE-SAMPLE-TIME", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 400, /* us */
        .min_value = 20, /* us */
        .max_value = 10000000, /* us */
    },
    [BDK_CONFIG_EYE_SETTLE_TIME] = {
        .format = "QLM-EYE-SETTLE-TIME", /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 50, /* us */
        .min_value = 20, /* us */
        .max_value = 100000, /* us */
    },

    /* SGPIO */
    [BDK_CONFIG_SGPIO_SCLOCK_FREQ] = {
        .format = "SGPIO-SCLOCK-FREQ.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 10000, /* Hz */
        .min_value = 128, /* Hz */
        .max_value = 100000, /* Hz */
    },
    [BDK_CONFIG_SGPIO_PIN_POWER] = {
        .format = "SGPIO-PIN-POWER.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 50,
    },
    [BDK_CONFIG_SGPIO_PIN_SCLOCK] = {
        .format = "SGPIO-PIN-SCLOCK.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 50,
    },
    [BDK_CONFIG_SGPIO_PIN_SLOAD] = {
        .format = "SGPIO-PIN-SLOAD.N%d", /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 50,
    },
    [BDK_CONFIG_SGPIO_PIN_SDATAOUT] = {
        .format = "SGPIO-PIN-SDATAOUT.N%d.D%d", /* Parameters: Node, Dataline */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* GPIO number, or -1 for none */
        .min_value = -1,
        .max_value = 50,
    },

    /* VRM temperature throttling */
    [BDK_CONFIG_VRM_TEMP_TRIP] = {
        .format = "VRM-TEMP-TRIP.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 110, /* Degrees C */
        .min_value = 0, /* Degrees C */
        .max_value = 110, /* Degrees C. Max die temp plus 5 for uncertainty of measurement */
    },
    [BDK_CONFIG_VRM_TEMP_HIGH] = {
        .format = "VRM-TEMP-HIGH.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 110, /* Degrees C */
        .min_value = 0, /* Degrees C */
        .max_value = 110, /* Degrees C. Max die temp plus 5 for uncertainty of measurement */
    },
    [BDK_CONFIG_VRM_TEMP_LOW] = {
        .format = "VRM-TEMP-LOW.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 100, /* Degrees C */
        .min_value = 0, /* Degrees C */
        .max_value = 110, /* Degrees C. Max die temp plus 5 for uncertainty of measurement */
    },
    [BDK_CONFIG_VRM_THROTTLE_NORMAL] = {
        .format = "VRM-THROTTLE-NORMAL.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 65, /* Percentage */
        .min_value = 1, /* Percentage */
        .max_value = 100, /* Percentage */
    },
    [BDK_CONFIG_VRM_THROTTLE_THERM] = {
        .format = "VRM-THROTTLE-THERM.N%d",  /* Parameters: Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 15, /* Percentage */
        .min_value = 1, /* Percentage */
        .max_value = 100, /* Percentage */
    },

    /* Generic GPIO, unrelated to a specific block */
    [BDK_CONFIG_GPIO_PIN_SELECT] = {
        .format = "GPIO-PIN-SELECT-GPIO%d.N%d",  /* Parameters: GPIO, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = -1, /* Hardware default, normal GPIO pin */
        .min_value = 0, /* GPIO_PIN_SEL_E enumeration */
        .max_value = 65535, /* GPIO_PIN_SEL_E enumeration */
    },
    [BDK_CONFIG_GPIO_POLARITY] = {
        .format = "GPIO-POLARITY-GPIO%d.N%d",  /* Parameters: GPIO, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Hardware default, not inverted */
        .min_value = 0, /* Not inverted */
        .max_value = 1, /* Inverted */
    },

    /* PBUS */
    [BDK_CONFIG_PBUS_CFG] = {
        .format = "PBUS-CFG.REGION%d.N%d",  /* Parameters: Region, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Hardware default */
        .min_value = 0, /* No change */
        .max_value = 0x0000ffffffffffffll, /* PBUS_REGX_CFG value */
    },
    [BDK_CONFIG_PBUS_TIM] = {
        .format = "PBUS-TIM.REGION%d.N%d",  /* Parameters: Region, Node */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Hardware default, not inverted */
        .min_value = 0x8000000000000000ll, /* PBUS_REGX_TIM value, zero is no change */
        .max_value = 0x7fffffffffffffffll, /* PBUS_REGX_TIM value */
    },

    /* Trusted boot information */
    [BDK_CONFIG_TRUST_CSIB] = {
        .format = "TRUST-CSIB",  /* No parameters */
        .ctype = BDK_CONFIG_TYPE_BINARY,
        .default_value = 0, /* Hardware default */
    },
    [BDK_CONFIG_TRUST_ROT_ADDR] = {
        .format = "TRUST-ROT-ADDR",  /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* Non-trusted */
        .min_value = 0, /* No key */
        .max_value = 0x0000ffffffffffffll, /* Address in key memory */
    },
    [BDK_CONFIG_TRUST_BSSK_ADDR] = {
        .format = "TRUST-BSSK-ADDR",  /* No parameters */
        .ctype = BDK_CONFIG_TYPE_INT,
        .default_value = 0, /* No HUK, so no BSSK */
        .min_value = 0, /* No HUK, so no BSSK */
        .max_value = 0x0000ffffffffffffll, /* Address in key memory */
    },
};

