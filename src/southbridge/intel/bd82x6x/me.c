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
#include <elog.h>
#include <halt.h>
#include <option.h>
#include <southbridge/intel/common/me.h>

#include "me.h"
#include "pch.h"

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

	/* Check for any error code and valid firmware */
	if (hfs.error_code || hfs.fpt_bad)
		path = ME_ERROR_BIOS_PATH;

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


/* Check whether ME is present and do basic init */
static void intel_me_init(struct device *dev)
{
	me_bios_path path = intel_me_path(dev);
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

		if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG) {
			/* Print ME firmware version */
			mkhi_get_fw_version();
			/* Print ME firmware capabilities */
			mkhi_get_fwcaps();
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
	.device	= 0x1c3a,
};
