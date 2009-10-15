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
	iasl -p $(CURDIR)/ssdt -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt/g' ssdt.hex
	mv ssdt.hex $@

$(obj)/northbridge/amd/amdfam10/sspr1.c: $(src)/northbridge/amd/amdfam10/sspr1.dsl
	iasl -p $(CURDIR)/sspr1 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr1/g' sspr1.hex
	mv sspr1.hex $@

$(obj)/northbridge/amd/amdfam10/sspr2.c: $(src)/northbridge/amd/amdfam10/sspr2.dsl
	iasl -p $(CURDIR)/sspr2 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr2/g' sspr2.hex
	mv sspr2.hex $@

$(obj)/northbridge/amd/amdfam10/sspr3.c: $(src)/northbridge/amd/amdfam10/sspr3.dsl
	iasl -p $(CURDIR)/sspr3 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr3/g' sspr3.hex
	mv sspr3.hex $@

$(obj)/northbridge/amd/amdfam10/sspr4.c: $(src)/northbridge/amd/amdfam10/sspr4.dsl
	iasl -p $(CURDIR)/sspr4 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr4/g' sspr4.hex
	mv sspr4.hex $@

$(obj)/northbridge/amd/amdfam10/sspr5.c: $(src)/northbridge/amd/amdfam10/sspr5.dsl
	iasl -p $(CURDIR)/sspr5 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_sspr5/g' sspr5.hex
	mv sspr5.hex $@
endif

