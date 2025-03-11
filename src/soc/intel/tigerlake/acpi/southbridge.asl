/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

/* PCR access */
#include <soc/intel/common/acpi/pch_pcr.asl>

/* PCH clock */
#include "camera_clock_ctl.asl"

/* GPIO controller */
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
#include "gpio_pch_h.asl"
#else
#include "gpio.asl"
#endif

/* ESPI 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include <soc/intel/common/block/acpi/acpi/smbus.asl>

/* ISH 0:12.0 */
#include <soc/intel/common/block/acpi/acpi/ish.asl>

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PMC Shared SRAM 0:14.2 */
#include <soc/intel/common/block/acpi/acpi/sram.asl>

/* CSE/HECI #1 0:16.0 */
#include <soc/intel/common/block/acpi/acpi/heci.asl>


/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* GbE 0:1f.6 */
#include <soc/intel/common/block/acpi/acpi/pch_glan.asl>
