#include <southbridge/intel/82801.h>
#include <pc80/mc146818rtc.h>
#include <pci.h>
#include <pci_ids.h>
#include <printk.h>
#include "82801.h"


void ich2_rtc_init(void)
{
	unsigned char byte;
	u32 dword;
	int rtc_failed;
	pcibios_read_config_byte(RTC_BUS, RTC_DEVFN, GEN_PMCON_3, &byte);
	rtc_failed = byte & RTC_FAILED;
	if (rtc_failed) {
		byte &= ~(1 << 1); /* preserve the power fail state */
		pcibios_write_config_byte(RTC_BUS, RTC_DEVFN, GEN_PMCON_3, byte);
	}
	pcibios_read_config_dword(RTC_BUS, RTC_DEVFN, GEN_STS, &dword);
	rtc_failed |= dword & (1 << 2);
	rtc_init(rtc_failed);
}
