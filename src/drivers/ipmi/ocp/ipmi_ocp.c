/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Place in devicetree.cb:
 *
 * chip drivers/ipmi/ocp # OCP specific IPMI porting
	device pnp ca2.1 on end
 * end
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ocp/dmi/ocp_dmi.h>
#include <types.h>

#include "ipmi_ocp.h"

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
	ret = ipmi_message(dev->path.pnp.port, IPMI_NETFN_OEM, 0x0, IPMI_OEM_SET_PPIN,
			   (const unsigned char *) &req, sizeof(req),
			   (unsigned char *) &rsp, sizeof(rsp));

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
