ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

romstage-y += h3finit.c ht_wrapper.c comlib.c AsPsNb.c h3ncmn.c

endif
