/* SPDX-License-Identifier: GPL-2.0-only */

/* Include Variant DPTF */
#include <variant/acpi/dptf.asl>

/* Include SoC DPTF */
#if !CONFIG(BOARD_GOOGLE_TERRA)
#include <acpi/dptf/dptf.asl>
#endif
