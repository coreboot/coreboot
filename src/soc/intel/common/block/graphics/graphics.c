/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootmode.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <drivers/intel/gma/opregion.h>
#include <intelblocks/cfg.h>
#include <intelblocks/graphics.h>
#include <fsp/graphics.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <types.h>

/*
 * This GUID is used to identify memory resources related to the memory bandwidth
 * compression functionality for Intel Integrated Graphics Devices (IGD).
 */
static const uint8_t memory_compression_guid[16] = {
	0x79, 0x15, 0x9f, 0x8a, 0x72, 0xea, 0xb5, 0x4b,
	0x90, 0x69, 0x54, 0x9a, 0x1b, 0xf7, 0xc4, 0xfd
};

/* Display Type:
*  0 - only internal display aka eDP attached
*  1 - only external display aka HDMI/USB-C attached
*  2 - dual display aka both internal and external display attached
*/
enum display_type {
	INTERNAL_DISPLAY_ONLY,
	EXTERNAL_DISPLAY_ONLY,
	DUAL_DISPLAY,
};

#define GFX_MBUS_CTL		0x4438C
#define GFX_MBUS_SEL(x)		(GFX_MBUS_CTL + (x))
#define GFX_MBUS_JOIN		BIT(31)
#define GFX_MBUS_HASHING_MODE	BIT(30)
#define GFX_MBUS_JOIN_PIPE_SEL	(BIT(28) | BIT(27) | BIT(26))

/* SoC Overrides */
__weak void graphics_soc_panel_init(struct device *dev)
{
	/*
	 * User needs to implement SoC override in case wishes
	 * to perform certain specific graphics initialization
	 */
}

__weak const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device)
{
	return NULL;
}

static uint32_t graphics_get_ddi_func_ctrl(unsigned long reg)
{
	uint32_t ddi_func_ctrl = graphics_gtt_read(reg);
	ddi_func_ctrl &= TRANS_DDI_PORT_MASK;

	return ddi_func_ctrl;
}

/*
 * Transcoders contain the timing generators for eDP, DP, and HDMI interfaces.
 * Intel transcoders are based on Quick Sync Video, which offloads video
 * encoding and decoding tasks from the CPU to the GPU.
 *
 * On Intel silicon, there are four display pipes (DDI-A to DDI-D) that support
 * blending, color adjustments, scaling, and dithering.
 *
 * From the display block diagram perspective, the front end of the display
 * contains the pipes. The pipes connect to the transcoder. The transcoder
 * (except for wireless) connects to the DDIs to drive the IO/PHY.
 *
 * This logic checks if the DDI-A port is attached to the transcoder and
 * enabled (bit 27). Traditionally, the on-board display (eDP) is attached to DDI-A.
 * If the above conditions is met, then the on-board display is present and enabled.
 *
 * On platforms without an on-board display (i.e., value at bits 27-30 is between 2-9),
 * meaning that DDI-A (eDP) is not enabled.
 *
 * Additionally, if bits 27-30 are all set to 0, this means that no DDI ports
 * are enabled, and there is no display.
 *
 * Consider external display is present and enabled, if eDP/DDI-A is not enabled
 * and transcoder is attached to any DDI port (bits 27-30 are not zero).
 */
static enum display_type get_external_display_status(void)
{
	/* Read the transcoder register for DDI-A (eDP) */
	uint32_t ddi_a_func_ctrl = graphics_get_ddi_func_ctrl(TRANS_DDI_FUNC_CTL_A);
	/* Read the transcoder register for DDI-B (HDMI) */
	uint32_t ddi_b_func_ctrl = graphics_get_ddi_func_ctrl(TRANS_DDI_FUNC_CTL_B);

	/*
	 * Check if transcoder is none or connected to DDI-A port (aka eDP).
	 * Report no external display in both cases.
	 */
	if (ddi_a_func_ctrl == TRANS_DDI_PORT_NONE) {
		return INTERNAL_DISPLAY_ONLY;
	} else {
		if (ddi_a_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_A) &&
			 (ddi_b_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_B)
#if CONFIG(INTEL_GMA_VERSION_2)
			 || ddi_b_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_USB_C1)
			 || ddi_b_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_USB_C2)
			 || ddi_b_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_USB_C3)
			 || ddi_b_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_USB_C4)
#endif
		)) {
			/*
			 * Dual display detected: both DDI-A(eDP) and
			 * DDI-B(HDMI) pipes are active
			 */
			return DUAL_DISPLAY;
		} else {
			if (ddi_a_func_ctrl == TRANS_DDI_SELECT_PORT(PORT_A))
				return INTERNAL_DISPLAY_ONLY;
			else
				return EXTERNAL_DISPLAY_ONLY;
		}
	}
}

/* Check and report if an external display is attached */
int fsp_soc_report_external_display(void)
{
	return graphics_get_framebuffer_address() && get_external_display_status();
}

static void configure_ddi_a_bifurcation(void)
{
	u32 ddi_buf_ctl = graphics_gtt_read(DDI_BUF_CTL_A);
	/* Only program if the buffer is not enabled yet. */
	if (ddi_buf_ctl & DDI_BUF_CTL_ENABLE)
		return;

	if (CONFIG(SOC_INTEL_GFX_ENABLE_DDI_E_BIFURCATION))
		ddi_buf_ctl &= ~DDI_A_4_LANES;
	else
		ddi_buf_ctl |= DDI_A_4_LANES;

	graphics_gtt_write(DDI_BUF_CTL_A, ddi_buf_ctl);
}

static void gma_init(struct device *const dev)
{
	intel_gma_init_igd_opregion();

	/* SoC specific panel init/configuration.
	   If FSP has already run/configured the IGD, we can assume the
	   panel/backlight control have already been set up sufficiently
	   and that we shouldn't attempt to reconfigure things. */
	if (!CONFIG(RUN_FSP_GOP))
		graphics_soc_panel_init(dev);

	if (CONFIG(SOC_INTEL_GFX_HAVE_DDI_A_BIFURCATION) && !acpi_is_wakeup_s3())
		configure_ddi_a_bifurcation();

	/*
	 * GFX PEIM module inside FSP binary is taking care of graphics
	 * initialization based on RUN_FSP_GOP Kconfig option and input
	 * VBT file. Need to report the framebuffer info after PCI enumeration.
	 *
	 * In case of non-FSP solution, SoC need to select another
	 * Kconfig to perform GFX initialization.
	 */
	if (CONFIG(RUN_FSP_GOP) && display_init_required()) {
		const struct soc_intel_common_config *config = chip_get_common_soc_structure();
		fsp_report_framebuffer_info(graphics_get_framebuffer_address(),
					    config->panel_orientation);
		return;
	}

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		if (!acpi_is_wakeup_s3() && display_init_required()) {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
			gfx_set_init_done(lightup_ok);
		}
	} else {
		/* Initialize PCI device, load/execute BIOS Option ROM */
		pci_dev_init(dev);
	}
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct i915_gpu_controller_info *gfx = intel_igd_get_controller_info(device);

	if (gfx)
		drivers_intel_gma_displays_ssdt_generate(gfx);
}

static int is_graphics_disabled(struct device *dev)
{
	/* Check if Graphics PCI device is disabled */
	if (!dev || !dev->enabled)
		return 1;

	return 0;
}

static uintptr_t graphics_get_bar(struct device *dev, unsigned long index)
{
	struct resource *gm_res;

	gm_res = probe_resource(dev, index);
	if (!gm_res)
		return 0;

	return gm_res->base;
}

uintptr_t graphics_get_framebuffer_address(void)
{
	uintptr_t memory_base;
	struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);

	if (is_graphics_disabled(dev))
		return 0;

	memory_base = graphics_get_bar(dev, PCI_BASE_ADDRESS_2);
	if (!memory_base)
		die_with_post_code(POSTCODE_HW_INIT_FAILURE,
				   "Graphic memory bar2 is not programmed!");

	memory_base += CONFIG_SOC_INTEL_GFX_FRAMEBUFFER_OFFSET;

	return memory_base;
}

static uintptr_t graphics_get_gtt_base(void)
{
	static uintptr_t gtt_base;
	struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);

	if (is_graphics_disabled(dev))
		die("IGD is disabled!");
	/*
	 * GFX PCI config space offset 0x10 know as Graphics
	 * Translation Table Memory Mapped Range Address
	 * (GTTMMADR)
	 */
	if (!gtt_base) {
		gtt_base = graphics_get_bar(dev, PCI_BASE_ADDRESS_0);
		if (!gtt_base)
			die_with_post_code(POSTCODE_HW_INIT_FAILURE,
					   "GTTMMADR is not programmed!");
	}
	return gtt_base;
}

uint32_t graphics_gtt_read(unsigned long reg)
{
	return read32p(graphics_get_gtt_base() + reg);
}

void graphics_gtt_write(unsigned long reg, uint32_t data)
{
	write32p(graphics_get_gtt_base() + reg, data);
}

void graphics_gtt_rmw(unsigned long reg, uint32_t andmask, uint32_t ormask)
{
	uint32_t val = graphics_gtt_read(reg);
	val &= andmask;
	val |= ormask;
	graphics_gtt_write(reg, val);
}

static void graphics_dev_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	if (CONFIG(SOC_INTEL_GFX_NON_PREFETCHABLE_MMIO)) {
		struct resource *res_bar0 = find_resource(dev, PCI_BASE_ADDRESS_0);
		if (res_bar0->flags & IORESOURCE_PREFETCH)
			res_bar0->flags &= ~IORESOURCE_PREFETCH;
	}

	/*
	 * If libhwbase static MMIO driver is used, IGD BAR 0 has to be set to
	 * CONFIG_GFX_GMA_DEFAULT_MMIO for the libgfxinit to operate properly.
	 */
	if (CONFIG(MAINBOARD_USE_LIBGFXINIT) && CONFIG(HWBASE_STATIC_MMIO)) {
		struct resource *res_bar0 = find_resource(dev, PCI_BASE_ADDRESS_0);
		res_bar0->base = CONFIG_GFX_GMA_DEFAULT_MMIO;
		res_bar0->flags |= IORESOURCE_ASSIGNED;
		pci_dev_set_resources(dev);
		res_bar0->flags |= IORESOURCE_FIXED;
	}

	const struct hob_resource *res =
		fsp_find_resource_hob_by_guid(memory_compression_guid);
	if (res) {
		printk(BIOS_DEBUG,
		       "Memory Compression HOB found: base=0x%08llx length=0x%08llx\n",
		       res->addr, res->length);
		reserved_ram_range(dev, 0, res->addr, res->length);
	}
}

static void graphics_join_mbus(void)
{
	enum display_type type = get_external_display_status();
	uint32_t hashing_mode = 0;  /* 2x2 */
	if (type == INTERNAL_DISPLAY_ONLY) {
		hashing_mode = GFX_MBUS_HASHING_MODE; /* 1x4 */
		/* Only eDP pipes is joining the MBUS */
		graphics_gtt_rmw(GFX_MBUS_SEL(PIPE_A), PIPE_A, GFX_MBUS_JOIN | hashing_mode);
	} else if (type == DUAL_DISPLAY) {
		/* All pipes are joining the MBUS */
		graphics_gtt_rmw(GFX_MBUS_SEL(PIPE_A), PIPE_A, GFX_MBUS_JOIN | hashing_mode);
		graphics_gtt_rmw(GFX_MBUS_SEL(PIPE_B), PIPE_B, GFX_MBUS_JOIN | hashing_mode);
		graphics_gtt_rmw(GFX_MBUS_SEL(PIPE_C), PIPE_C, GFX_MBUS_JOIN | hashing_mode);
#if CONFIG(INTEL_GMA_VERSION_2)
		graphics_gtt_rmw(GFX_MBUS_SEL(PIPE_D), PIPE_D, GFX_MBUS_JOIN | hashing_mode);
#endif
	} else {
		/* No pipe joins the MBUS */
		graphics_gtt_rmw(GFX_MBUS_CTL, GFX_MBUS_JOIN_PIPE_SEL,
				 GFX_MBUS_JOIN | hashing_mode);
	}
}

static void graphics_dev_final(struct device *dev)
{
	pci_dev_request_bus_master(dev);

	/*
	 * Call function to join the MBUS if GFX PEIM module inside FSP
	 * binary is taking care of graphics initialization based on
	 * RUN_FSP_GOP config option.
	 *
	 * Skip FW joining the MBUS in case of non-FSP solution.
	 */
	if (CONFIG(RUN_FSP_GOP) && CONFIG(SOC_INTEL_GFX_MBUS_JOIN) && display_init_required())
		graphics_join_mbus();
}

const struct device_operations graphics_ops = {
	.read_resources		= graphics_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_init,
	.final			= graphics_dev_final,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= gma_generate_ssdt,
#endif
	.scan_bus		= scan_generic_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_WCL_GT2_1,
	PCI_DID_INTEL_WCL_GT2_2,
	PCI_DID_INTEL_PTL_U_GT2_1,
	PCI_DID_INTEL_PTL_H_GT2_1,
	PCI_DID_INTEL_PTL_H_GT2_2,
	PCI_DID_INTEL_PTL_H_GT2_3,
	PCI_DID_INTEL_PTL_H_GT2_4,
	PCI_DID_INTEL_LNL_M_GT2,
	PCI_DID_INTEL_RPL_U_GT1,
	PCI_DID_INTEL_RPL_U_GT2,
	PCI_DID_INTEL_RPL_U_GT3,
	PCI_DID_INTEL_RPL_U_GT4,
	PCI_DID_INTEL_RPL_U_GT5,
	PCI_DID_INTEL_RPL_P_GT1,
	PCI_DID_INTEL_RPL_P_GT2,
	PCI_DID_INTEL_RPL_P_GT3,
	PCI_DID_INTEL_RPL_P_GT4,
	PCI_DID_INTEL_RPL_P_GT5,
	PCI_DID_INTEL_MTL_M_GT2,
	PCI_DID_INTEL_MTL_P_GT2_1,
	PCI_DID_INTEL_MTL_P_GT2_2,
	PCI_DID_INTEL_MTL_P_GT2_3,
	PCI_DID_INTEL_MTL_P_GT2_4,
	PCI_DID_INTEL_MTL_P_GT2_5,
	PCI_DID_INTEL_ARL_H_GT2_1,
	PCI_DID_INTEL_ARL_H_GT2_2,
	PCI_DID_INTEL_APL_IGD_HD_505,
	PCI_DID_INTEL_APL_IGD_HD_500,
	PCI_DID_INTEL_CNL_GT2_ULX_1,
	PCI_DID_INTEL_CNL_GT2_ULX_2,
	PCI_DID_INTEL_CNL_GT2_ULX_3,
	PCI_DID_INTEL_CNL_GT2_ULX_4,
	PCI_DID_INTEL_CNL_GT2_ULT_1,
	PCI_DID_INTEL_CNL_GT2_ULT_2,
	PCI_DID_INTEL_CNL_GT2_ULT_3,
	PCI_DID_INTEL_CNL_GT2_ULT_4,
	PCI_DID_INTEL_GLK_IGD,
	PCI_DID_INTEL_GLK_IGD_EU12,
	PCI_DID_INTEL_WHL_GT1_ULT_1,
	PCI_DID_INTEL_WHL_GT2_ULT_1,
	PCI_DID_INTEL_AML_GT2_ULX,
	PCI_DID_INTEL_CFL_H_GT2,
	PCI_DID_INTEL_CFL_H_XEON_GT2,
	PCI_DID_INTEL_CFL_S_GT1_1,
	PCI_DID_INTEL_CFL_S_GT1_2,
	PCI_DID_INTEL_CFL_S_GT2_1,
	PCI_DID_INTEL_CFL_S_GT2_2,
	PCI_DID_INTEL_CFL_S_GT2_3,
	PCI_DID_INTEL_CFL_S_GT2_4,
	PCI_DID_INTEL_CFL_S_GT2_5,
	PCI_DID_INTEL_CML_GT1_ULT_1,
	PCI_DID_INTEL_CML_GT1_ULT_2,
	PCI_DID_INTEL_CML_GT2_ULT_1,
	PCI_DID_INTEL_CML_GT2_ULT_2,
	PCI_DID_INTEL_CML_GT1_ULT_3,
	PCI_DID_INTEL_CML_GT1_ULT_4,
	PCI_DID_INTEL_CML_GT2_ULT_5,
	PCI_DID_INTEL_CML_GT2_ULT_6,
	PCI_DID_INTEL_CML_GT2_ULT_7,
	PCI_DID_INTEL_CML_GT2_ULT_8,
	PCI_DID_INTEL_CML_GT2_ULT_3,
	PCI_DID_INTEL_CML_GT2_ULT_4,
	PCI_DID_INTEL_CML_GT1_ULX_1,
	PCI_DID_INTEL_CML_GT2_ULX_1,
	PCI_DID_INTEL_CML_GT1_S_1,
	PCI_DID_INTEL_CML_GT1_S_2,
	PCI_DID_INTEL_CML_GT1_H_1,
	PCI_DID_INTEL_CML_GT1_H_2,
	PCI_DID_INTEL_CML_GT2_S_G0,
	PCI_DID_INTEL_CML_GT2_S_P0,
	PCI_DID_INTEL_CML_GT2_H_R0,
	PCI_DID_INTEL_CML_GT2_H_R1,
	PCI_DID_INTEL_TGL_GT0,
	PCI_DID_INTEL_TGL_GT1_H_32,
	PCI_DID_INTEL_TGL_GT1_H_16,
	PCI_DID_INTEL_TGL_GT2_ULT,
	PCI_DID_INTEL_TGL_GT2_ULX,
	PCI_DID_INTEL_TGL_GT3_ULT,
	PCI_DID_INTEL_TGL_GT2_ULT_1,
	PCI_DID_INTEL_EHL_GT1_1,
	PCI_DID_INTEL_EHL_GT2_1,
	PCI_DID_INTEL_EHL_GT1_2,
	PCI_DID_INTEL_EHL_GT2_2,
	PCI_DID_INTEL_EHL_GT1_2_1,
	PCI_DID_INTEL_EHL_GT1_3,
	PCI_DID_INTEL_EHL_GT2_3,
	PCI_DID_INTEL_JSL_GT1,
	PCI_DID_INTEL_JSL_GT2,
	PCI_DID_INTEL_JSL_GT3,
	PCI_DID_INTEL_JSL_GT4,
	PCI_DID_INTEL_ADL_GT0,
	PCI_DID_INTEL_ADL_GT1,
	PCI_DID_INTEL_ADL_GT1_1,
	PCI_DID_INTEL_ADL_GT1_2,
	PCI_DID_INTEL_ADL_GT1_3,
	PCI_DID_INTEL_ADL_GT1_4,
	PCI_DID_INTEL_ADL_GT1_5,
	PCI_DID_INTEL_ADL_GT1_6,
	PCI_DID_INTEL_ADL_GT1_7,
	PCI_DID_INTEL_ADL_GT1_8,
	PCI_DID_INTEL_ADL_GT1_9,
	PCI_DID_INTEL_ADL_P_GT2,
	PCI_DID_INTEL_ADL_P_GT2_1,
	PCI_DID_INTEL_ADL_P_GT2_2,
	PCI_DID_INTEL_ADL_P_GT2_3,
	PCI_DID_INTEL_ADL_P_GT2_4,
	PCI_DID_INTEL_ADL_P_GT2_5,
	PCI_DID_INTEL_ADL_P_GT2_6,
	PCI_DID_INTEL_ADL_P_GT2_7,
	PCI_DID_INTEL_ADL_P_GT2_8,
	PCI_DID_INTEL_ADL_P_GT2_9,
	PCI_DID_INTEL_ADL_S_GT1,
	PCI_DID_INTEL_ADL_S_GT1_1,
	PCI_DID_INTEL_ADL_S_GT2,
	PCI_DID_INTEL_ADL_S_GT2_1,
	PCI_DID_INTEL_ADL_S_GT2_2,
	PCI_DID_INTEL_ADL_M_GT1,
	PCI_DID_INTEL_ADL_M_GT2,
	PCI_DID_INTEL_ADL_M_GT3,
	PCI_DID_INTEL_ADL_N_GT1,
	PCI_DID_INTEL_ADL_N_GT2,
	PCI_DID_INTEL_ADL_N_GT3,
	PCI_DID_INTEL_ADL_N_GT4,
	PCI_DID_INTEL_ADL_N_GT5,
	PCI_DID_INTEL_RPL_S_GT0,
	PCI_DID_INTEL_RPL_S_GT1_1,
	PCI_DID_INTEL_RPL_S_GT1_2,
	PCI_DID_INTEL_RPL_S_GT1_3,
	PCI_DID_INTEL_RPL_HX_GT1,
	PCI_DID_INTEL_RPL_HX_GT2,
	PCI_DID_INTEL_RPL_HX_GT3,
	PCI_DID_INTEL_RPL_HX_GT4,
	PCI_DID_INTEL_TWL_GT1_1,
	PCI_DID_INTEL_TWL_GT1_2,
	0,
};

static const struct pci_driver graphics_driver __pci_driver = {
	.ops		= &graphics_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
