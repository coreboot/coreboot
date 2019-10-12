ramstage-y	+= mtrr.c

romstage-y	+= earlymtrr.c
bootblock-y	+= earlymtrr.c
verstage-y	+= earlymtrr.c

bootblock-y	+= debug.c
romstage-y	+= debug.c
postcar-y	+= debug.c
ramstage-y	+= debug.c

bootblock-$(CONFIG_SETUP_XIP_CACHE) += xip_cache.c
verstage-$(CONFIG_SETUP_XIP_CACHE) += xip_cache.c
