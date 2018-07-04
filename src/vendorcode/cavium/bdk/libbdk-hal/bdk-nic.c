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
#include <malloc.h>
#include "libbdk-arch/bdk-csrs-nic.h"

#define MAX_MTU 9212
#define CQ_ENTRIES_QSIZE 0
#define CQ_ENTRIES (1024 << CQ_ENTRIES_QSIZE)
#define SQ_ENTRIES_QSIZE 0
#define SQ_ENTRIES (1024 << SQ_ENTRIES_QSIZE)
#define RBDR_ENTRIES_QSIZE 0
#define RBDR_ENTRIES (8192 << RBDR_ENTRIES_QSIZE)

typedef struct
{
    /* VNIC related config */
    bdk_node_t  node : 8;       /* Node the NIC is on */
    bdk_nic_type_t ntype : 8;   /* They type of device this NIC is connected to */
    uint8_t     nic_vf;         /* NIC VF index number (0 - MAX_VNIC-1) */
    uint8_t     sq;             /* Send Queue (SQ) inside NIC VF (0-7) */
    uint8_t     cq;             /* Complete Queue (CQ) inside NIC VF (0-7) */
    uint8_t     rq;             /* Receive Queue (RQ) inside NIC VF (0-7) */
    uint8_t     rbdr;           /* Receive Buffer Descriptor Ring (RBDR) inside NIC VF (0-1) */
    uint8_t     bpid;           /* Backpressure ID (0-127) */
    bdk_if_handle_t handle;     /* bdk-if handle associated with this NIC */

    /* Transmit */
    void *      sq_base;        /* Pointer to the beginning of the SQ in memory */
    int         sq_loc;         /* Location where the next send should go */
    int         sq_available;   /* Amount of space left in the queue (fuzzy) */
} nic_t;

typedef struct
{
    void *base;
    int loc;
} nic_rbdr_state_t;

typedef struct
{
    int num_nic_vf;
    int next_free_nic_vf;
    int next_free_cpi;
    int next_free_rssi;
    int next_free_bpid;
    nic_t *nic_map[0]; /* Indexed by handle->nic_id */
} nic_node_state_t;

static nic_node_state_t *global_node_state[BDK_NUMA_MAX_NODES];
static int global_buffer_size = 0;

/**
 * Setup a receive Completion Queue (CQ). CQ can be shared across multiple NICs
 * to save space. This happens if the NIC has "shares_cq" set.
 *
 * @param nic    NIC to setup
 *
 * @return Zero on success, negative on failure
 */
static int vnic_setup_cq(nic_t *nic)
{
    /* CN88XX pass 1.x had the drop level reset value too low */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_CQM_CFG,
        c.s.drop_level = 128);

    /* All devices using the same NIC VF use the same CQ */
    if (nic->handle->index == 0)
    {
        BDK_TRACE(NIC, "%s: Setting up CQ(%d, %d)\n", nic->handle->name, nic->nic_vf, nic->cq);
        /* Note that the completion queue requires 512 byte alignment */
        void *cq_memory = memalign(512, 512 * CQ_ENTRIES);
        if (!cq_memory)
        {
            bdk_error("%s: Failed to allocate memory for completion queue\n", nic->handle->name);
            return -1;
        }
        /* Configure the completion queue (CQ) */
        BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_CQX_BASE(nic->nic_vf, nic->cq),
            bdk_ptr_to_phys(cq_memory));
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_QSX_CQX_CFG(nic->nic_vf, nic->cq),
            c.s.ena = 1;
            c.s.caching = 1;
            c.s.qsize = CQ_ENTRIES_QSIZE);
    }

    /* Configure our vnic to send to the CQ */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_SQX_CFG(nic->nic_vf, nic->sq),
        c.s.cq_qs = nic->nic_vf;
        c.s.cq_idx = nic->cq);
    return 0;
}

/**
 * Add buffers to a receive buffer descriptor ring (RBDR). Note that RBDRs are
 * shared between NICs using the same CQ.
 *
 * @param nic       NIC using the RBDR
 * @param rbdr_free Number of buffers to add
 */
static void vnic_fill_receive_buffer(const nic_t *nic, int rbdr_free)
{
    int nic_vf = nic->nic_vf;
    int rbdr = nic->rbdr;

    BDK_CSR_INIT(rbdr_base, nic->node, BDK_NIC_QSX_RBDRX_BASE(nic_vf, rbdr));
    BDK_CSR_INIT(rbdr_tail, nic->node, BDK_NIC_QSX_RBDRX_TAIL(nic_vf, rbdr));
    BDK_TRACE(NIC, "%s: In Filling RBDR(%d, %d) base 0x%lx\n", nic->handle->name, nic->nic_vf, nic->rbdr, rbdr_base.u);

    uint64_t *rbdr_ptr = bdk_phys_to_ptr(rbdr_base.u);
    int loc = rbdr_tail.s.tail_ptr;
    BDK_TRACE(NIC, "%s: In Filling RBDR(%d, %d) loc %d\n", nic->handle->name, nic->nic_vf, nic->rbdr, loc);

    int added = 0;
    for (int i = 0; i < rbdr_free; i++)
    {
        bdk_if_packet_t packet;
        if (bdk_if_alloc(&packet, global_buffer_size))
        {
            bdk_error("%s: Failed to allocate buffer for RX ring (added %d)\n", nic->handle->name, added);
            break;
        }
        rbdr_ptr[loc] = bdk_cpu_to_le64(packet.packet[0].s.address);
        BDK_TRACE(NIC, "%s: In Filling RBDR(%d, %d) loc %d = 0x%lx\n", nic->handle->name, nic->nic_vf, nic->rbdr, loc, rbdr_ptr[loc]);
        loc++;
        loc &= RBDR_ENTRIES - 1;
        added++;
    }
    BDK_WMB;
    BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_RBDRX_DOOR(nic_vf, rbdr), added);
    BDK_TRACE(NIC, "%s: In Filling RBDR(%d, %d) added %d\n", nic->handle->name, nic->nic_vf, nic->rbdr, added);
}

/**
 * Setup a receive buffer descriptor ring (RBDR). Note that NIC share the RBDR if
 * "share_cq" is set.
 *
 * @param nic    NIC to setup RBDR for
 *
 * @return Zero on success, negative on failure
 */
static int vnic_setup_rbdr(nic_t *nic)
{
    bool do_fill;

    /* All devices using the same NIC VF use the same RBDRs. Don't fill them
       for and ports except the first */
    if (nic->handle->index)
    {
        do_fill = false;
    }
    else
    {
        BDK_TRACE(NIC, "%s: Setting up RBDR(%d, %d)\n", nic->handle->name, nic->nic_vf, nic->rbdr);
        void *rbdr_base = memalign(BDK_CACHE_LINE_SIZE, 8 * RBDR_ENTRIES);
        if (!rbdr_base)
        {
            bdk_error("%s: Failed to allocate memory for RBDR\n", nic->handle->name);
            return -1;
        }
        /* Configure the receive buffer ring (RBDR) */
        BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_RBDRX_BASE(nic->nic_vf, nic->rbdr),
            bdk_ptr_to_phys(rbdr_base));
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_QSX_RBDRX_CFG(nic->nic_vf, nic->rbdr),
            c.s.ena = 1;
            c.s.ldwb = BDK_USE_DWB;
            c.s.qsize = RBDR_ENTRIES_QSIZE;
            c.s.lines = global_buffer_size / BDK_CACHE_LINE_SIZE);
        do_fill = true;
    }

    BDK_TRACE(NIC, "%s: Setting up RQ(%d, %d)\n", nic->handle->name, nic->nic_vf, nic->rq);
    /* Configure our vnic to use the RBDR */
    /* Connect this RQ to the RBDR. Both the first and next buffers come from
       the same RBDR */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_RQX_CFG(nic->nic_vf, nic->rq),
        c.s.caching = 1; /* Allocate to L2 */
        c.s.cq_qs = nic->nic_vf;
        c.s.cq_idx = nic->cq;
        c.s.rbdr_cont_qs = nic->nic_vf;
        c.s.rbdr_cont_idx = nic->rbdr;
        c.s.rbdr_strt_qs = nic->nic_vf;
        c.s.rbdr_strt_idx = nic->rbdr);
    /* NIC_PF_CQM_CFG is configure to drop everything if the CQ has 128 or
       less entries available. Start backpressure when we have 256 or less */
    int cq_bp = 256;
    int rbdr_bp = 256;
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_RQX_BP_CFG(nic->nic_vf, nic->rq),
        c.s.rbdr_bp_ena = 1;
        c.s.cq_bp_ena = 1;
        c.s.rbdr_bp = rbdr_bp * 256 / RBDR_ENTRIES; /* Zero means no buffers, 256 means lots available */
        c.s.cq_bp = cq_bp * 256 / CQ_ENTRIES; /* Zero means full, 256 means idle */
        c.s.bpid = nic->bpid);
    /* Errata (NIC-21269) Limited NIC receive scenario verification */
    /* RED drop set with pass=drop, so no statistical dropping */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_RQX_DROP_CFG(nic->nic_vf, nic->rq),
        c.s.rbdr_red = 0;
        c.s.cq_red = 0;
        c.s.rbdr_pass = 0; /* Zero means no buffers, 256 means lots available */
        c.s.rbdr_drop = 0;
        c.s.cq_pass = 0; /* Zero means full, 256 means idle */
        c.s.cq_drop = 0);

    if (do_fill)
    {
        BDK_TRACE(NIC, "%s: Filling RBDR(%d, %d)\n", nic->handle->name, nic->nic_vf, nic->rbdr);
        /* We probably don't have enough space to completely fill the RBDR. Use
           1/8 of the buffers available */
        int fill_num = bdk_config_get_int(BDK_CONFIG_NUM_PACKET_BUFFERS) / 8;
        if (CAVIUM_IS_MODEL(CAVIUM_CN83XX)) fill_num = fill_num/3; /* CN83XX has more nics */
        /* Note that RBDR must leave one spot empty */
        if (fill_num > RBDR_ENTRIES - 1)
            fill_num = RBDR_ENTRIES - 1;
        vnic_fill_receive_buffer(nic, fill_num);
    }

    return 0;
}

/**
 * Setup traffic shapping for a NIC. This put the shappers in passthrough mode
 * where no shapping is applied.
 *
 * @param nic    NIC to configure shaping for
 *
 * @return Zero on success, negative on failure
 */
static int vnic_setup_tx_shaping(nic_t *nic)
{
    int tl1_index = -1;
    int tl2_index = -1;
    int tl3_index = -1;
    int tl4_index = -1;
    int nic_chan_e = -1;

    BDK_TRACE(NIC, "%s: Setting up shaping(%d, %d)\n", nic->handle->name, nic->nic_vf, nic->sq);

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        /* TL1 feeds the DMA engines. One for each BGX */
        tl1_index = nic->handle->interface;
        /* TL2 feeds TL1 based on the top/bottom half. Use an independent TL1
           entry for each BGX port */
        tl2_index = tl1_index * 32 + nic->handle->index;
        /* Each block of 4 TL3 feed TL2 */
        tl3_index = tl2_index * 4;
        /* Each block of 4 TL4 feed TL3 */
        tl4_index = tl3_index * 4;
        nic_chan_e = BDK_NIC_CHAN_E_BGXX_PORTX_CHX(nic->handle->interface, nic->handle->index, 0/*channel*/);
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
    {
        switch (nic->ntype)
        {
            case BDK_NIC_TYPE_BGX:
                tl1_index = BDK_NIC_LMAC_E_BGXX_LMACX(nic->handle->interface, nic->handle->index);
                nic_chan_e = 0 ; /* Channel is lmac-relative */
                break;
            case BDK_NIC_TYPE_LBK:
                tl1_index = BDK_NIC_LMAC_E_LBKX_CN83XX((nic->handle->interface == 3) ? 1 : 0);
                nic_chan_e = nic->handle->index; /* Channel is lmac-relative */
                break;
            default:
                bdk_error("%s: Unsupported NIC TYPE %d\n", nic->handle->name, nic->ntype);
                return -1;
        }
        /* TL1 index by NIC_LMAC_E */
        /* Set in above switch statement */
        /* TL2 index is software defined, make it the same as TL1 for straight through */
        tl2_index = tl1_index;
        /* Each block of 4 TL3 feed TL2. This assumes there are never more than 4 ports per interface */
        tl3_index = tl2_index * 4 + nic->handle->index;
        /* TL4 index is the same as TL3, 1:1 hookup */
        tl4_index = tl3_index;
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        switch (nic->ntype)
        {
            case BDK_NIC_TYPE_BGX:
                tl1_index = BDK_NIC_LMAC_E_BGXX_LMACX(nic->handle->interface, nic->handle->index);
                nic_chan_e = BDK_NIC_CHAN_E_BGXX_LMACX_CHX(nic->handle->interface, nic->handle->index, 0/*channel*/);
                break;
            case BDK_NIC_TYPE_RGMII:
                tl1_index = BDK_NIC_LMAC_E_RGXX_LMACX(nic->handle->interface, nic->handle->index);
                nic_chan_e = 0; /* Channel is lmac-relative */
                break;
            case BDK_NIC_TYPE_LBK:
                tl1_index = BDK_NIC_LMAC_E_LBKX_CN81XX(nic->handle->interface);
                nic_chan_e = nic->handle->index; /* Channel is lmac-relative */
                break;
            default:
                bdk_error("%s: Unsupported NIC TYPE %d\n", nic->handle->name, nic->ntype);
                return -1;
        }
        /* TL1 index by NIC_LMAC_E */
        /* Set in above switch statement */
        /* TL2 index is software defined, make it the same as TL1 for straight through */
        tl2_index = tl1_index;
        /* Each block of 4 TL3 feed TL2. This assumes there are never more than 4 ports per interface */
        tl3_index = tl2_index * 4 + nic->handle->index;
        /* TL4 index is the same as TL3, 1:1 hookup */
        tl4_index = tl3_index;
    }
    else
    {
        bdk_error("%s: Unsupported chip (NIC shaping)\n", nic->handle->name);
        return -1;
    }

    /* Setup TL2 to TL1 mappings */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL2X_CFG(tl2_index),
        c.s.rr_quantum = (MAX_MTU+4) / 4);
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL2X_PRI(tl2_index),
        c.s.rr_pri = 0);
    if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL2X_LMAC(tl2_index),
            c.s.lmac = tl1_index);
    }

    /* TL3 feeds Tl2 */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL3AX_CFG(tl3_index / 4),
        c.s.tl3a = tl2_index);
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL3X_CFG(tl3_index),
        c.s.rr_quantum = (MAX_MTU+4) / 4);
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL3X_CHAN(tl3_index),
        c.s.chan = nic_chan_e);

    /* TL4 feeds TL3 */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL4AX_CFG(tl4_index / 4),
            c.s.tl4a = tl3_index);
    }
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_TL4X_CFG(tl4_index),
        c.s.sq_qs = nic->nic_vf;
        c.s.sq_idx = nic->sq;
        c.s.rr_quantum = (MAX_MTU+4) / 4);

    /* SQ feeds TL4 */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_SQX_CFG2(nic->nic_vf, nic->sq),
        c.s.tl4 = tl4_index);

    return 0;
}

/**
 * Free the buffers in a packet to the RBDR used by the port
 *
 * @param priv   Determines which RBDR is used
 * @param packet Packet to put in RBDR
 */
static void if_free_to_rbdr(bdk_if_packet_t *packet, nic_rbdr_state_t *vnic_rbdr_state)
{
    uint64_t *rbdr_ptr = vnic_rbdr_state->base;
    int loc = vnic_rbdr_state->loc;

    for (int s = 0; s < packet->segments; s++)
    {
        /* Make sure we strip off any padding added by the hardware in the address */
        uint64_t address = packet->packet[s].s.address & -BDK_CACHE_LINE_SIZE;
        rbdr_ptr[loc] = bdk_cpu_to_le64(address);
        loc++;
        loc &= RBDR_ENTRIES - 1;
    }
    vnic_rbdr_state->loc = loc;
}

/**
 * Process a CQ receive entry
 *
 * @param node      Node containing the CQ
 * @param vnic_rbdr_state
 *                  Current RBDR state for the RBDR connected to the CQ
 * @param cq_header CQ header to process
 * @param use_cqe_rx2
 *                  True of the CQ will contain an extended CQE_RX2 header
 *
 * @return Returns the amount the RBDR doorbell needs to increment
 */
static int if_process_complete_rx(int node, nic_rbdr_state_t *vnic_rbdr_state, const union bdk_nic_cqe_rx_s *cq_header, const union bdk_nic_cqe_rx_s *cq_header_le, bool use_cqe_rx2)
{
    nic_node_state_t *node_state = global_node_state[node];
    int nic_id = cq_header->s.rq_qs * 8 + cq_header->s.rq_idx;

    bdk_if_packet_t packet;
    packet.length = cq_header->s.len;
    packet.segments = cq_header->s.rb_cnt;
    packet.if_handle = node_state->nic_map[nic_id]->handle;
    /* Combine the errlev and errop into a single 11 bit number. Errop
       is 8 bits, so errlev will be in the top byte */
    packet.rx_error = cq_header->s.errlev;
    packet.rx_error <<= 8;
    packet.rx_error |= cq_header->s.errop;

    const uint16_t *rb_sizes = (void*)cq_header_le + 24; /* Offset of RBSZ0 */
    const uint64_t *rb_addresses = (uint64_t*)(cq_header_le+1);
    /* Update offset if nic_cqe_rx2_s is used */
    if (use_cqe_rx2)
        rb_addresses += sizeof(union bdk_nic_cqe_rx2_s) / 8;
    int segment_length = 0;

    for (int s = 0; s < packet.segments; s++)
    {
        uint64_t addr = bdk_le64_to_cpu(rb_addresses[s]);
        BDK_PREFETCH(bdk_phys_to_ptr(addr), 0);
        packet.packet[s].u = addr;
        packet.packet[s].s.size = bdk_le16_to_cpu(rb_sizes[s]);
        BDK_TRACE(NIC, "    Receive segment size %d address 0x%lx\n", packet.packet[s].s.size, addr);
        segment_length += packet.packet[s].s.size;
    }

    /* If we ran out of buffer the packet could be truncated */
    if (segment_length < packet.length)
        packet.length = segment_length;

    if (bdk_likely(packet.if_handle))
    {
        /* Do RX stats in software as it is fast and I don't really trust
           the hardware. The hardware tends to count packets that are received
           and dropped in some weird way. Hopefully the hardware counters
           looking for drops can find these. It is important that they
           aren't counted as good */
        packet.if_handle->stats.rx.packets++;
        packet.if_handle->stats.rx.octets += packet.length;
        if (packet.if_handle->flags & BDK_IF_FLAGS_HAS_FCS)
            packet.if_handle->stats.rx.octets += 4;
        if (packet.rx_error)
            packet.if_handle->stats.rx.errors++;
        bdk_if_dispatch_packet(&packet);
    }
    else
    {
        bdk_error("Unable to determine interface for NIC %d.%d\n", cq_header->s.rq_qs, cq_header->s.rq_idx);
    }

    if_free_to_rbdr(&packet, vnic_rbdr_state);
    return packet.segments;
}

/**
 * Process all entries in a completion queue (CQ). Note that a CQ is shared
 * among many ports, so packets will be dispatch for other port handles.
 *
 * @param handle Interface handle connected to the CQ
 *
 * @return Number of packets received
 */
static void if_receive(int unused, void *hand)
{
    const nic_t *nic = hand;

    /* Sadly the hardware team decided to change the meaning of NIC_PF_RX_CFG
       for chips after CN88XX. This stupid spec change was really hard to
       find */
    bool use_cqe_rx2 = !CAVIUM_IS_MODEL(CAVIUM_CN88XX);

    /* Figure out which completion queue we're using */
    int nic_vf = nic->nic_vf;
    int rbdr = nic->rbdr;
    int cq = nic->cq;

    BDK_CSR_INIT(cq_base, nic->node, BDK_NIC_QSX_CQX_BASE(nic_vf, cq));
    const void *cq_ptr = bdk_phys_to_ptr(cq_base.u);

    /* Find the current CQ location */
    BDK_CSR_INIT(cq_head, nic->node, BDK_NIC_QSX_CQX_HEAD(nic_vf, cq));
    int loc = cq_head.s.head_ptr;

    /* Store the RBDR data locally to avoid contention */
    BDK_CSR_INIT(rbdr_base, nic->node, BDK_NIC_QSX_RBDRX_BASE(nic_vf, rbdr));
    BDK_CSR_INIT(rbdr_tail, nic->node, BDK_NIC_QSX_RBDRX_TAIL(nic_vf, rbdr));
    nic_rbdr_state_t vnic_rbdr_state;
    vnic_rbdr_state.base = bdk_phys_to_ptr(rbdr_base.u);
    vnic_rbdr_state.loc = rbdr_tail.s.tail_ptr;

    BDK_TRACE(NIC, "%s: Receive thread for CQ(%d, %d) started\n", nic->handle->name, nic->nic_vf, nic->cq);

    while (1)
    {
        /* Exit immediately if the CQ is empty */
        BDK_CSR_INIT(cq_status, nic->node, BDK_NIC_QSX_CQX_STATUS(nic_vf, cq));
        int pending_count = cq_status.s.qcount;
        if (bdk_likely(!pending_count))
        {
            bdk_wait_usec(1);
            continue;
        }

        /* Loop through all pending CQs */
        int rbdr_doorbell = 0;
        int count = 0;
        const union bdk_nic_cqe_rx_s *cq_next = cq_ptr + loc * 512;
        BDK_TRACE(NIC, "%s: Receive thread CQ(%d, %d): %d pending\n", nic->handle->name, nic->nic_vf, nic->cq, pending_count);
        while (count < pending_count)
        {
            const union bdk_nic_cqe_rx_s *cq_header = cq_next;
            const union bdk_nic_cqe_rx_s *cq_header_le = cq_header;
#if __BYTE_ORDER == __BIG_ENDIAN
            union bdk_nic_cqe_rx_s cq_be;
            for (int i = 0; i < 6; i++)
                cq_be.u[i] = bdk_le64_to_cpu(cq_header_le->u[i]);
            cq_header = &cq_be;
#endif
            BDK_TRACE(NIC, "%s: Receive HDR[%p] = 0x%lx 0x%lx 0x%lx 0x%lx\n",
                nic->handle->name, cq_header_le, cq_header->u[0], cq_header->u[1], cq_header->u[2], cq_header->u[3]);
            loc++;
            loc &= CQ_ENTRIES - 1;
            cq_next = cq_ptr + loc * 512;
            BDK_PREFETCH(cq_next, 0);
            if (bdk_likely(cq_header->s.cqe_type == BDK_NIC_CQE_TYPE_E_RX))
                rbdr_doorbell += if_process_complete_rx(nic->node, &vnic_rbdr_state, cq_header, cq_header_le, use_cqe_rx2);
            else
                bdk_error("Unsupported CQ header type %d\n", cq_header->s.cqe_type);
            count++;
        }
        /* Ring the RBDR doorbell for all packets */
        BDK_WMB;
        BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_RBDRX_DOOR(nic_vf, rbdr), rbdr_doorbell);
        /* Free all the CQs that we've processed */
        BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_CQX_DOOR(nic_vf, cq), count);
        /* Yield before going through more packets. The low core count chips
           don't have enough cores to dedicate for TX and RX. This forces
           sharing under load. If there are enough cores, the yield does
           nothing */
        bdk_thread_yield();
    }
}

/**
 * Configure NIC for a specific port. This is called for each
 * port on every interface that connects to NIC.
 *
 * @param handle Handle for port to config
 * @param ntype  Type of LMAC this NIC connects to
 * @param lmac_credits
 *               Size of the LMAC buffer in bytes. Used to configure the number of credits to
 *               setup between the NIC and LMAC
 *
 * @return Zero on success, negative on failure
 */
int bdk_nic_port_init(bdk_if_handle_t handle, bdk_nic_type_t ntype, int lmac_credits)
{
    int nic_chan_idx_e;     /* Flow channel for the CPI */
    bool has_rx_nic = (-1 == handle->pki_channel);  /* true when nic rx channel exists - may be BGX or LBK-NIC*/
    bool has_tx_nic = (-1 == handle->pko_queue);  /* true when nic tx channel exists - may be BGX or LBK-NIC*/
    int nic_intf_e = -1;         /* Interface enumeration */
    int nic_intf_block_e;   /* Interface Block ID Enumeration */
    int nic_lmac_e=-1;         /* LMAC enumeration */

    if (global_buffer_size == 0)
        global_buffer_size = bdk_config_get_int(BDK_CONFIG_PACKET_BUFFER_SIZE);

    if (!has_rx_nic && !has_tx_nic) return 0;

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        /* Flow here is a compressed NIC_CHAN_E enum value. Flow is bit[8] and
           bit[6:0] from NIC_CHAN_E. This works out as:
           bit 7: BGX interface number(0-1)
           bit 6:4: BGX port number(0-3)
           bit 3:0: BGX channel on a port (0-15) */
        nic_chan_idx_e = (handle->interface) ? 0x80 : 0x00;
        nic_chan_idx_e += handle->index * 16;
        nic_chan_idx_e += 0; /* channel */
        nic_intf_e = BDK_NIC_INTF_E_BGXX(handle->interface);
        nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_BGXX_BLOCK(handle->interface);
        nic_lmac_e = BDK_NIC_LMAC_E_BGXX_LMACX(handle->interface, handle->index);
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
    {
        switch (ntype)
        {
            case BDK_NIC_TYPE_BGX:
                nic_chan_idx_e = BDK_NIC_CHAN_IDX_E_BGXX_LMACX_CHX(handle->interface, handle->index, 0/*channel*/);
                nic_intf_e = BDK_NIC_INTF_E_BGXX(handle->interface);
                nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_BGXX(handle->interface);
                nic_lmac_e = BDK_NIC_LMAC_E_BGXX_LMACX(handle->interface, handle->index);
                break;
            case BDK_NIC_TYPE_LBK:
                nic_chan_idx_e = BDK_NIC_CHAN_IDX_E_LBKX_CHX_CN83XX((handle->interface == 3) ? 1 : 0, handle->index);
                // rx interface
                if (3 == handle->interface) {
                    nic_intf_e = BDK_NIC_INTF_E_LBKX_CN83XX(1);
                } else if  (2 == handle->interface) {
                    nic_intf_e = BDK_NIC_INTF_E_LBKX_CN83XX(0);
                }
                nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_LBKX(handle->interface);
                // tx interface
                if (3 == handle->interface) {
                    nic_lmac_e = BDK_NIC_LMAC_E_LBKX_CN83XX(1);
                } else if  (1 == handle->interface) {
                    nic_lmac_e = BDK_NIC_LMAC_E_LBKX_CN83XX(0);
                }
                break;
            default:
                bdk_error("%s: Unsupported NIC TYPE %d\n", handle->name, ntype);
                return -1;
        }
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        switch (ntype)
        {
            case BDK_NIC_TYPE_BGX:
                nic_chan_idx_e = BDK_NIC_CHAN_IDX_E_BGXX_LMACX_CHX(handle->interface, handle->index, 0/*channel*/);
                nic_intf_e = BDK_NIC_INTF_E_BGXX(handle->interface);
                nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_BGXX(handle->interface);
                nic_lmac_e = BDK_NIC_LMAC_E_BGXX_LMACX(handle->interface, handle->index);
                break;
            case BDK_NIC_TYPE_RGMII:
                nic_chan_idx_e = BDK_NIC_CHAN_IDX_E_RGXX_LMACX_CHX(handle->interface, handle->index, 0/*channel*/);
                nic_intf_e = BDK_NIC_INTF_E_RGXX(handle->index);
                nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_BGXX(handle->interface + 2);
                nic_lmac_e = BDK_NIC_LMAC_E_RGXX_LMACX(handle->interface, handle->index);
                break;
            case BDK_NIC_TYPE_LBK:
                nic_chan_idx_e = BDK_NIC_CHAN_IDX_E_LBKX_CHX_CN81XX(handle->interface, handle->index);
                nic_intf_e = BDK_NIC_INTF_E_LBKX_CN81XX(handle->interface);
                nic_intf_block_e = BDK_NIC_INTF_BLOCK_E_LBKX(handle->interface);
                nic_lmac_e = BDK_NIC_LMAC_E_LBKX_CN81XX(handle->interface);
                break;
            default:
                bdk_error("%s: Unsupported NIC TYPE %d\n", handle->name, ntype);
                return -1;
        }
    }
    else
    {
        bdk_error("%s: Unsupported chip (NIC init)\n", handle->name);
        return -1;
    }

    /* Make sure the node global state has been allocated */
    if (global_node_state[handle->node] == NULL)
    {
        int num_nic_vf;
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        {
            /* NIC_PF_CONST1 didn't exist on this chip */
            num_nic_vf = 128;
        }
        else
        {
            BDK_CSR_INIT(nic_pf_const1, handle->node, BDK_NIC_PF_CONST1);
            num_nic_vf = nic_pf_const1.s.vnics;
        }
        global_node_state[handle->node] = calloc(1, sizeof(nic_node_state_t) + sizeof(handle) * num_nic_vf * 8);
        if (global_node_state[handle->node] == NULL)
        {
            bdk_error("N%d.NIC: Failed to allocate node state\n", handle->node);
            return -1;
        }
        global_node_state[handle->node]->num_nic_vf = num_nic_vf;
    }
    nic_node_state_t *node_state = global_node_state[handle->node];

    /* See if we have a free VF */
    if (!handle->index && (node_state->next_free_nic_vf >= node_state->num_nic_vf))
    {
        bdk_error("N%d.NIC: Ran out of NIC VFs\n", handle->node);
        return -1;
    }

    /* VNIC setup requirements
       The code in this file makes the following assumptions:
       1) One RBDR for each CQ. No locking is done on RBDR
       2) A CQ can be shared across multiple ports, saving space as the
            cost of performance.
       3) One SQ per physical port, no locking on TX
       4) One RQ per physical port, many RQ may share RBDR/CQ

        Current setup without DRAM:
        1) One NIC VF is used for an entire interface (BGX, LBK). The variable
            nic_vf represents the NIC virtual function.
        2) SQs are allocated one per port. SQ index equals handle->index
        3) RQs are allocated one per port. RQ index equals handle->index
        4) One CQ is allcoated per entire interface, using index 0
        5) One RBDR is used for the CQ, index 0

        Current setup with DRAM:
        FIXME: Same as without DRAM. There are not enough RBDR to have
            independent CQs without locking.
       */
    void *sq_memory = NULL;
    if (has_tx_nic) {
        sq_memory = memalign(BDK_CACHE_LINE_SIZE, 16 * SQ_ENTRIES);
        if (!sq_memory)
        {
            bdk_error("%s: Unable to allocate queues\n", handle->name);
            return -1;
        }
    }
    nic_t *nic = calloc(1, sizeof(nic_t));
    if (!nic)
    {
        if (sq_memory) free(sq_memory);
        bdk_error("%s: Unable to NIC state\n", handle->name);
        return -1;
    }

    /* Fill in the various NIC indexes */
    nic->node = handle->node;
    nic->ntype = ntype;
    if (handle->index)
        nic->nic_vf = node_state->next_free_nic_vf - 1; /* reuse last one */
    else
        nic->nic_vf = node_state->next_free_nic_vf++; /* New nic */
    nic->sq = handle->index;
    nic->cq = 0;
    nic->rq = handle->index;
    nic->rbdr = 0;
    nic->bpid = node_state->next_free_bpid++;
    nic->handle = handle;
    BDK_TRACE(NIC, "%s: Creating NIC(%d, sq=%d, cq=%d, rq=%d, rbdr=%d, bpid=%d)\n",
        nic->handle->name, nic->nic_vf, nic->sq, nic->cq, nic->rq, nic->rbdr, nic->bpid);

    /* Connect this NIC to the handle */
    handle->nic_id = nic->nic_vf * 8 + nic->rq;
    node_state->nic_map[handle->nic_id] = nic;

    /* Enable global BP state updates */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_BP_CFG,
        c.s.bp_poll_ena = 1;
        c.s.bp_poll_dly = 3);

    /* Enable interface level backpresure */
    if (-1 != nic_intf_e) {
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_INTFX_BP_CFG(nic_intf_e),
            c.s.bp_ena = 1;
            c.s.bp_type = ((nic->ntype == BDK_NIC_TYPE_BGX) ||
                           (nic->ntype == BDK_NIC_TYPE_RGMII)) ? 0 : 1; /* 0=BGX, 1=LBK/TNS */
            c.s.bp_id = nic_intf_block_e);
    }
    if (has_tx_nic) {
        /* Configure the submit queue (SQ) */
        nic->sq_base = sq_memory;
        nic->sq_loc = 0;
        nic->sq_available = SQ_ENTRIES;
        BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_SQX_BASE(nic->nic_vf, nic->sq),
                      bdk_ptr_to_phys(sq_memory));
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_QSX_SQX_CFG(nic->nic_vf, nic->sq),
                       if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
                           c.s.cq_limit = 1;
                       c.s.ena = 1;
                       c.s.ldwb = BDK_USE_DWB;
                       c.s.qsize = SQ_ENTRIES_QSIZE);
    }
    int cpi=0;
    int rssi=0;
    if (has_rx_nic) {
        /* Configure the receive queue (RQ) */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_QSX_RQ_GEN_CFG(nic->nic_vf),
                       c.s.vlan_strip = 0;
                       c.s.len_l4 = 0;
                       c.s.len_l3 = 0;
                       c.s.csum_l4 = 0;
                       c.s.ip6_udp_opt = 0;
                       c.s.splt_hdr_ena = 0;
                       c.s.cq_hdr_copy = 0;
                       c.s.max_tcp_reass = 0;
                       c.s.cq_pkt_size = 0;
                       c.s.later_skip = 0;
                       c.s.first_skip = 0);
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_QSX_RQX_CFG(nic->nic_vf, nic->rq),
                       c.s.ena = 1;
                       c.s.tcp_ena = 0);

        cpi = node_state->next_free_cpi++;  /* Allocate a new Channel Parse Index (CPI) */
        rssi = node_state->next_free_rssi++;/* Allocate a new Receive-Side Scaling Index (RSSI) */
        /* NIC_CHAN_E hard mapped to "flow". Flow chooses the CPI */

        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_CHANX_RX_CFG(nic_chan_idx_e),
                       c.s.cpi_alg = BDK_NIC_CPI_ALG_E_NONE;
                       c.s.cpi_base = cpi);
        /* Setup backpressure */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_CHANX_RX_BP_CFG(nic_chan_idx_e),
                       c.s.ena = 1;
                       c.s.bpid = nic->bpid);
    }
    if ( has_tx_nic) {
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_CHANX_TX_CFG(nic_chan_idx_e),
            c.s.bp_ena = 1);
    }

    if (has_rx_nic)  {
        /* CPI is the output of the above alogrithm, this is used to lookup the
           VNIC for receive and RSSI */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_CPIX_CFG(cpi),
                       c.cn88xxp1.vnic = nic->nic_vf; /* TX and RX use the same VNIC */
                       c.cn88xxp1.rss_size = 0; /* RSS hash is disabled */
                       c.s.padd = 0; /* Used if we have multiple channels per port */
                       c.cn88xxp1.rssi_base = rssi); /* Base RSSI */

        if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        {
            /* CN88XX pass 2 moved some fields to a different CSR */
            BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_MPIX_CFG(cpi),
                           c.s.vnic = nic->nic_vf; /* TX and RX use the same VNIC */
                           c.s.rss_size = 0; /* RSS hash is disabled */
                           c.s.rssi_base = rssi); /* Base RSSI */
        }

        /* The RSSI is used to determine which Receive Queue (RQ) we use */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_RSSIX_RQ(rssi),
                       c.s.rq_qs = nic->nic_vf;
                       c.s.rq_idx = nic->rq);
        /* Set the min and max packet size. PKND comes from BGX. It is always zero
           for now */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_PKINDX_CFG(handle->pknd),
                       c.s.lenerr_en = 0;
                       c.s.minlen = 0;
                       c.s.maxlen = 65535);
    }

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        /* Bypass the TNS */
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_INTFX_SEND_CFG(handle->interface),
           c.s.tns_nonbypass = 0;
           c.s.block = 0x8 + handle->interface);
    }

    /* Errata (NIC-21858) If NIC_PF_QS()_CFG ENA is set after RRM enabled...RRM breaks */
    /* Do global vnic init */
    BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_QSX_CFG(nic->nic_vf),
        c.s.ena = 1;
        c.s.vnic = nic->nic_vf);

    if (has_tx_nic && vnic_setup_tx_shaping(nic))
        return -1;

    /* Completion queue may be used by both tx and rx.
    ** Define it even if only one of rx/tx is in use
    */
    if (vnic_setup_cq(nic))
        return -1;
    /* RBDR is defined regardless of rx_nic to avoid possible backpressure */
    if ( vnic_setup_rbdr(nic))
        return -1;

    /* Program LMAC credits */
    if ((has_tx_nic) && (-1 != nic_lmac_e)) {
        int credit;
        if ((BDK_NIC_TYPE_LBK == nic->ntype) && CAVIUM_IS_MODEL(CAVIUM_CN83XX) )
            credit = 512; /* HRM guidance */
        else
            credit = (lmac_credits - MAX_MTU) / 16;
        BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_LMACX_CREDIT(nic_lmac_e),
            c.s.cc_unit_cnt = credit;
            c.s.cc_packet_cnt = 0x1ff;
            c.s.cc_enable = 1);

        /* Pad packets to 60 bytes, 15 32bit words (before FCS) */
        if (nic->ntype != BDK_NIC_TYPE_LBK)
            BDK_CSR_MODIFY(c, nic->node, BDK_NIC_PF_LMACX_CFG(nic_lmac_e),
                           c.s.min_pkt_size = 15);
    }
    /* Create a receive thread if this handle has its own CQ/RBDR */
    if (handle->index == 0)
    {
        /* FIXME
         * At this time thread monitors both CQ and RBDR and uses it only for receive
         * Setting up RBDR for tx only nics is wasteful.
         * When nic_tx in bdk starts using CQ, thread needs to change
         */
        if (has_rx_nic && bdk_thread_create(nic->node, 0, if_receive, 0, nic, 0))
        {
            bdk_error("%s: Failed to allocate receive thread\n", handle->name);
            return -1;
        }
    }

    return 0;
}

/**
 * Send a packet
 *
 * @param handle Handle of port to send on
 * @param packet Packet to send
 *
 * @return Zero on success, negative on failure
 */
int bdk_nic_transmit(bdk_if_handle_t handle, const bdk_if_packet_t *packet)
{
    /* The SQ can't be filled completely as it reguires at least one free
       entry so the head and pointer don't look like empty. SQ_SLOP is the
       amount of SQ space we reserve to make sure of this */
    const int SQ_SLOP = 1;
    const nic_node_state_t *node_state = global_node_state[handle->node];
    nic_t *nic = node_state->nic_map[handle->nic_id];
    BDK_TRACE(NIC, "%s: Transmit packet of %d bytes, %d segments\n",
        nic->handle->name, packet->length, packet->segments);

    /* Update the SQ available if we're out of space. The NIC should have sent
       packets, making more available. This allows us to only read the STATUS
       CSR when really necessary, normally using the L1 cached value */
    if (nic->sq_available < packet->segments + 1 + SQ_SLOP)
    {
        BDK_CSR_INIT(sq_status, nic->node, BDK_NIC_QSX_SQX_STATUS(nic->nic_vf, nic->sq));
        nic->sq_available = SQ_ENTRIES - sq_status.s.qcount;
        /* Re-Check for space. A packets is a header plus its segments */
        if (nic->sq_available < packet->segments + 1 + SQ_SLOP)
        {
            BDK_TRACE(NIC, "%s: Transmit fail, queue full\n", nic->handle->name);
            return -1;
        }
    }

    /* Build the command */
    void *sq_ptr = nic->sq_base;
    int loc = nic->sq_loc;
    union bdk_nic_send_hdr_s send_hdr;
    send_hdr.u[0] = 0;
    send_hdr.u[1] = 0;
    send_hdr.s.subdc = BDK_NIC_SEND_SUBDC_E_HDR;
    send_hdr.s.subdcnt = packet->segments;
    send_hdr.s.total = packet->length;
    switch (packet->packet_type)
    {
        case BDK_IF_TYPE_UNKNOWN:
            break;
        case BDK_IF_TYPE_UDP4:
            send_hdr.s.ckl3 = 1;        /* L3 - IPv4 checksum enable */
            send_hdr.s.l3ptr = 14;      /* L2 header is 14 bytes */
            send_hdr.s.ckl4 = BDK_NIC_SEND_CKL4_E_UDP; /* L4 - UDP checksum enable */
            send_hdr.s.l4ptr = 14 + 20; /* 14 bytes L2 + 20 bytes IPv4 */
            break;
        case BDK_IF_TYPE_TCP4:
            send_hdr.s.ckl3 = 1;        /* L3 - IPv4 checksum enable */
            send_hdr.s.l3ptr = 14;      /* L2 header is 14 bytes */
            send_hdr.s.ckl4 = BDK_NIC_SEND_CKL4_E_TCP; /* L4 - TCP checksum enable */
            send_hdr.s.l4ptr = 14 + 20; /* 14 bytes L2 + 20 bytes IPv4 */
            if (packet->mtu)
            {
                int headers = 14 + 20 + 20;
                send_hdr.s.tso = 1;     /* Use TCP offload */
                send_hdr.s.tso_sb = headers; /* 14 bytes L2 + 20 bytes IPv4, 20 bytes TCP */
                send_hdr.s.tso_mps = packet->mtu - headers; /* Max TCP data payload size */
            }
            break;
    }
    volatile uint64_t *wptr = (uint64_t *)(sq_ptr + loc * 16);
    wptr[0] = bdk_cpu_to_le64(send_hdr.u[0]);
    wptr[1] = bdk_cpu_to_le64(send_hdr.u[1]);
    BDK_TRACE(NIC, "%s: Transmit HDR[%p] = 0x%lx 0x%lx\n",
        nic->handle->name, sq_ptr + loc * 16, send_hdr.u[0], send_hdr.u[1]);
    loc++;
    loc &= SQ_ENTRIES - 1;
    for (int s = 0; s < packet->segments; s++)
    {
        union bdk_nic_send_gather_s gather;
        gather.u[0] = 0;
        gather.u[1] = 0;
        gather.s.addr = packet->packet[s].s.address;
        gather.s.subdc = BDK_NIC_SEND_SUBDC_E_GATHER;
        gather.s.ld_type = (BDK_USE_DWB) ? BDK_NIC_SEND_LD_TYPE_E_LDWB : BDK_NIC_SEND_LD_TYPE_E_LDD;
        gather.s.size = packet->packet[s].s.size;
        wptr = (uint64_t *)(sq_ptr + loc * 16);
        wptr[0] = bdk_cpu_to_le64(gather.u[0]);
        wptr[1] = bdk_cpu_to_le64(gather.u[1]);
        BDK_TRACE(NIC, "%s: Transmit Gather[%p] = 0x%lx 0x%lx\n",
            nic->handle->name, sq_ptr + loc * 16, gather.u[0], gather.u[1]);
        loc++;
        loc &= SQ_ENTRIES - 1;
    }

    BDK_WMB;

    /* Ring the doorbell */
    BDK_CSR_WRITE(nic->node, BDK_NIC_QSX_SQX_DOOR(nic->nic_vf, nic->sq),
        packet->segments + 1);
    BDK_TRACE(NIC, "%s: Transmit Doorbell %d\n", nic->handle->name, packet->segments + 1);

    /* Update our cached state */
    nic->sq_available -= packet->segments + 1;
    nic->sq_loc = loc;
    if (handle->iftype != BDK_IF_BGX) {
        /* Update stats as we do them in software for non-BGX */
        handle->stats.tx.packets++;
        handle->stats.tx.octets += packet->length;
        if (handle->flags & BDK_IF_FLAGS_HAS_FCS)
            handle->stats.tx.octets += 4;
    }
    return 0;
}

/**
 * Get the current TX queue depth. Note that this operation may be slow
 * and adversly affect packet IO performance.
 *
 * @param handle Port to check
 *
 * @return Depth of the queue in packets
 */
int bdk_nic_get_queue_depth(bdk_if_handle_t handle)
{
    const nic_node_state_t *node_state = global_node_state[handle->node];
    const nic_t *nic = node_state->nic_map[handle->nic_id];
    BDK_CSR_INIT(sq_status, nic->node, BDK_NIC_QSX_SQX_STATUS(nic->nic_vf, nic->sq));
    return sq_status.s.qcount;
}

/**
 * Query NIC and fill in the transmit stats for the supplied
 * interface handle.
 *
 * @param handle Port handle
 */
void bdk_nic_fill_tx_stats(bdk_if_handle_t handle)
{
    const int vnic = handle->nic_id >> 3;

    /* Transmit stats are done in software due to CN81XX not having enough NICs */

    /* Note drops are shared across a BGX. People will be confused */
    BDK_CSR_INIT(drps, handle->node, BDK_NIC_VNICX_TX_STATX(vnic, BDK_NIC_STAT_VNIC_TX_E_TX_DROP));
    handle->stats.tx.dropped_packets = bdk_update_stat_with_overflow(drps.u, handle->stats.tx.dropped_packets, 48);
    /* Dropped Octets are not available */
}

/**
 * Query NIC and fill in the receive stats for the supplied
 * interface handle.
 *
 * @param handle Port handle
 */
void bdk_nic_fill_rx_stats(bdk_if_handle_t handle)
{
    /* Account for RX FCS */
    const int bytes_off_rx = (handle->flags & BDK_IF_FLAGS_HAS_FCS) ? 4 : 0;
    const int vnic = handle->nic_id >> 3;

    /* Note stats are shared across a BGX. People will be confused */

    /* Read the RX statistics. These do not include the ethernet FCS */
    BDK_CSR_INIT(rx_red, handle->node, BDK_NIC_VNICX_RX_STATX(vnic, BDK_NIC_STAT_VNIC_RX_E_RX_RED));
    BDK_CSR_INIT(rx_red_octets, handle->node, BDK_NIC_VNICX_RX_STATX(vnic, BDK_NIC_STAT_VNIC_RX_E_RX_RED_OCTS));
    BDK_CSR_INIT(rx_ovr, handle->node, BDK_NIC_VNICX_RX_STATX(vnic, BDK_NIC_STAT_VNIC_RX_E_RX_ORUN));
    BDK_CSR_INIT(rx_ovr_octets, handle->node, BDK_NIC_VNICX_RX_STATX(vnic, BDK_NIC_STAT_VNIC_RX_E_RX_ORUN_OCTS));
    uint64_t drops = rx_red.u + rx_ovr.u;
    uint64_t drop_octets = rx_red_octets.u + rx_ovr_octets.u;

    /* Drop and error counters */
    handle->stats.rx.dropped_octets -= handle->stats.rx.dropped_packets * bytes_off_rx;
    handle->stats.rx.dropped_octets = bdk_update_stat_with_overflow(drop_octets, handle->stats.rx.dropped_octets, 48);
    handle->stats.rx.dropped_packets = bdk_update_stat_with_overflow(drops, handle->stats.rx.dropped_packets, 48);
    handle->stats.rx.dropped_octets += handle->stats.rx.dropped_packets * bytes_off_rx;

    /* Normal RX stats are done by software on receive */
}

