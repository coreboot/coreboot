/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This is a ramstage driver for the Intel Management Engine found in the
 * 6-series chipset.  It handles the required boot-time messages over the
 * MMIO-based Management Engine Interface to tell the ME that the BIOS is
 * finished with POST.  Additional messages are defined for debug but are
 * not used unless the console loglevel is high enough.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <string.h>
#include <delay.h>
#include <elog.h>
#include <halt.h>

#ifdef __SMM__
#include <arch/io.h>
#else
# include <device/device.h>
# include <device/pci.h>
#endif

#include <southbridge/intel/common/rcba.h>
#include "me.h"
#include "pch.h"

#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vendorcode/google/chromeos/gnvs.h>
#endif

#ifndef __SMM__
/* Path that the BIOS should take based on ME state */
static const char *me_bios_path_values[] = {
	[ME_NORMAL_BIOS_PATH]		= "Normal",
	[ME_S3WAKE_BIOS_PATH]		= "S3 Wake",
	[ME_ERROR_BIOS_PATH]		= "Error",
	[ME_RECOVERY_BIOS_PATH]		= "Recovery",
	[ME_DISABLE_BIOS_PATH]		= "Disable",
	[ME_FIRMWARE_UPDATE_BIOS_PATH]	= "Firmware Update",
};
#endif

/* MMIO base address for MEI interface */
static u32 *mei_base_address;

#if IS_ENABLED(CONFIG_DEBUG_INTEL_ME)
static void mei_dump(void *ptr, int dword, int offset, const char *type)
{
	struct mei_csr *csr;

	printk(BIOS_SPEW, "%-9s[%02x] : ", type, offset);

	switch (offset) {
	case MEI_H_CSR:
	case MEI_ME_CSR_HA:
		csr = ptr;
		if (!csr) {
			printk(BIOS_SPEW, "ERROR: 0x%08x\n", dword);
			break;
		}
		printk(BIOS_SPEW, "cbd=%u cbrp=%02u cbwp=%02u ready=%u "
		       "reset=%u ig=%u is=%u ie=%u\n", csr->buffer_depth,
		       csr->buffer_read_ptr, csr->buffer_write_ptr,
		       csr->ready, csr->reset, csr->interrupt_generate,
		       csr->interrupt_status, csr->interrupt_enable);
		break;
	case MEI_ME_CB_RW:
	case MEI_H_CB_WW:
		printk(BIOS_SPEW, "CB: 0x%08x\n", dword);
		break;
	default:
		printk(BIOS_SPEW, "0x%08x\n", offset);
		break;
	}
}
#else
# define mei_dump(ptr,dword,offset,type) do {} while (0)
#endif

/*
 * ME/MEI access helpers using memcpy to avoid aliasing.
 */

static inline void mei_read_dword_ptr(void *ptr, int offset)
{
	u32 dword = read32(mei_base_address + (offset/sizeof(u32)));
	memcpy(ptr, &dword, sizeof(dword));
	mei_dump(ptr, dword, offset, "READ");
}

static inline void mei_write_dword_ptr(void *ptr, int offset)
{
	u32 dword = 0;
	memcpy(&dword, ptr, sizeof(dword));
	write32(mei_base_address + (offset/sizeof(u32)), dword);
	mei_dump(ptr, dword, offset, "WRITE");
}

#ifndef __SMM__
static inline void pci_read_dword_ptr(device_t dev, void *ptr, int offset)
{
	u32 dword = pci_read_config32(dev, offset);
	memcpy(ptr, &dword, sizeof(dword));
	mei_dump(ptr, dword, offset, "PCI READ");
}
#endif

static inline void read_host_csr(struct mei_csr *csr)
{
	mei_read_dword_ptr(csr, MEI_H_CSR);
}

static inline void write_host_csr(struct mei_csr *csr)
{
	mei_write_dword_ptr(csr, MEI_H_CSR);
}

static inline void read_me_csr(struct mei_csr *csr)
{
	mei_read_dword_ptr(csr, MEI_ME_CSR_HA);
}

static inline void write_cb(u32 dword)
{
	write32(mei_base_address + (MEI_H_CB_WW/sizeof(u32)), dword);
	mei_dump(NULL, dword, MEI_H_CB_WW, "WRITE");
}

static inline u32 read_cb(void)
{
	u32 dword = read32(mei_base_address + (MEI_ME_CB_RW/sizeof(u32)));
	mei_dump(NULL, dword, MEI_ME_CB_RW, "READ");
	return dword;
}

/* Wait for ME ready bit to be asserted */
static int mei_wait_for_me_ready(void)
{
	struct mei_csr me;
	unsigned try = ME_RETRY;

	while (try--) {
		read_me_csr(&me);
		if (me.ready)
			return 0;
		udelay(ME_DELAY);
	}

	printk(BIOS_ERR, "ME: failed to become ready\n");
	return -1;
}

static void mei_reset(void)
{
	struct mei_csr host;

	if (mei_wait_for_me_ready() < 0)
		return;

	/* Reset host and ME circular buffers for next message */
	read_host_csr(&host);
	host.reset = 1;
	host.interrupt_generate = 1;
	write_host_csr(&host);

	if (mei_wait_for_me_ready() < 0)
		return;

	/* Re-init and indicate host is ready */
	read_host_csr(&host);
	host.interrupt_generate = 1;
	host.ready = 1;
	host.reset = 0;
	write_host_csr(&host);
}

static int mei_send_msg(struct mei_header *mei, struct mkhi_header *mkhi,
			void *req_data)
{
	struct mei_csr host;
	unsigned ndata, n;
	u32 *data;

	/* Number of dwords to write, ignoring MKHI */
	ndata = mei->length >> 2;

	/* Pad non-dword aligned request message length */
	if (mei->length & 3)
		ndata++;
	if (!ndata) {
		printk(BIOS_DEBUG, "ME: request does not include MKHI\n");
		return -1;
	}
	ndata++; /* Add MEI header */

	/*
	 * Make sure there is still room left in the circular buffer.
	 * Reset the buffer pointers if the requested message will not fit.
	 */
	read_host_csr(&host);
	if ((host.buffer_depth - host.buffer_write_ptr) < ndata) {
		printk(BIOS_ERR, "ME: circular buffer full, resetting...\n");
		mei_reset();
		read_host_csr(&host);
	}

	/*
	 * This implementation does not handle splitting large messages
	 * across multiple transactions.  Ensure the requested length
	 * will fit in the available circular buffer depth.
	 */
	if ((host.buffer_depth - host.buffer_write_ptr) < ndata) {
		printk(BIOS_ERR, "ME: message (%u) too large for buffer (%u)\n",
		       ndata + 2, host.buffer_depth);
		return -1;
	}

	/* Write MEI header */
	mei_write_dword_ptr(mei, MEI_H_CB_WW);
	ndata--;

	/* Write MKHI header */
	mei_write_dword_ptr(mkhi, MEI_H_CB_WW);
	ndata--;

	/* Write message data */
	data = req_data;
	for (n = 0; n < ndata; ++n)
		write_cb(*data++);

	/* Generate interrupt to the ME */
	read_host_csr(&host);
	host.interrupt_generate = 1;
	write_host_csr(&host);

	/* Make sure ME is ready after sending request data */
	return mei_wait_for_me_ready();
}

static int mei_recv_msg(struct mei_header *mei, struct mkhi_header *mkhi,
			void *rsp_data, int rsp_bytes)
{
	struct mei_header mei_rsp;
	struct mkhi_header mkhi_rsp;
	struct mei_csr me, host;
	unsigned ndata, n;
	unsigned expected;
	u32 *data;

	/* Total number of dwords to read from circular buffer */
	expected = (rsp_bytes + sizeof(mei_rsp) + sizeof(mkhi_rsp)) >> 2;
	if (rsp_bytes & 3)
		expected++;

	/*
	 * The interrupt status bit does not appear to indicate that the
	 * message has actually been received.  Instead we wait until the
	 * expected number of dwords are present in the circular buffer.
	 */
	for (n = ME_RETRY; n; --n) {
		read_me_csr(&me);
		if ((me.buffer_write_ptr - me.buffer_read_ptr) >= expected)
			break;
		udelay(ME_DELAY);
	}
	if (!n) {
		printk(BIOS_ERR, "ME: timeout waiting for data: expected "
		       "%u, available %u\n", expected,
		       me.buffer_write_ptr - me.buffer_read_ptr);
		return -1;
	}

	/* Read and verify MEI response header from the ME */
	mei_read_dword_ptr(&mei_rsp, MEI_ME_CB_RW);
	if (!mei_rsp.is_complete) {
		printk(BIOS_ERR, "ME: response is not complete\n");
		return -1;
	}

	/* Handle non-dword responses and expect at least MKHI header */
	ndata = mei_rsp.length >> 2;
	if (mei_rsp.length & 3)
		ndata++;
	if (ndata != (expected - 1)) {
		printk(BIOS_ERR, "ME: response is missing data\n");
		return -1;
	}

	/* Read and verify MKHI response header from the ME */
	mei_read_dword_ptr(&mkhi_rsp, MEI_ME_CB_RW);
	if (!mkhi_rsp.is_response ||
	    mkhi->group_id != mkhi_rsp.group_id ||
	    mkhi->command != mkhi_rsp.command) {
		printk(BIOS_ERR, "ME: invalid response, group %u ?= %u, "
		       "command %u ?= %u, is_response %u\n", mkhi->group_id,
		       mkhi_rsp.group_id, mkhi->command, mkhi_rsp.command,
		       mkhi_rsp.is_response);
		return -1;
	}
	ndata--; /* MKHI header has been read */

	/* Make sure caller passed a buffer with enough space */
	if (ndata != (rsp_bytes >> 2)) {
		printk(BIOS_ERR, "ME: not enough room in response buffer: "
		       "%u != %u\n", ndata, rsp_bytes >> 2);
		return -1;
	}

	/* Read response data from the circular buffer */
	data = rsp_data;
	for (n = 0; n < ndata; ++n)
		*data++ = read_cb();

	/* Tell the ME that we have consumed the response */
	read_host_csr(&host);
	host.interrupt_status = 1;
	host.interrupt_generate = 1;
	write_host_csr(&host);

	return mei_wait_for_me_ready();
}

static inline int mei_sendrecv(struct mei_header *mei, struct mkhi_header *mkhi,
			       void *req_data, void *rsp_data, int rsp_bytes)
{
	if (mei_send_msg(mei, mkhi, req_data) < 0)
		return -1;
	if (mei_recv_msg(mei, mkhi, rsp_data, rsp_bytes) < 0)
		return -1;
	return 0;
}

#ifdef __SMM__
/* Send END OF POST message to the ME */
static int mkhi_end_of_post(void)
{
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_GEN,
		.command	= MKHI_END_OF_POST,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
		.length		= sizeof(mkhi),
	};

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, NULL, NULL, 0) < 0) {
		printk(BIOS_ERR, "ME: END OF POST message failed\n");
		return -1;
	}

	printk(BIOS_INFO, "ME: END OF POST message successful\n");
	return 0;
}
#endif

#if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG) && !defined(__SMM__)
/* Get ME firmware version */
static int mkhi_get_fw_version(void)
{
	struct me_fw_version version;
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_GEN,
		.command	= MKHI_GET_FW_VERSION,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
		.length		= sizeof(mkhi),
	};

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, NULL, &version, sizeof(version)) < 0) {
		printk(BIOS_ERR, "ME: GET FW VERSION message failed\n");
		return -1;
	}

	printk(BIOS_INFO, "ME: Firmware Version %u.%u.%u.%u (code) "
	       "%u.%u.%u.%u (recovery)\n",
	       version.code_major, version.code_minor,
	       version.code_build_number, version.code_hot_fix,
	       version.recovery_major, version.recovery_minor,
	       version.recovery_build_number, version.recovery_hot_fix);

	return 0;
}

static inline void print_cap(const char *name, int state)
{
	printk(BIOS_DEBUG, "ME Capability: %-30s : %sabled\n",
	       name, state ? "en" : "dis");
}

/* Get ME Firmware Capabilities */
static int mkhi_get_fwcaps(void)
{
	u32 rule_id = 0;
	struct me_fwcaps cap;
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_FWCAPS,
		.command	= MKHI_FWCAPS_GET_RULE,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
		.length		= sizeof(mkhi) + sizeof(rule_id),
	};

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, &rule_id, &cap, sizeof(cap)) < 0) {
		printk(BIOS_ERR, "ME: GET FWCAPS message failed\n");
		return -1;
	}

	print_cap("Full Network manageability", cap.caps_sku.full_net);
	print_cap("Regular Network manageability", cap.caps_sku.std_net);
	print_cap("Manageability", cap.caps_sku.manageability);
	print_cap("Small business technology", cap.caps_sku.small_business);
	print_cap("Level III manageability", cap.caps_sku.l3manageability);
	print_cap("IntelR Anti-Theft (AT)", cap.caps_sku.intel_at);
	print_cap("IntelR Capability Licensing Service (CLS)",
		  cap.caps_sku.intel_cls);
	print_cap("IntelR Power Sharing Technology (MPC)",
		  cap.caps_sku.intel_mpc);
	print_cap("ICC Over Clocking", cap.caps_sku.icc_over_clocking);
	print_cap("Protected Audio Video Path (PAVP)", cap.caps_sku.pavp);
	print_cap("IPV6", cap.caps_sku.ipv6);
	print_cap("KVM Remote Control (KVM)", cap.caps_sku.kvm);
	print_cap("Outbreak Containment Heuristic (OCH)", cap.caps_sku.och);
	print_cap("Virtual LAN (VLAN)", cap.caps_sku.vlan);
	print_cap("TLS", cap.caps_sku.tls);
	print_cap("Wireless LAN (WLAN)", cap.caps_sku.wlan);

	return 0;
}
#endif

#if IS_ENABLED(CONFIG_CHROMEOS) && 0 /* DISABLED */
/* Tell ME to issue a global reset */
int mkhi_global_reset(void)
{
	struct me_global_reset reset = {
		.request_origin	= GLOBAL_RESET_BIOS_POST,
		.reset_type	= CBM_RR_GLOBAL_RESET,
	};
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_CBM,
		.command	= MKHI_GLOBAL_RESET,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.length		= sizeof(mkhi) + sizeof(reset),
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
	};

	printk(BIOS_NOTICE, "ME: Requesting global reset\n");

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, &reset, NULL, 0) < 0) {
		/* No response means reset will happen shortly... */
		halt();
	}

	/* If the ME responded it rejected the reset request */
	printk(BIOS_ERR, "ME: Global Reset failed\n");
	return -1;
}
#endif

#ifdef __SMM__
static void intel_me7_finalize_smm(void)
{
	struct me_hfs hfs;
	u32 reg32;

	mei_base_address = (u32 *)
		(pci_read_config32(PCH_ME_DEV, PCI_BASE_ADDRESS_0) & ~0xf);

	/* S3 path will have hidden this device already */
	if (!mei_base_address || mei_base_address == (u32 *)0xfffffff0)
		return;

	/* Make sure ME is in a mode that expects EOP */
	reg32 = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);
	memcpy(&hfs, &reg32, sizeof(u32));

	/* Abort and leave device alone if not normal mode */
	if (hfs.fpt_bad ||
	    hfs.working_state != ME_HFS_CWS_NORMAL ||
	    hfs.operation_mode != ME_HFS_MODE_NORMAL)
		return;

	/* Try to send EOP command so ME stops accepting other commands */
	mkhi_end_of_post();

	/* Make sure IO is disabled */
	reg32 = pci_read_config32(PCH_ME_DEV, PCI_COMMAND);
	reg32 &= ~(PCI_COMMAND_MASTER |
		   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
	pci_write_config32(PCH_ME_DEV, PCI_COMMAND, reg32);

	/* Hide the PCI device */
	RCBA32_OR(FD2, PCH_DISABLE_MEI1);
}

void intel_me_finalize_smm(void)
{
	u32 did = pci_read_config32(PCH_ME_DEV, PCI_VENDOR_ID);
	switch (did) {
	case 0x1c3a8086:
		intel_me7_finalize_smm();
		break;
	case 0x1e3a8086:
		intel_me8_finalize_smm();
		break;
	default:
		printk(BIOS_ERR, "No finalize handler for ME %08x.\n", did);
	}
}
#else /* !__SMM__ */

/* Determine the path that we should take based on ME status */
static me_bios_path intel_me_path(device_t dev)
{
	me_bios_path path = ME_DISABLE_BIOS_PATH;
	struct me_hfs hfs;
	struct me_gmes gmes;

	/* S3 wake skips all MKHI messages */
	if (acpi_is_wakeup_s3())
		return ME_S3WAKE_BIOS_PATH;

	pci_read_dword_ptr(dev, &hfs, PCI_ME_HFS);
	pci_read_dword_ptr(dev, &gmes, PCI_ME_GMES);

	/* Check and dump status */
	intel_me_status(&hfs, &gmes);

	/* Check Current Working State */
	switch (hfs.working_state) {
	case ME_HFS_CWS_NORMAL:
		path = ME_NORMAL_BIOS_PATH;
		break;
	case ME_HFS_CWS_REC:
		path = ME_RECOVERY_BIOS_PATH;
		break;
	default:
		path = ME_DISABLE_BIOS_PATH;
		break;
	}

	/* Check Current Operation Mode */
	switch (hfs.operation_mode) {
	case ME_HFS_MODE_NORMAL:
		break;
	case ME_HFS_MODE_DEBUG:
	case ME_HFS_MODE_DIS:
	case ME_HFS_MODE_OVER_JMPR:
	case ME_HFS_MODE_OVER_MEI:
	default:
		path = ME_DISABLE_BIOS_PATH;
		break;
	}

	/* Check for any error code and valid firmware */
	if (hfs.error_code || hfs.fpt_bad)
		path = ME_ERROR_BIOS_PATH;

#if IS_ENABLED(CONFIG_ELOG)
	if (path != ME_NORMAL_BIOS_PATH) {
		struct elog_event_data_me_extended data = {
			.current_working_state = hfs.working_state,
			.operation_state       = hfs.operation_state,
			.operation_mode        = hfs.operation_mode,
			.error_code            = hfs.error_code,
			.progress_code         = gmes.progress_code,
			.current_pmevent       = gmes.current_pmevent,
			.current_state         = gmes.current_state,
		};
		elog_add_event_byte(ELOG_TYPE_MANAGEMENT_ENGINE, path);
		elog_add_event_raw(ELOG_TYPE_MANAGEMENT_ENGINE_EXT,
				   &data, sizeof(data));
	}
#endif

	return path;
}

/* Prepare ME for MEI messages */
static int intel_mei_setup(device_t dev)
{
	struct resource *res;
	struct mei_csr host;
	u32 reg32;

	/* Find the MMIO base for the ME interface */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res || res->base == 0 || res->size == 0) {
		printk(BIOS_DEBUG, "ME: MEI resource not present!\n");
		return -1;
	}
	mei_base_address = (u32*)(uintptr_t)res->base;

	/* Ensure Memory and Bus Master bits are set */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Clean up status for next message */
	read_host_csr(&host);
	host.interrupt_generate = 1;
	host.ready = 1;
	host.reset = 0;
	write_host_csr(&host);

	return 0;
}

/* Read the Extend register hash of ME firmware */
static int intel_me_extend_valid(device_t dev)
{
	struct me_heres status;
	u32 extend[8] = {0};
	int i, count = 0;

	pci_read_dword_ptr(dev, &status, PCI_ME_HERES);
	if (!status.extend_feature_present) {
		printk(BIOS_ERR, "ME: Extend Feature not present\n");
		return -1;
	}

	if (!status.extend_reg_valid) {
		printk(BIOS_ERR, "ME: Extend Register not valid\n");
		return -1;
	}

	switch (status.extend_reg_algorithm) {
	case PCI_ME_EXT_SHA1:
		count = 5;
		printk(BIOS_DEBUG, "ME: Extend SHA-1: ");
		break;
	case PCI_ME_EXT_SHA256:
		count = 8;
		printk(BIOS_DEBUG, "ME: Extend SHA-256: ");
		break;
	default:
		printk(BIOS_ERR, "ME: Extend Algorithm %d unknown\n",
		       status.extend_reg_algorithm);
		return -1;
	}

	for (i = 0; i < count; ++i) {
		extend[i] = pci_read_config32(dev, PCI_ME_HER(i));
		printk(BIOS_DEBUG, "%08x", extend[i]);
	}
	printk(BIOS_DEBUG, "\n");

#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Save hash in NVS for the OS to verify */
	chromeos_set_me_hash(extend, count);
#endif

	return 0;
}

/* Hide the ME virtual PCI devices */
static void intel_me_hide(device_t dev)
{
	dev->enabled = 0;
	pch_enable(dev);
}

/* Check whether ME is present and do basic init */
static void intel_me_init(device_t dev)
{
	me_bios_path path = intel_me_path(dev);

	/* Do initial setup and determine the BIOS path */
	printk(BIOS_NOTICE, "ME: BIOS path: %s\n", me_bios_path_values[path]);

	switch (path) {
	case ME_S3WAKE_BIOS_PATH:
		intel_me_hide(dev);
		break;

	case ME_NORMAL_BIOS_PATH:
		/* Validate the extend register */
		if (intel_me_extend_valid(dev) < 0)
			break; /* TODO: force recovery mode */

		/* Prepare MEI MMIO interface */
		if (intel_mei_setup(dev) < 0)
			break;

#if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG)
		/* Print ME firmware version */
		mkhi_get_fw_version();
		/* Print ME firmware capabilities */
		mkhi_get_fwcaps();
#endif

		/*
		 * Leave the ME unlocked in this path.
		 * It will be locked via SMI command later.
		 */
		break;

	case ME_ERROR_BIOS_PATH:
	case ME_RECOVERY_BIOS_PATH:
	case ME_DISABLE_BIOS_PATH:
	case ME_FIRMWARE_UPDATE_BIOS_PATH:
		break;
	}
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= intel_me_init,
	.ops_pci		= &pci_ops,
};

static const struct pci_driver intel_me __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c3a,
};

#endif /* !__SMM__ */
