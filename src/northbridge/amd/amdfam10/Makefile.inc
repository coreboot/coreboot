driver-y += northbridge.o
driver-y += misc_control.o

obj-$(CONFIG_GENERATE_ACPI_TABLES) += amdfam10_acpi.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += sspr1.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += sspr2.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += sspr3.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += sspr4.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += sspr5.o

obj-y += get_pci1234.o

ifdef POST_EVALUATION
$(obj)/northbridge/amd/amdfam10/ssdt.c: $(src)/northbridge/amd/amdfam10/ssdt.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/ssdt -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt/g' $(obj)/northbridge/amd/amdfam10/ssdt.hex
	mv $(obj)/northbridge/amd/amdfam10/ssdt.hex $@

$(obj)/northbridge/amd/amdfam10/sspr1.c: $(src)/northbridge/amd/amdfam10/sspr1.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/sspr1 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr1/g' $(obj)/northbridge/amd/amdfam10/sspr1.hex
	mv $(obj)/northbridge/amd/amdfam10/sspr1.hex $@

$(obj)/northbridge/amd/amdfam10/sspr2.c: $(src)/northbridge/amd/amdfam10/sspr2.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/sspr2 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr2/g' $(obj)/northbridge/amd/amdfam10/sspr2.hex
	mv $(obj)/northbridge/amd/amdfam10/sspr2.hex $@

$(obj)/northbridge/amd/amdfam10/sspr3.c: $(src)/northbridge/amd/amdfam10/sspr3.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/sspr3 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr3/g' $(obj)/northbridge/amd/amdfam10/sspr3.hex
	mv $(obj)/northbridge/amd/amdfam10/sspr3.hex $@

$(obj)/northbridge/amd/amdfam10/sspr4.c: $(src)/northbridge/amd/amdfam10/sspr4.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/sspr4 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr4/g' $(obj)/northbridge/amd/amdfam10/sspr4.hex
	mv $(obj)/northbridge/amd/amdfam10/sspr4.hex $@

$(obj)/northbridge/amd/amdfam10/sspr5.c: $(src)/northbridge/amd/amdfam10/sspr5.dsl
	iasl -p $(obj)/northbridge/amd/amdfam10/sspr5 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr5/g' $(obj)/northbridge/amd/amdfam10/sspr5.hex
	mv $(obj)/northbridge/amd/amdfam10/sspr5.hex $@
endif

