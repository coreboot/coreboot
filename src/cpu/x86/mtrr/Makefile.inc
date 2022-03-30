ramstage-y	+= mtrr.c

ramstage-y	+= mtrrlib.c
romstage-y	+= mtrrlib.c
bootblock-y	+= mtrrlib.c
verstage_x86-y	+= mtrrlib.c

romstage-y	+= earlymtrr.c
bootblock-y	+= earlymtrr.c
verstage_x86-y	+= earlymtrr.c

bootblock-y	+= debug.c
romstage-y	+= debug.c
postcar-y	+= debug.c
ramstage-y	+= debug.c

bootblock-$(CONFIG_SETUP_XIP_CACHE) += xip_cache.c
verstage_x86-$(CONFIG_SETUP_XIP_CACHE) += xip_cache.c
