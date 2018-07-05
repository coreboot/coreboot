#ifndef __CB_BDK_HAL_IF_BDK_IF_H__
#define __CB_BDK_HAL_IF_BDK_IF_H__
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
 * Interface to external interfaces (SGMII, XAUI, etc)
 *
 * <hr>$Revision: 45089 $<hr>
 *
 * @defgroup packetio Packet Input / Output
 * @addtogroup packetio
 * @{
 */


#define BDK_BGX_PKI 0 /* When defined and non zero BGX will use PKI interface for receive */
#define BDK_BGX_PKO 0  /* When defined and non zero BGX will use PKO interface for transmit */

#define BDK_IF_MAX_GATHER 12 /* CN88XX RX supports 12 at most */

/*  PHY address encoding:
        Bits[31:24]: Node ID, 0xff for node the ethernet device is on
        Bits[23:16]: Only used for TWSI
        Bits[15:12]: PHY connection type (0=MDIO, 1=Fixed, 2=TWSI)
    For MDIO:
        Bits[31:24]: Node ID, 0xff for node the ethernet device is on
        Bits[23:16]: 0
        Bits[15:12]: 0=MDIO
        Bits[11:8]: MDIO bus number
        Bits[7:0]: MDIO address
    For Fixed:
        Bits[31:24]: 0
        Bits[23:16]: Zero
        Bits[15:12]: 1=Fixed
        Bits[11:0]:  0 = 1Gb, 1 = 100Mb
    For TWSI:
        Bits[31:24]: Node ID, 0xff for node the ethernet device is on
        Bits[23:16]: TWSI internal address width in bytes (0-2)
        Bits[15:12]: 2=TWSI
        Bits[11:8]: TWSI bus number
        Bits[7:0]: TWSI address
   */
#define BDK_IF_PHY_TYPE_MASK 0xf000
#define BDK_IF_PHY_MDIO 0x0000
#define BDK_IF_PHY_TWSI 0x2000
#define BDK_IF_PHY_FIXED_1GB 0x1000
#define BDK_IF_PHY_FIXED_100MB 0x1001

/**
 * Enumeration of different interfaces.
 */
typedef enum
{
    BDK_IF_BGX,
    BDK_IF_PCIE,
    BDK_IF_FAKE,
    BDK_IF_LBK,
    BDK_IF_RGX,
    BDK_IF_CGX,
    __BDK_IF_LAST
} bdk_if_t;

typedef enum
{
    BDK_IF_FLAGS_HAS_FCS = 1, /* The device adds and removes an FCS. On thunder we assume the hardware strips FCS */
    BDK_IF_FLAGS_ENABLED = 2,
} bdk_if_flags_t;

typedef union
{
    uint64_t u64;
    struct
    {
        uint64_t    reserved2   : 32;
        unsigned    speed       : 24;
        unsigned    lanes       : 6;
        int         full_duplex : 1;
        int         up          : 1;
    } s;
} bdk_if_link_t;

typedef struct
{
    uint64_t    dropped_octets;
    uint64_t    dropped_packets;
    uint64_t    octets;
    uint64_t    packets;
    uint64_t    errors;
} __bdk_if_stats_t;

typedef struct
{
    __bdk_if_stats_t rx;
    __bdk_if_stats_t tx;
} bdk_if_stats_t;

typedef struct __bdk_if_port
{
    bdk_if_t    iftype      : 8;
    bdk_node_t  node        : 8;
    int         interface   : 8;
    int         index       : 8;
    bdk_if_flags_t flags    : 8;
    int         pknd        : 8;    /* NIC/PKI packet kind */
    int16_t     pki_channel;        /* PKI/SSO channel number, or -1 if not used */
    int16_t     pki_dstat;          /* PKI DSTAT ID used for receive stats */
    int16_t     pko_queue;          /* PKO DQ number, or -1 if not used */
    int16_t     aura;               /* FPA aura number */
    int16_t     nic_id;             /* NIC ID in use, or -1 if not used. Encoded as NIC_VF * 8 + index */
    void *      receiver;           /* This is a bdk_if_packet_receiver_t */
    void *      receiver_arg;
    bdk_if_stats_t stats;
    bdk_if_link_t link_info;
    char        name[16];
    struct __bdk_if_port *next;
    struct __bdk_if_port *poll_next;
    char        priv[0];
} __bdk_if_port_t;

typedef __bdk_if_port_t *bdk_if_handle_t;

/**
 * Format of each gather/segment entry in a packet. This is unrelated to
 * the underlying hardware format, but is designed to be simple to munge
 * into a hardware format. Note that only 48 bits are stored for the
 * address. This address is a physical address not mean for SMMU translation.
 */
typedef union
{
    uint64_t u;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t size : 16;
        uint64_t address : 48;
#else
        uint64_t address : 48;
        uint64_t size : 16;
#endif
    } s;
} bdk_packet_ptr_t;

typedef enum
{
    BDK_IF_TYPE_UNKNOWN,    /* Not set */
    BDK_IF_TYPE_UDP4,       /* IPv4 + UDP */
    BDK_IF_TYPE_TCP4,       /* IPv4 + TCP */
} bdk_if_type_t;

/**
 * The packet format for the BDK. This structure is designed to be exactly
 * one cache line to promote alignment and avoid false aliasing. Note that the
 * packet structure is independent from the packet data and can have a shorter
 * lifespan. The packet structure is normally on the stack and disappears on
 * stack unwind. Code requiring its data to stick around needs to copy it, but
 * not the data in the gather list.
 */
typedef struct
{
    bdk_if_handle_t if_handle;  /* Handle to interface this packet was received on */
    int             length;     /* Length of the packet in bytes */
    int             segments;   /* Number of segments the packet is spread over */
    int             rx_error;   /* Error number when packet was receive or zero for no error */
    bdk_if_type_t   packet_type : 16; /* Type of the packet, so sender doesn't need to walk packet */
    uint16_t        mtu;        /* MTU for hardware fragment, such as TSO */
    uint64_t        reserved2;  /* Reserved for future use */
    bdk_packet_ptr_t packet[BDK_IF_MAX_GATHER]; /* List of segements. Each has a physical address and length */
} bdk_if_packet_t;

typedef enum
{
    BDK_IF_LOOPBACK_NONE = 0,
    BDK_IF_LOOPBACK_INTERNAL = 1,
    BDK_IF_LOOPBACK_EXTERNAL = 2,
    BDK_IF_LOOPBACK_INTERNAL_EXTERNAL = 3,
} bdk_if_loopback_t;

typedef void (*bdk_if_packet_receiver_t)(const bdk_if_packet_t *packet, void *arg);

typedef struct
{
    int priv_size;
    int (*if_num_interfaces)(bdk_node_t node); /* Returns the number of interfaces possible on this chip */
    int (*if_num_ports)(bdk_node_t node, int interface); /* For given interface, returns the number of ports on it */
    int (*if_probe)(bdk_if_handle_t handle); /* Probe to see if a port exists */
    int (*if_init)(bdk_if_handle_t handle); /* One time hardware init */
    int (*if_enable)(bdk_if_handle_t handle); /* Enable packet IO. must be called after init */
    int (*if_disable)(bdk_if_handle_t handle); /* Disable packet IO */
    bdk_if_link_t (*if_link_get)(bdk_if_handle_t handle); /* Get link speed and state */
    void (*if_link_set)(bdk_if_handle_t handle, bdk_if_link_t link_info); /* Set link speed and state */
    const bdk_if_stats_t *(*if_get_stats)(bdk_if_handle_t handle); /* Get stats */
    int (*if_transmit)(bdk_if_handle_t handle, const bdk_if_packet_t *packet); /* TX a packet */
    int (*if_loopback)(bdk_if_handle_t handle, bdk_if_loopback_t loopback); /* Configure loopback for the port */
    int (*if_get_queue_depth)(bdk_if_handle_t handle); /* Get the current TX queue depth */
    uint64_t (*if_get_lane_mask)(bdk_if_handle_t handle); /* Get a mask of the QLM lanes used by this handle */
} __bdk_if_ops_t;

typedef struct
{
    int (*init)(void);
    uint64_t (*alloc)(int length);
    void (*free)(uint64_t address, int length);
} __bdk_if_global_ops_t;

extern int bdk_if_is_configured(void);
extern int bdk_if_num_interfaces(bdk_node_t node, bdk_if_t iftype);
extern int bdk_if_num_ports(bdk_node_t node, bdk_if_t iftype, int interface);
extern bdk_if_handle_t bdk_if_next_port(bdk_if_handle_t handle);

extern int bdk_if_enable(bdk_if_handle_t handle);
extern int bdk_if_disable(bdk_if_handle_t handle);
extern int bdk_if_loopback(bdk_if_handle_t handle, bdk_if_loopback_t loopback);

extern const char *bdk_if_name(bdk_if_handle_t handle);
extern bdk_if_link_t bdk_if_link_get(bdk_if_handle_t handle);
extern bdk_if_link_t bdk_if_link_autoconf(bdk_if_handle_t handle);
extern const bdk_if_stats_t *bdk_if_get_stats(bdk_if_handle_t handle);
extern bdk_if_link_t __bdk_if_phy_get(bdk_node_t dev_node, int phy_addr);
extern int bdk_if_get_queue_depth(bdk_if_handle_t handle);
extern int bdk_if_link_wait_all(uint64_t timeout_us);
extern int bdk_if_phy_setup(bdk_node_t node);
extern int bdk_if_phy_vetesse_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr);
extern int bdk_if_phy_marvell_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr);
extern int bdk_if_phy_vsc8514_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr);
extern int bdk_if_transmit(bdk_if_handle_t handle, bdk_if_packet_t *packet);
extern void bdk_if_register_for_packets(bdk_if_handle_t handle, bdk_if_packet_receiver_t receiver, void *arg);
extern int bdk_if_alloc(bdk_if_packet_t *packet, int length);
extern void bdk_if_free(bdk_if_packet_t *packet);
extern void bdk_if_packet_read(const bdk_if_packet_t *packet, int location, int length, void *data);
extern void bdk_if_packet_write(bdk_if_packet_t *packet, int location, int length, const void *data);

extern uint64_t bdk_update_stat_with_overflow(uint64_t new_value, uint64_t old_value, int bit_size);
extern void __bdk_if_phy_xs_init(bdk_node_t dev_node, int phy_addr);

/**
 * Get interface type. Use this function instead of accessing the handle
 * directly. The handle is considered private and may change.
 *
 * @param handle Handle of port to get info for
 *
 * @return Interface type
 */
static inline bdk_if_t bdk_if_get_type(bdk_if_handle_t handle)
{
    return handle->iftype;
}

/**
 * Called by each interface driver to process a received packet. After calling
 * this function, it is the responsibility of each driver to free any resources
 * used by the packet, probably by calling bdk_if_free().
 *
 * @param packet Packet that was received
 */
static inline void bdk_if_dispatch_packet(const bdk_if_packet_t *packet)
{
    void *receiver_arg = packet->if_handle->receiver_arg;
    bdk_if_packet_receiver_t receiver = packet->if_handle->receiver;
    if (receiver)
        receiver(packet, receiver_arg);
}

/**
 * Get the base QLM used by this handle. For network interfaces that uses QLMs,
 * return the QLM number of lane 0. Note that some network interfaces span multiple
 * QLM/DLM. This will return the lowest QLM/DLM number.
 *
 * @param handle Handle to query
 *
 * @return QLM/DLM number, or -1 if handle doesn't use SERDES
 */
extern int bdk_if_get_qlm(bdk_if_handle_t handle);

/**
 * Get a mask of the QLM/DLM lanes used by this handle. A bit is set for each lane
 * used by the interface. Some ports span multiple QLM/DLM. In this case the bits
 * set will be wider than the QLM/DLM, signalling that the next QLM/DLM is needed
 * too.
 *
 * @param handle Handle to query
 *
 * @return Lane mask, or zero if no SERDES lanes are used
 */
extern uint64_t bdk_if_get_lane_mask(bdk_if_handle_t handle);

/** @} */
#endif
