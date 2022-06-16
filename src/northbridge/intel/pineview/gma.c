/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/opregion.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>
#include <types.h>

#include "chip.h"
#include "pineview.h"

#define GTTSIZE		(512 * 1024)

#define PGETBL2_CTL	0x20c4
#define PGETBL2_1MB	(1 << 8)

#define PGETBL_CTL	0x2020
#define PGETBL_1MB	(3 << 1)
#define PGETBL_512KB	0
#define PGETBL_ENABLED	0x1

#define ADPA_HOTPLUG_BITS (ADPA_CRT_HOTPLUG_PERIOD_128   | \
			   ADPA_CRT_HOTPLUG_WARMUP_10MS  | \
			   ADPA_CRT_HOTPLUG_MONITOR_COLOR| \
			   ADPA_CRT_HOTPLUG_SAMPLE_4S    | \
			   ADPA_CRT_HOTPLUG_VOLTAGE_50   | \
			   ADPA_CRT_HOTPLUG_VOLREF_325MV | \
			   ADPA_CRT_HOTPLUG_ENABLE)

static struct resource *gtt_res  = NULL;
static struct resource *mmio_res = NULL;

static int gtt_setup(u8 *mmiobase)
{
	u32 gttbase;
	struct device *dev = pcidev_on_root(0, 0);

	gttbase = pci_read_config32(dev, BGSM);
	printk(BIOS_DEBUG, "gttbase = %08x\n", gttbase);

	write32(mmiobase + PGETBL_CTL, gttbase | PGETBL_512KB);
	udelay(50);
	write32(mmiobase + PGETBL_CTL, gttbase | PGETBL_512KB);

	write32(mmiobase + GFX_FLSH_CNTL, 0);

	return 0;
}

static void intel_gma_init(const struct northbridge_intel_pineview_config *info,
	struct device *vga, u8 *mmio, u8 *gtt, u32 physbase, u16 piobase)
{
	int i;
	u32 hactive, vactive;
	u32 temp;

	printk(BIOS_SPEW, "gtt %x mmio %x addrport %x physbase %x\n",
		(u32)gtt, (u32)mmio, piobase, physbase);

	gtt_setup(mmio);

	pci_write_config16(vga, GGC, 0x130);

	/* Disable VGA.  */
	write32(mmio + VGACNTRL, VGA_DISP_DISABLE);

	/* Disable pipes.  */
	write32(mmio + PIPECONF(0), 0);
	write32(mmio + PIPECONF(1), 0);

	write32(mmio + INSTPM, 0x800);

	vga_gr_write(0x18, 0);

	write32(mmio + VGA0, 0x200074);
	write32(mmio + VGA1, 0x200074);

	write32(mmio + DSPFW3, 0x7f3f00c1 & ~PINEVIEW_SELF_REFRESH_EN);
	write32(mmio + DSPCLK_GATE_D, 0);
	write32(mmio + FW_BLC, 0x03060106);
	write32(mmio + FW_BLC2, 0x00000306);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_HOTPLUG_BITS
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON);

	write32(mmio + 0x7041c, 0x0);

	write32(mmio + DPLL_MD(0), 0x3);
	write32(mmio + DPLL_MD(1), 0x3);
	write32(mmio + DSPCNTR(1), 0x1000000);
	write32(mmio + PIPESRC(1), 0x027f01df);

	vga_misc_write(0x67);
	const u8 cr[25] = {
		0x5f, 0x4f, 0x50, 0x82, 0x55,
		0x81, 0xbf, 0x1f, 0x00, 0x4f,
		0x0d, 0x0e, 0x00, 0x00, 0x00,
		0x00, 0x9c, 0x8e, 0x8f, 0x28,
		0x1f, 0x96, 0xb9, 0xa3, 0xff,
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i < ARRAY_SIZE(cr); i++)
		vga_cr_write(i, cr[i]);

	// Disable screen memory to prevent garbage from appearing.
	vga_sr_write(1, vga_sr_read(1) | 0x20);
	hactive = 640;
	vactive = 400;

	mdelay(1);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_DAC_SERIAL
		| DPLL_VGA_MODE_DIS
		| DPLL_DAC_SERIAL_P2_CLOCK_DIV_10
		| 0x400601);

	mdelay(1);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_DAC_SERIAL
		| DPLL_VGA_MODE_DIS
		| DPLL_DAC_SERIAL_P2_CLOCK_DIV_10
		| 0x400601);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_HOTPLUG_BITS
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON);

	write32(mmio + HTOTAL(1), 0x031f027f);
	write32(mmio + HBLANK(1), 0x03170287);
	write32(mmio + HSYNC(1), 0x02ef028f);
	write32(mmio + VTOTAL(1), 0x020c01df);
	write32(mmio + VBLANK(1), 0x020401e7);
	write32(mmio + VSYNC(1), 0x01eb01e9);

	write32(mmio + HTOTAL(0), ((hactive - 1) << 16) | (hactive - 1));
	write32(mmio + HBLANK(0), ((hactive - 1) << 16) | (hactive - 1));
	write32(mmio + HSYNC(0),  ((hactive - 1) << 16) | (hactive - 1));
	write32(mmio + VTOTAL(0), ((vactive - 1) << 16) | (vactive - 1));
	write32(mmio + VBLANK(0), ((vactive - 1) << 16) | (vactive - 1));
	write32(mmio + VSYNC(0),  ((vactive - 1) << 16) | (vactive - 1));

	write32(mmio + PF_WIN_POS(0), 0);

	write32(mmio + PIPESRC(0), (639 << 16) | 399);
	write32(mmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
	write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	write32(mmio + PFIT_CONTROL, 0x0);

	mdelay(1);

	write32(mmio + FDI_RX_CTL(0), 0x00002040);
	mdelay(1);
	write32(mmio + FDI_RX_CTL(0), 0x80002050);
	write32(mmio + FDI_TX_CTL(0), 0x00044000);
	mdelay(1);
	write32(mmio + FDI_TX_CTL(0), 0x80044000);
	write32(mmio + PIPECONF(0), PIPECONF_ENABLE | PIPECONF_BPP_6 | PIPECONF_DITHER_EN);

	write32(mmio + VGACNTRL, 0x0);
	write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888);
	mdelay(1);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_HOTPLUG_BITS
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON);

	write32(mmio + DSPFW3, 0x7f3f00c1);
	write32(mmio + MI_MODE, 0x200 | VS_TIMER_DISPATCH);
	write32(mmio + CACHE_MODE_0, (0x6820 | (1 << 9)) & ~(1 << 5));
	write32(mmio + CACHE_MODE_1, 0x380 & ~(1 << 9));

	for (i = 0; i < (8192 - 512) / 4; i++) {
		outl((i << 2) | 1, piobase);
		outl((physbase + (i << 12)) | 1, piobase + 4);
	}

	temp = read32(mmio + PGETBL_CTL);
	printk(BIOS_INFO, "GTT PGETBL_CTL register : 0x%08x\n", temp);
	temp = read32(mmio + PGETBL2_CTL);
	printk(BIOS_INFO, "GTT PGETBL2_CTL register: 0x%08x\n", temp);

	/* Clear interrupts */
	write32(mmio + DEIIR,  0xffffffff);
	write32(mmio + SDEIIR, 0xffffffff);
	write32(mmio + IIR,    0xffffffff);
	write32(mmio + IMR,    0xffffffff);
	write32(mmio + EIR,    0xffffffff);

	vga_textmode_init();

	/* Enable screen memory */
	vga_sr_write(1, vga_sr_read(1) & ~0x20);
}

static void gma_func0_init(struct device *dev)
{
	intel_gma_init_igd_opregion();

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	if (!CONFIG(MAINBOARD_DO_NATIVE_VGA_INIT)) {
		/* PCI init, will run VBIOS */
		pci_dev_init(dev);
	} else {
		u32 physbase;
		struct resource *pio_res;
		struct northbridge_intel_pineview_config *conf = dev->chip_info;

		int vga_disable = (pci_read_config16(dev, GGC) & 2) >> 1;

		/* Find base addresses */
		mmio_res = find_resource(dev, PCI_BASE_ADDRESS_0);
		gtt_res  = find_resource(dev, PCI_BASE_ADDRESS_3);
		pio_res  = find_resource(dev, PCI_BASE_ADDRESS_1);
		physbase = pci_read_config32(dev, 0x5c) & ~0xf;

		if (gtt_res && gtt_res->base && physbase && pio_res && pio_res->base) {
			if (vga_disable) {
				printk(BIOS_INFO, "IGD is not decoding legacy VGA MEM and IO: "
						  "skipping NATIVE graphic init\n");
			} else {
				printk(BIOS_SPEW, "Initializing VGA. MMIO 0x%llx\n",
				       mmio_res->base);
				intel_gma_init(conf, dev,
					       res2mmio(mmio_res, 0, 0),
					       res2mmio(gtt_res, 0, 0),
					       physbase, pio_res->base);
			}
		}

		/* Linux relies on VBT for panel info.  */
		generate_fake_intel_oprom(&conf->gfx, dev, "$VBT PINEVIEW");
	}
}

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct device_operations gma_func0_ops = {
	.read_resources         = pci_dev_read_resources,
	.set_resources          = pci_dev_set_resources,
	.enable_resources       = pci_dev_enable_resources,
	.init                   = gma_func0_init,
	.ops_pci                = &pci_dev_ops_pci,
	.acpi_name              = gma_acpi_name,
};

static const unsigned short pci_device_ids[] =
{
	0xa001, 0,
};

static const struct pci_driver gma __pci_driver = {
	.ops     = &gma_func0_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
