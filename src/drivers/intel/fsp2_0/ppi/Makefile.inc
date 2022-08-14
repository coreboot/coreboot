# SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_MP_SERVICES_PPI) += mp_service_ppi.c
ramstage-$(CONFIG_MP_SERVICES_PPI_V1) += mp_service1.c
ramstage-$(CONFIG_MP_SERVICES_PPI_V2) += mp_service2.c
ramstage-$(CONFIG_MP_SERVICES_PPI_V2_NOOP) += mp_service2_noop.c
