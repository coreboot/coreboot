/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#include <console/console.h>
#include <drivers/ipmi/ipmi_if.h>
#include <string.h>
#include <build.h>
#include "ipmi_supermicro_oem.h"

#define IPMI_NETFN_OEM 0x30
#define IPMI_LUN0_AC_SET_BIOS_VER 0x100
#define IPMI_LUN0_AC_SET_BIOS_DATE 0x101
#define IPMI_LUN0_SET_BIOS_STRING 0xac

struct ipmi_oem_set_bios_str {
	uint16_t ver;
	char str[16]; // NULL terminated string
} __packed;

static void set_coreboot_ver(const uint16_t kcs_port)
{
	const char *coreboot_ver = COREBOOT_VERSION;
	struct ipmi_oem_set_bios_str bios_ver;
	struct ipmi_rsp rsp;
	int ret;
	size_t i;

	/* Only 8 characters are visible in UI. Cut of on first dash */
	for (i = 0; i < 15; i++) {
		if (coreboot_ver[i] == '-')
			break;
		bios_ver.str[i] = coreboot_ver[i];
	}
	bios_ver.str[i] = 0;
	bios_ver.ver = IPMI_LUN0_AC_SET_BIOS_VER;

	ret = ipmi_message(kcs_port, IPMI_NETFN_OEM, 0, IPMI_LUN0_SET_BIOS_STRING,
			   (const unsigned char *) &bios_ver, sizeof(bios_ver),
			   (unsigned char *) &rsp, sizeof(rsp));
	if (ret < sizeof(rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "BMC_IPMI: %s command failed (ret=%d resp=0x%x)\n",
		       __func__, ret, rsp.completion_code);
	}
}

static void set_coreboot_date(const uint16_t kcs_port)
{
	struct ipmi_oem_set_bios_str bios_ver;
	struct ipmi_rsp rsp;
	int ret;

	strncpy(bios_ver.str, COREBOOT_DMI_DATE, 15);
	bios_ver.str[15] = 0;
	bios_ver.ver = IPMI_LUN0_AC_SET_BIOS_DATE;

	ret = ipmi_message(kcs_port, IPMI_NETFN_OEM, 0, IPMI_LUN0_SET_BIOS_STRING,
			   (const unsigned char *) &bios_ver, sizeof(bios_ver),
			   (unsigned char *) &rsp, sizeof(rsp));
	if (ret < sizeof(rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "BMC_IPMI: %s command failed (ret=%d resp=0x%x)\n",
		       __func__, ret, rsp.completion_code);
	}
}

void supermicro_ipmi_oem(const uint16_t kcs_port)
{
	set_coreboot_ver(kcs_port);
	set_coreboot_date(kcs_port);
}
