/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Include Variant DPTF */
#include <variant/acpi/dptf.asl>

/* Include SoC DPTF */
#if !CONFIG(BOARD_GOOGLE_TERRA)
#include <acpi/dptf/dptf.asl>
#endif
