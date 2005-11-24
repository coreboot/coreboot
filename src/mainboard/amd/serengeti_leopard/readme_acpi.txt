/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
// 2005.9 serengeti support
// by yhlu
//
//=



At this time, For acpi support We got
1. support AMK K8 SRAT --- dynamically (LinuxBIOS run-time)  (src/northbridge/amd/amdk8/amdk8_acpi.c)
2. support MADT ---- dynamically (LinuxBIOS run-time)  (src/northbridge/amd/amdk8/amdk8_acpi.c , src/mainboard/amd/serengeti_leopard/acpi_tables.c)
3. support DSDT ---- dynamically (Compile time, LinuxBIOS run-time, ACPI run-time) (src/mainboard/amd/serengeti_leopard/{dx/*, get_bus_conf.c}, src/northbridge/amd/amdk8/get_sblk_pci1234.c)
4. Chipset support: amd8111, amd8132

The developers need to change for different MB

Change dx/dsdt_lb.dsl, according to MB layout 
	pci1, pci2, pci3, pci4, ...., pci8
	if there is HT-IO board, may use pci2.asl.... to create ssdt2.c, and ssdt3,c and ssdt4.c, ....ssdt8.c

Change acpi_tables.c
	sbdn: Real SB device Num. for 8111 =3 or 1 depend if 8131 presents.  ---- Actually you don't need to change it, it is LinuxBIOS run-time configurable now.
	if there is HT-IO board, need to adjust SSDTX_NUM...., ans preset pci1234 array. acpi_tables.c will decide to put the SSDT on the RSDT or not according if the HT-IO board is installed

Regarding pci bridge apic and pic
	need to modify entries amd8111.asl and amd8131.asl and amd8151.asl.... acording to your MB laybout, it is like that in mptable.c

About other chipsets, need to develop their special asl such as 
	ck804.asl  --- NB ck804
	bcm5785.asl or bcm5780.asl ---- Serverworks HT1000/HT2000

yhlu

09/18/2005
	
