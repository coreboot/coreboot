/* cocci issues ;-( */
#ifndef VIDEO_H
#define VIDEO_H 1
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <pci/pci.h>
#include <sys/io.h>
#include <sys/time.h>
#include <linux/types.h>
/* stuff we can't get coccinelle to do yet */
#define __iomem
#define __read_mostly
#define __always_unused
#define module_param_named(a, b, c, d)
#define MODULE_PARM_DESC(a, b)
#define DRM_DEBUG_KMS printf
#define CONFIG_DRM_I915_KMS 1
#define module_init(x);
#define module_exit(x);

#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define MODULE_LICENSE(a)
#define MODULE_DEVICE_TABLE(a, b)

/* constants that will never change from linux/vga.h */
/* Legacy VGA regions */
#define VGA_RSRC_NONE          0x00
#define VGA_RSRC_LEGACY_IO     0x01
#define VGA_RSRC_LEGACY_MEM    0x02
#define VGA_RSRC_LEGACY_MASK   (VGA_RSRC_LEGACY_IO | VGA_RSRC_LEGACY_MEM)
/* Non-legacy access */
#define VGA_RSRC_NORMAL_IO     0x04
#define VGA_RSRC_NORMAL_MEM    0x08


/* define in pci.h! */
#include <pci/pci.h>
/* idiocy. how many names to we need for a type? */
typedef u32 uint32_t;
typedef u64 uint64_t;
/* WTF */
typedef int bool;
enum {false = 0, true};

/* we define our own. The kernel one is too full of stuff. */
struct mode_config {
	int num_fb;
	int num_connector;
	int num_crtc;
	int num_encoder;
	int min_width, min_height, max_width, max_height;
};

struct drm_device {
	struct pci_dev *pdev;
	u8 *bios_bin;
	struct drm_i915_private *dev_private;
	struct mode_config mode_config;
};

/* we're willing to define our own here because it's relatively unchanging */
#define PCI_ANY_ID (~0)

struct pci_device_id {
        u32 vendor, device;           /* Vendor and device ID or PCI_ANY_ID*/
        u32 subvendor, subdevice;     /* Subsystem ID's or PCI_ANY_ID */
        u32 class, class_mask;        /* (class,subclass,prog-if) triplet */
        unsigned long driver_data;     /* Data private to the driver  */
};


/* per the cocinelle people, they can't handle this.
 * It also almost never changes */
#define INTEL_VGA_DEVICE(id, info) {            \
        .class = PCI_CLASS_DISPLAY_VGA << 8,    \
        .class_mask = 0xff0000,                 \
        .vendor = 0x8086,                       \
        .device = id,                           \
        .subvendor = PCI_ANY_ID,                \
        .subdevice = PCI_ANY_ID,                \
        .driver_data = (unsigned long) info }

#define wait_for(condition, time) (sleep(1+time/50)  && (!condition))


/* random crap from kernel.h.
 * Kernel.h is a catch-all for all kinds of junk and it's
 * not worth using coccinelle (yet) to pull it apart. Maybe later.
 * And, yes, gcc still does not have nelem!
 */
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))
#define __ALIGN_MASK(x, mask)	__ALIGN_KERNEL_MASK((x), (mask))
#define PTR_ALIGN(p, a)		((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)


/* temporary. */
void *dmi_check_system(unsigned long);

#include "final/drm_dp_helper.h"
#include "final/i915_reg.h"
#include "final/i915_drv.h"
#include "final/drm_mode.h"
#include "final/drm_crtc.h"

unsigned long I915_READ(unsigned long addr);
void I915_WRITE(unsigned long addr, unsigned long val);
u16 I915_READ16(unsigned long addr);
void I915_WRITE16(unsigned long addr, u16 val);
unsigned long msecs(void);
void mdelay(unsigned long ms);

/* these should be the same. */
#define POSTING_READ I915_READ
#define POSTING_READ16 I915_READ16

void *pci_map_rom(struct pci_dev *dev, size_t *size);
void *pci_unmap_rom(struct pci_dev *dev, void *p);
extern unsigned int i915_lvds_downclock;
extern int i915_vbt_sdvo_panel_type;
unsigned long lvds_do_not_use_alternate_frequency;
#endif /* VIDEO_H */
