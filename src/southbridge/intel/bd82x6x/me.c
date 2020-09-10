/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This is a ramstage driver for the Intel Management Engine found in the
 * 6-series chipset.  It handles the required boot-time messages over the
 * MMIO-based Management Engine Interface to tell the ME that the BIOS is
 * finished with POST.  Additional messages are defined for debug but are
 * not used unless the console loglevel is high enough.
 */

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <string.h>
#include <delay.h>
#include <elog.h>

#include "me.h"
#include "pch.h"

#if CONFIG(CHROMEOS)
#include <vendorcode/google/chromeos/gnvs.h>
#endif

/* Send END OF POST message to the ME */
static int __unused mkhi_end_of_post(void)
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

/* Get ME firmware version */
static int __unused mkhi_get_fw_version(void)
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
static int __unused mkhi_get_fwcaps(void)
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

#ifdef __SIMPLE_DEVICE__

static void intel_me7_finalize_smm(void)
{
	struct me_hfs hfs;
	u32 reg32;

	update_mei_base_address();

	/* S3 path will have hidden this device already */
	if (!is_mei_base_address_valid())
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
	pci_and_config16(PCH_ME_DEV, PCI_COMMAND,
			 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

	/* Hide the PCI device */
	RCBA32_OR(FD2, PCH_DISABLE_MEI1);
}

void intel_me_finalize_smm(void)
{
	u16 did = pci_read_config16(PCH_ME_DEV, PCI_DEVICE_ID);
	switch (did) {
	case 0x1c3a:
		intel_me7_finalize_smm();
		break;
	case 0x1e3a:
		intel_me8_finalize_smm();
		break;
	default:
		printk(BIOS_ERR, "No finalize handler for ME %04x.\n", did);
	}
}

#else

/* Determine the path that we should take based on ME status */
static me_bios_path intel_me_path(struct device *dev)
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

/* Check whether ME is present and do basic init */
static void intel_me_init(struct device *dev)
{
	me_bios_path path = intel_me_path(dev);

	/* Do initial setup and determine the BIOS path */
	printk(BIOS_NOTICE, "ME: BIOS path: %s\n", me_get_bios_path_string(path));

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

		if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG) {
			/* Print ME firmware version */
			mkhi_get_fw_version();
			/* Print ME firmware capabilities */
			mkhi_get_fwcaps();
		}

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

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= intel_me_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const struct pci_driver intel_me __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c3a,
};

#endif /* __SIMPLE_DEVICE__ */
