/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include "amdtool.h"
#include "smn.h"

#define SMN_PSP_PUBLIC_BASE		0x3800000

#define MP0_C2P_MSG_BASE		0x10500
#define MPASP_C2P_MSG_BASE		0x10900

#define CORE_2_PSP_MSG_37_OFFSET	(MPASP_C2P_MSG_BASE + (37 * 4))
#define CORE_2_PSP_MSG_38_OFFSET	(MPASP_C2P_MSG_BASE + (38 * 4))

static const io_register_t breithorn_c2p_msg_37_fields[] = {
	{0x00, 8, "PLATFORM_VENDOR_ID"},
	{0x08, 4, "PLATFORM_MODEL_ID"},
	{0x0c, 4, "BIOS_KEY_REVISION_ID"},
	{0x10, 4, "ROOT_KEY_SELECT"},
	{0x18, 1, "PLATFORM_SECURE_BOOT_EN"},
	{0x19, 1, "DISABLE_BIOS_KEY_ANTIROLLBACK"},
	{0x1a, 1, "DISABLE_AMD_KEY_USAGE"},
	{0x1b, 1, "DISABLE_SECURE_DEBUG_UNLOCK"},
	{0x1c, 1, "CUSTOMER_KEY_LOCK"},
};

static const io_register_t breithorn_c2p_msg_38_fields[] = {
	{0x00, 8, "PSB Status/Error"},
	{0x08, 1, "PSB Fusing Readiness"},
	{0x0c, 1, "SPL Fuse Update Required"},
	{0x0d, 1, "SPL Fuse Error"},
	{0x0e, 1, "SPL Entry Error"},
	{0x0f, 1, "SPL Table Missing"},
	{0x1c, 1, "HSTISTATE_PSP_SECURE_EN"},
	{0x1d, 1, "HSTISTATE_PSP_PLATFORM_SECURE_EN"},
	{0x1e, 1, "HSTISTATE_PSP_DEBUG_LOCK_ON"},
	{0x1f, 1, "HSTISTATE_PSP_CUSTOMER_KEY_LOCK_ON"},
};
/* For better readiability and less SLOC, we override the initialized values.
 * Hide the warnings, as they will overflow the screen and make it harder to
 * focus on real compiler errors and warnings.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"

const char *const c2p_msg_38_error_codes[] = {
	[0 ... 0xff] = "Other",
	[0x00] = "PSB passed/no error",
	[0x22] = "BIOS signing key entry not found",
	[0x3e] = "Error reading fuse info",
	[0x64] = "Timeout error attempting to fuse",
	[0x6c] = "Error in BIOS Directory Table - Reset image not found",
	[0x6f] = "OEM BIOS signing key usage flag violation",
	[0x78] = "BIOS RTM signature entry not found",
	[0x79] = "BIOS copy to DRAM failed",
	[0x7a] = "BIOS RTM signature verification failed",
	[0x7b] = "OEM BIOS signing key failed signature verification",
	[0x7c] = "Platform Vendor ID and/or Model ID binding violation",
	[0x7d] = "BIOS Copy bit is unset for reset image",
	[0x7e] = "Requested fuse is already blown",
	[0x7f] = "Error with actual fusing operation",
	[0x80] = "Processor1: Error reading fuse info",
	[0x81] = "Processor1: Requested fuse is already blown",
	[0x82] = "Processor1: Platform Vendor ID and/or Model ID binding violation",
	[0x83] = "Processor1: Error with actual fusing operation",
	[0x92] = "Error in BIOS Directory Table - Firmware type mismatch",
};

#pragma GCC diagnostic pop

static void print_psb_status_v2(const char * const *psb_status_codes,
				const io_register_t *status_fields, size_t status_size,
				const io_register_t *hsti_fields, size_t hsti_size)
{
	uint32_t status, hsti;
	uint8_t psb_state;
	size_t i;

	status = smn_read32(SMN_PSP_PUBLIC_BASE + CORE_2_PSP_MSG_37_OFFSET);
	printf("PSB: STATUS = %08"PRIx32"\n", status);

	if (status_fields) {
		for (i = 0; i < status_size; i++) {
			unsigned int val = status >> status_fields[i].addr;
			val &= ((1 << status_fields[i].size) -1);
			printf("0x%04x = %s\n", val, status_fields[i].name);
		}
	}

	hsti = smn_read32(SMN_PSP_PUBLIC_BASE + CORE_2_PSP_MSG_38_OFFSET);
	printf("PSB: HSTI = %08"PRIx32"\n", hsti);

	if (hsti_fields) {
		for (i = 0; i < hsti_size; i++) {
			unsigned int val = hsti >> hsti_fields[i].addr;
			val &= ((1 << hsti_fields[i].size) -1);
			if (psb_status_codes && i == 0)
				printf("0x%04x = %s (%s)\n", val, hsti_fields[i].name, psb_status_codes[val]);
			else
				printf("0x%04x = %s\n", val, hsti_fields[i].name);
		}
	}

	printf("\n\n");

	psb_state = ((status >> 23) & 2) | ((status >> 28) & 1);
	switch (psb_state) {
	case 0:
		printf("PSB state: NOT ENABLED\n");
		printf("You may flash other firmware!\n"
		       "But the Platform Secure Boot state may still be changed to ENABLED or DISABLED!\n");
		break;
	case 1:
		printf("PSB state: DISABLED\n");
		printf("You may flash other firmware!\n"
		       "Platform Secure Boot is permanently DISABLED!\n");
		break;
	case 2:
		printf("PSB state: ENABLED\n");
		printf("You may NOT flash other firmware!\n"
		       "Platform Secure Boot is permanently ENABLED!\n");
		break;
	default:
		printf("PSB state: UNKNOWN\n");
		break;
	}
}

void print_psb(struct pci_dev *nb)
{
	printf("============= Platform Secure Boot ==============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_AMD_BRH_ROOT_COMPLEX:
		print_psb_status_v2(c2p_msg_38_error_codes,
				    breithorn_c2p_msg_37_fields,
				    ARRAY_SIZE(breithorn_c2p_msg_37_fields),
				    breithorn_c2p_msg_38_fields,
				    ARRAY_SIZE(breithorn_c2p_msg_38_fields));
		break;
	default:
		printf("Error: Dumping PSB status on this northbridge is not (yet) supported.\n");
		break;
	}

	printf("\n=================================================\n");
}
