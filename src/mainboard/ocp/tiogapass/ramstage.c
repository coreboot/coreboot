/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <drivers/ipmi/ipmi_ops.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <soc/ramstage.h>

extern struct fru_info_str fru_strings;

static const struct port_information SMBIOS_type8_info[] = {
	/*
	* Port Information fields:
	*	Internal Reference Designator,
	*	Internal Connector Type,
	*	External Reference Designator,
	*	External Connector_Type,
	*	Port Type
	*/
	{
		"J7F5 - BMC JTAG HEADER",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		"J8A1 - MINISAS1",
		CONN_SAS_SATA,
		NULL,
		CONN_NONE,
		TYPE_SAS
	},
	{
		"J8A2 - MINISAS2",
		CONN_SAS_SATA,
		NULL,
		CONN_NONE,
		TYPE_SAS
	},
	{
		"J8A3 - SATA CONBINE1",
		CONN_SAS_SATA,
		NULL,
		CONN_NONE,
		TYPE_SAS
	},
	{
		"J8B1 - ME_DBG",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		"J8D1 - VR_DBG",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		"J8E1 - TPM_MODULE",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		"J8F1 - M.2 CONNECTOR",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		"J9A1 - SATA RAID KEY",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		NULL,
		CONN_NONE,
		"J9A2 - DEBUG 80 PORT",
		CONN_OTHER,
		TYPE_OTHER_PORT
	},
	{
		"J9A3 - CPU & PCH XDP",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		NULL,
		CONN_NONE,
		"J9A5 - USB conn",
		CONN_ACCESS_BUS_USB,
		TYPE_USB
	},
	{
		"J9B1 - BMC_DBG",
		CONN_OTHER,
		NULL,
		CONN_NONE,
		TYPE_OTHER_PORT
	},
	{
		NULL,
		CONN_NONE,
		"J9D1 - USB3.0 TYPE C",
		CONN_ACCESS_BUS_USB,
		TYPE_USB
	},
	{
		NULL,
		CONN_NONE,
		"J9E1 - VGA",
		CONN_OTHER,
		TYPE_OTHER_PORT
	},
	{
		NULL,
		CONN_NONE,
		"JA9G1 - ETH0",
		CONN_RJ_45,
		TYPE_NETWORK_PORT
	},
};

void mainboard_silicon_init_params(FSPS_UPD *params)
{
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int mainboard_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len = 0;

	// add port information
	len += smbios_write_type8(
		current, handle,
		SMBIOS_type8_info,
		ARRAY_SIZE(SMBIOS_type8_info)
		);

	return len;
}
#endif

static void tp_oem_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	/* OEM string 1 to 6 */
	ocp_oem_smbios_strings(dev, t);

	/* OEM string 7 */
	if (fru_strings.board_info.custom_count > 1 &&
			*(fru_strings.board_info.board_custom + 1) != NULL)
		t->count = smbios_add_oem_string(t->eos,
			*(fru_strings.board_info.board_custom + 1));
	else
		t->count = smbios_add_oem_string(t->eos, TBF);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_strings = tp_oem_smbios_strings;
	read_fru_areas(CONFIG_BMC_KCS_BASE, CONFIG_FRU_DEVICE_ID, 0, &fru_strings);
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif
}

static void mainboard_final(void *chip_info)
{
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
