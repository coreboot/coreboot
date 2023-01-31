/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <string.h>
#include <delay.h>
#include <halt.h>
#include <timer.h>

#include "me.h"
#include "pch.h"

#include <vendorcode/google/chromeos/chromeos.h>

/* Path that the BIOS should take based on ME state */
static const char *const me_bios_path_values[] = {
	[ME_NORMAL_BIOS_PATH]		= "Normal",
	[ME_S3WAKE_BIOS_PATH]		= "S3 Wake",
	[ME_ERROR_BIOS_PATH]		= "Error",
	[ME_RECOVERY_BIOS_PATH]		= "Recovery",
	[ME_DISABLE_BIOS_PATH]		= "Disable",
	[ME_FIRMWARE_UPDATE_BIOS_PATH]	= "Firmware Update",
};

const char *const me_get_bios_path_string(int path)
{
	return me_bios_path_values[path];
}

/* MMIO base address for MEI interface */
static u32 *mei_base_address;

static void mei_dump(void *ptr, int dword, int offset, const char *type)
{
	struct mei_csr *csr;

	if (!CONFIG(DEBUG_INTEL_ME))
		return;

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

/*
 * ME/MEI access helpers using memcpy to avoid aliasing.
 */

void mei_read_dword_ptr(void *ptr, int offset)
{
	u32 dword = read32(mei_base_address + (offset / sizeof(u32)));
	memcpy(ptr, &dword, sizeof(dword));
	mei_dump(ptr, dword, offset, "READ");
}

void mei_write_dword_ptr(void *ptr, int offset)
{
	u32 dword = 0;
	memcpy(&dword, ptr, sizeof(dword));
	write32(mei_base_address + (offset / sizeof(u32)), dword);
	mei_dump(ptr, dword, offset, "WRITE");
}

void read_host_csr(struct mei_csr *csr)
{
	mei_read_dword_ptr(csr, MEI_H_CSR);
}

void write_host_csr(struct mei_csr *csr)
{
	mei_write_dword_ptr(csr, MEI_H_CSR);
}

void read_me_csr(struct mei_csr *csr)
{
	mei_read_dword_ptr(csr, MEI_ME_CSR_HA);
}

void write_cb(u32 dword)
{
	write32(mei_base_address + (MEI_H_CB_WW / sizeof(u32)), dword);
	mei_dump(NULL, dword, MEI_H_CB_WW, "WRITE");
}

u32 read_cb(void)
{
	u32 dword = read32(mei_base_address + (MEI_ME_CB_RW / sizeof(u32)));
	mei_dump(NULL, dword, MEI_ME_CB_RW, "READ");
	return dword;
}

/* Wait for ME ready bit to be asserted */
static int mei_wait_for_me_ready(void)
{
	struct mei_csr me;
	unsigned int try = ME_RETRY;

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

static int mei_send_msg(struct mei_header *mei, struct mkhi_header *mkhi, void *req_data)
{
	struct mei_csr host;
	unsigned int ndata, n;
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

static int mei_recv_msg(struct mkhi_header *mkhi, void *rsp_data, int rsp_bytes)
{
	struct mei_header mei_rsp;
	struct mkhi_header mkhi_rsp;
	struct mei_csr me, host;
	unsigned int ndata, n;
	unsigned int expected;
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
		printk(BIOS_ERR, "ME: timeout waiting for data: expected %u, available %u\n",
		       expected, me.buffer_write_ptr - me.buffer_read_ptr);
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
		printk(BIOS_ERR, "ME: response is missing data %d != %d\n",
		       ndata, (expected - 1));
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
		printk(BIOS_ERR, "ME: not enough room in response buffer: %u != %u\n",
		       ndata, rsp_bytes >> 2);
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

int mei_sendrecv(struct mei_header *mei, struct mkhi_header *mkhi,
		 void *req_data, void *rsp_data, int rsp_bytes)
{
	if (mei_send_msg(mei, mkhi, req_data) < 0)
		return -1;
	if (mei_recv_msg(mkhi, rsp_data, rsp_bytes) < 0)
		return -1;
	return 0;
}

#ifdef __SIMPLE_DEVICE__

void update_mei_base_address(void)
{
	uint32_t reg32 = pci_read_config32(PCH_ME_DEV, PCI_BASE_ADDRESS_0) & ~0xf;
	mei_base_address = (u32 *)(uintptr_t)reg32;
}

bool is_mei_base_address_valid(void)
{
	return mei_base_address && mei_base_address != (u32 *)0xfffffff0;
}

#else

/* Prepare ME for MEI messages */
int intel_mei_setup(struct device *dev)
{
	struct resource *res;
	struct mei_csr host;

	/* Find the MMIO base for the ME interface */
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res || res->base == 0 || res->size == 0) {
		printk(BIOS_DEBUG, "ME: MEI resource not present!\n");
		return -1;
	}
	mei_base_address = (u32 *)(uintptr_t)res->base;

	/* Ensure Memory and Bus Master bits are set */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Clean up status for next message */
	read_host_csr(&host);
	host.interrupt_generate = 1;
	host.ready = 1;
	host.reset = 0;
	write_host_csr(&host);

	return 0;
}

/* Read the Extend register hash of ME firmware */
int intel_me_extend_valid(struct device *dev)
{
	union me_heres status;
	u32 extend[8] = {0};
	int i, count = 0;

	status.raw = pci_read_config32(dev, PCI_ME_HERES);
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

	/* Save hash in NVS for the OS to verify */
	if (CONFIG(CHROMEOS_NVS))
		chromeos_set_me_hash(extend, count);

	return 0;
}

/* Hide the ME virtual PCI devices */
void intel_me_hide(struct device *dev)
{
	dev->enabled = 0;
	pch_enable(dev);
}

bool enter_soft_temp_disable(void)
{
	/* The binary sequence for the disable command was found by PT in some vendor BIOS */
	struct me_disable message = {
		.rule_id = MKHI_DISABLE_RULE_ID,
		.data = 0x01,
	};
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_FWCAPS,
		.command	= MKHI_FWCAPS_SET_RULE,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.length		= sizeof(mkhi) + sizeof(message),
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
	};
	u32 resp;

	if (mei_sendrecv(&mei, &mkhi, &message, &resp, sizeof(resp)) < 0
	    || resp != MKHI_DISABLE_RULE_ID) {
		printk(BIOS_WARNING, "ME: disable command failed\n");
		return false;
	}

	return true;
}

void enter_soft_temp_disable_wait(void)
{
	/*
	 * TODO: Find smarter way to determine when we're ready to reboot.
	 *
	 * There has to be some bit in some register, or something, that indicates that ME has
	 * finished doing its thing and we're ready to reboot.
	 *
	 * It was not found yet, though, and waiting for a response after the disable command is
	 * not enough. If we reboot too early, ME will not be disabled on next boot. For now,
	 * let's just wait for 1 second here.
	 */
	mdelay(1000);
}

void exit_soft_temp_disable(struct device *dev)
{
	/* To bring ME out of Soft Temporary Disable Mode, host writes 0x20000000 to H_GS */
	pci_write_config32(dev, PCI_ME_H_GS, 0x2 << 28);
}

void exit_soft_temp_disable_wait(struct device *dev)
{
	union me_hfs hfs;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, ME_ENABLE_TIMEOUT);

	/**
	 * Wait for fw_init_complete. Check every 50 ms, give up after 20 sec.
	 * This is what vendor BIOS does. Usually it takes 1.5 seconds or so.
	 */
	do {
		mdelay(50);
		hfs.raw = pci_read_config32(dev, PCI_ME_HFS);
		if (hfs.fw_init_complete)
			break;
	} while (!stopwatch_expired(&sw));

	if (!hfs.fw_init_complete)
		printk(BIOS_ERR, "ME: giving up on waiting for fw_init_complete\n");
	else
		printk(BIOS_NOTICE, "ME: took %lldms to complete initialization\n",
		       stopwatch_duration_msecs(&sw));
}

#endif
