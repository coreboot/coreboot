## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVERS_INTEL_MIPI_CAMERA) += camera.c
ramstage-$(CONFIG_DRIVERS_INTEL_MIPI_CAMERA_HAS_PMIC) += pmic/pmic.c
