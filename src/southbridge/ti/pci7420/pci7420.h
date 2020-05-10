/* SPDX-License-Identifier: GPL-2.0-or-later */

		     // 0844d060 (old)
#define SYSCTL	0x80 // 08405061
#define   RIMUX		(1 << 0)

#define GENCTL	0x86
#define   FW1394_PRIO	(0 << 0)
#define   CARDBUS_PRIO	(1 << 0)
#define   FLASH_PRIO	(2 << 0)
#define   ROUNDR_PRIO	(3 << 0)
#define   DISABLE_OHCI  (1 << 3)
#define   DISABLE_SKTB	(1 << 4)
#define   DISABLE_FM	(1 << 5)
#define   P12V_SW_SEL	(1 << 10)

#define MFUNC	0x8c

#define CARDCTL	0x91
#define   SPKROUTEN	(1 << 1)

#define DEVCTL	0x92
#define   INT_MODE_PAR	(0 << 1)
#define   INT_MODE_RSV  (1 << 1)
#define   INT_MODE_MIX  (2 << 1)
#define   INT_MODE_SER  (3 << 1)

#define INTA 0
#define INTB 1
#define INTC 2
#define INTD 3
