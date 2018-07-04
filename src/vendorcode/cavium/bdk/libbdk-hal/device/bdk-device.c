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
#include "libbdk-arch/bdk-csrs-ap.h"
#include "libbdk-arch/bdk-csrs-pccpf.h"
#include "libbdk-hal/bdk-ecam.h"
#include "libbdk-hal/device/bdk-device.h"
#include "libbdk-hal/bdk-config.h"
#include "libbdk-driver/bdk-driver.h"
#include "libbdk-hal/bdk-utils.h"

static struct bdk_driver_s *driver_list = NULL;

#define DEVICE_GROW 64
static bdk_device_t *device_list = NULL;
static int device_list_count = 0;
static int device_list_max = 0;

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
int bdk_device_add_driver(struct bdk_driver_s *driver)
{
    driver->next = driver_list;
    driver_list = driver;
    BDK_TRACE(DEVICE, "Added driver for %08x\n", driver->id);
    return 0;
}

/**
 * Lookup the correct driver for a device
 *
 * @param device Device to lookup
 *
 * @return Driver, or NULL on failure
 */
static const bdk_driver_t *lookup_driver(const bdk_device_t *device)
{
    const bdk_driver_t *drv = driver_list;
    while (drv)
    {
        if (drv->id == device->id)
            return drv;
        drv = drv->next;
    }
    return NULL;
}

/**
 * Populate the fields of a new device from the ECAM
 *
 * @param device Device to populate
 */
static void populate_device(bdk_device_t *device)
{
    /* The default name may be replaced by the driver with something easier to read */
    snprintf(device->name, sizeof(device->name), "N%d.E%d:%d:%d.%d",
        device->node, device->ecam, device->bus, device->dev, device->func);

    BDK_TRACE(DEVICE_SCAN, "%s: Populating device\n", device->name);

    /* Get the current chip ID and pass. We'll need this to fill in version
       information for the device */
    bdk_ap_midr_el1_t midr_el1;
    BDK_MRS(MIDR_EL1, midr_el1.u);

    /* PCCPF_XXX_VSEC_SCTL[RID] with the revision of the chip,
       read from fuses */
    BDK_CSR_DEFINE(sctl, BDK_PCCPF_XXX_VSEC_SCTL);
    sctl.u = bdk_ecam_read32(device, BDK_PCCPF_XXX_VSEC_SCTL);
    sctl.s.rid = midr_el1.s.revision | (midr_el1.s.variant<<3);
    sctl.s.node = device->node; /* Program node bits */
    sctl.s.ea = bdk_config_get_int(BDK_CONFIG_PCIE_EA);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        sctl.s.ea = 0; /* EA is not supported on CN88XX pass 1.x */
    else
        sctl.s.ea = bdk_config_get_int(BDK_CONFIG_PCIE_EA);
    bdk_ecam_write32(device, BDK_PCCPF_XXX_VSEC_SCTL, sctl.u);

    /* Read the Device ID */
    device->id = bdk_ecam_read32(device, BDK_PCCPF_XXX_ID);

    /* Read the Device Type so we know how to handle BARs */
    bdk_pccpf_xxx_clsize_t clsize;
    clsize.u = bdk_ecam_read32(device, BDK_PCCPF_XXX_CLSIZE);
    int isbridge = (clsize.s.hdrtype & 0x7f) == 1;

    BDK_TRACE(DEVICE_SCAN, "%s: Device ID: 0x%08x%s\n", device->name, device->id,
        (isbridge) ? " (Bridge)" : "");

    /* Loop through all the BARs */
    int max_bar = (isbridge) ? BDK_PCCPF_XXX_BAR0U : BDK_PCCPF_XXX_BAR4U;
    int bar = BDK_PCCPF_XXX_BAR0L;
    unsigned guess_instance = 0;
    while (bar <= max_bar)
    {
        int bar_index = (bar - BDK_PCCPF_XXX_BAR0L) / 8;
        /* Read the BAR address and config bits [3:0] */
        uint64_t address = bdk_ecam_read32(device, bar);
        int ismem = !(address & 1);         /* Bit 0: 0 = mem, 1 = io */
        int is64 = ismem && (address & 4);  /* Bit 2: 0 = 32 bit, 1 = 64 bit if mem */
        /* Bit 3: 1 = Is prefetchable. We on't care for now */

        /* All internal BARs should be 64 bit. Skip if BAR isn't as that means
           it is using Enhanced Allocation (EA) */
        if (!is64)
        {
            BDK_TRACE(DEVICE_SCAN, "%s: BAR%d Disabled or EA bar skipped (0x%08llx)\n", device->name, bar_index, address);
            bar += 8;
            continue;
        }

        /* Get the upper part of 64bit BARs */
        address |= (uint64_t)bdk_ecam_read32(device, bar + 4) << 32;

        /* Write the bits to determine the size */
        bdk_ecam_write32(device, bar, -1);
        bdk_ecam_write32(device, bar + 4, -1);
        uint64_t size_mask = (uint64_t)bdk_ecam_read32(device, bar + 4) << 32;
        size_mask |= bdk_ecam_read32(device, bar);
        /* Make sure the node bits are correct in the address */
        address = (address & ~(3UL << 44)) | ((uint64_t)device->node << 44);
        /* Restore address value */
        bdk_ecam_write32(device, bar, address);
        bdk_ecam_write32(device, bar + 4, address >> 32);

        /* Convert the size into a power of 2 bits */
        int size_bits = bdk_dpop(~size_mask | 0xf);
        if (size_bits <= 4)
            size_bits = 0;

        /* Store the BAR info */
        device->bar[bar_index].address = address & ~0xfull;
        device->bar[bar_index].size2 = size_bits;
        device->bar[bar_index].flags = address & 0xf;
        BDK_TRACE(DEVICE_SCAN, "%s: BAR%d 0x%llx/%d flags=0x%x\n",
            device->name, bar_index, device->bar[bar_index].address,
            device->bar[bar_index].size2, device->bar[bar_index].flags);
        /* Move to the next BAR */
        bar += 8;
    }

    /* Walk the PCI capabilities looking for PCIe support and EA headers */
    BDK_TRACE(DEVICE_SCAN, "%s: Walking PCI capabilites\n", device->name);
    int has_pcie = 0;
    bdk_pccpf_xxx_cap_ptr_t cap_ptr;
    cap_ptr.u = bdk_ecam_read32(device, BDK_PCCPF_XXX_CAP_PTR);
    int cap_loc = cap_ptr.s.cp;
    while (cap_loc)
    {
        uint32_t cap = bdk_ecam_read32(device, cap_loc);
        int cap_id = cap & 0xff;
        int cap_next = (cap >> 8) & 0xff;

        BDK_TRACE(DEVICE_SCAN, "%s:    PCI Capability 0x%02x ID:0x%02x Next:0x%02x\n",
            device->name, cap_loc, cap_id, cap_next);

        if (cap_id == 0x10)
        {
            BDK_TRACE(DEVICE_SCAN, "%s:      PCIe\n", device->name);
            has_pcie = 1;
        }
        else if (cap_id == 0x01)
        {
            BDK_TRACE(DEVICE_SCAN, "%s:      PCI Power Management Interface\n", device->name);
            /* Do nothing for now */
        }
        else if (cap_id == 0x11)
        {
            bdk_pccpf_xxx_msix_cap_hdr_t msix_cap_hdr;
            bdk_pccpf_xxx_msix_table_t msix_table;
            bdk_pccpf_xxx_msix_pba_t msix_pba;
            msix_cap_hdr.u = cap;
            msix_table.u = bdk_ecam_read32(device, cap_loc + 4);
            msix_pba.u = bdk_ecam_read32(device, cap_loc + 8);
            BDK_TRACE(DEVICE_SCAN, "%s:      MSI-X Entries:%d, Func Mask:%d, Enable:%d\n",
                device->name, msix_cap_hdr.s.msixts + 1, msix_cap_hdr.s.funm, msix_cap_hdr.s.msixen);
            BDK_TRACE(DEVICE_SCAN, "%s:          Table BAR%d, Offset:0x%x\n",
                device->name, msix_table.s.msixtbir, msix_table.s.msixtoffs * 8);
            BDK_TRACE(DEVICE_SCAN, "%s:          PBA BAR%d, Offset:0x%x\n",
                device->name, msix_pba.s.msixpbir, msix_pba.s.msixpoffs * 8);
        }
        else if (cap_id == 0x05)
        {
            BDK_TRACE(DEVICE_SCAN, "%s:      MSI\n", device->name);
            /* Do nothing for now */
        }
        else if (cap_id == 0x14)
        {
            bdk_pccpf_xxx_ea_cap_hdr_t ea_cap_hdr;
            ea_cap_hdr.u = cap;
            cap_loc += 4;
            BDK_TRACE(DEVICE_SCAN, "%s:      Enhanced Allocation, %d entries\n",
                device->name, ea_cap_hdr.s.num_entries);
            if (isbridge)
            {
                cap = bdk_ecam_read32(device, cap_loc);
                cap_loc += 4;
                int fixed_secondary_bus = cap & 0xff;
                int fixed_subordinate_bus = cap & 0xff;
                BDK_TRACE(DEVICE_SCAN, "%s:      Fixed Secondary Bus:0x%02x Fixed Subordinate Bus:0x%02x\n",
                    device->name, fixed_secondary_bus, fixed_subordinate_bus);
            }
            for (int entry = 0; entry < ea_cap_hdr.s.num_entries; entry++)
            {
                union bdk_pcc_ea_entry_s ea_entry;
                memset(&ea_entry, 0, sizeof(ea_entry));
                uint32_t *ptr = (uint32_t *)&ea_entry;
                *ptr++ = bdk_ecam_read32(device, cap_loc);
#if __BYTE_ORDER == __BIG_ENDIAN
                /* For big endian we actually need the previous data
                   shifted 32 bits */
                *ptr = ptr[-1];
#endif
                asm volatile ("" ::: "memory"); /* Needed by gcc 5.0 to detect aliases on ea_entry */
                int entry_size = ea_entry.s.entry_size;
                for (int i = 0; i < entry_size; i++)
                {
                    *ptr++ = bdk_ecam_read32(device, cap_loc + 4*i + 4);
                }
#if __BYTE_ORDER == __BIG_ENDIAN
                /* The upper and lower 32bits need to be swapped */
                ea_entry.u[0] = (ea_entry.u[0] >> 32) | (ea_entry.u[0] << 32);
                ea_entry.u[1] = (ea_entry.u[1] >> 32) | (ea_entry.u[1] << 32);
                ea_entry.u[2] = (ea_entry.u[2] >> 32) | (ea_entry.u[2] << 32);
#endif
                asm volatile ("" ::: "memory"); /* Needed by gcc 5.0 to detect aliases on ea_entry */
                BDK_TRACE(DEVICE_SCAN, "%s:      Enable:%d Writeable:%d Secondary Prop:0x%02x Primary Prop:0x%02x BEI:%d Size:%d\n",
                    device->name, ea_entry.s.enable, ea_entry.s.w, ea_entry.s.sec_prop, ea_entry.s.pri_prop, ea_entry.s.bei, ea_entry.s.entry_size);
                if (ea_entry.s.entry_size > 0)
                {
                    BDK_TRACE(DEVICE_SCAN, "%s:        Base:0x%08x 64bit:%d\n",
                        device->name, ea_entry.s.basel << 2, ea_entry.s.base64);
                }
                if (ea_entry.s.entry_size > 1)
                {
                    BDK_TRACE(DEVICE_SCAN, "%s:        MaxOffset:0x%08x 64bit:%d\n",
                        device->name, (ea_entry.s.offsetl << 2) | 3, ea_entry.s.offset64);
                }
                if (ea_entry.s.entry_size > 2)
                {
                    BDK_TRACE(DEVICE_SCAN, "%s:        BaseUpper:0x%08x\n",
                        device->name, ea_entry.s.baseh);
                }
                if (ea_entry.s.entry_size > 3)
                {
                    BDK_TRACE(DEVICE_SCAN, "%s:        MaxOffsetUpper:0x%08x\n",
                        device->name, ea_entry.s.offseth);
                }
                if (ea_entry.s.enable)
                {
                    uint64_t base = (uint64_t)ea_entry.s.baseh << 32;
                    base |= (uint64_t)ea_entry.s.basel << 2;
                    /* Make sure the node bits are correct in the address */
                    base = (base & ~(3UL << 44)) | ((uint64_t)device->node << 44);
                    uint64_t offset = (uint64_t)ea_entry.s.offseth << 32;
                    offset |= ((uint64_t)ea_entry.s.offsetl << 2) | 3;
                    switch (ea_entry.s.bei)
                    {
                        case 0: /* BAR 0 */
                        case 2: /* BAR 1 */
                        case 4: /* BAR 2 */
                        {
                            int bar_index = ea_entry.s.bei/2;
                            device->bar[bar_index].address = base;
                            device->bar[bar_index].size2 = bdk_dpop(offset);
                            device->bar[bar_index].flags = ea_entry.s.base64 << 2;
                            BDK_TRACE(DEVICE_SCAN, "%s:        Updated BAR%d 0x%llx/%d flags=0x%x\n",
                                device->name, bar_index, device->bar[bar_index].address,
                                device->bar[bar_index].size2, device->bar[bar_index].flags);
                            if (0 == ea_entry.s.bei) {
                                /* PEMs eg PCIEEP and PCIERC do not have instance id
                                ** We can calculate it for PCIERC based on BAR0 allocation.
                                ** PCIEEP will be dropped by probe
                                */
                                guess_instance = (device->bar[bar_index].address >> 24) & 7;
                            }
                            break;
                        }
                        case 9: /* SR-IOV BAR 0 */
                        case 11: /* SR-IOV BAR 1 */
                        case 13: /* SR-IOV BAR 2 */
                            // FIXME
                            break;
                    }
                }
                cap_loc += ea_entry.s.entry_size * 4 + 4;
            }
        }
        else
        {
            /* Unknown PCI capability */
            bdk_warn("%s: ECAM device unknown PCI capability 0x%x\n", device->name, cap_id);
        }
        cap_loc = cap_next;
    }

    /* Walk the PCIe capabilities looking for instance header */
    if (has_pcie)
    {
        BDK_TRACE(DEVICE_SCAN, "%s: Walking PCIe capabilites\n", device->name);
        cap_loc = 0x100;
        while (cap_loc)
        {
            uint32_t cap = bdk_ecam_read32(device, cap_loc);
            int cap_id = cap & 0xffff;
            int cap_ver = (cap >> 16) & 0xf;
            int cap_next = cap >> 20;
            BDK_TRACE(DEVICE_SCAN, "%s:    PCIe Capability 0x%03x ID:0x%04x Version:0x%x Next:0x%03x\n",
                device->name, cap_loc, cap_id, cap_ver, cap_next);
            if (cap_id == 0xe)
            {
                /* ARI. Do nothing for now */
                BDK_TRACE(DEVICE_SCAN, "%s:      ARI\n", device->name);
            }
            else if (cap_id == 0xb)
            {
                /* Vendor specific*/
                int vsec_id = bdk_ecam_read32(device, cap_loc + 4);
                int vsec_id_id = vsec_id & 0xffff;
                int vsec_id_rev = (vsec_id >> 16) & 0xf;
                int vsec_id_len = vsec_id >> 20;
                BDK_TRACE(DEVICE_SCAN, "%s:      Vendor ID: 0x%04x Rev: 0x%x Size 0x%03x\n",
                    device->name, vsec_id_id, vsec_id_rev, vsec_id_len);
                switch (vsec_id_id)
                {
                    case 0x0001: /* RAS Data Path */
                        BDK_TRACE(DEVICE_SCAN, "%s:      Vendor RAS Data Path\n", device->name);
                        break;

                    case 0x0002: /* RAS DES */
                        BDK_TRACE(DEVICE_SCAN, "%s:      Vendor RAS DES\n", device->name);
                        break;

                    case 0x00a0: /* Cavium common */
                    case 0x00a1: /* Cavium CN88XX */
                    case 0x00a2: /* Cavium CN81XX */
                    case 0x00a3: /* Cavium CN83XX */
                        if ((vsec_id_rev == 1) || (vsec_id_rev == 2))
                        {
                            int vsec_ctl = bdk_ecam_read32(device, cap_loc + 8);
                            int vsec_ctl_inst_num = vsec_ctl & 0xff;
                            int vsec_ctl_subnum = (vsec_ctl >> 8) & 0xff;
                            BDK_TRACE(DEVICE_SCAN, "%s:        Cavium Instance: 0x%02x Static Bus: 0x%02x\n",
                                device->name, vsec_ctl_inst_num, vsec_ctl_subnum);
                            int vsec_sctl = bdk_ecam_read32(device, cap_loc + 12);
                            int vsec_sctl_rid = (vsec_sctl >> 16) & 0xff;
                            if (vsec_id_rev == 2)
                            {
                                int vsec_sctl_pi = (vsec_sctl >> 24) & 0xff; /* Only in Rev 2 */
                                BDK_TRACE(DEVICE_SCAN, "%s:        Revision ID: 0x%02x Programming Interface: 0x%02x\n",
                                    device->name, vsec_sctl_rid, vsec_sctl_pi);
                            }
                            else
                            {
                                BDK_TRACE(DEVICE_SCAN, "%s:        Revision ID: 0x%02x\n",
                                    device->name, vsec_sctl_rid);
                            }
                            /* Record the device instance */
                            device->instance = vsec_ctl_inst_num;
                        }
                        else
                        {
                            bdk_warn("%s: ECAM device Unknown Cavium extension revision\n", device->name);
                        }
                        break;

                    default: /* Unknown Vendor extension */
                        bdk_warn("%s: ECAM device unknown vendor extension ID 0x%x\n", device->name, vsec_id_id);
                        break;
                }
            }
            else if (cap_id == 0x10)
            {
                /* Single Root I/O Virtualization (SR-IOV) */
                BDK_TRACE(DEVICE_SCAN, "%s:      SR-IOV\n", device->name);
                /* Loop through all the SR-IOV BARs */
                bar = cap_loc + 0x24;
                while (bar <= (cap_loc + 0x3c))
                {
                    int bar_index = (bar - 0x24 - cap_loc) / 8;
                    /* Read the BAR address and config bits [3:0] */
                    uint64_t address = bdk_ecam_read32(device, bar);
                    int ismem = !(address & 1);         /* Bit 0: 0 = mem, 1 = io */
                    int is64 = ismem && (address & 4);  /* Bit 2: 0 = 32 bit, 1 = 64 bit if mem */
                    /* Bit 3: 1 = Is prefetchable. We don't care for now */

                    /* All internal BARs should be 64 bit. Skip if BAR isn't as that means
                       it is using Enhanced Allocation (EA) */
                    if (!is64)
                    {
                        BDK_TRACE(DEVICE_SCAN, "%s:        SR-IOV BAR%d Disabled or EA bar skipped (0x%08llx)\n", device->name, bar_index, address);
                        bar += 8;
                        continue;
                    }

                    /* Get the upper part of 64bit BARs */
                    address |= (uint64_t)bdk_ecam_read32(device, bar + 4) << 32;

                    /* Write the bits to determine the size */
                    bdk_ecam_write32(device, bar, -1);
                    bdk_ecam_write32(device, bar + 4, -1);
                    uint64_t size_mask = (uint64_t)bdk_ecam_read32(device, bar + 4) << 32;
                    size_mask |= bdk_ecam_read32(device, bar);
                    /* Make sure the node bits are correct in the address */
                    address = (address & ~(3UL << 44)) | ((uint64_t)device->node << 44);
                    /* Restore address value */
                    bdk_ecam_write32(device, bar, address);
                    bdk_ecam_write32(device, bar + 4, address >> 32);

                    /* Convert the size into a power of 2 bits */
                    int size_bits = bdk_dpop(size_mask | 0xf);
                    if (size_bits <= 4)
                        size_bits = 0;

                    BDK_TRACE(DEVICE_SCAN, "%s:        SR-IOV BAR%d 0x%llx/%d flags=0x%llx\n",
                        device->name, bar_index, address & ~0xfull,
                        size_bits, address & 0xf);
                    /* Move to the next BAR */
                    bar += 8;
                }
            }
            else if (cap_id == 0x01)
            {
                /* Advanced Error Reporting Capability */
                BDK_TRACE(DEVICE_SCAN, "%s:      Advanced Error Reporting\n", device->name);
            }
            else if (cap_id == 0x19)
            {
                /* Secondary PCI Express Extended Capability */
                BDK_TRACE(DEVICE_SCAN, "%s:      Secondary PCI Express Extended\n", device->name);
            }
            else if (cap_id == 0x15)
            {
                /* PCI Express Resizable BAR (RBAR) Capability */
                BDK_TRACE(DEVICE_SCAN, "%s:      PCI Express Resizable BAR (RBAR)\n", device->name);
            }
            else if (cap_id == 0x0d)
            {
                /* Extended access control := ACS Extended Capability */
                BDK_TRACE(DEVICE_SCAN, "%s:      ACS\n", device->name);
            }
            else
            {
                /* Unknown PCIe capability */
                bdk_warn("%s: ECAM device unknown PCIe capability 0x%x\n", device->name, cap_id);
            }
            cap_loc = cap_next;
        }
    }
    else
    {
        bdk_error("%s: ECAM device didn't have a PCIe capability\n", device->name);
    }
    if (BDK_NO_DEVICE_INSTANCE == device->instance) {
        device->instance = guess_instance;
    }
    BDK_TRACE(DEVICE_SCAN, "%s: Device populated\n", device->name);
}

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
int bdk_device_add(bdk_node_t node, int ecam, int bus, int dev, int func)
{
    if (device_list_count == device_list_max)
    {
        int grow = device_list_max + DEVICE_GROW;
        bdk_device_t *tmp = malloc(grow * sizeof(bdk_device_t));
        if (!tmp)
            memcpy(tmp, device_list, device_list_max * sizeof(bdk_device_t));
        free(device_list);
        if (tmp == NULL)
        {
            bdk_error("bdk-device: Failed to allocate space for device\n");
            return -1;
        }
        device_list = tmp;
        device_list_max = grow;
    }

    bdk_device_t *device = &device_list[device_list_count++];
    memset(device, 0, sizeof(*device));

    device->state = BDK_DEVICE_STATE_NOT_PROBED;
    device->node = node;
    device->ecam = ecam;
    device->bus = bus;
    device->dev = dev;
    device->func = func;
    device->instance = BDK_NO_DEVICE_INSTANCE;
    populate_device(device);

    const bdk_driver_t *drv = lookup_driver(device);
    if (drv)
        BDK_TRACE(DEVICE, "%s: Added device\n", device->name);
    else
        BDK_TRACE(DEVICE, "%s: Added device without driver (0x%08x)\n", device->name, device->id);
    return 0;
}

/**
 * Rename a device. Called by driver to give devices friendly names
 *
 * @param device Device to rename
 * @param format Printf style format string
 */
void bdk_device_rename(bdk_device_t *device, const char *format, ...)
{
    char tmp[sizeof(device->name)];
    va_list args;
    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);
    tmp[sizeof(tmp) - 1] = 0;
    BDK_TRACE(DEVICE, "%s: Renamed to %s\n", device->name, tmp);
    strcpy(device->name, tmp);
}

/**
 * Called by the ECAM code once all devices have been added
 *
 * @return Zero on success, negative on failure
 */
int bdk_device_init(void)
{
    /* Probe all devices first */
    for (int i = 0; i < device_list_count; i++)
    {
        bdk_device_t *dev = &device_list[i];
        const bdk_driver_t *drv = lookup_driver(dev);
        if (drv == NULL)
            continue;
        if (dev->state == BDK_DEVICE_STATE_NOT_PROBED)
        {
            BDK_TRACE(DEVICE, "%s: Probing\n", dev->name);
            if (drv->probe(dev))
            {
                BDK_TRACE(DEVICE, "%s: Probe failed\n", dev->name);
                dev->state = BDK_DEVICE_STATE_PROBE_FAIL;
            }
            else
            {
                BDK_TRACE(DEVICE, "%s: Probe complete\n", dev->name);
                dev->state = BDK_DEVICE_STATE_PROBED;
            }
        }
    }

    /* Do init() after all the probes. See comments in top of bdk-device.h */
    for (int i = 0; i < device_list_count; i++)
    {
        bdk_device_t *dev = &device_list[i];
        const bdk_driver_t *drv = lookup_driver(dev);
        if (drv == NULL)
            continue;
        if (dev->state == BDK_DEVICE_STATE_PROBED)
        {
            BDK_TRACE(DEVICE, "%s: Initializing\n", dev->name);
            if (drv->init(dev))
            {
                BDK_TRACE(DEVICE, "%s: Init failed\n", dev->name);
                dev->state = BDK_DEVICE_STATE_INIT_FAIL;
            }
            else
            {
                BDK_TRACE(DEVICE, "%s: Init complete\n", dev->name);
                dev->state = BDK_DEVICE_STATE_READY;
            }
        }
    }
    return 0;
}

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
const bdk_device_t *bdk_device_lookup(bdk_node_t node, uint32_t id, int instance)
{
    for (int i = 0; i < device_list_count; i++)
    {
        bdk_device_t *dev = &device_list[i];
        if ((dev->node == node) && (dev->id == id) && (dev->instance == instance))
            return dev;
    }
    BDK_TRACE(DEVICE, "No device found for node %d, ID %08x, instance %d\n", node, id, instance);
    return NULL;
}

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
uint64_t bdk_bar_read(const bdk_device_t *device, int bar, int size, uint64_t offset)
{
    uint64_t address = offset & bdk_build_mask(device->bar[bar/2].size2);
    address += device->bar[bar/2].address;
    if (offset+size > (1ULL << device->bar[bar/2].size2)) {
        /* The CSR address passed in offset doesn't contain the node number. Copy it
           from the BAR address */
        offset |= address & (0x3ull << 44);
        if (address != offset)
            bdk_fatal("BAR read address 0x%llx doesn't match CSR address 0x%llx\n", address, offset);
    }
    switch (size)
    {
        case 1:
            return bdk_read64_uint8(address);
        case 2:
            return bdk_le16_to_cpu(bdk_read64_uint16(address));
        case 4:
            return bdk_le32_to_cpu(bdk_read64_uint32(address));
        case 8:
            return bdk_le64_to_cpu(bdk_read64_uint64(address));
    }
    bdk_fatal("%s: Unexpected read size %d\n", device->name, size);
}

/**
 * Write to a device BAR
 *
 * @param device Device to write to
 * @param bar    Which BAR to read from (0-3)
 * @param size   Size of the write
 * @param offset Offset into the BAR
 * @param value  Value to write
 */
void bdk_bar_write(const bdk_device_t *device, int bar, int size, uint64_t offset, uint64_t value)
{
    uint64_t address = offset & bdk_build_mask(device->bar[bar/2].size2);
    address += device->bar[bar/2].address;
    if (offset+size > (1ULL << device->bar[bar/2].size2)) {
        /* The CSR address passed in offset doesn't contain the node number. Copy it
           from the BAR address */
        offset |= address & (0x3ull << 44);
        if (address != offset)
            bdk_fatal("BAR write address 0x%llx doesn't match CSR address 0x%llx\n", address, offset);
    }
    switch (size)
    {
        case 1:
            bdk_write64_uint8(address, value);
            return;
        case 2:
            bdk_write64_uint16(address, bdk_cpu_to_le16(value));
            return;
        case 4:
            bdk_write64_uint32(address, bdk_cpu_to_le32(value));
            return;
        case 8:
            bdk_write64_uint64(address, bdk_cpu_to_le64(value));
            return;
    }
    bdk_fatal("%s: Unexpected write size %d\n", device->name, size);
}
