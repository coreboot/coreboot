/*
 * intelmetool
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 * Copyright (C) 2009 Carl-Daniel Hailfinger
 * Copyright (C) 2015 Damien Zammit
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define ME_NOT_PRESENT 0
#define ME_FOUND_NOTHING 1
#define ME_FOUND_SOMETHING_NOT_SURE 2
#define ME_CAN_DISABLE_IF_PRESENT 3
#define ME_PRESENT_CAN_DISABLE 4
#define ME_PRESENT_CANNOT_DISABLE 5

#define INTELMETOOL_VERSION "1.0"

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: http://www.coreboot.org/DirectHW */
#define __DARWIN__
#include <DirectHW/DirectHW.h>
#endif
#ifdef __NetBSD__
#include <pciutils/pci.h>
#else
#include <pci/pci.h>
#endif

#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CMAG  "\x1B[35m"
#define CCYN  "\x1B[36m"
#define CWHT  "\x1B[37m"
#define RESET "\033[0m"

extern int debug;

// Definitely has ME and can be disabled
#define PCI_DEVICE_ID_INTEL_ICH8ME		0x2811
#define PCI_DEVICE_ID_INTEL_ICH9ME		0x2917
#define PCI_DEVICE_ID_INTEL_ICH9M		0x2919

#define PCI_DEV_HAS_ME_DISABLE(x) ( \
	( (x) == PCI_DEVICE_ID_INTEL_ICH8ME ) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9ME ) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9M  ))

// Definitely has ME and is very difficult to remove
#define PCI_DEVICE_ID_INTEL_ICH10R		0x3a16
#define PCI_DEVICE_ID_INTEL_3400_DESKTOP	0x3b00
#define PCI_DEVICE_ID_INTEL_3400_MOBILE		0x3b01
#define PCI_DEVICE_ID_INTEL_P55			0x3b02
#define PCI_DEVICE_ID_INTEL_PM55		0x3b03
#define PCI_DEVICE_ID_INTEL_H55			0x3b06
#define PCI_DEVICE_ID_INTEL_QM57		0x3b07
#define PCI_DEVICE_ID_INTEL_H57			0x3b08
#define PCI_DEVICE_ID_INTEL_HM55		0x3b09
#define PCI_DEVICE_ID_INTEL_Q57			0x3b0a
#define PCI_DEVICE_ID_INTEL_HM57		0x3b0b
#define PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF	0x3b0d
#define PCI_DEVICE_ID_INTEL_B55_A		0x3b0e
#define PCI_DEVICE_ID_INTEL_QS57		0x3b0f
#define PCI_DEVICE_ID_INTEL_3400		0x3b12
#define PCI_DEVICE_ID_INTEL_3420		0x3b14
#define PCI_DEVICE_ID_INTEL_3450		0x3b16
#define PCI_DEVICE_ID_INTEL_B55_B		0x3b1e
#define PCI_DEVICE_ID_INTEL_Z68			0x1c44
#define PCI_DEVICE_ID_INTEL_P67			0x1c46
#define PCI_DEVICE_ID_INTEL_UM67		0x1c47
#define PCI_DEVICE_ID_INTEL_HM65		0x1c49
#define PCI_DEVICE_ID_INTEL_H67			0x1c4a
#define PCI_DEVICE_ID_INTEL_HM67		0x1c4b
#define PCI_DEVICE_ID_INTEL_Q65			0x1c4c
#define PCI_DEVICE_ID_INTEL_QS67		0x1c4d
#define PCI_DEVICE_ID_INTEL_Q67			0x1c4e
#define PCI_DEVICE_ID_INTEL_QM67		0x1c4f
#define PCI_DEVICE_ID_INTEL_B65			0x1c50
#define PCI_DEVICE_ID_INTEL_C202		0x1c52
#define PCI_DEVICE_ID_INTEL_C204		0x1c54
#define PCI_DEVICE_ID_INTEL_C206		0x1c56
#define PCI_DEVICE_ID_INTEL_H61			0x1c5c
#define PCI_DEVICE_ID_INTEL_Z77			0x1e44
#define PCI_DEVICE_ID_INTEL_Z75			0x1e46
#define PCI_DEVICE_ID_INTEL_Q77			0x1e47
#define PCI_DEVICE_ID_INTEL_Q75			0x1e48
#define PCI_DEVICE_ID_INTEL_B75			0x1e49
#define PCI_DEVICE_ID_INTEL_H77			0x1e4a
#define PCI_DEVICE_ID_INTEL_C216		0x1e53
#define PCI_DEVICE_ID_INTEL_QM77		0x1e55
#define PCI_DEVICE_ID_INTEL_QS77		0x1e56
#define PCI_DEVICE_ID_INTEL_HM77		0x1e57
#define PCI_DEVICE_ID_INTEL_UM77		0x1e58
#define PCI_DEVICE_ID_INTEL_HM76		0x1e59
#define PCI_DEVICE_ID_INTEL_HM75		0x1e5d
#define PCI_DEVICE_ID_INTEL_HM70		0x1e5e
#define PCI_DEVICE_ID_INTEL_NM70		0x1e5f
#define PCI_DEVICE_ID_INTEL_QM87		0x8c4f
#define PCI_DEVICE_ID_INTEL_DH89XXCC		0x2310
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL	0x9c41
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM	0x9c43
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE	0x9c45
#define PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP	0x9cc5
#define PCI_DEVICE_ID_INTEL_X99			0x8d47

#define PCI_DEV_HAS_ME_DIFFICULT(x) ( \
	( (x) == PCI_DEVICE_ID_INTEL_ICH10R ) || \
	( (x) == PCI_DEVICE_ID_INTEL_3400_DESKTOP ) || \
	( (x) == PCI_DEVICE_ID_INTEL_3400_MOBILE ) || \
	( (x) == PCI_DEVICE_ID_INTEL_P55	) || \
	( (x) == PCI_DEVICE_ID_INTEL_PM55	) || \
	( (x) == PCI_DEVICE_ID_INTEL_H55	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QM57	) || \
	( (x) == PCI_DEVICE_ID_INTEL_H57	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM55	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Q57	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM57	) || \
	( (x) == PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF ) || \
	( (x) == PCI_DEVICE_ID_INTEL_B55_A	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QS57	) || \
	( (x) == PCI_DEVICE_ID_INTEL_3400	) || \
	( (x) == PCI_DEVICE_ID_INTEL_3420	) || \
	( (x) == PCI_DEVICE_ID_INTEL_3450	) || \
	( (x) == PCI_DEVICE_ID_INTEL_B55_B	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Z68	) || \
	( (x) == PCI_DEVICE_ID_INTEL_P67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_UM67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM65	) || \
	( (x) == PCI_DEVICE_ID_INTEL_H67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Q65	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QS67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Q67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QM67	) || \
	( (x) == PCI_DEVICE_ID_INTEL_B65	) || \
	( (x) == PCI_DEVICE_ID_INTEL_C202	) || \
	( (x) == PCI_DEVICE_ID_INTEL_C204	) || \
	( (x) == PCI_DEVICE_ID_INTEL_C206	) || \
	( (x) == PCI_DEVICE_ID_INTEL_H61	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Z77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Z75	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Q77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_Q75	) || \
	( (x) == PCI_DEVICE_ID_INTEL_B75	) || \
	( (x) == PCI_DEVICE_ID_INTEL_H77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_C216	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QM77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QS77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_UM77	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM76	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM75	) || \
	( (x) == PCI_DEVICE_ID_INTEL_HM70	) || \
	( (x) == PCI_DEVICE_ID_INTEL_NM70	) || \
	( (x) == PCI_DEVICE_ID_INTEL_QM87	) || \
	( (x) == PCI_DEVICE_ID_INTEL_DH89XXCC	) || \
	( (x) == PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL	) || \
	( (x) == PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM	) || \
	( (x) == PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE	) || \
	( (x) == PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP	) || \
	( (x) == PCI_DEVICE_ID_INTEL_X99	))

// Not sure if ME present, but should be able to disable it easily
#define PCI_DEVICE_ID_INTEL_ICH8		0x2810
#define PCI_DEVICE_ID_INTEL_ICH8M		0x2815
#define PCI_DEVICE_ID_INTEL_ICH9DH		0x2912
#define PCI_DEVICE_ID_INTEL_ICH9DO		0x2914
#define PCI_DEVICE_ID_INTEL_ICH9R		0x2916
#define PCI_DEVICE_ID_INTEL_ICH9		0x2918

#define PCI_DEV_CAN_DISABLE_ME_IF_PRESENT(x) ( \
	( (x) == PCI_DEVICE_ID_INTEL_ICH8	) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH8M	) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9DH	) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9DO	) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9R	) || \
	( (x) == PCI_DEVICE_ID_INTEL_ICH9	))

// Not sure at all
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC	0x8119
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO		0x8100

#define PCI_DEV_ME_NOT_SURE(x) ( \
	( (x) == PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC ) || \
	( (x) == PCI_DEVICE_ID_INTEL_SCH_POULSBO))

#define PCI_DEVICE_ID_INTEL_COUGARPOINT_1	0x1C3A /* Cougar Point */
#define PCI_DEVICE_ID_INTEL_PATSBURG_1	0x1D3A /* C600/X79 Patsburg */
#define PCI_DEVICE_ID_INTEL_PANTHERPOINT_1	0x1CBA /* Panther Point */
#define PCI_DEVICE_ID_INTEL_PANTHERPOINT_2	0x1DBA /* Panther Point */
#define PCI_DEVICE_ID_INTEL_PANTHERPOINT_3	0x1E3A /* Panther Point */
#define PCI_DEVICE_ID_INTEL_CAVECREEK	0x2364 /* Cave Creek */
#define PCI_DEVICE_ID_INTEL_BEARLAKE_1 0x28B4 /* Bearlake */
#define PCI_DEVICE_ID_INTEL_BEARLAKE_2 0x28C4 /* Bearlake */
#define PCI_DEVICE_ID_INTEL_BEARLAKE_3 0x28D4 /* Bearlake */
#define PCI_DEVICE_ID_INTEL_BEARLAKE_4 0x28E4 /* Bearlake */
#define PCI_DEVICE_ID_INTEL_BEARLAKE_5 0x28F4 /* Bearlake */
#define PCI_DEVICE_ID_INTEL_82946GZ 0x2974 /* 82946GZ/GL */
#define PCI_DEVICE_ID_INTEL_82G35	0x2984 /* 82G35 Express */
#define PCI_DEVICE_ID_INTEL_82Q963	0x2994 /* 82Q963/Q965 */
#define PCI_DEVICE_ID_INTEL_82P965 0x29A4 /* 82P965/G965 */
#define PCI_DEVICE_ID_INTEL_82Q35 0x29B4 /* 82Q35 Express */
#define PCI_DEVICE_ID_INTEL_82G33 0x29C4 /* 82G33/G31/P35/P31 Express */
#define PCI_DEVICE_ID_INTEL_82Q33 0x29D4 /* 82Q33 Express */
#define PCI_DEVICE_ID_INTEL_82X38 0x29E4 /* 82X38/X48 Express */
#define PCI_DEVICE_ID_INTEL_3200 0x29F4 /* 3200/3210 Server */
#define PCI_DEVICE_ID_INTEL_PM965	0x2A04 /* Mobile PM965/GM965 */
#define PCI_DEVICE_ID_INTEL_GME965	0x2A14 /* Mobile GME965/GLE960 */
#define PCI_DEVICE_ID_INTEL_CANTIGA_1	0x2A44 /* Cantiga */
#define PCI_DEVICE_ID_INTEL_CANTIGA_2	0x2a50 /* Cantiga */
#define PCI_DEVICE_ID_INTEL_CANTIGA_3	0x2A54 /* Cantiga */
#define PCI_DEVICE_ID_INTEL_CANTIGA_4 0x2A64 /* Cantiga */
#define PCI_DEVICE_ID_INTEL_CANTIGA_5	0x2A74 /* Cantiga */
#define PCI_DEVICE_ID_INTEL_EAGLELAKE_1 0x2E04 /* Eaglelake */
#define PCI_DEVICE_ID_INTEL_EAGLELAKE_2 0x2E14 /* Eaglelake */
#define PCI_DEVICE_ID_INTEL_EAGLELAKE_3	0x2E24 /* Eaglelake */
#define PCI_DEVICE_ID_INTEL_EAGLELAKE_4 0x2E34 /* Eaglelake */
#define PCI_DEVICE_ID_INTEL_CALPELLA_1 0x3B64 /* Calpella */
#define PCI_DEVICE_ID_INTEL_CALPELLA_2 0x3B65 /* Calpella */
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_1	0x8C3A /* Lynx Point H */
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_2 0x8CBA /* Lynx Point H Refresh */
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_3 0x8D3A /* Lynx Point - Wellsburg */
#define PCI_DEVICE_ID_INTEL_LYNXPOINT_4 0x9C3A /* Lynx Point LP */
#define PCI_DEVICE_ID_INTEL_WILDCAT_1 0x9CBA /* Wildcat Point LP */
#define PCI_DEVICE_ID_INTEL_WILDCAT_2 0x9CBB /* Wildcat Point LP 2 */

#define PCI_DEV_HAS_SUPPORTED_ME(x) ( \
	( (x) ==  PCI_DEVICE_ID_INTEL_COUGARPOINT_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_PATSBURG_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_PANTHERPOINT_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_PANTHERPOINT_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_PANTHERPOINT_3 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CAVECREEK ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_BEARLAKE_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_BEARLAKE_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_BEARLAKE_3 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_BEARLAKE_4 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_BEARLAKE_5 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82946GZ ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82G35 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82Q963 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82P965 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82Q35 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82G33 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82Q33 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_82X38 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_3200 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_PM965 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_GME965 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CANTIGA_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CANTIGA_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CANTIGA_3 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CANTIGA_4 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CANTIGA_5 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_EAGLELAKE_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_EAGLELAKE_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_EAGLELAKE_3 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_EAGLELAKE_4 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CALPELLA_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_CALPELLA_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_LYNXPOINT_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_LYNXPOINT_2 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_LYNXPOINT_3 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_LYNXPOINT_4 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_WILDCAT_1 ) || \
	( (x) ==  PCI_DEVICE_ID_INTEL_WILDCAT_2))
