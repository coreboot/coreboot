/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>

static unsigned int ext_cap_id(unsigned int cap)
{
	return cap & 0xffff;
}

static unsigned int ext_cap_next_offset(unsigned int cap)
{
	return cap >> 20 & 0xffc;
}

static unsigned int find_ext_cap_offset(const struct device *dev, unsigned int cap_id,
					unsigned int offset)
{
	unsigned int this_cap_offset = offset;

	while (this_cap_offset >= PCIE_EXT_CAP_OFFSET) {
		const unsigned int this_cap = pci_read_config32(dev, this_cap_offset);

		/* Bail out when this request is unsupported */
		if (this_cap == 0xffffffff)
			break;

		if (ext_cap_id(this_cap) == cap_id)
			return this_cap_offset;

		this_cap_offset = ext_cap_next_offset(this_cap);
	}

	return 0;
}

/*
 * Search for an extended capability with the ID `cap`.
 *
 * Returns the offset of the first matching extended
 * capability if found, or 0 otherwise.
 *
 * A new search is started with `offset == 0`.
 * To continue a search, the prior return value
 * should be passed as `offset`.
 */
unsigned int pciexp_find_extended_cap(const struct device *dev, unsigned int cap,
				      unsigned int offset)
{
	unsigned int next_cap_offset;

	if (offset)
		next_cap_offset = ext_cap_next_offset(pci_read_config32(dev, offset));
	else
		next_cap_offset = PCIE_EXT_CAP_OFFSET;

	return find_ext_cap_offset(dev, cap, next_cap_offset);
}

/*
 * Search for a vendor-specific extended capability,
 * with the vendor-specific ID `cap`.
 *
 * Returns the offset of the vendor-specific header,
 * i.e. the offset of the extended capability + 4,
 * or 0 if none is found.
 *
 * A new search is started with `offset == 0`.
 * To continue a search, the prior return value
 * should be passed as `offset`.
 */
unsigned int pciexp_find_ext_vendor_cap(const struct device *dev, unsigned int cap,
					unsigned int offset)
{
	/* Reconstruct capability offset from vendor-specific header offset. */
	if (offset >= 4)
		offset -= 4;

	for (;;) {
		offset = pciexp_find_extended_cap(dev, PCI_EXT_CAP_ID_VNDR, offset);
		if (!offset)
			return 0;

		const unsigned int vndr_cap = pci_read_config32(dev, offset + 4);
		if ((vndr_cap & 0xffff) == cap)
			return offset + 4;
	}
}

/*
 * Re-train a PCIe link
 */
#define PCIE_TRAIN_RETRY 10000
static int pciexp_retrain_link(struct device *dev, unsigned int cap)
{
	unsigned int try;
	u16 lnk;

	/*
	 * Implementation note (page 633) in PCIe Specification 3.0 suggests
	 * polling the Link Training bit in the Link Status register until the
	 * value returned is 0 before setting the Retrain Link bit to 1.
	 * This is meant to avoid a race condition when using the
	 * Retrain Link mechanism.
	 */
	for (try = PCIE_TRAIN_RETRY; try > 0; try--) {
		lnk = pci_read_config16(dev, cap + PCI_EXP_LNKSTA);
		if (!(lnk & PCI_EXP_LNKSTA_LT))
			break;
		udelay(100);
	}
	if (try == 0) {
		printk(BIOS_ERR, "%s: Link Retrain timeout\n", dev_path(dev));
		return -1;
	}

	/* Start link retraining */
	lnk = pci_read_config16(dev, cap + PCI_EXP_LNKCTL);
	lnk |= PCI_EXP_LNKCTL_RL;
	pci_write_config16(dev, cap + PCI_EXP_LNKCTL, lnk);

	/* Wait for training to complete */
	for (try = PCIE_TRAIN_RETRY; try > 0; try--) {
		lnk = pci_read_config16(dev, cap + PCI_EXP_LNKSTA);
		if (!(lnk & PCI_EXP_LNKSTA_LT))
			return 0;
		udelay(100);
	}

	printk(BIOS_ERR, "%s: Link Retrain timeout\n", dev_path(dev));
	return -1;
}

/*
 * Check the Slot Clock Configuration for root port and endpoint
 * and enable Common Clock Configuration if possible.  If CCC is
 * enabled the link must be retrained.
 */
static void pciexp_enable_common_clock(struct device *root, unsigned int root_cap,
				       struct device *endp, unsigned int endp_cap)
{
	u16 root_scc, endp_scc, lnkctl;

	/* Get Slot Clock Configuration for root port */
	root_scc = pci_read_config16(root, root_cap + PCI_EXP_LNKSTA);
	root_scc &= PCI_EXP_LNKSTA_SLC;

	/* Get Slot Clock Configuration for endpoint */
	endp_scc = pci_read_config16(endp, endp_cap + PCI_EXP_LNKSTA);
	endp_scc &= PCI_EXP_LNKSTA_SLC;

	/* Enable Common Clock Configuration and retrain */
	if (root_scc && endp_scc) {
		printk(BIOS_INFO, "Enabling Common Clock Configuration\n");

		/* Set in endpoint */
		lnkctl = pci_read_config16(endp, endp_cap + PCI_EXP_LNKCTL);
		lnkctl |= PCI_EXP_LNKCTL_CCC;
		pci_write_config16(endp, endp_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Set in root port */
		lnkctl = pci_read_config16(root, root_cap + PCI_EXP_LNKCTL);
		lnkctl |= PCI_EXP_LNKCTL_CCC;
		pci_write_config16(root, root_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Retrain link if CCC was enabled */
		pciexp_retrain_link(root, root_cap);
	}
}

static void pciexp_enable_clock_power_pm(struct device *endp, unsigned int endp_cap)
{
	/* check if per port clk req is supported in device */
	u32 endp_ca;
	u16 lnkctl;
	endp_ca = pci_read_config32(endp, endp_cap + PCI_EXP_LNKCAP);
	if ((endp_ca & PCI_EXP_CLK_PM) == 0) {
		printk(BIOS_INFO, "PCIE CLK PM is not supported by endpoint\n");
		return;
	}
	lnkctl = pci_read_config16(endp, endp_cap + PCI_EXP_LNKCTL);
	lnkctl = lnkctl | PCI_EXP_EN_CLK_PM;
	pci_write_config16(endp, endp_cap + PCI_EXP_LNKCTL, lnkctl);
}

static bool _pciexp_ltr_supported(struct device *dev, unsigned int cap)
{
	return pci_read_config16(dev, cap + PCI_EXP_DEVCAP2) & PCI_EXP_DEVCAP2_LTR;
}

static bool _pciexp_ltr_enabled(struct device *dev, unsigned int cap)
{
	return pci_read_config16(dev, cap + PCI_EXP_DEVCTL2) & PCI_EXP_DEV2_LTR;
}

static bool _pciexp_enable_ltr(struct device *parent, unsigned int parent_cap,
			       struct device *dev, unsigned int cap)
{
	if (!_pciexp_ltr_supported(dev, cap)) {
		printk(BIOS_DEBUG, "%s: No LTR support\n", dev_path(dev));
		return false;
	}

	if (_pciexp_ltr_enabled(dev, cap))
		return true;

	if (parent &&
	    (!_pciexp_ltr_supported(parent, parent_cap) ||
	     !_pciexp_ltr_enabled(parent, parent_cap)))
		return false;

	pci_or_config16(dev, cap + PCI_EXP_DEVCTL2, PCI_EXP_DEV2_LTR);
	printk(BIOS_INFO, "%s: Enabled LTR\n", dev_path(dev));
	return true;
}

static void pciexp_enable_ltr(struct device *dev)
{
	const unsigned int cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap)
		return;

	/*
	 * If we have get_ltr_max_latencies(), treat `dev` as the root.
	 * If not, let _pciexp_enable_ltr() query the parent's state.
	 */
	struct device *parent = NULL;
	unsigned int parent_cap = 0;
	if (!dev->ops->ops_pci || !dev->ops->ops_pci->get_ltr_max_latencies) {
		parent = dev->bus->dev;
		if (parent->path.type != DEVICE_PATH_PCI)
			return;
		parent_cap = pci_find_capability(parent, PCI_CAP_ID_PCIE);
		if (!parent_cap)
			return;
	}

	(void)_pciexp_enable_ltr(parent, parent_cap, dev, cap);
}

bool pciexp_get_ltr_max_latencies(struct device *dev, u16 *max_snoop, u16 *max_nosnoop)
{
	/* Walk the hierarchy up to find get_ltr_max_latencies(). */
	do {
		if (dev->ops->ops_pci && dev->ops->ops_pci->get_ltr_max_latencies)
			break;
		if (dev->bus->dev == dev || dev->bus->dev->path.type != DEVICE_PATH_PCI)
			return false;
		dev = dev->bus->dev;
	} while (true);

	dev->ops->ops_pci->get_ltr_max_latencies(max_snoop, max_nosnoop);
	return true;
}

static void pciexp_configure_ltr(struct device *parent, unsigned int parent_cap,
				 struct device *dev, unsigned int cap)
{
	if (!_pciexp_enable_ltr(parent, parent_cap, dev, cap))
		return;

	const unsigned int ltr_cap = pciexp_find_extended_cap(dev, PCIE_EXT_CAP_LTR_ID, 0);
	if (!ltr_cap)
		return;

	u16 max_snoop, max_nosnoop;
	if (!pciexp_get_ltr_max_latencies(dev, &max_snoop, &max_nosnoop))
		return;

	pci_write_config16(dev, ltr_cap + PCI_LTR_MAX_SNOOP, max_snoop);
	pci_write_config16(dev, ltr_cap + PCI_LTR_MAX_NOSNOOP, max_nosnoop);
	printk(BIOS_INFO, "%s: Programmed LTR max latencies\n", dev_path(dev));
}

static unsigned char pciexp_L1_substate_cal(struct device *dev, unsigned int endp_cap,
	unsigned int *data)
{
	unsigned char mult[4] = {2, 10, 100, 0};

	unsigned int L1SubStateSupport = *data & 0xf;
	unsigned int comm_mode_rst_time = (*data >> 8) & 0xff;
	unsigned int power_on_scale = (*data >> 16) & 0x3;
	unsigned int power_on_value = (*data >> 19) & 0x1f;

	unsigned int endp_data = pci_read_config32(dev, endp_cap + 4);
	unsigned int endp_L1SubStateSupport = endp_data & 0xf;
	unsigned int endp_comm_mode_restore_time = (endp_data >> 8) & 0xff;
	unsigned int endp_power_on_scale = (endp_data >> 16) & 0x3;
	unsigned int endp_power_on_value = (endp_data >> 19) & 0x1f;

	L1SubStateSupport &= endp_L1SubStateSupport;

	if (L1SubStateSupport == 0)
		return 0;

	if (power_on_value * mult[power_on_scale] <
		endp_power_on_value * mult[endp_power_on_scale]) {
		power_on_value = endp_power_on_value;
		power_on_scale = endp_power_on_scale;
	}
	if (comm_mode_rst_time < endp_comm_mode_restore_time)
		comm_mode_rst_time = endp_comm_mode_restore_time;

	*data = (comm_mode_rst_time << 8) | (power_on_scale << 16)
		| (power_on_value << 19) | L1SubStateSupport;

	return 1;
}

static void pciexp_L1_substate_commit(struct device *root, struct device *dev,
	unsigned int root_cap, unsigned int end_cap)
{
	struct device *dev_t;
	unsigned char L1_ss_ok;
	unsigned int rp_L1_support = pci_read_config32(root, root_cap + 4);
	unsigned int L1SubStateSupport;
	unsigned int comm_mode_rst_time;
	unsigned int power_on_scale;
	unsigned int endp_power_on_value;

	for (dev_t = dev; dev_t; dev_t = dev_t->sibling) {
		/*
		 * rp_L1_support is init'd above from root port.
		 * it needs coordination with endpoints to reach in common.
		 * if certain endpoint doesn't support L1 Sub-State, abort
		 * this feature enabling.
		 */
		L1_ss_ok = pciexp_L1_substate_cal(dev_t, end_cap,
						&rp_L1_support);
		if (!L1_ss_ok)
			return;
	}

	L1SubStateSupport = rp_L1_support & 0xf;
	comm_mode_rst_time = (rp_L1_support >> 8) & 0xff;
	power_on_scale = (rp_L1_support >> 16) & 0x3;
	endp_power_on_value = (rp_L1_support >> 19) & 0x1f;

	printk(BIOS_INFO, "L1 Sub-State supported from root port %d\n",
		root->path.pci.devfn >> 3);
	printk(BIOS_INFO, "L1 Sub-State Support = 0x%x\n", L1SubStateSupport);
	printk(BIOS_INFO, "CommonModeRestoreTime = 0x%x\n", comm_mode_rst_time);
	printk(BIOS_INFO, "Power On Value = 0x%x, Power On Scale = 0x%x\n",
		endp_power_on_value, power_on_scale);

	pci_update_config32(root, root_cap + 0x08, ~0xff00,
		(comm_mode_rst_time << 8));

	pci_update_config32(root, root_cap + 0x0c, 0xffffff04,
		(endp_power_on_value << 3) | (power_on_scale));

	/* TODO: 0xa0, 2 are values that work on some chipsets but really
	 * should be determined dynamically by looking at downstream devices.
	 */
	pci_update_config32(root, root_cap + 0x08,
		~(ASPM_LTR_L12_THRESHOLD_VALUE_MASK |
			ASPM_LTR_L12_THRESHOLD_SCALE_MASK),
		(0xa0 << ASPM_LTR_L12_THRESHOLD_VALUE_OFFSET) |
		(2 << ASPM_LTR_L12_THRESHOLD_SCALE_OFFSET));

	pci_update_config32(root, root_cap + 0x08, ~0x1f,
		L1SubStateSupport);

	for (dev_t = dev; dev_t; dev_t = dev_t->sibling) {
		pci_update_config32(dev_t, end_cap + 0x0c, 0xffffff04,
			(endp_power_on_value << 3) | (power_on_scale));

		pci_update_config32(dev_t, end_cap + 0x08,
			~(ASPM_LTR_L12_THRESHOLD_VALUE_MASK |
				ASPM_LTR_L12_THRESHOLD_SCALE_MASK),
			(0xa0 << ASPM_LTR_L12_THRESHOLD_VALUE_OFFSET) |
			(2 << ASPM_LTR_L12_THRESHOLD_SCALE_OFFSET));

		pci_update_config32(dev_t, end_cap + 0x08, ~0x1f,
			L1SubStateSupport);
	}
}

static void pciexp_config_L1_sub_state(struct device *root, struct device *dev)
{
	unsigned int root_cap, end_cap;

	/* Do it for function 0 only */
	if (dev->path.pci.devfn & 0x7)
		return;

	root_cap = pciexp_find_extended_cap(root, PCIE_EXT_CAP_L1SS_ID, 0);
	if (!root_cap)
		return;

	end_cap = pciexp_find_extended_cap(dev, PCIE_EXT_CAP_L1SS_ID, 0);
	if (!end_cap) {
		if (dev->vendor != PCI_VID_INTEL)
			return;

		end_cap = pciexp_find_ext_vendor_cap(dev, 0xcafe, 0);
		if (!end_cap)
			return;
	}

	pciexp_L1_substate_commit(root, dev, root_cap, end_cap);
}

/*
 * Determine the ASPM L0s or L1 exit latency for a link
 * by checking both root port and endpoint and returning
 * the highest latency value.
 */
static int pciexp_aspm_latency(struct device *root, unsigned int root_cap,
			       struct device *endp, unsigned int endp_cap,
			       enum aspm_type type)
{
	int root_lat = 0, endp_lat = 0;
	u32 root_lnkcap, endp_lnkcap;

	root_lnkcap = pci_read_config32(root, root_cap + PCI_EXP_LNKCAP);
	endp_lnkcap = pci_read_config32(endp, endp_cap + PCI_EXP_LNKCAP);

	/* Make sure the link supports this ASPM type by checking
	 * capability bits 11:10 with aspm_type offset by 1 */
	if (!(root_lnkcap & (1 << (type + 9))) ||
	    !(endp_lnkcap & (1 << (type + 9))))
		return -1;

	/* Find the one with higher latency */
	switch (type) {
	case PCIE_ASPM_L0S:
		root_lat = (root_lnkcap & PCI_EXP_LNKCAP_L0SEL) >> 12;
		endp_lat = (endp_lnkcap & PCI_EXP_LNKCAP_L0SEL) >> 12;
		break;
	case PCIE_ASPM_L1:
		root_lat = (root_lnkcap & PCI_EXP_LNKCAP_L1EL) >> 15;
		endp_lat = (endp_lnkcap & PCI_EXP_LNKCAP_L1EL) >> 15;
		break;
	default:
		return -1;
	}

	return (endp_lat > root_lat) ? endp_lat : root_lat;
}

/*
 * Enable ASPM on PCIe root port and endpoint.
 */
static void pciexp_enable_aspm(struct device *root, unsigned int root_cap,
					 struct device *endp, unsigned int endp_cap)
{
	const char *aspm_type_str[] = { "None", "L0s", "L1", "L0s and L1" };
	enum aspm_type apmc = PCIE_ASPM_NONE;
	int exit_latency, ok_latency;
	u16 lnkctl;
	u32 devcap;

	if (endp->disable_pcie_aspm)
		return;

	/* Get endpoint device capabilities for acceptable limits */
	devcap = pci_read_config32(endp, endp_cap + PCI_EXP_DEVCAP);

	/* Enable L0s if it is within endpoint acceptable limit */
	ok_latency = (devcap & PCI_EXP_DEVCAP_L0S) >> 6;
	exit_latency = pciexp_aspm_latency(root, root_cap, endp, endp_cap,
					   PCIE_ASPM_L0S);
	if (exit_latency >= 0 && exit_latency <= ok_latency)
		apmc |= PCIE_ASPM_L0S;

	/* Enable L1 if it is within endpoint acceptable limit */
	ok_latency = (devcap & PCI_EXP_DEVCAP_L1) >> 9;
	exit_latency = pciexp_aspm_latency(root, root_cap, endp, endp_cap,
					   PCIE_ASPM_L1);
	if (exit_latency >= 0 && exit_latency <= ok_latency)
		apmc |= PCIE_ASPM_L1;

	if (apmc != PCIE_ASPM_NONE) {
		/* Set APMC in root port first */
		lnkctl = pci_read_config16(root, root_cap + PCI_EXP_LNKCTL);
		lnkctl |= apmc;
		pci_write_config16(root, root_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Set APMC in endpoint device next */
		lnkctl = pci_read_config16(endp, endp_cap + PCI_EXP_LNKCTL);
		lnkctl |= apmc;
		pci_write_config16(endp, endp_cap + PCI_EXP_LNKCTL, lnkctl);
	}

	printk(BIOS_INFO, "ASPM: Enabled %s\n", aspm_type_str[apmc]);
}

/*
 * Set max payload size of endpoint in accordance with max payload size of root port.
 */
static void pciexp_set_max_payload_size(struct device *root, unsigned int root_cap,
					struct device *endp, unsigned int endp_cap)
{
	unsigned int endp_max_payload, root_max_payload, max_payload;
	u16 endp_devctl, root_devctl;
	u32 endp_devcap, root_devcap;

	/* Get max payload size supported by endpoint */
	endp_devcap = pci_read_config32(endp, endp_cap + PCI_EXP_DEVCAP);
	endp_max_payload = endp_devcap & PCI_EXP_DEVCAP_PAYLOAD;

	/* Get max payload size supported by root port */
	root_devcap = pci_read_config32(root, root_cap + PCI_EXP_DEVCAP);
	root_max_payload = root_devcap & PCI_EXP_DEVCAP_PAYLOAD;

	/* Set max payload to smaller of the reported device capability. */
	max_payload = MIN(endp_max_payload, root_max_payload);
	if (max_payload > 5) {
		/* Values 6 and 7 are reserved in PCIe 3.0 specs. */
		printk(BIOS_ERR, "PCIe: Max_Payload_Size field restricted from %d to 5\n",
		       max_payload);
		max_payload = 5;
	}

	endp_devctl = pci_read_config16(endp, endp_cap + PCI_EXP_DEVCTL);
	endp_devctl &= ~PCI_EXP_DEVCTL_PAYLOAD;
	endp_devctl |= max_payload << 5;
	pci_write_config16(endp, endp_cap + PCI_EXP_DEVCTL, endp_devctl);

	root_devctl = pci_read_config16(root, root_cap + PCI_EXP_DEVCTL);
	root_devctl &= ~PCI_EXP_DEVCTL_PAYLOAD;
	root_devctl |= max_payload << 5;
	pci_write_config16(root, root_cap + PCI_EXP_DEVCTL, root_devctl);

	printk(BIOS_INFO, "PCIe: Max_Payload_Size adjusted to %d\n", (1 << (max_payload + 7)));
}

/*
 * Clear Lane Error State at the end of PCIe link training.
 * Lane error status is cleared if PCIEXP_LANE_ERR_STAT_CLEAR is set.
 * Lane error is normal during link training, so we need to clear it.
 * At this moment, link has been used, but for a very short duration.
 */
static void clear_lane_error_status(struct device *dev)
{
	u32 reg32;
	u16 pos;

	pos = pciexp_find_extended_cap(dev, PCI_EXP_SEC_CAP_ID, 0);
	if (pos == 0)
		return;

	reg32 = pci_read_config32(dev, pos + PCI_EXP_SEC_LANE_ERR_STATUS);
	if (reg32 == 0)
		return;

	printk(BIOS_DEBUG, "%s: Clear Lane Error Status.\n", dev_path(dev));
	printk(BIOS_DEBUG, "LaneErrStat:0x%x\n", reg32);
	pci_write_config32(dev, pos + PCI_EXP_SEC_LANE_ERR_STATUS, reg32);
}

static void pciexp_tune_dev(struct device *dev)
{
	struct device *root = dev->bus->dev;
	unsigned int root_cap, cap;

	cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap)
		return;

	root_cap = pci_find_capability(root, PCI_CAP_ID_PCIE);
	if (!root_cap)
		return;

	/* Check for and enable Common Clock */
	if (CONFIG(PCIEXP_COMMON_CLOCK))
		pciexp_enable_common_clock(root, root_cap, dev, cap);

	/* Check if per port CLK req is supported by endpoint*/
	if (CONFIG(PCIEXP_CLK_PM))
		pciexp_enable_clock_power_pm(dev, cap);

	/* Enable L1 Sub-State when both root port and endpoint support */
	if (CONFIG(PCIEXP_L1_SUB_STATE))
		pciexp_config_L1_sub_state(root, dev);

	/* Check for and enable ASPM */
	if (CONFIG(PCIEXP_ASPM))
		pciexp_enable_aspm(root, root_cap, dev, cap);

	/* Clear PCIe Lane Error Status */
	if (CONFIG(PCIEXP_LANE_ERR_STAT_CLEAR))
		clear_lane_error_status(root);

	/* Adjust Max_Payload_Size of link ends. */
	pciexp_set_max_payload_size(root, root_cap, dev, cap);

	pciexp_configure_ltr(root, root_cap, dev, cap);
}

void pciexp_scan_bus(struct bus *bus, unsigned int min_devfn,
			     unsigned int max_devfn)
{
	struct device *child;

	pciexp_enable_ltr(bus->dev);

	pci_scan_bus(bus, min_devfn, max_devfn);

	for (child = bus->children; child; child = child->sibling) {
		if (child->path.type != DEVICE_PATH_PCI)
			continue;
		if ((child->path.pci.devfn < min_devfn) ||
		    (child->path.pci.devfn > max_devfn)) {
			continue;
		}
		pciexp_tune_dev(child);
	}
}

void pciexp_scan_bridge(struct device *dev)
{
	do_pci_scan_bridge(dev, pciexp_scan_bus);
}

/** Default device operations for PCI Express bridges */
static struct pci_operations pciexp_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_pciexp_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pciexp_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pciexp_bus_ops_pci,
};

static void pciexp_hotplug_dummy_read_resources(struct device *dev)
{
	struct resource *resource;

	/* Add extra memory space */
	resource = new_resource(dev, 0x10);
	resource->size = CONFIG_PCIEXP_HOTPLUG_MEM;
	resource->align = 12;
	resource->gran = 12;
	resource->limit = 0xffffffff;
	resource->flags |= IORESOURCE_MEM;

	/* Add extra prefetchable memory space */
	resource = new_resource(dev, 0x14);
	resource->size = CONFIG_PCIEXP_HOTPLUG_PREFETCH_MEM;
	resource->align = 12;
	resource->gran = 12;
	resource->limit = 0xffffffffffffffff;
	resource->flags |= IORESOURCE_MEM | IORESOURCE_PREFETCH;

	/* Set resource flag requesting allocation above 4G boundary. */
	if (CONFIG(PCIEXP_HOTPLUG_PREFETCH_MEM_ABOVE_4G))
		resource->flags |= IORESOURCE_ABOVE_4G;

	/* Add extra I/O space */
	resource = new_resource(dev, 0x18);
	resource->size = CONFIG_PCIEXP_HOTPLUG_IO;
	resource->align = 12;
	resource->gran = 12;
	resource->limit = 0xffff;
	resource->flags |= IORESOURCE_IO;
}

static struct device_operations pciexp_hotplug_dummy_ops = {
	.read_resources   = pciexp_hotplug_dummy_read_resources,
	.set_resources    = noop_set_resources,
};

void pciexp_hotplug_scan_bridge(struct device *dev)
{
	dev->hotplug_port = 1;
	dev->hotplug_buses = CONFIG_PCIEXP_HOTPLUG_BUSES;

	/* Normal PCIe Scan */
	pciexp_scan_bridge(dev);

	/* Add dummy slot to preserve resources, must happen after bus scan */
	struct device *dummy;
	struct device_path dummy_path = { .type = DEVICE_PATH_NONE };
	dummy = alloc_dev(dev->link_list, &dummy_path);
	dummy->ops = &pciexp_hotplug_dummy_ops;
}

struct device_operations default_pciexp_hotplug_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pciexp_hotplug_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pciexp_bus_ops_pci,
};
