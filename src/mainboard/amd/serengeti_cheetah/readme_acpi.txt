At this time, For acpi support We got
1. support AMK K8 SRAT --- dynamically (coreboot run-time)  (src/northbridge/amd/amdk8/amdk8_acpi.c)
2. support MADT ---- dynamically (coreboot run-time)  (src/northbridge/amd/amdk8/amdk8_acpi.c , src/mainboard/amd/serengeti_cheetah/acpi_tables.c)
3. support DSDT ---- dynamically (Compile time, coreboot run-time, ACPI run-time) (src/mainboard/amd/serengeti_cheetah/{acpi/*, get_bus_conf.c}, src/northbridge/amd/amdk8/get_sblk_pci1234.c)
4. Chipset support: amd8111, amd8132

The developers need to change for different MB

Change dsdt.asl, according to MB layout
	pci1, pci2, pci3, pci4, ...., pci8
	if there is HT-IO board, may use pci2.asl.... to create ssdt2.c, and ssdt3,c and ssdt4.c, ....ssdt8.c

Change acpi_tables.c
	sbdn: Real SB device Num. for 8111 =3 or 1 depend if 8131 presents.  ---- Actually you don't need to change it, it is coreboot run-time configurable now.
	if there is HT-IO board, need to adjust SSDTX_NUM...., and preset pci1234 array. acpi_tables.c will decide to put the SSDT on the RSDT or not according if the HT-IO board is installed

Regarding pci bridge apic and pic
	need to modify entries amd8111.asl and amd8131.asl and amd8151.asl.... acording to your MB laybout, it is like that in mptable.c

About other chipsets, need to develop their special asl such as
	ck804.asl  --- NB ck804
	bcm5785.asl or bcm5780.asl ---- Serverworks HT1000/HT2000

use a to create hex file
use c to delele hex file

yhlu

09/18/2005

