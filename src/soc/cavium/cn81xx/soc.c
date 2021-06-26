/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <console/console.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/sdram.h>
#include <soc/timer.h>
#include <soc/uart.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <libbdk-boot/bdk-boot.h>
#include <soc/ecam0.h>
#include <console/uart.h>
#include <libbdk-hal/bdk-pcie.h>
#include <device/pci.h>
#include <libbdk-hal/bdk-qlm.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-arch/bdk-csrs-bgx.h>
#include <bootmem.h>
#include <soc/bl31_plat_params.h>
#include <cbfs.h>
#include <cbmem.h>
#include <fit.h>

static const char *QLM_BGX_MODE_MAP[BDK_QLM_MODE_LAST] = {
	[BDK_QLM_MODE_SGMII_4X1]    = "sgmii",
	[BDK_QLM_MODE_SGMII_2X1]    = "sgmii",
	[BDK_QLM_MODE_SGMII_1X1]    = "sgmii",
	[BDK_QLM_MODE_XAUI_1X4]     = "xaui",
	[BDK_QLM_MODE_RXAUI_2X2]    = "rxaui",
	[BDK_QLM_MODE_RXAUI_1X2]    = "rxaui",
	[BDK_QLM_MODE_XFI_4X1]      = "xfi",
	[BDK_QLM_MODE_XFI_2X1]      = "xfi",
	[BDK_QLM_MODE_XFI_1X1]      = "xfi",
	[BDK_QLM_MODE_XLAUI_1X4]    = "xlaui",
	[BDK_QLM_MODE_10G_KR_4X1]   = "xfi-10g-kr",
	[BDK_QLM_MODE_10G_KR_2X1]   = "xfi-10g-kr",
	[BDK_QLM_MODE_10G_KR_1X1]   = "xfi-10g-kr",
	[BDK_QLM_MODE_40G_KR4_1X4]  = "xlaui-40g-kr",
	[BDK_QLM_MODE_QSGMII_4X1]   = "qsgmii",
};

static void dt_platform_fixup_phy(struct device_tree_node *node, char *path,
				  int64_t phy_address, bdk_qlm_modes_t qlm_mode)
{
	const char *data = NULL;
	size_t size = 0;
	dt_find_bin_prop(node, "qlm-mode", (const void **)&data, &size);

	if (!data || strncmp(data, path, 6) != 0)
		return; /* No key prefix match. */
	printk(BIOS_INFO, "%s: Node %s = %s\n", __func__, node->name, data);

	if (strlen(path) == strlen(data) && strcmp(data, path) == 0) {
		/* Keep node, remove "qlm-mode" property */
		dt_delete_prop(node, "qlm-mode");
		printk(BIOS_INFO, "%s: Removing qlm-mode on "
		       "node %s\n", __func__, node->name);
		/* Linux only access the Phy via MDIO.
		Remove 'phy-handle' if this option is not available */
		switch (qlm_mode) {
		case BDK_QLM_MODE_SGMII_4X1:
		case BDK_QLM_MODE_SGMII_2X1:
		case BDK_QLM_MODE_SGMII_1X1:
		case BDK_QLM_MODE_QSGMII_4X1:
			if ((phy_address & BDK_IF_PHY_TYPE_MASK) !=
			    BDK_IF_PHY_MDIO) {
				dt_delete_prop(node, "phy-handle");
				printk(BIOS_INFO, "%s: Removing phy-handle on "
				       "node %s\n", __func__, node->name);
			}
			break;
		default:
			break;
		}
	} else {
		printk(BIOS_INFO, "%s: Removing node %s\n", __func__,
		       node->name);
		/* No match, remove node */
		list_remove(&node->list_node);
	}
}

static void dt_iterate_phy(struct device_tree_node *parent,
				  const char *name,
				  char *path,
				  int64_t phy_address,
				  bdk_qlm_modes_t qlm_mode)
{
	struct device_tree_property *prop;

	/* Check if parent itself has the required property value. */
	list_for_each(prop, parent->properties, list_node) {
		if (!strcmp(name, prop->prop.name)) {
			dt_platform_fixup_phy(parent, path, phy_address,
					      qlm_mode);
		}
	}

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		dt_iterate_phy(child, name, path, phy_address, qlm_mode);
	}
}

static void dt_platform_fixup_mac(struct device_tree_node *node)
{
	const char *name = "local-mac-address";
	const u64 *localmac = NULL;
	size_t size = 0;

	dt_find_bin_prop(node, name, (const void **)&localmac, &size);

	if (!localmac)
		return;
	static size_t used_mac;

	/* Extract our MAC address info so we can assign them */
	size_t next_free_mac_address =
		bdk_config_get_int(BDK_CONFIG_MAC_ADDRESS);
	size_t num_free_mac_addresses =
		bdk_config_get_int(BDK_CONFIG_MAC_ADDRESS_NUM);
	size_t num_free_override =
		bdk_config_get_int(BDK_CONFIG_MAC_ADDRESS_NUM_OVERRIDE);
	if (num_free_override != -1)
		num_free_mac_addresses = num_free_override;

	if (size == 6) {
		if (*localmac)
			return;
		if (used_mac < num_free_mac_addresses) {
			u64 genmac = next_free_mac_address + used_mac;
			dt_add_bin_prop(node, name, &genmac, 6);
			used_mac++;
			return;
		}
	}

	printk(BIOS_INFO, "%s: Removing node %s\n", __func__, node->name);
	list_remove(&node->list_node);
}

static void dt_iterate_mac(struct device_tree_node *parent)
{
	struct device_tree_property *prop;
	const char *name = "local-mac-address";

	/* Check if parent itself has the required property value. */
	list_for_each(prop, parent->properties, list_node) {
		if (!strcmp(name, prop->prop.name))
			dt_platform_fixup_mac(parent);
	}

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		dt_iterate_mac(child);
	}
}

/* Do additional device_tree modifications. */
static int dt_platform_fixup(struct device_tree_fixup *fixup,
			      struct device_tree *tree)
{
	struct device_tree_node *dt_node;
	size_t i;

	/* Set the sclk clock rate. */
	dt_node = dt_find_node_by_path(tree, "/soc@0/sclk", NULL, NULL, 0);
	if (dt_node) {
		const u32 freq = thunderx_get_io_clock();
		printk(BIOS_INFO, "%s: Set SCLK to %u Hz\n", __func__, freq);
		dt_add_u32_prop(dt_node, "clock-frequency", freq);
	} else
		printk(BIOS_ERR, "%s: Node not found. OS might miss-behave !\n",
		       __func__);

	/* Set refclkuaa clock rate. */
	dt_node = dt_find_node_by_path(tree, "/soc@0/refclkuaa", NULL,
				       NULL, 0);
	if (dt_node) {
		const u32 freq = uart_platform_refclk();
		printk(BIOS_INFO, "%s: Set REFCLKUAA to %u Hz\n", __func__,
		       freq);
		dt_add_u32_prop(dt_node, "clock-frequency", freq);
	} else
		printk(BIOS_ERR, "%s: Node not found. OS might miss-behave !\n",
		       __func__);

	/* Remove unused UART entries */
	for (i = 0; i < 4; i++) {
		char path[32];
		const uint64_t addr = UAAx_PF_BAR0(i);
		/* Remove the node */
		snprintf(path, sizeof(path), "/soc@0/serial@%llx", addr);
		dt_node = dt_find_node_by_path(tree, path, NULL, NULL, 0);
		if (!dt_node || uart_is_enabled(i)) {
			printk(BIOS_INFO, "%s: ignoring %s\n", __func__, path);
			continue;
		}
		printk(BIOS_INFO, "%s: Removing node %s\n", __func__, path);
		list_remove(&dt_node->list_node);
	}

	/* Remove unused PEM entries */
	for (i = 0; i < 8; i++) {
		char path[32];
		u32 phandle = 0;
		const uint64_t addr = PEM_PEMX_PF_BAR0(i);
		/* Remove the node */
		snprintf(path, sizeof(path), "/soc@0/pci@%llx", addr);
		dt_node = dt_find_node_by_path(tree, path, NULL, NULL, 0);
		if (!dt_node || bdk_pcie_is_running(0, i)) {
			printk(BIOS_INFO, "%s: ignoring %s\n", __func__, path);
			continue;
		}
		/* Store the phandle */
		phandle = dt_node->phandle;
		printk(BIOS_INFO, "%s: Removing node %s\n", __func__, path);
		list_remove(&dt_node->list_node);

		/* Remove phandle to non existing nodes */
		snprintf(path, sizeof(path), "/soc@0/smmu0@%llx", SMMU_PF_BAR0);
		dt_node = dt_find_node_by_path(tree, path, NULL, NULL, 0);
		if (!dt_node) {
			printk(BIOS_ERR, "%s: SMMU entry not found\n",
			       __func__);
			continue;
		}
		const u32 *data = NULL;
		size_t size = 0;
		dt_find_bin_prop(dt_node, "mmu-masters", (const void **)&data,
				 &size);
		if (!size) {
			printk(BIOS_ERR, "%s: mmu-masters entry not found\n",
			       __func__);
			continue;
		}

		u32 *data_cleaned = malloc(size);
		if (!data_cleaned)
			continue;

		size_t n = 0;
		/* Remove phandle from mmu-masters list */
		for (size_t j = 0; j < size / (sizeof(u32) * 2); j++)
			if (be32_to_cpu(data[j * 2]) != phandle) {
				data_cleaned[n * 2] = data[j * 2];
				data_cleaned[n * 2 + 1] = data[j * 2 + 1];
				n++;
			}

		dt_add_bin_prop(dt_node, "mmu-masters", data_cleaned,
				n * sizeof(u32) * 2);

		free(data_cleaned);
	}

	/* Remove QLM mode entries */
	size_t bgx_index, bgx_iface;
	for (bgx_iface = 0; bgx_iface < 4; bgx_iface++) {
		for (bgx_index = 0; bgx_index < 4; bgx_index++) {
			char path[32];
			int qlm = bdk_qlm_get_qlm_num(0, BDK_IF_BGX,
						      bgx_iface, bgx_index);
			bdk_qlm_modes_t qlm_mode = (qlm == -1) ?
				BDK_QLM_MODE_DISABLED :
				bdk_qlm_get_mode(0, qlm);

			/* BGXX_CMRX_RX_DMAC_CTL is used to mark ports as
			 * disabled that would otherwise be enabled */
			if (qlm_mode != BDK_QLM_MODE_DISABLED) {
				BDK_CSR_INIT(rx_dmac_ctl, 0,
					BDK_BGXX_CMRX_RX_DMAC_CTL(bgx_iface,
								  bgx_index));
				if (rx_dmac_ctl.u == 0)
					qlm_mode = BDK_QLM_MODE_DISABLED;
			}

			if (qlm_mode == BDK_QLM_MODE_DISABLED)
				snprintf(path, sizeof(path), "0x0%x%x,disabled",
					 bgx_iface, bgx_index);
			else
				snprintf(path, sizeof(path), "0x0%x%x,%s",
					 bgx_iface, bgx_index,
					 QLM_BGX_MODE_MAP[qlm_mode]);

			int64_t phy_address =
			    bdk_config_get_int(BDK_CONFIG_PHY_ADDRESS, 0,
					       bgx_iface, bgx_index);

			dt_iterate_phy(tree->root, "qlm-mode", path,
				       phy_address, qlm_mode);
		}
	}

	/* Set local MAC address */
	dt_iterate_mac(tree->root);

	return 0;
}

extern u8 _sff8104[];
extern u8 _esff8104[];

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range((uintptr_t)_sff8104,
			  ((uintptr_t)_esff8104 - (uintptr_t)_sff8104),
			  BM_MEM_RESERVED);

	/* Scratchpad for ATF SATA quirks */
	bootmem_add_range((sdram_size_mb() - 1) * MiB, 1 * MiB,
			  BM_MEM_RESERVED);
}

static void soc_read_resources(struct device *dev)
{
	// HACK: Don't advertise bootblock romstage CAR region, it's broken...
	ram_from_to(dev, 0, 2 * MiB, sdram_size_mb() * (uint64_t)MiB);
}

static void soc_init_atf(void)
{
	static struct bl31_fdt_param fdt_param = {
		.h = { .type = PARAM_FDT, },
	};

	size_t size = 0;

	void *ptr = cbfs_map("sff8104-linux.dtb", &size);
	if (ptr)
		memcpy(_sff8104, ptr, size);
	/* Point to devicetree in secure memory */
	fdt_param.fdt_ptr = (uintptr_t)_sff8104;

	cn81xx_register_bl31_param(&fdt_param.h);

	static struct bl31_u64_param cbtable_param = {
		.h = { .type = PARAM_COREBOOT_TABLE, },
	};
	/* Point to coreboot tables */
	cbtable_param.value = (uint64_t)cbmem_find(CBMEM_ID_CBTABLE);
	if (cbtable_param.value)
		cn81xx_register_bl31_param(&cbtable_param.h);
}

static void soc_init(struct device *dev)
{
	/* Init ECAM, MDIO, PEM, PHY, QLM ... */
	bdk_boot();

	if (CONFIG(PAYLOAD_FIT_SUPPORT)) {
		struct device_tree_fixup *dt_fixup;

		dt_fixup = malloc(sizeof(*dt_fixup));
		if (dt_fixup) {
			dt_fixup->fixup = dt_platform_fixup;
			list_insert_after(&dt_fixup->list_node,
					  &device_tree_fixups);
		}
	}

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		soc_init_atf();
}

static void soc_final(struct device *dev)
{
	watchdog_disable(0);
}

static struct device_operations soc_ops = {
	.read_resources   = soc_read_resources,
	.set_resources    = noop_set_resources,
	.init             = soc_init,
	.final            = soc_final,
};

static void enable_soc_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN &&
		dev->path.domain.domain == 0) {
		dev->ops = &pci_domain_ops_ecam0;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	}
}

struct chip_operations soc_cavium_cn81xx_ops = {
	CHIP_NAME("SOC Cavium CN81XX")
	.enable_dev = enable_soc_dev,
};
