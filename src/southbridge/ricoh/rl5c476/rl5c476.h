/* SPDX-License-Identifier: GPL-2.0-or-later */

 /* rl5c476 routines and defines*/

#include <stdint.h>

/* the 16 bit control structure for ricoh cardbus bridge */
typedef struct pc16reg  {
	u8 idrevs;
	u8 ifstat;
	u8 pwctrl;
	u8 igctrl;
	u8 cschg;
	u8 cscint;
	u8 awinen;
	u8 ioctrl;
	u8 iostl0;
	u8 iosth0;
	u8 iospl0;
	u8 iosph0;
	u8 iostl1;
	u8 iosth1;
	u8 iospl1;
	u8 iosph1;
	u8 smstl0;
	u8 smsth0;
	u8 smspl0;
	u8 smsph0;
	u8 moffl0;
	u8 moffh0;
	u8 cdgenc;
	u8 resv1;
	u8 smstl1;
	u8 smsth1;
	u8 smspl1;
	u8 smsph1;
	u8 moffl1;
	u8 moffh1;
	u8 glctrl;
	u8 atctrl;
	u8 smstl2;
	u8 smsth2;
	u8 smspl2;
	u8 smsph2;
	u8 moffl2;
	u8 moffh2;
	u8 resv2;
	u8 resv3;
	u8 smstl3;
	u8 smsth3;
	u8 smspl3;
	u8 smsph3;
	u8 moffl3;
	u8 moffh3;
	u8 resv4;
	u8 miscc1;
	u8 smstl4;
	u8 smsth4;
	u8 smspl4;
	u8 smsph4;
	u8 moffl4;
	u8 moffh4;
	u8 ioffl0;
	u8 ioffh0;
	u8 ioffl1;
	u8 ioffh1;
	u8 gpio;
	u8 resv5;
	u8 resv6;
	u8 resv7;
	u8 resv8;
	u8 resv9;
	u8 smpga0;
} __packed pc16reg_t;
