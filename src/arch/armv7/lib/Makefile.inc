bootblock-y += syslib.c
bootblock-$(CONFIG_EARLY_CONSOLE) += early_console.c

romstage-y += cache_v7.c
romstage-y += cache-cp15.c
romstage-y += div0.c
romstage-y += div64.S
romstage-y += syslib.c
romstage-$(CONFIG_EARLY_CONSOLE) += early_console.c

ramstage-y += div0.c
ramstage-y += div64.S
#ramstage-y += interrupts.c
#ramstage-y += memcpy.S
#ramstage-y += memset.S
ramstage-y += syslib.c

#FIXME(dhendrix): should this be a config option?
romstage-y += eabi_compat.c
ramstage-y += eabi_compat.c
bootblock-y += eabi_compat.c
