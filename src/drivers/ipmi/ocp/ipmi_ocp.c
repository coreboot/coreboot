/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Place in devicetree.cb:
 *
 * chip drivers/ipmi/ocp # OCP specific IPMI porting
	device pnp ca2.1 on end
 * end
 */

#include <arch/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <drivers/ipmi/ipmi_kcs.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <intelblocks/cpulib.h>
#include <string.h>
#include <types.h>

#include "ipmi_ocp.h"

static int ipmi_set_processor_information_param1(struct device *dev)
{
	int ret;
	struct ipmi_processor_info_param1_req req1 = {0};
	struct ipmi_rsp rsp;
	int mfid = CONFIG_IPMI_OCP_MANU_ID;

	memcpy(&req1.data.manufacturer_id, &mfid, 3);
	printk(BIOS_DEBUG, "IPMI BMC manufacturer id: %02x%02x%02x\n",
		req1.data.manufacturer_id[2], req1.data.manufacturer_id[1],
		req1.data.manufacturer_id[0]);

	req1.data.index = 0;
	req1.data.parameter_selector = 1;

	/* Get processor name. */
	fill_processor_name(req1.product_name);
	printk(BIOS_DEBUG, "IPMI BMC CPU NAME: %s.\n", req1.product_name);

	ret = ipmi_kcs_message(dev->path.pnp.port, IPMI_NETFN_OEM_COMMON, 0,
				 IPMI_BMC_SET_PROCESSOR_INFORMATION, (u8 *) &req1,
				 sizeof(req1), (u8 *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI BMC: %s command failed (ret=%d rsp=0x%x)\n",
		       __func__, ret, rsp.completion_code);
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static int ipmi_set_processor_information_param2(struct device *dev)
{
	int ret;
	struct ipmi_processor_info_param2_req req2 = {0};
	struct ipmi_rsp rsp;
	uint8_t stepping_id;
	int mfid = CONFIG_IPMI_OCP_MANU_ID;
	unsigned int core_count, thread_count;
	struct cpuinfo_x86 c;

	memcpy(&req2.data.manufacturer_id, &mfid, 3);
	printk(BIOS_DEBUG, "IPMI BMC manufacturer id: %02x%02x%02x\n",
		req2.data.manufacturer_id[2], req2.data.manufacturer_id[1],
		req2.data.manufacturer_id[0]);

	req2.data.index = 0;
	req2.data.parameter_selector = 2;

	/* Get core number and thread number. */
	cpu_read_topology(&core_count, &thread_count);
	req2.core_number = core_count;
	req2.thread_number = thread_count;
	printk(BIOS_DEBUG, "IPMI BMC CPU has %u cores, %u threads enabled.\n",
	       req2.core_number, req2.thread_number);

	/* Get processor frequency. */
	req2.processor_freq = 100 * cpu_get_max_ratio();
	printk(BIOS_DEBUG, "IPMI BMC CPU frequency is %u MHz.\n",
	       req2.processor_freq);

	/* Get revision. */
	get_fms(&c, cpuid_eax(1));
	stepping_id = c.x86_mask;
	printk(BIOS_DEBUG, "IPMI BMC CPU stepping id is %x.\n", stepping_id);
	switch (stepping_id) {
	/* TBD */
	case 0x0a:
		req2.revision[0] = 'A';
		req2.revision[1] = '0';
		break;
	default:
		req2.revision[0] = 'X';
		req2.revision[1] = 'X';
	}

	ret = ipmi_kcs_message(dev->path.pnp.port, IPMI_NETFN_OEM_COMMON, 0,
				 IPMI_BMC_SET_PROCESSOR_INFORMATION, (u8 *) &req2,
				 sizeof(req2), (u8 *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d rsp=0x%x)\n",
		       __func__, ret, rsp.completion_code);
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static void ipmi_set_processor_information(struct device *dev)
{
	if (ipmi_set_processor_information_param1(dev))
		printk(BIOS_ERR, "IPMI BMC set param 1 processor info failed\n");

	if (ipmi_set_processor_information_param2(dev))
		printk(BIOS_ERR, "IPMI BMC set param 2 processor info failed\n");
}

static enum cb_err ipmi_set_ppin(struct device *dev)
{
	int ret;
	struct ipmi_rsp rsp;
	struct ppin_req req = {0};

	req.cpu0_lo = xeon_sp_ppin[0].lo;
	req.cpu0_hi = xeon_sp_ppin[0].hi;
	if (CONFIG_MAX_SOCKET > 1) {
		req.cpu1_lo = xeon_sp_ppin[1].lo;
		req.cpu1_hi = xeon_sp_ppin[1].hi;
	}
	ret = ipmi_kcs_message(dev->path.pnp.port, IPMI_NETFN_OEM, 0x0, IPMI_OEM_SET_PPIN,
		(const unsigned char *) &req, sizeof(req), (unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.completion_code);
			return CB_ERR;
	}
	printk(BIOS_DEBUG, "IPMI: %s command success\n", __func__);
	return CB_SUCCESS;
}

static void ipmi_ocp_init(struct device *dev)
{
	/* Add OCP specific IPMI command */
}

static void ipmi_ocp_final(struct device *dev)
{
	/* Add OCP specific IPMI command */

	/* Send processor information */
	ipmi_set_processor_information(dev);
	if (CONFIG(OCP_DMI))
		ipmi_set_ppin(dev);
}

static void ipmi_set_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED))
			continue;

		res->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, res, "");
	}
}

static void ipmi_read_resources(struct device *dev)
{
	struct resource *res = new_resource(dev, 0);
	res->base = dev->path.pnp.port;
	res->size = 2;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations ops = {
	.read_resources   = ipmi_read_resources,
	.set_resources    = ipmi_set_resources,
	.init             = ipmi_ocp_init,
	.final            = ipmi_ocp_final,
};

static void enable_dev(struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PNP)
		printk(BIOS_ERR, "%s: Unsupported device type\n",
		       dev_path(dev));
	else if (dev->path.pnp.port & 1)
		printk(BIOS_ERR, "%s: Base address needs to be aligned to 2\n",
		       dev_path(dev));
	else
		dev->ops = &ops;
}

struct chip_operations drivers_ipmi_ocp_ops = {
	CHIP_NAME("IPMI OCP")
	.enable_dev = enable_dev,
};
