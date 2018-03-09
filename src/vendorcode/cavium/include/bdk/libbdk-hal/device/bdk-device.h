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
 * bdk_device_t represents devices connected using ECAMs. This
 * are discover by scanning the ECAMs and instantiating devices
 * for what is found.
 *
 *  The discovery process for a device is: Scan all ECAMs:
 *      1) Device found on an ECAM that doesn't have a bdk_device_t
 *      2) bdk_device_t created, put in
 *          BDK_DEVICE_STATE_NOT_PROBED state
 *  For all devices in state BDK_DEVICE_STATE_NOT_PROBED:
 *      1) Lookup driver probe() function. If not found, skip
 *      2) Call probe() 3) Based on probe(), transition to
 *          either BDK_DEVICE_STATE_PROBED or
 *          BDK_DEVICE_STATE_PROBE_FAIL
 *  For all devices in state BDK_DEVICE_STATE_PROBED:
 *      1) Lookup driver init() function. If not found, skip
 *      2) Call init() 3) Based on init(), transition to either
 *          BDK_DEVICE_STATE_READY or BDK_DEVICE_STATE_INIT_FAIL
 *  In general all devices should transition to
 *  BDK_DEVICE_STATE_PROBED before any init() functions are
 *  called. This can be used for synchronization. For example,
 *  the FPA should be functional after a probe() so PKI/PKO can
 *  succeed when calling alloc in init().
 *
 * @defgroup device ECAM Attached Devices
 * @addtogroup device
 * @{
 */

/**
 * Possible states of a device
 */
typedef enum
{
    BDK_DEVICE_STATE_NOT_PROBED,    /* Device is known and offline. We haven't probed it */
    BDK_DEVICE_STATE_PROBE_FAIL,    /* Device failed probing and is offline */
    BDK_DEVICE_STATE_PROBED,        /* Device succeeded probing, about to go online */
    BDK_DEVICE_STATE_INIT_FAIL,     /* Device init call failed, offline */
    BDK_DEVICE_STATE_READY,         /* Device init call success, online */
} bdk_device_state_t;

/**
 * The structure of a ECAM BAR entry inside if a device
 */
typedef struct
{
    uint64_t address;   /* Base physical address */
    uint32_t size2;     /* Size in bytes as 2^size */
    uint32_t flags;     /* Type flags for the BAR */
} bdk_device_bar_t;

/**
 * Defines the BDK's representation of a ECAM connected device
 */
typedef struct
{
    char name[16];                  /* Name of the device */
    bdk_device_bar_t    bar[4];     /* Device BARs, first for better alignment */
    bdk_device_state_t  state : 8;  /* Current state of bdk_device_t */
    bdk_node_t          node : 3;   /* Node the device is on */
    uint8_t             ecam : 5;   /* ECAM for the device */
    uint8_t             bus;        /* ECAM bus number (0-255) */
    uint8_t             dev : 5;    /* ECAM device (0-31) */
    uint8_t             func : 3;   /* ECAM deivce function (0-7) */
    uint32_t            id;         /* ECAM device ID */
    uint16_t            instance;   /* Cavium internal instance number */
} bdk_device_t;
#define BDK_NO_DEVICE_INSTANCE 0xffffu

/**
 * Defines the main entry points for a device driver. Full
 * definition is in bdk-device.h
 */
struct bdk_driver_s;

/**
 * Called to register a new driver with the bdk-device system. Drivers are probed
 * and initialized as device are found for them. If devices have already been
 * added before the driver was registered, the driver will be probed and
 * initialized before this function returns.
 *
 * @param driver Driver functions
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_device_add_driver(struct bdk_driver_s *driver);

/**
 * Called by the ECAM code whan a new device is detected in the system
 *
 * @param node   Node the ECAM is on
 * @param ecam   ECAM the device is on
 * @param bus    Bus number for the device
 * @param dev    Device number
 * @param func   Function number
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_device_add(bdk_node_t node, int ecam, int bus, int dev, int func);

/**
 * Rename a device. Called by driver to give devices friendly names
 *
 * @param device Device to rename
 * @param format Printf style format string
 */
extern void bdk_device_rename(bdk_device_t *device, const char *format, ...) __attribute__ ((format(printf, 2, 3)));

/**
 * Called by the ECAM code once all devices have been added
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_device_init(void);

/**
 * Lookup a device by ECAM ID and internal instance number. This can be used by
 * one device to find a handle to an associated device. For example, PKI would
 * use this function to get a handle to the FPA.
 *
 * @param node     Node to lookup for
 * @param id       ECAM ID
 * @param instance Cavium internal instance number
 *
 * @return Device pointer, or NULL if the device isn't found
 */
extern const bdk_device_t *bdk_device_lookup(bdk_node_t node, uint32_t id, int instance);

/**
 * Read from a device BAR
 *
 * @param device Device to read from
 * @param bar    Which BAR to read from (0-3)
 * @param size   Size of the read
 * @param offset Offset into the BAR
 *
 * @return Value read
 */
extern uint64_t bdk_bar_read(const bdk_device_t *device, int bar, int size, uint64_t offset);

/**
 * Write to a device BAR
 *
 * @param device Device to write to
 * @param bar    Which BAR to read from (0-3)
 * @param size   Size of the write
 * @param offset Offset into the BAR
 * @param value  Value to write
 */
extern void bdk_bar_write(const bdk_device_t *device, int bar, int size, uint64_t offset, uint64_t value);

/**
 * This macro makes it easy to define a variable of the correct
 * type for a BAR.
 */
#define BDK_BAR_DEFINE(name, REG) typedef_##REG name

/**
 * This macro makes it easy to define a variable and initialize it
 * with a BAR.
 */
#define BDK_BAR_INIT(name, device, REG) typedef_##REG name = {.u = bdk_bar_read(device, device_bar_##REG, sizeof(typedef_##REG), REG)}

/**
 * Macro to read a BAR
 */
#define BDK_BAR_READ(device, REG) bdk_bar_read(device, device_bar_##REG, sizeof(typedef_##REG), REG)

/**
 * Macro to write a BAR
 */
#define BDK_BAR_WRITE(device, REG, value) bdk_bar_write(device, device_bar_##REG, sizeof(typedef_##REG), REG, value)

/**
 * Macro to make a read, modify, and write sequence easy. The "code_block"
 * should be replaced with a C code block or a comma separated list of
 * "name.s.field = value", without the quotes.
 */
#define BDK_BAR_MODIFY(name, device, REG, code_block) do { \
        uint64_t _tmp_address = REG; \
        typedef_##REG name = {.u = bdk_bar_read(device, device_bar_##REG, sizeof(typedef_##REG), _tmp_address)}; \
        code_block; \
        bdk_bar_write(device, device_bar_##REG, sizeof(typedef_##REG), _tmp_address, name.u); \
    } while (0)

/**
 * This macro spins on a field waiting for it to reach a value. It
 * is common in code to need to wait for a specific field in a
 * REG to match a specific value. Conceptually this macro
 * expands to:
 *
 * 1) read REG
 * 2) Check if ("type".s."field" "op" "value")
 * 3) If #2 isn't true loop to #1 unless too much time has passed.
 */
#define BDK_BAR_WAIT_FOR_FIELD(device, REG, field, op, value, timeout_usec) \
    ({int result;                                                       \
    do {                                                                \
        uint64_t done = bdk_clock_get_count(BDK_CLOCK_TIME) + (uint64_t)timeout_usec * \
                        bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / 1000000;   \
        typedef_##REG c;                                                \
        uint64_t _tmp_address = REG;                                    \
        while (1)                                                       \
        {                                                               \
            c.u = bdk_bar_read(device, device_bar_##REG, sizeof(typedef_##REG), _tmp_address); \
            if ((c.s.field) op (value)) {                               \
                result = 0;                                             \
                break;                                                  \
            } else if (bdk_clock_get_count(BDK_CLOCK_TIME) > done) {    \
                result = -1;                                            \
                break;                                                  \
            } else                                                      \
                bdk_thread_yield();                                     \
        }                                                               \
    } while (0);                                                        \
    result;})

/** @} */
