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

/* LPC 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include <soc/intel/common/block/acpi/acpi/smbus.asl>

/* ISH 0:13.0 */
#include <soc/intel/common/block/acpi/acpi/ish.asl>

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* GbE 0:1f.6 */
#include <soc/intel/common/block/acpi/acpi/pch_glan.asl>

/* Intel Power Engine Plug-in */
#include <soc/intel/common/block/acpi/acpi/pep.asl>
