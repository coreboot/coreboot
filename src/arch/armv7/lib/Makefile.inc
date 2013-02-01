romstage-y += cache_v7.c
romstage-y += cache-cp15.c
romstage-y += div0.c
romstage-y += div64.S
romstage-y += hang_spl.c
romstage-y += romstage_console.c
romstage-y += syslib.c

#ramstage-y += printk_init.c
#romstage-y += walkcbfs.S

ramstage-y += div0.c
ramstage-y += div64.S
ramstage-y += hang_spl.c
#ramstage-y += interrupts.c
#ramstage-y += memcpy.S
#ramstage-y += memset.S
#ramstage-y += reset.c
ramstage-y += syslib.c

#FIXME(dhendrix): should this be a config option?
romstage-y += eabi_compat.c
ramstage-y += eabi_compat.c
