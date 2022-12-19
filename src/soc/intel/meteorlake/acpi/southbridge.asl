/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>

/* PCR access */
#include <soc/intel/common/acpi/pcr.asl>

/* PCH clock */
#include "camera_clock_ctl.asl"

/* GPIO controller */
#include "gpio.asl"

/* ESPI 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* HDA */
#include "hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include <soc/intel/common/block/acpi/acpi/smbus.asl>

/* ISH 0:12.0 */
#if CONFIG(DRIVERS_INTEL_ISH)
#include <soc/intel/common/block/acpi/acpi/ish.asl>
#endif

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* GbE 0:1f.6 */
#if CONFIG(MAINBOARD_USES_IFD_GBE_REGION)
#include <soc/intel/common/block/acpi/acpi/pch_glan.asl>
#endif
