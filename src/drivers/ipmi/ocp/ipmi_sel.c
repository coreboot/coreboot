/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <drivers/ipmi/ipmi_if.h>
#include <string.h>

#include "ipmi_ocp.h"

static int ipmi_add_sel_entry(int port, unsigned char *data, int size)
{
	return ipmi_message(port, IPMI_NETFN_STORAGE, 0, IPMI_ADD_SEL_ENTRY, data, size,
				    NULL, 0);
}

void ipmi_send_to_bmc(unsigned char *data, size_t size)
{
	if (CONFIG(IPMI_KCS)) {
		_Static_assert(CONFIG_BMC_KCS_BASE != 0,
			"\tBMC_ERROR: Unable to send record: Port #:0\n");

		ipmi_add_sel_entry(CONFIG_BMC_KCS_BASE, data, size);
	}
}

void ipmi_send_sel_iio_err(uint8_t iio_stack_num, uint8_t err_id)
{
	struct ipmi_sel_iio_err ubslp = {
		.record_id = SEL_RECORD_ID,
		.record_type = CONFIG_RAS_SEL_VENDOR_ID,
		.general_info = SEL_PCIE_IIO_ERR,
		.iio_stack_num = iio_stack_num,
		.iio_err_id = err_id,
	};

	ipmi_send_to_bmc((unsigned char *)&ubslp, sizeof(ubslp));
	printk(BIOS_DEBUG, "\tsending PCIE IIO device error record to BMC\n");
	printk(BIOS_DEBUG, "\tstack # = %x\n", ubslp.iio_stack_num);
}

void ipmi_send_sel_pcie_dev_err(pci_devfn_t bdf, uint16_t prmry_cnt, uint8_t sec_id,
				uint8_t prmry_id)
{
	struct pci_dev_fn *inbdf = (struct pci_dev_fn *)&bdf;
	struct ipmi_sel_pcie_dev_err ubslp = {
		.record_id = SEL_RECORD_ID,
		.record_type = CONFIG_RAS_SEL_VENDOR_ID,
		.general_info = SEL_PCIE_DEV_ERR,
		.timestamp = 0, /* BMC will apply timestamp */
		.aux_loc = 0,
		.bdf.bus = inbdf->bus,
		.bdf.dev = inbdf->dev,
		.bdf.func = inbdf->func >> 12,
		.primary_err_count = prmry_cnt,
		.primary_id = prmry_id,
		.secondary_id = sec_id,
	};

	ipmi_send_to_bmc((unsigned char *)&ubslp, sizeof(ubslp));
	printk(BIOS_DEBUG, "\tsending PCIE device error record to BMC\n");
	printk(BIOS_DEBUG, "\tbdf = %x:%x:%x\n", ubslp.bdf.bus, ubslp.bdf.dev, ubslp.bdf.func);
	printk(BIOS_DEBUG, "\tubslp.primary_id = %x\n", ubslp.primary_id);
	printk(BIOS_DEBUG, "\tsecondary_id = %x\n", ubslp.secondary_id);
}

void ipmi_send_sel_pcie_dev_fail(uint16_t sts_reg, uint16_t src_id, enum fail_type code)
{
	struct ipmi_sel_pcie_dev_fail ubslp = {
		.record_id = SEL_RECORD_ID,
		.record_type = CONFIG_RAS_SEL_VENDOR_ID,
		.general_info = SEL_PCIE_DEV_FAIL_ID,
		.timestamp = 0, /* BMC will apply timestamp */
		.type = code,
		.failure_details1 = sts_reg,
		.failure_details2 = src_id,
	};

	ipmi_send_to_bmc((unsigned char *)&ubslp, sizeof(ubslp));
	printk(BIOS_DEBUG, "\tsending PCI device FAILURE record to BMC\n");
	printk(BIOS_DEBUG, "\terror_code = %x, src_id = %x\n", ubslp.type,
	       ubslp.failure_details2);
}

enum cb_err ipmi_get_board_config(const int port, struct ipmi_config_rsp *config)
{
	int ret;
	struct ipmi_oem_rsp {
		struct ipmi_rsp resp;
		struct ipmi_config_rsp data;
	} __packed;

	struct ipmi_oem_rsp rsp;

	ret = ipmi_message(port, IPMI_NETFN_OEM, 0x0, IPMI_OEM_GET_BOARD_ID, NULL, 0,
			   (unsigned char *)&rsp, sizeof(rsp));
	if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.resp.completion_code);
		return CB_ERR;
	}
	*config = rsp.data;
	return CB_SUCCESS;
}

__weak uint8_t get_blade_id(void)
{
	struct ipmi_config_rsp rsp = {.slot_id = UINT8_MAX};

	if (CONFIG(IPMI_KCS) && CONFIG_BMC_KCS_BASE) {
		if (ipmi_get_board_config(CONFIG_BMC_KCS_BASE, &rsp) != CB_SUCCESS)
			return UINT8_MAX;
	}
	return rsp.slot_id;
}
