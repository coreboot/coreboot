/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This is a ramstage driver for the Intel Management Engine found in the
 * 6-series chipset.  It handles the required boot-time messages over the
 * MMIO-based Management Engine Interface to tell the ME that the BIOS is
 * finished with POST.  Additional messages are defined for debug but are
 * not used unless the console loglevel is high enough.
 */

#include <acpi/acpi.h>
#include <cf9_reset.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <string.h>
#include <elog.h>
#include <halt.h>
#include <option.h>
#include <southbridge/intel/common/me.h>

#include "me.h"
#include "pch.h"

static inline void print_cap(const char *name, int state)
{
	printk(BIOS_DEBUG, "ME Capability: %-41s : %sabled\n",
	       name, state ? " en" : "dis");
}

static void me_print_fw_version(mbp_fw_version_name *vers_name)
{
	if (!vers_name->major_version) {
		printk(BIOS_ERR, "ME: mbp missing version report\n");
		return;
	}

	printk(BIOS_DEBUG, "ME: found version %d.%d.%d.%d\n",
	       vers_name->major_version, vers_name->minor_version,
	       vers_name->hotfix_version, vers_name->build_version);
}

/* Determine the path that we should take based on ME status */
static me_bios_path intel_me_path(struct device *dev)
{
	me_bios_path path = ME_DISABLE_BIOS_PATH;
	union me_hfs hfs;
	union me_gmes gmes;

	/* S3 wake skips all MKHI messages */
	if (acpi_is_wakeup_s3())
		return ME_S3WAKE_BIOS_PATH;

	hfs.raw = pci_read_config32(dev, PCI_ME_HFS);
	gmes.raw = pci_read_config32(dev, PCI_ME_GMES);

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

	/* Check for any error code and valid firmware and MBP */
	if (hfs.error_code || hfs.fpt_bad)
		path = ME_ERROR_BIOS_PATH;

	/* Check if the MBP is ready */
	if (!gmes.mbp_rdy) {
		printk(BIOS_CRIT, "%s: mbp is not ready!\n", __func__);
		path = ME_ERROR_BIOS_PATH;
	}

	if (CONFIG(ELOG) && path != ME_NORMAL_BIOS_PATH) {
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

	return path;
}

static int intel_me_read_mbp(me_bios_payload *mbp_data);

/* Get ME Firmware Capabilities */
static int mkhi_get_fwcaps(mefwcaps_sku *cap)
{
	u32 rule_id = 0;
	struct me_fwcaps cap_msg;
	struct mkhi_header mkhi = {
		.group_id       = MKHI_GROUP_ID_FWCAPS,
		.command        = MKHI_FWCAPS_GET_RULE,
	};
	struct mei_header mei = {
		.is_complete    = 1,
		.host_address   = MEI_HOST_ADDRESS,
		.client_address = MEI_ADDRESS_MKHI,
		.length         = sizeof(mkhi) + sizeof(rule_id),
	};

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, &rule_id, &cap_msg, sizeof(cap_msg)) < 0) {
		printk(BIOS_ERR, "ME: GET FWCAPS message failed\n");
		return -1;
	}
	*cap = cap_msg.caps_sku;
	return 0;
}

/* Get ME Firmware Capabilities */
static void me_print_fwcaps(mbp_fw_caps *caps_section)
{
	mefwcaps_sku *cap = &caps_section->fw_capabilities;
	if (!caps_section->available) {
		printk(BIOS_ERR, "ME: mbp missing fwcaps report\n");
		if (mkhi_get_fwcaps(cap))
			return;
	}

	print_cap("Full Network manageability", cap->full_net);
	print_cap("Regular Network manageability", cap->std_net);
	print_cap("Manageability", cap->manageability);
	print_cap("Small business technology", cap->small_business);
	print_cap("Level III manageability", cap->l3manageability);
	print_cap("IntelR Anti-Theft (AT)", cap->intel_at);
	print_cap("IntelR Capability Licensing Service (CLS)", cap->intel_cls);
	print_cap("IntelR Power Sharing Technology (MPC)", cap->intel_mpc);
	print_cap("ICC Over Clocking", cap->icc_over_clocking);
	print_cap("Protected Audio Video Path (PAVP)", cap->pavp);
	print_cap("IPV6", cap->ipv6);
	print_cap("KVM Remote Control (KVM)", cap->kvm);
	print_cap("Outbreak Containment Heuristic (OCH)", cap->och);
	print_cap("Virtual LAN (VLAN)", cap->vlan);
	print_cap("TLS", cap->tls);
	print_cap("Wireless LAN (WLAN)", cap->wlan);
}

/* Check whether ME is present and do basic init */
static void intel_me_init(struct device *dev)
{
	me_bios_path path = intel_me_path(dev);
	me_bios_payload mbp_data;
	bool need_reset = false;
	union me_hfs hfs;

	/* Do initial setup and determine the BIOS path */
	printk(BIOS_NOTICE, "ME: BIOS path: %s\n", me_get_bios_path_string(path));

	u8 me_state = get_uint_option("me_state", 0);
	u8 me_state_prev = get_uint_option("me_state_prev", 0);

	printk(BIOS_DEBUG, "ME: me_state=%u, me_state_prev=%u\n", me_state, me_state_prev);

	switch (path) {
	case ME_S3WAKE_BIOS_PATH:
#if CONFIG(HIDE_MEI_ON_ERROR)
	case ME_ERROR_BIOS_PATH:
#endif
		intel_me_hide(dev);
		break;

	case ME_NORMAL_BIOS_PATH:
		/* Validate the extend register */
		if (intel_me_extend_valid(dev) < 0)
			break; /* TODO: force recovery mode */

		/* Prepare MEI MMIO interface */
		if (intel_mei_setup(dev) < 0)
			break;

		if (intel_me_read_mbp(&mbp_data))
			break;

		if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG) {
			me_print_fw_version(&mbp_data.fw_version_name);
			me_print_fwcaps(&mbp_data.fw_caps_sku);
		}

		/* Put ME in Software Temporary Disable Mode, if needed */
		if (me_state == CMOS_ME_STATE_DISABLED
				&& CMOS_ME_STATE(me_state_prev) == CMOS_ME_STATE_NORMAL) {
			printk(BIOS_INFO, "ME: disabling ME\n");
			if (enter_soft_temp_disable()) {
				enter_soft_temp_disable_wait();
				need_reset = true;
			} else {
				printk(BIOS_ERR, "ME: failed to enter Soft Temporary Disable mode\n");
			}

			break;
		}

		/*
		 * Leave the ME unlocked in this path.
		 * It will be locked via SMI command later.
		 */
		break;

	case ME_DISABLE_BIOS_PATH:
		/* Bring ME out of Soft Temporary Disable mode, if needed */
		hfs.raw = pci_read_config32(dev, PCI_ME_HFS);
		if (hfs.operation_mode == ME_HFS_MODE_DIS
				&& me_state == CMOS_ME_STATE_NORMAL
				&& (CMOS_ME_STATE(me_state_prev) == CMOS_ME_STATE_DISABLED
					|| !CMOS_ME_CHANGED(me_state_prev))) {
			printk(BIOS_INFO, "ME: re-enabling ME\n");

			exit_soft_temp_disable(dev);
			exit_soft_temp_disable_wait(dev);

			/*
			 * ME starts loading firmware immediately after writing to H_GS,
			 * but Lenovo BIOS performs a reboot after bringing ME back to
			 * Normal mode. Assume that global reset is needed.
			 */
			need_reset = true;
		} else {
			intel_me_hide(dev);
		}
		break;

#if !CONFIG(HIDE_MEI_ON_ERROR)
	case ME_ERROR_BIOS_PATH:
#endif
	case ME_RECOVERY_BIOS_PATH:
	case ME_FIRMWARE_UPDATE_BIOS_PATH:
		break;
	}

	/* To avoid boot loops if ME fails to get back from disabled mode,
	   set the 'changed' bit here. */
	if (me_state != CMOS_ME_STATE(me_state_prev) || need_reset) {
		u8 new_state = me_state | CMOS_ME_STATE_CHANGED;
		set_uint_option("me_state_prev", new_state);
	}

	if (need_reset) {
		set_global_reset(true);
		full_reset();
	}
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= intel_me_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const struct pci_driver intel_me __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x1e3a,
};

/******************************************************************************
 *									     */
static u32 me_to_host_words_pending(void)
{
	struct mei_csr me;
	read_me_csr(&me);
	if (!me.ready)
		return 0;
	return (me.buffer_write_ptr - me.buffer_read_ptr) &
		(me.buffer_depth - 1);
}

/*
 * mbp seems to be following its own flow, let's retrieve it in a dedicated
 * function.
 */
static int intel_me_read_mbp(me_bios_payload *mbp_data)
{
	mbp_header mbp_hdr;
	mbp_item_header	mbp_item_hdr;
	u32 me2host_pending;
	u32 mbp_item_id;
	struct mei_csr host;

	me2host_pending = me_to_host_words_pending();
	if (!me2host_pending) {
		printk(BIOS_ERR, "ME: no mbp data!\n");
		return -1;
	}

	/* we know for sure that at least the header is there */
	mei_read_dword_ptr(&mbp_hdr, MEI_ME_CB_RW);

	if ((mbp_hdr.num_entries > (mbp_hdr.mbp_size / 2)) ||
	    (me2host_pending < mbp_hdr.mbp_size)) {
		printk(BIOS_ERR, "ME: mbp of %d entries, total size %d words"
		       " buffer contains %d words\n",
		       mbp_hdr.num_entries, mbp_hdr.mbp_size,
		       me2host_pending);
		return -1;
	}

	me2host_pending--;
	memset(mbp_data, 0, sizeof(*mbp_data));

	while (mbp_hdr.num_entries--) {
		u32 *copy_addr;
		u32 copy_size, buffer_room;
		void *p;

		if (!me2host_pending) {
			printk(BIOS_ERR, "ME: no mbp data %d entries to go!\n",
			       mbp_hdr.num_entries + 1);
			return -1;
		}

		mei_read_dword_ptr(&mbp_item_hdr, MEI_ME_CB_RW);

		if (mbp_item_hdr.length > me2host_pending) {
			printk(BIOS_ERR, "ME: insufficient mbp data %d "
			       "entries to go!\n",
			       mbp_hdr.num_entries + 1);
			return -1;
		}

		me2host_pending -= mbp_item_hdr.length;

		mbp_item_id = (((u32)mbp_item_hdr.item_id) << 8) +
			mbp_item_hdr.app_id;

		copy_size = mbp_item_hdr.length - 1;

#define SET_UP_COPY(field) { copy_addr = (u32 *)&mbp_data->field;	     \
			buffer_room = sizeof(mbp_data->field) / sizeof(u32); \
			break;					             \
		}

		p = &mbp_item_hdr;
		printk(BIOS_INFO, "ME: MBP item header %8.8x\n", *((u32*)p));

		switch (mbp_item_id) {
		case 0x101:
			SET_UP_COPY(fw_version_name);

		case 0x102:
			SET_UP_COPY(icc_profile);

		case 0x103:
			SET_UP_COPY(at_state);

		case 0x201:
			mbp_data->fw_caps_sku.available = 1;
			SET_UP_COPY(fw_caps_sku.fw_capabilities);

		case 0x301:
			SET_UP_COPY(rom_bist_data);

		case 0x401:
			SET_UP_COPY(platform_key);

		case 0x501:
			mbp_data->fw_plat_type.available = 1;
			SET_UP_COPY(fw_plat_type.rule_data);

		case 0x601:
			SET_UP_COPY(mfsintegrity);

		default:
			printk(BIOS_ERR, "ME: unknown mbp item id 0x%x! Skipping\n",
			       mbp_item_id);
			while (copy_size--)
				read_cb();
			continue;
		}

		if (buffer_room != copy_size) {
			printk(BIOS_ERR, "ME: buffer room %d != %d copy size"
			       " for item  0x%x!!!\n",
			       buffer_room, copy_size, mbp_item_id);
			return -1;
		}
		while (copy_size--)
			*copy_addr++ = read_cb();
	}

	read_host_csr(&host);
	host.interrupt_generate = 1;
	write_host_csr(&host);

	{
		int cntr = 0;
		while (host.interrupt_generate) {
			read_host_csr(&host);
			cntr++;
		}
		printk(BIOS_SPEW, "ME: mbp read OK after %d cycles\n", cntr);
	}

	return 0;
}
