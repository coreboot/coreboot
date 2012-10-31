romstage-y += exynos_cache.c

# TODO(dhendrix): added soc.c just to include exynos5-common.h. Maybe there
# is a better place for this?
romstage-y += soc.c

ramstage-y += clock.c
ramstage-y += clock_init.c
ramstage-y += power.c
ramstage-y += spl.c
ramstage-y += pinmux.c
#ramstage-y += tzpc_init.c

#ramstage-$(CONFIG_EXYNOS_ACE_SHA) += ace_sha.c
#ramstage-$(CONFIG_SATA_AHCI) += sata.c
ramstage-$(CONFIG_SPL_BUILD) += lowlevel_init_c.c
ramstage-$(CONFIG_SPL_BUILD) += dmc_common.c
ramstage-$(CONFIG_SPL_BUILD) += dmc_init_ddr3.c
