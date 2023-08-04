## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += conn
ramstage-$(CONFIG_DRIVERS_INTEL_PMC) += mux.c
