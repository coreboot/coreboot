/* Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scaning the bus for
 * devices which is done by the kernel
 *
 * written in 2003 by Eric Biederman
 *
 *  - Athlon64 workarounds by Stefan Reinauer
 *  - "reset once" logic by Yinghai Lu
 */

#include <console.h>
#include <lib.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#warning Broken hard_reset
//#include <part/hard_reset.h>
#include <mc146818rtc.h>
#include <amd/k8/k8.h>

#warning Make AGP_APERTURE_SIZE a CONFIG variable
#define AGP_APERTURE_SIZE 0x4000000	//64M

/**
 * @brief Read resources for AGP aperture
 *
 * @param
 *
 * There is only one AGP aperture resource needed. The resoruce is added to
 * the northbridge of BSP.
 *
 * The same trick can be used to augment legacy VGA resources which can
 * be detected by the generic pci resource allocator for VGA devices.
 * BAD: it is more tricky than I think, the resource allocation code is
 * implemented in a way to NOT DOING legacy VGA resource allcation on
 * purpose :-(.
 */
static void mcf3_read_resources(struct device *dev)
{
	struct resource *resource;
	unsigned char iommu;
	/* Read the generic PCI resources */
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the GART aperture */
	if (dev->path.pci.devfn != PCI_DEVFN(0x18, 3)) {
		return;
	}

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		/* Add a GART aperture resource */
		resource = new_resource(dev, 0x94);
		resource->size = iommu ? AGP_APERTURE_SIZE : 1;
		resource->align = log2f(resource->size);
		resource->gran = log2f(resource->size);
		resource->limit = 0xffffffff;	/* 4G */
		resource->flags = IORESOURCE_MEM;
	}
}

static void set_agp_aperture(struct device *dev)
{
	struct resource *resource;

	resource = probe_resource(dev, 0x94);
	if (resource) {
		struct device *pdev;
		struct device_id id;
		u32 gart_base, gart_acr;

		/* Remember this resource has been stored */
		resource->flags |= IORESOURCE_STORED;

		/* Find the size of the GART aperture */
		gart_acr = (0 << 6) | (0 << 5) | (0 << 4) |
		           ((resource->gran - 25) << 1) | (0 << 0);

		/* Get the base address */
		gart_base = ((resource->base) >> 25) & 0x00007fff;

		/* Update the other northbriges */
		pdev = NULL;
		id.type = DEVICE_ID_PCI;
		id.pci.vendor = PCI_VENDOR_ID_AMD;
		id.pci.device = 0x1103;
		while ((pdev = dev_find_device(&id, pdev))) {
			/* Store the GART size but don't enable it */
			pci_write_config32(pdev, 0x90, gart_acr);

			/* Store the GART base address */
			pci_write_config32(pdev, 0x94, gart_base);

			/* Don't set the GART Table base address */
			pci_write_config32(pdev, 0x98, 0);

			/* Report the resource has been stored... */
			report_resource_stored(pdev, resource, " <gart>");
		}
	}
}

static void mcf3_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s...\n", __func__);
	/* Set the GART aperture */
	set_agp_aperture(dev);

	/* Set the generic PCI resources */
	pci_set_resources(dev);
}

static void misc_control_init(struct device *dev)
{
	u32 cmd, cmd_ref;
	int needs_reset;
	struct device *f0_dev;

	printk(BIOS_DEBUG, "NB: Function 3 Misc Control..\n");
	needs_reset = 0;

	/* Disable Machine checks from Invalid Locations.
	 * This is needed for PC backwards compatibility.
	 */
	cmd = pci_read_config32(dev, 0x44);
	cmd |= (1 << 6) | (1 << 25);
	pci_write_config32(dev, 0x44, cmd);

	/* Optimize the Link read pointers */
	f0_dev = dev_find_slot(0, dev->path.pci.devfn - 3);
	if (f0_dev) {
		int link;
		cmd_ref = cmd = pci_read_config32(dev, 0xdc);
		for (link = 0; link < 3; link++) {
			u32 link_type;
			unsigned reg;
			/* This works on an Athlon64 because unimplemented
			 * links return 0.
			 */
			reg = 0x98 + (link * 0x20);
			link_type = pci_read_config32(f0_dev, reg);
			/* Only handle coherent links here please. */
			if ((link_type &
			     (LinkConnected | InitComplete | NonCoherent))
			    == (LinkConnected | InitComplete)) {
				cmd &= ~(0xff << (link * 8));
				/* FIXME this assumes the device on the other
				 * side is an AMD device.
				 */
				cmd |= 0x25 << (link * 8);
			}
		}
		if (cmd != cmd_ref) {
			pci_write_config32(dev, 0xdc, cmd);
			needs_reset = 1;
		}
	} else {
		printk(BIOS_ERR, "Missing f0 device!\n");
	}
	if (needs_reset) {
		printk(BIOS_DEBUG, "Not resetting cpu, but it's needed.\n");
		//hard_reset();
	}
	printk(BIOS_DEBUG, "done.\n");
}

struct device_operations mcf3_ops = {
	.id = {.type = DEVICE_ID_PCI,
	       {.pci = {.vendor = PCI_VENDOR_ID_AMD,
			.device = 0x1103}}},
	.phase3_scan		 = 0,
	.phase4_read_resources	 = mcf3_read_resources,
	.phase4_set_resources	 = mcf3_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = misc_control_init,
	.ops_pci		 = 0,
};

