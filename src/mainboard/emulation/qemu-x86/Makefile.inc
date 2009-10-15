initobj-y += crt0.o
crt0-y += ../../../../src/cpu/x86/16bit/entry16.inc
crt0-y += ../../../../src/cpu/x86/32bit/entry32.inc
crt0-y += ../../../../src/cpu/x86/16bit/reset16.inc
crt0-y += ../../../../src/arch/i386/lib/id.inc
crt0-y += auto.inc

obj-y += mainboard.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o

ldscript-y += ../../../../src/arch/i386/init/ldscript_fallback_cbfs.lb
ldscript-y += ../../../../src/cpu/x86/16bit/entry16.lds
ldscript-y += ../../../../src/cpu/x86/32bit/entry32.lds
ldscript-y += ../../../../src/cpu/x86/16bit/reset16.lds
ldscript-y += ../../../../src/arch/i386/lib/id.lds

ifdef POST_EVALUATION

$(obj)/mainboard/$(MAINBOARDDIR)/auto.inc: $(obj)/romcc $(src)/mainboard/$(MAINBOARDDIR)/auto.c $(obj)/option_table.h
	$(obj)/romcc -mcpu=i386 -O $(INCLUDES) $(src)/mainboard/$(MAINBOARDDIR)/auto.c -o $@

endif
