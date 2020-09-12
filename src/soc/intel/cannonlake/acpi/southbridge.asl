/* SPDX-License-Identifier: GPL-2.0-or-later */

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* PCR access */
#include <soc/intel/common/acpi/pcr.asl>

/* eMMC, SD Card */
#include "scs.asl"

/* GPIO controller */
#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
#include "gpio_cnp_h.asl"
#else
#include "gpio.asl"
#endif

/* GFX 00:02.0 */
#include "gfx.asl"

/* LPC 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include "smbus.asl"

/* ISH 0:13.0 */
#include "ish.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* GBe 0:1f.6 */
#include "pch_glan.asl"

/* PMC Core */
#include <soc/intel/common/block/acpi/acpi/pmc.asl>
